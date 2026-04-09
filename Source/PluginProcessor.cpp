#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
constexpr int maxModes = 8;
constexpr int maxFilterBankBands = 16;
constexpr float pi = juce::MathConstants<float>::pi;

struct Preset
{
    const char* name;
    int mode;
    float mix;
    float output;
    float cutoff;
    float resonance;
    int ellipticResponse;
    float ellipticTransition;
    float ellipticRippleDb;
    float ellipticStopbandDb;
    float feedback;
    float delayMs;
    float decay;
    float spread;
    int modalCount;
    int bankCount;
    int bankStyle;
    float bankSpacing;
    float bankQ;
};

const std::array<Preset, 10> factoryPresets {{
    { "Comb Resonator", 0, 1.0f, -1.0f, 900.0f, 0.55f, 0, 0.025f, 0.5f, 72.0f, 0.82f, 12.0f, 0.55f, 1.0f, 4, 8, 0, 0.7f, 2.0f },
    { "Comb Flange", 0, 0.72f, -2.0f, 1400.0f, 0.45f, 0, 0.025f, 0.5f, 72.0f, 0.67f, 4.8f, 0.55f, 1.1f, 4, 8, 0, 0.7f, 2.0f },
    { "Diffusion Allpass", 1, 1.0f, -1.0f, 1200.0f, 0.5f, 0, 0.025f, 0.5f, 72.0f, 0.74f, 18.0f, 0.55f, 1.0f, 4, 8, 0, 0.7f, 2.0f },
    { "Elliptic Lowpass", 2, 1.0f, 0.0f, 2400.0f, 0.4f, 0, 0.018f, 0.35f, 84.0f, 0.65f, 18.0f, 0.6f, 1.0f, 4, 8, 0, 0.7f, 2.0f },
    { "Elliptic Highpass", 2, 1.0f, 0.0f, 1800.0f, 0.4f, 1, 0.02f, 0.45f, 90.0f, 0.65f, 18.0f, 0.6f, 1.0f, 4, 8, 0, 0.7f, 2.0f },
    { "Modal Chime", 3, 1.0f, 8.0f, 760.0f, 0.55f, 0, 0.025f, 0.5f, 72.0f, 0.65f, 18.0f, 0.97f, 1.95f, 8, 8, 0, 0.7f, 2.0f },
    { "Modal Drum Body", 3, 1.0f, 8.5f, 340.0f, 0.55f, 0, 0.025f, 0.5f, 72.0f, 0.65f, 18.0f, 0.94f, 1.05f, 8, 8, 0, 0.7f, 2.0f },
    { "Spectral Bank", 4, 1.0f, 10.0f, 2400.0f, 0.55f, 0, 0.025f, 0.5f, 72.0f, 0.65f, 18.0f, 0.6f, 1.45f, 4, 14, 0, 0.95f, 18.0f },
    { "Wide Smear Bank", 4, 1.0f, 9.0f, 1850.0f, 0.55f, 0, 0.025f, 0.5f, 72.0f, 0.65f, 18.0f, 0.6f, 2.35f, 4, 16, 1, 1.9f, 6.5f },
    { "Formant Bank", 4, 1.0f, 9.5f, 1350.0f, 0.55f, 0, 0.025f, 0.5f, 72.0f, 0.65f, 18.0f, 0.6f, 0.72f, 4, 10, 2, 0.52f, 18.0f }
}};

void setParameterValue (juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID, float value)
{
    if (auto* parameter = apvts.getParameter (parameterID))
        parameter->setValueNotifyingHost (parameter->convertTo0to1 (value));
}

