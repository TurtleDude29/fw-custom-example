/**
 * @file default_tune.cpp
 * @brief Factory / safe-start tune defaults — custom STM32F767VIT6 ECU
 *
 * Called AFTER board pin configuration and AFTER flash config is loaded.
 * These are engine-level defaults the tuner refines in TunerStudio.
 * Do NOT place pin assignments here.
 */

#include "pch.h"

void boardTuneDefaults() {

    // -----------------------------------------------------------------------
    // Engine geometry — generic 4-cylinder placeholder
    // UPDATE these values to match the actual engine before first start.
    // -----------------------------------------------------------------------
    engineConfiguration->cylindersCount = 4;
    engineConfiguration->displacement   = 2.0f;        // litres — change me
    engineConfiguration->firingOrder    = FO_1_3_4_2;  // common inline-4

    // -----------------------------------------------------------------------
    // Trigger wheel
    // Hall sensors: 60-2 crank wheel + single cam tooth — most common setup.
    // Change TT_ type in TunerStudio to match the actual reluctor wheel.
    // -----------------------------------------------------------------------
    engineConfiguration->trigger.type = trigger_type_e::TT_TOOTHED_WHEEL_60_2;
    engineConfiguration->trigger.customTotalToothCount   = 60;
    engineConfiguration->trigger.customSkippedToothCount = 2;

    // Hall sensors output clean digital edges — no inversion required
    engineConfiguration->invertPrimaryTriggerSignal   = false;
    engineConfiguration->invertSecondaryTriggerSignal = false;

    // -----------------------------------------------------------------------
    // Injection — conservative cranking values
    // -----------------------------------------------------------------------
    engineConfiguration->injector.flow              = 240.0f; // cc/min
    engineConfiguration->cranking.baseFuel          = 25.0f;  // ms
    engineConfiguration->startOfCrankingPrimingPulse = 0.0f;
    engineConfiguration->stoichRatioPrimary          = 14.7f; // petrol λ=1

    // -----------------------------------------------------------------------
    // Ignition — fixed timing for first start; switch to map in TunerStudio
    // -----------------------------------------------------------------------
    engineConfiguration->timing_mode    = TM_FIXED;
    engineConfiguration->fixedTiming    = 15.0f; // degrees BTDC
    engineConfiguration->ignitionDwellForCrankingMs = 5.0f;

    // -----------------------------------------------------------------------
    // Temperature sensors — generic NTC pull-up bias resistors
    // Match to your sensor part number calibration table in TunerStudio.
    // -----------------------------------------------------------------------
    engineConfiguration->clt.config.bias_resistor = 2700.0f; // 2.7 kΩ
    engineConfiguration->iat.config.bias_resistor = 2700.0f;

    // -----------------------------------------------------------------------
    // MAP sensor — set to custom linear if using a MAP with known datasheet
    // Change to MT_CUSTOM and set transfer function in TunerStudio
    // -----------------------------------------------------------------------
    engineConfiguration->map.sensor.type = MT_MPXH6400; // 4-bar default

    // -----------------------------------------------------------------------
    // MAF — disable at factory default by setting channel to NONE.
    // Re-enable in TunerStudio (Sensors → MAF) once transfer function is known.
    // The physical pin (PC4 / EFI_ADC_14) is set in board_configuration.cpp;
    // overriding to NONE here disables MAF processing without removing the pin.
    // -----------------------------------------------------------------------
    engineConfiguration->mafAdcChannel = EFI_ADC_NONE;

    // -----------------------------------------------------------------------
    // Flex fuel — sensor pin is wired to PE11 (set in board_configuration.cpp).
    // No separate enable bool exists; flex fuel is active when flexSensorPin
    // is not Gpio::Unassigned.  Leave pin assigned; disable via TS if needed.

    // -----------------------------------------------------------------------
    // Idle control — open-loop starting point
    // -----------------------------------------------------------------------
    engineConfiguration->manIdlePosition = 30.0f; // % duty

    // -----------------------------------------------------------------------
    // Rev limits — conservative safe values; tune upward as needed
    // -----------------------------------------------------------------------
    engineConfiguration->rpmHardLimit       = 7000;
    engineConfiguration->cutFuelOnHardLimit  = true;
    engineConfiguration->cutSparkOnHardLimit = true;

    // -----------------------------------------------------------------------
    // VSS — pulses per km.  Set to match your diff ratio / ABS ring.
    // -----------------------------------------------------------------------
    engineConfiguration->vehicleSpeedCoef = 1.0f; // calibrate in TS

    // -----------------------------------------------------------------------
    // CAN telemetry — 50 ms period set in board_configuration.cpp
    // -----------------------------------------------------------------------
    engineConfiguration->canNbcType = CAN_BUS_NBC_NONE; // raw frames

    // -----------------------------------------------------------------------
    // Knock retard — disabled by default.
    // Enable and configure threshold/retard amounts in TunerStudio → Knock.
    // The knock ADC channel (PA2 / ADC3) is wired and active; only the
    // retard response needs to be enabled in software once signal is verified.
    // enableKnockSpectrogram is the TS ini field; no C++ bool to set here.
}
