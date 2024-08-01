/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CambadaAudioProcessor::CambadaAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    auto* param = apvts.getParameter(gainParameterID.getParamID());
    gainParameter = dynamic_cast<juce::AudioParameterFloat*>(param);
}

CambadaAudioProcessor::~CambadaAudioProcessor()
{
}

//==============================================================================
const juce::String CambadaAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CambadaAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CambadaAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CambadaAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CambadaAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CambadaAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CambadaAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CambadaAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CambadaAudioProcessor::getProgramName (int index)
{
    return {};
}

void CambadaAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CambadaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rmsLevelLeft.reset(sampleRate, 0.3);
    rmsLevelLeft.reset(sampleRate, 0.3);
    
    rmsLevelLeft.setCurrentAndTargetValue(-100.f);
    rmsLevelRigth.setCurrentAndTargetValue(-100.f);
}

void CambadaAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CambadaAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void CambadaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    float gainToDecibels = gainParameter->get();
    
    float gain = juce::Decibels::decibelsToGain(gainToDecibels);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= gain;
        }
    }

    rmsLevelLeft.skip(buffer.getNumSamples());
    rmsLevelRigth.skip(buffer.getNumSamples());
    
    {
        const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
        if (value < rmsLevelLeft.getNextValue())
        {
            rmsLevelLeft.setTargetValue(value);
        }
        else
        {
            rmsLevelLeft.setCurrentAndTargetValue(value);
        }
    }
    {
        const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1 , 0, buffer.getNumSamples()));
        if (value < rmsLevelRigth.getNextValue())
        {
            rmsLevelRigth.setTargetValue(value);
        }
        else
        {
            rmsLevelRigth.setCurrentAndTargetValue(value);
        }
    }
}

//==============================================================================
bool CambadaAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CambadaAudioProcessor::createEditor()
{
    return new CambadaAudioProcessorEditor (*this);
}

//==============================================================================
void CambadaAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CambadaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CambadaAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout CambadaAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(gainParameterID,
                                                          "OutputGain",
                                                          juce::NormalisableRange<float> { -36.0f, 12.0f },
                                                          0.0f));
    return layout;
}

float CambadaAudioProcessor::getRmsValue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0)
        return rmsLevelLeft.getCurrentValue();
    if (channel == 1)
        return rmsLevelRigth.getCurrentValue();
    
    return 0.f;
}
