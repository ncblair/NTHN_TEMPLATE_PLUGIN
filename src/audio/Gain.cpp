#include "Gain.h"
#include "../Util/Util.h"

Gain::Gain(float sample_rate, int, int, float default_gain_, float default_mix_)
    : smooth_pole(nthn_utils::tau2pole(0.05f, sample_rate)), default_gain(default_gain_),
      default_mix(default_mix_) {
  setState(default_gain, default_mix);
}

Gain::~Gain() {}

void Gain::process(float *const *buffer, const int numSamples, const int numChannels,
                   const float gain, const float mix) {
  // get variable smooth_gain in a register
  float local_gain = smooth_gain;
  float local_mix = smooth_mix;
  for (int i = 0; i < numSamples; ++i) {
    // smooth gain to next value using IIR
    local_gain = nthn_utils::lerp(gain, local_gain, smooth_pole);
    local_mix = nthn_utils::lerp(mix, local_mix, smooth_pole);
    // apply gain
    for (int c = 0; c < numChannels; ++c) {
      buffer[c][i] *= nthn_utils::lerp(1.0f, local_gain, local_mix);
    }
  }
  // store smooth gain state variable in object
  smooth_gain = local_gain;
  smooth_mix = local_mix;
}

void Gain::setState(const float gain, const float mix) {
  // force update, no smoothing applied
  smooth_gain = gain;
  smooth_mix = mix;
}