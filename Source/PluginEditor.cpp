#include "PluginEditor.h"

MinstrelFolkAudioProcessorEditor::MinstrelFolkAudioProcessorEditor (MinstrelFolkAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (520, 300);
    auto setupSlider = [] (juce::Slider& s)
    {
        s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 22);
        s.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffd79b43));
        s.setColour (juce::Slider::thumbColourId, juce::Colour (0xffffe6ad));
        s.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xfffff1d6));
        s.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff3b3027));
    };
    setupSlider (mix); setupSlider (warmth); setupSlider (room);
    addAndMakeVisible (mix); addAndMakeVisible (warmth); addAndMakeVisible (room);

    auto setupLabel = [this] (juce::Label& l, const juce::String& t)
    {
        l.setText (t, juce::dontSendNotification);
        l.setJustificationType (juce::Justification::centred);
        l.setColour (juce::Label::textColourId, juce::Colour (0xffffe6ad));
        addAndMakeVisible (l);
    };
    setupLabel (title, "MINSTREL FOLK");
    setupLabel (subtitle, "1960s folk-pop • strings + close-harmony glow");
    setupLabel (mixLabel, "String / Choir");
    setupLabel (warmthLabel, "Warmth");
    setupLabel (roomLabel, "Room");

    mixAttachment = std::make_unique<Attachment> (processor.parameters, "mix", mix);
    warmthAttachment = std::make_unique<Attachment> (processor.parameters, "warmth", warmth);
    roomAttachment = std::make_unique<Attachment> (processor.parameters, "space", room);
}

void MinstrelFolkAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff201b18));
    g.setColour (juce::Colour (0xff49372a));
    g.fillRoundedRectangle (getLocalBounds().toFloat().reduced (10.0f), 12.0f);
    g.setColour (juce::Colour (0xffd79b43));
    g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (10.0f), 12.0f, 2.0f);
    g.setColour (juce::Colour (0xffffe6ad));
    g.setFont (juce::Font (juce::FontOptions (24.0f, juce::Font::bold)));
    g.drawFittedText ("MINSTREL FOLK", 20, 18, getWidth() - 40, 34, juce::Justification::centred, 1);
    g.setFont (juce::Font (juce::FontOptions (12.0f)));
    g.drawFittedText ("A sunny vintage folk ensemble instrument", 20, 50, getWidth() - 40, 22, juce::Justification::centred, 1);
}

void MinstrelFolkAudioProcessorEditor::resized()
{
    const int w = getWidth() / 3;
    mix.setBounds (w * 0, 94, w, 130);
    warmth.setBounds (w * 1, 94, w, 130);
    room.setBounds (w * 2, 94, w, 130);
    mixLabel.setBounds (w * 0, 222, w, 24);
    warmthLabel.setBounds (w * 1, 222, w, 24);
    roomLabel.setBounds (w * 2, 222, w, 24);
    title.setBounds (0, 0, 0, 0);
    subtitle.setBounds (0, 0, 0, 0);
}
