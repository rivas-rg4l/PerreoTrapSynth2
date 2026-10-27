#include "PluginEditor.h"

using namespace NeonColours;

namespace
{
    juce::Font boldFont (float size)
    {
        return juce::Font (juce::FontOptions (size, juce::Font::bold));
    }

    // Trazo limpio (sin halo)
    void strokeGlow (juce::Graphics& g, const juce::Path& path, juce::Colour c, float width, float = 0.0f)
    {
        g.setColour (c);
        g.strokePath (path, juce::PathStrokeType (width, juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));
    }

    void outlineGlow (juce::Graphics& g, juce::Rectangle<float> r, float corner, juce::Colour c,
                      float width, float alpha = 1.0f)
    {
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

    g.setFont (boldFont (juce::jlimit (10.5f, 13.5f, (float) button.getHeight() * 0.42f)));

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
NeonPanel::NeonPanel (const juce::String& t, juce::Colour c, const juce::String& sub)
    : title (t), subtitle (sub), accent (c)
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

    // Subtitulo explicativo (en gris, a la derecha del titulo)
    if (subtitle.isNotEmpty())
    {
        juce::GlyphArrangement ga;
        ga.addLineOfText (boldFont (12.5f), title, 0.0f, 0.0f);
        const int tw = (int) ga.getBoundingBox (0, -1, true).getWidth();
        const int x0 = 14 + tw + 10;
        const int x1 = getWidth() - 14 - (hasPower ? 60 : 0);

        if (x1 - x0 > 40)
        {
            g.setFont (juce::Font (juce::FontOptions (11.0f)));
            g.setColour (textDim.withAlpha (on ? 0.9f : 0.5f));
            g.drawFittedText (subtitle, x0, 6, x1 - x0, 18, juce::Justification::centredLeft, 1, 0.85f);
        }
    }

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

NeonPanel& PerreoTrapSynthAudioProcessorEditor::addPanel (const juce::String& title, juce::Colour accent, int row,
                                                          const juce::String& subtitle)
{
    panels.push_back (std::make_unique<NeonPanel> (title, accent, subtitle));
    addAndMakeVisible (*panels.back());
    rows[(size_t) row].push_back (panels.back().get());
    return *panels.back();
}

PerreoTrapSynthAudioProcessorEditor::PerreoTrapSynthAudioProcessorEditor (PerreoTrapSynthAudioProcessor& p)
    : juce::AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&neonLookAndFeel);

    //--------------------------------------------------------------------------
    // Navegador: un visor con el preset actual; click = menu completo
    //--------------------------------------------------------------------------
    presetDisplay.onClick = [this] { showPresetMenu(); };
    addAndMakeVisible (presetDisplay);

    for (auto* b : { &prevButton, &nextButton, &saveButton, &importButton, &exportButton, &wavesButton, &randomButton })
    {
        b->setColour (juce::TextButton::buttonOnColourId, cyan);
        addAndMakeVisible (*b);
    }

    prevButton.onClick   = [this] { stepPreset (-1); };
    nextButton.onClick   = [this] { stepPreset (+1); };
    saveButton.onClick   = [this] { saveToUser(); };
    importButton.onClick = [this] { importPack(); };
    exportButton.onClick = [this] { exportPackMenu(); };

    wavesButton.setClickingTogglesState (true);
    wavesButton.onClick  = [this] { setWaveEditorOpen (wavesButton.getToggleState()); };
    randomButton.onClick = [this] { processorRef.randomizeSound(); syncBrowser(); };
    addChildComponent (waveEditor);

    wavesButton.setTooltip ("Abre el editor de ondas");
    randomButton.setTooltip ("Genera un sonido aleatorio");
    prevButton.setTooltip ("Preset anterior");
    nextButton.setTooltip ("Preset siguiente");

    //--------------------------------------------------------------------------
    // Paneles de sonido
    //--------------------------------------------------------------------------
    // Fila 1: osciladores
    {
        auto& a = addPanel ("OSCILLATOR A", magenta, 0, "el sonido base");
        addCombo (a, "aWave", "WAVE", magenta);
        addKnob (a, "aOct",  "OCTAVE", magenta);
        addKnob (a, "aSemi", "SEMI",   magenta);
        addKnob (a, "aFine", "FINE",   magenta);
        addKnob (a, "aLvl",  "LEVEL",  magenta);

        auto& b = addPanel ("OSCILLATOR B", cyan, 0, "una segunda capa");
        addCombo (b, "bWave", "WAVE", cyan);
        addKnob (b, "bOct",  "OCTAVE", cyan);
        addKnob (b, "bSemi", "SEMI",   cyan);
        addKnob (b, "bFine", "FINE",   cyan);
        addKnob (b, "bLvl",  "LEVEL",  cyan);

        auto& u = addPanel ("UNISON", violet, 0, "mas voces = mas ancho y grueso");
        addKnob (u, "uniVoices", "VOICES", violet);
        addKnob (u, "uniDetune", "DETUNE", violet);
        addKnob (u, "uniSpread", "STEREO", violet);
    }

