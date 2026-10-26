# Firmware

The final system targets the **STM32H750IBT6**, **PCM3168A** codec, and **48 kHz** audio. Baseline effects are three-band EQ, overdrive/distortion, and digital delay.

Prototype boards let us develop and test parts of the system before the final PCB is ready.

## Code Organization

Keep one buildable project per board and share reusable code between projects. Add folders when code is ready; do not create empty projects in advance.

```text
firmware/
├── shared/
│   ├── dsp/          # Effects and mixing
│   ├── app/          # Parameters and application behavior
│   ├── ui/           # Menus and display-independent UI logic
│   ├── drivers/      # Reusable device drivers
│   └── interfaces/   # Small interfaces between shared code and hardware
└── targets/
    ├── daisy-seed/
    ├── stm32h755-prototype/
    ├── stm32f411-display/
    ├── stm32h750vb-prototype/
    └── final-h750ib/
```

- Keep effects, parameters, and UI behavior in `shared/` when they can be reused.
- Keep clocks, pins, DMA, startup code, linker scripts, and board initialization in the target project.
- Have each target call shared code through a small hardware adapter. Reference shared source files using relative paths rather than copying them into each project.
- Keep shared DSP independent of Daisy board objects, STM32 peripheral handles, and display code. Define sample format, channel layout, sample rate, buffer size, and memory ownership clearly.

## Board Projects

Each target must include the files needed to build it and a short README covering:

- Exact board/revision, connected hardware, and wiring.
- Toolchain, CubeMX/firmware package, and dependency versions, as applicable.
- How to build, flash, and debug.
- Tests performed, known limitations, and work still needed for the final MCU.

Give each Cube-based target its own `.ioc` and project configuration. Keep application logic in dedicated modules or CubeMX `USER CODE` sections. Review generated changes carefully, especially clocks, DMA, audio interfaces, and pins.

Keep IDE workspaces outside the repository. Import projects from the clone without copying them into the workspace.

## What to Commit

Commit enough for a teammate to clone the repository and reproduce the documented build:

- Source and headers, including generated source needed to build.
- `.ioc` files, startup code, linker scripts, and build files.
- Required portable IDE project settings, including `.project` and `.cproject`.
- Dependency versions, licenses, and setup instructions. Pin dependencies to a specific version or commit.
- Project instructions and test procedures/results under `tests/`.

Do not commit build output (`Debug/`, `Release/`, `.elf`, `.hex`, `.bin`), workspace `.metadata/`, temporary files, credentials, or personal paths/settings. Review `git status` and `git diff` before committing.

Use task branches and reviewed PRs as described in [CONTRIBUTING.md](../CONTRIBUTING.md). Merge useful prototype work regularly; keep board projects together on `main` after review rather than maintaining permanent branches per board or person.

## Testing and Integration

Keep audio processing bounded: avoid blocking calls, logging, dynamic allocation, and display/UI work in the audio callback. Track processing time, memory use, DMA/cache handling, and latency as the audio path develops.

Test shared code on another target early. Prototype success does not verify final-board memory, timing, codec operation, or electrical behavior. State whether work is compiled, simulated, bench tested, or still untested, and identify remaining hardware validation.

See [requirements](../docs/requirements.md) and [architecture](../docs/architecture.md) for the current baseline. Record significant firmware architecture decisions in [docs/decisions/](../docs/decisions/).
