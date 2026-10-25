#include "PluginEditor.h"

using namespace NeonColours;

namespace
{
    juce::Font boldFont (float size)
    {
        return juce::Font (juce::FontOptions (size, juce::Font::bold));
    }

    // Dibuja un trazo con halo de neon
    void strokeGlow (juce::Graphics& g, const juce::Path& path, juce::Colour c, float width, float glowAlpha = 0.07f)
    {
        for (int i = 3; i >= 1; --i)
        {
            g.setColour (c.withAlpha (glowAlpha * (float) (4 - i)));
            g.strokePath (path, juce::PathStrokeType (width + (float) i * 3.5f,
                                                      juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));
        }

        g.setColour (c);
        g.strokePath (path, juce::PathStrokeType (width, juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));
    }

    void outlineGlow (juce::Graphics& g, juce::Rectangle<float> r, float corner, juce::Colour c,
                      float width, float alpha = 1.0f)
    {
        for (int i = 3; i >= 1; --i)
        {
            g.setColour (c.withAlpha (0.05f * (float) (4 - i) * alpha));
            g.drawRoundedRectangle (r.expanded ((float) i * 0.8f), corner + (float) i * 0.8f, width + (float) i * 2.0f);
        }

        g.setColour (c.withAlpha (alpha));
        g.drawRoundedRectangle (r, corner, width);
    }
}

//==============================================================================
// NeonLookAndFeel
//==============================================================================
NeonLookAndFeel::NeonLookAndFeel()
{
    setColour (juce::ComboBox::backgroundColourId, panelDeep);
    setColour (juce::ComboBox::textColourId,       juce::Colours::white);
    setColour (juce::ComboBox::outlineColourId,    cyan);
    setColour (juce::ComboBox::arrowColourId,      cyan);

    setColour (juce::PopupMenu::backgroundColourId,            panel);
    setColour (juce::PopupMenu::textColourId,                  juce::Colours::white);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, magenta.withAlpha (0.55f));
    setColour (juce::PopupMenu::highlightedTextColourId,       juce::Colours::white);

    setColour (juce::Slider::textBoxTextColourId,       juce::Colours::white);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId,  magenta.withAlpha (0.5f));

    setColour (juce::Label::textColourId, juce::Colours::white);

    setColour (juce::TextButton::buttonColourId,   panelDeep);
    setColour (juce::TextButton::buttonOnColourId, magenta);
    setColour (juce::TextButton::textColourOffId,  textDim);
    setColour (juce::TextButton::textColourOnId,   juce::Colours::white);
}

juce::Font NeonLookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    return boldFont (13.0f);
}

juce::Font NeonLookAndFeel::getPopupMenuFont()
{
    return boldFont (14.0f);
}

void NeonLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float startAngle, float endAngle,
                                        juce::Slider& slider)
{
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);

    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (3.0f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();

    const float track  = juce::jmax (3.0f, radius * 0.15f);
    const float arcR   = radius - track * 1.2f;
    const float angle  = startAngle + sliderPos * (endAngle - startAngle);

    const bool bipolar = slider.getMinimum() < 0.0 && slider.getMaximum() > 0.0;
    const float zeroPos = bipolar ? (float) slider.valueToProportionOfLength (0.0) : 0.0f;
    const float zeroAngle = startAngle + zeroPos * (endAngle - startAngle);

    // Pista de fondo
    juce::Path bg;
    bg.addCentredArc (centre.x, centre.y, arcR, arcR, 0.0f, startAngle, endAngle, true);
    g.setColour (accent.withAlpha (0.14f));
    g.strokePath (bg, juce::PathStrokeType (track, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Arco de valor
    const float from = bipolar ? juce::jmin (zeroAngle, angle) : startAngle;
    const float to   = bipolar ? juce::jmax (zeroAngle, angle) : angle;

    if (to - from > 0.01f)
    {
        juce::Path value;
        value.addCentredArc (centre.x, centre.y, arcR, arcR, 0.0f, from, to, true);
        strokeGlow (g, value, accent, track);
    }

    // Cuerpo
    const float inner = juce::jmax (4.0f, arcR - track - 2.0f);
    g.setGradientFill (juce::ColourGradient (panel.brighter (0.35f), centre.x, centre.y - inner,
                                             panelDeep, centre.x, centre.y + inner, false));
    g.fillEllipse (centre.x - inner, centre.y - inner, inner * 2.0f, inner * 2.0f);

    g.setColour (accent.withAlpha (0.5f));
    g.drawEllipse (centre.x - inner, centre.y - inner, inner * 2.0f, inner * 2.0f, 1.2f);

    // Indicador
    juce::Path pointer;
    pointer.addRoundedRectangle (-1.6f, -inner + 2.5f, 3.2f, inner * 0.5f, 1.6f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
    g.setColour (accent.brighter (0.5f));
    g.fillPath (pointer);
}

void NeonLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                    int, int, int, int, juce::ComboBox& box)
{
    const auto bounds = juce::Rectangle<float> (0.0f, 0.0f, (float) width, (float) height).reduced (1.0f);

    g.setColour (panelDeep);
    g.fillRoundedRectangle (bounds, 7.0f);

    const auto c = box.findColour (juce::ComboBox::outlineColourId);
    outlineGlow (g, bounds, 7.0f, c, 1.3f, box.hasKeyboardFocus (true) ? 1.0f : 0.8f);

    const float cx = (float) width - 14.0f;
    const float cy = (float) height * 0.5f;

    juce::Path arrow;
    arrow.startNewSubPath (cx - 4.0f, cy - 2.0f);
    arrow.lineTo (cx, cy + 2.5f);
    arrow.lineTo (cx + 4.0f, cy - 2.0f);

    g.setColour (c);
    g.strokePath (arrow, juce::PathStrokeType (1.8f));
}

void NeonLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                                            bool highlighted, bool down)
{
    const auto bounds = button.getLocalBounds().toFloat().reduced (1.5f);
    const auto accent = button.findColour (juce::TextButton::buttonOnColourId);
    const bool on = button.getToggleState();
    const float corner = 7.0f;

    if (on)
    {
        g.setColour (accent.withAlpha (down ? 0.45f : 0.30f));
        g.fillRoundedRectangle (bounds, corner);
        outlineGlow (g, bounds, corner, accent, 1.6f);
    }
    else
    {
        g.setColour (panelDeep.brighter (highlighted ? 0.25f : 0.06f));
        g.fillRoundedRectangle (bounds, corner);
        g.setColour (accent.withAlpha (highlighted ? 0.85f : 0.30f));
        g.drawRoundedRectangle (bounds, corner, 1.2f);
    }
}

void NeonLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button, bool highlighted, bool)
{
    const bool on = button.getToggleState();

    g.setFont (boldFont (juce::jlimit (11.0f, 17.0f, (float) button.getHeight() * 0.42f)));

    if (on)
        g.setColour (juce::Colours::white);
    else
        g.setColour (highlighted ? juce::Colours::white : button.findColour (juce::TextButton::textColourOffId));

    g.drawFittedText (button.getButtonText(), button.getLocalBounds().reduced (6, 2),
                      juce::Justification::centred, 1);
}

void NeonLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool highlighted, bool)
{
    const auto accent = button.findColour (juce::ToggleButton::tickColourId);
    const bool on = button.getToggleState();

    const auto full = button.getLocalBounds().toFloat();
    const float h = juce::jmin (20.0f, full.getHeight() - 2.0f);
    const float w = juce::jmin (46.0f, full.getWidth() - 2.0f);
    const auto pill = juce::Rectangle<float> (w, h).withCentre (full.getCentre());

    g.setColour (on ? accent.withAlpha (0.25f) : panelDeep);
    g.fillRoundedRectangle (pill, h * 0.5f);

    if (on)
        outlineGlow (g, pill, h * 0.5f, accent, 1.4f);
    else
    {
        g.setColour (textDim.withAlpha (highlighted ? 0.9f : 0.55f));
        g.drawRoundedRectangle (pill, h * 0.5f, 1.2f);
    }

    const float d = h - 6.0f;
    const float knobX = on ? pill.getRight() - d - 3.0f : pill.getX() + 3.0f;
    const auto knob = juce::Rectangle<float> (knobX, pill.getY() + 3.0f, d, d);

    g.setColour (on ? accent : textDim.withAlpha (0.8f));
    g.fillEllipse (knob);

    g.setFont (boldFont (9.5f));
    g.setColour (on ? juce::Colours::white : textDim);

    auto textArea = pill.reduced (6.0f, 0.0f);

    if (on)
        g.drawText ("ON", textArea.withTrimmedRight (d + 2.0f), juce::Justification::centredLeft, false);
    else
        g.drawText ("OFF", textArea.withTrimmedLeft (d + 2.0f), juce::Justification::centredRight, false);
}

