/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <bootblock_common.h>
#include <device/pnp_def.h>
#include <device/pnp_ops.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define SERIAL_DEV	PNP_DEV(0x2e, IT8728F_SP1)
#define GPIO_DEV	PNP_DEV(0x2e, IT8728F_GPIO)
#define ENVC_DEV	PNP_DEV(0x2e, IT8728F_EC)

static void ite_evc_conf(pnp_devfn_t dev)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_write_config(dev, PNP_IDX_MSC1, 0x40);
	pnp_write_config(dev, PNP_IDX_MSC4, 0x80);
	pnp_write_config(dev, PNP_IDX_MSC5, 0x00);
	pnp_write_config(dev, PNP_IDX_MSC6, 0xf0);
	pnp_write_config(dev, PNP_IDX_MSC9, 0x48);
	pnp_write_config(dev, PNP_IDX_MSCA, 0x00);
	pnp_write_config(dev, PNP_IDX_MSCB, 0x00);
	pnp_exit_conf_state(dev);
}

static void ite_gpio_conf(pnp_devfn_t dev)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_write_config(dev, 0x25, 0x80);
	pnp_write_config(dev, 0x26, 0x07);
	pnp_write_config(dev, 0x28, 0x81);
	pnp_write_config(dev, 0x2c, 0x06);
	pnp_write_config(dev, PNP_IDX_IRQ1, 0x00);
	pnp_write_config(dev, 0x73, 0x00);
	pnp_write_config(dev, 0xb3, 0x01);
	pnp_write_config(dev, 0xb8, 0x00);
	pnp_write_config(dev, 0xc0, 0x00);
	pnp_write_config(dev, 0xc3, 0x00);
	pnp_write_config(dev, 0xc8, 0x00);
	pnp_write_config(dev, 0xc9, 0x07);
	pnp_write_config(dev, 0xcb, 0x01);
	pnp_write_config(dev, PNP_IDX_MSC0, 0x10);
	pnp_write_config(dev, PNP_IDX_MSC4, 0x27);
	pnp_write_config(dev, PNP_IDX_MSC8, 0x20);
	pnp_write_config(dev, PNP_IDX_MSC9, 0x01);
	pnp_exit_conf_state(dev);
}

void bootblock_mainboard_early_init(void)
{
	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	pm_write8(0xea, 0x1);

	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 48MHz */
	misc_write32(0x28, misc_read32(0x28) & 0xfff8ffff);

	/* Enable Auxiliary Clock1, disable FCH 14 MHz OscClk */
	misc_write32(0x40, misc_read32(0x40) & 0xffffbffb);

	/* Configure SIO as made under vendor BIOS */
	ite_evc_conf(ENVC_DEV);
	ite_gpio_conf(GPIO_DEV);

	/* Enable serial output on it8728f */
	ite_kill_watchdog(GPIO_DEV);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
