#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// SynthVoice
//==============================================================================
SynthVoice::SynthVoice (juce::AudioProcessorValueTreeState& apvts)
{
    waveParam      = apvts.getRawParameterValue ("oscWave");
    cutoffParam    = apvts.getRawParameterValue ("cutoff");
    resonanceParam = apvts.getRawParameterValue ("resonance");
    driveParam     = apvts.getRawParameterValue ("drive");

    adsr.setParameters (juce::ADSR::Parameters (0.005f, 0.25f, 0.75f, 0.30f));
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate (newRate);

    if (newRate > 0.0)
        sampleRateHz = newRate;

    adsr.setSampleRate (sampleRateHz);
    adsr.setParameters (juce::ADSR::Parameters (0.005f, 0.25f, 0.75f, 0.30f));

    cutoffSmoothed.reset (sampleRateHz, 0.02);
    driveSmoothed.reset (sampleRateHz, 0.02);
}

void SynthVoice::updatePitchBend (int pitchWheelPosition) noexcept
{
    const float semitones = ((float) pitchWheelPosition - 8192.0f) / 8192.0f * 2.0f;
    bendRatio = std::pow (2.0f, semitones / 12.0f);
}

void SynthVoice::startNote (int midiNoteNumber, float velocity,
                            juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    baseFrequency = (float) juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    updatePitchBend (currentPitchWheelPosition);
    level = velocity;

    phase1 = 0.0f;
    phase2 = 0.0f;
    ic1eq  = 0.0f;
    ic2eq  = 0.0f;

    cutoffSmoothed.setCurrentAndTargetValue (juce::jlimit (20.0f, 20000.0f, cutoffParam->load()));
    driveSmoothed.setCurrentAndTargetValue (driveParam->load());

    adsr.noteOn();
}

void SynthVoice::stopNote (float, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        adsr.reset();
        clearCurrentNote();
    }
}

void SynthVoice::pitchWheelMoved (int newPitchWheelValue)
{
    updatePitchBend (newPitchWheelValue);
}

void SynthVoice::controllerMoved (int, int)
{
}

float SynthVoice::polyBlep (float t, float dt) noexcept
{
    if (t < dt)
    {
        t /= dt;
        return t + t - t * t - 1.0f;
    }

    if (t > 1.0f - dt)
    {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }

    return 0.0f;
}

float SynthVoice::generateSample (int wave, float phase, float dt) noexcept
{
    switch (wave)
    {
        case 0: // Sine
            return std::sin (juce::MathConstants<float>::twoPi * phase);

        case 1: // Saw (polyBLEP)
            return (2.0f * phase - 1.0f) - polyBlep (phase, dt);

        case 2: // Square (polyBLEP)
        {
            float shifted = phase + 0.5f;
            if (shifted >= 1.0f)
                shifted -= 1.0f;

            const float naive = phase < 0.5f ? 1.0f : -1.0f;
            return naive + polyBlep (phase, dt) - polyBlep (shifted, dt);
        }

        case 3: // Triangle
        default:
            return 2.0f * std::abs (2.0f * phase - 1.0f) - 1.0f;
    }
}

