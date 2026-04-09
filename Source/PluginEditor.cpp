#include "PluginEditor.h"

UtilityFiltersAudioProcessorEditor::UtilityFiltersAudioProcessorEditor (UtilityFiltersAudioProcessor& pluginProcessor)
    : AudioProcessorEditor (&pluginProcessor), audioProcessor (pluginProcessor)
{
    titleLabel.setText ("Utility Filters", juce::dontSendNotification);
    titleLabel.setFont (juce::FontOptions (28.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("Comb, allpass, elliptic, modal, and filter-bank processing in one utility effect.",
                           juce::dontSendNotification);
    subtitleLabel.setColour (juce::Label::textColourId, juce::Colours::lightgrey);
    subtitleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (subtitleLabel);

    configureTabButton (corePageButton, "Core Filters");
    configureTabButton (resonatorsPageButton, "Resonators");
    corePageButton.onClick = [this] { setPage (Page::core); };
    resonatorsPageButton.onClick = [this] { setPage (Page::resonators); };

    modeLabel.setText ("Mode", juce::dontSendNotification);
    addAndMakeVisible (modeLabel);
    modeBox.addItemList ({ "Comb", "Allpass", "Elliptic", "Modal", "Filter Bank" }, 1);
    addAndMakeVisible (modeBox);
    ellipticResponseLabel.setText ("Elliptic", juce::dontSendNotification);
    addAndMakeVisible (ellipticResponseLabel);
    ellipticResponseBox.addItemList ({ "Lowpass", "Highpass" }, 1);
    addAndMakeVisible (ellipticResponseBox);
    bankStyleLabel.setText ("Bank Style", juce::dontSendNotification);
    addAndMakeVisible (bankStyleLabel);
    bankStyleBox.addItemList ({ "Neutral", "Wide", "Formant" }, 1);
    addAndMakeVisible (bankStyleBox);

    configureSlider (mixSlider, mixLabel, "Mix");
    configureSlider (outputSlider, outputLabel, "Output");
    configureSlider (cutoffSlider, cutoffLabel, "Cutoff");
    configureSlider (resonanceSlider, resonanceLabel, "Resonance");
    configureSlider (ellipticTransitionSlider, ellipticTransitionLabel, "Transition");
    configureSlider (ellipticRippleSlider, ellipticRippleLabel, "Ripple dB");
    configureSlider (ellipticStopbandSlider, ellipticStopbandLabel, "Stopband dB");
    configureSlider (feedbackSlider, feedbackLabel, "Feedback");
    configureSlider (delaySlider, delayLabel, "Delay");
    configureSlider (decaySlider, decayLabel, "Decay");
    configureSlider (spreadSlider, spreadLabel, "Spread");
    configureSlider (modalCountSlider, modalCountLabel, "Modes");
    configureSlider (bankCountSlider, bankCountLabel, "Bank Count");
    configureSlider (bankSpacingSlider, bankSpacingLabel, "Spacing");
    configureSlider (bankQSlider, bankQLabel, "Bank Q");

    modeAttachment = std::make_unique<ComboAttachment> (audioProcessor.apvts, "mode", modeBox);
    ellipticResponseAttachment = std::make_unique<ComboAttachment> (audioProcessor.apvts, "ellipticResponse", ellipticResponseBox);
    bankStyleAttachment = std::make_unique<ComboAttachment> (audioProcessor.apvts, "bankStyle", bankStyleBox);
    mixAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "mix", mixSlider);
    outputAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "output", outputSlider);
    cutoffAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "cutoff", cutoffSlider);
    resonanceAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "resonance", resonanceSlider);
    ellipticTransitionAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "ellipticTransition", ellipticTransitionSlider);
    ellipticRippleAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "ellipticRippleDb", ellipticRippleSlider);
    ellipticStopbandAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "ellipticStopbandDb", ellipticStopbandSlider);
    feedbackAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "feedback", feedbackSlider);
    delayAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "delayMs", delaySlider);
    decayAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "decay", decaySlider);
    spreadAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "spread", spreadSlider);
    modalCountAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "modalCount", modalCountSlider);
    bankCountAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "bankCount", bankCountSlider);
    bankSpacingAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "bankSpacing", bankSpacingSlider);
    bankQAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "bankQ", bankQSlider);
    modeBox.onChange = [this] { updateModePresentation(); };

    setSize (920, 620);
    updateModePresentation();
}

UtilityFiltersAudioProcessorEditor::~UtilityFiltersAudioProcessorEditor() = default;

