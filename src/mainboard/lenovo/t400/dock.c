/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__
#include <console/console.h>
#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/device.h>
#include <device/pnp.h>
#include <delay.h>
#include "dock.h"
#include <superio/nsc/pc87382/pc87382.h>

#include <southbridge/intel/i82801ix/i82801ix.h>
#include <ec/lenovo/h8/h8.h>
#include <ec/acpi/ec.h>

struct pin_config {
	u8 port;
	u8 mode;
};

static int poll_clk_stable(pnp_devfn_t dev, int timeout)
{
	/* Enable 14.318MHz CLK on CLKIN */
	pnp_write_config(dev, 0x29, 0xa0);
	while(!(pnp_read_config(dev, 0x29) & 0x10) && timeout--)
		udelay(1000);
	if (!timeout)
		return 1;

	return 0;
}

static int gpio_init(pnp_devfn_t gpio, u16 gpio_base,
	const struct pin_config pincfg[], int num_cfgs)
{
	int i;

	/* Enable GPIO LDN. */
	pnp_set_logical_device(gpio);
	pnp_set_iobase(gpio, PNP_IDX_IO0, gpio_base);
	pnp_set_enable(gpio, 1);

	for (i = 0; i < num_cfgs; i++) {
		pnp_write_config(gpio, 0xf0, pincfg[i].port);
		pnp_write_config(gpio, 0xf1, pincfg[i].mode);
		pnp_write_config(gpio, 0xf2, 0x0);
	}
	return 0;
}

static const pnp_devfn_t l_dlpc = PNP_DEV(0x164e, PC87382_DOCK);
static const pnp_devfn_t l_gpio = PNP_DEV(0x164e, PC87382_GPIO);

static int pc87382_init(pnp_devfn_t dlpc, u16 dlpc_base)
{
	/* Maximum 3300 LCLKs at 14.318MHz */
	int timeout = 230;

	/* Enable LPC bridge LDN. */
	pnp_set_logical_device(dlpc);
	pnp_set_iobase(dlpc, PNP_IDX_IO0, dlpc_base);
	pnp_set_enable(dlpc, 1);

	/* Reset docking state */
	outb(0x00, dlpc_base);
	outb(0x07, dlpc_base);
	while (!(inb(dlpc_base) & 8) && timeout--)
		udelay(1);
	if (!timeout)
		return 1;

	return 0;
}

static void pc87382_close(pnp_devfn_t dlpc)
{
	pnp_set_logical_device(dlpc);

	/* Disconnect LPC bus */
	u16 dlpc_base = pnp_read_iobase(dlpc, PNP_IDX_IO0);
	if (dlpc_base) {
		outb(0x00, dlpc_base);
		pnp_set_enable(dlpc, 0);
	}
}

static const struct pin_config local_gpio[] = {
	{0x00, 3},	{0x01, 3},	{0x02, 0},	{0x03, 3},
	{0x04, 4},	{0x20, 4},	{0x21, 4},	{0x23, 4},
};

/* Enable internal clock and configure GPIO LDN */
int pc87382_early(void)
{
	/* Wake-up time is 33 msec (maximum). */
	if (poll_clk_stable(l_gpio, 33) != 0)
		return 1;

	/* Set up GPIOs */
	if (gpio_init(l_gpio, DLPC_GPIO_BASE,
		local_gpio, ARRAY_SIZE(local_gpio)) != 0) {
		return 1;
	}

	return 0;
}

static int pc87382_connect(void)
{
	u8 reg;

	reg = inb(DLPC_GPDO0);
	reg |= D_PLTRST | D_LPCPD;
	/* Deassert D_PLTRST# and D_LPCPD# */
	outb(reg, DLPC_GPDO0);

	if (pc87382_init(l_dlpc, DLPC_CONTROL) != 0)
		return 1;

	/* Assert D_PLTRST# */
	reg &= ~D_PLTRST;
	outb(reg, DLPC_GPDO0);
	udelay(1000);

	/* Deassert D_PLTRST# */
	reg |= D_PLTRST;
	outb(reg, DLPC_GPDO0);
	mdelay(10);

	return 0;
}