void SynthVoice::updateFilterCoefficients (float cutoffHz) noexcept
{
    const float maxCutoff = (float) (sampleRateHz * 0.45);
    const float fc = juce::jlimit (20.0f, juce::jmax (21.0f, maxCutoff), cutoffHz);

    g  = std::tan (juce::MathConstants<float>::pi * fc / (float) sampleRateHz);
    a1 = 1.0f / (1.0f + g * (g + k));
    a2 = g * a1;
    a3 = g * a2;
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    const int wave = juce::jlimit (0, 3, (int) (waveParam->load() + 0.5f));

    const float q = juce::jlimit (0.1f, 10.0f, resonanceParam->load());
    k = 1.0f / q;

    cutoffSmoothed.setTargetValue (juce::jlimit (20.0f, 20000.0f, cutoffParam->load()));
    driveSmoothed.setTargetValue (juce::jlimit (0.0f, 10.0f, driveParam->load()));

    const float freq = baseFrequency * bendRatio;
    const float dt1  = juce::jmin (0.45f, freq / (float) sampleRateHz);
    const float dt2  = juce::jmin (0.45f, dt1 * (wave == 0 ? 1.0f : 1.006f));
    const float mix2 = (wave == 0) ? 0.0f : 0.5f;
    const float mix1 = 1.0f - mix2;

    const int numChannels = outputBuffer.getNumChannels();
    unsigned int coefCounter = 0;

    for (int i = 0; i < numSamples; ++i)
    {
        if (! adsr.isActive())
        {
            clearCurrentNote();
            break;
        }

        const float fc = cutoffSmoothed.getNextValue();

        if ((coefCounter++ & 15u) == 0u)
            updateFilterCoefficients (fc);

        // Osciladores
        const float o1 = generateSample (wave, phase1, dt1);
        const float o2 = generateSample (wave, phase2, dt2);

        phase1 += dt1;
        if (phase1 >= 1.0f) phase1 -= 1.0f;

        phase2 += dt2;
        if (phase2 >= 1.0f) phase2 -= 1.0f;

        const float osc = o1 * mix1 + o2 * mix2;

        // Filtro SVF pasa-bajos
        const float v3 = osc - ic2eq;
        const float v1 = a1 * ic1eq + a2 * v3;
        const float v2 = ic2eq + a2 * ic1eq + a3 * v3;
        ic1eq = 2.0f * v1 - ic1eq;
        ic2eq = 2.0f * v2 - ic2eq;

        float s = v2;

        // Drive (saturacion tanh normalizada)
        const float d   = driveSmoothed.getNextValue();
        const float pre = 1.0f + d * 1.6f;
        s = std::tanh (s * pre) / std::tanh (pre);

        // Envolvente, velocidad y headroom
        s *= adsr.getNextSample() * level * 0.30f;

        for (int ch = 0; ch < numChannels; ++ch)
            outputBuffer.addSample (ch, startSample + i, s);
    }
}

