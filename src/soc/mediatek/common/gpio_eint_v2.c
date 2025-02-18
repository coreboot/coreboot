/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/gpio.h>
#include <soc/gpio_eint_v2.h>

void gpio_calc_eint_pos_bit(gpio_t gpio, u32 *pos, u32 *bit)
{
	uint32_t idx = gpio.id;

	*pos = 0;
	*bit = 0;

	if (idx >= eint_data_len)
		return;

	uint8_t index = eint_data[idx].index;

	*pos = index / MAX_EINT_REG_BITS;
	*bit = index % MAX_EINT_REG_BITS;
}

struct eint_regs *gpio_get_eint_reg(gpio_t gpio)
{
	uint32_t idx = gpio.id;
	uintptr_t addr;

	if (idx >= eint_data_len)
		return NULL;

	switch (eint_data[idx].instance) {
	case EINT_E:
		addr = EINT_E_BASE;
		break;
	case EINT_S:
		addr = EINT_S_BASE;
		break;
	case EINT_W:
		addr = EINT_W_BASE;
		break;
	case EINT_N:
		addr = EINT_N_BASE;
		break;
	case EINT_C:
		addr = EINT_C_BASE;
		break;
	default:
		printk(BIOS_ERR, "%s: Failed to look up a valid EINT base for %d\n",
		       __func__, idx);
		return NULL;
	}

	return (void *)addr;
}
