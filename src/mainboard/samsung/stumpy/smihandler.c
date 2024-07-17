/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>

/* Include for SIO helper functions */
#include <superio/ite/common/ite.h>
#include <superio/ite/common/ite_gpio.h>
#include <superio/ite/it8772f/it8772f.h>
#define GPIO_DEV PNP_DEV(0x2e, IT8772F_GPIO)

/*
 * Change LED_POWER# (SIO GPIO 45) state based on sleep type.
*/
void mainboard_smi_sleep(u8 slp_typ)
{
	printk(BIOS_DEBUG, "SMI: sleep S%d\n", slp_typ);
	switch (slp_typ) {
	case ACPI_S3:
	case ACPI_S4:
		ite_gpio_setup(GPIO_DEV, 45, ITE_GPIO_OUTPUT, ITE_GPIO_ALT_FN_MODE,
			       ITE_GPIO_POL_INVERT | ITE_GPIO_PULLUP_ENABLE);
		ite_gpio_setup_led(GPIO_DEV, 45, ITE_GPIO_LED_1, ITE_LED_FREQ_1HZ,
				   ITE_LED_CONTROL_DEFAULT);
		break;
	case ACPI_S5:
		ite_gpio_setup(GPIO_DEV, 45, ITE_GPIO_OUTPUT, ITE_GPIO_SIMPLE_IO_MODE,
			       ITE_GPIO_CONTROL_DEFAULT);
		break;
	default:
		break;
	}
}
