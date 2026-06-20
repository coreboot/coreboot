/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef MAINBOARD_EC_H
#define MAINBOARD_EC_H

#include <ec/google/chromeec/ec_commands.h>
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

/* ACPI feature toggles to enable EC eSPI, lid and wake */
#define EC_SCI_GPI GPE0_ESPI
#define EC_ENABLE_LID_SWITCH
#define EC_ENABLE_POWER_BUTTON
#define EC_ENABLE_WAKE_PIN GPE0_ESPI

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

#endif /* MAINBOARD_EC_H */