//==============================================================================
// Tabla de presets
//==============================================================================
const std::array<PerreoPreset, PerreoTrapSynthAudioProcessor::numPresets>&
PerreoTrapSynthAudioProcessor::getPresetTable()
{
    // wave: 0 Sine, 1 Saw, 2 Square, 3 Triangle
    static const std::array<PerreoPreset, numPresets> table {{
        { "[Default] Perreo Init Synth (Editable)",   1, 8000.0f, 0.707f, 0.5f, 0.15f, 0.70f },
        { "[Lead] Dembow Hyper-Lead",                 1, 6500.0f, 2.5f,   3.0f, 0.20f, 0.70f },
        { "[Pluck] Reggaeton Cyber-Pluck",            2, 2200.0f, 4.5f,   1.5f, 0.25f, 0.70f },
        { "[Keys] Melodic Trap Electric Piano",       3, 3500.0f, 1.2f,   0.8f, 0.30f, 0.75f },
        { "[Synth] West Coast Glide Synth",           1, 2800.0f, 5.5f,   2.0f, 0.15f, 0.70f },
        { "[Pad] Atmospheric Miami Pad",              1, 1800.0f, 1.0f,   0.0f, 0.65f, 0.65f },
        { "[Choir] Epic Reggaeton Choir",             3, 1400.0f, 2.0f,   0.3f, 0.70f, 0.70f },
        { "[Orchestral] Electro Flow Strings",        1, 4200.0f, 0.9f,   0.4f, 0.45f, 0.70f },
        { "[Orchestral] Dark Trap Brass & Strings",   1, 1500.0f, 3.0f,   2.5f, 0.40f, 0.70f },
        { "[Sub] 808 Heavy Distortion",               0,  600.0f, 0.707f, 8.0f, 0.05f, 0.70f },

        // ---- Presets extra (10 en adelante) ----
        // Lead
        { "[Lead] Neon Perreo Lead",                  1, 7500.0f, 1.8f,   2.0f, 0.25f, 0.70f },
        { "[Lead] Sunset Trap Lead",                  3, 5200.0f, 1.0f,   1.2f, 0.35f, 0.72f },
        { "[Lead] Hard Reggaeton Screamer",           2, 9000.0f, 3.5f,   6.0f, 0.15f, 0.62f },
        { "[Lead] Club Siren Lead",                   1, 10000.0f, 6.0f,  4.0f, 0.30f, 0.62f },
        { "[Lead] Romantico Soft Lead",               0, 4500.0f, 0.8f,   0.2f, 0.40f, 0.75f },
        // Pluck
        { "[Pluck] Bubble Pluck",                     0, 2600.0f, 3.0f,   1.0f, 0.20f, 0.75f },
        { "[Pluck] Glass Marimba Pluck",              3, 3800.0f, 2.2f,   0.5f, 0.30f, 0.75f },
        { "[Pluck] Latin Trap Pluck",                 2, 1800.0f, 5.0f,   2.5f, 0.20f, 0.68f },
        { "[Pluck] Digital Kalimba",                  0, 5200.0f, 1.5f,   0.3f, 0.35f, 0.75f },
        { "[Pluck] Dark Harp Pluck",                  3, 1600.0f, 2.8f,   1.5f, 0.45f, 0.72f },
        // Keys
        { "[Keys] Rhodes Nocturno",                   3, 2400.0f, 0.9f,   1.0f, 0.40f, 0.75f },
        { "[Keys] Bell Keys",                         0, 6500.0f, 2.0f,   0.2f, 0.50f, 0.72f },
        { "[Keys] Lo-Fi Perreo Keys",                 3, 1500.0f, 1.5f,   1.8f, 0.30f, 0.75f },
        { "[Keys] Crystal Celesta",                   0, 8000.0f, 1.2f,   0.0f, 0.55f, 0.72f },
        // Synth
        { "[Synth] Retro Wave Synth",                 1, 3600.0f, 2.5f,   1.5f, 0.30f, 0.70f },
        { "[Synth] Hyperpop Synth",                   2, 12000.0f, 1.5f,  3.5f, 0.20f, 0.65f },
        { "[Synth] Future Funk Synth",                1, 2200.0f, 4.0f,   1.2f, 0.25f, 0.70f },
        { "[Synth] Acid Perreo Synth",                1, 1200.0f, 8.0f,   3.0f, 0.10f, 0.62f },
        // Pad
        { "[Pad] Night Drive Pad",                    1, 1300.0f, 1.4f,   0.2f, 0.70f, 0.65f },
        { "[Pad] Warm Analog Pad",                    3, 2000.0f, 0.9f,   0.5f, 0.55f, 0.70f },
        { "[Pad] Glass Cathedral Pad",                0, 3000.0f, 1.0f,   0.0f, 0.90f, 0.65f },
        { "[Pad] Dark Ambient Pad",                   1,  800.0f, 1.8f,   0.8f, 0.80f, 0.62f },
        // Choir
        { "[Choir] Angelic Ooh Choir",                0, 2800.0f, 1.0f,   0.0f, 0.80f, 0.70f },
        { "[Choir] Dark Gregorian Choir",             3, 1000.0f, 2.5f,   0.6f, 0.85f, 0.68f },
        // Orchestral
        { "[Orchestral] Cinematic Strings",           1, 3200.0f, 0.8f,   0.2f, 0.60f, 0.70f },
        { "[Orchestral] Epic Brass Hit",              1, 2600.0f, 2.0f,   1.8f, 0.35f, 0.65f },
        { "[Orchestral] Pizzicato Perreo Strings",    3, 4000.0f, 1.6f,   0.4f, 0.25f, 0.75f },
        { "[Orchestral] Horror Strings",              1, 1800.0f, 6.0f,   2.0f, 0.70f, 0.60f },
        // Sub / Bass
        { "[Sub] Clean 808 Sub",                      0,  400.0f, 0.707f, 0.0f, 0.00f, 0.80f },
        { "[Sub] Long 808 Glide Sub",                 0,  500.0f, 1.0f,   3.0f, 0.05f, 0.75f },
        { "[Sub] Dembow Sub Bass",                    3,  300.0f, 1.0f,   1.5f, 0.00f, 0.78f },
        { "[Sub] Reese Trap Bass",                    1,  700.0f, 3.0f,   4.0f, 0.05f, 0.65f },
        { "[Sub] Growl Bass Square",                  2,  900.0f, 4.5f,   6.5f, 0.05f, 0.60f }
    }};

    return table;
}

//==============================================================================
// Layout de parametros
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
PerreoTrapSynthAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "oscWave", 1 },
        "Oscillator Wave",
        juce::StringArray { "Sine", "Saw", "Square", "Triangle" },
        1));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "cutoff", 1 },
        "Cutoff",
        juce::NormalisableRange<float> (20.0f, 20000.0f, 0.01f, 0.3f),
        8000.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("Hz")
            .withStringFromValueFunction ([] (float value, int)
            {
                if (value >= 1000.0f)
                    return juce::String (value / 1000.0f, 2) + " kHz";

                return juce::String (juce::roundToInt (value)) + " Hz";
            })
            .withValueFromStringFunction ([] (const juce::String& text)
            {
                const float v = text.getFloatValue();
                return text.containsIgnoreCase ("k") ? v * 1000.0f : v;
            })));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "resonance", 1 },
        "Resonance",
        juce::NormalisableRange<float> (0.1f, 10.0f, 0.001f, 0.5f),
        0.707f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "drive", 1 },
        "Drive",
        juce::NormalisableRange<float> (0.0f, 10.0f, 0.01f),
        0.5f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverb", 1 },
        "Reverb",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
        0.15f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "masterVol", 1 },
        "Master Vol",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
        0.7f));

    return layout;
}