    // Fila 2: filtro, envolventes, master
    {
        auto& f = addPanel ("FILTER", cyan, 1, "oscurece o abre el sonido");
        addCombo (f, "fltType", "TYPE", cyan);
        addKnob (f, "cutoff", "CUTOFF", cyan);
        addKnob (f, "reso",   "RESO",   cyan);
        addKnob (f, "fltEnv", "ENV AMT", cyan);

        auto& fe = addPanel ("FILTER ENVELOPE", cyan, 1, "como se mueve el filtro");
        addKnob (fe, "fA", "ATTACK",  cyan);
        addKnob (fe, "fD", "DECAY",   cyan);
        addKnob (fe, "fS", "SUSTAIN", cyan);
        addKnob (fe, "fR", "RELEASE", cyan);

        auto& ae = addPanel ("AMP ENVELOPE", magenta, 1, "forma del volumen");
        addKnob (ae, "aA", "ATTACK",  magenta);
        addKnob (ae, "aD", "DECAY",   magenta);
        addKnob (ae, "aS", "SUSTAIN", magenta);
        addKnob (ae, "aR", "RELEASE", magenta);

        auto& m = addPanel ("OUTPUT", lime, 1, "volumen general");
        addKnob (m, "master", "MASTER", lime);
    }

    // Fila 3: LFO, pitch env, mezcla
    {
        auto& l = addPanel ("LFO", lime, 2, "movimiento automatico");
        addCombo (l, "lfoShape", "SHAPE",  lime);
        addCombo (l, "lfoDest",  "TARGET", lime);
        addKnob  (l, "lfoRate",  "RATE",   lime);
        addKnob  (l, "lfoDepth", "DEPTH",  lime);

        auto& pe = addPanel ("PITCH ENVELOPE", orange, 2, "caida o subida de tono");
        addKnob (pe, "pitEnv", "AMOUNT", orange);
        addKnob (pe, "pitDec", "TIME",   orange);

        auto& warp = addPanel ("WARP / GLIDE", orange, 2, "pliega la onda y desliza entre notas");
        addKnob (warp, "fold",  "FOLD",  orange);
        addKnob (warp, "glide", "GLIDE", orange);

        auto& mix = addPanel ("SUB / NOISE / PULSE", violet, 2, "grave extra, aire y ancho");
        addKnob (mix, "subLvl",   "SUB",   violet);
        addKnob (mix, "noiseLvl", "NOISE", violet);
        addKnob (mix, "pw",       "PULSE W", violet);
    }

    // Fila 4: efectos (cada uno con su interruptor)
    {
        auto& d = addPanel ("DRIVE", orange, 3, "distorsion");
        addKnob (d, "drive", "AMOUNT", orange);
        d.addPowerSwitch (processorRef.apvts, "drvOn");

        auto& c = addPanel ("CHORUS", cyan, 3, "ensancha");
        addKnob (c, "choRate",  "RATE",  cyan);
        addKnob (c, "choDepth", "DEPTH", cyan);
        addKnob (c, "choMix",   "MIX",   cyan);
        c.addPowerSwitch (processorRef.apvts, "choOn");

        auto& dl = addPanel ("DELAY", magenta, 3, "eco");
        addKnob (dl, "dlyTime", "TIME",     magenta);
        addKnob (dl, "dlyFb",   "FEEDBACK", magenta);
        addKnob (dl, "dlyMix",  "MIX",      magenta);
        dl.addPowerSwitch (processorRef.apvts, "dlyOn");

        auto& r = addPanel ("REVERB", violet, 3, "espacio");
        addKnob (r, "revSize", "SIZE", violet);
        addKnob (r, "revMix",  "MIX",  violet);
        r.addPowerSwitch (processorRef.apvts, "revOn");
    }

    syncBrowser();

