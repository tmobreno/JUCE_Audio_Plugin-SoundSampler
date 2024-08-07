/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleSamplerAudioProcessor::SimpleSamplerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), APVTS(*this, nullptr, "PARAMETERS", createParameters())
#endif
{
    mFormatManager.registerBasicFormats();
    APVTS.state.addListener(this);

    for (int i = 0; i < mNumVoices; i++) {
        mSampler.addVoice(new juce::SamplerVoice());
    }
}

SimpleSamplerAudioProcessor::~SimpleSamplerAudioProcessor()
{
    mFormatReader = nullptr;
}

//==============================================================================
const juce::String SimpleSamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleSamplerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleSamplerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleSamplerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleSamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleSamplerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleSamplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleSamplerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleSamplerAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleSamplerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleSamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSampler.setCurrentPlaybackSampleRate(sampleRate);

    updateADSR();
}

void SimpleSamplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleSamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleSamplerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (shouldUpdate) {
        updateADSR();
    }

    juce::MidiMessage m;
    juce::MidiBuffer::Iterator it{ midiMessages };
    int sample;

    while (it.getNextEvent(m, sample)) {
        if (m.isNoteOn()) {
            isNotePlayed = true;
        }
        else if (m.isNoteOff()) {
            isNotePlayed = false;
        }
    }

    sampleCount = isNotePlayed ? sampleCount += buffer.getNumSamples() : 0;

    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool SimpleSamplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleSamplerAudioProcessor::createEditor()
{
    return new SimpleSamplerAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleSamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleSamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void SimpleSamplerAudioProcessor::loadFile() {
    mSampler.clearSounds();

    juce::FileChooser chooser{ "Please load a file" };

    if (chooser.browseForFileToOpen()) {
        auto file = chooser.getResult();
        mFormatReader = mFormatManager.createReaderFor(file);
    }

    setKeysAndSound();
}

void SimpleSamplerAudioProcessor::loadFile(const juce::String& path) {
    mSampler.clearSounds();

    auto file = juce::File(path);
    mFormatReader = mFormatManager.createReaderFor(file);

    auto sampleLength = static_cast<int>(mFormatReader->lengthInSamples);

    mWaveForm.setSize(1, sampleLength);
    mFormatReader->read(&mWaveForm, 0, sampleLength, 0, true, false);

    auto buffer = mWaveForm.getReadPointer(0);

    setKeysAndSound();

    updateADSR();
}

void SimpleSamplerAudioProcessor::setKeysAndSound() {
    juce::BigInteger range;
    range.setRange(0, 128, true);

    mSampler.addSound(new juce::SamplerSound("Sample", *mFormatReader, range, 60, 0.01, 0.01, 300));
}

void SimpleSamplerAudioProcessor::updateADSR() {
    ADSRparams.attack = APVTS.getRawParameterValue("ATTACK")->load();
    ADSRparams.decay = APVTS.getRawParameterValue("DECAY")->load();
    ADSRparams.sustain = APVTS.getRawParameterValue("SUSTAIN")->load();
    ADSRparams.release = APVTS.getRawParameterValue("RELEASE")->load();

    for (int i = 0; i < mSampler.getNumSounds(); ++i) {
        if (auto sound = dynamic_cast<juce::SamplerSound*>(mSampler.getSound(i).get())) {
            sound->setEnvelopeParameters(ADSRparams);
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleSamplerAudioProcessor::createParameters() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat> ("ATTACK", "Attack", 0.0f, 5.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", 0.0f, 5.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain", 0.0f, 5.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release", 0.0f, 5.0f, 0.0f));

    return { parameters.begin(), parameters.end() };
}

void SimpleSamplerAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) {
    shouldUpdate = true;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleSamplerAudioProcessor();
}
