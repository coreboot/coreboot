/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6687d/nct6687d.h>
#include <superio/nuvoton/nct6687d/nct6687d_ec.h>

#define GPIO_DEV PNP_DEV(0x4e, NCT6687D_GPIO_0_7)
#define SERIAL_DEV PNP_DEV(0x4e, NCT6687D_SP1)
#define POWER_DEV PNP_DEV(0x4e, NCT6687D_SLEEP_PWR)
#define P80_UART_DEV PNP_DEV(0x4e, NCT6687D_P80_UART)
#define EC_DEV PNP_DEV(0x4e, NCT6687D_EC)

#define EC_IO_BASE 0xa20

void bootblock_mainboard_early_init(void)
{
	/* Replicate vendor settings for multi-function pins in global config LDN */
	nuvoton_pnp_enter_conf_state(GPIO_DEV);
	pnp_write_config(GPIO_DEV, 0x13, 0xff); // IRQ8-15 level triggered, low
	pnp_write_config(GPIO_DEV, 0x14, 0xff); // IRQ0-7 level triggered, low

	/* Below are multi-pin function */
	pnp_write_config(GPIO_DEV, 0x15, 0xaa);
	pnp_write_config(GPIO_DEV, 0x1a, 0x02);
	pnp_write_config(GPIO_DEV, 0x1b, 0x02);
	pnp_write_config(GPIO_DEV, 0x1d, 0x00);
	pnp_write_config(GPIO_DEV, 0x1e, 0xaa);
	pnp_write_config(GPIO_DEV, 0x1f, 0xb2);
	pnp_write_config(GPIO_DEV, 0x22, 0xbd);
	pnp_write_config(GPIO_DEV, 0x23, 0xdf);
	pnp_write_config(GPIO_DEV, 0x24, 0x39);
	pnp_write_config(GPIO_DEV, 0x25, 0xfe);
	pnp_write_config(GPIO_DEV, 0x26, 0x40);
	pnp_write_config(GPIO_DEV, 0x27, 0x77);
	pnp_write_config(GPIO_DEV, 0x28, 0x00);
	pnp_write_config(GPIO_DEV, 0x29, 0xfb);
	pnp_write_config(GPIO_DEV, 0x2a, 0x80);
	pnp_write_config(GPIO_DEV, 0x2b, 0x20);
	pnp_write_config(GPIO_DEV, 0x2c, 0x8a);
	pnp_write_config(GPIO_DEV, 0x2d, 0xaa);

	/* Set GPIO 06 as high output */
	pnp_write_config(GPIO_DEV, 0xF0, 0x00);
	pnp_unset_and_set_config(GPIO_DEV, 0xe0, 0x40, 0x40);
	pnp_unset_and_set_config(GPIO_DEV, 0xe3, 0x40, 0x00);

	pnp_set_logical_device(POWER_DEV);
	/* Configure pin for PECI */
	pnp_write_config(POWER_DEV, 0xf3, 0x80);

	/* Configure power fault detection */
	pnp_unset_and_set_config(POWER_DEV, 0xf0, 0xF2, 0x30);
	pnp_unset_and_set_config(POWER_DEV, 0xf0, 0x03, 0x01);

	/* Configure yellow and green LED */
	pnp_write_config(POWER_DEV, 0xe7, 0x88);
	pnp_write_config(POWER_DEV, 0xe8, 0x07);
	pnp_set_logical_device(P80_UART_DEV);
	pnp_write_config(P80_UART_DEV, 0xe5, 0x0f);

	/* Configure EC */
	pnp_set_logical_device(EC_DEV);
	pnp_set_iobase(EC_DEV, PNP_IDX_IO0, EC_IO_BASE);
	pnp_set_enable(EC_DEV, 1);

	nct6687d_ec_and_or_page_ff(EC_IO_BASE, 6, 0x61, 0x3f, 0xC0);
	nct6687d_ec_and_or_page(EC_IO_BASE, 0, 0x34, 0xc0, 0x00);
	nct6687d_ec_and_or_page_ff(EC_IO_BASE, 8, 0x07, 0xff, 0x03);
	nct6687d_ec_and_or_page(EC_IO_BASE, 0, 0x3d, 0xbf, 0x40);
	nct6687d_ec_and_or_page_ff(EC_IO_BASE, 8, 0x00, 0xff, 0x01);
	nct6687d_ec_and_or_page_ff(EC_IO_BASE, 0, 0x34, 0xfb, 0x04);


	nuvoton_pnp_exit_conf_state(EC_DEV);

	if (CONFIG(CONSOLE_SERIAL))
		nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
