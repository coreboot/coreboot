/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PHY_M88E1512_H_
#define _PHY_M88E1512_H_

/* Register layout */
#define PAGE_REG		0x16
#define LED_FUNC_CTRL_REG	0x10
#define LED_FUNC_CTRL_MASK	0x0FFF
#define LED_TIMER_CTRL_REG	0x12
#define  LED_IRQ_ENABLE		(1 << 7)

#endif /* _PHY_M88E1512_H_ */