//==============================================================================
// KnobComp
//==============================================================================
KnobComp::KnobComp (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID,
                    const juce::String& name, juce::Colour accent)
{
    nameLabel.setText (name, juce::dontSendNotification);
    nameLabel.setFont (boldFont (11.0f));
    nameLabel.setColour (juce::Label::textColourId, accent);
    nameLabel.setJustificationType (juce::Justification::centred);
    nameLabel.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (nameLabel);

    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 15);
    slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                juce::MathConstants<float>::pi * 2.8f, true);
    slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
    slider.setPopupDisplayEnabled (false, false, nullptr);
    addAndMakeVisible (slider);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, paramID, slider);
}

void KnobComp::resized()
{
    auto r = getLocalBounds();
    nameLabel.setBounds (r.removeFromTop (14));
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, juce::jmax (40, r.getWidth()), 15);
    slider.setBounds (r);
}

//==============================================================================
// ComboComp
//==============================================================================
ComboComp::ComboComp (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID,
                      const juce::String& name, juce::Colour accent)
{
    nameLabel.setText (name, juce::dontSendNotification);
    nameLabel.setFont (boldFont (11.0f));
    nameLabel.setColour (juce::Label::textColourId, accent);
    nameLabel.setJustificationType (juce::Justification::centred);
    nameLabel.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (nameLabel);

    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramID)))
        box.addItemList (choice->choices, 1);

    box.setColour (juce::ComboBox::outlineColourId, accent);
    box.setColour (juce::ComboBox::arrowColourId, accent);
    box.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (box);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, paramID, box);
}

void ComboComp::resized()
{
    auto r = getLocalBounds();
    nameLabel.setBounds (r.removeFromTop (14));

    // El combo se centra verticalmente a la altura de las perillas
    const int h = 26;
    const int top = juce::jmax (4, (r.getHeight() - h) / 2 - 4);
    box.setBounds (r.withTrimmedTop (top).withHeight (h).reduced (2, 0));
}

//==============================================================================
// NeonPanel
//==============================================================================
NeonPanel::NeonPanel (const juce::String& t, juce::Colour c) : title (t), accent (c)
{
}

void NeonPanel::addControl (juce::Component& c, float weight)
{
    items.push_back ({ &c, weight });
    totalWeight += weight;
    addAndMakeVisible (c);
}

void NeonPanel::addPowerSwitch (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID)
{
    hasPower = true;
    powerButton.setColour (juce::ToggleButton::tickColourId, accent);
    powerButton.setClickingTogglesState (true);
    powerButton.setMouseCursor (juce::MouseCursor::PointingHandCursor);
    powerButton.setTooltip ("Activar / desactivar este efecto");
    addAndMakeVisible (powerButton);

    powerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, paramID, powerButton);
    powerButton.onClick = [this] { refreshPowerLook(); };

    lastPowerState = ! powerButton.getToggleState(); // fuerza el primer refresco
    refreshPowerLook();
}

void NeonPanel::refreshPowerLook()
{
    if (! hasPower)
        return;

    const bool on = powerButton.getToggleState();

    if (on == lastPowerState)
        return;

    lastPowerState = on;

    for (auto& it : items)
        it.comp->setAlpha (on ? 1.0f : 0.35f);

    repaint();
}

void NeonPanel::paint (juce::Graphics& g)
{
    const auto r = getLocalBounds().toFloat().reduced (1.5f);
    const bool on = ! hasPower || powerButton.getToggleState();

    g.setGradientFill (juce::ColourGradient (panel.brighter (0.06f), r.getX(), r.getY(),
                                             panelDeep, r.getX(), r.getBottom(), false));
    g.fillRoundedRectangle (r, 12.0f);

    outlineGlow (g, r, 12.0f, accent, 1.4f, on ? 0.85f : 0.25f);

    // Titulo
    g.setFont (boldFont (12.5f));
    g.setColour (on ? accent : textDim);
    g.drawText (title, 14, 6, getWidth() - 80, 18, juce::Justification::centredLeft, false);

    // Linea bajo el titulo
    g.setColour (accent.withAlpha (on ? 0.35f : 0.12f));
    g.fillRect (14.0f, 26.0f, (float) getWidth() - 28.0f, 1.0f);
}

