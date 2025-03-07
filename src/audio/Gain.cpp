#include "Gain.h"
#include "../Util/Util.h"

Gain::Gain(float sample_rate, int, int, float default_gain_)
    : smooth_pole(nthn_utils::tau2pole(0.05f, sample_rate)), default_gain(default_gain_) {
  setState(default_gain);
}

Gain::~Gain() {}

void Gain::process(float *const *buffer, const int numSamples, const int numChannels,
                   const float gain) {
  // get variable smooth_gain in a register
  float local_gain = smooth_gain;
  for (int i = 0; i < numSamples; ++i) {
    // smooth gain to next value using IIR
    local_gain = nthn_utils::lerp(gain, local_gain, smooth_pole);

    // apply gain
    for (int c = 0; c < numChannels; ++c) {
      buffer[c][i] *= local_gain;
    }
  }
  // store smooth gain state variable in object
  smooth_gain = local_gain;
}

void Gain::setState(const float gain) {
  // force update, no smoothing applied
  smooth_gain = gain;
}