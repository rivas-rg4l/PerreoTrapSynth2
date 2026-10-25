#pragma once

#include <algorithm>
#include <vector>
#include <cmath>
#include <juce_audio_basics/juce_audio_basics.h>

//==============================================================================
// Efectos simples de bus (stereo). Cada uno tiene un "mix" suavizado que se
// lleva a 0 cuando el efecto se apaga, asi no hay clicks al activar/desactivar.
//==============================================================================

//------------------------------------------------------------------------------
// Chorus estereo (2 lineas con LFO desfasado)
//------------------------------------------------------------------------------
class SimpleChorus
{
public:
    void prepare (double sr)
    {
        sampleRate = sr;
        const int size = (int) (sr * 0.08) + 4;
        bufL.assign ((size_t) size, 0.0f);
        bufR.assign ((size_t) size, 0.0f);
        writePos = 0;
        phase = 0.0f;
        mixSm.reset (sr, 0.03);
        mixSm.setCurrentAndTargetValue (0.0f);
    }

    void reset()
    {
        std::fill (bufL.begin(), bufL.end(), 0.0f);
        std::fill (bufR.begin(), bufR.end(), 0.0f);
    }

    bool isActive() const noexcept { return active; }

    void process (float* l, float* r, int n, bool on, float rateHz, float depth, float mix)
    {
        mixSm.setTargetValue (on ? mix : 0.0f);

        if (! on && ! mixSm.isSmoothing() && mixSm.getCurrentValue() < 0.0001f)
        {
            if (active) { reset(); active = false; }
            return;
        }

        active = true;
        const int size = (int) bufL.size();
        const float inc = rateHz / (float) sampleRate;
        const float modDepth = (0.002f + 0.007f * depth) * (float) sampleRate;
        const float centre   = 0.012f * (float) sampleRate;

        for (int i = 0; i < n; ++i)
        {
            bufL[(size_t) writePos] = l[i];
            bufR[(size_t) writePos] = r[i];

            const float lfoL = std::sin (juce::MathConstants<float>::twoPi * phase);
            const float lfoR = std::sin (juce::MathConstants<float>::twoPi * (phase + 0.25f));

            auto readLine = [&] (const std::vector<float>& b, float d)
            {
                float rp = (float) writePos - d;
                while (rp < 0.0f) rp += (float) size;
                const int i0 = (int) rp;
                const int i1 = (i0 + 1) % size;
                const float f = rp - (float) i0;
                return b[(size_t) i0] * (1.0f - f) + b[(size_t) i1] * f;
            };

            const float wl = readLine (bufL, centre + modDepth * lfoL);
            const float wr = readLine (bufR, centre + modDepth * lfoR);
            const float m  = mixSm.getNextValue();

            l[i] = l[i] * (1.0f - 0.35f * m) + wl * m * 0.9f;
            r[i] = r[i] * (1.0f - 0.35f * m) + wr * m * 0.9f;

            phase += inc;
            if (phase >= 1.0f) phase -= 1.0f;
            if (++writePos >= size) writePos = 0;
        }
    }

private:
    double sampleRate = 44100.0;
    std::vector<float> bufL, bufR;
    int writePos = 0;
    float phase = 0.0f;
    bool active = false;
    juce::SmoothedValue<float> mixSm { 0.0f };
};

//------------------------------------------------------------------------------
// Delay ping-pong con filtro pasa-bajos en el feedback
//------------------------------------------------------------------------------
class PingPongDelay
{
public:
    void prepare (double sr)
    {
        sampleRate = sr;
        const int size = (int) (sr * 2.2) + 4;
        bufL.assign ((size_t) size, 0.0f);
        bufR.assign ((size_t) size, 0.0f);
        writePos = 0;
        lpL = lpR = 0.0f;
        mixSm.reset (sr, 0.03);
        mixSm.setCurrentAndTargetValue (0.0f);
        timeSm.reset (sr, 0.15);
        timeSm.setCurrentAndTargetValue (0.3f);
    }

    void reset()
    {
        std::fill (bufL.begin(), bufL.end(), 0.0f);
        std::fill (bufR.begin(), bufR.end(), 0.0f);
        lpL = lpR = 0.0f;
    }

    void process (float* l, float* r, int n, bool on, float timeMs, float feedback, float mix)
    {
        mixSm.setTargetValue (on ? mix : 0.0f);
        timeSm.setTargetValue (juce::jlimit (0.02f, 2.0f, timeMs * 0.001f));

        if (! on && ! mixSm.isSmoothing() && mixSm.getCurrentValue() < 0.0001f)
        {
            if (active) { reset(); active = false; }
            return;
        }

        active = true;
        const int size = (int) bufL.size();
        const float fb = juce::jlimit (0.0f, 0.92f, feedback);
        const float lpCoef = 0.55f; // oscurece cada repeticion

        for (int i = 0; i < n; ++i)
        {
            const float d = timeSm.getNextValue() * (float) sampleRate;
            float rp = (float) writePos - d;
            while (rp < 0.0f) rp += (float) size;
            const int i0 = (int) rp;
            const int i1 = (i0 + 1) % size;
            const float f = rp - (float) i0;

            const float dl = bufL[(size_t) i0] * (1.0f - f) + bufL[(size_t) i1] * f;
            const float dr = bufR[(size_t) i0] * (1.0f - f) + bufR[(size_t) i1] * f;

            lpL += lpCoef * (dl - lpL);
            lpR += lpCoef * (dr - lpR);

            const float in = 0.5f * (l[i] + r[i]);
            bufL[(size_t) writePos] = in + fb * lpR;
            bufR[(size_t) writePos] = lpL;

            const float m = mixSm.getNextValue();
            l[i] += dl * m;
            r[i] += dr * m;

            if (++writePos >= size) writePos = 0;
        }
    }

private:
    double sampleRate = 44100.0;
    std::vector<float> bufL, bufR;
    int writePos = 0;
    float lpL = 0.0f, lpR = 0.0f;
    bool active = false;
    juce::SmoothedValue<float> mixSm { 0.0f };
    juce::SmoothedValue<float> timeSm { 0.3f };
};
