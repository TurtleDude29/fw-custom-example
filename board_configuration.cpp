/**
 * @file board_configuration.cpp
 * @brief Hardware pin configuration — custom STM32F767VIT6 ECU
 *
 * MCU:  STM32F767VIT6  LQFP100  216 MHz Cortex-M7
 *       2 MB Flash, 512 KB RAM
 *       Available GPIO ports on LQFP100: A, B, C, D, E, H
 *       (Ports F, G, I, J, K are NOT bonded out on LQFP100)
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * COMPLETE PIN OWNERSHIP TABLE
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * IGNITION OUTPUTS (logic-level, active-high → external igniters)
 *   IGN1  PC13     IGN3  PE4      IGN4  PE3
 *   IGN5  PE2      IGN6  PB8
 *   NOTE: IGN2 position not populated on this board.
 *         ignitionPins[1] left Unassigned.
 *
 * INJECTOR OUTPUTS (low-side drivers)
 *   INJ1  PD3      INJ2  PA9      INJ3  PD11
 *   INJ4  PD10     INJ5  PD2      INJ6  PA8
 *
 * LOW-SIDE / RELAY OUTPUTS
 *   LOWSIDE1   PD15  (generic low-side, e.g. fan, AC clutch)
 *   IDLE VALVE PD12  (PWM idle solenoid — TIM4_CH1 AF2)
 *   LOWSIDE2   PD13  (generic low-side)
 *   FP RELAY   PC6   (fuel pump relay)
 *   MAIN RELAY PB9   (main power relay)
 *
 * ANALOG INPUTS  (ADC channel / EFI_ADC_n)
 *   TPS1       PA4   ADC12_IN4    EFI_ADC_4
 *   TPS2       PB0   ADC12_IN8    EFI_ADC_8   (spare analog IN2)
 *   PPS1       PA3   ADC123_IN3   EFI_ADC_3   (spare analog IN1)
 *   PPS2       PC5   ADC12_IN15   EFI_ADC_15  (spare analog IN3)
 *   IAT        PC3   ADC123_IN13  EFI_ADC_13
 *   CLT        PC2   ADC123_IN12  EFI_ADC_12
 *   EXT MAP    PC0   ADC123_IN10  EFI_ADC_10
 *   AUX KNOCK  PA0   ADC123_IN0   EFI_ADC_0   (ADC1 slow-scan)
 *   MAF        PC4   ADC12_IN14   EFI_ADC_14
 *   KNOCK1     PA2   ADC3_IN2     → ADC3 ONLY (see knock_config.h)
 *
 * DIGITAL / FREQUENCY INPUTS
 *   CRANK HALL PE13  — trigger decoder (Hall, no VR conditioner)
 *   CAM HALL   PE12  — trigger decoder
 *   VSS        PE14  — vehicle speed sensor (frequency input)
 *   FLEX FUEL  PE11  — ethanol content sensor (frequency input)
 *   CLUTCH SW  PB1   — clutch switch (digital in)
 *   BUTTON 2   PA6   — user button (digital in)
 *   BUTTON 3   PE15  — user button (digital in)
 *
 * COMMUNICATION
 *   CAN1       PD0 (RX) / PD1 (TX)  AF9  — OpenBLT + runtime CAN
 *   USB OTG-FS PA11 (DM) / PA12 (DP)      — TunerStudio virtual serial
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * TIMER / DMA OWNERSHIP
 * ═══════════════════════════════════════════════════════════════════════════
 *   TIM1   rusEFI event scheduler         — DO NOT TOUCH
 *   TIM2   Trigger decoder (PE13/PE12)    — DO NOT TOUCH
 *   TIM3   PWM aux outputs (boost etc.)
 *   TIM4   Idle valve PWM  PD12 CH1 AF2
 *   TIM8   Ignition dwell scheduler       — DO NOT TOUCH
 *   ADC1 + DMA2/Stream0/Ch0  — main analog scan
 *          channels: IN0(PA0) IN3(PA3) IN4(PA4) IN8(PB0)
 *                    IN10(PC0) IN12(PC2) IN13(PC3) IN14(PC4) IN15(PC5)
 *   ADC3   Knock only — PA2 (ADC3_IN2), no DMA, ISR-triggered
 *   USART3 — spare / wideband UART if needed
 */

