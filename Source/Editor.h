/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Knob.h"
typedef juce::AudioProcessorValueTreeState::SliderAttachment KnobAttachment;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Editor  : public juce::AudioProcessorEditor,
                public juce::Slider::Listener
{
public:
    //==============================================================================
    Editor (REVERSEGATEAudioProcessor &p);
    ~Editor() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;

    // Binary resources:
    static const char* bg_png;
    static const int bg_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    REVERSEGATEAudioProcessor& processor;
    Knob knob;

    juce::AudioProcessorValueTreeState& valueTreeState;
    std::unique_ptr<KnobAttachment> delayTimeAttachment;
    std::unique_ptr<KnobAttachment> roomSizeAttachment;
    std::unique_ptr<KnobAttachment> mixAttachment;
    std::unique_ptr<KnobAttachment> volumeAttachment;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Slider> volumeKnob;
    std::unique_ptr<juce::Slider> mixKnob;
    std::unique_ptr<juce::Slider> roomSizeKnob;
    std::unique_ptr<juce::Slider> delayTimeKnob;
    juce::Image cachedImage_bg_png_1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

