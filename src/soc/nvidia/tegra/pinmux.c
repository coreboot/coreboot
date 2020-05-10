/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/addressmap.h>

#include "pinmux.h"

static uint32_t *pinmux_regs = (void *)TEGRA_APB_PINMUX_BASE;

void pinmux_set_config(int pin_index, uint32_t config)
{
	write32(&pinmux_regs[pin_index], config);
}

uint32_t pinmux_get_config(int pin_index)
{
	return read32(&pinmux_regs[pin_index]);
}