    setResizeLimits (900, 640, 1600, 1400);
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
// WaveEditor
//------------------------------------------------------------------------------
WaveEditor::WaveEditor (PerreoTrapSynthAudioProcessor& p) : proc (p)
{
    for (int i = 0; i < Waves::numWaves; ++i)
        waveBox.addItem (Waves::waveName (i), i + 1);

    addAndMakeVisible (waveBox);

    for (auto* b : { &oscAButton, &oscBButton, &sineBtn, &sawBtn, &squareBtn, &triBtn,
                     &smoothBtn, &invertBtn, &randomBtn, &mirrorBtn })
        addAndMakeVisible (*b);

    oscAButton.onClick = [this] { selectOsc (0); };
    oscBButton.onClick = [this] { selectOsc (1); };

    auto fill = [this] (std::function<float (float)> f)
    {
        applyTool ([f] (std::array<float, Waves::tableSize>& t)
        {
            for (int i = 0; i < Waves::tableSize; ++i)
                t[(size_t) i] = f ((float) i / (float) Waves::tableSize);
        });
    };

    sineBtn.onClick   = [fill] { fill ([] (float ph) { return std::sin (6.2831853f * ph); }); };
    sawBtn.onClick    = [fill] { fill ([] (float ph) { return 2.0f * ph - 1.0f; }); };
    squareBtn.onClick = [fill] { fill ([] (float ph) { return ph < 0.5f ? 1.0f : -1.0f; }); };
    triBtn.onClick    = [fill] { fill ([] (float ph) { return 2.0f * std::abs (2.0f * ph - 1.0f) - 1.0f; }); };

    smoothBtn.onClick = [this]
    {
        applyTool ([] (std::array<float, Waves::tableSize>& t)
        {
            auto copy = t;

            for (int i = 0; i < Waves::tableSize; ++i)
            {
                const auto prev = copy[(size_t) ((i + Waves::tableSize - 1) % Waves::tableSize)];
                const auto next = copy[(size_t) ((i + 1) % Waves::tableSize)];
                t[(size_t) i] = 0.25f * prev + 0.5f * copy[(size_t) i] + 0.25f * next;
            }
        });
    };

    invertBtn.onClick = [this]
    {
        applyTool ([] (std::array<float, Waves::tableSize>& t) { for (auto& v : t) v = -v; });
    };

    mirrorBtn.onClick = [this]
    {
        applyTool ([] (std::array<float, Waves::tableSize>& t) { std::reverse (t.begin(), t.end()); });
    };

    randomBtn.onClick = [this]
    {
        applyTool ([] (std::array<float, Waves::tableSize>& t)
        {
            juce::Random rnd;
            constexpr int nodes = 12;
            std::array<float, nodes> pts {};

            for (auto& v : pts)
                v = rnd.nextFloat() * 2.0f - 1.0f;

            for (int i = 0; i < Waves::tableSize; ++i)
            {
                const float pos = (float) i / (float) Waves::tableSize * (float) nodes;
                const int a = (int) pos % nodes;
                const int b = (a + 1) % nodes;
                const float f = 0.5f - 0.5f * std::cos ((pos - std::floor (pos)) * juce::MathConstants<float>::pi);
                t[(size_t) i] = pts[(size_t) a] + (pts[(size_t) b] - pts[(size_t) a]) * f;
            }
        });
    };

    selectOsc (0);
    startTimerHz (15);
}

int WaveEditor::currentWave() const
{
    return juce::jlimit (0, Waves::numWaves - 1, (int) (proc.apvts.getRawParameterValue (waveParamId())->load() + 0.5f));
}

void WaveEditor::selectOsc (int newOsc)
{
    osc = newOsc;
    const auto accent = osc == 0 ? magenta : cyan;

    waveAttach.reset();
    waveAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (proc.apvts, waveParamId(), waveBox);

    oscAButton.setColour (juce::TextButton::buttonColourId, osc == 0 ? magenta.withAlpha (0.8f) : panelDeep);
    oscBButton.setColour (juce::TextButton::buttonColourId, osc == 1 ? cyan.withAlpha (0.8f) : panelDeep);
    waveBox.setColour (juce::ComboBox::outlineColourId, accent);
    repaint();
}

void WaveEditor::makeCustom()
{
    const int w = currentWave();

    if (w == Waves::customIndex)
        return;

    const float pw = proc.apvts.getRawParameterValue ("pw")->load();
    std::array<float, Waves::tableSize> t;

    for (int i = 0; i < Waves::tableSize; ++i)
        t[(size_t) i] = Waves::displaySample (w, (float) i / (float) Waves::tableSize, pw);

    proc.getWaveTable (osc).store (t);

    if (auto* prm = proc.apvts.getParameter (waveParamId()))
    {
        prm->beginChangeGesture();
        prm->setValueNotifyingHost (prm->convertTo0to1 ((float) Waves::customIndex));
        prm->endChangeGesture();
    }
}

void WaveEditor::applyTool (std::function<void (std::array<float, Waves::tableSize>&)> fn)
{
    makeCustom();

    std::array<float, Waves::tableSize> t;
    proc.getWaveTable (osc).load (t);
    fn (t);
    proc.getWaveTable (osc).store (t);
    repaint();
}

void WaveEditor::drawAt (juce::Point<int> pos)
{
    if (canvas.isEmpty())
        return;

    const float fx = juce::jlimit (0.0f, 0.9999f, (float) (pos.x - canvas.getX()) / (float) canvas.getWidth());
    const int idx = (int) (fx * (float) Waves::tableSize);
    const float val = juce::jlimit (-1.0f, 1.0f, 1.0f - 2.0f * (float) (pos.y - canvas.getY()) / (float) canvas.getHeight());

    makeCustom();

    std::array<float, Waves::tableSize> t;
    proc.getWaveTable (osc).load (t);

    if (lastIdx < 0 || lastIdx == idx)
    {
        t[(size_t) idx] = val;
    }
    else
    {
        const int lo = juce::jmin (lastIdx, idx);
        const int hi = juce::jmax (lastIdx, idx);

        for (int i = lo; i <= hi; ++i)
        {
            const float f = (float) (i - lastIdx) / (float) (idx - lastIdx);
            t[(size_t) i] = lastVal + (val - lastVal) * f;
        }
    }

    proc.getWaveTable (osc).store (t);
    lastIdx = idx;
    lastVal = val;
    repaint();
}

void WaveEditor::mouseDown (const juce::MouseEvent& e)
{
    lastIdx = -1;

    if (canvas.contains (e.getPosition()))
        drawAt (e.getPosition());
}

void WaveEditor::mouseDrag (const juce::MouseEvent& e)
{
    if (e.mouseWasDraggedSinceMouseDown())
        drawAt (e.getPosition());
}

void WaveEditor::resized()
{
    auto r = getLocalBounds().reduced (12, 10);

    auto left = r.removeFromLeft (150);
    auto row = left.removeFromTop (28);
    oscAButton.setBounds (row.removeFromLeft (70));
    oscBButton.setBounds (row.removeFromRight (70));
    left.removeFromTop (8);
    waveBox.setBounds (left.removeFromTop (28));

    auto right = r.removeFromRight (190);
    const int bw = (right.getWidth() - 6) / 2;
    const int bh = juce::jmin (34, (right.getHeight() - 18) / 4);
    juce::TextButton* tools[] = { &sineBtn, &sawBtn, &squareBtn, &triBtn, &smoothBtn, &invertBtn, &randomBtn, &mirrorBtn };

    for (int i = 0; i < 8; ++i)
        tools[i]->setBounds (right.getX() + (i % 2) * (bw + 6), right.getY() + (i / 2) * (bh + 6), bw, bh);

    canvas = r.reduced (12, 0);
}

void WaveEditor::paint (juce::Graphics& g)
{
    const auto accent = osc == 0 ? magenta : cyan;

    g.setColour (panel);
    g.fillRoundedRectangle (getLocalBounds().toFloat().reduced (1.5f), 10.0f);
    g.setColour (accent.withAlpha (0.35f));
    g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (1.5f), 10.0f, 1.2f);

