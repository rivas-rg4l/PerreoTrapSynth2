#pragma once

#include <array>
#include <atomic>
#include <utility>
#include <vector>
#include <cmath>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Effects.h"
#include "Waves.h"
#include "Presets.h"

//==============================================================================
// Punteros directos a los parametros (lectura lock-free desde el audio thread)
//==============================================================================
struct SynthParams
{
    explicit SynthParams (juce::AudioProcessorValueTreeState& apvts);

    std::atomic<float> *aWave, *aOct, *aSemi, *aFine, *aLvl;
    std::atomic<float> *bWave, *bOct, *bSemi, *bFine, *bLvl;
    std::atomic<float> *uniVoices, *uniDetune, *uniSpread;
    std::atomic<float> *pw, *subLvl, *noiseLvl;
    std::atomic<float> *fltType, *cutoff, *reso, *fltEnv, *fA, *fD, *fS, *fR;
    std::atomic<float> *aA, *aD, *aS, *aR;
    std::atomic<float> *lfoShape, *lfoDest, *lfoRate, *lfoDepth;
    std::atomic<float> *pitEnv, *pitDec;
    std::atomic<float> *glide = nullptr, *fold = nullptr;

    // Ondas editables y ultima nota (para el glide); los asigna el procesador
    const Waves::Table* tableA = nullptr;
    const Waves::Table* tableB = nullptr;
    std::atomic<float>* lastNoteHz = nullptr;
};

//==============================================================================
// Sonido y voz del sintetizador
//==============================================================================
class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override    { return true; }
    bool appliesToChannel (int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    static constexpr int maxUnison = 7;

    explicit SynthVoice (const SynthParams& params);

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newPitchWheelValue) override;
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    void setCurrentPlaybackSampleRate (double newRate) override;
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    struct Svf
    {
        float ic1 = 0.0f, ic2 = 0.0f;
        void reset() { ic1 = ic2 = 0.0f; }
    };

    static float polyBlep (float t, float dt) noexcept;
    static float oscSample (int wave, float phase, float dt, float pulseWidth, const float* custom) noexcept;
    static float wavefold (float x, float amount) noexcept;
    float lfoSample (int shape, float increment) noexcept;
    float filterProcess (Svf& s, float in, int type) const noexcept;
    void updateFilterCoefficients (float cutoffHz) noexcept;
    void updatePitchBend (int pitchWheelPosition) noexcept;

    const SynthParams& p;

    juce::ADSR ampEnv, fltEnvGen;
    juce::Random random;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> cutoffSmoothed { 8000.0f };

    double sampleRateHz  = 44100.0;
    float  baseFrequency = 440.0f;
    float  currentFreq   = 440.0f;   // frecuencia con glide
    std::array<float, Waves::tableSize + 1> customA {}, customB {};
    float  bendRatio     = 1.0f;
    float  velocityGain  = 0.0f;

    std::array<float, maxUnison> phaseA {}, phaseB {};
    float subPhase = 0.0f;

    float lfoPhase = 0.0f, lfoHold = 0.0f;
    float pitchEnvValue = 1.0f;

    // Filtro SVF (TPT) estereo
    Svf filtL, filtR;
    float g = 0.0f, k = 1.0f, a1 = 0.0f, a2 = 0.0f, a3 = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthVoice)
};

//==============================================================================
// Librerias (packs) de presets del usuario
//==============================================================================
struct LibraryPreset
{
    juce::String name;
    juce::String category;                                  // puede estar vacia
    juce::String waveA, waveB;                              // ondas dibujadas (vacio si no se usan)
    std::vector<std::pair<juce::String, float>> values;     // (idParametro, valor)
};

struct PresetLibrary
{
    juce::String name;
    std::vector<LibraryPreset> presets;
};

//==============================================================================
// Procesador principal
//==============================================================================
class PerreoTrapSynthAudioProcessor : public juce::AudioProcessor
{
public:
    static constexpr int numPresets = PresetLayout::total;

    PerreoTrapSynthAudioProcessor();
    ~PerreoTrapSynthAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "PerreoTrapSynth"; }
    bool acceptsMidi() const override  { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    // Programas = presets
    int getNumPrograms() override { return numPresets; }
    int getCurrentProgram() override { return currentPreset.load(); }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    // Nombre corto (solo el nombre del preset, sin genero/categoria)
    juce::String getPresetShortName (int index) const;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //--------------------------------------------------------------------------
    // Librerias / packs (.ptspack).  La libreria 0 siempre es "USER".
    //--------------------------------------------------------------------------
    const std::vector<PresetLibrary>& getLibraries() const { return libraries; }

    // Vacios si el sonido actual viene de los presets de fabrica
    juce::String getCustomPresetName() const  { return customPresetName; }
    juce::String getCustomLibraryName() const { return customLibName; }

    void loadLibraryPreset (int libIndex, int presetIndex);
    bool saveCurrentSoundToUser (const juce::String& name, const juce::String& category);
    void removeLibrary (int libIndex);

    // Ondas editables (Custom) y sonido aleatorio
    Waves::Table& getWaveTable (int osc) { return osc == 0 ? waveTableA : waveTableB; }
    juce::String encodeWave (int osc) const;
    void decodeWave (int osc, const juce::String& text);
    void randomizeSound();

    // Packs
    std::vector<LibraryPreset> collectLibrary (int libIndex) const;
    std::vector<LibraryPreset> collectFactory (int genre) const;   // genre = -1 -> todos
    bool exportPack (const juce::File& file, const juce::String& packName,
                     const std::vector<LibraryPreset>& presets) const;
    int  importPack (const juce::File& file, juce::String& packNameOut);   // devuelve n de presets (0 = error)

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts;

private:
    void applyPreset (int index);
    void applyValues (const std::vector<std::pair<juce::String, float>>& values);
    LibraryPreset captureCurrentSound (const juce::String& name, const juce::String& category) const;
    void loadLibrariesFromDisk();
    void saveLibraryToDisk (int libIndex) const;
    void setParameterValue (const juce::String& paramID, float realValue);
    void resetParameterToDefault (const juce::String& paramID);

    SynthParams synthParams;
    juce::Synthesiser synth;

    // Efectos
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;
    juce::AudioBuffer<float> reverbScratch;
    juce::AudioBuffer<float> monoRight;
    bool reverbActive = false;
    SimpleChorus chorus;
    PingPongDelay delay;

    juce::SmoothedValue<float> driveMixSm { 0.0f };
    juce::SmoothedValue<float> driveAmtSm { 0.0f };
    juce::SmoothedValue<float> reverbMixSm { 0.0f };
    juce::SmoothedValue<float> masterSm { 0.7f };

    std::atomic<float> *drvOn, *drive, *choOn, *choRate, *choDepth, *choMix;
    std::atomic<float> *dlyOn, *dlyTime, *dlyFb, *dlyMix, *revOn, *revSize, *revMix, *master;

    std::atomic<int> currentPreset { 0 };
    juce::String customPresetName; // nombre del preset de libreria (vacio si se usa uno de fabrica)
    juce::String customLibName;    // libreria a la que pertenece
    std::vector<PresetLibrary> libraries;

    Waves::Table waveTableA, waveTableB;
    std::atomic<float> lastNoteHz { 0.0f };
    juce::Random rng;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PerreoTrapSynthAudioProcessor)
};
