// Nathan Blair January 2023

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../parameters/StateManager.h"

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
    juce::ignoreUnused(samplesPerBlock);
    juce::ignoreUnused(sampleRate);
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // auto output_channels = size_t(getTotalNumOutputChannels());
    // auto num_samples = size_t(buffer.getNumSamples());
    // auto sr = getSampleRate();

    // auto read_pointers = buffer.getArrayOfReadPointers();

    //--------------------------------------------------------------------------------
    // read in some parameter values here, if you want
    // in this case, gain goes from 0 to 100 (see: ../parameters/parameters.csv)
    // so we normalize it to 0 to 1
    //--------------------------------------------------------------------------------
    auto gain = state->param_value(PARAM::GAIN) / 100.0f;

    //--------------------------------------------------------------------------------
    // process samples below. use the buffer argument that is passed in.
    // for an audio effect, buffer is filled with input samples, and you should fill it with output samples
    // for a synth, buffer is filled with zeros, and you should fill it with output samples
    // see: https://docs.juce.com/master/classAudioBuffer.html
    //--------------------------------------------------------------------------------
    buffer.applyGain(gain);

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