// Nathan Blair January 2023

#include "PluginEditor.h"
#include "../parameters/StateManager.h"
#include "../interface/ParameterSlider.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    state = processorRef.state.get();

    // add slider BEFORE setting size
    gain_slider = std::make_unique<ParameterSlider>(state, PARAM::GAIN);
    addAndMakeVisible(*gain_slider);

    // some settings about UI
    setOpaque(true);
    setSize(W, H);
    setColour(0, juce::Colour(0xff00ffa1)); // background color

    // resizable window
    setResizable(true, true);
    setResizeLimits((W * 4) / 5, (H * 4) / 5, (W * 3) / 2, (H * 3) / 2);
    getConstrainer()->setFixedAspectRatio(float(W) / float(H));

    // VBlank attachment / Timer
    repaint_callback_handler = std::make_unique<juce::VBlankAttachment>(this, [this](double)
                                                                        { windowReadyToPaint(); });
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // remove any listeners here

    // also, if we have a lookAndFeel object we should call:
    // setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    // Our component is opaque, so we must completely fill the background with a solid colour
    g.fillAll(findColour(0));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // set the position of your components here
    int slider_size = proportionOfWidth(0.1f);
    int slider_x = proportionOfWidth(0.5f) - (slider_size / 2);
    int slider_y = proportionOfHeight(0.5f) - (slider_size / 2);
    gain_slider->setBounds(slider_x, slider_y, slider_size, slider_size);
}

void AudioPluginAudioProcessorEditor::windowReadyToPaint()
{
    // repaint UI and note that we have updated ui, if parameter values have changed
    if (state->any_parameter_changed.exchange(false))
    {
        for (size_t param_id{0}; param_id < TOTAL_NUMBER_PARAMETERS; ++param_id)
        {
            if (state->get_parameter_modified(param_id))
            {
                for (const auto& [component, callback_fn] : state->get_callbacks(param_id))
                  callback_fn();
            }
        }
    }

    state->update_preset_modified();
}