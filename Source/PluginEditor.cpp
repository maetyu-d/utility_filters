#include "PluginEditor.h"

UtilityFiltersAudioProcessorEditor::UtilityFiltersAudioProcessorEditor (UtilityFiltersAudioProcessor& pluginProcessor)
    : AudioProcessorEditor (&pluginProcessor), audioProcessor (pluginProcessor)
{
    titleLabel.setText ("Utility Filters", juce::dontSendNotification);
    titleLabel.setFont (juce::FontOptions (18.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("", juce::dontSendNotification);
    subtitleLabel.setColour (juce::Label::textColourId, juce::Colours::transparentBlack);
    subtitleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (subtitleLabel);

    modeLabel.setText ("Mode", juce::dontSendNotification);
    addAndMakeVisible (modeLabel);
    modeBox.addItemList ({ "Comb", "Allpass", "Elliptic" }, 1);
    addAndMakeVisible (modeBox);

    ellipticResponseLabel.setText ("Elliptic", juce::dontSendNotification);
    addAndMakeVisible (ellipticResponseLabel);
    ellipticResponseBox.addItemList ({ "Lowpass", "Highpass" }, 1);
    addAndMakeVisible (ellipticResponseBox);

    configureSlider (mixSlider, mixLabel, "Mix");
    configureSlider (outputSlider, outputLabel, "Output");
    configureSlider (cutoffSlider, cutoffLabel, "Cutoff");
    configureSlider (resonanceSlider, resonanceLabel, "Focus");
    configureSlider (feedbackSlider, feedbackLabel, "Feedback");
    configureSlider (delaySlider, delayLabel, "Delay");
    configureSlider (ellipticTransitionSlider, ellipticTransitionLabel, "Transition");
    configureSlider (ellipticRippleSlider, ellipticRippleLabel, "Ripple dB");
    configureSlider (ellipticStopbandSlider, ellipticStopbandLabel, "Stopband dB");

    modeAttachment = std::make_unique<ComboAttachment> (audioProcessor.apvts, "mode", modeBox);
    ellipticResponseAttachment = std::make_unique<ComboAttachment> (audioProcessor.apvts, "ellipticResponse", ellipticResponseBox);
    mixAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "mix", mixSlider);
    outputAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "output", outputSlider);
    cutoffAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "cutoff", cutoffSlider);
    resonanceAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "resonance", resonanceSlider);
    feedbackAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "feedback", feedbackSlider);
    delayAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "delayMs", delaySlider);
    ellipticTransitionAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "ellipticTransition", ellipticTransitionSlider);
    ellipticRippleAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "ellipticRippleDb", ellipticRippleSlider);
    ellipticStopbandAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "ellipticStopbandDb", ellipticStopbandSlider);

    modeBox.onChange = [this] { updateControlVisibility(); };

    setSize (760, 340);
    updateControlVisibility();
}

UtilityFiltersAudioProcessorEditor::~UtilityFiltersAudioProcessorEditor() = default;

void UtilityFiltersAudioProcessorEditor::configureSlider (juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 18);
    slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB (214, 178, 120));
    slider.setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (236, 232, 224));
    slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGBA (255, 255, 255, 16));
    addAndMakeVisible (slider);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour::fromRGBA (255, 255, 255, 150));
    label.setFont (juce::FontOptions (13.0f));
    addAndMakeVisible (label);
}

int UtilityFiltersAudioProcessorEditor::getSelectedModeIndex() const
{
    return juce::jmax (0, modeBox.getSelectedItemIndex());
}

void UtilityFiltersAudioProcessorEditor::updateControlVisibility()
{
    const auto isElliptic = getSelectedModeIndex() == 2;

    ellipticResponseLabel.setVisible (isElliptic);
    ellipticResponseBox.setVisible (isElliptic);
    ellipticTransitionLabel.setVisible (isElliptic);
    ellipticTransitionSlider.setVisible (isElliptic);
    ellipticRippleLabel.setVisible (isElliptic);
    ellipticRippleSlider.setVisible (isElliptic);
    ellipticStopbandLabel.setVisible (isElliptic);
    ellipticStopbandSlider.setVisible (isElliptic);

    feedbackLabel.setVisible (! isElliptic);
    feedbackSlider.setVisible (! isElliptic);
    delayLabel.setVisible (! isElliptic);
    delaySlider.setVisible (! isElliptic);

    resized();
    repaint();
}

void UtilityFiltersAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.fillAll (juce::Colour::fromRGB (20, 21, 24));

    g.setColour (juce::Colour::fromRGBA (255, 255, 255, 12));
    g.drawLine (20.0f, 48.0f, bounds.getWidth() - 20.0f, 48.0f, 1.0f);
}

void UtilityFiltersAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20, 14);
    auto header = area.removeFromTop (34);

    titleLabel.setBounds (header.removeFromTop (22));
    subtitleLabel.setBounds (0, 0, 0, 0);

    auto selectorRow = area.removeFromTop (34);
    modeLabel.setBounds (selectorRow.removeFromLeft (44));
    modeBox.setBounds (selectorRow.removeFromLeft (170).reduced (0, 4));
    ellipticResponseLabel.setBounds (selectorRow.removeFromLeft (56));
    ellipticResponseBox.setBounds (selectorRow.removeFromLeft (150).reduced (0, 4));

    auto grid = area.reduced (0, 4);
    auto topRow = grid.removeFromTop (126);
    auto bottomRow = grid.removeFromTop (126);

    auto placeKnob = [] (juce::Rectangle<int> slot, juce::Slider& slider, juce::Label& label)
    {
        label.setBounds (slot.removeFromTop (16));
        slider.setBounds (slot.reduced (10));
    };

    const std::array<std::pair<juce::Slider*, juce::Label*>, 5> topControls {{
        { &mixSlider, &mixLabel },
        { &outputSlider, &outputLabel },
        { &cutoffSlider, &cutoffLabel },
        { &resonanceSlider, &resonanceLabel },
        { &feedbackSlider, &feedbackLabel }
    }};

    const std::array<std::pair<juce::Slider*, juce::Label*>, 4> bottomControls {{
        { &delaySlider, &delayLabel },
        { &ellipticTransitionSlider, &ellipticTransitionLabel },
        { &ellipticRippleSlider, &ellipticRippleLabel },
        { &ellipticStopbandSlider, &ellipticStopbandLabel }
    }};

    for (auto [slider, label] : topControls)
        placeKnob (topRow.removeFromLeft (grid.getWidth() / static_cast<int> (topControls.size())), *slider, *label);

    for (auto [slider, label] : bottomControls)
        placeKnob (bottomRow.removeFromLeft (grid.getWidth() / static_cast<int> (bottomControls.size())), *slider, *label);
}
