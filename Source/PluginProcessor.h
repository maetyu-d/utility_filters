#pragma once

#include <JuceHeader.h>

class UtilityFiltersAudioProcessor final : public juce::AudioProcessor
{
public:
    UtilityFiltersAudioProcessor();
    ~UtilityFiltersAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    APVTS apvts;

    static APVTS::ParameterLayout createParameterLayout();

private:
    enum class FilterMode
    {
        comb = 0,
        allpass,
        elliptic,
        modal,
        bank
    };

    enum class EllipticResponse
    {
        lowpass = 0,
        highpass
    };

    enum class BankStyle
    {
        neutral = 0,
        wide,
        formant
    };

    struct DelayLine
    {
        void prepare (double newSampleRate, int maximumDelaySamples);
        void reset();
        float readInterpolated (float delayInSamples) const;
        void write (float sample) noexcept;

        double sampleRate = 44100.0;
        std::vector<float> buffer;
        int writeIndex = 0;
    };

    struct Biquad
    {
        void setCoefficients (float b0, float b1, float b2, float a1, float a2) noexcept;
        void reset() noexcept;
        float process (float input) noexcept;

        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
        float x1 = 0.0f, x2 = 0.0f;
        float y1 = 0.0f, y2 = 0.0f;
    };

    struct EllipticSettings
    {
        int response = 0;
        float cutoff = 0.0f;
        float transition = 0.0f;
        float rippleDb = 0.0f;
        float stopbandDb = 0.0f;
    };

    static float clampFrequency (float sampleRate, float frequency) noexcept;
    static Biquad makeBandPass (float sampleRate, float frequency, float q);
    static Biquad makeLowPass (float sampleRate, float frequency, float q);
    static Biquad makeNotch (float sampleRate, float frequency, float q);
    static Biquad makeBiquadFromCoefficients (const juce::dsp::IIR::Coefficients<float>& coefficients, bool highpassTransform);
    static bool approximatelyEqual (const EllipticSettings& a, const EllipticSettings& b) noexcept;

    void updateDspState();
    void updateEllipticFiltersIfNeeded();
    void resetProcessors();
    FilterMode getSelectedMode() const noexcept;
    EllipticResponse getEllipticResponse() const noexcept;

    std::array<DelayLine, 2> delays;
    std::array<std::vector<Biquad>, 2> ellipticFilters;
    std::array<std::vector<Biquad>, 2> modalFilters;
    std::array<std::vector<Biquad>, 2> filterBankFilters;

    juce::SmoothedValue<float> mixSmoother;
    juce::SmoothedValue<float> outputSmoother;

    double currentSampleRate = 44100.0;
    EllipticSettings lastEllipticSettings;
    int currentProgram = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilityFiltersAudioProcessor)
};
