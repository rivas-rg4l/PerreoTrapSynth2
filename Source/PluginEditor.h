#pragma once

#include "PluginProcessor.h"

//==============================================================================
// Paleta neon
//==============================================================================
namespace NeonColours
{
    inline const juce::Colour bgTop      { (juce::uint32) 0xff0a0714 };
    inline const juce::Colour bgBottom   { (juce::uint32) 0xff05040b };
    inline const juce::Colour panel      { (juce::uint32) 0xff120d20 };
    inline const juce::Colour panelDeep  { (juce::uint32) 0xff0a0714 };

    inline const juce::Colour magenta    { (juce::uint32) 0xffff2bd6 };
    inline const juce::Colour cyan       { (juce::uint32) 0xff19f0ff };
    inline const juce::Colour violet     { (juce::uint32) 0xff9d5cff };
    inline const juce::Colour lime       { (juce::uint32) 0xffb6ff3b };
    inline const juce::Colour orange     { (juce::uint32) 0xffff8a1f };

    inline const juce::Colour textDim    { (juce::uint32) 0xff9a93b8 };
}

//==============================================================================
// LookAndFeel neon
//==============================================================================
class NeonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    NeonLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonText (juce::Graphics&, juce::TextButton&,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Font getComboBoxFont (juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;
};

//==============================================================================
// Perilla con nombre + valor
//==============================================================================
class KnobComp : public juce::Component
{
public:
    KnobComp (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID,
              const juce::String& name, juce::Colour accent);

    void resized() override;

private:
    juce::Label  nameLabel;
    juce::Slider slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};

//==============================================================================
// Selector desplegable con nombre
//==============================================================================
class ComboComp : public juce::Component
{
public:
    ComboComp (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID,
               const juce::String& name, juce::Colour accent);

    void resized() override;

private:
    juce::Label    nameLabel;
    juce::ComboBox box;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
};

//==============================================================================
// Panel con titulo, color de acento y (opcional) interruptor ON/OFF
//==============================================================================
class NeonPanel : public juce::Component
{
public:
    NeonPanel (const juce::String& title, juce::Colour accent);

    // weight = ancho relativo dentro del panel
    void addControl (juce::Component& c, float weight = 1.0f);
    void addPowerSwitch (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);

    float getTotalWeight() const noexcept { return totalWeight; }
    void  refreshPowerLook();

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Item { juce::Component* comp; float weight; };

    juce::String title;
    juce::Colour accent;
    std::vector<Item> items;
    float totalWeight = 0.0f;

    juce::ToggleButton powerButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
    bool hasPower = false;
    bool lastPowerState = true;
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
    void timerCallback() override;

    void loadPreset (int genre, int category, int slot);
    void syncBrowser();
    void importPreset();
    void exportPreset();

    KnobComp&  addKnob  (NeonPanel& panel, const juce::String& id, const juce::String& name, juce::Colour accent);
    ComboComp& addCombo (NeonPanel& panel, const juce::String& id, const juce::String& name, juce::Colour accent);
    NeonPanel& addPanel (const juce::String& title, juce::Colour accent, int row);

    PerreoTrapSynthAudioProcessor& processorRef;
    NeonLookAndFeel neonLookAndFeel;

    // ---- Navegador de presets ----
    std::array<juce::TextButton, PresetLayout::numGenres>     genreButtons;
    std::array<juce::TextButton, PresetLayout::numCategories> categoryButtons;
    std::array<juce::TextButton, PresetLayout::perCategory>   slotButtons;
    juce::TextButton prevButton { "<" }, nextButton { ">" };
    juce::TextButton importButton { "IMPORT" }, exportButton { "EXPORT" };
    juce::Label presetNameLabel;
    std::unique_ptr<juce::FileChooser> fileChooser;

    int shownGenre = -1, shownCategory = -1;
    juce::String shownName;

    // ---- Paneles y controles ----
    static constexpr int numRows = 4;
    std::vector<std::unique_ptr<NeonPanel>> panels;
    std::array<std::vector<NeonPanel*>, numRows> rows;
    std::vector<std::unique_ptr<KnobComp>>  knobs;
    std::vector<std::unique_ptr<ComboComp>> combos;

    juce::Rectangle<int> browserBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PerreoTrapSynthAudioProcessorEditor)
};
