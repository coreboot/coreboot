/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/i2c.h>
#include <console/console.h>
#include <types.h>
#include "i23c_pad_def.h"

void fch_i23c_pad_init(unsigned int bus,
		       enum i2c_speed speed,
		       const struct i2c_pad_control *ctrl)
{
	uint32_t pad_ctrl;

	pad_ctrl = misc_read32(MISC_I23C_PAD_CTRL(bus));

	pad_ctrl &= ~I23C_PAD_CTRL_MODE_I3C_I2C_MASK;
	pad_ctrl |= I23C_PAD_CTRL_MODE_I2C;

	switch (ctrl->rx_level) {
	case I2C_PAD_RX_NO_CHANGE:
		/* Default is enabled and thresholds for 1.8V operation */
		break;
	case I2C_PAD_RX_OFF:
		pad_ctrl &= ~I23C_PAD_CTRL_RX_SEL_MASK;
		pad_ctrl |= I23C_PAD_CTRL_RX_SEL_OFF;
		pad_ctrl &= ~I23C_PAD_CTRL_MODE_1_8V_1_1V_MASK;
		pad_ctrl |= I23C_PAD_CTRL_MODE_1_8V;
		break;
	case I2C_PAD_RX_1_8V:
		pad_ctrl &= ~I23C_PAD_CTRL_RX_SEL_MASK;
		pad_ctrl |= I23C_PAD_CTRL_RX_SEL_ON;
		pad_ctrl &= ~I23C_PAD_CTRL_MODE_1_8V_1_1V_MASK;
		pad_ctrl |= I23C_PAD_CTRL_MODE_1_8V;
		break;
	case I2C_PAD_RX_1_1V:
		pad_ctrl &= ~I23C_PAD_CTRL_RX_SEL_MASK;
		pad_ctrl |= I23C_PAD_CTRL_RX_SEL_ON;
		pad_ctrl &= ~I23C_PAD_CTRL_MODE_1_8V_1_1V_MASK;
		pad_ctrl |= I23C_PAD_CTRL_MODE_1_1V;
		break;
	default:
		printk(BIOS_WARNING, "Invalid I2C/I3C pad RX level for bus %u\n", bus);
		break;
	}

	pad_ctrl &= ~I23C_PAD_CTRL_FALLSLEW_SEL_MASK;
	pad_ctrl |= speed == I2C_SPEED_STANDARD ?
		I23C_PAD_CTRL_FALLSLEW_SEL_STD : I23C_PAD_CTRL_FALLSLEW_SEL_LOW;

	pad_ctrl &= ~I23C_PAD_CTRL_SLEW_N_MASK;
	pad_ctrl |= I23C_PAD_CTRL_SLEW_N_FAST;

	misc_write32(MISC_I23C_PAD_CTRL(bus), pad_ctrl);
}
