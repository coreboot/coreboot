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

void gpio_set_func(u8 port, u8 pin, u8 pad_func);

#endif				/* __CPU_ALLWINNER_A10_PINMUX_H */
