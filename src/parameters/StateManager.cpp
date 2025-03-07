// Nathan Blair January 2023

#include "StateManager.h"
#include "../plugin/PluginProcessor.h"
#include "../plugin/ProjectInfo.h"
#include <cassert>

StateManager::StateManager(PluginProcessor *proc)
    : PRESETS_DIR(
          juce::File::getSpecialLocation(juce::File::SpecialLocationType::userMusicDirectory)
              .getChildFile(juce::String(JucePlugin_Manufacturer) + "_plugins")
              .getChildFile(JucePlugin_Name)
              .getChildFile("presets")),
      PRESET_EXTENSION("." + juce::String(JucePlugin_Name).toLowerCase()) {
  //==============================================================================
  //-> ADD PARAMS/PROPERTIES
  //==============================================================================
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  property_tree = juce::ValueTree(PROPERTIES_ID);

  for (size_t p_id = 0; p_id < PARAM::TOTAL_NUMBER_PARAMETERS; ++p_id) {
    if (PARAMETER_AUTOMATABLE[p_id]) {
      params.push_back(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{PARAMETER_NAMES[p_id], ProjectInfo::versionNumber}, // parameter ID
          PARAMETER_NICKNAMES[p_id],                                            // parameter name
          PARAMETER_RANGES[p_id],                                               // range
          PARAMETER_DEFAULTS[p_id],                                             // default value
          "", // parameter label (description?)
          juce::AudioProcessorParameter::Category::genericParameter,
          [this, p_id](float value, int maximumStringLength) {
            std::array<float, TOTAL_NUMBER_PARAMETERS> dependencies_values;
            const int num_dependencies = PARAMETER_DEPENDENCY_IDS[p_id].size();
            for (int i = 0; i < num_dependencies; ++i) {
              dependencies_values[i] =
                  param_value(get_parameter_index(PARAMETER_DEPENDENCY_IDS[p_id][i].toString()));
            }
            return PARAMETER_VALUE_TO_STRING_FUNCTIONS[p_id](
                value, maximumStringLength, dependencies_values.data(), num_dependencies);
          },
          PARAMETER_STRING_TO_VALUE_FUNCTIONS[p_id]));
    } else {
      property_tree.setProperty(PARAMETER_IDS[p_id], PARAMETER_DEFAULTS[p_id], nullptr);
      property_atomics[PARAMETER_NAMES[p_id]].store(PARAMETER_DEFAULTS[p_id]);
    }
    parameter_modified_flags[PARAMETER_NAMES[p_id]].store(false);
  }

  param_tree_ptr.reset(new juce::AudioProcessorValueTreeState(*proc, &undo_manager, PARAMETERS_ID,
                                                              {params.begin(), params.end()}));
  property_tree.addListener(this);

  for (size_t p_id = 0; p_id < PARAM::TOTAL_NUMBER_PARAMETERS; ++p_id) {
    if (PARAMETER_AUTOMATABLE[p_id]) {
      param_tree_ptr->addParameterListener(PARAMETER_NAMES[p_id], this);
    }
  }

  //==============================================================================
  //-> SETUP PRESETS
  //==============================================================================
  preset_tree = juce::ValueTree(PRESET_ID);
  preset_tree.setProperty(PRESET_NAME_ID, DEFAULT_PRESET, nullptr);
}

StateManager::~StateManager() {
  property_tree.removeListener(this);
  for (size_t p_id = 0; p_id < PARAM::TOTAL_NUMBER_PARAMETERS; ++p_id) {
    if (PARAMETER_AUTOMATABLE[p_id]) {
      param_tree_ptr->removeParameterListener(PARAMETER_NAMES[p_id], this);
    }
  }
}

float StateManager::param_value(size_t param_id) {
  // returns the parameter value of a certain ID in a thread safe way
  if (PARAMETER_AUTOMATABLE[param_id]) {
    return param_tree_ptr->getRawParameterValue(PARAMETER_NAMES[param_id])->load();
  } else {
    return property_atomics[PARAMETER_NAMES[param_id]].load();
  }
}

juce::AudioProcessorValueTreeState *StateManager::get_param_tree() { return param_tree_ptr.get(); }

juce::ValueTree StateManager::get_property_tree() { return property_tree; }

juce::ValueTree StateManager::get_preset_tree() { return preset_tree; }

