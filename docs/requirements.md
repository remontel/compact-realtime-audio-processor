# Project Requirements

This document defines the current baseline requirements for the Compact Real-Time Audio Processor.

Baseline requirements should be completed before work on stretch goals takes priority.

Some implementation details are still under team review and will be updated as the design develops.

---

## System Requirements

| ID     | Requirement                                                  | Verification                                                 |
| ------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| SYS-01 | The system shall operate as a standalone device without requiring a connected computer. | Demonstrate normal startup and operation without a host computer. |
| SYS-02 | The system shall support multiple simultaneous audio input channels and software-controlled mixing. | Operate multiple inputs simultaneously and verify independent contribution to the output mix. |
| SYS-03 | Desired end-to-end audio latency is less than 10 ms.         | Measure input-to-output latency under documented test conditions. |
| SYS-04 | The completed system shall be implemented using a custom multilayer PCB. | Inspect and test the manufactured PCB.                       |

---

## Audio Input Requirements

| ID       | Requirement                                                  | Verification                                                 |
| -------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| AUDIO-01 | The system shall accept a balanced XLR dynamic microphone input without requiring phantom power. | Apply a microphone or representative test signal and verify operation. |
| AUDIO-02 | The system shall accept a 1/4-inch high-impedance instrument input. | Apply an instrument or representative test signal and verify operation. |
| AUDIO-03 | The system shall accept a 1/8-inch stereo consumer line-level input. | Apply independent left and right line-level signals and verify both channels. |
| AUDIO-04 | Audio shall be sampled at 48 kHz.                            | Verify the configured and/or measured audio sample rate.     |

Input impedance, gain range, clipping level, and other analog performance targets will be defined as the analog front ends are designed.

---

## DSP Requirements

| ID     | Requirement                                                  | Verification                                                 |
| ------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| DSP-01 | The system shall provide a global three-band equalizer.      | Apply test signals and verify the expected frequency-response changes. |
| DSP-02 | The system shall provide an overdrive/distortion effect.     | Apply test signals and verify controllable nonlinear processing. |
| DSP-03 | The system shall provide a digital delay effect.             | Apply a transient or test signal and measure delayed output behavior. |
| DSP-04 | The baseline DSP chain shall operate in real time with multiple active input channels. | Run the baseline processing chain and check for audible or measured processing failures/dropouts. |

Exact effect parameters, ranges, and processing order will be defined during DSP implementation.

---

## User Interface Requirements

| ID    | Requirement                                                  | Verification                                                 |
| ----- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| UI-01 | The system shall provide physical controls for user interaction. | Demonstrate control of implemented audio or system parameters. |
| UI-02 | The system shall provide a graphical display for system and effect feedback. | Demonstrate visible feedback corresponding to system operation and controls. |

The final control mapping and UI behavior are still under development.

---

## Memory Requirements

| ID     | Requirement                                           | Verification                                                 |
| ------ | ----------------------------------------------------- | ------------------------------------------------------------ |
| MEM-01 | The system shall include external nonvolatile memory. | Verify communication with the selected memory and successful write/read operation. |

---

The exact use of external memory for presets, configuration, graphical assets, or other data will be defined as the firmware architecture develops.

---

## Output Requirements

| ID     | Requirement                                                  | Verification                                                 |
| ------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| OUT-01 | The baseline system shall support an integrated Class-D amplifier and full-range loudspeaker. | Apply processed audio and verify operation through the amplifier and speaker. |

> **TEAM REVIEW REQUIRED:** Determine whether stereo 1/4-inch line outputs will be included in addition to the integrated speaker system or whether the final output architecture will be revised.

---

## Design Selections

The following components are current project design selections rather than system requirements:

| Subsystem       | Current Selection        |
| --------------- | ------------------------ |
| Microcontroller | STM32H750IBT6            |
| Audio Codec     | PCM3168A                 |
| External Flash  | W25Q128JV                |
| SDRAM           | AS4C16M16SA              |
| Display         | Waveshare 2.42-inch OLED |

Changing one of these parts does not automatically change the project requirements, but major component substitutions should be reviewed by the team and documented.

---

## Stretch Goals

The following features are optional and should only be pursued after the
baseline system is working reliably:

- Reverb
- Chorus or other modulation effects
- Compression
- Noise gate
- Preset storage and recall
- SD-card recording and playback
- USB audio
- 48 V phantom power
- Stereo speaker output
- Advanced per-input DSP

Stretch goals should not delay completion or testing of the baseline system.
