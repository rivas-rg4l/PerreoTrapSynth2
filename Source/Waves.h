#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>

//==============================================================================
// Ondas del sintetizador: lista de formas, tablas precalculadas y onda editable
//==============================================================================
namespace Waves
{
    constexpr int tableSize   = 256;    // puntos de la onda dibujable
    constexpr int bankSize    = 1024;   // resolucion de las ondas precalculadas
    constexpr int firstTable  = 5;      // 0..4 se calculan al vuelo; 5..11 salen de tablas
    constexpr int customIndex = 12;     // "Custom (draw)"
    constexpr int numWaves    = 13;

    inline const char* waveName (int i)
    {
        static const char* const names[numWaves] =
        {
            "Sine", "Saw", "Square", "Triangle", "Pulse",
            "Half Sine", "Soft Saw", "Organ", "Stairs", "Parabolic", "Formant", "FM Bell",
            "Custom (draw)"
        };

        return names[std::clamp (i, 0, numWaves - 1)];
    }

    //--------------------------------------------------------------------------
    // Onda editable: la escribe el editor (hilo de mensajes) y la lee el audio
    //--------------------------------------------------------------------------
    struct Table
    {
        std::array<std::atomic<float>, tableSize> pts;

        Table()
        {
            for (int i = 0; i < tableSize; ++i)
                pts[(size_t) i].store (std::sin (6.2831853f * (float) i / (float) tableSize));
        }

        void load (std::array<float, tableSize>& out) const
        {
            for (int i = 0; i < tableSize; ++i)
                out[(size_t) i] = pts[(size_t) i].load (std::memory_order_relaxed);
        }

        void store (const std::array<float, tableSize>& in)
        {
            for (int i = 0; i < tableSize; ++i)
                pts[(size_t) i].store (std::clamp (in[(size_t) i], -1.0f, 1.0f), std::memory_order_relaxed);
        }
    };

    //--------------------------------------------------------------------------
    inline void removeDcAndNormalise (float* t, int n)
    {
        double mean = 0.0;

        for (int i = 0; i < n; ++i)
            mean += t[i];

        mean /= (double) n;

        float peak = 0.0f;

        for (int i = 0; i < n; ++i)
        {
            t[i] -= (float) mean;
            peak = std::max (peak, std::abs (t[i]));
        }

        for (int i = 0; i < n; ++i)
            t[i] = peak > 1.0e-4f ? t[i] / peak : 0.0f;
    }

    inline float builtInRaw (int wave, float p)
    {
        const float tp = 6.2831853f * p;

        switch (wave)
        {
            case 5:  return std::abs (std::sin (tp));                                   // Half Sine
            case 6:  return std::sin (tp) - 0.5f * std::sin (2.0f * tp)                  // Soft Saw
                          + 0.3333f * std::sin (3.0f * tp) - 0.25f * std::sin (4.0f * tp)
                          + 0.2f * std::sin (5.0f * tp);
            case 7:  return std::sin (tp) + 0.7f * std::sin (2.0f * tp)                  // Organ
                          + 0.5f * std::sin (3.0f * tp) + 0.4f * std::sin (4.0f * tp)
                          + 0.25f * std::sin (6.0f * tp) + 0.2f * std::sin (8.0f * tp);
            case 8:  return std::floor (p * 6.0f);                                       // Stairs
            case 9:  return 6.0f * p * (1.0f - p);                                       // Parabolic
            case 10:                                                                     // Formant
            {
                float v = 0.0f;

                for (int h = 1; h <= 14; ++h)
                {
                    const float d = (float) (h - 6);
                    v += std::sin ((float) h * tp) / (float) h * (1.0f + 3.0f * std::exp (-d * d / 3.0f));
                }

                return v;
            }
            case 11: return std::sin (tp + 3.0f * std::sin (2.0f * tp));                 // FM Bell
            default: return std::sin (tp);
        }
    }

    // Ondas 5..11 precalculadas (sin DC, pico = 1). Un punto extra para interpolar.
    inline const std::array<std::array<float, bankSize + 1>, 7>& bank()
    {
        static const auto table = []
        {
            std::array<std::array<float, bankSize + 1>, 7> out {};

            for (int w = 0; w < 7; ++w)
            {
                auto& t = out[(size_t) w];

                for (int i = 0; i < bankSize; ++i)
                    t[(size_t) i] = builtInRaw (firstTable + w, (float) i / (float) bankSize);

                removeDcAndNormalise (t.data(), bankSize);
                t[(size_t) bankSize] = t[0];
            }

            return out;
        }();

        return table;
    }

    inline float readTable (const float* t, int size, float phase)
    {
        const float pos = phase * (float) size;
        int i = (int) pos;
        i = std::clamp (i, 0, size - 1);
        const float frac = pos - (float) i;
        return t[i] + (t[i + 1] - t[i]) * frac;
    }

    inline float sampleBuiltIn (int wave, float phase)
    {
        return readTable (bank()[(size_t) (wave - firstTable)].data(), bankSize, phase);
    }

    // Copia la onda editable lista para el audio (sin DC, normalizada, con punto extra)
    inline void prepareCustom (const Table& tab, std::array<float, tableSize + 1>& out)
    {
        std::array<float, tableSize> raw;
        tab.load (raw);

        for (int i = 0; i < tableSize; ++i)
            out[(size_t) i] = raw[(size_t) i];

        removeDcAndNormalise (out.data(), tableSize);
        out[(size_t) tableSize] = out[0];
    }

    // Forma de cada onda para dibujarla en pantalla (wave 0..11)
    inline float displaySample (int wave, float phase, float pulseWidth)
    {
        switch (wave)
        {
            case 0:  return std::sin (6.2831853f * phase);
            case 1:  return 2.0f * phase - 1.0f;
            case 2:  return phase < 0.5f ? 1.0f : -1.0f;
            case 3:  return 2.0f * std::abs (2.0f * phase - 1.0f) - 1.0f;
            case 4:  return phase < pulseWidth ? 1.0f : -1.0f;
            default: return wave >= firstTable && wave < customIndex ? sampleBuiltIn (wave, phase) : 0.0f;
        }
    }
}
