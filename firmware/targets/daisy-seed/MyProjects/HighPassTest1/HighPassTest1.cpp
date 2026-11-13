#include "daisy_seed.h"
#include <cmath>

using namespace daisy;

// Define highpass filter with difference equation
class OnePoleHighPass
{
private:
	float alpha_;
	float previous_input_;
	float previous_output_;

public:
	void Init(float cutoff_frequency, float sample_rate)
	{
		constexpr float PI = 3.14159265f;

		alpha_ = expf(-2.0f * PI * cutoff_frequency / sample_rate);

		previous_input_ = 0.0f;
		previous_output_ = 0.0f;
	}

	float Process(float input)
	{
		float output = alpha_ * (previous_output_ + input - previous_input_);

		previous_input_ = input;
		previous_output_ = output;

		return output;
	}

	void Reset()
	{
		previous_input_ = 0.0f;
		previous_output_ = 0.0f;
	}
};

DaisySeed hw;

OnePoleHighPass highpass_left;
OnePoleHighPass highpass_right;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for(size_t i = 0; i < size; i++)
	{
		out[0][i] = highpass_left.Process(in[0][i]);
		out[1][i] = highpass_right.Process(in[1][i]);
	}
}

int main()
{
	hw.Init();

	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	hw.SetAudioBlockSize(48);

	float sample_rate = hw.AudioSampleRate();

	// Use highpass function
	highpass_left.Init(1000.0f, sample_rate);
	highpass_right.Init(1000.0f, sample_rate);

	hw.StartAudio(AudioCallback);

	while(1){}
}