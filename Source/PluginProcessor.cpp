#include "PluginProcessor.h"
#include <algorithm>
#include "PluginEditor.h"

//==============================================================================
// SynthParams
//==============================================================================
SynthParams::SynthParams (juce::AudioProcessorValueTreeState& s)
{
    auto get = [&s] (const char* id) { return s.getRawParameterValue (id); };

    aWave = get ("aWave"); aOct = get ("aOct"); aSemi = get ("aSemi"); aFine = get ("aFine"); aLvl = get ("aLvl");
    bWave = get ("bWave"); bOct = get ("bOct"); bSemi = get ("bSemi"); bFine = get ("bFine"); bLvl = get ("bLvl");
    uniVoices = get ("uniVoices"); uniDetune = get ("uniDetune"); uniSpread = get ("uniSpread");
    pw = get ("pw"); subLvl = get ("subLvl"); noiseLvl = get ("noiseLvl");
    fltType = get ("fltType"); cutoff = get ("cutoff"); reso = get ("reso"); fltEnv = get ("fltEnv");
    fA = get ("fA"); fD = get ("fD"); fS = get ("fS"); fR = get ("fR");
    aA = get ("aA"); aD = get ("aD"); aS = get ("aS"); aR = get ("aR");
    lfoShape = get ("lfoShape"); lfoDest = get ("lfoDest"); lfoRate = get ("lfoRate"); lfoDepth = get ("lfoDepth");
    pitEnv = get ("pitEnv"); pitDec = get ("pitDec");
    glide = get ("glide"); fold = get ("fold");
}

//==============================================================================
// SynthVoice
//==============================================================================
SynthVoice::SynthVoice (const SynthParams& params) : p (params)
{
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

    ampEnv.setSampleRate (sampleRateHz);
    fltEnvGen.setSampleRate (sampleRateHz);
    cutoffSmoothed.reset (sampleRateHz, 0.02);
}

void SynthVoice::updatePitchBend (int pitchWheelPosition) noexcept
{
    const float semitones = ((float) pitchWheelPosition - 8192.0f) / 8192.0f * 2.0f;
    bendRatio = std::exp2 (semitones / 12.0f);
}

void SynthVoice::startNote (int midiNoteNumber, float velocity,
                            juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    baseFrequency = (float) juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);

    // Glide: arranca desde la ultima nota tocada
    const float lastHz = p.lastNoteHz != nullptr ? p.lastNoteHz->load() : 0.0f;
    currentFreq = (p.glide->load() > 0.001f && lastHz > 0.0f) ? lastHz : baseFrequency;

    if (p.lastNoteHz != nullptr)
        p.lastNoteHz->store (baseFrequency);
    updatePitchBend (currentPitchWheelPosition);
    velocityGain = 0.25f + 0.75f * velocity;

    for (int i = 0; i < maxUnison; ++i)
    {
        phaseA[(size_t) i] = std::fmod ((float) i * 0.173f, 1.0f);
        phaseB[(size_t) i] = std::fmod ((float) i * 0.211f + 0.07f, 1.0f);
    }

    subPhase = 0.0f;
    lfoPhase = 0.0f;
    lfoHold  = random.nextFloat() * 2.0f - 1.0f;
    pitchEnvValue = 1.0f;

    filtL.reset();
    filtR.reset();

    cutoffSmoothed.setCurrentAndTargetValue (juce::jlimit (20.0f, 20000.0f, p.cutoff->load()));

    ampEnv.setParameters (juce::ADSR::Parameters (p.aA->load(), p.aD->load(), p.aS->load(), p.aR->load()));
    fltEnvGen.setParameters (juce::ADSR::Parameters (p.fA->load(), p.fD->load(), p.fS->load(), p.fR->load()));
    ampEnv.noteOn();
    fltEnvGen.noteOn();
}

