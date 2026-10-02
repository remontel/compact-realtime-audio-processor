# Hardware

This directory contains the electrical hardware design for the Compact Real-Time Audio Processor.

Hardware files will be added as the individual subsystems are designed.

---

## Hardware Scope

The hardware design includes:

- Analog front ends for the audio inputs
- PCM3168A audio codec circuitry
- STM32H750IBT6 support circuitry
- External memory
- Power regulation and distribution
- Audio output circuitry
- Class-D amplifier and speaker circuitry, if retained in the final design
- User controls and display connections
- Custom multilayer PCB integration

See the project [requirements](../docs/requirements.md) and [architecture](../docs/architecture.md) for system-level context.

---

## Design Files

Editable design sources should be kept in the repository when practical.

For KiCad projects, this includes files such as:

- Schematics
- PCB layouts
- Project files
- Custom symbols
- Custom footprints
- Project-specific libraries

Do not replace editable design files with PDFs, screenshots, Gerbers, or other exports.

Manufacturing files should be generated from the reviewed source design when a PCB revision is ready for fabrication.

---

## Component Selection

Major component selections should be based on manufacturer documentation and checked for:

- Electrical compatibility
- Supply requirements
- Signal levels
- Package and footprint compatibility
- Interface requirements
- Availability
- Cost
- PCB complexity
- Manufacturability
- Testability

Do not assume a footprint is correct only because the package name appears to match. Verify manufacturer pin numbering and package dimensions.

---

## PCB Design

The project will eventually integrate the major subsystems onto a custom multilayer PCB.

Before fabrication, review the design for:

- Power distribution
- Grounding and return paths
- Analog and digital noise
- Gain staging and clipping
- Clock and digital-audio routing
- EMI
- Thermal behavior
- Connector placement
- Mechanical clearances
- Debug access and test points

Run KiCad ERC and DRC before releasing a board for fabrication.

Passing ERC or DRC does not prove that the circuit will function correctly, so the assembled hardware must also be tested.

---

## Hardware Validation

Hardware should be tested incrementally as subsystems become available.

Where practical, document:

- Test objective
- Equipment and setup
- Procedure
- Expected result
- Actual measurement or result
- Pass/fail conclusion

Prototype and bench-test individual subsystems before relying on the fully integrated PCB whenever possible.

---

## Repository Organization

Create hardware subdirectories as real designs are added.

For example, the repository may eventually include:

```text
hardware/
├── analog-front-end/
├── codec/
├── power/
├── amplifier/
├── pcb/
└── README.md
