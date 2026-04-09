#pragma once

#include "PluginProcessor.h"

class UtilityFiltersAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit UtilityFiltersAudioProcessorEditor (UtilityFiltersAudioProcessor&);
    ~UtilityFiltersAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void configureSlider (juce::Slider& slider, juce::Label& label, const juce::String& name);
    void updateControlVisibility();
    int getSelectedModeIndex() const;

    UtilityFiltersAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::Label subtitleLabel;

    juce::ComboBox modeBox;
    juce::Label modeLabel;
    juce::ComboBox ellipticResponseBox;
    juce::Label ellipticResponseLabel;

    juce::Slider mixSlider;
    juce::Slider outputSlider;
    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;
    juce::Slider feedbackSlider;
    juce::Slider delaySlider;
    juce::Slider ellipticTransitionSlider;
    juce::Slider ellipticRippleSlider;
    juce::Slider ellipticStopbandSlider;

    juce::Label mixLabel;
    juce::Label outputLabel;
    juce::Label cutoffLabel;
    juce::Label resonanceLabel;
    juce::Label feedbackLabel;
    juce::Label delayLabel;
    juce::Label ellipticTransitionLabel;
    juce::Label ellipticRippleLabel;
    juce::Label ellipticStopbandLabel;

    std::unique_ptr<ComboAttachment> modeAttachment;
    std::unique_ptr<ComboAttachment> ellipticResponseAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;
    std::unique_ptr<SliderAttachment> cutoffAttachment;
    std::unique_ptr<SliderAttachment> resonanceAttachment;
    std::unique_ptr<SliderAttachment> feedbackAttachment;
    std::unique_ptr<SliderAttachment> delayAttachment;
    std::unique_ptr<SliderAttachment> ellipticTransitionAttachment;
    std::unique_ptr<SliderAttachment> ellipticRippleAttachment;
    std::unique_ptr<SliderAttachment> ellipticStopbandAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilityFiltersAudioProcessorEditor)
};
