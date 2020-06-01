/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
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

static void early_lpc_init(void)
{
	u32 setting = 0x0;

	//
	// Configure output disabled, value low, pull up/down disabled
	//
	if (CONFIG(BOARD_PCENGINES_APU5)) {
		configure_gpio(GPIO_22, Function0, setting);
	}

	if (CONFIG(BOARD_PCENGINES_APU2) ||
		CONFIG(BOARD_PCENGINES_APU3) ||
		CONFIG(BOARD_PCENGINES_APU4)) {
		configure_gpio(GPIO_32, Function0, setting);
	}

	configure_gpio(GPIO_49, Function2, setting);
	configure_gpio(GPIO_50, Function2, setting);
	configure_gpio(GPIO_71, Function0, setting);

	//
	// Configure output enabled, value low, pull up/down disabled
	//
	setting = GPIO_OUTPUT_ENABLE;
	if (CONFIG(BOARD_PCENGINES_APU3) ||
		CONFIG(BOARD_PCENGINES_APU4)) {
		configure_gpio(GPIO_33, Function0, setting);
	}

	configure_gpio(GPIO_57, Function1, setting);
	configure_gpio(GPIO_58, Function1, setting);
	configure_gpio(GPIO_59, Function3, setting);

	//
	// Configure output enabled, value high, pull up/down disabled
	//
	setting = GPIO_OUTPUT_ENABLE | GPIO_OUTPUT_VALUE;

	if (CONFIG(BOARD_PCENGINES_APU5)) {
		configure_gpio(GPIO_32, Function0, setting);
		configure_gpio(GPIO_33, Function0, setting);
	}

	configure_gpio(GPIO_51, Function2, setting);
	configure_gpio(GPIO_55, Function3, setting);
	configure_gpio(GPIO_64, Function2, setting);
	configure_gpio(GPIO_68, Function0, setting);
}
