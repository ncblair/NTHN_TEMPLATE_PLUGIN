#pragma once

class StateManager;

#include <juce_gui_basics/juce_gui_basics.h>

class ParameterSlider : public juce::SettableTooltipClient, public juce::Component {
public:
  ParameterSlider(StateManager *s, size_t p_id);
  ~ParameterSlider() override;
  void paint(juce::Graphics &g) override;
  void update_param_id(size_t p_id);
  void update_slider_sensitivity(float pixels_per_percent_);

  void mouseDown(const juce::MouseEvent &e) override;
  void mouseDrag(const juce::MouseEvent &e) override;
  void mouseDoubleClick(const juce::MouseEvent &e) override;
  void mouseUp(const juce::MouseEvent &e) override;

  enum ColourIds { backgroundColourId, sliderColourId };

protected:
  size_t param_id;
  StateManager *state;

private:
  void draw_rotary_slider(juce::Graphics &g, float normed_value, float x = 0.25f, float y = 0.25f,
                          float w = 0.5f, float h = 0.5f);
  float get_current_param_value_normalized(); // 0 to 1

  float pixels_per_percent{100.0f};
  juce::Point<int> mouse_down_position;
  float mouse_down_value{0.0f};
};