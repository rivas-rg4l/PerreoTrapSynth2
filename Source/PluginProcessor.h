#pragma once

#include <array>
#include <atomic>
#include <cmath>
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
// Definicion de un preset nativo
//==============================================================================
struct PerreoPreset
{
    const char* name;
    int         wave;       // 0 Sine, 1 Saw, 2 Square, 3 Triangle
    float       cutoff;     // Hz
    float       resonance;  // Q
    float       drive;      // 0..10
    float       reverb;     // 0..1
    float       master;     // 0..1
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
    explicit SynthVoice (juce::AudioProcessorValueTreeState& apvts);

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newPitchWheelValue) override;
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    void setCurrentPlaybackSampleRate (double newRate) override;
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    static float generateSample (int wave, float phase, float dt) noexcept;
    static float polyBlep (float t, float dt) noexcept;
    void updatePitchBend (int pitchWheelPosition) noexcept;
    void updateFilterCoefficients (float cutoffHz) noexcept;

    std::atomic<float>* waveParam      = nullptr;
    std::atomic<float>* cutoffParam    = nullptr;
    std::atomic<float>* resonanceParam = nullptr;
    std::atomic<float>* driveParam     = nullptr;

    juce::ADSR adsr;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> cutoffSmoothed { 8000.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>         driveSmoothed  { 0.0f };

    double sampleRateHz   = 44100.0;
    float  baseFrequency  = 440.0f;
    float  bendRatio      = 1.0f;
    float  level          = 0.0f;
    float  phase1         = 0.0f;
    float  phase2         = 0.0f;

    // Filtro SVF (TPT, Zavalishin) pasa-bajos
    float g  = 0.0f;
    float k  = 1.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
    float a3 = 0.0f;
    float ic1eq = 0.0f;
    float ic2eq = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthVoice)
};

//==============================================================================
// Procesador principal
//==============================================================================
class PerreoTrapSynthAudioProcessor : public juce::AudioProcessor
{
public:
    static constexpr int numPresets = 43;

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
    double getTailLengthSeconds() const override { return 3.0; }

    // Programas = presets
    int getNumPrograms() override { return numPresets; }
    int getCurrentProgram() override { return currentPreset.load(); }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static const std::array<PerreoPreset, numPresets>& getPresetTable();
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts;

private:
    void applyPreset (int index);
    void setParameterValue (const juce::String& paramID, float realValue);

    juce::Synthesiser synth;
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;

    std::atomic<float>* reverbParam = nullptr;
    std::atomic<float>* masterParam = nullptr;

    std::atomic<int> currentPreset { 0 };
    float lastMaster = 0.7f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PerreoTrapSynthAudioProcessor)
};
