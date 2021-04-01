/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MultiTapDelay.h"

//==============================================================================
/**
*/
class REVERSEGATEAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    REVERSEGATEAudioProcessor();
    ~REVERSEGATEAudioProcessor() override;

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

    juce::AudioProcessorValueTreeState parameters;

private:
    std::vector<MultiTapDelay> delay;

    juce::AudioProcessorValueTreeState::Listener* listener;
    std::atomic<float>* delayTimeParameter = nullptr;
    std::atomic<float>* roomSizeParameter = nullptr;
    std::atomic<float>* mixParameter = nullptr;
    std::atomic<float>* volumeParameter = nullptr;
    
    class ParameterListener : public juce::AudioProcessorValueTreeState::Listener {
        public:
        ParameterListener(REVERSEGATEAudioProcessor& _p) : p(_p){}
        void parameterChanged(const juce::String& parameterID, float newValue) override {
            for (int i = 0; i < p.delay.size(); i++) {
                if (parameterID == "DELAY TIME") p.delay[i].setDelayTime(newValue);
                else if (parameterID == "ROOM SIZE") p.delay[i].setRoomSize(newValue);
                else if (parameterID == "MIX") p.delay[i].setMix(newValue);
                else if (parameterID == "VOLUME") p.delay[i].setVolume(newValue);
            }
        }
        private:
        REVERSEGATEAudioProcessor& p;
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (REVERSEGATEAudioProcessor)
};
