/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <gpio.h>
#include <northbridge/amd/agesa/state_machine.h>

#include "gpio_ftns.h"

static void early_lpc_init(void);

void board_BeforeAgesa(struct sysinfo *cb)
{
	u32 val;

	early_lpc_init();

	/* Disable SVI2 controller to wait for command completion */
	val = pci_read_config32(PCI_DEV(0, 0x18, 5), 0x12C);
	if (!(val & (1 << 30))) {
		val |= (1 << 30);
		pci_write_config32(PCI_DEV(0, 0x18, 5), 0x12C, val);
	}

	/* Release GPIO32/33 for other uses. */
	pm_write8(0xea, 1);
}

static void pin_input(gpio_t gpio, u8 iomux_ftn)
{
	iomux_write8(gpio, iomux_ftn);
	gpio_input(gpio);
}

static void pin_low(gpio_t gpio, u8 iomux_ftn)
{
	iomux_write8(gpio, iomux_ftn);
	gpio_output(gpio, 0);
}

static void pin_high(gpio_t gpio, u8 iomux_ftn)
{
	iomux_write8(gpio, iomux_ftn);
	gpio_output(gpio, 1);
}

static void early_lpc_init(void)
{
	//
	// Configure output disabled, pull up/down disabled
	//
	if (CONFIG(BOARD_PCENGINES_APU5))
		pin_input(GPIO_22, Function0);

	if (CONFIG(BOARD_PCENGINES_APU2) ||
		CONFIG(BOARD_PCENGINES_APU3) ||
		CONFIG(BOARD_PCENGINES_APU4)) {
		pin_input(GPIO_32, Function0);
	}

	pin_input(GPIO_49, Function2);
	pin_input(GPIO_50, Function2);
	pin_input(GPIO_71, Function0);

	//
	// Configure output enabled, value low, pull up/down disabled
	//
	if (CONFIG(BOARD_PCENGINES_APU3) ||
		CONFIG(BOARD_PCENGINES_APU4)) {
		pin_low(GPIO_33, Function0);
	}
	pin_low(GPIO_57, Function1);
	pin_low(GPIO_58, Function1);
	pin_low(GPIO_59, Function3);

	//
	// Configure output enabled, value high, pull up/down disabled
	//
	if (CONFIG(BOARD_PCENGINES_APU5)) {
		pin_high(GPIO_32, Function0);
		pin_high(GPIO_33, Function0);
	}

	pin_high(GPIO_51, Function2);
	pin_high(GPIO_55, Function3);
	pin_high(GPIO_64, Function2);
	pin_high(GPIO_68, Function0);
}
