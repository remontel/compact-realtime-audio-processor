# SSD1309 OLED bring-up

## Objective

Bring up the OLED in small steps: check power, verify control outputs, send commands with the panel off, then transfer a framebuffer and test a visible pattern.

Hardware and pin assignments are in the [firmware README](../../firmware/targets/stm32f411-display/README.md#wiring). The current setup uses the Black Pill 3.3 V rail and SPI1.

## Results — November 5, 2026

Results combine bench observations, build records and source inspection.

| Check | Setup and criterion | Actual result | What it establishes |
| --- | --- | --- | --- |
| Board rails | OLED disconnected; record DC voltages to GND | 5 V header: 4.93 V; 3V3 header: 3.29 V | Powered-board baseline; no formal rail tolerance was recorded |
| OLED power only | VCC/GND connected, five signal leads disconnected; compare with baseline | OLED VCC/GND: 3.29 V | Same reported rail voltage at meter resolution |
| GPIO initialization | OLED signals disconnected; reach main loop and check initial outputs | CS: 3.29 V; DC: 0 V; RST: 0 V | Initialization completed and levels matched the configured states |
| Reset/display off | Seven connections complete; execute reset and `0xAE`, inspect status | `HAL_OK`; VCC, CS and RST each 3.29 V | MCU-side transaction completed; controller receipt and pixels not verified |

The reset/display-off version used 10 ms low and high reset delays, 1 ms CS setup/hold delays and a 100 ms SPI timeout. CS returned high before the status check. These were bring-up implementation choices, not measured waveform timings.

## Implementation and build checkpoints

RAM figures include reserved heap and stack. They are static linker totals.

| Change | Flash (bytes) | RAM (bytes) | Validation recorded |
| --- | ---: | ---: | --- |
| Generated SPI/GPIO setup | 6,340 | 1,680 | Build passed; initialization and output-voltage checks completed |
| Inline reset/display-off transaction | 7,616 | 1,680 | Build passed; MCU-side execution returned `HAL_OK` |
| Extracted `OLED_WriteCommand` | 7,648 | 1,680 | Build passed; separate execution test not recorded |
| Added static framebuffer and block pattern | 7,684 | 2,704 | Build passed; no transfer result recorded at this checkpoint |
| Added data helper, address window and full transfer | 7,868 | 2,704 | Debug build record: exit 0; execution not yet verified |

The framebuffer occupies:

```text
128 columns x 64 rows x 1 bit/pixel ÷ 8 bits/byte = 1,024 bytes
```

The RAM increase from 1,680 to 2,704 bytes is exactly 1,024 bytes. Eight bytes set to `0xFF` at page 3, columns 60–67 describe a logical 8x8 block.

### Intermediate build and debugger checks

These checkpoints precede the visible U8g2 tests below. They establish build or debugger behavior only.

| Checkpoint | Result | Limit |
| --- | --- | --- |
| Nov. 6: diagnostic framebuffer transfer | Build exit 0; 7,868 bytes flash, 2,704 bytes static RAM | Transfer execution and visible pixels unverified |
| U8g2 import (date not recorded) | Library objects compiled; debugger stopped at `main()` before `HAL_Init()` | Application still used diagnostic helpers |
| Nov. 9: SPI callback | One verified 7,868-byte download; stopped at `main()` | Expected unused-function warning; callback not registered |
| Nov. 9: reset/delay callback | One verified 7,868-byte download; stopped at `main()` | Two unused-function warnings; callbacks not registered |

Download sizes are debugger-reported payloads. Earlier reset/display-off results apply only to that earlier implementation.

### November 12, 2026 — U8g2 initialization execution

**Objective and setup.** Verify that the SSD1309 noname0 full-buffer setup invokes the registered HAL callbacks and completes initialization on the F411. Current source contains the setup and InitDisplay calls.

**Procedure and expected result.** Break at the callback's SPI transmit call, inspect the message and byte count, remove that breakpoint and continue to the main loop. Both breakpoints should be reached without entering Error_Handler.

**Actual result.** `msg = 23` and `arg_int = 1` at the SPI send line, with U8g2 initialization in the call stack. The main-loop HAL_GetTick line was reached. This records one successful MCU-side initialization run.

**Conclusion.** Registration and initialization execution passed for this checkpoint. The byte count of one applies to the observed request. Controller receipt, visible orientation and illuminated-load behavior still require a panel test.

### November 12, 2026 — first visible U8g2 pattern

**Objective and setup.** Verify visible text and shape rendering with the SSD1309 noname0 full-buffer preset and U8G2_R0 on the F411. Source inspection confirms contrast 16, the 6x10 font with top positioning, text at (4, 4), and an 8x8 filled square at (20, 32). The OLED uses the board's 3.3 V rail. Tested commit/hash, meter model, run duration and current were not recorded.

**Procedure and expected result.** Clear the RAM image, draw “OLED OK” and the square, send the image, then enable panel output. Check readability and approximate positions. Measure VCC relative to GND with this sparse image visible.

**Actual result.** Bench observations report readable “OLED OK” at the top left and a square below it, shifted toward the center. OLED VCC to GND measured 3.29 V.

**Conclusion.** The U8g2/HAL path produced the intended sparse image with broadly correct position and readable text. The supply reading matches the earlier reported display-off voltage at the reported precision. Current consumption, rail transients, thermal margin, transfer duration and repeated-start reliability remain open.

### November 12, 2026 — visible counter updates

**Objective and setup.** Verify repeated image updates using the existing F411/U8g2 setup. Source inspection confirms the heartbeat value is formatted with snprintf into a 16-byte text array and drawn below “Count” after each nominal 1,000 ms heartbeat interval.

**Procedure and expected result.** Build through the STM32 CMake workflow, launch the ST-LINK debugger, and continue with loop breakpoints cleared. The OLED should show an advancing number.

**Actual result.** The displayed count advanced after continuing execution in the debugger. This is one reported successful run. Run duration, number of updates, digit-transition behavior, build memory totals and source hash were not recorded.

**Conclusion.** Repeated visible updates passed in the reported run. Update period accuracy, transfer duration and long-run reliability remain unmeasured.

### Meter manual review — November 12, 2026

The Etekcity MSR-R500 manual specifies a fused VΩmA input (315 mA fast fuse), a 200 mA DC range with 0.1 mA resolution and ±(1.2% of reading + 2 counts) accuracy, and an unfused 10 A input with 10 mA resolution. The 10 A range has a maximum 10-second continuous measurement and a minimum 15-minute interval. These are manual specifications, with the delivered fuse and meter condition unverified.

This meter supports the current prototype's basic DC voltage, continuity and bounded DC current checks. A supply-current test needs a reviewed range and series connection.

### November 12, 2026 — prototype controls ordered

**Purpose and status.** Temporary controls were ordered for breadboard UI development while the selected final parts are pending. Ordered; delivery and operation are unverified. No input GPIO assignments or control firmware have been added.

| Item | Procurement status | Intended role and limits |
| --- | --- | --- |
| [Cylewet KY-040 five-pack with knobs, CYT1062](https://www.amazon.com/dp/B06XQTHDRR) | Ordered | Temporary direction/count and knob-button input. Listing reviewed previously advertises 20 pulses per revolution and 5 V; delivered circuitry and 3.3 V interface suitability require inspection. |
| [QTEATAK two-pin tactile switches](https://www.amazon.com/dp/B07WF76VHT) | Ordered | Breadboard momentary-button exercises; verify released/pressed continuity on delivery. |
| Five tactile buttons from the Elegoo UNO R3 Super Starter Kit | Already available | Can support the first button exercise after contact identification; exact switch part number not recorded. |

The selected final parts remain [C&K PTS645SM70-2 LFS](https://www.digikey.com/en/products/detail/c-k/PTS645SM70-2-LFS/3861377) and [Bourns PEC11R-4215F-S0024](https://www.digikey.com/en/products/detail/bourns-inc/PEC11R-4215F-S0024/4499665). The [Kailh Choc v2 Brown](https://shop.keyboard.io/products/kailh-choc-v2-brown-tactile-switches-x-10?variant=31921939152969) remains a possible mechanical switch alternative. These substitutions do not qualify final mechanical fit, actuation feel, lifetime or PCB footprints.

**Compatibility review to complete on arrival.** With power disconnected, verify button contact behavior and inspect encoder pin labels and board components. Determine where the encoder board's pull-ups connect before selecting its supply; the 5 V listing is not approval to connect 5 V signals to the prototype. Select free MCU input pins only after reviewing the existing `.ioc` and OLED/SWD assignments. Measure and document the delivered encoder's transitions per click rather than assuming the final part's behavior. The [Bourns datasheet](https://www.bourns.com/docs/product-datasheets/pec11r.pdf) identifies the selected final encoder as 24 pulses per revolution, 24 detents and a momentary push switch.

**Next exercise.** Establish one reliable button action on the display, then add encoder rotation and its push button in separate tests. Follow the [resume checklist](README.md#resume-after-the-controls-arrive). No substitute-control bench result is claimed.

### November 13, 2026 — clean build before review

**Setup:** Current counter source prepared for the initial display prototype commit; GNU Tools for STM32 14.3.1+st.2, CMake 4.4.3 and Ninja 1.13.2. U8g2's 138 imported files matched the official 2.36.19 archive byte for byte.

**Procedure:** Configure a fresh out-of-tree Debug build with the target's GCC toolchain file and Ninja, then build. Expected result: successful compilation and linking.

**Result:** Build exited successfully with no compiler warnings in the build log. Linker allocation: 19,708 bytes flash and 3,264 bytes RAM, including configured heap/stack reservations. These are static totals, not measured runtime use.

**Conclusion:** Clean compilation passed. No flashing or hardware test was performed for this checkpoint; earlier bench results remain tied to their recorded checkpoints. Build artifacts and the local build log are not committed.

## Power findings

The earlier review identified two concerns that remain relevant:

- Shared 3.3 V power needs a load and thermal check. The board reference identifies an AP7343 regulator, but its total output rating is not spare capacity available to the OLED. Delivered regulator identity and thermal headroom remain unverified.
- A 5 V module supply cannot be assumed compatible with direct 3.3 V MCU signaling. The earlier module-schematic review identified a TXB0108 translation stage and pull-ups tied to module VCC. Keep the present supply arrangement until an alternative is electrically reviewed.

The prior review recorded a Waveshare example of about 196 mA with all pixels on at 3.3 V. It is a vendor example, not a guaranteed maximum or a measurement of this module. Using nominal 5 V input and 3.3 V output, the corresponding regulator dissipation contribution would be:

```text
P = (Vin − Vout) x I
  = (5.0 V − 3.3 V) x 0.196 A
  ≈ 0.333 W
```

This excludes the MCU and other board loads. The calculation explains why a stable display-off voltage does not qualify continuous full-screen illumination.

## Open work

The current U8g2 implementation has reported visible-pattern and counter-update results under [ADR-001](../../docs/decisions/ADR-001-u8g2-oled-display.md). The removed diagnostic transfer remains a historical build-only checkpoint; it does not need to be restored for the next exercise.

1. Measure transfer duration and record the build memory totals and tested source revision.
2. Exercise a displayed parameter controlled by a physical input.
3. Define illuminated-load acceptance limits from the verified power components, then measure current, rail stability and temperature under documented patterns and run times.
4. Repeat startup checks with a documented count and power-off interval.

Record each completed test in the results section above as a dated entry with setup, procedure, expected result, actual result and conclusion. Link evidence and identify the tested revision. Keep future procedures separate from completed results.

## References

- [Waveshare module wiki](https://www.waveshare.com/wiki/2.42inch_OLED_Module): module interface and example current consumption.

- [Waveshare module schematic](https://files.waveshare.com/wiki/2.42inch-OLED-Module/2.42inch-OLED-Module-Schematic.pdf): module power and signal translation.

- [SSD1309 datasheet](https://files.waveshare.com/wiki/2.42inch-OLED-Module/SSD1309Datasheet.pdf): reset, serial interface and commands.

- [AP7343 datasheet](https://www.diodes.com/datasheet/download/AP7343.pdf): regulator review.

- [TXB0108 datasheet](https://www.ti.com/lit/ds/symlink/txb0108.pdf): translation-stage review.

- [WeAct board references](https://github.com/WeActStudio/WeActStudio.MiniSTM32F4x1): board revision references.

- [WeAct Black Pill V3.0](https://stm32-base.org/boards/STM32F401CEU6-WeAct-Black-Pill-V3.0): board reference (F401 variant; not the tested F411 board)

- [Waveshare demo archive](https://files.waveshare.com/upload/2/2c/OLED_Module_Code.7z): F103 OLED example reviewed for understanding

- [U8g2 Github](https://github.com/olikraus/u8g2/wiki): U8g2 is a monochrome graphics library for embedded devices.

- [muimanual Github](https://github.com/olikraus/u8g2/wiki/muimanual): The monochrome minimal graphical user interface (MUI) is a small addon for u8g2 library. The purpose is to create interactive user interfaces with u8g2.

- [Etekcity Multimeter MSR-R500](https://images-na.ssl-images-amazon.com/images/I/A1agxml6wXL.pdf): link to manual
