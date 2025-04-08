// Nathan Blair January 2023

#pragma once

class PluginProcessor;

#include <shared_mutex>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "ParameterDefines.h"

/*
StateManager manages Parameters, Properties, Presets

  -> In other words, StateManager manages the plugin state.

  properties are non-automatable parameters
  properties are stored in the property tree which is a ValueTree
  parameters are stored in the parameter tree which is an APVTS attached to the
PluginProcessor

  -> outside of StateManager, you interact with properties and parameters as
pretty much equivalent that is: you can get the param_value() of either a
property or a parameter however, properties will not be exposed to the host and
you have to be careful to note that the APVTS is not the same type as ValueTree

  presets are stored in the preset tree which is a ValueTree with a property
PRESET_NAME_ID
*/

class StateManager : public juce::ValueTree::Listener,
                     public juce::AudioProcessorValueTreeState::Listener {
public:
  StateManager(PluginProcessor *proc);
  ~StateManager() override;

  //--------------------------------------------------------------------------------
  // the main api for accessing plugin state is param_value()
  // example: param_value(PARAM::PARAM_1)

  // PARAM enums are stored in ParameterDefines.h and will be available to all
  // files that include the state manager

  // param_value is a thread safe/realtime safe method to be called on any
  // thread
  //--------------------------------------------------------------------------------
  float param_value(size_t param_id);

  //--------------------------------------------------------------------------------
  // You can also use these methods from the UI thread to access parameters
  // These methods are not necessarily realtime safe, so don't call from audio
  // thread And don't call these from apvts listeners (which might be invoked on
  // audio thread)

  // an example might be setting parameters from a custom knob or button
  // component
  //--------------------------------------------------------------------------------
  juce::RangedAudioParameter *get_parameter(size_t param_id);
  void begin_change_gesture(size_t param_id);
  void end_change_gesture(size_t param_id);
  void set_parameter(size_t param_id, float value);
  void set_parameter_normalized(size_t param_id, float normalized_value);
  void randomize_parameter(size_t param_id, float min = 0.0f, float max = 1.0f);
  void reset_parameter(size_t param_id);
  void init();
  void randomize_parameters();
  juce::String get_parameter_text(size_t param_id);

  //--------------------------------------------------------------------------------
  // ValueTrees are essentially light references to shared JSON data and can be
  // copied freely Use these methods to get ValueTrees for use in a UI component
  // – say a preset browser or use get_state() if you want the whole state of
  // the plugin – say for saving to disk
  //--------------------------------------------------------------------------------
  juce::ValueTree get_state();

  //----------------------------------------x----------------------------------------
  // Saving and Loading Presets, called from UI thread
  // preset_modified is true when any parameter has been changed, after loading
  // a preset
  //--------------------------------------------------------------------------------
  void save_preset(juce::String preset_name);
  void load_preset(juce::String preset_name);
  void load_from(juce::XmlElement *xml);
  void set_preset_name(juce::String preset_name);
  juce::String get_preset_name();
  void update_preset_modified();
  bool get_parameter_modified(size_t param_id, bool exchange_value = false);

  //--------------------------------------------------------------------------------
  // a single UndoManager is shared by all ValueTrees
  //--------------------------------------------------------------------------------
  void undo();
  void redo();
  juce::UndoManager *get_undo_manager();

  //--------------------------------------------------------------------------------
  // value tree listener callbacks – so we can mark when the state has changed
  //--------------------------------------------------------------------------------
  void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged,
                                const juce::Identifier &property) override;
  void parameterChanged(const juce::String &parameterID,
                        float newValue) override;

  //--------------------------------------------------------------------------------
  // each component registers itself with the state manager
  // allowing the PluginEditor to loop over each component registerd with the
  // state manager and call repaint() if the value of the underlying parameter
  // has changed also supports a custom callback function that does not repaint
  // by default call this only one time per per component parameter pair
  //--------------------------------------------------------------------------------
  void register_component(size_t param_id, juce::Component *component,
                          std::function<void()> custom_callback = {});
  void unregister_component(size_t param_id, juce::Component *component);
  std::unordered_map<juce::Component *, std::function<void()>> &
  get_callbacks(size_t param_id) {
    return param_to_callback[param_id];
  }

  //--------------------------------------------------------------------------------
  // const identifiers used for accessing ValueTrees
  // You might be able to make these private, depends on your implementation
  // One reason they should be public: if you copy a valueTree to a separate
  // component
  //    and you want to get the children of that tree by identifier
  //        (but don't want to copy a bunch of strings around)
  //--------------------------------------------------------------------------------
  static inline const juce::Identifier PARAMETERS_ID{"PARAMETERS"};
  static inline const juce::Identifier PRESET_ID{"PRESET"};
  static inline const juce::Identifier PRESET_NAME_ID{"PRESET_NAME"};
  static inline const juce::Identifier PRESET_MODIFIED_ID{"PRESET_MODIFIED"};
  static inline const juce::Identifier PROPERTIES_ID{"PROPERTIES"};
  static inline const juce::Identifier STATE_ID{"STATE"};

  //--------------------------------------------------------------------------------
  // Some preset info
  // these are public for convenience if you make a preset browser component
  //--------------------------------------------------------------------------------
  const juce::File PRESETS_DIR; // initialized in initializer list
  const juce::String PRESET_EXTENSION;
  const juce::String DEFAULT_PRESET{"INIT"};

  //--------------------------------------------------------------------------------
  // any_parameter_changed is true after any parameter is changed (including
  // preset changes) preset modified is true after any parameter is changed (but
  // not after preset changes)
  //--------------------------------------------------------------------------------
  std::atomic<bool> any_parameter_changed{false};
  std::atomic<bool> preset_modified{true};

private:
  void thread_safe_set_value_tree_property(juce::ValueTree tree,
                                           const juce::Identifier &name,
                                           const juce::var &new_value,
                                           juce::UndoManager *undo_manager_);
  // state
  juce::ValueTree state_tree;
  std::unique_ptr<juce::AudioProcessorValueTreeState> param_tree_ptr;
  juce::ValueTree property_tree;
  std::unordered_map<juce::String, std::atomic<float>> property_atomics;
  std::unordered_map<juce::String, std::atomic<bool>> parameter_modified_flags;
  std::unordered_map<juce::Component *, std::function<void()>>
      param_to_callback[TOTAL_NUMBER_PARAMETERS] = {};

  juce::ValueTree preset_tree;

  // random number generator for randomizing parameters
  juce::Random rng;

  // Undo Manager
  juce::UndoManager undo_manager;

  std::shared_mutex state_mutex; // protect all the value trees.

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateManager)
};