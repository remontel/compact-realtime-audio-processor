#include "daisy_seed.h"
#include <cmath>

using namespace daisy;

// Define lowpass filter with difference equation
class OnePoleLowPass
{
private:
	float alpha_;
	float previous_output_;

public:
	void Init(float cutoff_frequency, float sample_rate)
	{
		constexpr float PI = 3.14159265f;

		alpha_ = expf(-2.0f * PI * cutoff_frequency / sample_rate);

		previous_output_ = 0.0f;
	}

	float Process(float input)
	{
		float output = (1.0f - alpha_) * input + alpha_ * previous_output_;

		previous_output_ = output;

		return output;
	}

	void Reset()
	{
		previous_output_ = 0.0f;
	}
};

DaisySeed hw;

OnePoleLowPass lowpass_left;
OnePoleLowPass lowpass_right;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for(size_t i = 0; i < size; i++)
	{
		out[0][i] = lowpass_left.Process(in[0][i]);
		out[1][i] = lowpass_right.Process(in[1][i]);
	}
}

int main()
{
	hw.Init();

	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	hw.SetAudioBlockSize(48);

	float sample_rate = hw.AudioSampleRate();

	// Use lowpass function
	lowpass_left.Init(1000.0f, sample_rate);
	lowpass_right.Init(1000.0f, sample_rate);

	hw.StartAudio(AudioCallback);

	while(1){}
}