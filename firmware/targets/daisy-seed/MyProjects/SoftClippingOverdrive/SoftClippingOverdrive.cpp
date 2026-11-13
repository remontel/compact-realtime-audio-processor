#include "daisy_seed.h"
#include <cmath>

using namespace daisy;

class SoftClipOverdrive
{
private:
	float drive_gain_ = 1.0f;
	float threshold_ = 1.0f;
	float output_level_ = 1.0f;

public:
	void Init(float drive_db, float threshold = 1.0f, float output_level = 1.0f)
	{
		threshold_ = threshold;
		output_level_ = output_level;

		// Convert decibels to linear amplitude gain
		drive_gain_ = std::pow(10.0f, drive_db / 20.0f);
	}

	float Process(float input)
	{
		// Apply drive gain and scale relative to threshold
		float scaled_input = (drive_gain_ * input) / threshold_;

		// Smooth nonlinear clipping
		float clipped_signal = std::tanh(scaled_input);

		// Scale back to the desired threshold
		clipped_signal *= threshold_;

		// Apply output level
		return clipped_signal * output_level_;
	}
};

DaisySeed hw;

SoftClipOverdrive overdrive_left;
SoftClipOverdrive overdrive_right;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = overdrive_left.Process(in[0][i]);
		out[1][i] = overdrive_right.Process(in[1][i]);
	}
}

int main(void)
{
	hw.Init();

	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	hw.SetAudioBlockSize(48);

	// 12 dB of drive
	// Clipping threshold of 1.0
	// Output level of 0.8
	overdrive_left.Init(12.0f, 1.0f, 0.8f);
	overdrive_right.Init(12.0f, 1.0f, 0.8f);

	hw.StartAudio(AudioCallback);

	while(1) {}
}