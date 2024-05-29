/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8772f/it8772f.h>

#define GPIO_DEV PNP_DEV(0x2e, IT8772F_GPIO)

void mainboard_smi_sleep(u8 slp_typ)
{
	switch (slp_typ) {
	case ACPI_S3:
		ite_set_3vsbsw(GPIO_DEV, true);
		ite_delay_pwrgd3(GPIO_DEV);
		it8772f_gpio_led(
			GPIO_DEV,
			2 /* set */,
			0x02 /* select */,
			0x01 /* polarity */,
			0x01 /* 1 = pullup */,
			0x01 /* output */,
			0x00 /* 1 = Simple IO function */,
			SIO_GPIO_BLINK_GPIO22,
			IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
		break;
	default:
		break;
	}
}
