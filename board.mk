# =============================================================================
# board.mk — fw-custom-stm32f767
# MCU: STM32F767VIT6  (LQFP100, 2 MB Flash, 512 KB RAM, Cortex-M7 @ 216 MHz)
# Architecture: mega-uaefi derived, Hall-only triggers, no VR conditioner
# =============================================================================

# Primary board source file
BOARDCPPSRC = $(BOARD_DIR)/board_configuration.cpp

# Board identity (SHORT_BOARD_NAME, BOARD_CPU, USE_OPENBLT)
include $(BOARD_DIR)/meta-info.env

# ---------------------------------------------------------------------------
# STM32F7 system
# ---------------------------------------------------------------------------
# Custom pinout — suppress rusEFI pin-state validator
DDEFS += -DDISABLE_PIN_STATE_VALIDATION=TRUE

# Reduce background RAM monitoring
DDEFS += -DRAM_UNUSED_SIZE=100

# No dedicated critical-error LED pin on this board
DDEFS += -DLED_CRITICAL_ERROR_BRAIN_PIN=Gpio::H15

# ---------------------------------------------------------------------------
# ADC — ADC3 required for software knock on PA2
# ADC1 handles all remaining analog channels via DMA scan group
# ---------------------------------------------------------------------------
DDEFS += -DSTM32_ADC_USE_ADC3=TRUE
DDEFS += -DEFI_SOFTWARE_KNOCK=TRUE

# ---------------------------------------------------------------------------
# CAN1 bootloader (OpenBLT) — PD0 RX / PD1 TX
# AF9 on STM32F7 for CAN1 alternate mapping
# ---------------------------------------------------------------------------
DDEFS += -DBOOT_COM_CAN_CHANNEL_INDEX=0
DDEFS += -DOPENBLT_CAN_RX_PORT=GPIOD
DDEFS += -DOPENBLT_CAN_RX_PIN=0
DDEFS += -DOPENBLT_CAN_TX_PORT=GPIOD
DDEFS += -DOPENBLT_CAN_TX_PIN=1

# ---------------------------------------------------------------------------
# Feature enables / disables
# ---------------------------------------------------------------------------
# Main relay is software-controlled (PB9)
DDEFS += -DEFI_MAIN_RELAY_CONTROL=TRUE

# Fuel pump relay is software-controlled (PC6)
DDEFS += -DEFI_FUEL_PUMP=TRUE

# Flex fuel frequency input on PE11
DDEFS += -DEFI_FLEX_FUEL=TRUE

# VSS vehicle speed sensor on PE14
DDEFS += -DEFI_VEHICLE_SPEED=TRUE

# MAF sensor present (PC4)
DDEFS += -DEFI_MAF_ENABLED=TRUE

# No HPFP PWM on this board
DDEFS += -DEFI_HPFP=FALSE

# Disable over-the-air wideband firmware update
DDEFS += -DEFI_WIDEBAND_FIRMWARE_UPDATE=FALSE

# Saves ~80 KB flash — INI served via TunerStudio project file
DDEFS += -DEFI_EMBED_INI_MSD=FALSE

# Disable debug/emulation features to recover RAM
DDEFS += -DEFI_LOGIC_ANALYZER=FALSE
DDEFS += -DEFI_ENGINE_SNIFFER=FALSE
DDEFS += -DEFI_ENGINE_EMULATOR=FALSE
DDEFS += -DEFI_TOOTH_LOGGER=FALSE

# Noisy PPS line — widen ETB intermittent fault window
DDEFS += -DETB_INTERMITTENT_LIMIT=60001
