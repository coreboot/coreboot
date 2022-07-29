/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <option.h>
#include <superio/conf_mode.h>

#include "nct6687d.h"

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

/* 0xeb register definitions */
#define NCT6687D_ACPI_POWER_LOSS_CONTROL_MASK	0x60
#define NCT6687D_ACPI_POWER_LOSS_CONTROL_SHIFT	5
#define NCT6687D_ACPI_POWER_ALWAYS_OFF		(0 << NCT6687D_ACPI_POWER_LOSS_CONTROL_SHIFT)
#define NCT6687D_ACPI_POWER_ALWAYS_ON		(1 << NCT6687D_ACPI_POWER_LOSS_CONTROL_SHIFT)
#define NCT6687D_ACPI_POWER_PREV_STATE		(2 << NCT6687D_ACPI_POWER_LOSS_CONTROL_SHIFT)
#define NCT6687D_ACPI_POWER_USER_DEFINED	(3 << NCT6687D_ACPI_POWER_LOSS_CONTROL_SHIFT)

static void nct6687d_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	uint8_t byte;
	uint8_t power_status;
	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for EC, GPIOs, etc. */
	case NCT6687D_KBC:
		pc_keyboard_init(PROBE_AUX_DEVICE);
		break;
	case NCT6687D_ACPI:
		/* Set power state after power fail */
		power_status = get_uint_option("power_on_after_fail",
				CONFIG_MAINBOARD_POWER_FAILURE_STATE);
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		byte = pnp_read_config(dev, 0xeb);
		byte &= ~NCT6687D_ACPI_POWER_LOSS_CONTROL_MASK;
		if (power_status == MAINBOARD_POWER_ON)
			byte |= NCT6687D_ACPI_POWER_ALWAYS_ON;
		else if (power_status == MAINBOARD_POWER_KEEP)
			byte |= NCT6687D_ACPI_POWER_PREV_STATE;
		pnp_write_config(dev, 0xeb, byte);
		pnp_exit_conf_mode(dev);
		printk(BIOS_INFO, "set power %s after power fail\n",
		       power_status ? "on" : "off");
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nct6687d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT6687D_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_MSC0, 0x0FF8,},
	{ NULL, NCT6687D_SP1, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0FF8, },
	{ NULL, NCT6687D_SP2, PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1, 0x0FF8, },
	{ NULL, NCT6687D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1 | PNP_MSC0,
		0x0FFF, 0x0FFF, },
	{ NULL, NCT6687D_CIR, PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 | PNP_MSC2 | PNP_MSC3,
		0x0FF8, },
	{ NULL, NCT6687D_GPIO_0_7, PNP_IO0 | PNP_IRQ0, 0x0FF0,},
	{ NULL, NCT6687D_P80_UART},
	{ NULL, NCT6687D_GPIO_8_9_AF},
	{ NULL, NCT6687D_ACPI, PNP_IO0 | PNP_IRQ0, 0x0FF8,},
	{ NULL, NCT6687D_EC, PNP_IO0 | PNP_IRQ0, 0x0FF8,},
	{ NULL, NCT6687D_RTC},
	{ NULL, NCT6687D_SLEEP_PWR},
	{ NULL, NCT6687D_TACH_PWM},
	{ NULL, NCT6687D_FREG},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct6687d_ops = {
	CHIP_NAME("NUVOTON NCT6687D Super I/O")
	.enable_dev = enable_dev,
};
