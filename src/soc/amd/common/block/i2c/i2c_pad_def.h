/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_I2C_PAD_DEF_H
#define AMD_BLOCK_I2C_PAD_DEF_H

#include <types.h>

/* MISC_I2Cx_PAD_CTRL and MISC_I23Cx_PAD_CTRL are in the same place, but have different bit
   definitions. Which one is present depends on the SoC. */
#define MISC_I2C0_PAD_CTRL		0xd8
#define MISC_I2C_PAD_CTRL(bus)		(MISC_I2C0_PAD_CTRL + 4 * (bus))

#define   I2C_PAD_CTRL_NG_MASK		(BIT(0) | BIT(1) | BIT(2) | BIT(3))
#define     I2C_PAD_CTRL_NG_NORMAL	0xc
#define   I2C_PAD_CTRL_RX_SEL_MASK	(BIT(4) | BIT(5))
#define     I2C_PAD_CTRL_RX_SHIFT	4
#define     I2C_PAD_CTRL_RX_SEL_OFF	(0 << I2C_PAD_CTRL_RX_SHIFT)
#define     I2C_PAD_CTRL_RX_SEL_3_3V	(1 << I2C_PAD_CTRL_RX_SHIFT)
#define     I2C_PAD_CTRL_RX_SEL_1_8V	(3 << I2C_PAD_CTRL_RX_SHIFT)
#define   I2C_PAD_CTRL_PULLDOWN_EN	BIT(6)
#define   I2C_PAD_CTRL_FALLSLEW_MASK	(BIT(7) | BIT(8))
#define     I2C_PAD_CTRL_FALLSLEW_SHIFT	7
#define     I2C_PAD_CTRL_FALLSLEW_STD	(0 << I2C_PAD_CTRL_FALLSLEW_SHIFT)
#define     I2C_PAD_CTRL_FALLSLEW_LOW	(1 << I2C_PAD_CTRL_FALLSLEW_SHIFT)
#define   I2C_PAD_CTRL_FALLSLEW_EN	BIT(9)
#define   I2C_PAD_CTRL_SPIKE_RC_EN	BIT(10)
#define   I2C_PAD_CTRL_SPIKE_RC_SEL	BIT(11) /* 0 = 50ns, 1 = 20ns */
#define   I2C_PAD_CTRL_CAP_DOWN		BIT(12)
#define   I2C_PAD_CTRL_CAP_UP		BIT(13)
#define   I2C_PAD_CTRL_RES_DOWN		BIT(14)
#define   I2C_PAD_CTRL_RES_UP		BIT(15)
#define   I2C_PAD_CTRL_BIAS_CRT_EN	BIT(16)
#define   I2C_PAD_CTRL_SPARE0		BIT(17)
#define   I2C_PAD_CTRL_SPARE1		BIT(18)
/* The following bits are reserved in Picasso and Cezanne */
#define   I2C_PAD_CTRL_PD_EN		BIT(19)
#define   I2C_PAD_CTRL_COMP_SEL		BIT(20)
#define   I2C_PAD_CTRL_RES_BIAS_EN	BIT(21)

#endif /* AMD_BLOCK_I2C_PAD_DEF_H */
