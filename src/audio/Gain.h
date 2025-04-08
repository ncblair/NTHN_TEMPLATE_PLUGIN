#pragma once

class Gain {
public:
  Gain(float sample_rate, int samples_per_block, int num_channels, float default_gain_,
       float default_mix_);
  ~Gain();
  void process(float *const *buffer, const int numSamples, const int numChannels, const float gain,
               const float mix);
  void setState(const float gain, const float mix);

private:
  float smooth_gain;
  float smooth_mix;
  const float smooth_pole, default_gain, default_mix;
};