void UtilityFiltersAudioProcessorEditor::configureSlider (juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB (248, 180, 83));
    slider.setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (255, 231, 191));
    addAndMakeVisible (slider);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (label);
}

void UtilityFiltersAudioProcessorEditor::configureTabButton (juce::TextButton& button, const juce::String& title)
{
    button.setButtonText (title);
    button.setClickingTogglesState (false);
    button.setColour (juce::TextButton::buttonOnColourId, juce::Colour::fromRGB (248, 180, 83));
    button.setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGBA (255, 255, 255, 24));
    button.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible (button);
}

void UtilityFiltersAudioProcessorEditor::setPage (Page newPage)
{
    currentPage = newPage;
    updatePageVisibility();
    resized();
    repaint();
}

void UtilityFiltersAudioProcessorEditor::updatePageVisibility()
{
    const auto showCore = currentPage == Page::core;
    const auto showResonators = currentPage == Page::resonators;
    const auto modeIndex = getSelectedModeIndex();
    const auto isCombLike = modeIndex == 0 || modeIndex == 1;
    const auto isElliptic = modeIndex == 2;
    const auto isModal = modeIndex == 3;
    const auto isBank = modeIndex == 4;

    corePageButton.setColour (juce::TextButton::buttonColourId,
                              showCore ? juce::Colour::fromRGB (248, 180, 83)
                                       : juce::Colour::fromRGBA (255, 255, 255, 24));
    resonatorsPageButton.setColour (juce::TextButton::buttonColourId,
                                    showResonators ? juce::Colour::fromRGB (248, 180, 83)
                                                   : juce::Colour::fromRGBA (255, 255, 255, 24));

    modeLabel.setVisible (showCore);
    modeBox.setVisible (showCore);
    ellipticResponseLabel.setVisible (showCore);
    ellipticResponseBox.setVisible (showCore);

    bankStyleLabel.setVisible (showResonators);
    bankStyleBox.setVisible (showResonators);

    mixLabel.setVisible (showCore || showResonators);
    mixSlider.setVisible (showCore || showResonators);
    outputLabel.setVisible (showCore || showResonators);
    outputSlider.setVisible (showCore || showResonators);
    cutoffLabel.setVisible ((showCore && isElliptic) || (showResonators && (isModal || isBank)));
    cutoffSlider.setVisible ((showCore && isElliptic) || (showResonators && (isModal || isBank)));
    resonanceLabel.setVisible ((showCore && isElliptic) || showResonators);
    resonanceSlider.setVisible ((showCore && isElliptic) || showResonators);
    ellipticTransitionLabel.setVisible (showCore && isElliptic);
    ellipticTransitionSlider.setVisible (showCore && isElliptic);
    ellipticRippleLabel.setVisible (showCore && isElliptic);
    ellipticRippleSlider.setVisible (showCore && isElliptic);
    ellipticStopbandLabel.setVisible (showCore && isElliptic);
    ellipticStopbandSlider.setVisible (showCore && isElliptic);
    feedbackLabel.setVisible (showCore && isCombLike);
    feedbackSlider.setVisible (showCore && isCombLike);
    delayLabel.setVisible (showCore && isCombLike);
    delaySlider.setVisible (showCore && isCombLike);

    decayLabel.setVisible (showResonators && isModal);
    decaySlider.setVisible (showResonators && isModal);
    spreadLabel.setVisible (showResonators && (isModal || isBank));
    spreadSlider.setVisible (showResonators && (isModal || isBank));
    modalCountLabel.setVisible (showResonators && isModal);
    modalCountSlider.setVisible (showResonators && isModal);
    bankCountLabel.setVisible (showResonators && isBank);
    bankCountSlider.setVisible (showResonators && isBank);
    bankSpacingLabel.setVisible (showResonators && isBank);
    bankSpacingSlider.setVisible (showResonators && isBank);
    bankQLabel.setVisible (showResonators && isBank);
    bankQSlider.setVisible (showResonators && isBank);
}

int UtilityFiltersAudioProcessorEditor::getSelectedModeIndex() const
{
    return juce::jmax (0, modeBox.getSelectedItemIndex());
}

void UtilityFiltersAudioProcessorEditor::updateModePresentation()
{
    setPage (getSelectedModeIndex() <= 2 ? Page::core : Page::resonators);
}

void UtilityFiltersAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient gradient (juce::Colour::fromRGB (19, 24, 31), bounds.getTopLeft(),
                                   juce::Colour::fromRGB (47, 60, 76), bounds.getBottomRight(), false);
    g.setGradientFill (gradient);
    g.fillAll();

    g.setColour (juce::Colour::fromRGB (255, 255, 255).withAlpha (0.08f));
    g.fillRoundedRectangle (20.0f, 20.0f, bounds.getWidth() - 40.0f, bounds.getHeight() - 40.0f, 24.0f);

    g.setColour (juce::Colour::fromRGB (255, 206, 142).withAlpha (0.12f));
    g.fillEllipse (560.0f, 36.0f, 180.0f, 180.0f);

    g.setColour (juce::Colour::fromRGB (255, 255, 255).withAlpha (0.1f));
    g.drawRoundedRectangle (20.0f, 20.0f, bounds.getWidth() - 40.0f, bounds.getHeight() - 40.0f, 24.0f, 1.0f);
}

void UtilityFiltersAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (34);
    auto header = area.removeFromTop (74);

    titleLabel.setBounds (header.removeFromTop (34));
    subtitleLabel.setBounds (header.removeFromTop (24));

    auto tabArea = area.removeFromTop (44);
    corePageButton.setBounds (tabArea.removeFromLeft (160).reduced (0, 4));
    tabArea.removeFromLeft (10);
    resonatorsPageButton.setBounds (tabArea.removeFromLeft (160).reduced (0, 4));

    auto grid = area.reduced (0, 18);

    auto placeKnob = [] (juce::Rectangle<int> slot, juce::Slider& slider, juce::Label& label)
    {
        label.setBounds (slot.removeFromTop (20));
        slider.setBounds (slot.reduced (8));
    };

    if (currentPage == Page::core)
    {
        auto selectorRow = grid.removeFromTop (50);
        modeLabel.setBounds (selectorRow.removeFromLeft (80));
        modeBox.setBounds (selectorRow.removeFromLeft (220).reduced (0, 8));
        ellipticResponseLabel.setBounds (selectorRow.removeFromLeft (90));
        ellipticResponseBox.setBounds (selectorRow.removeFromLeft (180).reduced (0, 8));

        auto topRow = grid.removeFromTop (170);
        auto bottomRow = grid.removeFromTop (170);

        const std::array<std::pair<juce::Slider*, juce::Label*>, 4> topControls {{
            { &mixSlider, &mixLabel },
            { &outputSlider, &outputLabel },
            { &feedbackSlider, &feedbackLabel },
            { &delaySlider, &delayLabel }
        }};

        const std::array<std::pair<juce::Slider*, juce::Label*>, 5> bottomControls {{
            { &cutoffSlider, &cutoffLabel },
            { &resonanceSlider, &resonanceLabel },
            { &ellipticTransitionSlider, &ellipticTransitionLabel },
            { &ellipticRippleSlider, &ellipticRippleLabel },
            { &ellipticStopbandSlider, &ellipticStopbandLabel }
        }};

        for (auto [slider, label] : topControls)
            placeKnob (topRow.removeFromLeft (grid.getWidth() / static_cast<int> (topControls.size())), *slider, *label);

        for (auto [slider, label] : bottomControls)
            placeKnob (bottomRow.removeFromLeft (grid.getWidth() / static_cast<int> (bottomControls.size())), *slider, *label);
    }
    else
    {
        auto selectorRow = grid.removeFromTop (50);
        bankStyleLabel.setBounds (selectorRow.removeFromLeft (90));
        bankStyleBox.setBounds (selectorRow.removeFromLeft (180).reduced (0, 8));

        auto topRow = grid.removeFromTop (170);
        auto bottomRow = grid.removeFromTop (170);

        const std::array<std::pair<juce::Slider*, juce::Label*>, 6> topControls {{
            { &mixSlider, &mixLabel },
            { &outputSlider, &outputLabel },
            { &cutoffSlider, &cutoffLabel },
            { &resonanceSlider, &resonanceLabel },
            { &spreadSlider, &spreadLabel },
            { &decaySlider, &decayLabel }
        }};

        const std::array<std::pair<juce::Slider*, juce::Label*>, 4> bottomControls {{
            { &modalCountSlider, &modalCountLabel },
            { &bankCountSlider, &bankCountLabel },
            { &bankSpacingSlider, &bankSpacingLabel },
            { &bankQSlider, &bankQLabel }
        }};

        for (auto [slider, label] : topControls)
            placeKnob (topRow.removeFromLeft (grid.getWidth() / static_cast<int> (topControls.size())), *slider, *label);

        for (auto [slider, label] : bottomControls)
            placeKnob (bottomRow.removeFromLeft (grid.getWidth() / static_cast<int> (bottomControls.size())), *slider, *label);
    }
}
