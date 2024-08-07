/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveThumbnail.h"
#include "ADSRComponent.h"

//==============================================================================
/**
*/
class SimpleSamplerAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Timer
{
public:
    SimpleSamplerAudioProcessorEditor (SimpleSamplerAudioProcessor&);
    ~SimpleSamplerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleSamplerAudioProcessor& audioProcessor;

    WaveThumbnail waveThumbnail;
    ADSRComponent mADSR;
    juce::ImageComponent mImageComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleSamplerAudioProcessorEditor)
};