void SynthVoice::stopNote (float, bool allowTailOff)
{
    if (allowTailOff)
    {
        ampEnv.noteOff();
        fltEnvGen.noteOff();
    }
    else
    {
        ampEnv.reset();
        fltEnvGen.reset();
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

float SynthVoice::wavefold (float x, float amount) noexcept
{
    const float k = 1.0f + amount * 5.0f;
    return x + amount * (std::sin (k * x * 1.5708f) - x);
}

float SynthVoice::oscSample (int wave, float phase, float dt, float pulseWidth, const float* custom) noexcept
{
    if (wave >= Waves::firstTable)
        return wave == Waves::customIndex ? Waves::readTable (custom, Waves::tableSize, phase)
                                          : Waves::sampleBuiltIn (wave, phase);

    switch (wave)
    {
        case 0: // Sine
            return std::sin (juce::MathConstants<float>::twoPi * phase);

        case 1: // Saw (polyBLEP)
            return (2.0f * phase - 1.0f) - polyBlep (phase, dt);

        case 2: // Square (polyBLEP)
        case 4: // Pulse (polyBLEP, ancho variable)
        {
            const float width = (wave == 2) ? 0.5f : pulseWidth;

            float shifted = phase - width;
            if (shifted < 0.0f)
                shifted += 1.0f;

            const float naive = phase < width ? 1.0f : -1.0f;
            const float v = naive + polyBlep (phase, dt) - polyBlep (shifted, dt);
            return v - (2.0f * width - 1.0f); // quita el DC del pulso
        }

        case 3: // Triangle
        default:
            return 2.0f * std::abs (2.0f * phase - 1.0f) - 1.0f;
    }
}

float SynthVoice::lfoSample (int shape, float increment) noexcept
{
    float out = 0.0f;

    switch (shape)
    {
        case 0:  out = std::sin (juce::MathConstants<float>::twoPi * lfoPhase); break;
        case 1:  out = 4.0f * std::abs (lfoPhase - 0.5f) - 1.0f; break;
        case 2:  out = 2.0f * lfoPhase - 1.0f; break;
        case 3:  out = lfoPhase < 0.5f ? 1.0f : -1.0f; break;
        default: out = lfoHold; break;
    }

    lfoPhase += increment;

    if (lfoPhase >= 1.0f)
    {
        lfoPhase -= 1.0f;
        lfoHold = random.nextFloat() * 2.0f - 1.0f;
    }

    return out;
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

float SynthVoice::filterProcess (Svf& s, float in, int type) const noexcept
{
    const float v3 = in - s.ic2;
    const float v1 = a1 * s.ic1 + a2 * v3;
    const float v2 = s.ic2 + a2 * s.ic1 + a3 * v3;
    s.ic1 = 2.0f * v1 - s.ic1;
    s.ic2 = 2.0f * v2 - s.ic2;

    switch (type)
    {
        case 1:  return in - k * v1 - v2; // High-pass
        case 2:  return k * v1;           // Band-pass (pico normalizado)
        case 3:  return in - k * v1;      // Notch
        default: return v2;               // Low-pass
    }
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    const double sr = sampleRateHz;

    // ---- Lectura de parametros (una vez por bloque) ----
    const int   waveA  = juce::jlimit (0, Waves::numWaves - 1, (int) (p.aWave->load() + 0.5f));
    const int   waveB  = juce::jlimit (0, Waves::numWaves - 1, (int) (p.bWave->load() + 0.5f));

    if (waveA == Waves::customIndex && p.tableA != nullptr) Waves::prepareCustom (*p.tableA, customA);
    if (waveB == Waves::customIndex && p.tableB != nullptr) Waves::prepareCustom (*p.tableB, customB);
    const float lvlA   = p.aLvl->load();
    const float lvlB   = p.bLvl->load();
    const float ratioA = std::exp2 ((p.aOct->load() * 12.0f + p.aSemi->load() + p.aFine->load() * 0.01f) / 12.0f);
    const float ratioB = std::exp2 ((p.bOct->load() * 12.0f + p.bSemi->load() + p.bFine->load() * 0.01f) / 12.0f);

    const int   nUni     = juce::jlimit (1, maxUnison, (int) (p.uniVoices->load() + 0.5f));
    const float detuneCt = p.uniDetune->load() * 50.0f;
    const float spread   = p.uniSpread->load();
    const float uniGain  = 1.0f / std::sqrt ((float) nUni);

    std::array<float, maxUnison> uniRatio {}, panL {}, panR {};

    for (int i = 0; i < nUni; ++i)
    {
        const float pos   = (nUni == 1) ? 0.0f : ((float) i / (float) (nUni - 1)) * 2.0f - 1.0f;
        const float angle = (pos * spread * 0.5f + 0.5f) * juce::MathConstants<float>::halfPi;
        uniRatio[(size_t) i] = std::exp2 (pos * detuneCt / 1200.0f);
        panL[(size_t) i] = std::cos (angle) * 1.4142f;
        panR[(size_t) i] = std::sin (angle) * 1.4142f;
    }

    const float subLvl   = p.subLvl->load();
    const float noiseLvl = p.noiseLvl->load();
    const float pwBase   = p.pw->load();
    const float foldAmt  = p.fold->load();
    const float glideT   = p.glide->load();
    const float glideCoef = glideT > 0.001f ? 1.0f - std::exp (-1.0f / (glideT * (float) sr)) : 1.0f;

    const int   fType   = juce::jlimit (0, 3, (int) (p.fltType->load() + 0.5f));
    const float fEnvAmt = p.fltEnv->load();
    k = 1.0f / juce::jlimit (0.5f, 12.0f, p.reso->load());
    cutoffSmoothed.setTargetValue (juce::jlimit (20.0f, 20000.0f, p.cutoff->load()));

    ampEnv.setParameters (juce::ADSR::Parameters (p.aA->load(), p.aD->load(), p.aS->load(), p.aR->load()));
    fltEnvGen.setParameters (juce::ADSR::Parameters (p.fA->load(), p.fD->load(), p.fS->load(), p.fR->load()));

    const int   lfoShape = (int) (p.lfoShape->load() + 0.5f);
    const int   lfoDest  = (int) (p.lfoDest->load() + 0.5f);   // 0 off, 1 cutoff, 2 pitch, 3 amp, 4 pw
    const float lfoDepth = p.lfoDepth->load();
    const float lfoInc   = (float) (p.lfoRate->load() / sr);

    const float pitAmt  = p.pitEnv->load();
    const float pitCoef = std::exp (-1.0f / (juce::jmax (0.005f, p.pitDec->load()) * (float) sr));

    const int numChannels = outputBuffer.getNumChannels();
    unsigned int coefCounter = 0;

    for (int i = 0; i < numSamples; ++i)
    {
        if (! ampEnv.isActive())
        {
            clearCurrentNote();
            break;
        }

        // ---- Moduladores ----
        const float lfoRaw = (lfoDest != 0 && lfoDepth > 0.0001f) ? lfoSample (lfoShape, lfoInc) : 0.0f;
        const float lfoMod = lfoRaw * lfoDepth;
        const float fEnv   = fltEnvGen.getNextSample();
        const float fcBase = cutoffSmoothed.getNextValue();

        if ((coefCounter++ & 15u) == 0u)
        {
            float octaves = fEnvAmt * 6.0f * fEnv;

            if (lfoDest == 1)
                octaves += lfoMod * 3.0f;

            updateFilterCoefficients (fcBase * std::exp2 (octaves));
        }

        float semis = pitAmt * pitchEnvValue;
        pitchEnvValue *= pitCoef;

        if (lfoDest == 2)
            semis += lfoMod * 1.5f;

        const float modRatio = (semis != 0.0f) ? std::exp2 (semis / 12.0f) : 1.0f;
        currentFreq += (baseFrequency - currentFreq) * glideCoef;
        const float baseDt   = currentFreq * bendRatio * modRatio / (float) sr;

        const float pwEff = juce::jlimit (0.05f, 0.95f, pwBase + (lfoDest == 4 ? lfoMod * 0.4f : 0.0f));

        // ---- Osciladores A y B con unison ----
        float sumL = 0.0f, sumR = 0.0f;

        for (int u = 0; u < nUni; ++u)
        {
            const size_t ui = (size_t) u;
            float s = 0.0f;

            {
                const float dt = juce::jmin (0.45f, baseDt * ratioA * uniRatio[ui]);
                s += oscSample (waveA, phaseA[ui], dt, pwEff, customA.data()) * lvlA;
                phaseA[ui] += dt;
                if (phaseA[ui] >= 1.0f) phaseA[ui] -= 1.0f;
            }

            if (lvlB > 0.0005f)
            {
                const float dt = juce::jmin (0.45f, baseDt * ratioB * uniRatio[ui]);
                s += oscSample (waveB, phaseB[ui], dt, pwEff, customB.data()) * lvlB;
                phaseB[ui] += dt;
                if (phaseB[ui] >= 1.0f) phaseB[ui] -= 1.0f;
            }

            sumL += s * panL[ui];
            sumR += s * panR[ui];
        }

        sumL *= uniGain;
        sumR *= uniGain;

        if (foldAmt > 0.001f)
        {
            sumL = wavefold (sumL, foldAmt);
            sumR = wavefold (sumR, foldAmt);
        }

        // ---- Sub (seno una octava abajo) y ruido ----
        if (subLvl > 0.0005f)
        {
            const float sub = std::sin (juce::MathConstants<float>::twoPi * subPhase) * subLvl;
            subPhase += baseDt * 0.5f;
            if (subPhase >= 1.0f) subPhase -= 1.0f;
            sumL += sub;
            sumR += sub;
        }

        if (noiseLvl > 0.0005f)
        {
            sumL += (random.nextFloat() * 2.0f - 1.0f) * noiseLvl * 0.5f;
            sumR += (random.nextFloat() * 2.0f - 1.0f) * noiseLvl * 0.5f;
        }

        // ---- Filtro ----
        float outL = filterProcess (filtL, sumL, fType);
        float outR = filterProcess (filtR, sumR, fType);

        // ---- Amplitud ----
        float amp = ampEnv.getNextSample() * velocityGain * 0.22f;

        if (lfoDest == 3)
            amp *= 1.0f - lfoDepth * 0.5f * (1.0f - lfoRaw);

        outL *= amp;
        outR *= amp;

        if (numChannels >= 2)
        {
            outputBuffer.addSample (0, startSample + i, outL);
            outputBuffer.addSample (1, startSample + i, outR);
        }
        else
        {
            outputBuffer.addSample (0, startSample + i, 0.5f * (outL + outR));
        }
    }
}

//==============================================================================
// Layout de parametros
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
PerreoTrapSynthAudioProcessor::createParameterLayout()
{
    using FloatAttr = juce::AudioParameterFloatAttributes;
    using IntAttr   = juce::AudioParameterIntAttributes;
    using Fmt       = std::function<juce::String (float, int)>;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const Fmt pct = [] (float v, int) { return juce::String (juce::roundToInt (v * 100.0f)) + "%"; };
    const Fmt secs = [] (float v, int)
    {
        return v < 1.0f ? juce::String (juce::roundToInt (v * 1000.0f)) + " ms" : juce::String (v, 2) + " s";
    };
    const Fmt ms   = [] (float v, int) { return juce::String (juce::roundToInt (v)) + " ms"; };
    const Fmt hz   = [] (float v, int) { return juce::String (v, 2) + " Hz"; };
    const Fmt cents = [] (float v, int) { return (v > 0.0f ? "+" : "") + juce::String (juce::roundToInt (v)) + " ct"; };
    const Fmt semisFmt = [] (float v, int) { return (v > 0.0f ? "+" : "") + juce::String (v, 1) + " st"; };
    const Fmt bipolar = [] (float v, int) { return (v > 0.0f ? "+" : "") + juce::String (juce::roundToInt (v * 100.0f)) + "%"; };
    const Fmt num2 = [] (float v, int) { return juce::String (v, 2); };
    const Fmt cutoffFmt = [] (float v, int)
    {
        return v >= 1000.0f ? juce::String (v / 1000.0f, 2) + " kHz" : juce::String (juce::roundToInt (v)) + " Hz";
    };

    auto addFloat = [&layout] (const char* id, const char* name, float lo, float hi, float def,
                               float skew, Fmt fmt)
    {
        auto attr = FloatAttr();
        if (fmt) attr = attr.withStringFromValueFunction (fmt);

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { id, 1 }, name,
            juce::NormalisableRange<float> (lo, hi, 0.0001f, skew), def, attr));
    };

    auto addChoice = [&layout] (const char* id, const char* name, juce::StringArray items, int def)
    {
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { id, 1 }, name, items, def));
    };

    auto addInt = [&layout] (const char* id, const char* name, int lo, int hi, int def, bool signedDisplay)
    {
        auto attr = IntAttr();
        if (signedDisplay)
            attr = attr.withStringFromValueFunction ([] (int v, int) { return (v > 0 ? "+" : "") + juce::String (v); });

        layout.add (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { id, 1 }, name, lo, hi, def, attr));
    };

    auto addBool = [&layout] (const char* id, const char* name, bool def)
    {
        layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { id, 1 }, name, def));
    };

    juce::StringArray waves;

    for (int i = 0; i < Waves::numWaves; ++i)
        waves.add (Waves::waveName (i));

    // ---- Oscilador A ----
    addChoice ("aWave", "Osc A Wave", waves, 1);
    addInt    ("aOct",  "Osc A Octave", -2, 2, 0, true);
    addInt    ("aSemi", "Osc A Semi", -12, 12, 0, true);
    addFloat  ("aFine", "Osc A Fine", -100.0f, 100.0f, 0.0f, 1.0f, cents);
    addFloat  ("aLvl",  "Osc A Level", 0.0f, 1.0f, 1.0f, 1.0f, pct);

    // ---- Oscilador B ----
    addChoice ("bWave", "Osc B Wave", waves, 1);
    addInt    ("bOct",  "Osc B Octave", -2, 2, 0, true);
    addInt    ("bSemi", "Osc B Semi", -12, 12, 0, true);
    addFloat  ("bFine", "Osc B Fine", -100.0f, 100.0f, 0.0f, 1.0f, cents);
    addFloat  ("bLvl",  "Osc B Level", 0.0f, 1.0f, 0.0f, 1.0f, pct);

    // ---- Unison / mezcla ----
    addInt   ("uniVoices", "Unison Voices", 1, 7, 1, false);
    addFloat ("uniDetune", "Unison Detune", 0.0f, 1.0f, 0.25f, 1.0f, pct);
    addFloat ("uniSpread", "Unison Stereo", 0.0f, 1.0f, 0.7f, 1.0f, pct);
    addFloat ("pw",        "Pulse Width", 0.05f, 0.95f, 0.5f, 1.0f, pct);
    addFloat ("subLvl",    "Sub Level", 0.0f, 1.0f, 0.0f, 1.0f, pct);
    addFloat ("noiseLvl",  "Noise Level", 0.0f, 1.0f, 0.0f, 1.0f, pct);

    // ---- Filtro ----
    addChoice ("fltType", "Filter Type", { "Low Pass", "High Pass", "Band Pass", "Notch" }, 0);
    addFloat  ("cutoff", "Cutoff", 20.0f, 20000.0f, 8000.0f, 0.3f, cutoffFmt);
    addFloat  ("reso",   "Resonance", 0.5f, 10.0f, 0.707f, 0.5f, num2);
    addFloat  ("fltEnv", "Filter Env Amount", -1.0f, 1.0f, 0.0f, 1.0f, bipolar);
    addFloat  ("fA", "Filter Attack",  0.001f, 5.0f, 0.005f, 0.3f, secs);
    addFloat  ("fD", "Filter Decay",   0.005f, 5.0f, 0.3f,   0.3f, secs);
    addFloat  ("fS", "Filter Sustain", 0.0f,   1.0f, 0.3f,   1.0f, pct);
    addFloat  ("fR", "Filter Release", 0.005f, 8.0f, 0.3f,   0.3f, secs);

    // ---- Amplitud ----
    addFloat ("aA", "Amp Attack",  0.001f, 5.0f, 0.005f, 0.3f, secs);
    addFloat ("aD", "Amp Decay",   0.005f, 5.0f, 0.3f,   0.3f, secs);
    addFloat ("aS", "Amp Sustain", 0.0f,   1.0f, 0.7f,   1.0f, pct);
    addFloat ("aR", "Amp Release", 0.005f, 8.0f, 0.25f,  0.3f, secs);

    // ---- LFO ----
    addChoice ("lfoShape", "LFO Shape", { "Sine", "Triangle", "Saw", "Square", "S&H" }, 0);
    addChoice ("lfoDest",  "LFO Target", { "Off", "Cutoff", "Pitch", "Volume", "Pulse W" }, 0);
    addFloat  ("lfoRate",  "LFO Rate", 0.05f, 20.0f, 5.0f, 0.4f, hz);
    addFloat  ("lfoDepth", "LFO Depth", 0.0f, 1.0f, 0.3f, 1.0f, pct);

    // ---- Pitch envelope ----
    addFloat ("pitEnv", "Pitch Env", -24.0f, 24.0f, 0.0f, 1.0f, semisFmt);
    addFloat ("pitDec", "Pitch Env Time", 0.01f, 1.0f, 0.08f, 0.5f, secs);
    addFloat ("glide", "Glide", 0.0f, 1.0f, 0.0f, 0.5f, secs);
    addFloat ("fold",  "Wave Fold", 0.0f, 1.0f, 0.0f, 1.0f, pct);

    // ---- FX (cada uno con su interruptor) ----
    addBool  ("drvOn", "Drive On", false);
    addFloat ("drive", "Drive", 0.0f, 10.0f, 2.0f, 1.0f, num2);

    addBool  ("choOn",    "Chorus On", false);
    addFloat ("choRate",  "Chorus Rate", 0.1f, 5.0f, 0.8f, 0.5f, hz);
    addFloat ("choDepth", "Chorus Depth", 0.0f, 1.0f, 0.5f, 1.0f, pct);
    addFloat ("choMix",   "Chorus Mix", 0.0f, 1.0f, 0.4f, 1.0f, pct);

    addBool  ("dlyOn",   "Delay On", false);
    addFloat ("dlyTime", "Delay Time", 20.0f, 1500.0f, 375.0f, 0.5f, ms);
    addFloat ("dlyFb",   "Delay Feedback", 0.0f, 0.92f, 0.35f, 1.0f, pct);
    addFloat ("dlyMix",  "Delay Mix", 0.0f, 1.0f, 0.25f, 1.0f, pct);

    addBool  ("revOn",   "Reverb On", true);
    addFloat ("revSize", "Reverb Size", 0.0f, 1.0f, 0.5f, 1.0f, pct);
    addFloat ("revMix",  "Reverb Mix", 0.0f, 1.0f, 0.18f, 1.0f, pct);

    addFloat ("master", "Master Vol", 0.0f, 1.0f, 0.7f, 1.0f, pct);

    return layout;
}

