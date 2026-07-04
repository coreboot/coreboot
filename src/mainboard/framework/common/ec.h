/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef MAINBOARD_EC_H
#define MAINBOARD_EC_H

#include <mainboard/framework/common/board_host_command.h>
#include <soc/gpe.h>

/*
 * EC events that the Framework CrosEC fork raises as SCIs
 * Does NOT include Chromebook events like MKBP, PD_MCU, ...
 */
#define MAINBOARD_EC_SCI_EVENTS                               \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_CLOSED) |       \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_OPEN) |         \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON) |     \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_CONNECTED) |     \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_DISCONNECTED) |  \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY_LOW) |      \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY_CRITICAL) | \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY))
#define MAINBOARD_EC_SMI_EVENTS (EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_CLOSED))
/* EC can wake from S5 with power button only */
#define MAINBOARD_EC_S5_WAKE_EVENTS EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON)
/* EC can additionally wake from S3/S0ix on AC change and critical battery */
#define MAINBOARD_EC_S3_WAKE_EVENTS                                                     \
	(MAINBOARD_EC_S5_WAKE_EVENTS | EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_CONNECTED) | \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_DISCONNECTED) |                            \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY_CRITICAL))
#define MAINBOARD_EC_S0IX_WAKE_EVENTS MAINBOARD_EC_S3_WAKE_EVENTS
/* Log EC shutdown events */
#define MAINBOARD_EC_LOG_EVENTS                               \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_THERMAL_SHUTDOWN) | \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY_SHUTDOWN) | \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_PANIC))

/*
 * ACPI feature toggles to enable EC SCI, lid and wake.
 *
 * On sunflower SCI is signaled through GPIO instead of eSPI virtual wire.
 * That has been shown to be more reliable to wake the system from suspend.
 */
#if CONFIG(BOARD_FRAMEWORK_SUNFLOWER)
#define EC_SCI_GPI GPE0_DW1_19
#define EC_ENABLE_WAKE_PIN GPE0_DW1_19
#else
#define EC_SCI_GPI GPE0_ESPI
#define EC_ENABLE_WAKE_PIN GPE0_ESPI
#endif
#define EC_ENABLE_LID_SWITCH
#define EC_ENABLE_POWER_BUTTON

/*
 * Enable standard PS2 keyboard (not e.g. Chrome Vivaldi)
 * Also when the OS has no I2C HID driver (e.g. Windows installer), the EC
 * presents an emulated PS2 mouse interface to allow using the touchpad.
 */
#define SIO_EC_ENABLE_PS2K /* Enable PS/2 Keyboard */
#define SIO_EC_ENABLE_PS2M /* Enable PS/2 Mouse */

/*
 * Framework ACPI shared memory with the EC
 *
 * Contains a couple of flags and up-to-date system information.
 */
#define EC_FRAMEWORK_ACPI_SHARED_MEM_IO 0xF00

/*
 * CFR option controlling the fingerprint reader LED brightness.
 *
 * The stored value is an enum fp_led_brightness_level (see board_host_command.h),
 * except for FP_LED_LEVEL_EC_DEFAULT which means "don't touch the EC and keep
 * whatever level it has stored". Any other value is sent to the EC on every
 * boot via EC_CMD_FP_LED_LEVEL_CONTROL.
 */
#define FP_LED_LEVEL_OPTION_NAME	"fp_led_level"
#define FP_LED_LEVEL_EC_DEFAULT		0x100	/* out of uint8_t range on purpose */

/*
 * CFR options for the PS/2 touchpad emulation and EC standalone mode.
 *
 * Both are simple on/off toggles that are sent to the EC on every boot via
 * EC_CMD_DISABLE_PS2_EMULATION and EC_CMD_STANDALONE_MODE respectively.
 */
#define PS2_EMULATION_OPTION_NAME	"ps2_emulation"
#define STANDALONE_MODE_OPTION_NAME	"standalone_mode"

/*
 * CFR option controlling the maximum battery charge level.
 *
 * The stored value is the maximum charge percentage (50-100) to enforce via
 * EC_CMD_CHARGE_LIMIT_CONTROL on every boot. A value of 100 disables the limit
 * (charge to full). BATTERY_CHARGE_LIMIT_EC_DEFAULT (0) means "don't touch the
 * EC and keep whatever limit it has stored", e.g. one set at runtime via
 * framework_tool, so a reboot doesn't override the user's OS-side choice. 0 is
 * used as the sentinel because it is not a meaningful charge limit.
 */
#define BATTERY_CHARGE_LIMIT_OPTION_NAME	"battery_charge_limit"
#define BATTERY_CHARGE_LIMIT_EC_DEFAULT		0

/*
 * CFR option selecting the touchscreen stylus protocol on boards with a stylus
 * protocol switch (Framework Laptop 12).
 *
 * The stored value is the level written to the EC's "stylus_sw_r" GPIO via
 * EC_CMD_GPIO_SET on every boot: HIGH selects MPP, LOW selects USI.
 */
#define STYLUS_PROTOCOL_OPTION_NAME	"stylus_protocol"
#define STYLUS_PROTOCOL_GPIO_NAME	"stylus_sw_r"
#define STYLUS_PROTOCOL_USI		0	/* GPIO low */
#define STYLUS_PROTOCOL_MPP		1	/* GPIO high */

/*
 * CFR option controlling the input deck (input module) power mode, sent to the
 * EC via EC_CMD_CHECK_DECK_STATE on every boot. The stored value is the mode
 * byte: AUTO requires all input modules to be correctly installed before
 * enabling their power, while FORCE_ON/FORCE_OFF override that check.
 */
#define INPUT_DECK_MODE_OPTION_NAME	"input_deck_mode"
#define INPUT_DECK_MODE_AUTO		0x01
#define INPUT_DECK_MODE_FORCE_ON	0x02
#define INPUT_DECK_MODE_FORCE_OFF	0x04

#endif /* MAINBOARD_EC_H */