static void pc87382_disconnect(void)
{
	pc87382_close(l_dlpc);

	/* Assert D_PLTRST# and D_LPCPD# */
	u8 reg = inb(DLPC_GPDO0);
	reg &= ~(D_PLTRST | D_LPCPD);
	outb(reg, DLPC_GPDO0);
}

/* Returns 3bit dock id */
static u8 dock_identify(void)
{
	u8 id;

	/* Make sure GPIO LDN is configured first ! */
	id = (inb(DLPC_GPDI0) >> 4) & 1;
	id |= (inb(DLPC_GPDI2) & 3) << 1;

	return id;
}

/* Docking station side. */

#include <superio/nsc/pc87384/pc87384.h>

static const pnp_devfn_t r_gpio = PNP_DEV(SUPERIO_DEV, PC87384_GPIO);
static const pnp_devfn_t r_serial = PNP_DEV(SUPERIO_DEV, PC87384_SP1);

static const struct pin_config remote_gpio[] = {
	{0x00, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x01, PC87384_GPIO_PIN_TYPE_PUSH_PULL | PC87384_GPIO_PIN_OE},
	{0x02, PC87384_GPIO_PIN_TYPE_PUSH_PULL | PC87384_GPIO_PIN_OE},
	{0x03, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x04, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x05, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x06, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x07, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
};

static int pc87384_init(void)
{
	if (poll_clk_stable(r_gpio, 1000) != 0)
		return 1;

	/* set GPIO pins to Serial/Parallel Port
	 * functions
	 */
	pnp_write_config(r_gpio, 0x22, 0xa9);

	/* enable serial port */

	if (CONFIG_TTYS0_BASE > 0) {
		pnp_set_logical_device(r_serial);
		pnp_set_iobase(r_serial, PNP_IDX_IO0, CONFIG_TTYS0_BASE);
		pnp_set_enable(r_serial, 1);
	}

	if (gpio_init(r_gpio, DOCK_GPIO_BASE,
		remote_gpio, ARRAY_SIZE(remote_gpio)) != 0)
		return 1;

	/* no GPIO events enabled for PORT0 */
	outb(0x00, DOCK_GPIO_BASE + 0x02);
	/* clear GPIO events on PORT0 */
	outb(0xff, DOCK_GPIO_BASE + 0x03);
	outb(0xff, DOCK_GPIO_BASE + 0x04);

	/* no GPIO events enabled for PORT1 */
	outb(0x00, DOCK_GPIO_BASE + 0x06);
	/* clear GPIO events on PORT1*/
	outb(0xff, DOCK_GPIO_BASE + 0x07);
	outb(0x1f, DOCK_GPIO_BASE + 0x08);

	outb(0xfd, DOCK_GPIO_BASE + 0x00);

	return 0;
}

/* Mainboard */

void dock_connect(void)
{
	const u8 id = dock_identify();

	/* Dock type 2505 doesn't have serial, LPT port or LEDs */
	if (id == DOCK_TYPE_NONE || id == DOCK_TYPE_2505)
		return;

	if (pc87382_connect() != 0 || pc87384_init() != 0) {
		pc87382_disconnect();
		return;
	}

	ec_write(H8_LED_CONTROL,
		 H8_LED_CONTROL_OFF | H8_LED_CONTROL_DOCK_LED1);
	ec_write(H8_LED_CONTROL,
		 H8_LED_CONTROL_ON  | H8_LED_CONTROL_DOCK_LED2);
}

void dock_disconnect(void)
{
	pc87382_disconnect();

	ec_write(H8_LED_CONTROL,
		 H8_LED_CONTROL_OFF | H8_LED_CONTROL_DOCK_LED1);
	ec_write(H8_LED_CONTROL,
		 H8_LED_CONTROL_OFF | H8_LED_CONTROL_DOCK_LED2);
}

void dock_info(void)
{
	const u8 id = dock_identify();

	if (id != DOCK_TYPE_NONE)
		printk(BIOS_DEBUG, "DOCK: is present: id=%d\n", id);
	else
		printk(BIOS_DEBUG, "DOCK: not connected\n");
}