#include "pch.h"
#include "board_overrides.h"

// ============================================================================
// Status LEDs
// ============================================================================
// PD15 is LOWSIDE1 on this board — no dedicated comms LED pin.
// Map comms LED to Unassigned; the LOWSIDE1 output is claimed below.

Gpio getCommsLedPin() {
    return Gpio::Unassigned;
}

Gpio getRunningLedPin() {
    return Gpio::Unassigned;
}

Gpio getWarningLedPin() {
    return Gpio::Unassigned;
}

// ============================================================================
// Board output metadata (for rusEFI pin-state checker / diagnostics)
// Lists every low-side output the board physically drives.
// Ordered: injectors → ignition → aux low-sides → relays
// ============================================================================
static Gpio BOARD_META_OUTPUTS[] = {
    // Injectors
    Gpio::D3,   // INJ1
    Gpio::A9,   // INJ2
    Gpio::D11,  // INJ3
    Gpio::D10,  // INJ4
    Gpio::D2,   // INJ5
    Gpio::A8,   // INJ6
    // Ignition
    Gpio::C13,  // IGN1
    Gpio::E4,   // IGN3
    Gpio::E3,   // IGN4
    Gpio::E2,   // IGN5
    Gpio::B8,   // IGN6
    // Low-side / relay outputs
    Gpio::D15,  // LOWSIDE1
    Gpio::D12,  // IDLE VALVE
    Gpio::D13,  // LOWSIDE2
    Gpio::C6,   // FP RELAY
    Gpio::B9,   // MAIN RELAY
};

int getBoardMetaOutputsCount() {
    return efi::size(BOARD_META_OUTPUTS);
}

Gpio* getBoardMetaOutputs() {
    return BOARD_META_OUTPUTS;
}

// ============================================================================
// Trigger inputs — Hall sensors only (no VR conditioner on this board)
// ============================================================================
static void setTriggerInputs() {
    // CRANK HALL — PE13
    // STM32F767 LQFP100: PE13 supports TIM1_CH3 (AF1) and EXTI13.
    // rusEFI trigger decoder uses TIM2 input capture or EXTI depending on
    // build config.  Hall sensor output is a clean 3.3 V digital edge;
    // set pullup/pulldown to match your sensor's idle state.
    engineConfiguration->triggerInputPins[0] = Gpio::E13;

    // CAM HALL — PE12
    // PE12 supports TIM1_CH3N (AF1) / EXTI12.
    engineConfiguration->triggerInputPins[1] = Gpio::E12;

    // Second cam / additional sync — not populated
    engineConfiguration->triggerInputPins[2] = Gpio::Unassigned;

    // Hall sensors produce clean edges — no inversion needed
    engineConfiguration->invertPrimaryTriggerSignal   = false;
    engineConfiguration->invertSecondaryTriggerSignal = false;
}

// ============================================================================
// Analog inputs
// ============================================================================
// All channels except KNOCK1 are in the ADC1 DMA scan group.
// KNOCK1 (PA2 / ADC3_IN2) is bound to ADC3 in knock_config.h.
// DO NOT add PA2 to the ADC1 scan group — it will corrupt the knock stream.