//==============================================================================
// Procesador
//==============================================================================
PerreoTrapSynthAudioProcessor::PerreoTrapSynthAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout()),
      synthParams (apvts)
{
    auto get = [this] (const char* id) { return apvts.getRawParameterValue (id); };

    drvOn = get ("drvOn"); drive = get ("drive");
    choOn = get ("choOn"); choRate = get ("choRate"); choDepth = get ("choDepth"); choMix = get ("choMix");
    dlyOn = get ("dlyOn"); dlyTime = get ("dlyTime"); dlyFb = get ("dlyFb"); dlyMix = get ("dlyMix");
    revOn = get ("revOn"); revSize = get ("revSize"); revMix = get ("revMix");
    master = get ("master");

    jassert (getPresetTable().size() == (size_t) numPresets);

    Waves::bank();   // precalcula las ondas antes de que arranque el audio
    synthParams.tableA = &waveTableA;
    synthParams.tableB = &waveTableB;
    synthParams.lastNoteHz = &lastNoteHz;

    loadLibrariesFromDisk();

    synth.clearVoices();
    for (int i = 0; i < 10; ++i)
        synth.addVoice (new SynthVoice (synthParams));

    synth.clearSounds();
    synth.addSound (new SynthSound());
    synth.setNoteStealingEnabled (true);
}

