# Testing

This directory contains test procedures, measurements, and results for the Compact Real-Time Audio Processor.

Testing should be added as real subsystems become available rather than creating procedures for hardware or firmware that does not yet exist.

---

## Testing Approach

Subsystems should be tested individually where practical before full-system
integration.

Examples of testing that may be performed during the project include:

- Power-supply verification
- Analog input gain and clipping
- Audio codec bring-up and pass-through
- Audio channel routing
- DSP effect validation
- User-interface operation
- External memory operation
- Audio output verification
- End-to-end latency
- Frequency response
- Noise
- Clipping behavior
- Stability
- Thermal performance
- Integrated system operation

Exact procedures and acceptance criteria should be defined when the relevant
subsystem is ready to test.

---

## Test Records

Create a separate directory for a significant test or test group.

Example:

```text
tests/
├── README.md
├── audio-passthrough/
│   ├── procedure.md
│   └── results.md
└── latency/
    ├── procedure.md
    ├── results.md
    └── measurements.csv
