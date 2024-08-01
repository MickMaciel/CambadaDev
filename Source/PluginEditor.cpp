/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CambadaAudioProcessorEditor::CambadaAudioProcessorEditor (CambadaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    gainGroup.setText("Output Gain");
    gainGroup.addAndMakeVisible(gainSlider);
    
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainSlider.setNumDecimalPlacesToDisplay(1);
    gainSlider.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);
    gainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkcyan);
    
    addAndMakeVisible(gainGroup);
    
    addAndMakeVisible(meterL);
    addAndMakeVisible(meterR);
    
    startTimerHz(60);
    
    
}

CambadaAudioProcessorEditor::~CambadaAudioProcessorEditor()
{
}

//==============================================================================
void CambadaAudioProcessorEditor::timerCallback()
{
    
    meterL.setLevel(audioProcessor.getRmsValue(0));
    meterR.setLevel(audioProcessor.getRmsValue(1));
    
    meterL.repaint();
    meterR.repaint();
}


void CambadaAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (25.0f));
    g.drawFittedText ("CambadaDev", getLocalBounds(), juce::Justification::centredTop, 1);
}

void CambadaAudioProcessorEditor::resized()
{
    int marginX = 30;
    int marginY = 50;
    int groupW = 340;
    int groupH = 200;
    
    
    gainGroup.setBounds(marginX, marginY, groupW, groupH);
    
    gainSlider.setBounds(100, 30, 150, 150);
    
    meterL.setBounds(marginX, 255, groupW, 10);
    meterR.setBounds(marginX, 268, groupW, 10);
    
    
    
}
