/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <superio/ite/common/ite.h>
#include "common_defines.h"
#include "led.h"

/* Change power led state based on sleep type. */
void mainboard_smi_sleep(u8 slp_typ)
{
	printk(BIOS_DEBUG, "SMI: sleep S%d\n", slp_typ);
	switch (slp_typ) {
	case ACPI_S3:
		/*
		 * Vendor firmware also only enables this when entering sleep.
		 * This saves some power when entering S5 via shutdown.
		 */
		ite_enable_3vsbsw(GPIO_DEV);
		set_power_led(LED_YELLOW);
		break;
	case ACPI_S4:
		/* Without 3vsbsw enabled the LED will be off in any case */
	case ACPI_S5:
		set_power_led(LED_OFF);
		break;
	default:
		break;
	}
}
