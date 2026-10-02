# Compact Real-Time Audio Processor

CSUN ECE 492/493 Senior Design Project

## Overview

The Compact Real-Time Audio Processor is a standalone embedded audio system designed to accept multiple common audio sources, perform real-time digital signal processing, and reproduce the processed audio.

The project combines analog audio circuitry, embedded firmware, digital signal processing, user-interface design, power electronics, and custom PCB development.

---

## Baseline System

The system is being designed around:

- STM32H750IBT6 microcontroller
- PCM3168A multichannel audio codec
- 48 kHz audio sample rate
- Multiple simultaneous audio inputs
- Software-controlled mixing
- Three-band EQ
- Overdrive / distortion
- Digital delay
- Physical controls and graphical UI
- External nonvolatile memory
- Custom multilayer PCB
- Desired end-to-end latency below 10 ms

### Audio Inputs

- Balanced XLR dynamic microphone
- 1/4-inch high-impedance instrument input
- 1/8-inch stereo line-level input

The final output architecture is still under team review.

Additional effects, recording features, USB audio, phantom power, and other enhancements are considered stretch goals and will not take priority over completing the baseline system.

## System Architecture

At a high level:

Audio Inputs → Analog Front End → Audio Codec → STM32 DSP → Audio Codec → Output Stage

Physical controls, the display, external memory, and power system interface with the appropriate processing and hardware subsystems.

See [docs/architecture.md](docs/architecture.md) for the current system architecture.

---

## Development Tools

The project currently uses:

- Git and GitHub for version control and collaboration
- STM32CubeMX / STM32CubeIDE for STM32 configuration and firmware development
- KiCad for schematic and PCB design

Additional setup instructions will be added as the firmware and hardware projects are created.

---

## Repository

| Directory     | Purpose                                               |
| ------------- | ----------------------------------------------------- |
| `firmware/`   | STM32 firmware and DSP                                |
| `hardware/`   | Schematics, PCB design, and electrical hardware       |
| `mechanical/` | Enclosure and mechanical design                       |
| `tests/`      | Test procedures and results                           |
| `docs/`       | Requirements, architecture, and engineering decisions |

---

## Contributing

Development follows an Issue → Branch → Pull Request → Review → Merge workflow.

See [CONTRIBUTING.md](CONTRIBUTING.md) before making your first change.

Project requirements and current architecture are documented in:

- [Requirements](docs/requirements.md)
- [Architecture](docs/architecture.md)

---

## Project Resources

Supporting course documents and larger project files are maintained in the team's Google Drive. Discord is used for team communication.

**Important engineering decisions and implementation changes should be captured in GitHub rather than existing only in chat.**
