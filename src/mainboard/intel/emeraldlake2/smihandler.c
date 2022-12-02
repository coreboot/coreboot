/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/io.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>

/*
 * Change LED_POWER# (SIO GPIO 45) state based on sleep type.
 * The IO address is hardcoded as we don't have device path in SMM.
 */
#define SIO_GPIO_BASE_SET4	(0x730 + 3)
#define SIO_GPIO_BLINK_GPIO45	0x25
void mainboard_smi_sleep(u8 slp_typ)
{
	u8 reg8;

	switch (slp_typ) {
	case ACPI_S3:
	case ACPI_S4:
		break;

	case ACPI_S5:
		/* Turn off LED */
		reg8 = inb(SIO_GPIO_BASE_SET4);
		reg8 |= (1 << 5);
		outb(reg8, SIO_GPIO_BASE_SET4);
		break;
	}
}
