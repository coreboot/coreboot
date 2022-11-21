/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PHY_M88E1512_H_
#define _PHY_M88E1512_H_

/* Register layout */
#define PAGE_REG		0x16
/* Page 0 registers */
#define COPPER_CTRL_REG		0
#define  SOFTWARE_RESET		(1 << 15)
#define COPPER_SPEC_CTRL_REG_1	0x10
#define DOWNSHIFT_CNT_MASK	0x7000
#define DOWNSHIFT_CNT_MAX	8
#define  DOWNSHIFT_CNT(cnt)	((cnt - 1) << 12)
#define  DOWNSHIFT_EN		(1 << 11)
/* Page 3 registers */
#define LED_FUNC_CTRL_REG	0x10
#define LED_FUNC_CTRL_MASK	0x0FFF
#define LED_TIMER_CTRL_REG	0x12
#define  LED_IRQ_ENABLE		(1 << 7)

#endif /* _PHY_M88E1512_H_ */
