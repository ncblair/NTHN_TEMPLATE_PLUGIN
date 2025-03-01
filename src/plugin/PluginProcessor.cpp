// Nathan Blair June 2023

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../parameters/StateManager.h"
#include "../audio/Gain.h"

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
void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Called after the constructor, but before playback starts
    // Use this to allocate up any resources you need, and to reset any
    // variables that depend on sample rate or block size

    gain = std::make_unique<Gain>(float(sampleRate), samplesPerBlock, getTotalNumOutputChannels(), PARAMETER_DEFAULTS[PARAM::GAIN] / 100.0f);
}

void PluginProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                   juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // get audio buffer references outside of JUCE, so we can pass to non-juce processors
    float *const *bufferPtrs = buffer.getArrayOfWritePointers();
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    //--------------------------------------------------------------------------------
    // read in some parameter values here, if you want
    // in this case, gain goes from 0 to 100 (see: ../parameters/parameters.csv)
    // so we normalize it to 0 to 1
    //--------------------------------------------------------------------------------
    auto requested_gain = state->param_value(PARAM::GAIN) / 100.0f;

    //--------
    // Tell all of our processors to force their parameters to update
    // This should get run any time the host sets state from setStateInformation
    // i.e. there should be no startup time for the plugin parameters to load at the beginning of a render
    // this should also get called when the plugin needs to clear tails, in reset()
    //----
    if (needsToSnapSmoothedParameters.exchange(false))
    {
        // force state, to end any internal smoothing
        if (gain)
            gain->setState(state->param_value(PARAM::GAIN) / 100.0f);
    }

    //--------------------------------------------------------------------------------
    // process samples below.
    // for an audio effect, buffer is filled with input samples, and you should fill it with output samples
    // for a synth, buffer is filled with zeros, and you should fill it with output samples
    // see: https://docs.juce.com/master/classAudioBuffer.html
    //--------------------------------------------------------------------------------
    gain->process(bufferPtrs, numSamples, numChannels, requested_gain);
    //--------------------------------------------------------------------------------
    // you can use midiMessages to read midi if you need.
    // since we are not using midi yet, we clear the buffer.
    //--------------------------------------------------------------------------------
    midiMessages.clear();
}

void PluginProcessor::reset()
{
    // called to clear any "tails" and make sure the plugin is ready to process.
    needsToSnapSmoothedParameters.store(true);
}

//==============================================================================
void PluginProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    // We will just store our parameter state, for now
    auto plugin_state = state->get_state();
    std::unique_ptr<juce::XmlElement> xml(plugin_state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // Restore our parameters from file
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    state->load_from(xmlState.get());

    // we have entirely new parameters ready, so set parameters immediately, don't smooth
    needsToSnapSmoothedParameters.store(true);
}

juce::AudioProcessorEditor *PluginProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}