void NeonPanel::resized()
{
    if (hasPower)
        powerButton.setBounds (getWidth() - 12 - 48, 5, 48, 20);

    auto area = getLocalBounds().withTrimmedTop (30).reduced (8, 4);

    if (totalWeight <= 0.0f)
        return;

    const float unit = (float) area.getWidth() / totalWeight;
    float x = (float) area.getX();

    for (auto& it : items)
    {
        const float w = unit * it.weight;
        it.comp->setBounds (juce::Rectangle<float> (x, (float) area.getY(), w, (float) area.getHeight())
                                .reduced (2.0f, 0.0f).toNearestInt());
        x += w;
    }
}

//==============================================================================
// Editor
//==============================================================================
KnobComp& PerreoTrapSynthAudioProcessorEditor::addKnob (NeonPanel& target, const juce::String& id,
                                                        const juce::String& name, juce::Colour accent)
{
    knobs.push_back (std::make_unique<KnobComp> (processorRef.apvts, id, name, accent));
    target.addControl (*knobs.back(), 1.0f);
    return *knobs.back();
}

ComboComp& PerreoTrapSynthAudioProcessorEditor::addCombo (NeonPanel& target, const juce::String& id,
                                                          const juce::String& name, juce::Colour accent)
{
    combos.push_back (std::make_unique<ComboComp> (processorRef.apvts, id, name, accent));
    target.addControl (*combos.back(), 1.45f);
    return *combos.back();
}

NeonPanel& PerreoTrapSynthAudioProcessorEditor::addPanel (const juce::String& title, juce::Colour accent, int row)
{
    panels.push_back (std::make_unique<NeonPanel> (title, accent));
    addAndMakeVisible (*panels.back());
    rows[(size_t) row].push_back (panels.back().get());
    return *panels.back();
}