static void setAnalogInputs() {
    // TPS1 — PA4, ADC12_IN4
    engineConfiguration->tps1_1AdcChannel = EFI_ADC_4;

    // TPS2 / spare analog IN2 — PB0, ADC12_IN8
    // Used as secondary TPS for ETB redundancy or spare channel
    engineConfiguration->tps1_2AdcChannel = EFI_ADC_8;

    // PPS1 / spare analog IN1 — PA3, ADC123_IN3
    engineConfiguration->throttlePedalPositionAdcChannel = EFI_ADC_3;

    // PPS2 / spare analog IN3 — PC5, ADC12_IN15
    // Secondary pedal sensor for redundancy
    engineConfiguration->throttlePedalPositionSecondAdcChannel = EFI_ADC_15;

    // IAT — PC3, ADC123_IN13
    engineConfiguration->iat.adcChannel = EFI_ADC_13;

    // CLT — PC2, ADC123_IN12
    engineConfiguration->clt.adcChannel = EFI_ADC_12;

    // External MAP — PC0, ADC123_IN10
    engineConfiguration->map.sensor.hwChannel = EFI_ADC_10;

    // AUX KNOCK (analog signal monitoring) — PA0, ADC123_IN0
    // This is the auxiliary knock input routed through ADC1.
    // It can also be used as a general-purpose analog spare.
    // NOTE: Primary knock (PA2) is on ADC3 — configured in knock_config.h
    engineConfiguration->auxAnalog[0].hwChannel = EFI_ADC_0;  // AUX KNOCK / PA0

    // MAF sensor — PC4, ADC12_IN14
    engineConfiguration->mafAdcChannel = EFI_ADC_14;

    // KNOCK1 (PA2 / ADC3_IN2) — do NOT set here.
    // It is bound to ADCD3 by knock_config.h / the SW knock subsystem.
}

// ============================================================================
// VBAT monitor
// ============================================================================
// IMPORTANT: Your board must have a resistor divider connected to one of the
// remaining ADC-capable pins.  PC1 (ADC123_IN11) or an otherwise unused pin
// is the recommended location.  Adjust EFI_ADC_n and divider coefficient
// to match your actual hardware resistor network.
//
// Typical divider: 10 kΩ / 3.3 kΩ → coefficient ≈ 4.03 (for a 0–15 V range)
// Measure actual battery voltage and trim vbattDividerCoeff in TunerStudio.

static void setVbatt() {
    // TODO: confirm physical VBAT divider pin with schematic.
    // Placeholder: PC1 / ADC123_IN11 — change if your board differs.
    engineConfiguration->vbattAdcChannel  = EFI_ADC_11;   // PC1
    engineConfiguration->vbattDividerCoeff = 7.0f;         // tune to actual network
}

// ============================================================================
// Injector outputs — low-side drivers
// ============================================================================
static void setInjectorPins() {
    engineConfiguration->injectionPins[0] = Gpio::D3;   // INJ1
    engineConfiguration->injectionPins[1] = Gpio::A9;   // INJ2
    engineConfiguration->injectionPins[2] = Gpio::D11;  // INJ3
    engineConfiguration->injectionPins[3] = Gpio::D10;  // INJ4
    engineConfiguration->injectionPins[4] = Gpio::D2;   // INJ5
    engineConfiguration->injectionPins[5] = Gpio::A8;   // INJ6

    // Low-side drivers are active-high on this board (pull FET gate high to open)
    engineConfiguration->injectionPinMode = OM_DEFAULT;
}

// ============================================================================
// Ignition outputs — logic-level, drive external igniters
// ============================================================================
static void setIgnitionPins() {
    // IGN1  PC13
    engineConfiguration->ignitionPins[0] = Gpio::C13;

    // IGN2 — not populated on this board; leave Unassigned to prevent
    // rusEFI from toggling a floating GPIO which could cause noise.
    engineConfiguration->ignitionPins[1] = Gpio::Unassigned;

    // IGN3  PE4
    engineConfiguration->ignitionPins[2] = Gpio::E4;

    // IGN4  PE3
    engineConfiguration->ignitionPins[3] = Gpio::E3;

    // IGN5  PE2
    engineConfiguration->ignitionPins[4] = Gpio::E2;

    // IGN6  PB8
    engineConfiguration->ignitionPins[5] = Gpio::B8;

    // Active-high into igniters (most external igniters expect active-high)
    engineConfiguration->ignitionPinMode = OM_DEFAULT;
}