void loadPreset (juce::AudioProcessorValueTreeState& apvts, const Preset& preset)
{
    setParameterValue (apvts, "mode", static_cast<float> (preset.mode));
    setParameterValue (apvts, "mix", preset.mix);
    setParameterValue (apvts, "output", preset.output);
    setParameterValue (apvts, "cutoff", preset.cutoff);
    setParameterValue (apvts, "resonance", preset.resonance);
    setParameterValue (apvts, "ellipticResponse", static_cast<float> (preset.ellipticResponse));
    setParameterValue (apvts, "ellipticTransition", preset.ellipticTransition);
    setParameterValue (apvts, "ellipticRippleDb", preset.ellipticRippleDb);
    setParameterValue (apvts, "ellipticStopbandDb", preset.ellipticStopbandDb);
    setParameterValue (apvts, "feedback", preset.feedback);
    setParameterValue (apvts, "delayMs", preset.delayMs);
    setParameterValue (apvts, "decay", preset.decay);
    setParameterValue (apvts, "spread", preset.spread);
    setParameterValue (apvts, "modalCount", static_cast<float> (preset.modalCount));
    setParameterValue (apvts, "bankCount", static_cast<float> (preset.bankCount));
    setParameterValue (apvts, "bankStyle", static_cast<float> (preset.bankStyle));
    setParameterValue (apvts, "bankSpacing", preset.bankSpacing);
    setParameterValue (apvts, "bankQ", preset.bankQ);
}
}

UtilityFiltersAudioProcessor::UtilityFiltersAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

UtilityFiltersAudioProcessor::~UtilityFiltersAudioProcessor() = default;