    // Texto de ayuda (columna izquierda)
    g.setFont (boldFont (12.5f));
    g.setColour (juce::Colours::white);
    g.drawText ("WAVE EDITOR", 12, 12 + 28 + 8 + 28 + 10, 150, 18, juce::Justification::centredLeft, false);
    g.setFont (juce::Font (juce::FontOptions (11.0f)));
    g.setColour (textDim);
    g.drawFittedText ("Dibuja con el mouse sobre la onda. Al dibujar, la onda actual pasa a CUSTOM y se guarda con el proyecto y con tus presets.",
                      juce::Rectangle<int> (12, 12 + 28 + 8 + 28 + 32, 150, 84), juce::Justification::topLeft, 6, 0.9f);

    if (canvas.isEmpty())
        return;

    // Lienzo
    const auto cf = canvas.toFloat();
    g.setColour (panelDeep);
    g.fillRoundedRectangle (cf, 6.0f);

    g.setColour (juce::Colours::white.withAlpha (0.06f));
    for (int q = 1; q < 4; ++q)
        g.drawVerticalLine (canvas.getX() + canvas.getWidth() * q / 4, cf.getY(), cf.getBottom());

    g.drawHorizontalLine (canvas.getY() + canvas.getHeight() / 4, cf.getX(), cf.getRight());
    g.drawHorizontalLine (canvas.getY() + canvas.getHeight() * 3 / 4, cf.getX(), cf.getRight());
    g.setColour (juce::Colours::white.withAlpha (0.18f));
    g.drawHorizontalLine (canvas.getCentreY(), cf.getX(), cf.getRight());

