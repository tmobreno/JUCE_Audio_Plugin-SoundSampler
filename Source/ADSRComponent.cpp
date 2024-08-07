/*
  ==============================================================================

    ADSRComponent.cpp
    Created: 9 Jul 2024 4:18:47pm
    Author:  tmobr

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ADSRComponent.h"

//==============================================================================
ADSRComponent::ADSRComponent(SimpleSamplerAudioProcessor& p) : audioProcessor (p)
{
    // Attack Slider
    attackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    attackSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::red);
    attackSlider.setRange(0.0f, 5.0f, 0.01f);
    addAndMakeVisible(attackSlider);

    attackLabel.setFont(10.0f);
    attackLabel.setText("Attack", juce::NotificationType::dontSendNotification);
    attackLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    attackLabel.setJustificationType(juce::Justification::centredTop);
    attackLabel.attachToComponent(&attackSlider, false);

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "ATTACK", attackSlider);

    // Decay Slider
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    decaySlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::red);
    decaySlider.setRange(0.0f, 5.0f, 0.01f);
    addAndMakeVisible(decaySlider);

    decayLabel.setFont(10.0f);
    decayLabel.setText("Decay", juce::NotificationType::dontSendNotification);
    decayLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    decayLabel.setJustificationType(juce::Justification::centredTop);
    decayLabel.attachToComponent(&decaySlider, false);

    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "DECAY", decaySlider);

    // Sustain Slider
    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    sustainSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::red);
    sustainSlider.setRange(0.0f, 1.0f, 0.01f);
    addAndMakeVisible(sustainSlider);

    sustainLabel.setFont(10.0f);
    sustainLabel.setText("Sustain", juce::NotificationType::dontSendNotification);
    sustainLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    sustainLabel.setJustificationType(juce::Justification::centredTop);
    sustainLabel.attachToComponent(&sustainSlider, false);

    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "SUSTAIN", sustainSlider);

    // Release Slider
    releaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    releaseSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::red);
    releaseSlider.setRange(0.0f, 1.0f, 0.01f);
    addAndMakeVisible(releaseSlider);

    releaseLabel.setFont(10.0f);
    releaseLabel.setText("Release", juce::NotificationType::dontSendNotification);
    releaseLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    releaseLabel.setJustificationType(juce::Justification::centredTop);
    releaseLabel.attachToComponent(&releaseSlider, false);

    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "RELEASE", releaseSlider);
}

ADSRComponent::~ADSRComponent()
{
}

void ADSRComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void ADSRComponent::resized()
{
    const auto startX = 0.6f;
    const auto startY = 0.2f;
    const auto dialWidth = 0.1f;
    const auto dialHeight = 0.75f;

    attackSlider.setBoundsRelative(startX, startY, dialWidth, dialHeight);
    decaySlider.setBoundsRelative(startX + dialWidth, startY, dialWidth, dialHeight);
    sustainSlider.setBoundsRelative(startX + (dialWidth * 2), startY, dialWidth, dialHeight);
    releaseSlider.setBoundsRelative(startX + (dialWidth * 3), startY, dialWidth, dialHeight);
}
