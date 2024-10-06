/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <superio/hwm5_conf.h>
#include <superio/nuvoton/common/hwm.h>
#include "gpio.h"
#include "static.h"

void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static void mainboard_final(void *chip_info)
{
	const u16 hwm_base = 0x290;
	nuvoton_hwm_select_bank(hwm_base, 0);
	/* Configure CPU temperature sensor for this board */
	/* VBAT_MONITOR_CONTROL = 0 */
	pnp_write_hwm5_index(hwm_base, 0x5d, 0x00);
	/* CURRENT_MODE_ENABLE = 0 */
	pnp_write_hwm5_index(hwm_base, 0x5e, 0x00);

	/*
	 * Turn off the POST display at end of POST.  This is in a GCR (Global
	 * Control Register), but we have to use one of the LDNs as the device
	 * because the chip ops are only assigned to the LDNs.
	 */
	pnp_enter_conf_mode(DEV_PTR(nvt_superio_gpio1));
	printk(BIOS_DEBUG, "GCR 0x2f was: %02X\n",
		pnp_read_config(DEV_PTR(nvt_superio_gpio1), 0x2f));
	pnp_write_config(DEV_PTR(nvt_superio_gpio1), 0x2f, 0x00);
	printk(BIOS_DEBUG, "GCR 0x2f is now: %02X\n",
		pnp_read_config(DEV_PTR(nvt_superio_gpio1), 0x2f));
	pnp_exit_conf_mode(DEV_PTR(nvt_superio_gpio1));
}

struct chip_operations mainboard_ops = {
	.final = mainboard_final,
};
