/*
  ==============================================================================

    WaveThumbnail.cpp
    Created: 9 Jul 2024 4:00:44pm
    Author:  tmobr

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveThumbnail.h"

//==============================================================================
WaveThumbnail::WaveThumbnail(SimpleSamplerAudioProcessor& p) : audioProcessor(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

WaveThumbnail::~WaveThumbnail()
{
}

void WaveThumbnail::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::cadetblue.darker());
    auto waveform = audioProcessor.getWaveForm();

    if (waveform.getNumSamples() > 0) {
        juce::Path p;
        audioPoints.clear();

        auto ratio = waveform.getNumSamples() / getWidth();
        auto buffer = waveform.getReadPointer(0);

        for (int sample = 0; sample < waveform.getNumSamples(); sample += ratio) {
            audioPoints.push_back(buffer[sample]);
        }

        g.setColour(juce::Colours::yellow);
        p.startNewSubPath(0, getHeight() / 2);

        for (int sample = 0; sample < audioPoints.size(); ++sample) {
            auto point = juce::jmap<float>(audioPoints[sample], -1.0f, +1.0f, getHeight(), 0);
            p.lineTo(sample, point);
        }

        g.strokePath(p, juce::PathStrokeType(2));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
        auto bounds = getLocalBounds().reduced(10, 10);
        g.drawFittedText(fileName, bounds, juce::Justification::topRight, 1);

        int numSamples = audioProcessor.getWaveForm().getNumSamples();

        if (numSamples > 0) {
            auto playerHeadPosition = juce::jmap<int>(audioProcessor.getSampleCount(), 0, numSamples, 0, getWidth());
            g.setColour(juce::Colours::white);
            g.drawLine(playerHeadPosition, 0, playerHeadPosition, getHeight(), 2.0f);

            g.setColour(juce::Colours::black.withAlpha(0.2f));
            g.fillRect(0, 0, playerHeadPosition, getHeight());
        }
    }
    else {
        g.setColour(juce::Colours::white);
        g.setFont(20.0f);
        g.drawFittedText("Drop File to Load", getLocalBounds(), juce::Justification::centred, 1);
    }
}

void WaveThumbnail::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

bool WaveThumbnail::isInterestedInFileDrag(const juce::StringArray& files) {
    for (auto file : files) {
        if (file.contains(".mp3") || file.contains(".wav")) {
            return true;
        }
    }

    return false;
}

void WaveThumbnail::filesDropped(const juce::StringArray& files, int x, int y) {
    for (auto file : files) {
        if (isInterestedInFileDrag(files)) {
            auto myFile = std::make_unique<juce::File>(file);
            fileName = myFile->getFileNameWithoutExtension();

            audioProcessor.loadFile(file);
        }
    }
    repaint();
}