PerreoTrapSynthAudioProcessorEditor::PerreoTrapSynthAudioProcessorEditor (PerreoTrapSynthAudioProcessor& p)
    : juce::AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&neonLookAndFeel);

    //--------------------------------------------------------------------------
    // Navegador:  DIVISION  ->  CATEGORIA  ->  PRESET
    //--------------------------------------------------------------------------
    const juce::Colour genreColours[] = { magenta, cyan };

    for (int g = 0; g < PresetLayout::numGenres; ++g)
    {
        auto& b = genreButtons[(size_t) g];
        b.setButtonText (PresetLayout::genreName (g));
        b.setClickingTogglesState (true);
        b.setRadioGroupId (101);
        b.setColour (juce::TextButton::buttonOnColourId, genreColours[g]);
        b.onClick = [this, g]
        {
            const int cur = processorRef.getCurrentProgram();
            loadPreset (g, PresetLayout::categoryOf (cur), PresetLayout::slotOf (cur));
        };
        addAndMakeVisible (b);
    }

    for (int c = 0; c < PresetLayout::numCategories; ++c)
    {
        auto& b = categoryButtons[(size_t) c];
        b.setButtonText (PresetLayout::categoryName (c));
        b.setClickingTogglesState (true);
        b.setRadioGroupId (102);
        b.onClick = [this, c]
        {
            loadPreset (PresetLayout::genreOf (processorRef.getCurrentProgram()), c, 0);
        };
        addAndMakeVisible (b);
    }

    for (int s = 0; s < PresetLayout::perCategory; ++s)
    {
        auto& b = slotButtons[(size_t) s];
        b.setClickingTogglesState (true);
        b.setRadioGroupId (103);
        b.onClick = [this, s]
        {
            const int cur = processorRef.getCurrentProgram();
            loadPreset (PresetLayout::genreOf (cur), PresetLayout::categoryOf (cur), s);
        };
        addAndMakeVisible (b);
    }

    for (auto* b : { &prevButton, &nextButton, &importButton, &exportButton })
    {
        b->setColour (juce::TextButton::buttonOnColourId, cyan);
        addAndMakeVisible (*b);
    }

    prevButton.onClick = [this]
    {
        const int n = PerreoTrapSynthAudioProcessor::numPresets;
        processorRef.setCurrentProgram ((processorRef.getCurrentProgram() + n - 1) % n);
        syncBrowser();
    };

    nextButton.onClick = [this]
    {
        processorRef.setCurrentProgram ((processorRef.getCurrentProgram() + 1) % PerreoTrapSynthAudioProcessor::numPresets);
        syncBrowser();
    };

    importButton.onClick = [this] { importPreset(); };
    exportButton.onClick = [this] { exportPreset(); };

    presetNameLabel.setFont (boldFont (16.0f));
    presetNameLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    presetNameLabel.setJustificationType (juce::Justification::centred);
    presetNameLabel.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (presetNameLabel);

    //--------------------------------------------------------------------------
    // Paneles de sonido
    //--------------------------------------------------------------------------
    // Fila 1: osciladores
    {
        auto& a = addPanel ("OSCILLATOR A", magenta, 0);
        addCombo (a, "aWave", "WAVE", magenta);
        addKnob (a, "aOct",  "OCTAVE", magenta);
        addKnob (a, "aSemi", "SEMI",   magenta);
        addKnob (a, "aFine", "FINE",   magenta);
        addKnob (a, "aLvl",  "LEVEL",  magenta);

        auto& b = addPanel ("OSCILLATOR B", cyan, 0);
        addCombo (b, "bWave", "WAVE", cyan);
        addKnob (b, "bOct",  "OCTAVE", cyan);
        addKnob (b, "bSemi", "SEMI",   cyan);
        addKnob (b, "bFine", "FINE",   cyan);
        addKnob (b, "bLvl",  "LEVEL",  cyan);

        auto& u = addPanel ("UNISON", violet, 0);
        addKnob (u, "uniVoices", "VOICES", violet);
        addKnob (u, "uniDetune", "DETUNE", violet);
        addKnob (u, "uniSpread", "STEREO", violet);
    }

    // Fila 2: filtro, envolventes, master
    {
        auto& f = addPanel ("FILTER", cyan, 1);
        addCombo (f, "fltType", "TYPE", cyan);
        addKnob (f, "cutoff", "CUTOFF", cyan);
        addKnob (f, "reso",   "RESO",   cyan);
        addKnob (f, "fltEnv", "ENV AMT", cyan);

        auto& fe = addPanel ("FILTER ENVELOPE", cyan, 1);
        addKnob (fe, "fA", "ATTACK",  cyan);
        addKnob (fe, "fD", "DECAY",   cyan);
        addKnob (fe, "fS", "SUSTAIN", cyan);
        addKnob (fe, "fR", "RELEASE", cyan);

        auto& ae = addPanel ("AMP ENVELOPE", magenta, 1);
        addKnob (ae, "aA", "ATTACK",  magenta);
        addKnob (ae, "aD", "DECAY",   magenta);
        addKnob (ae, "aS", "SUSTAIN", magenta);
        addKnob (ae, "aR", "RELEASE", magenta);

        auto& m = addPanel ("OUTPUT", lime, 1);
        addKnob (m, "master", "MASTER", lime);
    }

    // Fila 3: LFO, pitch env, mezcla
    {
        auto& l = addPanel ("LFO", lime, 2);
        addCombo (l, "lfoShape", "SHAPE",  lime);
        addCombo (l, "lfoDest",  "TARGET", lime);
        addKnob  (l, "lfoRate",  "RATE",   lime);
        addKnob  (l, "lfoDepth", "DEPTH",  lime);

        auto& pe = addPanel ("PITCH ENVELOPE", orange, 2);
        addKnob (pe, "pitEnv", "AMOUNT", orange);
        addKnob (pe, "pitDec", "TIME",   orange);

        auto& mix = addPanel ("SUB / NOISE / PULSE", violet, 2);
        addKnob (mix, "subLvl",   "SUB",   violet);
        addKnob (mix, "noiseLvl", "NOISE", violet);
        addKnob (mix, "pw",       "PULSE W", violet);
    }

    // Fila 4: efectos (cada uno con su interruptor)
    {
        auto& d = addPanel ("DRIVE", orange, 3);
        addKnob (d, "drive", "AMOUNT", orange);
        d.addPowerSwitch (processorRef.apvts, "drvOn");

        auto& c = addPanel ("CHORUS", cyan, 3);
        addKnob (c, "choRate",  "RATE",  cyan);
        addKnob (c, "choDepth", "DEPTH", cyan);
        addKnob (c, "choMix",   "MIX",   cyan);
        c.addPowerSwitch (processorRef.apvts, "choOn");

        auto& dl = addPanel ("DELAY", magenta, 3);
        addKnob (dl, "dlyTime", "TIME",     magenta);
        addKnob (dl, "dlyFb",   "FEEDBACK", magenta);
        addKnob (dl, "dlyMix",  "MIX",      magenta);
        dl.addPowerSwitch (processorRef.apvts, "dlyOn");

        auto& r = addPanel ("REVERB", violet, 3);
        addKnob (r, "revSize", "SIZE", violet);
        addKnob (r, "revMix",  "MIX",  violet);
        r.addPowerSwitch (processorRef.apvts, "revOn");
    }

    syncBrowser();

    setResizeLimits (900, 640, 1600, 1100);
    setResizable (true, true);
    setSize (1120, 780);

    startTimerHz (10);
}

