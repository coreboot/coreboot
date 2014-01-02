/*
 * Definitions for GPIO and pin multiplexing on Allwinner CPUs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef __CPU_ALLWINNER_A10_PINMUX_H
#define __CPU_ALLWINNER_A10_PINMUX_H

#include <types.h>

#define GPIO_BASE		 0x01C20800

#define GPA			0
#define GPB			1
#define GPC			2
#define GPD			3
#define GPE			4
#define GPF			5
#define GPG			6
#define GPH			7
#define GPI			8
#define GPS			9

/* GPIO pad functions valid for all pins */
#define GPIO_PIN_FUNC_INPUT	0
#define GPIO_PIN_FUNC_OUTPUT	1

struct a10_gpio_port {
	u32 cfg[4];
	u32 dat;
	u32 drv[2];
	u32 pul[2];
} __attribute__ ((packed));

struct a10_gpio {
	struct a10_gpio_port port[10];
	u8 reserved_0x168[0x98];

	/* Offset 0x200 */
	u32 int_cfg[4];

	u32 int_ctl;
	u32 int_sta;
	u8 reserved_0x21C[4];
	u32 int_deb;

	u32 sdr_pad_drv;
	u32 sdr_pad_pul;
} __attribute__ ((packed));

/* gpio.c */
void gpio_set(u8 port, u8 pin);
void gpio_clear(u8 port, u8 pin);
int gpio_get(u8 port, u8 pin);
void gpio_write(u8 port, u32 val);
u32 gpio_read(u8 port);

/* pinmux.c */
void gpio_set_pin_func(u8 port, u8 pin, u8 pad_func);
void gpio_set_multipin_func(u8 port, u32 pin_mask, u8 pad_func);

#endif				/* __CPU_ALLWINNER_A10_PINMUX_H */
