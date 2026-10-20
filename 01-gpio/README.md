# 01 - GPIO (Bare-Metal)

## What this does

Register-level GPIO on the Nucleo-F411RE, no HAL. Builds on the clock-enable / `MODER` pattern from `00-toolchain-setup` and goes from "blink the onboard LED" to driving pins atomically, reading inputs, and understanding why a floating input is a problem and how `PUPDR` fixes it.

## How I learned this chapter

Same method as chapter 00: read the GPIO section of the reference manual (RM0383), work out what each register is called and what it does, then write small exercises that poke one register at a time and watch what changes on the board.

That method worked well here because GPIO is a small, self-contained peripheral. There are only about ten registers and each one does one job. (It scales badly: for something like timers I leaned on AI almost entirely and only understand the basics, not what is actually going on. I'll be honest about that in those chapters.)

## Registers covered

| Register       | Job                                                                           |
| -------------- | ----------------------------------------------------------------------------- |
| `RCC->AHB1ENR` | Enables the port clock. Nothing works without it.                             |
| `GPIOx->MODER` | 2 bits per pin: `00` input, `01` output, `10` alternate function, `11` analog |
| `GPIOx->ODR`   | Output data. Read/modify/write to change pins.                                |
| `GPIOx->BSRR`  | Write-only set/reset. Bits 0-15 set a pin, bits 16-31 reset it.               |
| `GPIOx->IDR`   | Read the actual pin level.                                                    |
| `GPIOx->PUPDR` | 2 bits per pin: `00` none, `01` pull-up, `10` pull-down                       |

## Exercises

They loosely ramp up, but the real order is "each one exists because of the one before it".

### 1. LED

Configure a pin as output and drive an LED, first the onboard one, then an external one.

- Enable the port clock in `RCC->AHB1ENR`.
- Clear the pin's two `MODER` bits, then set them to `01`.
- Toggle with `ODR ^= (1U << pin)`.
- Use the `_Pos` / `_Msk` macros from the CMSIS headers instead of hardcoded shifts.

**Pin / wiring:** TODO (pin, resistor value, LED color)

### 2. BSRR

Same LED, but driven through `BSRR` instead of `ODR`.

`ODR` toggling is a read-modify-write: read the register, change a bit, write it back. If an interrupt fires in the middle and touches the same port, one of the two writes can be lost. `BSRR` avoids this because you write only the bits you want to change, and writing `0` to the rest does nothing. The lower half sets pins and the upper half (bits 16-31) resets them.

```c
GPIOA->BSRR = (1U << pin);          // set
GPIOA->BSRR = (1U << (pin + 16));   // reset
```

Observed: TODO

### 3. Floating input

Configure a pin as input (`MODER = 00`), with no pull resistor, and read it through `IDR`.

With nothing connected, the pin isn't at 0 or 1. It is just an antenna. The value I read depends on noise and what my hand or the breadboard is doing near it. This is the exercise that makes you understand why an unconnected input is a bug and not a neutral state.

Observed: TODO (what the LED did when the pin was left floating / touched)

### 4. PUPDR

The direct fix for exercise 3. Enable the internal pull-up or pull-down so the input has a defined idle level.

- Pull-up (`01`): idle reads `1`, button to GND reads `0`.
- Pull-down (`10`): idle reads `0`, button to 3.3V reads `1`.

With this, a push button finally behaves like a button. Same setup as exercise 3, one register changed, completely different behavior.

**Pin / wiring:** TODO

## Mentioned but not exercised

I read about these but did not build exercises for them. Noting them here so they aren't forgotten.

- **`OTYPER` / `OSPEEDR`:** push-pull vs open-drain, and output slew rate. Left at defaults.
- **Alternate functions (`AFRL` / `AFRH`):** setting `MODER` to `10` hands the pin to a peripheral, and the 4-bit fields in `AFRL` (pins 0-7) and `AFRH` (pins 8-15) choose which one (AF0-AF15). I'll use this properly in the UART and PWM chapters, where it actually matters.
- **`LCKR` (lock register):** a write sequence on bit 16 (`LCKK`) of 1, 0, 1, followed by a read, freezes the configuration of the chosen pins until the next reset. It's a safety feature against stray writes that would reconfigure a pin. Read it, understood the sequence, never needed it.

## What I got wrong / notes

- TODO: add anything that bit you during these exercises.

## Status

- Nucleo-F411RE: all four exercises done and verified on hardware.
- Blue Pill: not started for this chapter.

## Next

`02-...` TODO
