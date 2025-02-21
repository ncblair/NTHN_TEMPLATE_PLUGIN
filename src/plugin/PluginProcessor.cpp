// Nathan Blair June 2023

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../parameters/StateManager.h"
#include "../audio/Gain.h"
#include "Utils.h"

//==============================================================================
PluginProcessor::PluginProcessor()
{
    state = std::make_unique<StateManager>(this);
}

PluginProcessor::~PluginProcessor()
{
    // stop any threads, delete any raw pointers, remove any listeners, etc
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Called after the constructor, but before playback starts
    // Use this to allocate up any resources you need, and to reset any
    // variables that depend on sample rate or block size

    // Flags to indicate that the plugin is prepared in case setStateInformation is called before prepareToPlay
    isPrepared = true;

    // Set up the smoothing pole for desired smoothing time
    float tau = 0.050f; // 50ms smoothing time
    smoothPole = Utils::tau2pole(tau, sampleRate);

    // Update the smoothed variables
    prepareSmoothedVariables();

    gain = std::make_unique<Gain>(float(sampleRate), samplesPerBlock, getTotalNumOutputChannels(), PARAMETER_DEFAULTS[PARAM::GAIN] / 100.0f);
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    //--------------------------------------------------------------------------------
    // read in some parameter values here, if you want
    // in this case, gain goes from 0 to 100 (see: ../parameters/parameters.csv)
    // so we normalize it to 0 to 1
    //--------------------------------------------------------------------------------
    auto requested_gain = state->param_value(PARAM::GAIN) / 100.0f;

    // Apply smoothing to incoming parameter changes
    smoothedGain = Utils::lerp(smoothedGain, requested_gain, smoothPole);

    //--------------------------------------------------------------------------------
    // process samples below. use the buffer argument that is passed in.
    // for an audio effect, buffer is filled with input samples, and you should fill it with output samples
    // for a synth, buffer is filled with zeros, and you should fill it with output samples
    // see: https://docs.juce.com/master/classAudioBuffer.html
    //--------------------------------------------------------------------------------

    gain->setGain(smoothedGain);
    gain->process(buffer);
    //--------------------------------------------------------------------------------
    // you can use midiMessages to read midi if you need.
    // since we are not using midi yet, we clear the buffer.
    //--------------------------------------------------------------------------------
    midiMessages.clear();
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    // We will just store our parameter state, for now
    auto plugin_state = state->get_state();
    std::unique_ptr<juce::XmlElement> xml (plugin_state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // Restore our parameters from file
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    state->load_from(xmlState.get());

    // Update the smoothed variables if prepareToPlay hasn't been called
    if (!isPrepared)
    {
        prepareSmoothedVariables();
    }
}

void PluginProcessor::prepareSmoothedVariables()
{
    // Function to update the smoothed variables both in prepareToPlay and setStateInformation
    smoothedGain = juce::jlimit(0.0f, 100.0f, state->param_value(GAIN));
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}