/*
  ==============================================================================

    GUIMeter.h
    Created: 18 Jul 2024 11:12:28am
    Author:  Mick Maciel

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


namespace GUI
{
    class GUIMeter : public juce::Component, public juce::Timer
    {
    public:
        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();
            
            auto gradient = juce::ColourGradient
            {
                juce::Colours::greenyellow,
                bounds.getBottomLeft(),
                juce::Colours::orange,
                bounds.getBottomRight(),
                false
            };
            gradient.addColour(0.3, juce::Colours::yellow);
            
            g.setColour(juce::Colours::grey);
            g.fillRoundedRectangle(bounds, 5.f);
            g.setGradientFill(gradient);
                
            const auto scaledX = juce::jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getWidth()));
            g.fillRoundedRectangle(bounds.removeFromLeft(scaledX), 5.f);
        }
        
        
        void timerCallback() override
        {
            repaint();
        }
        
        void setLevel(const float value) { level = value; }
        
    private:
        float level = -60.0f;
    };
}

