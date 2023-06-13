#include "Gain.h"

Gain::Gain(float sample_rate, int samples_per_block, int num_channels, float init_gain) {
    gain = init_gain;
    requested_gain = init_gain;
    // set iir filter s.t. we sample once per block and the filter cutoff is 1000hz
    iir_gamma = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 1000.0f * samples_per_block / sample_rate);
    output_channels = num_channels;
}

Gain::~Gain() {

}

void Gain::process(juce::AudioBuffer<float>& buffer) {
    float target_gain = gain * (1.0f - iir_gamma) + requested_gain * iir_gamma;
    if (std::abs(target_gain - requested_gain) < 0.001f) 
        target_gain = requested_gain;
    buffer.applyGainRamp(0, buffer.getNumSamples(), gain, target_gain);
    gain = target_gain;
}

void Gain::setGain(float requested_gain_) {
    requested_gain = requested_gain_;
}