// ============================================================================
// Auxiliary low-side and relay outputs
// ============================================================================
static void setAuxOutputs() {
    // Idle air control valve — PD12
    // TIM4_CH1 (AF2) is available on PD12 for hardware PWM idle control.
    // rusEFI will use PWM scheduling via the idle subsystem.
    engineConfiguration->idle.solenoidPin       = Gpio::D12;
    engineConfiguration->idle.solenoidFrequency = 200;  // Hz — typical for PWM idle

    // Fuel pump relay — PC6
    // rusEFI controls FP relay: on at key-on, off after stall timeout
    engineConfiguration->fuelPumpPin = Gpio::C6;

    // Main relay — PB9
    // Controlled by rusEFI main relay logic (key-on / run / post-run)
    engineConfiguration->mainRelayPin = Gpio::B9;

    // LOWSIDE1 — PD15
    // General purpose low-side output.  Assign function in TunerStudio
    // (e.g. radiator fan, A/C compressor clutch, etc.)
    engineConfiguration->auxPidPins[0] = Gpio::D15;

    // LOWSIDE2 — PD13
    // Second general-purpose low-side output
    engineConfiguration->auxPidPins[1] = Gpio::D13;
}

// ============================================================================
// Digital / frequency inputs
// ============================================================================
static void setDigitalInputs() {
    // VSS — PE14  (vehicle speed, frequency input)
    // rusEFI uses a pulse counter / frequency decoder on this pin.
    engineConfiguration->vehicleSpeedSensorInputPin = Gpio::E14;

    // Flex fuel sensor — PE11  (ethanol %, frequency input 50–150 Hz)
    engineConfiguration->flexSensorPin = Gpio::E11;

    // Clutch switch — PB1  (digital input, active-low or active-high per wiring)
    engineConfiguration->clutchDownPin     = Gpio::B1;
    engineConfiguration->clutchDownPinMode = PI_PULLUP;

    // Button 2 — PA6  (user-assignable digital input)
    engineConfiguration->luaDigitalInputPins[0] = Gpio::A6;

    // Button 3 — PE15  (user-assignable digital input)
    engineConfiguration->luaDigitalInputPins[1] = Gpio::E15;
}

// ============================================================================
// CAN configuration
// ============================================================================
static void setCanPins() {
    // CAN1: PD0 (RX) / PD1 (TX) — alternate function AF9 on STM32F7
    // Shared by OpenBLT firmware update and rusEFI runtime CAN telemetry
    engineConfiguration->canRxPin        = Gpio::D0;
    engineConfiguration->canTxPin        = Gpio::D1;
    engineConfiguration->canBaudRate     = B500KBPS;
    engineConfiguration->canWriteEnabled = true;
    engineConfiguration->canReadEnabled  = true;
    engineConfiguration->canSleepPeriodMs = 50;  // telemetry broadcast period
}

// ============================================================================
// Main board default configuration
// Called once during rusEFI startup via setup_custom_board_overrides()
// ============================================================================
static void customBoardDefaultConfiguration() {
    // --- Triggers (Hall only) ---
    setTriggerInputs();

    // --- Analog ---
    setAnalogInputs();
    setVbatt();

    // --- Outputs ---
    setInjectorPins();
    setIgnitionPins();
    setAuxOutputs();

    // --- Digital inputs ---
    setDigitalInputs();

    // --- CAN ---
    setCanPins();

    // --- Injection mode ---
    // Start in batch; switch to sequential in TunerStudio once cam sync confirmed
    engineConfiguration->injectionMode        = IM_BATCH;
    engineConfiguration->twoWireBatchInjection = true;

    // --- Ignition mode ---
    // Wasted spark default; change to COP in TunerStudio for sequential ign
    engineConfiguration->ignitionMode = IM_WASTED_SPARK;

    // --- Analog input divider ---
    // Board analog inputs have a 2:1 divider network (10k/10k).
    // Adjust to match your actual board schematic.
    engineConfiguration->analogInputDividerCoefficient = 2.0f;

    // --- Global trigger offset ---
    // Set to 0 initially; calibrate with timing light before enabling dynamic timing
    engineConfiguration->globalTriggerAngleOffset = 0;
}

// ============================================================================
// Hook into rusEFI board-override system
// This function is called from boardInit() in the ChibiOS HAL layer.
// ============================================================================
void setup_custom_board_overrides() {
    custom_board_DefaultConfiguration = customBoardDefaultConfiguration;
}
