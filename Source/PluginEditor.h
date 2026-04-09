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

    enum class Page
    {
        core = 0,
        resonators
    };

    void configureSlider (juce::Slider& slider, juce::Label& label, const juce::String& name);
    void configureTabButton (juce::TextButton& button, const juce::String& title);
    void setPage (Page newPage);
    void updatePageVisibility();
    void updateModePresentation();
    int getSelectedModeIndex() const;
    void drawPanel (juce::Graphics& g, juce::Rectangle<float> bounds, const juce::String& title, const juce::String& subtitle);

    UtilityFiltersAudioProcessor& audioProcessor;
    Page currentPage = Page::core;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::TextButton corePageButton;
    juce::TextButton resonatorsPageButton;

    juce::ComboBox modeBox;
    juce::Label modeLabel;
    juce::ComboBox ellipticResponseBox;
    juce::Label ellipticResponseLabel;
    juce::ComboBox bankStyleBox;
    juce::Label bankStyleLabel;

    juce::Slider mixSlider;
    juce::Slider outputSlider;
    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;
    juce::Slider ellipticTransitionSlider;
    juce::Slider ellipticRippleSlider;
    juce::Slider ellipticStopbandSlider;
    juce::Slider feedbackSlider;
    juce::Slider delaySlider;
    juce::Slider decaySlider;
    juce::Slider spreadSlider;
    juce::Slider modalCountSlider;
    juce::Slider bankCountSlider;
    juce::Slider bankSpacingSlider;
    juce::Slider bankQSlider;

    juce::Label mixLabel;
    juce::Label outputLabel;
    juce::Label cutoffLabel;
    juce::Label resonanceLabel;
    juce::Label ellipticTransitionLabel;
    juce::Label ellipticRippleLabel;
    juce::Label ellipticStopbandLabel;
    juce::Label feedbackLabel;
    juce::Label delayLabel;
    juce::Label decayLabel;
    juce::Label spreadLabel;
    juce::Label modalCountLabel;
    juce::Label bankCountLabel;
    juce::Label bankSpacingLabel;
    juce::Label bankQLabel;

    std::unique_ptr<ComboAttachment> modeAttachment;
    std::unique_ptr<ComboAttachment> ellipticResponseAttachment;
    std::unique_ptr<ComboAttachment> bankStyleAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;
    std::unique_ptr<SliderAttachment> cutoffAttachment;
    std::unique_ptr<SliderAttachment> resonanceAttachment;
    std::unique_ptr<SliderAttachment> ellipticTransitionAttachment;
    std::unique_ptr<SliderAttachment> ellipticRippleAttachment;
    std::unique_ptr<SliderAttachment> ellipticStopbandAttachment;
    std::unique_ptr<SliderAttachment> feedbackAttachment;
    std::unique_ptr<SliderAttachment> delayAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> spreadAttachment;
    std::unique_ptr<SliderAttachment> modalCountAttachment;
    std::unique_ptr<SliderAttachment> bankCountAttachment;
    std::unique_ptr<SliderAttachment> bankSpacingAttachment;
    std::unique_ptr<SliderAttachment> bankQAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilityFiltersAudioProcessorEditor)
};
