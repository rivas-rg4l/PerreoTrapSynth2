#pragma once

#include "PluginProcessor.h"

//==============================================================================
// Paleta neon urbano
//==============================================================================
namespace NeonColours
{
    inline const juce::Colour charcoal     { (juce::uint32) 0xff16161c };
    inline const juce::Colour charcoalDeep { (juce::uint32) 0xff0b0b0f };
    inline const juce::Colour panel        { (juce::uint32) 0xff1d1d26 };
    inline const juce::Colour magenta      { (juce::uint32) 0xffff2bd6 };
    inline const juce::Colour cyan         { (juce::uint32) 0xff19f0ff };
    inline const juce::Colour textDim      { (juce::uint32) 0xffb9b9c8 };
}

//==============================================================================
// LookAndFeel neon
//==============================================================================
class NeonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    NeonLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override;

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override;
};

//==============================================================================
// Editor
//==============================================================================
class PerreoTrapSynthAudioProcessorEditor : public juce::AudioProcessorEditor,
                                            private juce::Timer
{
public:
    explicit PerreoTrapSynthAudioProcessorEditor (PerreoTrapSynthAudioProcessor&);
    ~PerreoTrapSynthAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment   = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void timerCallback() override;
    void setupKnob (juce::Slider& slider, juce::Label& label, const juce::String& text, juce::Colour accent);
    void updateActivePresetLabel (int presetIndex);

    PerreoTrapSynthAudioProcessor& processorRef;

    NeonLookAndFeel neonLookAndFeel;

    juce::Label presetTitleLabel;
    juce::ComboBox presetBox;
    juce::Label activePresetLabel;

    juce::Label waveTitleLabel;
    juce::ComboBox waveBox;

    juce::Slider cutoffSlider, resonanceSlider, driveSlider, reverbSlider, masterSlider;
    juce::Label  cutoffLabel,  resonanceLabel,  driveLabel,  reverbLabel,  masterLabel;

    juce::Rectangle<int> knobPanelBounds;

    // Los attachments se declaran al final para destruirse primero
    std::unique_ptr<ComboBoxAttachment> waveAttachment;
    std::unique_ptr<SliderAttachment> cutoffAttachment;
    std::unique_ptr<SliderAttachment> resonanceAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> reverbAttachment;
    std::unique_ptr<SliderAttachment> masterAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PerreoTrapSynthAudioProcessorEditor)
};
