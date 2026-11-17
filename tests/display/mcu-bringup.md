# F411 MCU bring-up

## Objective

Verify that the F411 can be identified, programmed and debugged, then check that the firmware starts after a software reset and a power cycle. Display operation is covered separately in [OLED bring-up](oled-bringup.md).

## Setup

- WeAct Black Pill V3.0, STM32F411CEU6. The board revision was corrected by physical inspection on November 5, 2026; earlier V2.0 notes are superseded.
- ST-LINK-compatible dongle connected through a direct USB-to-USB-C adapter.
- Three SWD connections and MCU USB power, as listed in the [firmware guide](../../firmware/targets/stm32f411-display/README.md#wiring).
- CubeProgrammer 2.23.0 and the ST VS Code debugger.
- Original heartbeat firmware built with managed GNU Tools for STM32 14.3.1+st.2. Later OLED revisions are separate checkpoints.

## Procedure and pass criteria

Use a known heartbeat revision for an isolated MCU test. The current target also performs OLED transactions before entering the loop, so a failure before the heartbeat may involve display initialization.

| Step           | Procedure                                                    | Pass criterion                                               |
| -------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Identify       | Connect through Normal SWD at 100 kHz without programming    | Tool reads the expected F411 identity and flash size         |
| Build          | Configure and build the selected Debug preset                | Build completes successfully; record compiler and memory summary |
| Program        | Download the intended ELF with verification                  | Programmer reports successful verification                   |
| Execute        | Stop at `main`, continue, pause, record heartbeat; repeat    | Heartbeat increases between observations                     |
| Software reset | Reset, inspect initial heartbeat, continue and inspect again | Heartbeat returns to zero and then advances                  |
| Cold startup   | Stop debug, remove both USB supplies, restore power, then read heartbeat twice using Hot Plug | Heartbeat advances without a download or reset operation in the read logs |

For cold-start reads, resolve the heartbeat address from the tested ELF. The address in the historical result below is not a fixed address for future builds. Record the power-off interval and repeat count in future tests.

## Results

### November 2, 2026 — connection and execution

**Connection problem.** Initial enumeration produced `DEV_USB_COMM_ERR`. Probe identification succeeded twice after switching to a direct adapter and disconnecting the target, though USB timeout warnings remained. Because both connections changed together, that test did not isolate a single cause.

The first target connection failed to read the core ID. After correcting miswired pins, the same Normal SWD / 100 kHz command succeeded without USB warnings in that run.

| Check              | Recorded result                                     | Interpretation                                       |
| ------------------ | --------------------------------------------------- | ---------------------------------------------------- |
| Target identity    | ID `0x431`, Rev A, `STM32F411xC/E`, 512 KBytes      | Successful SWD identification                        |
| Flash verification | 4,856-byte heartbeat ELF; exit 0; download verified | Programming passed; log reported sector 0 erase only |
| Debugger launch    | Stopped at `main`, then continued into the loop     | Launch and initialization passed                     |
| Heartbeat          | 13 → 20; observed HAL tick value: 20,052 | Main loop and HAL tick advanced                      |
| Software reset     | 0 → 10 after reset and resume                       | One reset/restart cycle passed                       |

Programming used no mass erase or option-byte changes.

### November 4, 2026 — cold startup

After a reported power cycle, two four-byte Hot Plug reads at 100 kHz returned:

| Historical address | First read            | Second read           | Change |
| ------------------ | --------------------- | --------------------- | ------ |
| `0x2000002C`       | `0x0000568E` = 22,158 | `0x000056B4` = 22,196 | +38    |

Both logs recorded successful reads without a download or reset operation. This supports one standalone startup and continued execution. The printed reset-mode field was a connection setting, not evidence of a reset command.

The elapsed wall time was not measured. This test does not establish oscillator accuracy or repeated-cycle reliability.

## Build history

These sizes belong to the revisions listed, not to the current OLED-transfer source. RAM includes configured heap and stack reservations, not measured peak usage.

| Date   | Revision description                       | Compiler                        | Flash (bytes) | RAM (bytes) |
| ------ | ------------------------------------------ | ------------------------------- | ------------: | ----------: |
| Nov. 2 | First generated target, command-line build | Arm GNU 15.3.rel1               |         4,908 |       1,584 |
| Nov. 2 | Generated target, VS Code build            | GNU Tools for STM32 14.3.1+st.2 |         4,812 |       1,584 |
| Nov. 2 | Heartbeat added                            | GNU Tools for STM32 14.3.1+st.2 |         4,856 |       1,592 |

The initial managed-tool configure reported an unused `CMAKE_COMMAND` variable. No compiler warning was reported. The heartbeat rebuild was recorded without warnings.

## Remaining checks

- Repeat software resets and cold starts with a documented count and power-off interval.
- Link future logs and screenshots to the tested revision.
- Treat timing accuracy as a separate measurement if it becomes a requirement.

Board-rail measurements taken on November 5 are recorded in [OLED bring-up](oled-bringup.md). The dongle's earlier voltage display was not an independent rail measurement.
