/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/common/ite_gpio.h>
#include <superio/ite/it8772f/it8772f.h>

#define GPIO_DEV PNP_DEV(0x2e, IT8772F_GPIO)

void mainboard_smi_sleep(u8 slp_typ)
{
	switch (slp_typ) {
	case ACPI_S3:
		ite_set_3vsbsw(GPIO_DEV, true);
		ite_delay_pwrgd3(GPIO_DEV);
		ite_reg_write(GPIO_DEV, ITE_GPIO_REG_SELECT(1), 0x02);
		ite_gpio_setup(GPIO_DEV, 22, ITE_GPIO_OUTPUT, ITE_GPIO_ALT_FN_MODE,
			       ITE_GPIO_POL_INVERT | ITE_GPIO_PULLUP_ENABLE);
		ite_gpio_setup_led(GPIO_DEV, 22, ITE_GPIO_LED_1, ITE_LED_FREQ_1HZ,
				   ITE_LED_CONTROL_DEFAULT);
		break;
	default:
		break;
	}
}
