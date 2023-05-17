// Nathan Blair January 2023

#pragma once

class StateManager;
class ParameterSlider;

#include "PluginProcessor.h"

//==============================================================================
// Graphics and UI
// This is the main graphics component for the plugin
// -----
// I also have it inherit from juce's Timer class
// This enables polling: https://en.wikipedia.org/wiki/Polling_(computer_science)
// I use polling for:
//      • Undo/Redo functionality
//      • Updating the UI when parameter changes come from the host
//
// The other option is to use listeners, which have the advantage of being more efficient
// However, 90% of the time, in audio code, I prefer polling
// parameter listeners CAN get called by the host from the audio thread, which leads
// to issues with thread safety and realtime safety
//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (PluginProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    
    void resized() override;
    void timerCallback() override;

private:
    
    const int TIMER_HZ = 60;
    const int UNDO_HZ = 1;
    const int W = 900; // width
    const int H = 500; // height
    unsigned int timer_counter{0};
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;

    // a convenient pointer to the state manager 
    // which is owned by the processor
    StateManager* state;

    // A single slider
    std::unique_ptr<ParameterSlider> gain_slider;

    //==============================================================================
    // UNDO REDO
    // to use, attach buttons to state->undo() / state->redo()
    //==============================================================================
    juce::UndoManager* undo_manager;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};