/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <arch/io.h>
#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8623e/it8623e.h>

#if CONFIG_UART_FOR_CONSOLE == 0
#define SERIAL_DEV		PNP_DEV(0x2e, IT8623E_SP1)
#elif CONFIG_UART_FOR_CONSOLE == 1
#define SERIAL_DEV		PNP_DEV(0x2e, IT8623E_SP2)
#else
#error "Invalid value for CONFIG_UART_FOR_CONSOLE"
#endif

#define GPIO_DEV		PNP_DEV(0x2e, IT8623E_GPIO)
#define CLKIN_DEV		PNP_DEV(0x2e, IT8623E_GPIO)
#define ENVC_DEV		PNP_DEV(0x2e, IT8623E_EC)

/* Sets up EC configuration as per vendor defaults */
static void ite_evc_conf(pnp_devfn_t dev)
{
	pnp_set_enable(dev, 0);
	ite_reg_write(dev, 0x70, 0x00);
	ite_reg_write(dev, 0xf0, 0x00);
	ite_reg_write(dev, 0xf1, 0x00);
	ite_reg_write(dev, 0xf2, 0x06);
	ite_reg_write(dev, 0xf3, 0x00);
	ite_reg_write(dev, 0xf4, 0x00);
	ite_reg_write(dev, 0xf5, 0x36);
	ite_reg_write(dev, 0xf6, 0x03);
	ite_reg_write(dev, 0xf9, 0x48);
	ite_reg_write(dev, 0xfa, 0x00);
	ite_reg_write(dev, 0xfb, 0x10);
	pnp_set_enable(dev, 1);
}

/*
 * Sets up GPIO configuration as per vendor defaults
 * SIO defaults are unknown therefore all GPIO pins are configured
*/
static void ite_gpio_conf(pnp_devfn_t dev)
{
	ite_reg_write(dev, 0x23, 0x08);
	ite_reg_write(dev, 0x25, 0x10);
	ite_reg_write(dev, 0x26, 0x00);
	ite_reg_write(dev, 0x27, 0x80);
	ite_reg_write(dev, 0x28, 0x45);
	ite_reg_write(dev, 0x29, 0x00);
	ite_reg_write(dev, 0x2a, 0x00);
	ite_reg_write(dev, 0x2b, 0x48);
	ite_reg_write(dev, 0x2c, 0x10);
	ite_reg_write(dev, 0x2d, 0x80);
	ite_reg_write(dev, 0x71, 0x00);
	ite_reg_write(dev, 0x72, 0x00);
	ite_reg_write(dev, 0x73, 0x38);
	ite_reg_write(dev, 0x74, 0x00);
	ite_reg_write(dev, 0xb0, 0x00);
	ite_reg_write(dev, 0xb1, 0x00);
	ite_reg_write(dev, 0xb2, 0x00);
	ite_reg_write(dev, 0xb3, 0x00);
	ite_reg_write(dev, 0xb4, 0x00);
	ite_reg_write(dev, 0xb8, 0x00);
	ite_reg_write(dev, 0xb9, 0x00);
	ite_reg_write(dev, 0xba, 0x00);
	ite_reg_write(dev, 0xbb, 0x00);
	ite_reg_write(dev, 0xbc, 0x00);
	ite_reg_write(dev, 0xbd, 0x00);
	ite_reg_write(dev, 0xc0, 0x01);
	ite_reg_write(dev, 0xc1, 0x00);
	ite_reg_write(dev, 0xc2, 0x00);
	ite_reg_write(dev, 0xc3, 0x00);
	ite_reg_write(dev, 0xc4, 0x00);
	ite_reg_write(dev, 0xc8, 0x01);
	ite_reg_write(dev, 0xc9, 0x00);
	ite_reg_write(dev, 0xca, 0x00);
	ite_reg_write(dev, 0xcb, 0x00);
	ite_reg_write(dev, 0xcc, 0x00);
	ite_reg_write(dev, 0xcd, 0x20);
	ite_reg_write(dev, 0xce, 0x00);
	ite_reg_write(dev, 0xcf, 0x00);
	ite_reg_write(dev, 0xe0, 0x00);
	ite_reg_write(dev, 0xe1, 0x00);
	ite_reg_write(dev, 0xe2, 0x00);
	ite_reg_write(dev, 0xe3, 0x00);
	ite_reg_write(dev, 0xe4, 0x00);
	ite_reg_write(dev, 0xe9, 0x21);
	ite_reg_write(dev, 0xf0, 0x00);
	ite_reg_write(dev, 0xf1, 0x00);
	ite_reg_write(dev, 0xf2, 0x00);
	ite_reg_write(dev, 0xf3, 0x00);
	ite_reg_write(dev, 0xf4, 0x00);
	ite_reg_write(dev, 0xf5, 0x00);
	ite_reg_write(dev, 0xf6, 0x00);
	ite_reg_write(dev, 0xf7, 0x00);
	ite_reg_write(dev, 0xf8, 0x00);
	ite_reg_write(dev, 0xf9, 0x00);
	ite_reg_write(dev, 0xfa, 0x00);
	ite_reg_write(dev, 0xfb, 0x00);
}

void bootblock_mainboard_early_init(void)
{
	u32 i;

	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	pm_write8(0xea, 0x1);

	/* Configure ClkDrvStr1 settings */
	misc_write32(0x24, 0x030800aa);

	/* Configure MiscClkCntl1 settings */
	misc_write32(0x40, 0x000c4050);

	/* Configure SIO as made under vendor BIOS */
	ite_gpio_conf(GPIO_DEV);
	ite_evc_conf(ENVC_DEV);

	/* Enable serial output on it8623e */
	ite_conf_clkin(CLKIN_DEV, ITE_UART_CLK_PREDIVIDE_48);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	ite_kill_watchdog(GPIO_DEV);

	/*
	* On Larne, after LpcClkDrvSth is set, it needs some time to be stable,
	* because of the buffer ICS551M
	*/
	for (i = 0; i < 200000; i++)
		inb(0xcd6);
}
