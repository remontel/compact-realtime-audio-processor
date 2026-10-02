# Engineering Decisions

This folder contains Architecture Decision Records (ADRs) for significant engineering decisions made during the project.

ADRs are intended to preserve the reasoning behind decisions that affect the system architecture, multiple subsystems, or other long-term aspects of the design.

They are not required for every implementation detail.

---

## When to Create an ADR

Consider creating an ADR when the team makes a significant decision such as:

- selecting or replacing a major component
- choosing an audio interface or clocking architecture
- defining the power architecture
- choosing the final audio output architecture
- selecting a PCB stackup
- making a major memory or firmware architecture decision
- changing an established system architecture

Small implementation decisions can normally remain in GitHub Issues, Pull Requests, code comments, or subsystem documentation.

---

## Workflow

1. Create or use a GitHub Issue to discuss the decision.
2. Compare the reasonable alternatives and tradeoffs.
3. Once the team reaches a decision, copy `ADR-template.md`.
4. Rename it using the next available number and a short description.

Example:

```text
ADR-001-audio-output-architecture.md