PerreoTrapSynthAudioProcessor::~PerreoTrapSynthAudioProcessor()
{
}

void PerreoTrapSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);

    reverb.setSampleRate (sampleRate);
    reverb.reset();
    reverbActive = false;

    chorus.prepare (sampleRate);
    delay.prepare (sampleRate);

    const int blockSize = juce::jmax (512, samplesPerBlock);
    reverbScratch.setSize (2, blockSize, false, true, false);
    monoRight.setSize (1, blockSize, false, true, false);

    driveMixSm.reset (sampleRate, 0.02);
    driveAmtSm.reset (sampleRate, 0.03);
    reverbMixSm.reset (sampleRate, 0.03);
    masterSm.reset (sampleRate, 0.02);

    driveMixSm.setCurrentAndTargetValue (drvOn->load() > 0.5f ? 1.0f : 0.0f);
    driveAmtSm.setCurrentAndTargetValue (drive->load());
    reverbMixSm.setCurrentAndTargetValue (revOn->load() > 0.5f ? revMix->load() : 0.0f);
    masterSm.setCurrentAndTargetValue (master->load());
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

    const int n = buffer.getNumSamples();
    buffer.clear();

    synth.renderNextBlock (buffer, midiMessages, 0, n);

    // Vista estereo del bus (si el host es mono se usa un canal auxiliar)
    const bool isMono = buffer.getNumChannels() < 2;

    if (monoRight.getNumSamples() < n)
        monoRight.setSize (1, n, false, false, true);

    float* l = buffer.getWritePointer (0);
    float* r = isMono ? monoRight.getWritePointer (0) : buffer.getWritePointer (1);

    if (isMono)
        juce::FloatVectorOperations::copy (r, l, n);

    //--------------------------------------------------------------------------
    // DRIVE (saturacion tanh)
    //--------------------------------------------------------------------------
    driveMixSm.setTargetValue (drvOn->load() > 0.5f ? 1.0f : 0.0f);
    driveAmtSm.setTargetValue (drive->load());

    if (driveMixSm.isSmoothing() || driveMixSm.getTargetValue() > 0.5f)
    {
        constexpr float ref = 0.3f;

        for (int i = 0; i < n; ++i)
        {
            const float m   = driveMixSm.getNextValue();
            const float pre = 1.0f + driveAmtSm.getNextValue() * 1.6f;
            const float inv = ref / std::tanh (pre);

            const float wl = std::tanh (pre * l[i] / ref) * inv;
            const float wr = std::tanh (pre * r[i] / ref) * inv;

            l[i] += m * (wl - l[i]);
            r[i] += m * (wr - r[i]);
        }
    }
    else
    {
        driveMixSm.skip (n);
        driveAmtSm.skip (n);
    }

    //--------------------------------------------------------------------------
    // CHORUS y DELAY
    //--------------------------------------------------------------------------
    chorus.process (l, r, n, choOn->load() > 0.5f, choRate->load(), choDepth->load(), choMix->load());
    delay.process  (l, r, n, dlyOn->load() > 0.5f, dlyTime->load(), dlyFb->load(), dlyMix->load());

    //--------------------------------------------------------------------------
    // REVERB (100% wet en paralelo, mezcla suavizada)
    //--------------------------------------------------------------------------
    const bool revEnabled = revOn->load() > 0.5f;
    reverbMixSm.setTargetValue (revEnabled ? revMix->load() : 0.0f);

    if (revEnabled || reverbMixSm.isSmoothing() || reverbMixSm.getCurrentValue() > 0.0001f)
    {
        reverbActive = true;

        if (reverbScratch.getNumSamples() < n)
            reverbScratch.setSize (2, n, false, false, true);

        float* sl = reverbScratch.getWritePointer (0);
        float* sr = reverbScratch.getWritePointer (1);
        juce::FloatVectorOperations::copy (sl, l, n);
        juce::FloatVectorOperations::copy (sr, r, n);

        reverbParams.roomSize   = 0.30f + 0.68f * juce::jlimit (0.0f, 1.0f, revSize->load());
        reverbParams.damping    = 0.45f;
        reverbParams.wetLevel   = 1.0f / 3.0f;
        reverbParams.dryLevel   = 0.0f;
        reverbParams.width      = 1.0f;
        reverbParams.freezeMode = 0.0f;
        reverb.setParameters (reverbParams);
        reverb.processStereo (sl, sr, n);

        for (int i = 0; i < n; ++i)
        {
            const float m = reverbMixSm.getNextValue() * 1.5f;
            l[i] += sl[i] * m;
            r[i] += sr[i] * m;
        }
    }
    else if (reverbActive)
    {
        reverb.reset();
        reverbActive = false;
    }

    //--------------------------------------------------------------------------
    // MASTER + limitador de seguridad
    //--------------------------------------------------------------------------
    masterSm.setTargetValue (juce::jlimit (0.0f, 1.0f, master->load()));

    for (int i = 0; i < n; ++i)
    {
        const float gain = masterSm.getNextValue();
        l[i] = juce::jlimit (-1.0f, 1.0f, l[i] * gain);
        r[i] = juce::jlimit (-1.0f, 1.0f, r[i] * gain);
    }

    if (isMono)
        for (int i = 0; i < n; ++i)
            l[i] = 0.5f * (l[i] + r[i]);
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

        if (std::abs (param->getValue() - normalised) < 1.0e-6f)
            return;

        param->beginChangeGesture();
        param->setValueNotifyingHost (normalised);
        param->endChangeGesture();
    }
}