    // Onda
    const int w = currentWave();
    const float pw = proc.apvts.getRawParameterValue ("pw")->load();
    std::array<float, Waves::tableSize> raw;
    proc.getWaveTable (osc).load (raw);

    juce::Path path;
    const int steps = juce::jmax (2, canvas.getWidth());

    for (int x = 0; x < steps; ++x)
    {
        const float ph = (float) x / (float) steps;
        const float v = (w == Waves::customIndex) ? raw[(size_t) juce::jmin (Waves::tableSize - 1, (int) (ph * (float) Waves::tableSize))]
                                                  : Waves::displaySample (w, ph, pw);
        const float px = cf.getX() + (float) x;
        const float py = cf.getY() + (1.0f - v) * 0.5f * cf.getHeight();

        if (x == 0) path.startNewSubPath (px, py);
        else        path.lineTo (px, py);
    }

    juce::Path fillPath (path);
    fillPath.lineTo (cf.getRight(), (float) canvas.getCentreY());
    fillPath.lineTo (cf.getX(), (float) canvas.getCentreY());
    fillPath.closeSubPath();

    g.saveState();
    g.reduceClipRegion (canvas);
    g.setColour (accent.withAlpha (0.14f));
    g.fillPath (fillPath);
    g.setColour (accent);
    g.strokePath (path, juce::PathStrokeType (2.2f));
    g.restoreState();

    g.setColour (accent.withAlpha (0.5f));
    g.drawRoundedRectangle (cf, 6.0f, 1.0f);

    g.setFont (juce::Font (juce::FontOptions (11.0f)));
    g.setColour (textDim);
    g.drawText (juce::String (Waves::waveName (w)) + (w == Waves::customIndex ? "   |   dibujando" : "   |   click y arrastra para editar"),
                canvas.reduced (8, 4), juce::Justification::topLeft, false);
}

//------------------------------------------------------------------------------
// PresetDisplay
//------------------------------------------------------------------------------
void PresetDisplay::setText (const juce::String& pathText, const juce::String& nameText)
{
    path = pathText;
    name = nameText;
    repaint();
}

void PresetDisplay::mouseUp (const juce::MouseEvent& e)
{
    if (e.mouseWasClicked() && getLocalBounds().contains (e.getPosition()) && onClick != nullptr)
        onClick();
}

void PresetDisplay::paint (juce::Graphics& g)
{
    const auto r = getLocalBounds().toFloat().reduced (1.0f);

    g.setColour (panelDeep.brighter (hover ? 0.12f : 0.0f));
    g.fillRoundedRectangle (r, 8.0f);
    g.setColour (cyan.withAlpha (hover ? 1.0f : 0.6f));
    g.drawRoundedRectangle (r, 8.0f, 1.3f);

    auto text = getLocalBounds().reduced (16, 5);
    text.removeFromRight (30);

    g.setFont (juce::Font (juce::FontOptions (11.5f)));
    g.setColour (textDim);
    g.drawFittedText (path, text.removeFromTop (14), juce::Justification::centredLeft, 1);

    g.setFont (boldFont (19.0f));
    g.setColour (juce::Colours::white);
    g.drawFittedText (name, text, juce::Justification::centredLeft, 1);

    // Flecha de desplegable
    const float cx = (float) getWidth() - 20.0f;
    const float cy = (float) getHeight() * 0.5f;
    juce::Path arrow;
    arrow.startNewSubPath (cx - 5.0f, cy - 3.0f);
    arrow.lineTo (cx, cy + 3.0f);
    arrow.lineTo (cx + 5.0f, cy - 3.0f);
    g.setColour (cyan);
    g.strokePath (arrow, juce::PathStrokeType (2.0f));
}

//------------------------------------------------------------------------------
// Navegador
//------------------------------------------------------------------------------
void PerreoTrapSynthAudioProcessorEditor::syncBrowser()
{
    const auto& libs = processorRef.getLibraries();
    const auto customName = processorRef.getCustomPresetName();
    const auto customLib  = processorRef.getCustomLibraryName();

    juce::String path, name;
    bool fromLibrary = false;

    if (customName.isNotEmpty())
    {
        for (const auto& lib : libs)
        {
            if (lib.name != customLib)
                continue;

            fromLibrary = true;
            path = lib.name.toUpperCase() + "  /  LIBRARY";

            for (const auto& p : lib.presets)
                if (p.name == customName && p.category.isNotEmpty())
                    path = lib.name.toUpperCase() + "  /  " + p.category;
        }

        name = customName;
    }

    if (! fromLibrary && customName.isNotEmpty())
    {
        fromLibrary = true;
        path = "SESSION";
        name = customName;
    }

    if (! fromLibrary)
    {
        using namespace PresetLayout;
        const int idx = processorRef.getCurrentProgram();
        path = juce::String (genreName (genreOf (idx))) + "  /  " + categoryName (categoryOf (idx));
        name = processorRef.getPresetShortName (idx);
    }

    if (path != shownPath || name != shownName)
    {
        shownPath = path;
        shownName = name;
        presetDisplay.setText (path, name);
    }
}

