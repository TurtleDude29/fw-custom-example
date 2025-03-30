#include "pch.h"
#include "defaults.h"


Gpio getCommsLedPin() {
	return Gpio::E3;
}

Gpio getRunningLedPin() {
	return Gpio::Unassigned;
}

Gpio getWarningLedPin() {
	return Gpio::Unassigned;
}

void setBoardDefaultConfiguration() {
	//MAIN
   setTPS1Inputs(EFI_ADC_12, EFI_ADC_13);
        engineConfiguration->mainRelayPin = Gpio::B8;
        engineConfiguration->fuelPumpPin = Gpio::E0;
	engineConfiguration->malfunctionIndicatorPin = Gpio::B9;
        engineConfiguration->tachOutputPin = Gpio::Unassigned;
	//IGNITION STOCK
	engineConfiguration->ignitionMode = IM_INDIVIDUAL_COILS;
	engineConfiguration->ignitionPins[0] = Gpio::D4;
	//INJECTION STOCK
	engineConfiguration->injectionPins[0] = Gpio::G12;
	engineConfiguration->injectionPins[1] = Gpio::G13;
	engineConfiguration->injectionPins[2] = Gpio::G11;
	engineConfiguration->injectionPins[3] = Gpio::E1;
	engineConfiguration->injectionPins[4] = Gpio::E2;
	engineConfiguration->injectionPins[5] = Gpio::G14;
	//AUX IGN
	engineConfiguration->ignitionPins[1] = Gpio::G3;
	engineConfiguration->ignitionPins[2] = Gpio::G4;
	engineConfiguration->ignitionPins[3] = Gpio::G5;
	engineConfiguration->ignitionPins[4] = Gpio::G6;
	engineConfiguration->ignitionPins[5] = Gpio::G7;
	engineConfiguration->ignitionPins[6] = Gpio::G8;
	//AUX INJ
	engineConfiguration->injectionPins[6] = Gpio::G9;
	engineConfiguration->injectionPins[7] = Gpio::G10;
	engineConfiguration->injectionPins[8] = Gpio::D3;
	engineConfiguration->injectionPins[9] = Gpio::D7;
	engineConfiguration->injectionPins[10] = Gpio::A8;
	engineConfiguration->injectionPins[11] = Gpio::A9;
	//AUX FEATURE
    	// PWM pin
     	engineConfiguration->etbIo[0].controlPin = Gpio::D12;
    	// DIR pin
    	engineConfiguration->etbIo[0].directionPin1 = Gpio::D10;
    	// Disable pin
     	engineConfiguration->etbIo[0].disablePin = Gpio::D11;
	engineConfiguration->fanPinMode = OM_DEFAULT;
	engineConfiguration->fanPin = Gpio::D14;
	engineConfiguration->launchActivationMode = CLUTCH_INPUT_LAUNCH;
	engineConfiguration->camInputs[0] = Gpio::E12; // HALL1
    	engineConfiguration->camInputs[1] = Gpio::E13; // HALL2
	//SD
	engineConfiguration->spi3mosiPin = Gpio::C12;
	engineConfiguration->spi3misoPin = Gpio::C11;
	engineConfiguration->spi3sckPin = Gpio::C10;
	//CAN
	engineConfiguration->canTxPin = Gpio::D1;
     	engineConfiguration->canRxPin = Gpio::D0;
	//VBATT
	engineConfiguration->vbattDividerCoeff = 10.2f;
	engineConfiguration->vbattAdcChannel = EFI_ADC_7;
}

void setupDefaultSensorInputs() {
     	engineConfiguration->clt.adcChannel = EFI_ADC_2;
	engineConfiguration->triggerInputPins[0] = Gpio::E8;   /* cam */
     	engineConfiguration->triggerInputPins[1] = Gpio::E7;   /* crank pos #1 */
}

static Gpio OUTPUTS[] = {
    Gpio::D11, // 
    Gpio::D12, // 
    Gpio::D13, //
    Gpio::D14, //
    Gpio::E11, //
    Gpio::E12,
};

int getBoardMetaOutputsCount() {
    return efi::size(OUTPUTS);
}

Gpio* getBoardMetaOutputs() {
    return OUTPUTS;
}

void setCustomVbatt() {
	// set vbatt_divider 5.835
	// 33k / 6.8k
	engineConfiguration->vbattDividerCoeff = 10.2; // 5.835

	engineConfiguration->vbattAdcChannel = EFI_ADC_7;

}
