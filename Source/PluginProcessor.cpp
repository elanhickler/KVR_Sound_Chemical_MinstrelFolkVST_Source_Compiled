#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
    constexpr double pi = 3.14159265358979323846;

    class FolkSound final : public juce::SynthesiserSound
    {
    public:
        bool appliesToNote (int) override { return true; }
        bool appliesToChannel (int) override { return true; }
    };

    class FolkVoice final : public juce::SynthesiserVoice
    {
    public:
        bool canPlaySound (juce::SynthesiserSound* s) override
        {
            return dynamic_cast<FolkSound*> (s) != nullptr;
        }

        void startNote (int midiNote, float velocity, juce::SynthesiserSound*, int) override
        {
            noteHz = juce::MidiMessage::getMidiNoteInHertz (midiNote);
            level = velocity * 0.16f;
            age = 0.0;
            phase = 0.0;
            pluckEnv = 1.0f;
            releasing = false;
            active = true;
        }

        void stopNote (float, bool allowTailOff) override
        {
            if (allowTailOff) releasing = true;
            else { clearCurrentNote(); active = false; }
        }

        void pitchWheelMoved (int) override {}
        void controllerMoved (int, int) override {}

        void renderNextBlock (juce::AudioBuffer<float>& output, int start, int count) override
        {
            if (!active || getSampleRate() <= 0.0) return;
            const double sr = getSampleRate();
            const double baseInc = 2.0 * pi * noteHz / sr;

            for (int i = 0; i < count; ++i)
            {
                const double t = age;
                // A plucked, slightly nasal string approximation: fundamental plus inharmonic upper partials.
                const float decay = static_cast<float>(std::exp(-t * 2.7));
                const float pluck = static_cast<float>(
                    std::sin(phase) +
                    0.43 * std::sin(phase * 2.01 + 0.15) +
                    0.19 * std::sin(phase * 3.98 + 0.33) +
                    0.08 * std::sin(phase * 6.02));
                // A soft "group vowel" layer built from gentle detuned harmonics, not a sampled voice.
                const float slow = static_cast<float>(std::sin(2.0 * pi * (noteHz * 0.997) * t));
                const float choir = 0.42f * slow
                    + 0.24f * static_cast<float>(std::sin(2.0 * pi * noteHz * 2.01 * t))
                    + 0.10f * static_cast<float>(std::sin(2.0 * pi * noteHz * 3.0 * t));
                const float choirRise = 1.0f - std::exp(static_cast<float>(-t * 4.0));
                float sample = level * (0.76f * pluck * decay + 0.28f * choir * choirRise);

                if (releasing)
                {
                    pluckEnv *= 0.99925f;
                    sample *= pluckEnv;
                    if (pluckEnv < 0.0005f) { clearCurrentNote(); active = false; break; }
                }

                const float pan = 0.5f + 0.22f * std::sin(static_cast<float>(noteHz * 0.013));
                const float left = sample * std::sqrt(1.0f - pan);
                const float right = sample * std::sqrt(pan);
                if (output.getNumChannels() > 0) output.addSample(0, start + i, left);
                if (output.getNumChannels() > 1) output.addSample(1, start + i, right);

                phase += baseInc;
                if (phase > 2.0 * pi) phase -= 2.0 * pi;
                age += 1.0 / sr;
            }
        }

    private:
        double noteHz = 220.0, phase = 0.0, age = 0.0;
        float level = 0.0f, pluckEnv = 1.0f;
        bool active = false, releasing = false;
    };
}

juce::AudioProcessorValueTreeState::ParameterLayout MinstrelFolkAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "String / Choir Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.35f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("warmth", "Warmth",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.55f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("space", "Room",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.22f));
    return { p.begin(), p.end() };
}

MinstrelFolkAudioProcessor::MinstrelFolkAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    for (int i = 0; i < 24; ++i) synth.addVoice (new FolkVoice());
    synth.addSound (new FolkSound());
}

void MinstrelFolkAudioProcessor::prepareToPlay (double sampleRate, int)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void MinstrelFolkAudioProcessor::releaseResources() {}

bool MinstrelFolkAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void MinstrelFolkAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());

    const float mix = parameters.getRawParameterValue("mix")->load();
    const float warmth = parameters.getRawParameterValue("warmth")->load();
    const float room = parameters.getRawParameterValue("space")->load();

    // Slight tape-like softening and a small stereo room tail.
    const float gain = 0.82f + warmth * 0.18f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* d = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float dry = d[i];
            const float softened = std::tanh(dry * (1.0f + warmth * 1.8f));
            d[i] = gain * ((1.0f - mix * 0.18f) * softened + mix * 0.18f * dry);
        }
    }

    // Simple feedback delay for a modest vintage room impression.
    static juce::AudioBuffer<float> delay;
    static int writePos = 0;
    const int delaySize = juce::jmax(1, (int) (getSampleRate() * 0.17));
    if (delay.getNumChannels() != buffer.getNumChannels() || delay.getNumSamples() != delaySize)
    {
        delay.setSize(buffer.getNumChannels(), delaySize);
        delay.clear();
        writePos = 0;
    }
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        const int readPos = (writePos + 1) % delaySize;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            const float in = buffer.getSample(ch, i);
            const float echo = delay.getSample(ch, readPos);
            buffer.addSample(ch, i, echo * room * 0.22f);
            delay.setSample(ch, writePos, in + echo * room * 0.22f);
        }
        writePos = (writePos + 1) % delaySize;
    }
}

juce::AudioProcessorEditor* MinstrelFolkAudioProcessor::createEditor()
{
    return new MinstrelFolkAudioProcessorEditor (*this);
}

void MinstrelFolkAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, dest);
}

void MinstrelFolkAudioProcessor::setStateInformation (const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, size));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MinstrelFolkAudioProcessor();
}
