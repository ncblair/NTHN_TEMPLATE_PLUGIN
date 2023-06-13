#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class Gain {
  public:
    Gain(float sample_rate, int samples_per_block, int num_channels, float init_gain);
    ~Gain();
    void process(juce::AudioBuffer<float>& buffer);
    void setGain(float requested_gain_);
  private:
    float gain;
    float requested_gain;
    float iir_gamma;
    int output_channels;
};