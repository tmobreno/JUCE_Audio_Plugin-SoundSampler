/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SimpleSamplerAudioProcessor  : public juce::AudioProcessor,
                                     public juce::ValueTree::Listener
{
public:
    //==============================================================================
    SimpleSamplerAudioProcessor();
    ~SimpleSamplerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadFile();
    void loadFile(const juce::String& path);
    void setKeysAndSound();

    int getNumSamplerSounds() { return mSampler.getNumSounds(); }
    juce::AudioBuffer<float>& getWaveForm() { return mWaveForm; };

    void updateADSR();

    juce::ADSR::Parameters& getADSRParams() { return ADSRparams; }
    juce::AudioProcessorValueTreeState& getAPVTS() { return APVTS; }
    std::atomic<bool>& getIsNotePlayed() { return isNotePlayed; }
    std::atomic<int>& getSampleCount() { return sampleCount; }

private:
    juce::Synthesiser mSampler;
    const int mNumVoices{ 3 };
    juce::AudioBuffer<float> mWaveForm;

    juce::ADSR::Parameters ADSRparams;

    juce::AudioFormatManager mFormatManager;
    juce::AudioFormatReader* mFormatReader{ nullptr };

    juce::AudioProcessorValueTreeState APVTS;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    std::atomic<bool> shouldUpdate { false };
    std::atomic<bool> isNotePlayed { false };
    std::atomic<int> sampleCount { 0 };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleSamplerAudioProcessor)
};
