/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/gpio_banks.h>
#include <amdblocks/gpio_defs.h>
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

const struct soc_amd_gpio gpio_common[] = {
	/* Output disabled, pull up/down disabled */
	PAD_CFG_STRUCT(GPIO_49, Function2, PAD_PULL(PULL_NONE)),
	PAD_CFG_STRUCT(GPIO_50, Function2, PAD_PULL(PULL_NONE)),
	PAD_CFG_STRUCT(GPIO_71, Function0, PAD_PULL(PULL_NONE)),
	/* Output enabled, value low, pull up/down disabled */
	PAD_CFG_STRUCT(GPIO_57, Function1, PAD_OUTPUT(LOW)),
	PAD_CFG_STRUCT(GPIO_58, Function1, PAD_OUTPUT(LOW)),
	PAD_CFG_STRUCT(GPIO_59, Function3, PAD_OUTPUT(LOW)),
	/* Output enabled, value high, pull up/down disabled */
	PAD_CFG_STRUCT(GPIO_51, Function2, PAD_OUTPUT(HIGH)),
	PAD_CFG_STRUCT(GPIO_55, Function3, PAD_OUTPUT(HIGH)),
	PAD_CFG_STRUCT(GPIO_64, Function2, PAD_OUTPUT(HIGH)),
	PAD_CFG_STRUCT(GPIO_68, Function0, PAD_OUTPUT(HIGH)),
};

const struct soc_amd_gpio gpio_apu2[] = {
	/* Output disabled, pull up/down disabled */
	PAD_CFG_STRUCT(GPIO_32, Function0, PAD_PULL(PULL_NONE)),
};

const struct soc_amd_gpio gpio_apu34[] = {
	/* Output disabled, pull up/down disabled */
	PAD_CFG_STRUCT(GPIO_32, Function0, PAD_PULL(PULL_NONE)),
	/* Output enabled, value low, pull up/down disabled */
	PAD_CFG_STRUCT(GPIO_33, Function0, PAD_OUTPUT(LOW)),
};

const struct soc_amd_gpio gpio_apu5[] = {
	/* Output disabled, pull up/down disabled */
	PAD_CFG_STRUCT(GPIO_22, Function0, PAD_PULL(PULL_NONE)),
	/* Output enabled, value high, pull up/down disabled */
	PAD_CFG_STRUCT(GPIO_32, Function0, PAD_OUTPUT(HIGH)),
	PAD_CFG_STRUCT(GPIO_33, Function0, PAD_OUTPUT(HIGH)),
};

static void early_lpc_init(void)
{
	program_gpios(gpio_common, ARRAY_SIZE(gpio_common));

	if (CONFIG(BOARD_PCENGINES_APU2))
		program_gpios(gpio_apu2, ARRAY_SIZE(gpio_apu2));

	if (CONFIG(BOARD_PCENGINES_APU3) || CONFIG(BOARD_PCENGINES_APU4))
		program_gpios(gpio_apu34, ARRAY_SIZE(gpio_apu34));

	if (CONFIG(BOARD_PCENGINES_APU5))
		program_gpios(gpio_apu5, ARRAY_SIZE(gpio_apu5));
}
