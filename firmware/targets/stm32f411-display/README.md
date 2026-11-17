# STM32F411 display prototype

This target is used to bring up the SSD1309 OLED and develop the display interface before moving it to the final STM32H750 system. It uses a WeAct Black Pill V3.0 with an STM32F411CEU6.

For progress and bench results, start with the [display test overview](../../../tests/display/README.md). Work is tracked in [issue #10](https://github.com/remontel/compact-realtime-audio-processor/issues/10).

## What the firmware does

The firmware initializes GPIO, SPI1 and the U8g2 SSD1309 noname0 full-buffer preset through two HAL callbacks. It displays a text-and-square startup image at contrast 16, then redraws a heartbeat counter approximately once per second.

Bench observations confirm a visible pattern and advancing counter. See the [OLED test record](../../../tests/display/oled-bringup.md#november-12-2026--visible-counter-updates).

The target has no audio processing, touch, SD support or RTOS. The final system remains STM32H750IBT6 / PCM3168A / 48 kHz.

## Project files

| File or directory                               | Purpose                                                      |
| ----------------------------------------------- | ------------------------------------------------------------ |
| `stm32f411-display.ioc`                         | CubeMX configuration source                                  |
| `Core/Src/main.c`                               | U8g2 setup, HAL callbacks and visible heartbeat counter |
| `Core/Inc/main.h`                               | Generated control-pin definitions                            |
| `Core/Src/stm32f4xx_hal_msp.c`                  | Peripheral clocks and SPI pin setup                          |
| `Drivers/`                                      | ST HAL and CMSIS dependencies                                |
| `CMakeLists.txt`, `CMakePresets.json`, `cmake/` | Build configuration                                          |
| `STM32F411xx_FLASH.ld`, `startup_stm32f411xe.s` | Memory layout and startup                                    |
| `.settings/`, `.vscode/`                        | Selected tools and debug configuration                       |

Keep application changes in dedicated modules or CubeMX `USER CODE` sections. Review generated changes after regeneration. Future shared rendering and navigation belong in `firmware/shared/`, referenced by relative paths rather than copied between targets.

## Configuration

| Item            | Setting                                                      |
| --------------- | ------------------------------------------------------------ |
| MCU             | STM32F411CEU6; build definition `STM32F411xE`                |
| Clock           | HSI; nominal 16 MHz SYSCLK, HCLK, PCLK1 and PCLK2; no PLL    |
| Debug           | Serial Wire on PA13/PA14; SysTick timebase                   |
| SPI1            | Full-duplex master, 8-bit, MSB first, mode 0, software NSS   |
| SPI clock       | Prescaler 16: nominal 16 MHz / 16 = 1 MHz; not measured      |
| Control outputs | CS initially high; DC and RST initially low; push-pull, no pull, low GPIO speed |
| SPI pins        | AF5, generated very-high GPIO speed; this setting controls edge rate, not SCK frequency |
| Linker regions  | 512 KiB flash, 128 KiB RAM                                   |
| Reservations    | 512-byte minimum heap, 1,024-byte stack; not measured peak usage |
| Debug build     | GCC/Ninja, `-O0 -g3`, Cortex-M4 hard-float                   |

## Wiring

Make wiring changes with both USB supplies disconnected. Read the actual pin legends rather than inferring orientation from a photograph.

### Programmer

| Dongle pin   | Black Pill debug header |
| ------------ | ----------------------- |
| 2 — SWCLK    | SWSCK / PA14            |
| 4 — SWDIO    | SWDIO / PA13            |
| 5 or 6 — GND | GND                     |

Power the Black Pill through its USB connector. Leave dongle RST, SWIM and pins 7–10 disconnected, including its supply outputs. Do not join supply outputs. The recorded setup uses a direct USB-to-USB-C adapter for the programmer.

### OLED

The recorded module is the Waveshare 2.42-inch SSD1309 OLED with its two selector links reported in SPI positions.

| OLED label | Black Pill connection | Purpose                                 |
| ---------- | --------------------- | --------------------------------------- |
| VCC        | 3V3                   | Module supply                           |
| GND        | GND                   | Common reference                        |
| CLK        | A5 / PA5              | SPI clock; mode 0 idles low             |
| DIN        | A7 / PA7              | Serial data from MCU                    |
| CS         | A4 / PA4              | Low selects the OLED                    |
| DC         | B0 / PB0              | Low for commands, high for display data |
| RST        | B1 / PB1              | Low holds the controller in reset       |

PA6 is configured as MISO but remains unconnected. The OLED path used here is write-only: `HAL_OK` confirms MCU-side transfer completion, not controller acknowledgment.

This is the staged bring-up supply arrangement. Rail voltage has been checked with the display off and with a sparse image visible. Illuminated-load current and temperature are still unverified. Do not treat it as approval for unrestricted all-white operation or substitute a 5 V module supply without reviewing the interface. See [power findings](../../../tests/display/oled-bringup.md#power-findings).

## Build

The selected tools and recorded setup are:

| Dependency                | Version                                         |
| ------------------------- | ----------------------------------------------- |
| CubeMX                    | 6.18.1; database DB.6.0.181                     |
| STM32CubeF4               | V1.28.3; automatic latest-version selection off |
| GNU Tools for STM32       | 14.3.1+st.2                                     |
| ST-managed CMake          | 4.3.1+st.1                                      |
| ST-managed Ninja          | 1.13.2+st.1                                     |
| ST VS Code extension pack | 3.11.0, recorded during bring-up                |
| CubeProgrammer            | 2.23.0                                          |

Open this target folder in VS Code and select the Debug preset and managed compiler. For terminal builds, put the selected tool executables on PATH and check their versions first. Reconfigure the CMake cache when changing compilers. The first command-line build used a different compiler; see [build history](../../../tests/display/mcu-bringup.md#build-history).

The debug launch reads this target's `.vscode/launch.json` and resolves `${workspaceFolder}/build/Debug/stm32f411-display.elf` relative to the target folder. To browse the shared code in the same window, open this target first and use **File → Add Folder to Workspace** to add `firmware/shared` as a separate folder. Select the F411 project when building or debugging.

Run from this target directory:

```sh
arm-none-eabi-gcc --version
cmake --version
ninja --version
cmake --preset Debug
cmake --build --preset Debug
arm-none-eabi-size build/Debug/stm32f411-display.elf
```

The ELF, map and stack-usage output are under the ignored `build/Debug/` directory. Static memory totals do not measure peak stack or runtime memory use.

## Program and debug

Close other debugger/programmer sessions first. Confirm whether the board's existing firmware needs to be preserved before replacing it.

With the selected programmer executable on PATH:

```sh
STM32_Programmer_CLI -c port=SWD mode=NORMAL freq=100 -w build/Debug/stm32f411-display.elf -v
```

This command writes and verifies the ELF.

In VS Code, select **Run and Debug → F411: Debug at main (ST-LINK)**. The supplied launch configuration builds, downloads, verifies, performs a software system reset and stops at `main`. It uses 140 kHz SWD; the CLI command above uses 100 kHz.

Use CMake **Build** and the named ST-LINK debug configuration. The editor's Code Runner **Run Code** action invokes a host compiler and is not the firmware build/flash workflow. Clear loop and SPI breakpoints before observing continuous counter updates.

Add `bringup_heartbeat` to WATCH. Continue, pause and record its value; repeat to check that it advances. Reset and cold-start procedures are in [MCU bring-up](../../../tests/display/mcu-bringup.md). Do not change protection, option bytes or probe firmware as an implicit troubleshooting step.

## Dependencies and licenses

ST HAL/CMSIS files were copied from STM32CubeF4 V1.28.3. Preserve component notices under `Drivers/`, generated-file notices and [package license terms](Package_license.md). The package license inventory includes components this target does not use. U8g2 source and its `u8g2_font_6x10_tf` font are imported through the shared dependency described below.

The selected U8g2 release is 2.36.19. The top-level CMake file compiles `../../shared/third_party/u8g2/csrc/*.c` and adds that directory to the include path. All 138 imported upstream files were verified against the official release archive on November 13, 2026; see [provenance](../../shared/third_party/u8g2/PROVENANCE.md). Preserve the upstream [license](../../shared/third_party/u8g2/LICENSE), source/font notices and [README](../../shared/third_party/u8g2/README.md). The selection rationale is in [ADR-001](../../../docs/decisions/ADR-001-u8g2-oled-display.md).

## Pending control integration

The current checkpoint is a working display counter. Ordered breadboard controls are recorded in the [OLED record](../../../tests/display/oled-bringup.md#november-12-2026--prototype-controls-ordered); none is wired or configured yet. Follow the [resume checklist](../../../tests/display/README.md#resume-after-the-controls-arrive) for button verification, encoder electrical review and incremental UI testing.

SPI transfers and callback delays currently block the calling code, and SPI failures enter `Error_Handler`. These are isolated bring-up choices; transfer duration and input responsiveness must be measured before extending the UI, and display operations must stay outside the final real-time audio path.

Commit source and reproducible project configuration, but keep build outputs, local IDE workspaces and machine-specific tool paths out of Git.
