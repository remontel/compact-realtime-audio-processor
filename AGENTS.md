# AGENTS.md

This file contains project-level instructions for AI agents working in this repository.

---

## Project

This repository contains the CSUN ECE 492/493 Compact Real-Time Audio Processor senior design project.

The system is a standalone STM32-based embedded audio processor integrating analog audio circuitry, a multichannel audio codec, real-time DSP, physical controls, a graphical interface, power electronics, and a custom PCB.

Current baseline selections include:

- MCU: STM32H750IBT6
- Audio codec: PCM3168A
- Sample rate: 48 kHz
- Baseline DSP: three-band EQ, overdrive/distortion, and digital delay

Baseline functionality takes priority over stretch goals.

---

## Before Making Changes

Read only the documentation relevant to the task.

Important project references include:

- `README.md` — project overview
- `docs/requirements.md` — baseline requirements and scope
- `docs/architecture.md` — current system architecture
- `docs/decisions/` — accepted engineering decisions
- `CONTRIBUTING.md` — repository workflow

Do not assume older notes or unfinished ideas override the current documented architecture.

---

## Engineering Accuracy

Never invent:

- component specifications
- pinouts
- electrical limits
- timing requirements
- register behavior
- clock capabilities
- interface capabilities
- package dimensions
- datasheet values

Use manufacturer datasheets, reference manuals, application notes, and other primary engineering sources when electrical or hardware specifications matter.

If a required value cannot be verified, identify it as unknown or TBD rather than guessing.

Clearly distinguish calculations, estimates, simulations, measurements, and verified hardware results.

---

## Architecture and Scope

Do not silently change established system architecture or project requirements.

If a task conflicts with an existing requirement or documented decision:

1. identify the conflict,
2. explain the consequences,
3. stop before making the conflicting architectural change unless explicitly instructed.

Do not introduce stretch-goal functionality at the expense of baseline functionality or project reliability.

---

## STM32 Firmware

Treat STM32CubeMX configuration as engineering source.

Do not casually modify:

- MCU clock configuration
- DMA assignments
- SAI/I2S configuration
- GPIO assignments
- interrupt configuration
- external memory interfaces
- codec timing or audio framing

Preserve CubeMX regeneration-safe code organization.

Prefer application code in dedicated modules or designated `USER CODE` sections rather than editing generated regions unnecessarily.

Review generated diffs carefully after CubeMX regeneration.

---

## Real-Time Audio

Code executing in the real-time audio path must remain bounded and predictable.

Avoid where practical:

- blocking operations
- dynamic memory allocation
- console logging
- display/UI operations
- unnecessary peripheral access

When modifying the audio path, consider:

- sample rate
- DMA buffer size
- processing time
- CPU load
- memory use
- synchronization
- cache coherency where applicable
- added latency

DSP changes should document their algorithm, state requirements, and validation method when relevant.

---

## Hardware and PCB

Verify hardware changes against the applicable datasheet or reference design.

Consider where relevant:

- voltage and current limits
- gain staging and clipping
- power supplies
- grounding
- analog/digital noise
- EMI
- thermal behavior
- connector interfaces
- PCB routing
- manufacturability
- testability

Do not assume a footprint is correct solely because the package name appears compatible.

Verify manufacturer pin numbering and package dimensions before approving a footprint.

Do not resolve schematic or PCB merge conflicts by blindly selecting one version.

---

## Testing

Never claim that something works unless it was actually tested.

Report exactly what validation was performed.

When relevant, distinguish between:

- implemented
- compiled
- simulated
- bench tested
- hardware verified
- untested

Hardware-dependent behavior that cannot currently be tested should be explicitly identified as requiring later validation.

---

## Documentation

Update relevant documentation when an implementation change makes it inaccurate.

Significant engineering decisions should be recorded under `docs/decisions/`.

Useful engineering records may include:

- design rationale
- calculations
- test procedures
- measurements
- problems encountered
- changes made
- final results

These records may later support ECE 492/493 reports and presentations.

---

## Git and Repository Changes

Keep changes focused on the requested task.

Do not mix unrelated cleanup or refactoring into a functional change unless explicitly requested.

Follow `CONTRIBUTING.md` for branch and Pull Request workflow.

Do not commit:

- credentials or secrets
- personal information
- machine-specific paths
- temporary files
- build artifacts
- IDE caches
- unintentional generated files

Do not push directly to `main`.

Do not merge a Pull Request on behalf of the user unless explicitly requested.

---

## Definition of Done

Before reporting a task as complete:

1. Review the changed files and diff.
2. Run available build or validation steps where applicable.
3. Confirm unrelated files were not modified.
4. Update relevant documentation.
5. Report tests actually performed.
6. Identify anything still requiring hardware validation.
7. Call out assumptions, limitations, or unresolved engineering questions.
