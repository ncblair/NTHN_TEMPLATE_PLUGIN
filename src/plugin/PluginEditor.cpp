// Nathan Blair January 2023

#include "PluginEditor.h"
#include "../parameters/StateManager.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    state = processorRef.state.get();
    startTimerHz(TIMER_HZ);

    // INIT UNDO/REDO
    undo_manager = state->get_undo_manager();

    // some settings about UI
    setOpaque (true);
    setSize(W, H);
    
    // resizable window
    setResizable(true, true);
    setResizeLimits((W * 4) / 5, (H * 4) / 5, (W * 3) / 2, (H * 3) / 2);
    getConstrainer()->setFixedAspectRatio(float(W) / float(H));
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // remove any listeners here

    // also, if we have a lookAndFeel object we should call:
    // setLookAndFeel(nullptr); 
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Our component is opaque, so we must completely fill the background with a solid colour
    // Use gain parameter to determine background color
    auto gain = state->param_value(PARAM::GAIN) / 100.0f;
    g.fillAll (juce::Colour(0xff00ffa1).withBrightness(gain));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // set the position of your components here
}

void AudioPluginAudioProcessorEditor::timerCallback() {
    // repaint UI and note that we have updated ui
    if (state->any_parameter_changed.load()) {
        repaint();
        state->any_parameter_changed.store(false);
    }
    state->update_preset_modified();

    if (timer_counter % (TIMER_HZ / UNDO_HZ) == 0 ) {
        if (!isMouseButtonDownAnywhere()) {
            processorRef.state->get_undo_manager()->beginNewTransaction();
        }
    }

    timer_counter++;
}