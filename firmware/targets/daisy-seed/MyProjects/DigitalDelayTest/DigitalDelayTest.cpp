#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// One second maximum delay at 48 kHz.
constexpr size_t MAX_DELAY_SAMPLES = 48000;

// Effect parameters
constexpr float DELAY_TIME_MS = 350.0f;	// Spacing between the original sound and the echo
constexpr float FEEDBACK = 0.35f;		// How much of the delayed signal is written back into the buffer, 0.0 generates one echo, near 1.0 could result in a runaway oscillation
constexpr float WET_LEVEL = 0.45f;		// Delayed signal volume
constexpr float DRY_LEVEL = 0.75f;		// Controls the original signal volume

// Use separate delay lines for left and right channels.
DelayLine<float, MAX_DELAY_SAMPLES> delay_left;
DelayLine<float, MAX_DELAY_SAMPLES> delay_right;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for(size_t i = 0; i < size; i++)
	{
		float input_left = in[0][i];
		float input_right = in[1][i];

		// Read the delayed samples before writing the current samples.
		float delayed_left = delay_left.Read();
		float delayed_right = delay_right.Read();

		// Write the current input plus a portion of the delayed signal.
		delay_left.Write(input_left + delayed_left * FEEDBACK);
		delay_right.Write(input_right + delayed_right * FEEDBACK);

		// Mix dry and delayed signals.
		out[0][i] = DRY_LEVEL * input_left + WET_LEVEL * delayed_left;
		out[1][i] = DRY_LEVEL * input_right + WET_LEVEL * delayed_right;
	}
}

int main()
{
	hw.Init();

	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	delay_left.Init();
	delay_right.Init();

	// Convert milliseconds into samples.
	float delay_samples = DELAY_TIME_MS * 0.001f * hw.AudioSampleRate();

	delay_left.SetDelay(delay_samples);
	delay_right.SetDelay(delay_samples);

	hw.StartAudio(AudioCallback);

	while(1) {}
}