/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6687d/nct6687d.h>

#define GLOBAL_DEV PNP_DEV(0x2e, 0)
/* Change to NCT6687D_SP1 to use COM2 header */
#define SERIAL_DEV PNP_DEV(0x2e, NCT6687D_SP2)
#define POWER_DEV  PNP_DEV(0x2e, NCT6687D_SLEEP_PWR)

void bootblock_mainboard_early_init(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/*
	 * Replicate non-default vendor settings (mostly multi-function pin
	 * selection settings) in the global LDN. It seems like some bits are
	 * set to non-default values before coreboot configures them; possibly
	 * by the MCU firmware. Comments provided for notable settings.
	 */
	pnp_write_config(GLOBAL_DEV, 0x13, 0x0c);
	/* Pins 121, 122 as TACHPWM */
	pnp_write_config(GLOBAL_DEV, 0x15, 0xf0);
	/* Pin 125 as TACHPWM */
	pnp_write_config(GLOBAL_DEV, 0x1a, 0x07);
	pnp_write_config(GLOBAL_DEV, 0x1b, 0xf0);
	pnp_write_config(GLOBAL_DEV, 0x1d, 0x08);
	/* Pins 95, 98, 124 as TACHPWM */
	pnp_write_config(GLOBAL_DEV, 0x1e, 0xfc);
	/* Pins 126, 127 as TACHPWM */
	pnp_write_config(GLOBAL_DEV, 0x1f, 0xf0);
	pnp_write_config(GLOBAL_DEV, 0x22, 0xbc);
	pnp_write_config(GLOBAL_DEV, 0x23, 0xdf);
	/* Route pins 29-36 to COM A (COM2 header) */
	pnp_write_config(GLOBAL_DEV, 0x24, 0x61);
	pnp_write_config(GLOBAL_DEV, 0x25, 0xff);
	/* Route pins to parallel port */
	pnp_write_config(GLOBAL_DEV, 0x27, 0xbe);
	pnp_write_config(GLOBAL_DEV, 0x29, 0xfd);
	/* Route pins 7-13 to COM B (Back panel COM1) */
	pnp_write_config(GLOBAL_DEV, 0x2a, 0xcf);

	/* Configure pin for PECI */
	pnp_set_logical_device(POWER_DEV);
	pnp_write_config(POWER_DEV, 0xf3, 0x0c);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	/* Back panel COM1 */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