juce::ValueTree StateManager::get_state() {
  state_tree = juce::ValueTree(STATE_ID);
  state_tree.appendChild(param_tree_ptr->copyState(), nullptr);
  state_tree.appendChild(property_tree.createCopy(), nullptr);
  state_tree.appendChild(preset_tree.createCopy(), nullptr);
  return state_tree;
}

void StateManager::save_preset(juce::String preset_name) {
  // not undo-able
  preset_tree.setProperty(PRESET_NAME_ID, preset_name, nullptr);
  preset_tree.setProperty(PRESET_MODIFIED_ID, false, nullptr);
  auto file = PRESETS_DIR.getChildFile(preset_name).withFileExtension(PRESET_EXTENSION);
  if (!PRESETS_DIR.exists()) {
    // create directory if it doesn't exist
    PRESETS_DIR.createDirectory();
  }
  if (!file.existsAsFile()) {
    // create file
    file.create();
  }
  auto plugin_state = get_state();

  std::unique_ptr<juce::XmlElement> xml(plugin_state.createXml());
  auto temp = juce::File::createTempFile("preset_temp");
  xml->writeTo(temp);
  temp.replaceFileIn(file);
}

void StateManager::load_preset(juce::String preset_name) {
  auto file = PRESETS_DIR.getChildFile(preset_name).withFileExtension(PRESET_EXTENSION);
  if (file.existsAsFile()) {
    std::unique_ptr<juce::XmlElement> xmlState = juce::XmlDocument::parse(file);
    load_from(xmlState.get());
  }
}

void StateManager::load_from(juce::XmlElement *xml) {
  if (xml != nullptr) {
    if (xml->hasTagName(STATE_ID)) {
      auto new_tree = juce::ValueTree::fromXml(*xml);
      param_tree_ptr->state.copyPropertiesAndChildrenFrom(new_tree.getChildWithName(PARAMETERS_ID),
                                                          &undo_manager);
      property_tree.copyPropertiesFrom(new_tree.getChildWithName(PROPERTIES_ID), &undo_manager);
      preset_tree.copyPropertiesFrom(new_tree.getChildWithName(PRESET_ID), &undo_manager);
      preset_modified.store(false);
    }
  }
}

void StateManager::set_preset_name(juce::String preset_name) {
  preset_tree.setProperty(PRESET_NAME_ID, preset_name, &undo_manager);
}

juce::String StateManager::get_preset_name() {
  if (bool(preset_tree.getProperty(PRESET_MODIFIED_ID))) {
    return preset_tree.getProperty(PRESET_NAME_ID).toString() + "*";
  } else {
    return preset_tree.getProperty(PRESET_NAME_ID).toString();
  }
}

void StateManager::update_preset_modified() {
  // called from UI thread - updates preset_modified property, if the preset has been modified
  if (preset_modified.exchange(false)) {
    preset_tree.setProperty(PRESET_MODIFIED_ID, true, nullptr);
  }
}

bool StateManager::get_parameter_modified(size_t param_id, bool exchange_value) {
  return parameter_modified_flags[PARAMETER_NAMES[param_id]].exchange(exchange_value);
}
size_t StateManager::get_parameter_index(juce::String param_name) {
  auto it = std::find(PARAMETER_NAMES.begin(), PARAMETER_NAMES.end(), param_name);
  if (it != PARAMETER_NAMES.end()) {
    return std::distance(PARAMETER_NAMES.begin(), it);
  }
  return PARAM::TOTAL_NUMBER_PARAMETERS;
}

void StateManager::undo() { undo_manager.undo(); }

void StateManager::redo() { undo_manager.redo(); }

juce::RangedAudioParameter *StateManager::get_parameter(size_t param_id) {
  assert(PARAMETER_AUTOMATABLE[param_id]);
  return param_tree_ptr->getParameter(PARAMETER_NAMES[param_id]);
}

void StateManager::begin_change_gesture(size_t param_id) {
  undo_manager.beginNewTransaction();
  if (PARAMETER_AUTOMATABLE[param_id]) {
    auto parameter = get_parameter(param_id);
    parameter->beginChangeGesture();
  }
}

void StateManager::end_change_gesture(size_t param_id) {
  if (PARAMETER_AUTOMATABLE[param_id]) {
    auto parameter = get_parameter(param_id);
    parameter->endChangeGesture();
  }
}