void PerreoTrapSynthAudioProcessor::resetParameterToDefault (const juce::String& paramID)
{
    if (auto* param = apvts.getParameter (paramID))
    {
        const float def = param->getDefaultValue();

        if (std::abs (param->getValue() - def) < 1.0e-6f)
            return;

        param->beginChangeGesture();
        param->setValueNotifyingHost (def);
        param->endChangeGesture();
    }
}

void PerreoTrapSynthAudioProcessor::applyPreset (int index)
{
    index = juce::jlimit (0, numPresets - 1, index);
    currentPreset.store (index);
    customPresetName = juce::String();
    customLibName    = juce::String();

    const auto& preset = getPresetTable()[(size_t) index];

    for (auto* param : getParameters())
    {
        auto* withID = dynamic_cast<juce::AudioProcessorParameterWithID*> (param);

        if (withID == nullptr)
            continue;

        bool found = false;

        for (const auto& kv : preset.values)
        {
            if (withID->paramID == kv.first)
            {
                setParameterValue (withID->paramID, kv.second);
                found = true;
                break;
            }
        }

        if (! found)
            resetParameterToDefault (withID->paramID);
    }
}

void PerreoTrapSynthAudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= numPresets)
        return;

    applyPreset (index);
}

juce::String PerreoTrapSynthAudioProcessor::getPresetShortName (int index) const
{
    if (index < 0 || index >= numPresets)
        return {};

    return juce::String (getPresetTable()[(size_t) index].name);
}