PerreoTrapSynthAudioProcessorEditor::~PerreoTrapSynthAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

//------------------------------------------------------------------------------
void PerreoTrapSynthAudioProcessorEditor::loadPreset (int genre, int category, int slot)
{
    processorRef.setCurrentProgram (PresetLayout::indexOf (genre, category, slot));
    syncBrowser();
}

void PerreoTrapSynthAudioProcessorEditor::syncBrowser()
{
    const int idx = processorRef.getCurrentProgram();
    const int g = PresetLayout::genreOf (idx);
    const int c = PresetLayout::categoryOf (idx);
    const int s = PresetLayout::slotOf (idx);

    if (g != shownGenre)
    {
        shownGenre = g;
        const auto accent = (g == 0) ? magenta : cyan;

        for (auto& b : categoryButtons) b.setColour (juce::TextButton::buttonOnColourId, accent);
        for (auto& b : slotButtons)     b.setColour (juce::TextButton::buttonOnColourId, accent);

        repaint();
    }

    for (int i = 0; i < PresetLayout::numGenres; ++i)
        genreButtons[(size_t) i].setToggleState (i == g, juce::dontSendNotification);

    for (int i = 0; i < PresetLayout::numCategories; ++i)
        categoryButtons[(size_t) i].setToggleState (i == c, juce::dontSendNotification);

    for (int i = 0; i < PresetLayout::perCategory; ++i)
    {
        auto& b = slotButtons[(size_t) i];
        b.setButtonText (processorRef.getPresetShortName (PresetLayout::indexOf (g, c, i)));
        b.setToggleState (i == s, juce::dontSendNotification);
    }

    const juce::String custom = processorRef.getCustomPresetName();
    juce::String text;

    if (custom.isNotEmpty())
        text = "IMPORTED:  " + custom;
    else
        text = processorRef.getProgramName (idx).replace (" - ", "   /   ");

    if (text != shownName)
    {
        shownName = text;
        presetNameLabel.setText (text, juce::dontSendNotification);
    }
}

void PerreoTrapSynthAudioProcessorEditor::timerCallback()
{
    syncBrowser();

    for (auto& p : panels)
        p->refreshPowerLook();
}

//------------------------------------------------------------------------------
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

        safeThis->syncBrowser();
    });
}

