/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <option.h>
#include <acpi/acpi.h>
#include <superio/conf_mode.h>

#include "nct5572d.h"

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

static void nct5572d_init(struct device *dev)
{
	uint8_t byte;
	uint8_t power_status;
	uint8_t mouse_detected;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for HWM or FDC etc. */
	case NCT5572D_KBC:
		/* Enable mouse controller */
		pnp_enter_conf_mode(dev);
		byte = pnp_read_config(dev, 0x2a);
		byte &= ~(0x1 << 1);
		pnp_write_config(dev, 0x2a, byte);
		pnp_exit_conf_mode(dev);

		mouse_detected = pc_keyboard_init(PROBE_AUX_DEVICE);

		if (!mouse_detected) {
			printk(BIOS_INFO, "%s: Disable mouse controller.\n",
					__func__);
			pnp_enter_conf_mode(dev);
			byte = pnp_read_config(dev, 0x2a);
			byte |= 0x1 << 1;
			pnp_write_config(dev, 0x2a, byte);
			pnp_exit_conf_mode(dev);
		}
		break;
	case NCT5572D_ACPI:
		/* Set power state after power fail */
		power_status = CONFIG_MAINBOARD_POWER_FAILURE_STATE;
		get_option(&power_status, "power_on_after_fail");
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		byte = pnp_read_config(dev, 0xe4);
		byte &= ~0x60;
		if (power_status == MAINBOARD_POWER_ON)
			byte |= (0x1 << 5);
		else if (power_status == MAINBOARD_POWER_KEEP)
			byte |= (0x2 << 5);
		pnp_write_config(dev, 0xe4, byte);
		pnp_exit_conf_mode(dev);
		printk(BIOS_INFO, "set power %s after power fail\n", power_status ? "on" : "off");
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nct5572d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT5572D_FDC}, /* no pins, removed from datasheet */
	{ NULL, NCT5572D_PP}, /* no pins, removed from datasheet */
	{ NULL, NCT5572D_SP1, PNP_IO0 | PNP_IRQ0, 0x0FF8, },
	{ NULL, NCT5572D_IR, PNP_IO0 | PNP_IRQ0, 0x0FF8, },
	{ NULL, NCT5572D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0FFF, 0x0FFF, },
	{ NULL, NCT5572D_CIR, PNP_IO0 | PNP_IRQ0, 0x0FF8, },
	{ NULL, NCT5572D_WDT1},
	{ NULL, NCT5572D_ACPI},
	{ NULL, NCT5572D_HWM_TSI_FPLED, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x0FFE, 0x0FFE, },
	{ NULL, NCT5572D_PECI},
	{ NULL, NCT5572D_SUSLED},
	{ NULL, NCT5572D_CIRWKUP, PNP_IO0 | PNP_IRQ0, 0x0FF8, },
	{ NULL, NCT5572D_GPIO_PP_OD},
	{ NULL, NCT5572D_GPIO2},
	{ NULL, NCT5572D_GPIO3},
	{ NULL, NCT5572D_GPIO4}, /* no pins, removed from datasheet */
	{ NULL, NCT5572D_GPIO5},
	{ NULL, NCT5572D_GPIO6},
	{ NULL, NCT5572D_GPIO7}, /* no pins, removed from datasheet */
	{ NULL, NCT5572D_GPIO8},
	{ NULL, NCT5572D_GPIO9},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct5572d_ops = {
	CHIP_NAME("NUVOTON NCT5572D Super I/O")
	.enable_dev = enable_dev,
};