const juce::String PerreoTrapSynthAudioProcessor::getProgramName (int index)
{
    if (index < 0 || index >= numPresets)
        return {};

    using namespace PresetLayout;

    const juce::String cat (categoryName (categoryOf (index)));
    const juce::String catNice = cat.substring (0, 1) + cat.substring (1).toLowerCase();

    return juce::String (genreName (genreOf (index))) + " - " + catNice + " - " + getPresetShortName (index);
}

void PerreoTrapSynthAudioProcessor::changeProgramName (int, const juce::String&)
{
}

//==============================================================================
// Librerias y packs (.ptspack = XML con varios presets)
//==============================================================================
namespace
{
    constexpr int maxPresetsPerPack = 500;

    juce::File getLibrariesFolder()
    {
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                   .getChildFile ("PerreoTrapSynth").getChildFile ("Libraries");
    }

    juce::File getLibraryFile (const juce::String& libName)
    {
        return getLibrariesFolder().getChildFile (juce::File::createLegalFileName (libName) + ".ptspack");
    }

    std::unique_ptr<juce::XmlElement> packToXml (const juce::String& packName,
                                                 const std::vector<LibraryPreset>& presets)
    {
        auto xml = std::make_unique<juce::XmlElement> ("PerreoTrapSynthPack");
        xml->setAttribute ("version", 1);
        xml->setAttribute ("name", packName);

        for (const auto& p : presets)
        {
            auto* e = xml->createNewChildElement ("Preset");
            e->setAttribute ("name", p.name);
            e->setAttribute ("category", p.category);

            if (p.waveA.isNotEmpty()) e->setAttribute ("waveA", p.waveA);
            if (p.waveB.isNotEmpty()) e->setAttribute ("waveB", p.waveB);

            for (const auto& kv : p.values)
                e->setAttribute (kv.first, (double) kv.second);
        }

        return xml;
    }

    bool xmlToPack (const juce::XmlElement& xml, PresetLibrary& out)
    {
        if (! xml.hasTagName ("PerreoTrapSynthPack"))
            return false;

        out.name = xml.getStringAttribute ("name").trim();
        out.presets.clear();

        for (auto* e : xml.getChildWithTagNameIterator ("Preset"))
        {
            if ((int) out.presets.size() >= maxPresetsPerPack)
                break;

            LibraryPreset lp;
            lp.name     = e->getStringAttribute ("name", "Preset");
            lp.category = e->getStringAttribute ("category").trim().toUpperCase();

            for (int i = 0; i < e->getNumAttributes(); ++i)
            {
                const auto attr = e->getAttributeName (i);

                if (attr == "name" || attr == "category")
                    continue;

                if (attr == "waveA") { lp.waveA = e->getAttributeValue (i); continue; }
                if (attr == "waveB") { lp.waveB = e->getAttributeValue (i); continue; }

                lp.values.emplace_back (attr, (float) e->getAttributeValue (i).getDoubleValue());
            }

            out.presets.push_back (std::move (lp));
        }

        return out.name.isNotEmpty() && ! out.presets.empty();
    }
}

void PerreoTrapSynthAudioProcessor::loadLibrariesFromDisk()
{
    libraries.clear();

    PresetLibrary user;
    user.name = "USER";

    std::vector<PresetLibrary> others;

    for (const auto& f : getLibrariesFolder().findChildFiles (juce::File::findFiles, false, "*.ptspack"))
    {
        auto xml = juce::XmlDocument::parse (f);
        PresetLibrary lib;

        if (xml == nullptr || ! xmlToPack (*xml, lib))
            continue;

        if (lib.name == "USER")
            user = std::move (lib);
        else
            others.push_back (std::move (lib));
    }

    std::sort (others.begin(), others.end(),
               [] (const PresetLibrary& a, const PresetLibrary& b) { return a.name.compareIgnoreCase (b.name) < 0; });

    libraries.push_back (std::move (user));

    for (auto& l : others)
        libraries.push_back (std::move (l));
}

void PerreoTrapSynthAudioProcessor::saveLibraryToDisk (int libIndex) const
{
    if (libIndex < 0 || libIndex >= (int) libraries.size())
        return;

    const auto& lib = libraries[(size_t) libIndex];
    getLibrariesFolder().createDirectory();

    if (auto xml = packToXml (lib.name, lib.presets))
        xml->writeTo (getLibraryFile (lib.name));
}

