#pragma once

class Gain
{
public:
    Gain(float sample_rate, int samples_per_block, int num_channels, float default_gain_);
    ~Gain();
    void process(float *const *buffer, const int numSamples, const int numChannels, const float gain);
    void setState(const float gain);

private:
    float smooth_gain;
    const float smooth_pole, default_gain;
};