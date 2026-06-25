/**
 * @file knock_config.h
 * @brief Knock sensor ADC routing — custom STM32F767VIT6 ECU
 *
 * Two knock inputs are available on this board:
 *
 *   KNOCK1  PA2  ADC123_IN2  → bound to ADC3 (dedicated knock peripheral)
 *   AUX KNOCK  PA0  ADC123_IN0  → routed through ADC1 scan group
 *                                  (secondary / diagnostic only)
 *
 * Why ADC3 for KNOCK1?
 *   The rusEFI software knock subsystem triggers an ADC conversion on a
 *   per-tooth basis from within the trigger ISR.  It requires its own ADC
 *   peripheral so the DMA-driven ADC1 main scan group is never interrupted.
 *   PA2 is ADC123_IN2 — valid on ADC1, 2, or 3.  We bind it to ADC3 only.
 *   PA0 (AUX) can safely live on ADC1 as a normal slow-scan channel.
 *
 * Timing (STM32F767 @ 216 MHz, APB2 prescaler /2 = 108 MHz):
 *   ADC3 clock  = APB2 / 4 = 27 MHz
 *   Sample time = ADC_SAMPLE_84 cycles  → total = 84 + 12 = 96 cycles
 *   Sample rate = 27 000 000 / 96 ≈ 281 kSPS   Nyquist ≈ 140 kHz
 *   Knock band  ≈ 6–20 kHz  — well within Nyquist limit
 */

#pragma once

// Primary knock uses ADC3 (dedicated, no DMA, ISR-triggered)
#define KNOCK_ADC       ADCD3

// KNOCK1 — PA2, ADC3 channel IN2
#define KNOCK_ADC_CH1   ADC_CHANNEL_IN2
#define KNOCK_PIN_CH1   Gpio::A2

// 84-cycle sample time gives good SNR on knock band
#define KNOCK_SAMPLE_TIME   ADC_SAMPLE_84

// Effective sample rate in Hz (used by SW knock DSP init)
#define KNOCK_SAMPLE_RATE   (STM32_PCLK2 / (4 * (84 + 12)))

// AUX KNOCK (PA0) is handled as a regular ADC1 analog channel (EFI_ADC_0)
// and is NOT part of the ADC3 knock group above.
// Assign it in board_configuration.cpp as a spare analog input.
