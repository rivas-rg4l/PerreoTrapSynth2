#include "PluginEditor.h"

//==============================================================================
// NeonLookAndFeel
//==============================================================================
NeonLookAndFeel::NeonLookAndFeel()
{
    using namespace NeonColours;

    setColour (juce::ComboBox::backgroundColourId, panel);
    setColour (juce::ComboBox::textColourId,       juce::Colours::white);
    setColour (juce::ComboBox::outlineColourId,    cyan);
    setColour (juce::ComboBox::arrowColourId,      cyan);

    setColour (juce::PopupMenu::backgroundColourId,            panel);
    setColour (juce::PopupMenu::textColourId,                  juce::Colours::white);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, magenta.withAlpha (0.55f));
    setColour (juce::PopupMenu::highlightedTextColourId,       juce::Colours::white);

    setColour (juce::Slider::textBoxTextColourId,       juce::Colours::white);
    setColour (juce::Slider::textBoxBackgroundColourId, charcoalDeep);
    setColour (juce::Slider::textBoxOutlineColourId,    cyan.withAlpha (0.35f));
    setColour (juce::Slider::textBoxHighlightColourId,  magenta.withAlpha (0.5f));

    setColour (juce::Label::textColourId, juce::Colours::white);

    setColour (juce::TextButton::buttonColourId,  panel);
    setColour (juce::TextButton::buttonOnColourId, magenta.withAlpha (0.6f));
    setColour (juce::TextButton::textColourOffId, cyan);
    setColour (juce::TextButton::textColourOnId,  juce::Colours::white);
}

void NeonLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPosProportional, float rotaryStartAngle,
                                        float rotaryEndAngle, juce::Slider& slider)
{
    using namespace NeonColours;

    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);

    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10.0f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();

    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const float trackWidth = 6.0f;
    const float arcRadius = radius - trackWidth;

    // Pista de fondo
    juce::Path backgroundArc;
    backgroundArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                 rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (charcoalDeep);
    g.strokePath (backgroundArc, juce::PathStrokeType (trackWidth, juce::PathStrokeType::curved,
                                                       juce::PathStrokeType::rounded));

    // Arco de valor con brillo
    if (sliderPosProportional > 0.001f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                rotaryStartAngle, angle, true);

        for (int i = 3; i >= 1; --i)
        {
            g.setColour (accent.withAlpha (0.07f * (float) (4 - i)));
            g.strokePath (valueArc, juce::PathStrokeType (trackWidth + (float) i * 4.0f,
                                                          juce::PathStrokeType::curved,
                                                          juce::PathStrokeType::rounded));
        }

        g.setColour (accent);
        g.strokePath (valueArc, juce::PathStrokeType (trackWidth, juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));
    }

    // Cuerpo del knob
    const float innerRadius = juce::jmax (4.0f, arcRadius - 10.0f);
    juce::ColourGradient bodyGradient (panel.brighter (0.25f), centre.x, centre.y - innerRadius,
                                       charcoalDeep, centre.x, centre.y + innerRadius, false);
    g.setGradientFill (bodyGradient);
    g.fillEllipse (centre.x - innerRadius, centre.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

    g.setColour (accent.withAlpha (0.55f));
    g.drawEllipse (centre.x - innerRadius, centre.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.5f);

    // Indicador
    juce::Path pointer;
    pointer.addRoundedRectangle (-2.0f, -innerRadius + 4.0f, 4.0f, innerRadius * 0.45f, 2.0f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));

    g.setColour (accent.brighter (0.4f));
    g.fillPath (pointer);
}

void NeonLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                    int, int, int, int, juce::ComboBox& box)
{
    using namespace NeonColours;

    const auto bounds = juce::Rectangle<float> (0.0f, 0.0f, (float) width, (float) height).reduced (1.0f);

    g.setColour (panel);
    g.fillRoundedRectangle (bounds, 8.0f);

    g.setColour (box.hasKeyboardFocus (true) ? magenta : cyan.withAlpha (0.85f));
    g.drawRoundedRectangle (bounds, 8.0f, 1.5f);

    const float cx = (float) width - 20.0f;
    const float cy = (float) height * 0.5f;

    juce::Path arrow;
    arrow.startNewSubPath (cx - 5.0f, cy - 2.5f);
    arrow.lineTo (cx, cy + 3.0f);
    arrow.lineTo (cx + 5.0f, cy - 2.5f);

    g.setColour (cyan);
    g.strokePath (arrow, juce::PathStrokeType (2.0f));
}

//==============================================================================
// Editor
//==============================================================================
PerreoTrapSynthAudioProcessorEditor::PerreoTrapSynthAudioProcessorEditor (PerreoTrapSynthAudioProcessor& p)
    : juce::AudioProcessorEditor (&p), processorRef (p)
{
    using namespace NeonColours;

    setLookAndFeel (&neonLookAndFeel);

    const juce::Font smallBold (juce::FontOptions (14.0f, juce::Font::bold));

    // Selector de presets
    presetTitleLabel.setText ("PRESET", juce::dontSendNotification);
    presetTitleLabel.setFont (smallBold);
    presetTitleLabel.setColour (juce::Label::textColourId, magenta);
    presetTitleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (presetTitleLabel);

    for (int i = 0; i < PerreoTrapSynthAudioProcessor::numPresets; ++i)
    {
        if (i == 10)
        {
            presetBox.addSeparator();
            presetBox.addSectionHeading ("EXTRA PRESETS");
        }

        presetBox.addItem (processorRef.getProgramName (i), i + 1);
    }

    presetBox.setSelectedItemIndex (processorRef.getCurrentProgram(), juce::dontSendNotification);
    presetBox.onChange = [this]
    {
        const int idx = presetBox.getSelectedItemIndex();

        if (idx >= 0)
        {
            processorRef.setCurrentProgram (idx);
            updateActivePresetLabel (idx);
        }
    };
    addAndMakeVisible (presetBox);

    activePresetLabel.setFont (juce::Font (juce::FontOptions (16.0f, juce::Font::bold)));
    activePresetLabel.setColour (juce::Label::textColourId, cyan);
    activePresetLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (activePresetLabel);
    updateActivePresetLabel (processorRef.getCurrentProgram());

    importButton.onClick = [this] { importPreset(); };
    exportButton.onClick = [this] { exportPreset(); };
    addAndMakeVisible (importButton);
    addAndMakeVisible (exportButton);

    // Selector de onda
    waveTitleLabel.setText ("WAVE", juce::dontSendNotification);
    waveTitleLabel.setFont (smallBold);
    waveTitleLabel.setColour (juce::Label::textColourId, cyan);
    waveTitleLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (waveTitleLabel);

    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*> (processorRef.apvts.getParameter ("oscWave")))
        waveBox.addItemList (choiceParam->choices, 1);

    addAndMakeVisible (waveBox);
    waveAttachment = std::make_unique<ComboBoxAttachment> (processorRef.apvts, "oscWave", waveBox);

    // Knobs
    setupKnob (cutoffSlider,    cutoffLabel,    "CUTOFF (Hz)", magenta);
    setupKnob (resonanceSlider, resonanceLabel, "RESONANCE",   cyan);
    setupKnob (driveSlider,     driveLabel,     "DRIVE",       magenta);
    setupKnob (reverbSlider,    reverbLabel,    "REVERB",      cyan);
    setupKnob (masterSlider,    masterLabel,    "MASTER VOL",  magenta);

    cutoffAttachment    = std::make_unique<SliderAttachment> (processorRef.apvts, "cutoff",    cutoffSlider);
    resonanceAttachment = std::make_unique<SliderAttachment> (processorRef.apvts, "resonance", resonanceSlider);
    driveAttachment     = std::make_unique<SliderAttachment> (processorRef.apvts, "drive",     driveSlider);
    reverbAttachment    = std::make_unique<SliderAttachment> (processorRef.apvts, "reverb",    reverbSlider);
    masterAttachment    = std::make_unique<SliderAttachment> (processorRef.apvts, "masterVol", masterSlider);

    setSize (780, 460);
    startTimerHz (10);
}