void PerreoTrapSynthAudioProcessorEditor::exportPreset()
{
    const juce::String custom = processorRef.getCustomPresetName();
    const juce::String presetName = custom.isNotEmpty() ? custom
                                                        : processorRef.getPresetShortName (processorRef.getCurrentProgram());

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

//------------------------------------------------------------------------------
void PerreoTrapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    const float w = (float) getWidth();
    const float h = (float) getHeight();

    // Fondo
    g.setGradientFill (juce::ColourGradient (bgTop, 0.0f, 0.0f, bgBottom, 0.0f, h, false));
    g.fillAll();

    // Resplandores de esquina
    g.setGradientFill (juce::ColourGradient (magenta.withAlpha (0.16f), 0.0f, 0.0f,
                                             magenta.withAlpha (0.0f), w * 0.45f, h * 0.35f, true));
    g.fillRect (getLocalBounds());
    g.setGradientFill (juce::ColourGradient (cyan.withAlpha (0.13f), w, h,
                                             cyan.withAlpha (0.0f), w * 0.55f, h * 0.6f, true));
    g.fillRect (getLocalBounds());

    // Rejilla sutil
    g.setColour (violet.withAlpha (0.045f));
    for (float x = 0.0f; x < w; x += 32.0f) g.drawVerticalLine ((int) x, 0.0f, h);
    for (float y = 0.0f; y < h; y += 32.0f) g.drawHorizontalLine ((int) y, 0.0f, w);

    // Lineas neon superior e inferior
    g.setGradientFill (juce::ColourGradient (magenta, 0.0f, 0.0f, cyan, w, 0.0f, false));
    g.fillRect (0.0f, 0.0f, w, 3.0f);
    g.fillRect (0.0f, h - 3.0f, w, 3.0f);

    // Titulo con halo
    const auto titleFont = boldFont (28.0f);
    const juce::String part1 = "PERREO TRAP ";
    const juce::String part2 = "SYNTH";

    juce::GlyphArrangement ga1, ga2;
    ga1.addLineOfText (titleFont, part1, 0.0f, 0.0f);
    ga2.addLineOfText (titleFont, part2, 0.0f, 0.0f);

    const float w1 = ga1.getBoundingBox (0, -1, true).getWidth();
    const float w2 = ga2.getBoundingBox (0, -1, true).getWidth();

    const juce::Rectangle<float> area1 (18.0f, 6.0f, w1 + 4.0f, 38.0f);
    const juce::Rectangle<float> area2 (18.0f + w1, 6.0f, w2 + 4.0f, 38.0f);

    g.setFont (titleFont);

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

    g.setFont (boldFont (11.0f));
    g.setColour (textDim);
    g.drawText ("60 PRESETS   |   TRAP  x  REGGAETON   |   DOUBLE-CLICK A KNOB TO RESET",
                getLocalBounds().withTrimmedRight (18).withHeight (44),
                juce::Justification::centredRight, false);

    // Caja del navegador
    if (! browserBounds.isEmpty())
    {
        const auto accent = (shownGenre == 1) ? cyan : magenta;
        const auto r = browserBounds.toFloat().reduced (1.5f);

        g.setColour (panel.withAlpha (0.9f));
        g.fillRoundedRectangle (r, 12.0f);
        outlineGlow (g, r, 12.0f, accent, 1.4f, 0.7f);
    }
}

void PerreoTrapSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (14, 8);
    area.removeFromTop (38); // titulo

    //--------------------------------------------------------------------------
    // Navegador
    //--------------------------------------------------------------------------
    browserBounds = area.removeFromTop (118);
    auto b = browserBounds.reduced (12, 8);

    {
        auto row = b.removeFromTop (34);

        for (auto& g : genreButtons)
        {
            g.setBounds (row.removeFromLeft (150));
            row.removeFromLeft (8);
        }

        exportButton.setBounds (row.removeFromRight (84));
        row.removeFromRight (6);
        importButton.setBounds (row.removeFromRight (84));
        row.removeFromRight (12);
        nextButton.setBounds (row.removeFromRight (34));
        row.removeFromRight (6);
        prevButton.setBounds (row.removeFromRight (34));

        presetNameLabel.setBounds (row.reduced (10, 0));
    }

    b.removeFromTop (8);

    {
        auto row = b.removeFromTop (28);
        const int gap = 6;
        const int w = (row.getWidth() - gap * (PresetLayout::numCategories - 1)) / PresetLayout::numCategories;

        for (auto& c : categoryButtons)
        {
            c.setBounds (row.removeFromLeft (w));
            row.removeFromLeft (gap);
        }
    }

    b.removeFromTop (8);

    {
        auto row = b.removeFromTop (juce::jmin (32, b.getHeight()));
        const int gap = 6;
        const int w = (row.getWidth() - gap * (PresetLayout::perCategory - 1)) / PresetLayout::perCategory;

        for (auto& s : slotButtons)
        {
            s.setBounds (row.removeFromLeft (w));
            row.removeFromLeft (gap);
        }
    }

    //--------------------------------------------------------------------------
    // Filas de paneles
    //--------------------------------------------------------------------------
    area.removeFromTop (8);

    const int gap = 8;
    const int rowH = (area.getHeight() - gap * (numRows - 1)) / numRows;

    for (int r = 0; r < numRows; ++r)
    {
        auto rowArea = area.removeFromTop (rowH);
        area.removeFromTop (gap);

        auto& list = rows[(size_t) r];

        float sum = 0.0f;
        for (auto* p : list)
            sum += p->getTotalWeight() + 0.6f;

        const float avail = (float) rowArea.getWidth() - (float) gap * (float) ((int) list.size() - 1);
        float x = (float) rowArea.getX();

        for (auto* p : list)
        {
            const float pw = avail * (p->getTotalWeight() + 0.6f) / sum;
            p->setBounds (juce::Rectangle<float> (x, (float) rowArea.getY(), pw, (float) rowArea.getHeight()).toNearestInt());
            x += pw + (float) gap;
        }
    }
}
