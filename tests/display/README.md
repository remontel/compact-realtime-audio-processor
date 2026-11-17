# Display prototype

The display prototype uses a WeAct Black Pill V3.0 with an STM32F411CEU6 and a Waveshare 2.42-inch SSD1309 OLED. This lets us develop the display separately before moving it to the final STM32H750 system.

Work is tracked in [issue #10](https://github.com/remontel/compact-realtime-audio-processor/issues/10). The earlier TFT plan was retired on November 4, 2026.

## Start here

| Document                                                     | What it covers                                               |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [Firmware setup](../../firmware/targets/stm32f411-display/README.md) | Wiring, configuration, source files, build and debug commands |
| [MCU bring-up](mcu-bringup.md)                               | SWD connection, programming, heartbeat, reset and cold startup |
| [OLED bring-up](oled-bringup.md)                             | Power checks, SPI/GPIO checks, command transfer and framebuffer work |

## Current status

Status reviewed November 13, 2026. Bench results below identify the firmware checkpoint tested.

| Checkpoint | Status |
| --- | --- |
| Clean Debug build, November 13 | Passed; 19,708 bytes flash, 3,264 bytes static RAM |
| MCU programming and debugger execution | Passed with the heartbeat firmware |
| Software reset and cold startup | One successful cycle recorded for each; repeated cycles untested |
| OLED power and control pins | DC voltage checks recorded |
| U8g2 initialization and sparse image | One reported visible test: readable text and expected relative square position |
| Repeated counter updates | Working in one reported run; duration and update count not recorded |
| Buttons, encoder and navigation | Not implemented or bench tested; substitute controls ordered |
| Transfer timing, current and temperature | Not measured |

Earlier diagnostic-helper build results are historical checkpoints in the [OLED record](oled-bringup.md#implementation-and-build-checkpoints); those helpers have been replaced by U8g2.

## Resume after the controls arrive

1. Build and run the current counter firmware. Record the source revision and memory totals.
2. Identify button contacts with power disconnected, select a free GPIO, and test one action per press, including hold and release. Filter contact bounce.
3. Inspect the KY-040 pin labels and pull-ups before selecting its supply. Verify 3.3 V signal compatibility, direction, counts per click and push-button behavior.
4. Connect the controls to a displayed demo parameter. Define navigation before evaluating MUI.
5. Measure transfer duration and power behavior, and repeat startup checks. H750 audio integration remains separate work.

See the [control inventory](oled-bringup.md#november-12-2026--prototype-controls-ordered) for parts and compatibility checks.

## Scope

This prototype supports the baseline graphical interface. Audio processing, touch, SD cards, an RTOS and external-memory infrastructure are outside this work. Physical control mappings and final PCB pins are still undecided.

Reusable rendering and navigation will live in `firmware/shared/`. Board clocks, pins and peripheral setup stay with each target. F411 results do not establish H750 timing, memory placement or audio integration behavior.

## Quick reference

### GPIO and SPI

`HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);`
sets DC (PB0) LOW. `GPIO_PIN_SET` sets it HIGH. The pin macro selects a pin; `RESET` here means a low output level, not an MCU reset.

| Signal | Meaning |
| --- | --- |
| CS | LOW selects the OLED |
| DC | LOW for commands and parameters; HIGH for pixel data |
| RST | LOW asserts controller reset |
| CLK / DIN | SPI clock and outgoing data |
| VCC / GND | Supply and common reference; not control signals |

Use the [wiring guide](../../firmware/targets/stm32f411-display/README.md#wiring) for connections.

### C syntax used in this target

| Syntax | Quick meaning |
| --- | --- |
| `static` on a function | Callable only within its source file |
| `uint8_t`, `uint32_t` | Unsigned integers with 8 or 32 bits |
| `&value` | Address of an object |
| `uint8_t *data` | Pointer to bytes |
| `const uint8_t *data` | Bytes cannot be modified through this pointer |
| `1U` | Unsigned integer constant |
| `0xAE` | Hexadecimal value |
| `return status;` | Return a result to the caller |
| `sizeof(array)` | Array size in bytes; a pointer's size is not its data length |

A function definition declares parameters; a call supplies arguments in that order. Editor parameter hints are annotations, not source code.

```c
HAL_SPI_Transmit(&hspi1, data, length, 100U);
```

The arguments are the SPI handle address, data address, byte count and timeout in HAL tick milliseconds. This blocking call returns a HAL status. `HAL_OK` confirms MCU-side completion, not OLED acknowledgment.

### U8g2 callbacks and drawing

The setup function receives `OLED_U8g2Byte` and `OLED_U8g2GpioDelay` as function addresses, without calling them immediately.

| Callback input | Purpose |
| --- | --- |
| `display` | Library display state |
| `msg` | Requested action |
| `arg_int` | Byte count, pin level or delay, depending on the action |
| `arg_ptr` | Data address when needed |

The byte callback controls CS/DC and sends SPI data. The GPIO/delay callback handles reset and waits. Returning `1U` means the request was handled; it is not `HAL_OK` or an OLED acknowledgment. SPI errors enter `Error_Handler()`.

Drawing changes the RAM image. `u8g2_SendBuffer()` transfers that image through the callback and HAL to the screen. The 128 × 64 monochrome buffer uses 1,024 bytes.

The counter uses `snprintf` to format a number into a bounded text array. `%lu` matches the explicit `unsigned long` cast. Each update clears the buffer, draws the label and number, then sends the image. Clearing prevents old digits from remaining visible.

### Working method

For new controls, review the circuit and code, add one behavior, then test it before continuing. Start with one button action, then encoder rotation and its push button. Keep dated results in the test records.

## References

- [U8g2 decision](../../docs/decisions/ADR-001-u8g2-oled-display.md)
- [U8g2 C setup](https://github.com/olikraus/u8g2/wiki/u8g2setupc)
- [MCU porting guide](https://github.com/olikraus/u8g2/wiki/Porting-to-new-MCU-platform)
- [MUI manual](https://github.com/olikraus/u8g2/wiki/muimanual)
- [OLED test results and hardware references](oled-bringup.md)