void PerreoTrapSynthAudioProcessorEditor::setWaveEditorOpen (bool open)
{
    if (waveEditor.isVisible() == open)
        return;

    waveEditor.setVisible (open);
    setSize (getWidth(), getHeight() + (open ? waveBandHeight : -waveBandHeight));
}

void PerreoTrapSynthAudioProcessorEditor::timerCallback()
{
    syncBrowser();

    for (auto& p : panels)
        p->refreshPowerLook();
}

void PerreoTrapSynthAudioProcessorEditor::showPresetMenu()
{
    using namespace PresetLayout;

    // IDs: fabrica 1..total | quitar libreria 9000+i | preset de libreria 10000 + lib*500 + n
    juce::PopupMenu root;
    const int current = processorRef.getCustomPresetName().isEmpty() ? processorRef.getCurrentProgram() : -1;

    root.addSectionHeader ("FACTORY");

    for (int g = 0; g < numGenres; ++g)
    {
        juce::PopupMenu genreMenu;

        for (int c = 0; c < numCategories; ++c)
        {
            juce::PopupMenu catMenu;

            for (int s = 0; s < perCategory; ++s)
            {
                const int idx = indexOf (g, c, s);
                catMenu.addItem (1 + idx, processorRef.getPresetShortName (idx), true, idx == current);
            }

            genreMenu.addSubMenu (categoryName (c), catMenu);
        }

        root.addSubMenu (genreName (g), genreMenu);
    }

    const auto& libs = processorRef.getLibraries();
    const auto customLib  = processorRef.getCustomLibraryName();
    const auto customName = processorRef.getCustomPresetName();

    root.addSeparator();
    root.addSectionHeader ("LIBRARIES");

    for (int li = 0; li < (int) libs.size(); ++li)
    {
        const auto& lib = libs[(size_t) li];
        juce::PopupMenu libMenu;

        if (lib.presets.empty())
        {
            libMenu.addItem (-1, "(empty - use SAVE to add sounds)", false, false);
        }
        else
        {
            // Agrupa por categoria si los presets la traen
            juce::StringArray cats;

            for (const auto& p : lib.presets)
                if (p.category.isNotEmpty())
                    cats.addIfNotAlreadyThere (p.category);

            auto isCurrent = [&] (const LibraryPreset& p) { return lib.name == customLib && p.name == customName; };

            for (const auto& cat : cats)
            {
                juce::PopupMenu catMenu;

                for (int pi = 0; pi < (int) lib.presets.size(); ++pi)
                    if (lib.presets[(size_t) pi].category == cat)
                        catMenu.addItem (10000 + li * 500 + pi, lib.presets[(size_t) pi].name, true,
                                         isCurrent (lib.presets[(size_t) pi]));

                libMenu.addSubMenu (cat, catMenu);
            }

            for (int pi = 0; pi < (int) lib.presets.size(); ++pi)
                if (lib.presets[(size_t) pi].category.isEmpty())
                    libMenu.addItem (10000 + li * 500 + pi, lib.presets[(size_t) pi].name, true,
                                     isCurrent (lib.presets[(size_t) pi]));
        }

        if (li > 0)
        {
            libMenu.addSeparator();
            libMenu.addItem (9000 + li, "Remove this library");
        }

        root.addSubMenu (lib.name + "  (" + juce::String ((int) lib.presets.size()) + ")", libMenu);
    }

    juce::Component::SafePointer<PerreoTrapSynthAudioProcessorEditor> safeThis (this);

    root.showMenuAsync (juce::PopupMenu::Options()
                            .withTargetComponent (&presetDisplay)
                            .withMinimumWidth (presetDisplay.getWidth()),
                        [safeThis] (int result)
    {
        if (safeThis == nullptr || result <= 0)
            return;

        auto& proc = safeThis->processorRef;

        if (result <= PresetLayout::total)
            proc.setCurrentProgram (result - 1);
        else if (result >= 10000)
            proc.loadLibraryPreset ((result - 10000) / 500, (result - 10000) % 500);
        else if (result >= 9000)
            proc.removeLibrary (result - 9000);

        safeThis->syncBrowser();
    });
}

