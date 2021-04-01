/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "Editor.h"

#define VALUE_MIN_DELAY_TIME 0.0
#define VALUE_MAX_DELAY_TIME 50.0
#define VALUE_MIN_ROOM_SIZE 0.0
#define VALUE_MAX_ROOM_SIZE 500.0f

//==============================================================================
REVERSEGATEAudioProcessor::REVERSEGATEAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else 
    :
#endif
    parameters(*this, nullptr, juce::Identifier(JucePlugin_Name), // processor, undoManager, valueTreeType, parameterLayout
    {
        std::make_unique<juce::AudioParameterFloat>("DELAY TIME", "DELAY TIME", juce::NormalisableRange<float>(VALUE_MIN_DELAY_TIME, VALUE_MAX_DELAY_TIME, 0.05), 30.0),
        std::make_unique<juce::AudioParameterFloat>("ROOM SIZE", "ROOM SIZE", juce::NormalisableRange<float>(VALUE_MIN_ROOM_SIZE, VALUE_MAX_ROOM_SIZE, 0.05), 15.0),
        std::make_unique<juce::AudioParameterFloat>("MIX", "MIX", juce::NormalisableRange<float>(0.0, 100.0, 0.1), 50.0),
        std::make_unique<juce::AudioParameterFloat>("VOLUME", "VOLUME", juce::NormalisableRange<float>(0.0, 1.0, 0.1), 0.8),

    })    
{
    delayTimeParameter = parameters.getRawParameterValue("DELAY TIME");
    roomSizeParameter = parameters.getRawParameterValue("ROOM SIZE");
    mixParameter = parameters.getRawParameterValue("MIX");
    volumeParameter = parameters.getRawParameterValue("VOLUME");
    listener = new ParameterListener(*this);
    parameters.addParameterListener("DELAY TIME", listener);
    parameters.addParameterListener("ROOM SIZE", listener);
    parameters.addParameterListener("MIX", listener);
    parameters.addParameterListener("VOLUME", listener);
    
}

REVERSEGATEAudioProcessor::~REVERSEGATEAudioProcessor()
{
    parameters.removeParameterListener("DELAY TIME", listener);
    parameters.removeParameterListener("ROOM SIZE", listener);
    parameters.removeParameterListener("MIX", listener);
    parameters.removeParameterListener("VOLUME", listener);
    listener = nullptr;
    delayTimeParameter = nullptr;
    roomSizeParameter = nullptr;
    mixParameter = nullptr;
    volumeParameter = nullptr;
}

//==============================================================================
const juce::String REVERSEGATEAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool REVERSEGATEAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool REVERSEGATEAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool REVERSEGATEAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double REVERSEGATEAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int REVERSEGATEAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int REVERSEGATEAudioProcessor::getCurrentProgram()
{
    return 0;
}

void REVERSEGATEAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String REVERSEGATEAudioProcessor::getProgramName (int index)
{
    return {};
}

void REVERSEGATEAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void REVERSEGATEAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void REVERSEGATEAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool REVERSEGATEAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void REVERSEGATEAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    juce::int32 numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
        
    if (totalNumInputChannels > 0) delay.resize(totalNumInputChannels);
    for (int i = 0; i < delay.size(); i++) {
        delay[i].setSampleRate(getSampleRate());
        delay[i].setRoomSizeMax(VALUE_MAX_ROOM_SIZE);
        delay[i].setDelayTimeMax(VALUE_MAX_DELAY_TIME);
    }
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        delay[channel].process(buffer, channel, numSamples);
    }
}

//==============================================================================
bool REVERSEGATEAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* REVERSEGATEAudioProcessor::createEditor()
{
    return new Editor (*this);
}

//==============================================================================
void REVERSEGATEAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // load from xml
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void REVERSEGATEAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // save to xml
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
 
    if (xmlState.get() != nullptr) 
    {
        if (xmlState->hasTagName (parameters.state.getType())) 
        {
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new REVERSEGATEAudioProcessor();
}
