/*
  ==============================================================================

    WaveThumbnail.h
    Created: 9 Jul 2024 4:00:44pm
    Author:  tmobr

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class WaveThumbnail  : public juce::Component,
                       public juce::FileDragAndDropTarget
{
public:
    WaveThumbnail(SimpleSamplerAudioProcessor& p);
    ~WaveThumbnail() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:

    bool shouldBePainting{ false };
    std::vector<float> audioPoints;

    juce::String fileName{ "" };

    SimpleSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveThumbnail)
};
