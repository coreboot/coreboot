/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <mainboard/gpio.h>
#include <soc/gpio.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6687d/nct6687d.h>

#define SERIAL_DEV PNP_DEV(0x2e, NCT6687D_SP2)
#define POWER_DEV  PNP_DEV(0x2e, NCT6687D_SLEEP_PWR)

void bootblock_mainboard_early_init(void)
{
	/* Replicate vendor settings for multi-function pins in global config LDN */
	nuvoton_pnp_enter_conf_state(SERIAL_DEV);
	pnp_write_config(SERIAL_DEV, 0x13, 0xff);
	pnp_write_config(SERIAL_DEV, 0x14, 0xff);

	/* Below are multi-pin function */
	pnp_write_config(SERIAL_DEV, 0x1b, 0xf8);
	pnp_write_config(SERIAL_DEV, 0x1f, 0xf0);
	pnp_write_config(SERIAL_DEV, 0x20, 0xd4);
	pnp_write_config(SERIAL_DEV, 0x21, 0x41);
	pnp_write_config(SERIAL_DEV, 0x22, 0xbc);
	pnp_write_config(SERIAL_DEV, 0x23, 0xff);
	pnp_write_config(SERIAL_DEV, 0x24, 0x07);
	pnp_write_config(SERIAL_DEV, 0x25, 0xff);
	pnp_write_config(SERIAL_DEV, 0x26, 0x80);
	pnp_write_config(SERIAL_DEV, 0x28, 0x08);
	pnp_write_config(SERIAL_DEV, 0x29, 0x95);
	pnp_write_config(SERIAL_DEV, 0x2a, 0xcf);

	pnp_set_logical_device(POWER_DEV);
	/* Configure pin for PECI */
	pnp_write_config(POWER_DEV, 0xf3, 0x18);

	nuvoton_pnp_exit_conf_state(POWER_DEV);

	if (CONFIG(CONSOLE_SERIAL))
		nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
