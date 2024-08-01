/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUIMeter.h"

//==============================================================================
/**
*/
class CambadaAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    CambadaAudioProcessorEditor (CambadaAudioProcessor&);
    ~CambadaAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CambadaAudioProcessor& audioProcessor;
    
    juce::GroupComponent gainGroup;
    
    juce::Slider gainSlider;
   
    juce::AudioProcessorValueTreeState::SliderAttachment attachment
    {
        audioProcessor.apvts, gainParameterID.getParamID(), gainSlider
    };
    

    GUI::GUIMeter meterL, meterR;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CambadaAudioProcessorEditor)
};
