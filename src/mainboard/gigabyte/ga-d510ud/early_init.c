/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <console/console.h>
#include <device/pnp_ops.h>
#include <northbridge/intel/pineview/pineview.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8720f/it8720f.h>

#define SERIAL_DEV	PNP_DEV(0x2e, IT8720F_SP1)
#define GPIO_DEV	PNP_DEV(0x2e, IT8720F_GPIO)
#define EC_DEV		PNP_DEV(0x2e, IT8720F_EC)

void bootblock_mainboard_early_init(void)
{
	/* Set default GPIOs on superio */
	pnp_enter_conf_state(GPIO_DEV);

	pnp_set_logical_device(GPIO_DEV);
	pnp_write_config(GPIO_DEV, 0x25, 0x00);
	pnp_write_config(GPIO_DEV, 0x26, 0x0c);
	pnp_write_config(GPIO_DEV, 0x28, 0x41);
	pnp_write_config(GPIO_DEV, 0x2c, 0x01);
	pnp_write_config(GPIO_DEV, 0x62, 0x08);
	pnp_write_config(GPIO_DEV, 0x72, 0x00);
	pnp_write_config(GPIO_DEV, 0xb8, 0x00);
	pnp_write_config(GPIO_DEV, 0xc0, 0x00);
	pnp_write_config(GPIO_DEV, 0xc1, 0x0c);
	pnp_write_config(GPIO_DEV, 0xc3, 0x41);
	pnp_write_config(GPIO_DEV, 0xc8, 0x00);
	pnp_write_config(GPIO_DEV, 0xcb, 0x00);
	pnp_write_config(GPIO_DEV, 0xe9, 0x01);
	pnp_write_config(GPIO_DEV, 0xfc, 0xa4);

	pnp_exit_conf_state(GPIO_DEV);

	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	pnp_enter_conf_state(GPIO_DEV);
	pnp_set_logical_device(GPIO_DEV);

	printk(BIOS_DEBUG, "ITE register 0xef: 0x%02x ", pnp_read_config(GPIO_DEV, 0xef));

	/* Disable SIO reboot */
	pnp_write_config(GPIO_DEV, 0xef, 0x7e);

	printk(BIOS_DEBUG, "=> 0x%02x\n", pnp_read_config(GPIO_DEV, 0xef));

	pnp_set_logical_device(EC_DEV);
	pnp_write_config(EC_DEV, 0x70, 0x00); /* Don't use IRQ9 */
	pnp_write_config(EC_DEV, 0x30, 0x01); /* Enable */

	pnp_exit_conf_state(GPIO_DEV);
}

void get_mb_spd_addrmap(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[1] = 0x51;
}
