# ADR-001: Use U8g2 for the OLED display

- **Status:** Accepted
- **Date:** 2026-11-09
- **Related Issue:** [#10](https://github.com/remontel/compact-realtime-audio-processor/issues/10)

## Context

The processor needs visible feedback for its physical controls and effects (UI-01 and UI-02) while the audio processing chain runs in real time (DSP-04). The display prototype uses a Waveshare 128×64 SSD1309 OLED over SPI on an STM32F411. The final target is the STM32H750. At the November 9 decision checkpoint, the prototype had compiled a diagnostic framebuffer transfer and still needed a visible panel test. [Requirements](../requirements.md) · [Display prototype](../../tests/display/README.md)

## Options considered

| Option | Tradeoff |
| --- | --- |
| Extend diagnostic code | Simple HAL integration, but requires maintaining initialization, graphics and fonts |
| [stm32-ssd1306](https://github.com/afiskon/stm32-ssd1306) | Direct HAL integration; upstream lists SSD1309 compatibility through its SSD1306 driver |
| [U8g2](https://github.com/olikraus/u8g2/wiki/u8g2setupc) | Explicit SSD1309 presets, graphics and fonts; requires board-specific callbacks |
| [LVGL](https://github.com/lvgl/lvgl) | Broader widget system with more integration work than this prototype needs |

All options require validation on the actual panel.

## Decision

Use U8g2 for OLED drawing and controller communication. Start with release 2.36.19 on the F411. Store a single upstream copy in `firmware/shared/third_party/u8g2/` and compile it only in targets that use it. Keep the STM32 HAL connection specific to each target. The optional U8g2 MUI menu library can be evaluated when the control behavior is defined. [U8g2 release](https://github.com/olikraus/u8g2/releases/tag/2.36.19)

## Rationale

The selected library already supplies the graphics and controller functions needed for a small monochrome display. The F411 can reuse its configured SPI peripheral and GPIO outputs. A board-specific connection lets the application reuse the drawing code when moving to the H750. The existing diagnostic code provides a reference for investigating failures at the SPI and GPIO boundary.

## Consequences

- Record the dependency version and retain upstream notices for the source and any selected fonts. [U8g2 license](https://github.com/olikraus/u8g2/blob/2.36.19/LICENSE)
- On the F411, verify the panel preset, orientation, supply behavior and transfer duration with a sparse image. Then verify that a displayed parameter follows its control.
- On the H750, schedule screen updates outside the deadline-sensitive audio work. Verify audio block timing and continuity with screen updates and baseline effects active. F411 results cover prototype display behavior.

## Validation follow-up — November 12, 2026

The F411 noname0 full-buffer preset produced a readable sparse image and repeated visible counter updates in reported bench runs. Detailed evidence and limitations are in the [OLED test record](../../tests/display/oled-bringup.md#november-12-2026--visible-counter-updates). These results support continuing the selected library; control input, transfer timing, illuminated-load power and operation alongside H750 audio remain unverified.
