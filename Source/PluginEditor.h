#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class MinstrelFolkAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit MinstrelFolkAudioProcessorEditor (MinstrelFolkAudioProcessor&);
    ~MinstrelFolkAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MinstrelFolkAudioProcessor& processor;
    juce::Slider mix, warmth, room;
    juce::Label mixLabel, warmthLabel, roomLabel, title, subtitle;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> mixAttachment, warmthAttachment, roomAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MinstrelFolkAudioProcessorEditor)
};
