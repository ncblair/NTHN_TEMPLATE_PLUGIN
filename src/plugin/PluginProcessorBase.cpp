/*
Created by:  Nathan Blair
Last Edited: April 19, 2022

-> Base class for the JUCE audio processor that interacts the host
*/

#include "PluginProcessorBase.h"

//==============================================================================
PluginProcessorBase::PluginProcessorBase()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
#if NEEDS_SIDECHAIN
                         .withInput("Sidechain", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ) {
}

PluginProcessorBase::~PluginProcessorBase() {}

//==============================================================================
const juce::String PluginProcessorBase::getName() const { return JucePlugin_Name; }

bool PluginProcessorBase::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool PluginProcessorBase::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool PluginProcessorBase::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double PluginProcessorBase::getTailLengthSeconds() const { return 10.0; }

int PluginProcessorBase::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
            // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessorBase::getCurrentProgram() { return 0; }

void PluginProcessorBase::setCurrentProgram(int index) { juce::ignoreUnused(index); }

const juce::String PluginProcessorBase::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void PluginProcessorBase::changeProgramName(int index, const juce::String &newName) {
  juce::ignoreUnused(index, newName);
}

void PluginProcessorBase::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool PluginProcessorBase::isBusesLayoutSupported(const BusesLayout &layouts) const {
  // modify as necessary
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
#if NEEDS_SIDECHAIN
  // sidechain audio effect
  if (layouts.getChannelSet(true, 1) !=
      layouts.getMainInputChannelSet()) { // number of inputs in sidechain should match number of
                                          // inputs in main set
    return false;
  }
#endif
  // audio effect
  // number of inputs in outputChannel should match main number of inputs
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) return false;
#else
#if NEEDS_SIDECHAIN
  // synth with sidechain
  // check that sidechain channel set matches number of output channels
  if (layouts.getChannelSet(true, 0) != layouts.getMainOutputChannelSet()) {
    return false;
  }
#endif
#endif

  return true;
#endif
}

//==============================================================================
bool PluginProcessorBase::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}