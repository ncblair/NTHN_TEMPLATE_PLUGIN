// Nathan Blair June 2023

#pragma once

class StateManager;
class Gain;

#include <juce_audio_basics/juce_audio_basics.h>

#include "PluginProcessorBase.h"

//==============================================================================
class PluginProcessor : public PluginProcessorBase
{
public:
    //==============================================================================
    PluginProcessor();
    ~PluginProcessor() override;
    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;
    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
    //==============================================================================
    // state
    //==============================================================================
    std::unique_ptr<StateManager> state;

    //==============================================================================
    float tau2pole(float tau, float sr);

    void prepareSmoothedVariables();

private:

    bool isPrepared = false;

    float smoothPole;
    float smoothedGain;

    std::unique_ptr<Gain> gain;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};