/*
Created by:  Nathan Blair
Last Edited: April 19, 2022

-> Base class for the JUCE audio processor that interacts with the host
*/

#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class PluginProcessorBase  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PluginProcessorBase();
    virtual ~PluginProcessorBase() override;
    //==============================================================================
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    bool hasEditor() const override;
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
private:

};