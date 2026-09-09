# 00 - Toolchain Setup (Bare-Metal, No IDE)

## What this does

Sets up a full bare-metal STM32 toolchain from the command line — no
CubeIDE, no CubeMX-generated project. Covers CMSIS headers, linker
scripts, a hand-written Makefile, and register-level GPIO code, built
and verified on both a Blue Pill (STM32F103C8T6) and a Nucleo-F411RE.

## Why bare-metal, why no IDE

Wanted to actually understand what a toolchain does before letting an
IDE do it for me — clock trees, memory layout, linking, flashing. CubeIDE
would have hidden all of this behind generated code. This chapter is the
foundation every later chapter (GPIO, PWM, UART, etc.) builds on, so the
plan was to build it manually once and reuse it going forward.

## Toolchain

- arm-none-eabi-gcc
- GNU Make
- OpenOCD (flashing/debugging)
- External ST-Link (Nucleo's onboard one bypassed — see wiring notes)
- VS Code + Git, Python for supporting scripts

## Repo structure

```
common/
  F103/    <- CMSIS headers + startup file for Blue Pill
  F411/    <- CMSIS headers + startup file for Nucleo
00-toolchain-setup/
  Makefile
  linker_f103.ld
  linker_f411.ld
  main.c
```

CMSIS headers and startup files were pulled directly from ST's official
GitHub repos — kept at the repo
root under `common/` so every chapter can reference them by relative path
instead of duplicating them per-chapter.

## Linker scripts — written from scratch

No template used. Each `MEMORY` and `SECTIONS` block was built by reading
the actual datasheet/memory map for that chip.

- **Blue Pill (F103C8T6):** 64K flash / 20K SRAM. Worth noting: it's easy
  to assume 128K flash from the "C8" naming pattern used on other STM32
  lines — the actual datasheet number is 64K. Confirmed against the real
  memory map diagram rather than assuming.
- **Nucleo (F411RE):** 512K flash / 128K SRAM.
- Caught and fixed two mismatches by cross-checking against the actual
  startup file: `ENTRY(_reset)` should have been `ENTRY(Reset_Handler)`,
  and `.vectors` should have been `.isr_vector`. Both were silent
  failures until traced back to the startup file's real symbol names.

## Hardware wiring — external ST-Link on the Nucleo

Removed the Nucleo's onboard ST-Link (CN2 jumpers) and wired an external
ST-Link to PA13/PA14/GND/3.3V via the Morpho header, so the external
debugger drives SWD instead of the onboard one.

**Note on CN2 vs CN4:** CN4 is the onboard ST-Link's own SWD output —
it's for using the Nucleo's ST-Link to program _other_ boards, not for
feeding an external ST-Link into this one. The correct connector for an
external ST-Link on this board is the Morpho header pins. Worth writing
down explicitly here since it's an easy mix-up if you're going off a
diagram without checking which direction the signal actually flows.

## Makefile — written from zero, debugged from real errors

No boilerplate reused. Issues hit and fixed by reading the actual
compiler/linker output, not by guessing:

- Spaces in folder names breaking path resolution
- A missing `mpu_armv7.h` include
- A malformed line in the linker script caught only once GCC pointed at
  the exact line

## main.c — register-level GPIO

No HAL calls. Direct register manipulation:

- `RCC->AHB1ENR` — enable the GPIO port clock (nothing works until this
  is set — this was the first real "aha" of the chapter)
- `GPIOx->MODER` — clear the two bits for the pin, then set them to
  output mode
- `GPIOx->ODR` — XOR to toggle the pin state in the main loop

Learned to read the `_Pos` / `_Msk` macros in the CMSIS headers directly
instead of hardcoding bit shifts, which is what makes the register code
portable across pins/ports.

## What I got wrong first / debugging notes

- Had CMSIS, HAL, and bare-metal conceptually confused at the start —
  specifically had HAL and bare-metal's roles backwards. Now clear:
  CMSIS = names/addresses for registers, HAL = ST's abstraction over
  register behavior, bare-metal = writing directly to those registers
  with only CMSIS naming as a convenience.
- Linker script `ENTRY`/section name mismatches above.
- ST-Link wasn't recognized initially — fixed via a driver reinstall in
  Device Manager, not a wiring issue.

## Verification

Flashed and ran on the Nucleo. Confirmed this was actually running my
code (not the board's factory-loaded demo) by changing the delay
constant and watching the blink rate change accordingly.

## Status

- Blue Pill: linker script done, blocked on further chapters until wired
  up (currently prioritizing Nucleo).
- Nucleo: fully verified, blink running on custom bare-metal code.

## Next

`01-gpio` — builds directly on the clock-enable / MODER pattern from
this chapter, moving from onboard LED to external LEDs + push buttons
(input handling).