void PerreoTrapSynthAudioProcessorEditor::stepPreset (int direction)
{
    const auto& libs = processorRef.getLibraries();
    const auto customName = processorRef.getCustomPresetName();
    const auto customLib  = processorRef.getCustomLibraryName();

    if (customName.isNotEmpty())
    {
        for (int li = 0; li < (int) libs.size(); ++li)
        {
            if (libs[(size_t) li].name != customLib)
                continue;

            const int n = (int) libs[(size_t) li].presets.size();

            for (int pi = 0; pi < n; ++pi)
            {
                if (libs[(size_t) li].presets[(size_t) pi].name == customName)
                {
                    processorRef.loadLibraryPreset (li, (pi + direction + n) % n);
                    syncBrowser();
                    return;
                }
            }
        }
    }

    const int n = PerreoTrapSynthAudioProcessor::numPresets;
    processorRef.setCurrentProgram ((processorRef.getCurrentProgram() + direction + n) % n);
    syncBrowser();
}

//------------------------------------------------------------------------------
// Guardar el sonido actual en USER
//------------------------------------------------------------------------------
void PerreoTrapSynthAudioProcessorEditor::saveToUser()
{
    auto* aw = new juce::AlertWindow ("Save sound",
                                      "Se guarda en la libreria USER. Despues puedes exportarla como pack.",
                                      juce::MessageBoxIconType::NoIcon, this);

    const auto suggested = processorRef.getCustomPresetName().isNotEmpty()
                               ? processorRef.getCustomPresetName()
                               : processorRef.getPresetShortName (processorRef.getCurrentProgram());

    aw->addTextEditor ("name", suggested, "Name");

    juce::StringArray cats { "(no category)" };

    for (int c = 0; c < PresetLayout::numCategories; ++c)
        cats.add (PresetLayout::categoryName (c));

    aw->addComboBox ("cat", cats, "Category");
    aw->getComboBoxComponent ("cat")->setSelectedItemIndex (0, juce::dontSendNotification);

    aw->addButton ("Save",   1, juce::KeyPress (juce::KeyPress::returnKey));
    aw->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey));

    juce::Component::SafePointer<PerreoTrapSynthAudioProcessorEditor> safeThis (this);

    aw->enterModalState (true, juce::ModalCallbackFunction::create ([safeThis, aw] (int result)
    {
        if (result != 1 || safeThis == nullptr)
            return;

        const int catIndex = aw->getComboBoxComponent ("cat")->getSelectedItemIndex();
        const juce::String cat = catIndex > 0 ? juce::String (PresetLayout::categoryName (catIndex - 1)) : juce::String();

        safeThis->processorRef.saveCurrentSoundToUser (aw->getTextEditorContents ("name"), cat);
        safeThis->syncBrowser();
    }), true);
}

//------------------------------------------------------------------------------
// Packs
//------------------------------------------------------------------------------
void PerreoTrapSynthAudioProcessorEditor::importPack()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Import pack",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
        "*.ptspack;*.ptspreset");

    juce::Component::SafePointer<PerreoTrapSynthAudioProcessorEditor> safeThis (this);

    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [safeThis] (const juce::FileChooser& chooser)
    {
        if (safeThis == nullptr)
            return;

        const auto file = chooser.getResult();

        if (! file.existsAsFile())
            return;

        juce::String packName;
        const int n = safeThis->processorRef.importPack (file, packName);

        if (n == 0)
        {
            juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon, "Import pack",
                                                    "El archivo no es un pack valido de PerreoTrapSynth.");
            return;
        }

        juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::InfoIcon, "Import pack",
                                                "Libreria \"" + packName + "\" importada (" + juce::String (n)
                                                    + " presets). La encuentras en el menu de presets, seccion LIBRARIES.");
        safeThis->syncBrowser();
    });
}

void PerreoTrapSynthAudioProcessorEditor::exportPackMenu()
{
    juce::PopupMenu menu;
    menu.addSectionHeader ("EXPORT AS PACK");

    const auto& libs = processorRef.getLibraries();

    for (int li = 0; li < (int) libs.size(); ++li)
        menu.addItem (100 + li, libs[(size_t) li].name + "  (" + juce::String ((int) libs[(size_t) li].presets.size()) + ")",
                      ! libs[(size_t) li].presets.empty());

    menu.addSeparator();
    menu.addItem (10, "Factory - TRAP");
    menu.addItem (11, "Factory - REGGAETON");
    menu.addItem (12, "Factory - ALL");

    juce::Component::SafePointer<PerreoTrapSynthAudioProcessorEditor> safeThis (this);

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&exportButton),
                        [safeThis] (int result)
    {
        if (safeThis == nullptr || result <= 0)
            return;

        auto& proc = safeThis->processorRef;

        if (result >= 100)
        {
            const int li = result - 100;
            safeThis->doExport (proc.getLibraries()[(size_t) li].name, proc.collectLibrary (li));
        }
        else if (result == 10) safeThis->doExport ("Factory TRAP",      proc.collectFactory (0));
        else if (result == 11) safeThis->doExport ("Factory REGGAETON", proc.collectFactory (1));
        else if (result == 12) safeThis->doExport ("Factory ALL",       proc.collectFactory (-1));
    });
}

