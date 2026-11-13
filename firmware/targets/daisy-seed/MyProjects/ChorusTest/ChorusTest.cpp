#include "daisy_seed.h"
#include <cmath>

using namespace daisy;

class Chorus
{
public:
	// Buffer supports approximately 50 ms of delay at a 48 kHz sample rate
	static constexpr size_t MaxDelaySamples = 2405;

	void Init(float sample_rate, float base_delay_ms, float depth_ms, float lfo_rate_hz, float wet_mix)
	{
		sample_rate_ = sample_rate;
		base_delay_ms_ = base_delay_ms;
		depth_ms_ = depth_ms;
		lfo_rate_hz_ = lfo_rate_hz;
		wet_mix_ = wet_mix;

		// Convert the LFO frequency to phase increment
		phase_increment_ = 2.0f * PI * lfo_rate_hz_ / sample_rate_;

		phase_ = 0.0f;

		write_index_ = 0;

		// Clear the delay buffer
		for(size_t i = 0; i < MaxDelaySamples; i++)
		{
			delay_buffer_[i] = 0.0f;
		}
	}

	float Process(float input)
	{
		//Write the current input sample into the delay buffer
		delay_buffer_[write_index_] = input;

		// Calculate the current delay time in milliseconds
		float current_delay_ms = base_delay_ms_ + depth_ms_ * std::sin(phase_);
		
		// Convert delay time from milliseconds to samples
		float delay_samples = current_delay_ms * sample_rate_ / 1000.0f;

		// Calculate where to read from the circular buffer
		float read_position = static_cast<float>(write_index_) - delay_samples;

		// Wrap the read position if it becomes negative
		if(read_position < 0.0f)
		{
			read_position += MaxDelaySamples;
		}

		// Integer portion
		size_t index_ = static_cast<size_t>(read_position);

		// Fractional portion
		float fraction = read_position - static_cast<float>(index_);

		// Next sample in the circular buffer
		size_t next_index = (index_ + 1) % MaxDelaySamples;

		// Linear interpolation
		float delayed_sample = (1.0f - fraction) * delay_buffer_[index_] + fraction * delay_buffer_[next_index];

		// Mix dry and wet signals
		float output = (1.0f - wet_mix_) * input + wet_mix_ * delayed_sample;

		// Advance the circular buffer write position
		write_index_ = (write_index_ + 1) % MaxDelaySamples;

		// Advance the LFO phase
		phase_ += phase_increment_;

		// Keep phase bounded between 0 and 2pi
		if(phase_ >= 2.0f * PI)
		{
			phase_ -= 2.0f * PI;
		}

		return output;
	}

private:
	static constexpr float PI = 3.14159265359f;

	float delay_buffer_[MaxDelaySamples] = {};

	float sample_rate_ = 48000.0f;
	float base_delay_ms_ = 20.0f;
	float depth_ms_ = 5.0f;
	float lfo_rate_hz_ = 0.5f;
	float wet_mix_ = 0.5f;

	float phase_ = 0.0f;
	float phase_increment_ = 0.0f;

	size_t write_index_ = 0;

};

DaisySeed hw;

Chorus chorus_left;
Chorus chorus_right;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = chorus_left.Process(in[0][i]);
		out[1][i] = chorus_right.Process(in[1][i]);
	}
}

int main(void)
{
	hw.Init();
	
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	hw.SetAudioBlockSize(48);

	float sample_rate = hw.AudioSampleRate();

	// Average delay of 20 ms
	// Modulation depth of 5 ms
	// LFO frequency of 0.5 Hz
	// 50% wet signal mix
	chorus_left.Init(sample_rate, 20.0f, 5.0f, 0.5f, 0.5f);
	chorus_right.Init(sample_rate, 20.0f, 5.0f, 0.5f, 0.5f);

	hw.StartAudio(AudioCallback);

	while(1) {}
}