//==============================================================================
// Procesador
//==============================================================================
PerreoTrapSynthAudioProcessor::PerreoTrapSynthAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    reverbParam = apvts.getRawParameterValue ("reverb");
    masterParam = apvts.getRawParameterValue ("masterVol");

    synth.clearVoices();
    for (int i = 0; i < 8; ++i)
        synth.addVoice (new SynthVoice (apvts));

    synth.clearSounds();
    synth.addSound (new SynthSound());
    synth.setNoteStealingEnabled (true);

    lastMaster = masterParam->load();
}

PerreoTrapSynthAudioProcessor::~PerreoTrapSynthAudioProcessor()
{
}

void PerreoTrapSynthAudioProcessor::prepareToPlay (double sampleRate, int)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);

    reverb.setSampleRate (sampleRate);
    reverb.reset();

    lastMaster = masterParam->load();
}

void PerreoTrapSynthAudioProcessor::releaseResources()
{
}

bool PerreoTrapSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::stereo() || out == juce::AudioChannelSet::mono();
}

void PerreoTrapSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    buffer.clear();

    synth.renderNextBlock (buffer, midiMessages, 0, numSamples);

    // Reverb
    const float rev = juce::jlimit (0.0f, 1.0f, reverbParam->load());
    reverbParams.roomSize   = 0.35f + 0.60f * rev;
    reverbParams.damping    = 0.45f;
    reverbParams.wetLevel   = 0.45f * rev;
    reverbParams.dryLevel   = 1.0f - 0.35f * rev;
    reverbParams.width      = 1.0f;
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters (reverbParams);

    if (buffer.getNumChannels() >= 2)
        reverb.processStereo (buffer.getWritePointer (0), buffer.getWritePointer (1), numSamples);
    else if (buffer.getNumChannels() == 1)
        reverb.processMono (buffer.getWritePointer (0), numSamples);

    // Volumen maestro con rampa para evitar clicks
    const float master = juce::jlimit (0.0f, 1.0f, masterParam->load());
    buffer.applyGainRamp (0, numSamples, lastMaster, master);
    lastMaster = master;

    // Limitador de seguridad
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* data = buffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
            data[i] = juce::jlimit (-1.0f, 1.0f, data[i]);
    }
}

juce::AudioProcessorEditor* PerreoTrapSynthAudioProcessor::createEditor()
{
    return new PerreoTrapSynthAudioProcessorEditor (*this);
}

//==============================================================================
// Presets
//==============================================================================
void PerreoTrapSynthAudioProcessor::setParameterValue (const juce::String& paramID, float realValue)
{
    if (auto* param = apvts.getParameter (paramID))
    {
        const float normalised = param->convertTo0to1 (realValue);

        param->beginChangeGesture();
        param->setValueNotifyingHost (normalised);
        param->endChangeGesture();
    }
}

void PerreoTrapSynthAudioProcessor::applyPreset (int index)
{
    index = juce::jlimit (0, numPresets - 1, index);
    currentPreset.store (index);

    const auto& preset = getPresetTable()[(size_t) index];

    setParameterValue ("oscWave",   (float) preset.wave);
    setParameterValue ("cutoff",    preset.cutoff);
    setParameterValue ("resonance", preset.resonance);
    setParameterValue ("drive",     preset.drive);
    setParameterValue ("reverb",    preset.reverb);
    setParameterValue ("masterVol", preset.master);
}

void PerreoTrapSynthAudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= numPresets)
        return;

    applyPreset (index);
}

const juce::String PerreoTrapSynthAudioProcessor::getProgramName (int index)
{
    if (index < 0 || index >= numPresets)
        return {};

    return juce::String (getPresetTable()[(size_t) index].name);
}

void PerreoTrapSynthAudioProcessor::changeProgramName (int, const juce::String&)
{
}

//==============================================================================
// Estado
//==============================================================================
void PerreoTrapSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty ("presetIndex", currentPreset.load(), nullptr);

    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void PerreoTrapSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (apvts.state.getType()))
        {
            auto tree = juce::ValueTree::fromXml (*xml);
            currentPreset.store (juce::jlimit (0, numPresets - 1, (int) tree.getProperty ("presetIndex", 0)));
            apvts.replaceState (tree);
        }
    }
}

//==============================================================================
// Punto de entrada del plugin
//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PerreoTrapSynthAudioProcessor();
}