PerreoTrapSynthAudioProcessorEditor::~PerreoTrapSynthAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void PerreoTrapSynthAudioProcessorEditor::setupKnob (juce::Slider& slider, juce::Label& label,
                                                     const juce::String& text, juce::Colour accent)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 96, 22);
    slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                juce::MathConstants<float>::pi * 2.8f, true);
    slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
    label.setColour (juce::Label::textColourId, accent);
    label.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (label);
}

void PerreoTrapSynthAudioProcessorEditor::updateActivePresetLabel (int presetIndex)
{
    const juce::String custom = processorRef.getCustomPresetName();

    const juce::String text = custom.isNotEmpty()
                                ? "IMPORTED PRESET:  " + custom
                                : "ACTIVE PRESET:  " + processorRef.getProgramName (presetIndex);

    if (activePresetLabel.getText() != text)
        activePresetLabel.setText (text, juce::dontSendNotification);
}

void PerreoTrapSynthAudioProcessorEditor::importPreset()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Importar preset",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
        "*.ptspreset");

    juce::Component::SafePointer<PerreoTrapSynthAudioProcessorEditor> safeThis (this);

    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [safeThis] (const juce::FileChooser& chooser)
    {
        if (safeThis == nullptr)
            return;

        const auto file = chooser.getResult();

        if (! file.existsAsFile())
            return;

        if (! safeThis->processorRef.importPresetFromFile (file))
        {
            juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon,
                                                    "Importar preset",
                                                    "El archivo no es un preset valido de PerreoTrapSynth.");
            return;
        }

        safeThis->updateActivePresetLabel (safeThis->processorRef.getCurrentProgram());
    });
}

void PerreoTrapSynthAudioProcessorEditor::exportPreset()
{
    const juce::String custom = processorRef.getCustomPresetName();
    const juce::String presetName = custom.isNotEmpty() ? custom
                                                        : processorRef.getProgramName (processorRef.getCurrentProgram());

    const auto defaultFile = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                                 .getChildFile (juce::File::createLegalFileName (presetName) + ".ptspreset");

    fileChooser = std::make_unique<juce::FileChooser> ("Exportar preset", defaultFile, "*.ptspreset");

    juce::Component::SafePointer<PerreoTrapSynthAudioProcessorEditor> safeThis (this);

    fileChooser->launchAsync (juce::FileBrowserComponent::saveMode
                                  | juce::FileBrowserComponent::canSelectFiles
                                  | juce::FileBrowserComponent::warnAboutOverwriting,
                              [safeThis, presetName] (const juce::FileChooser& chooser)
    {
        if (safeThis == nullptr)
            return;

        auto file = chooser.getResult();

        if (file == juce::File())
            return;

        if (! file.hasFileExtension ("ptspreset"))
            file = juce::File (file.getFullPathName() + ".ptspreset");

        if (! safeThis->processorRef.exportPresetToFile (file, presetName))
        {
            juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon,
                                                    "Exportar preset",
                                                    "No se pudo guardar el archivo.");
        }
    });
}

void PerreoTrapSynthAudioProcessorEditor::timerCallback()
{
    const int idx = processorRef.getCurrentProgram();

    if (presetBox.getSelectedItemIndex() != idx)
        presetBox.setSelectedItemIndex (idx, juce::dontSendNotification);

    updateActivePresetLabel (idx);
}

void PerreoTrapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace NeonColours;

    // Fondo carbon
    g.setGradientFill (juce::ColourGradient (charcoal, 0.0f, 0.0f,
                                             charcoalDeep, 0.0f, (float) getHeight(), false));
    g.fillAll();

    // Lineas neon superior e inferior
    g.setGradientFill (juce::ColourGradient (magenta, 0.0f, 0.0f, cyan, (float) getWidth(), 0.0f, false));
    g.fillRect (0, 0, getWidth(), 3);
    g.fillRect (0, getHeight() - 3, getWidth(), 3);

    // Titulo
    const juce::Font titleFont (juce::FontOptions (32.0f, juce::Font::bold));
    const juce::String part1 = "PERREO TRAP ";
    const juce::String part2 = "SYNTH";

    juce::GlyphArrangement ga1, ga2;
    ga1.addLineOfText (titleFont, part1, 0.0f, 0.0f);
    ga2.addLineOfText (titleFont, part2, 0.0f, 0.0f);

    const float w1 = ga1.getBoundingBox (0, -1, true).getWidth();
    const float w2 = ga2.getBoundingBox (0, -1, true).getWidth();
    const float startX = ((float) getWidth() - (w1 + w2)) * 0.5f;

    const juce::Rectangle<float> area1 (startX, 12.0f, w1 + 4.0f, 44.0f);
    const juce::Rectangle<float> area2 (startX + w1, 12.0f, w2 + 4.0f, 44.0f);

    g.setFont (titleFont);

    // Brillo
    for (int dx = -2; dx <= 2; dx += 2)
    {
        for (int dy = -2; dy <= 2; dy += 2)
        {
            g.setColour (magenta.withAlpha (0.10f));
            g.drawText (part1, area1.translated ((float) dx, (float) dy), juce::Justification::centredLeft, false);
            g.setColour (cyan.withAlpha (0.10f));
            g.drawText (part2, area2.translated ((float) dx, (float) dy), juce::Justification::centredLeft, false);
        }
    }

    g.setColour (magenta);
    g.drawText (part1, area1, juce::Justification::centredLeft, false);
    g.setColour (cyan);
    g.drawText (part2, area2, juce::Justification::centredLeft, false);

    // Panel de knobs
    if (! knobPanelBounds.isEmpty())
    {
        const auto panelF = knobPanelBounds.toFloat();

        g.setColour (panel.withAlpha (0.85f));
        g.fillRoundedRectangle (panelF, 14.0f);

        g.setGradientFill (juce::ColourGradient (magenta.withAlpha (0.8f), panelF.getX(), panelF.getY(),
                                                 cyan.withAlpha (0.8f), panelF.getRight(), panelF.getBottom(), false));
        g.drawRoundedRectangle (panelF, 14.0f, 1.8f);
    }
}

void PerreoTrapSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (24);
    area.removeFromTop (48); // espacio del titulo

    // Fila de presets y onda
    auto presetRow = area.removeFromTop (38);
    presetTitleLabel.setBounds (presetRow.removeFromLeft (70));
    waveBox.setBounds (presetRow.removeFromRight (150));
    waveTitleLabel.setBounds (presetRow.removeFromRight (60));
    presetRow.removeFromRight (12);
    presetBox.setBounds (presetRow);

    area.removeFromTop (8);
    auto activeRow = area.removeFromTop (28);
    importButton.setBounds (activeRow.removeFromLeft (96));
    exportButton.setBounds (activeRow.removeFromRight (96));
    activePresetLabel.setBounds (activeRow);
    area.removeFromTop (12);

    // Panel de knobs
    knobPanelBounds = area;
    auto knobArea = area.reduced (10);

    const int colWidth = knobArea.getWidth() / 5;

    auto layoutColumn = [&] (juce::Label& label, juce::Slider& slider)
    {
        auto col = knobArea.removeFromLeft (colWidth).reduced (6);
        label.setBounds (col.removeFromTop (22));
        slider.setBounds (col);
    };

    layoutColumn (cutoffLabel,    cutoffSlider);
    layoutColumn (resonanceLabel, resonanceSlider);
    layoutColumn (driveLabel,     driveSlider);
    layoutColumn (reverbLabel,    reverbSlider);
    layoutColumn (masterLabel,    masterSlider);
}
