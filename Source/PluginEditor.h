#pragma once

#include "PluginProcessor.h"

//==============================================================================
// Paleta neon
//==============================================================================
namespace NeonColours
{
    // Paleta sobria: grafito + azul acero, con acentos apagados por funcion
    inline const juce::Colour bgTop      { (juce::uint32) 0xff171a20 };
    inline const juce::Colour bgBottom   { (juce::uint32) 0xff0f1116 };
    inline const juce::Colour panel      { (juce::uint32) 0xff1d2128 };
    inline const juce::Colour panelDeep  { (juce::uint32) 0xff13161b };

    inline const juce::Colour magenta    { (juce::uint32) 0xff5b8fc9 };   // azul acero  (osc A / delay / amp)
    inline const juce::Colour cyan       { (juce::uint32) 0xff4fb3bf };   // turquesa    (osc B / filtro / chorus)
    inline const juce::Colour violet     { (juce::uint32) 0xff8a94b8 };   // gris azulado (unison / mezcla / reverb)
    inline const juce::Colour lime       { (juce::uint32) 0xff7fae8b };   // verde salvia (LFO / salida)
    inline const juce::Colour orange     { (juce::uint32) 0xffd0a054 };   // ambar        (pitch / drive)

    inline const juce::Colour textDim    { (juce::uint32) 0xff8b93a3 };
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
    NeonPanel (const juce::String& title, juce::Colour accent, const juce::String& subtitle = {});

    // weight = ancho relativo dentro del panel
    void addControl (juce::Component& c, float weight = 1.0f);
    void addPowerSwitch (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);

    float getTotalWeight() const noexcept { return totalWeight; }
    void  refreshPowerLook();

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Item { juce::Component* comp; float weight; };

    juce::String title, subtitle;
    juce::Colour accent;
    std::vector<Item> items;
    float totalWeight = 0.0f;

    juce::ToggleButton powerButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
    bool hasPower = false;
    bool lastPowerState = true;
};

//==============================================================================
// Visor del preset actual: muestra solo el seleccionado; al hacer click
// el editor abre el menu completo.
//==============================================================================
class PresetDisplay : public juce::Component
{
public:
    std::function<void()> onClick;

    void setText (const juce::String& pathText, const juce::String& nameText);

    void paint (juce::Graphics&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseEnter (const juce::MouseEvent&) override { hover = true;  repaint(); }
    void mouseExit  (const juce::MouseEvent&) override { hover = false; repaint(); }

private:
    juce::String path, name;
    bool hover = false;
};

//==============================================================================
// Editor de ondas: dibuja con el mouse la onda Custom de cada oscilador
//==============================================================================
class WaveEditor : public juce::Component, private juce::Timer
{
public:
    explicit WaveEditor (PerreoTrapSynthAudioProcessor& p);
    ~WaveEditor() override { stopTimer(); }

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override { lastIdx = -1; }

private:
    void timerCallback() override { if (isShowing()) repaint(); }
    void selectOsc (int newOsc);
    int  currentWave() const;
    juce::String waveParamId() const { return osc == 0 ? "aWave" : "bWave"; }
    void makeCustom();
    void applyTool (std::function<void (std::array<float, Waves::tableSize>&)> fn);
    void drawAt (juce::Point<int> pos);

    PerreoTrapSynthAudioProcessor& proc;
    int osc = 0;
    int lastIdx = -1;
    float lastVal = 0.0f;
    juce::Rectangle<int> canvas;

    juce::TextButton oscAButton { "OSC A" }, oscBButton { "OSC B" };
    juce::ComboBox waveBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveAttach;
    juce::TextButton sineBtn { "SINE" }, sawBtn { "SAW" }, squareBtn { "SQUARE" }, triBtn { "TRIANGLE" },
                     smoothBtn { "SMOOTH" }, invertBtn { "INVERT" }, randomBtn { "RANDOM" }, mirrorBtn { "MIRROR" };
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

    void syncBrowser();
    void setWaveEditorOpen (bool open);
    void showPresetMenu();
    void stepPreset (int direction);
    void saveToUser();
    void importPack();
    void exportPackMenu();
    void doExport (const juce::String& packName, std::vector<LibraryPreset> presets);

    KnobComp&  addKnob  (NeonPanel& panel, const juce::String& id, const juce::String& name, juce::Colour accent);
    ComboComp& addCombo (NeonPanel& panel, const juce::String& id, const juce::String& name, juce::Colour accent);
    NeonPanel& addPanel (const juce::String& title, juce::Colour accent, int row, const juce::String& subtitle = {});

    PerreoTrapSynthAudioProcessor& processorRef;
    NeonLookAndFeel neonLookAndFeel;

    // ---- Navegador de presets ----
    PresetDisplay presetDisplay;
    juce::TextButton prevButton { "<" }, nextButton { ">" };
    juce::TextButton saveButton { "SAVE" }, importButton { "IMPORT PACK" }, exportButton { "EXPORT PACK" };
    juce::TextButton wavesButton { "WAVES" }, randomButton { "RANDOM" };
    WaveEditor waveEditor { processorRef };
    static constexpr int waveBandHeight = 210;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::String shownPath, shownName;

    // ---- Paneles y controles ----
    static constexpr int numRows = 4;
    std::vector<std::unique_ptr<NeonPanel>> panels;
    std::array<std::vector<NeonPanel*>, numRows> rows;
    std::vector<std::unique_ptr<KnobComp>>  knobs;
    std::vector<std::unique_ptr<ComboComp>> combos;

    juce::Rectangle<int> browserBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PerreoTrapSynthAudioProcessorEditor)
};