void StateManager::set_parameter(size_t param_id, float value) {
  if (PARAMETER_AUTOMATABLE[param_id]) {
    auto range = PARAMETER_RANGES[param_id];
    auto normalized_value = range.convertTo0to1(range.snapToLegalValue(value));
    set_parameter_normalized(param_id, normalized_value);
  } else {
    property_tree.setProperty(PARAMETER_IDS[param_id], value, &undo_manager);
  }
}

void StateManager::set_parameter_normalized(size_t param_id, float normalized_value) {
  if (normalized_value > 1.0)
    normalized_value = 1.0;
  else if (normalized_value < 0.0)
    normalized_value = 0.0;
  if (PARAMETER_AUTOMATABLE[param_id]) {
    auto parameter = get_parameter(param_id);
    parameter->setValueNotifyingHost(normalized_value);
  } else {
    auto unnormalized_value = PARAMETER_RANGES[param_id].convertFrom0to1(normalized_value);
    set_parameter(param_id, unnormalized_value);
  }
}

void StateManager::randomize_parameter(size_t param_id, float min, float max) {
  // min, max between 0 and 1
  jassert(min >= 0.0f && max <= 1.0f && max >= min);
  auto value = rng.nextFloat() * (max - min) + min;
  if (PARAMETER_AUTOMATABLE[param_id]) {
    auto parameter = get_parameter(param_id);
    parameter->setValueNotifyingHost(value);
  } else {
    auto range = PARAMETER_RANGES[param_id];
    auto scaled_val = range.convertFrom0to1(value);
    property_tree.setProperty(PARAMETER_IDS[param_id], scaled_val, &undo_manager);
  }
}

juce::String StateManager::get_parameter_text(size_t param_id) {
  return get_parameter(param_id)->getText(
      PARAMETER_RANGES[param_id].convertTo0to1(param_value(param_id)), 20);
}
void StateManager::reset_parameter(size_t param_id) {
  set_parameter(param_id, PARAMETER_DEFAULTS[param_id]);
}

void StateManager::init() {
  for (size_t i = 0; i < PARAM::TOTAL_NUMBER_PARAMETERS; ++i) {
    reset_parameter(i);
  }
  // reset value trees
  set_preset_name(DEFAULT_PRESET);
  preset_tree.setProperty(PRESET_MODIFIED_ID, false, &undo_manager);
  preset_modified.store(false);
}

void StateManager::randomize_parameters() {
  for (size_t i = 0; i < PARAM::TOTAL_NUMBER_PARAMETERS; ++i) {
    randomize_parameter(i);
  }
}

juce::UndoManager *StateManager::get_undo_manager() { return &undo_manager; }

void StateManager::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged,
                                            const juce::Identifier &property) {
  // this will be polled by UI to update when UI changes
  if (treeWhosePropertyHasChanged != preset_tree) {
    preset_modified.store(true);
    any_parameter_changed.store(true);
    if (treeWhosePropertyHasChanged == property_tree) {
      property_atomics[property.toString()].store(float(property_tree.getProperty(property)));
      parameter_modified_flags[property.toString()].store(true);
    }
  }
}

void StateManager::parameterChanged(const juce::String &parameterID, float newValue) {
  // parameter changed, note as modified
  // might be called from audio thread, so must be thread safe
  preset_modified.store(true);
  any_parameter_changed.store(true);
  parameter_modified_flags[parameterID].store(true);
  juce::ignoreUnused(newValue);
}

void StateManager::register_component(size_t param_id, juce::Component *component,
                                      std::function<void()> custom_callback)
// custom_callback is called when the parameter changes. default is to just repaint the component
// call this only one time per per component parameter pair
{
  assert(param_id <= TOTAL_NUMBER_PARAMETERS);
  assert(param_to_callback[param_id].find(component) == param_to_callback[param_id].end());
  if (custom_callback)
    param_to_callback[param_id][component] = custom_callback;
  else
    param_to_callback[param_id][component] = [component]() { component->repaint(); };
}

void StateManager::unregister_component(size_t param_id, juce::Component *component) {
  assert(param_id <= TOTAL_NUMBER_PARAMETERS);
  param_to_callback[param_id].erase(component);
}