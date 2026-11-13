#include "daisy_seed.h"
#include <cmath>

using namespace daisy;

// Define second order bandpass filter with difference equation
class BiquadBandPass
{
private:
    float b0_;
    float b1_;
    float b2_;
    float a1_;
    float a2_;

    float previous_input_1_;
    float previous_input_2_;
    float previous_output_1_;
    float previous_output_2_;

public:
    void Init(float center_frequency, float sample_rate, float Q = 1.0f)
    {
        constexpr float PI = 3.14159265f;

        float omega = 2.0f * PI * center_frequency / sample_rate;

        float cos_omega = cosf(omega);
        float sin_omega = sinf(omega);

        float alpha = sin_omega / (2.0f * Q);

        float a0 = 1.0f + alpha;

        // Bandpass numerator coefficients
        b0_ = alpha / a0;
        b1_ = 0.0f;
        b2_ = -alpha / a0;

        // Feedback denominator coefficients
        a1_ = (-2.0f * cos_omega) / a0;
        a2_ = (1.0f - alpha) / a0;

        Reset();
    }

    float Process(float input)
    {
        float output =
            b0_ * input
            + b1_ * previous_input_1_
            + b2_ * previous_input_2_
            - a1_ * previous_output_1_
            - a2_ * previous_output_2_;

        // Shift old samples
        previous_input_2_ = previous_input_1_;
        previous_input_1_ = input;

        previous_output_2_ = previous_output_1_;
        previous_output_1_ = output;

        return output;
    }

    void Reset()
    {
        previous_input_1_ = 0.0f;
        previous_input_2_ = 0.0f;
        previous_output_1_ = 0.0f;
        previous_output_2_ = 0.0f;
    }
};

DaisySeed hw;

BiquadBandPass bandpass_left;
BiquadBandPass bandpass_right;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        out[0][i] = bandpass_left.Process(in[0][i]);
        out[1][i] = bandpass_right.Process(in[1][i]);
    }
}

int main()
{
    hw.Init();

    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    hw.SetAudioBlockSize(48);

    float sample_rate = hw.AudioSampleRate();

    // Use second order bandpass function
    bandpass_left.Init(1000.0f, sample_rate, 2.0f);
    bandpass_right.Init(1000.0f, sample_rate, 2.0f);

    hw.StartAudio(AudioCallback);

    while(1){}
}