/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleSamplerAudioProcessorEditor::SimpleSamplerAudioProcessorEditor (SimpleSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p), waveThumbnail(p), mADSR(p), audioProcessor (p)
{
    auto image = juce::ImageCache::getFromMemory(BinaryData::logo_png , BinaryData::logo_pngSize);

    if (!image.isNull()) {
        mImageComponent.setImage(image, juce::RectanglePlacement::stretchToFit);
    }
    else {
        jassert(!image.isNull());
    }

    addAndMakeVisible(waveThumbnail);
    addAndMakeVisible(mADSR);
    addAndMakeVisible(mImageComponent);

    startTimerHz(30);

    setSize (600, 400);
}

SimpleSamplerAudioProcessorEditor::~SimpleSamplerAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void SimpleSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void SimpleSamplerAudioProcessorEditor::resized()
{
    waveThumbnail.setBoundsRelative(0.0f, 0.25f, 1.0f, 0.5f);
    mADSR.setBoundsRelative(0.0f, 0.75f, 1.0f, 0.25f);
    mImageComponent.setBoundsRelative(0.02f, 0.02f, 0.2f, 0.2f);
}

void SimpleSamplerAudioProcessorEditor::timerCallback() {
    repaint();
}