void PerreoTrapSynthAudioProcessor::applyValues (const std::vector<std::pair<juce::String, float>>& values)
{
    for (auto* param : getParameters())
    {
        auto* withID = dynamic_cast<juce::AudioProcessorParameterWithID*> (param);

        if (withID == nullptr)
            continue;

        bool found = false;

        for (const auto& kv : values)
        {
            if (withID->paramID == kv.first)
            {
                const auto range = apvts.getParameterRange (withID->paramID);
                setParameterValue (withID->paramID, juce::jlimit (range.start, range.end, kv.second));
                found = true;
                break;
            }
        }

        if (! found)
            resetParameterToDefault (withID->paramID);
    }
}

LibraryPreset PerreoTrapSynthAudioProcessor::captureCurrentSound (const juce::String& name,
                                                                  const juce::String& category) const
{
    LibraryPreset lp;
    lp.name = name;
    lp.category = category;

    for (auto* param : getParameters())
        if (auto* withID = dynamic_cast<juce::AudioProcessorParameterWithID*> (param))
            if (auto* raw = apvts.getRawParameterValue (withID->paramID))
                lp.values.emplace_back (withID->paramID, raw->load());

    // Si algun oscilador usa la onda dibujada, se guarda con el preset
    if (auto* wa = apvts.getRawParameterValue ("aWave"); wa != nullptr && (int) (wa->load() + 0.5f) == Waves::customIndex)
        lp.waveA = encodeWave (0);

    if (auto* wb = apvts.getRawParameterValue ("bWave"); wb != nullptr && (int) (wb->load() + 0.5f) == Waves::customIndex)
        lp.waveB = encodeWave (1);

    return lp;
}

void PerreoTrapSynthAudioProcessor::loadLibraryPreset (int libIndex, int presetIndex)
{
    if (libIndex < 0 || libIndex >= (int) libraries.size())
        return;

    const auto& lib = libraries[(size_t) libIndex];

    if (presetIndex < 0 || presetIndex >= (int) lib.presets.size())
        return;

    const auto& p = lib.presets[(size_t) presetIndex];
    applyValues (p.values);

    if (p.waveA.isNotEmpty()) decodeWave (0, p.waveA);
    if (p.waveB.isNotEmpty()) decodeWave (1, p.waveB);

    customLibName    = lib.name;
    customPresetName = p.name;
}

bool PerreoTrapSynthAudioProcessor::saveCurrentSoundToUser (const juce::String& name, const juce::String& category)
{
    const auto cleanName = name.trim();

    if (cleanName.isEmpty() || libraries.empty())
        return false;

    auto& user = libraries[0];
    auto lp = captureCurrentSound (cleanName, category);

    bool replaced = false;

    for (auto& existing : user.presets)
    {
        if (existing.name.equalsIgnoreCase (cleanName))
        {
            existing = lp;
            replaced = true;
            break;
        }
    }

    if (! replaced)
        user.presets.push_back (std::move (lp));

    saveLibraryToDisk (0);

    customLibName    = user.name;
    customPresetName = cleanName;
    return true;
}

void PerreoTrapSynthAudioProcessor::removeLibrary (int libIndex)
{
    if (libIndex <= 0 || libIndex >= (int) libraries.size())   // USER no se borra
        return;

    const auto name = libraries[(size_t) libIndex].name;
    getLibraryFile (name).deleteFile();
    libraries.erase (libraries.begin() + libIndex);

    if (customLibName == name)
    {
        customLibName    = juce::String();
        customPresetName = juce::String();
    }
}

std::vector<LibraryPreset> PerreoTrapSynthAudioProcessor::collectLibrary (int libIndex) const
{
    if (libIndex < 0 || libIndex >= (int) libraries.size())
        return {};

    return libraries[(size_t) libIndex].presets;
}

std::vector<LibraryPreset> PerreoTrapSynthAudioProcessor::collectFactory (int genre) const
{
    std::vector<LibraryPreset> out;
    const auto& table = getPresetTable();

    for (int i = 0; i < numPresets; ++i)
    {
        if (genre >= 0 && PresetLayout::genreOf (i) != genre)
            continue;

        LibraryPreset lp;
        lp.name = table[(size_t) i].name;
        lp.category = PresetLayout::categoryName (PresetLayout::categoryOf (i));

        // Valores completos: lo que el preset indica + el valor por defecto del resto
        for (auto* param : getParameters())
        {
            auto* withID = dynamic_cast<juce::AudioProcessorParameterWithID*> (param);

            if (withID == nullptr)
                continue;

            float value = apvts.getParameterRange (withID->paramID).convertFrom0to1 (param->getDefaultValue());
            for (const auto& kv : table[(size_t) i].values)
                if (withID->paramID == kv.first)
                    value = kv.second;

            lp.values.emplace_back (withID->paramID, value);
        }

        out.push_back (std::move (lp));
    }

    return out;
}

bool PerreoTrapSynthAudioProcessor::exportPack (const juce::File& file, const juce::String& packName,
                                                const std::vector<LibraryPreset>& presets) const
{
    if (presets.empty())
        return false;

    auto xml = packToXml (packName, presets);
    return xml != nullptr && xml->writeTo (file);
}

int PerreoTrapSynthAudioProcessor::importPack (const juce::File& file, juce::String& packNameOut)
{
    auto xml = juce::XmlDocument::parse (file);

    if (xml == nullptr)
        return 0;

    // Compatibilidad: un .ptspreset suelto (version anterior) se guarda en USER
    if (xml->hasTagName ("PerreoTrapSynthPreset"))
    {
        LibraryPreset lp;
        lp.name = xml->getStringAttribute ("name", file.getFileNameWithoutExtension());

        for (int i = 0; i < xml->getNumAttributes(); ++i)
        {
            const auto attr = xml->getAttributeName (i);

            if (attr != "name" && attr != "version")
                lp.values.emplace_back (attr, (float) xml->getAttributeValue (i).getDoubleValue());
        }

        if (lp.values.empty() || libraries.empty())
            return 0;

        libraries[0].presets.push_back (std::move (lp));
        saveLibraryToDisk (0);
        packNameOut = "USER";
        return 1;
    }

    PresetLibrary lib;

    if (! xmlToPack (*xml, lib))
        return 0;

    if (lib.name.equalsIgnoreCase ("USER"))
        lib.name = "USER (imported)";

    const int count = (int) lib.presets.size();
    packNameOut = lib.name;

    // Misma libreria importada otra vez = se reemplaza
    int existing = -1;

    for (int i = 1; i < (int) libraries.size(); ++i)
        if (libraries[(size_t) i].name.equalsIgnoreCase (lib.name))
            existing = i;

    int saveIndex = existing;

    if (existing >= 0)
    {
        libraries[(size_t) existing] = std::move (lib);
    }
    else
    {
        libraries.push_back (std::move (lib));
        saveIndex = (int) libraries.size() - 1;
    }

    saveLibraryToDisk (saveIndex);
    return count;
}