auto UtilityFiltersAudioProcessor::createParameterLayout() -> APVTS::ParameterLayout
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterChoice> ("mode", "Mode",
                                                                     juce::StringArray { "Comb", "Allpass", "Elliptic", "Modal", "Filter Bank" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("mix", "Mix", juce::NormalisableRange<float> (0.0f, 1.0f), 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("output", "Output", juce::NormalisableRange<float> (-24.0f, 24.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("cutoff", "Cutoff", juce::NormalisableRange<float> (20.0f, 18000.0f, 0.0f, 0.35f), 1200.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("resonance", "Resonance", juce::NormalisableRange<float> (0.1f, 1.0f), 0.55f));
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("ellipticResponse", "Elliptic Response",
                                                                     juce::StringArray { "Lowpass", "Highpass" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("ellipticTransition", "Elliptic Transition",
                                                                    juce::NormalisableRange<float> (0.002f, 0.12f, 0.0f, 0.45f), 0.025f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("ellipticRippleDb", "Elliptic Ripple",
                                                                    juce::NormalisableRange<float> (0.1f, 3.0f, 0.0f, 0.4f), 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("ellipticStopbandDb", "Elliptic Stopband",
                                                                    juce::NormalisableRange<float> (24.0f, 120.0f, 0.0f, 0.45f), 72.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("feedback", "Feedback", juce::NormalisableRange<float> (-0.97f, 0.97f), 0.65f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("delayMs", "Delay", juce::NormalisableRange<float> (1.0f, 100.0f), 18.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("decay", "Decay", juce::NormalisableRange<float> (0.1f, 0.99f), 0.6f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("spread", "Spread", juce::NormalisableRange<float> (0.1f, 4.0f), 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterInt> ("modalCount", "Modal Count", 1, maxModes, 4));
    params.push_back (std::make_unique<juce::AudioParameterInt> ("bankCount", "Bank Count", 2, maxFilterBankBands, 8));
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("bankStyle", "Bank Style",
                                                                     juce::StringArray { "Neutral", "Wide", "Formant" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("bankSpacing", "Bank Spacing", juce::NormalisableRange<float> (0.25f, 2.5f), 0.7f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("bankQ", "Bank Q", juce::NormalisableRange<float> (0.2f, 18.0f, 0.0f, 0.35f), 2.0f));

    return { params.begin(), params.end() };
}

const juce::String UtilityFiltersAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UtilityFiltersAudioProcessor::acceptsMidi() const { return false; }
bool UtilityFiltersAudioProcessor::producesMidi() const { return false; }
bool UtilityFiltersAudioProcessor::isMidiEffect() const { return false; }
double UtilityFiltersAudioProcessor::getTailLengthSeconds() const { return 2.0; }

int UtilityFiltersAudioProcessor::getNumPrograms() { return static_cast<int> (factoryPresets.size()); }
int UtilityFiltersAudioProcessor::getCurrentProgram() { return currentProgram; }
void UtilityFiltersAudioProcessor::setCurrentProgram (int index)
{
    auto clampedIndex = juce::jlimit (0, getNumPrograms() - 1, index);
    currentProgram = clampedIndex;

    loadPreset (apvts, factoryPresets[static_cast<size_t> (clampedIndex)]);
    resetProcessors();
    lastEllipticSettings = {};
    updateDspState();
}

const juce::String UtilityFiltersAudioProcessor::getProgramName (int index)
{
    if (! juce::isPositiveAndBelow (index, getNumPrograms()))
        return {};

    return factoryPresets[static_cast<size_t> (index)].name;
}

void UtilityFiltersAudioProcessor::changeProgramName (int, const juce::String&) {}

void UtilityFiltersAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;

    for (auto& delay : delays)
        delay.prepare (sampleRate, static_cast<int> (sampleRate * 0.12));

    modalFilters[0].resize (maxModes);
    modalFilters[1].resize (maxModes);
    filterBankFilters[0].resize (maxFilterBankBands);
    filterBankFilters[1].resize (maxFilterBankBands);
    ellipticFilters[0].clear();
    ellipticFilters[1].clear();

    mixSmoother.reset (sampleRate, 0.02);
    outputSmoother.reset (sampleRate, 0.02);

    lastEllipticSettings = {};

    resetProcessors();
    updateDspState();
}

void UtilityFiltersAudioProcessor::releaseResources() {}

bool UtilityFiltersAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;

    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void UtilityFiltersAudioProcessor::updateDspState()
{
    auto cutoff = apvts.getRawParameterValue ("cutoff")->load();
    auto spread = apvts.getRawParameterValue ("spread")->load();
    auto bankStyle = static_cast<BankStyle> (apvts.getRawParameterValue ("bankStyle")->load());
    auto bankSpacing = apvts.getRawParameterValue ("bankSpacing")->load();
    auto bankQ = apvts.getRawParameterValue ("bankQ")->load();
    auto decay = apvts.getRawParameterValue ("decay")->load();

    mixSmoother.setTargetValue (apvts.getRawParameterValue ("mix")->load());
    outputSmoother.setTargetValue (juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("output")->load()));
    updateEllipticFiltersIfNeeded();

    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < maxModes; ++i)
        {
            auto partial = static_cast<float> (i + 1);
            auto modalFrequency = cutoff * std::pow (1.0f + (spread * 0.22f), partial);
            auto modeQ = juce::jmap (decay, 0.1f, 0.99f, 0.8f, 25.0f) / partial;
            modalFilters[static_cast<size_t> (ch)][static_cast<size_t> (i)] = makeBandPass (static_cast<float> (currentSampleRate), modalFrequency, juce::jmax (0.35f, modeQ));
        }

        for (int i = 0; i < maxFilterBankBands; ++i)
        {
            auto centered = static_cast<float> (i) - (static_cast<float> (maxFilterBankBands) - 1.0f) * 0.5f;
            auto styleSpacing = bankSpacing;
            auto styleQ = bankQ;
            auto frequencySkew = 0.0f;

            switch (bankStyle)
            {
                case BankStyle::neutral:
                    break;

                case BankStyle::wide:
                    styleSpacing *= 1.45f;
                    styleQ = juce::jmax (0.25f, bankQ * 0.55f);
                    frequencySkew = 0.08f * centered * centered;
                    break;

                case BankStyle::formant:
                    styleSpacing *= 0.62f;
                    styleQ = bankQ * 1.5f;
                    frequencySkew = -0.035f * centered * centered;
                    break;
            }

            auto multiplier = std::pow (2.0f, centered * styleSpacing * 0.33f) * (1.0f + frequencySkew);
            filterBankFilters[static_cast<size_t> (ch)][static_cast<size_t> (i)] = makeBandPass (static_cast<float> (currentSampleRate),
                                                                                                   cutoff * multiplier,
                                                                                                   styleQ);
        }
    }
}

void UtilityFiltersAudioProcessor::resetProcessors()
{
    for (auto& delay : delays)
        delay.reset();

    for (auto& channel : ellipticFilters)
        for (auto& biquad : channel)
            biquad.reset();

    for (auto& channel : modalFilters)
        for (auto& biquad : channel)
            biquad.reset();

    for (auto& channel : filterBankFilters)
        for (auto& biquad : channel)
            biquad.reset();
}

auto UtilityFiltersAudioProcessor::getSelectedMode() const noexcept -> FilterMode
{
    return static_cast<FilterMode> (apvts.getRawParameterValue ("mode")->load());
}

auto UtilityFiltersAudioProcessor::getEllipticResponse() const noexcept -> EllipticResponse
{
    return static_cast<EllipticResponse> (apvts.getRawParameterValue ("ellipticResponse")->load());
}

bool UtilityFiltersAudioProcessor::approximatelyEqual (const EllipticSettings& a, const EllipticSettings& b) noexcept
{
    constexpr float tolerance = 1.0e-5f;

    return a.response == b.response
        && std::abs (a.cutoff - b.cutoff) < tolerance
        && std::abs (a.transition - b.transition) < tolerance
        && std::abs (a.rippleDb - b.rippleDb) < tolerance
        && std::abs (a.stopbandDb - b.stopbandDb) < tolerance;
}

auto UtilityFiltersAudioProcessor::makeBiquadFromCoefficients (const juce::dsp::IIR::Coefficients<float>& coefficients,
                                                               bool highpassTransform) -> Biquad
{
    auto* raw = coefficients.getRawCoefficients();
    auto count = coefficients.coefficients.size();

    auto b0 = raw[0];
    auto b1 = raw[1];
    auto b2 = count > 4 ? raw[2] : 0.0f;
    auto a0 = count > 4 ? raw[3] : raw[2];
    auto a1 = count > 4 ? raw[4] : raw[3];
    auto a2 = count > 4 ? raw[5] : 0.0f;

    if (highpassTransform)
    {
        b1 = -b1;
        a1 = -a1;
    }

    Biquad biquad;
    biquad.setCoefficients (b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
    return biquad;
}

void UtilityFiltersAudioProcessor::updateEllipticFiltersIfNeeded()
{
    auto cutoff = clampFrequency (static_cast<float> (currentSampleRate), apvts.getRawParameterValue ("cutoff")->load());
    auto response = static_cast<int> (getEllipticResponse());
    auto transition = apvts.getRawParameterValue ("ellipticTransition")->load();
    auto rippleDb = apvts.getRawParameterValue ("ellipticRippleDb")->load();
    auto stopbandDb = apvts.getRawParameterValue ("ellipticStopbandDb")->load();

    auto normalisedCutoff = cutoff / static_cast<float> (currentSampleRate);
    auto maxTransition = juce::jmax (0.002f, (juce::jmin (normalisedCutoff, 0.5f - normalisedCutoff) * 1.8f));
    transition = juce::jlimit (0.002f, maxTransition, transition);

    EllipticSettings currentSettings { response, cutoff, transition, rippleDb, stopbandDb };

    if (approximatelyEqual (currentSettings, lastEllipticSettings))
        return;

    auto designFrequency = cutoff;
    auto highpassTransform = false;

    if (getEllipticResponse() == EllipticResponse::highpass)
    {
        designFrequency = clampFrequency (static_cast<float> (currentSampleRate), static_cast<float> (currentSampleRate * 0.5) - cutoff);
        auto mirroredNormalised = designFrequency / static_cast<float> (currentSampleRate);
        auto mirroredMaxTransition = juce::jmax (0.002f, (juce::jmin (mirroredNormalised, 0.5f - mirroredNormalised) * 1.8f));
        transition = juce::jlimit (0.002f, mirroredMaxTransition, transition);
        highpassTransform = true;
    }

    auto coeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderEllipticMethod (designFrequency,
                                                                                             currentSampleRate,
                                                                                             transition,
                                                                                             -rippleDb,
                                                                                             -stopbandDb);

    for (auto& channel : ellipticFilters)
    {
        channel.clear();
        channel.reserve (static_cast<size_t> (coeffs.size()));

        for (auto* coeff : coeffs)
            channel.push_back (makeBiquadFromCoefficients (*coeff, highpassTransform));
    }

    lastEllipticSettings = currentSettings;
}

void UtilityFiltersAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalInputChannels = getTotalNumInputChannels();
    auto totalOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalInputChannels; i < totalOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateDspState();

    auto feedback = apvts.getRawParameterValue ("feedback")->load();
    auto delayMs = apvts.getRawParameterValue ("delayMs")->load();
    auto delaySamples = delayMs * 0.001f * static_cast<float> (currentSampleRate);
    auto decay = apvts.getRawParameterValue ("decay")->load();
    auto modalCount = static_cast<int> (apvts.getRawParameterValue ("modalCount")->load());
    auto bankCount = static_cast<int> (apvts.getRawParameterValue ("bankCount")->load());
    auto bankStyle = static_cast<BankStyle> (apvts.getRawParameterValue ("bankStyle")->load());

    for (int channel = 0; channel < totalInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto& delay = delays[static_cast<size_t> (channel)];

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            auto dry = channelData[sample];
            auto wet = dry;

            switch (getSelectedMode())
            {
                case FilterMode::comb:
                {
                    auto delayed = delay.readInterpolated (delaySamples);
                    wet = dry + delayed;
                    delay.write (dry + (delayed * feedback));
                    break;
                }

                case FilterMode::allpass:
                {
                    auto delayed = delay.readInterpolated (delaySamples);
                    wet = (-feedback * dry) + delayed;
                    delay.write (dry + (wet * feedback));
                    break;
                }

                case FilterMode::elliptic:
                {
                    wet = dry;

                    for (auto& biquad : ellipticFilters[static_cast<size_t> (channel)])
                        wet = biquad.process (wet);

                    break;
                }

                case FilterMode::modal:
                {
                    wet = 0.0f;
                    for (int i = 0; i < modalCount; ++i)
                    {
                        auto partialIndex = static_cast<float> (i);
                        auto modeGain = std::pow (decay, partialIndex * 0.22f) / (1.0f + (partialIndex * 0.18f));
                        wet += modalFilters[static_cast<size_t> (channel)][static_cast<size_t> (i)].process (dry) * modeGain;
                    }
                    wet *= 2.6f;
                    break;
                }

                case FilterMode::bank:
                {
                    wet = 0.0f;
                    auto startIndex = (maxFilterBankBands - bankCount) / 2;
                    auto endIndex = startIndex + bankCount;

                    for (int i = startIndex; i < endIndex; ++i)
                        wet += filterBankFilters[static_cast<size_t> (channel)][static_cast<size_t> (i)].process (dry);

                    auto bankGain = juce::jmap (static_cast<float> (bankCount), 2.0f, static_cast<float> (maxFilterBankBands), 0.75f, 1.15f);

                    if (bankStyle == BankStyle::wide)
                        bankGain *= 1.18f;
                    else if (bankStyle == BankStyle::formant)
                        bankGain *= 1.08f;

                    wet *= bankGain;
                    break;
                }
            }

            auto mix = mixSmoother.getNextValue();
            auto output = outputSmoother.getNextValue();
            channelData[sample] = ((dry * (1.0f - mix)) + (wet * mix)) * output;
        }
    }
}

bool UtilityFiltersAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* UtilityFiltersAudioProcessor::createEditor()
{
    return new UtilityFiltersAudioProcessorEditor (*this);
}

void UtilityFiltersAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void UtilityFiltersAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

void UtilityFiltersAudioProcessor::DelayLine::prepare (double newSampleRate, int maximumDelaySamples)
{
    sampleRate = newSampleRate;
    buffer.assign (static_cast<size_t> (maximumDelaySamples + 2), 0.0f);
    writeIndex = 0;
}

void UtilityFiltersAudioProcessor::DelayLine::reset()
{
    std::fill (buffer.begin(), buffer.end(), 0.0f);
    writeIndex = 0;
}

float UtilityFiltersAudioProcessor::DelayLine::readInterpolated (float delayInSamples) const
{
    if (buffer.empty())
        return 0.0f;

    auto size = static_cast<int> (buffer.size());
    auto readPosition = static_cast<float> (writeIndex) - delayInSamples;

    while (readPosition < 0.0f)
        readPosition += static_cast<float> (size);

    auto indexA = static_cast<int> (readPosition) % size;
    auto indexB = (indexA + 1) % size;
    auto frac = readPosition - static_cast<float> (indexA);

    return juce::jmap (frac, buffer[static_cast<size_t> (indexA)], buffer[static_cast<size_t> (indexB)]);
}

void UtilityFiltersAudioProcessor::DelayLine::write (float sample) noexcept
{
    if (buffer.empty())
        return;

    buffer[static_cast<size_t> (writeIndex)] = sample;
    writeIndex = (writeIndex + 1) % static_cast<int> (buffer.size());
}

void UtilityFiltersAudioProcessor::Biquad::setCoefficients (float newB0, float newB1, float newB2, float newA1, float newA2) noexcept
{
    b0 = newB0;
    b1 = newB1;
    b2 = newB2;
    a1 = newA1;
    a2 = newA2;
}

void UtilityFiltersAudioProcessor::Biquad::reset() noexcept
{
    x1 = x2 = y1 = y2 = 0.0f;
}

float UtilityFiltersAudioProcessor::Biquad::process (float input) noexcept
{
    auto output = (b0 * input) + (b1 * x1) + (b2 * x2) - (a1 * y1) - (a2 * y2);
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;
    return output;
}

float UtilityFiltersAudioProcessor::clampFrequency (float sampleRate, float frequency) noexcept
{
    return juce::jlimit (20.0f, (sampleRate * 0.5f) - 200.0f, frequency);
}

auto UtilityFiltersAudioProcessor::makeBandPass (float sampleRate, float frequency, float q) -> Biquad
{
    Biquad biquad;
    frequency = clampFrequency (sampleRate, frequency);
    q = juce::jmax (0.1f, q);

    auto w0 = 2.0f * pi * frequency / sampleRate;
    auto alpha = std::sin (w0) / (2.0f * q);
    auto cosw0 = std::cos (w0);
    auto a0 = 1.0f + alpha;

    biquad.setCoefficients (alpha / a0,
                            0.0f,
                            (-alpha) / a0,
                            (-2.0f * cosw0) / a0,
                            (1.0f - alpha) / a0);
    return biquad;
}

auto UtilityFiltersAudioProcessor::makeLowPass (float sampleRate, float frequency, float q) -> Biquad
{
    Biquad biquad;
    frequency = clampFrequency (sampleRate, frequency);
    q = juce::jmax (0.1f, q);

    auto w0 = 2.0f * pi * frequency / sampleRate;
    auto alpha = std::sin (w0) / (2.0f * q);
    auto cosw0 = std::cos (w0);
    auto a0 = 1.0f + alpha;

    biquad.setCoefficients (((1.0f - cosw0) * 0.5f) / a0,
                            (1.0f - cosw0) / a0,
                            ((1.0f - cosw0) * 0.5f) / a0,
                            (-2.0f * cosw0) / a0,
                            (1.0f - alpha) / a0);
    return biquad;
}

auto UtilityFiltersAudioProcessor::makeNotch (float sampleRate, float frequency, float q) -> Biquad
{
    Biquad biquad;
    frequency = clampFrequency (sampleRate, frequency);
    q = juce::jmax (0.1f, q);

    auto w0 = 2.0f * pi * frequency / sampleRate;
    auto alpha = std::sin (w0) / (2.0f * q);
    auto cosw0 = std::cos (w0);
    auto a0 = 1.0f + alpha;

    biquad.setCoefficients (1.0f / a0,
                            (-2.0f * cosw0) / a0,
                            1.0f / a0,
                            (-2.0f * cosw0) / a0,
                            (1.0f - alpha) / a0);
    return biquad;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UtilityFiltersAudioProcessor();
}
