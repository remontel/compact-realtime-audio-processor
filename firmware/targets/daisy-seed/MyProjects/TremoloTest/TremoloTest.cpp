#include "daisy_seed.h"
#include <cmath>

using namespace daisy;

class Tremolo
{
public:
	void Init(float sample_rate, float rate_hz, float depth, float output_level = 1.0f)
	{
		sample_rate_ = sample_rate;
		rate_hz_ = rate_hz;
		output_level_ = output_level;

		// Keep depth between 0 and 1
		if(depth < 0.0f)
		{
			depth = 0.0f;
		}
		else if(depth > 1.0f)
		{
			depth = 1.0f;
		}

		depth_ = depth;

		// LFO phase increment
		phase_increment_ = 2.0f * PI * rate_hz_ / sample_rate_;

		phase_ = 0.0f;
	}

	float Process(float input)
	{
		// Generate a bipolar sine wave LFO
		float lfo = std::sin(phase_);

		// Convert LFO from [-1, 1] to [0, 1]
		float unipolar_lfo = 0.5f * (1.0f + lfo);

		// Calculate amplitude multiplier
		float amplitude = 1.0f - depth_ * unipolar_lfo;

		// Apply tremolo and output level
		float output = input * amplitude * output_level_;

		// Advance LFO phase
		phase_ += phase_increment_;

		// Keep phase between 0 and 2pi
		if(phase_ >= 2.0f * PI)
		{
			phase_ -= 2.0f * PI;
		}

		return output;
	}

private:
	static constexpr float PI = 3.14159265359f;

	float sample_rate_ = 48000.0f;
	float rate_hz_ = 5.0f;
	float depth_ = 0.5f;
	float output_level_ = 1.0f;

	float phase_ = 0.0f;
	float phase_increment_ = 0.0f;
};

DaisySeed hw;

Tremolo tremolo_left;
Tremolo tremolo_right;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = tremolo_left.Process(in[0][i]);
		out[1][i] = tremolo_right.Process(in[1][i]);
	}
}

int main(void)
{
	hw.Init();
	
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	hw.SetAudioBlockSize(48);

	float sample_rate = hw.AudioSampleRate();

	// 5 Hz rate
	// 75% depth
	// Output level of 1.0
	tremolo_left.Init(sample_rate, 5.0f, 0.75f, 1.0f);
	tremolo_right.Init(sample_rate, 5.0f, 0.75f, 1.0f);

	hw.StartAudio(AudioCallback);

	while(1) {}
}