//==============================================================================
// Ondas editables
//==============================================================================
juce::String PerreoTrapSynthAudioProcessor::encodeWave (int osc) const
{
    const auto& table = osc == 0 ? waveTableA : waveTableB;
    std::array<float, Waves::tableSize> t;
    table.load (t);

    juce::StringArray parts;

    for (float v : t)
        parts.add (juce::String (juce::roundToInt (v * 1000.0f)));

    return parts.joinIntoString (" ");
}

void PerreoTrapSynthAudioProcessor::decodeWave (int osc, const juce::String& text)
{
    const auto parts = juce::StringArray::fromTokens (text, " ", "");

    if (parts.size() != Waves::tableSize)
        return;

    std::array<float, Waves::tableSize> t;

    for (int i = 0; i < Waves::tableSize; ++i)
        t[(size_t) i] = (float) parts[i].getIntValue() / 1000.0f;

    getWaveTable (osc).store (t);
}

//==============================================================================
// Sonido aleatorio (para encontrar ideas rapido)
//==============================================================================
void PerreoTrapSynthAudioProcessor::randomizeSound()
{
    auto r  = [this] (float lo, float hi) { return lo + rng.nextFloat() * (hi - lo); };
    auto ri = [this] (int lo, int hi)     { return (float) rng.nextInt (juce::Range<int> (lo, hi + 1)); };
    auto chance = [this] (float p)        { return rng.nextFloat() < p; };
    auto set = [this] (const char* id, float v) { setParameterValue (id, v); };

    set ("aWave", ri (0, Waves::customIndex - 1));
    set ("bWave", ri (0, Waves::customIndex - 1));
    set ("aOct", ri (-1, 0));
    set ("bOct", ri (-1, 1));
    set ("aSemi", 0.0f);
    set ("bSemi", chance (0.5f) ? 0.0f : (chance (0.5f) ? 7.0f : 12.0f));
    set ("aFine", 0.0f);
    set ("bFine", r (-15.0f, 15.0f));
    set ("aLvl", 1.0f);
    set ("bLvl", chance (0.6f) ? r (0.2f, 0.8f) : 0.0f);

    set ("uniVoices", ri (1, 5));
    set ("uniDetune", r (0.05f, 0.4f));
    set ("uniSpread", r (0.3f, 1.0f));
    set ("pw", r (0.2f, 0.7f));
    set ("subLvl", chance (0.3f) ? r (0.1f, 0.4f) : 0.0f);
    set ("noiseLvl", chance (0.25f) ? r (0.02f, 0.15f) : 0.0f);

    set ("fltType", chance (0.8f) ? 0.0f : ri (1, 3));
    set ("cutoff", r (500.0f, 9000.0f));
    set ("reso", r (0.7f, 4.0f));
    set ("fltEnv", r (0.0f, 0.8f));
    set ("fA", r (0.0f, 0.3f));
    set ("fD", r (0.1f, 1.2f));
    set ("fS", r (0.0f, 0.8f));
    set ("fR", r (0.1f, 1.0f));

    set ("aA", r (0.002f, 0.5f));
    set ("aD", r (0.1f, 1.5f));
    set ("aS", r (0.1f, 1.0f));
    set ("aR", r (0.05f, 1.5f));

    set ("lfoShape", ri (0, 4));
    set ("lfoDest", ri (0, 4));
    set ("lfoRate", r (0.2f, 7.0f));
    set ("lfoDepth", r (0.0f, 0.5f));
    set ("pitEnv", chance (0.3f) ? r (-5.0f, 5.0f) : 0.0f);
    set ("pitDec", r (0.02f, 0.3f));
    set ("fold", chance (0.3f) ? r (0.05f, 0.35f) : 0.0f);
    set ("glide", 0.0f);

    set ("drvOn", chance (0.25f) ? 1.0f : 0.0f);
    set ("drive", r (1.0f, 4.0f));
    set ("choOn", chance (0.5f) ? 1.0f : 0.0f);
    set ("choMix", r (0.2f, 0.5f));
    set ("dlyOn", chance (0.5f) ? 1.0f : 0.0f);
    set ("dlyTime", ri (180, 500));
    set ("dlyFb", r (0.2f, 0.5f));
    set ("dlyMix", r (0.1f, 0.35f));
    set ("revOn", 1.0f);
    set ("revSize", r (0.3f, 0.9f));
    set ("revMix", r (0.1f, 0.5f));

    customLibName    = juce::String();
    customPresetName = "Random Sound";
}

//==============================================================================
// Estado
//==============================================================================
void PerreoTrapSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty ("presetIndex", currentPreset.load(), nullptr);
    state.setProperty ("customPresetName", customPresetName, nullptr);
    state.setProperty ("customLibName", customLibName, nullptr);
    state.setProperty ("waveA", encodeWave (0), nullptr);
    state.setProperty ("waveB", encodeWave (1), nullptr);

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
            customPresetName = tree.getProperty ("customPresetName", juce::String()).toString();
            customLibName    = tree.getProperty ("customLibName", juce::String()).toString();
            decodeWave (0, tree.getProperty ("waveA", juce::String()).toString());
            decodeWave (1, tree.getProperty ("waveB", juce::String()).toString());
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