void PerreoTrapSynthAudioProcessorEditor::doExport (const juce::String& packName, std::vector<LibraryPreset> presets)
{
    const auto defaultFile = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                                 .getChildFile (juce::File::createLegalFileName (packName) + ".ptspack");

    fileChooser = std::make_unique<juce::FileChooser> ("Export pack", defaultFile, "*.ptspack");

    juce::Component::SafePointer<PerreoTrapSynthAudioProcessorEditor> safeThis (this);

    fileChooser->launchAsync (juce::FileBrowserComponent::saveMode
                                  | juce::FileBrowserComponent::canSelectFiles
                                  | juce::FileBrowserComponent::warnAboutOverwriting,
                              [safeThis, packName, presets = std::move (presets)] (const juce::FileChooser& chooser)
    {
        if (safeThis == nullptr)
            return;

        auto file = chooser.getResult();

        if (file == juce::File())
            return;

        if (! file.hasFileExtension ("ptspack"))
            file = juce::File (file.getFullPathName() + ".ptspack");

        if (! safeThis->processorRef.exportPack (file, packName, presets))
            juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon, "Export pack",
                                                    "No se pudo guardar el pack.");
    });
}

//------------------------------------------------------------------------------
void PerreoTrapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    const float w = (float) getWidth();
    const float h = (float) getHeight();

    // Fondo liso
    g.setGradientFill (juce::ColourGradient (bgTop, 0.0f, 0.0f, bgBottom, 0.0f, h, false));
    g.fillAll();

    // Linea de acento superior
    g.setColour (magenta);
    g.fillRect (0.0f, 0.0f, w, 2.0f);

    // Titulo
    g.setFont (boldFont (24.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("PERREO TRAP", 18, 6, 190, 34, juce::Justification::centredLeft, false);
    g.setColour (cyan);
    g.drawText ("SYNTH", 18 + 164, 6, 120, 34, juce::Justification::centredLeft, false);

    g.setFont (juce::Font (juce::FontOptions (11.5f)));
    g.setColour (textDim);
    g.drawText (juce::String (PresetLayout::total) + " factory presets   |   click the preset name to browse   |   WAVES = draw your own waveforms",
                getLocalBounds().withTrimmedRight (18).withHeight (44),
                juce::Justification::centredRight, false);

    // Caja del navegador
    if (! browserBounds.isEmpty())
    {
        const auto r = browserBounds.toFloat().reduced (1.5f);
        g.setColour (panel);
        g.fillRoundedRectangle (r, 10.0f);
        g.setColour (cyan.withAlpha (0.35f));
        g.drawRoundedRectangle (r, 10.0f, 1.2f);
    }
}

void PerreoTrapSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (14, 8);
    area.removeFromTop (38); // titulo

    //--------------------------------------------------------------------------
    // Navegador:  [<] [ preset actual (click = menu) ] [>]   SAVE | IMPORT | EXPORT
    //--------------------------------------------------------------------------
    browserBounds = area.removeFromTop (60);
    auto row = browserBounds.reduced (10, 8);

    exportButton.setBounds (row.removeFromRight (104));
    row.removeFromRight (6);
    importButton.setBounds (row.removeFromRight (104));
    row.removeFromRight (6);
    saveButton.setBounds (row.removeFromRight (60));
    row.removeFromRight (14);
    randomButton.setBounds (row.removeFromRight (70));
    row.removeFromRight (6);
    wavesButton.setBounds (row.removeFromRight (66));
    row.removeFromRight (14);

    prevButton.setBounds (row.removeFromLeft (36));
    row.removeFromLeft (6);
    nextButton.setBounds (row.removeFromRight (36));
    row.removeFromRight (6);
    presetDisplay.setBounds (row);

    //--------------------------------------------------------------------------
    // Filas de paneles
    //--------------------------------------------------------------------------
    area.removeFromTop (8);

    if (waveEditor.isVisible())
    {
        waveEditor.setBounds (area.removeFromTop (waveBandHeight - 8));
        area.removeFromTop (8);
    }

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
