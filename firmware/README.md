# Firmware

This directory contains the embedded firmware for the Compact Real-Time Audio Processor.

The firmware will run on the **STM32H750IBT6** and interface with the **PCM3168A** audio codec. The baseline audio sample rate is **48 kHz**.

Firmware structure will be added as implementation begins rather than creating empty directories in advance.

---

## Firmware Responsibilities

The firmware is expected to handle:

- MCU and peripheral initialization
- Audio codec configuration and control
- Real-time digital audio transport
- DMA-based audio buffering
- Software-controlled input mixing
- Real-time DSP
- Physical controls
- Graphical display
- External memory
- Overall system control

---

## Baseline DSP

The baseline DSP chain includes:

- Three-band EQ
- Overdrive / distortion
- Digital delay

The final processing order, parameters, and implementation details will be defined during DSP development.

---

## Real-Time Audio

Audio processing must remain predictable and low latency.

Avoid blocking operations, unnecessary logging, dynamic memory allocation, and display/UI work inside time-critical audio processing.

When implementing the audio path, consider:

- Buffer size
- Processing time
- CPU usage
- Memory usage
- DMA synchronization
- Added latency

More detailed implementation decisions should be documented when the audio engine exists.

---

## STM32CubeMX / STM32CubeIDE

STM32CubeMX configuration should be treated as part of the firmware source and tracked in Git.

Avoid unnecessary changes to generated code. Application logic should generally live in dedicated source files or CubeMX `USER CODE` sections so that project regeneration does not overwrite custom code.

Changes to clocks, DMA, audio peripherals, GPIO assignments, or external memory interfaces should be reviewed carefully because they may affect both firmware and hardware.

---

## Development

The exact development environment and project structure will be documented once the initial STM32 project is created.

At that point this README should include:

- Required STM32CubeIDE / STM32CubeMX versions
- STM32 firmware package version
- How to open and build the project
- How to flash and debug the MCU
- Required development hardware
- Any project-specific setup steps

See the project [requirements](../docs/requirements.md) and [architecture](../docs/architecture.md) for system-level context.
