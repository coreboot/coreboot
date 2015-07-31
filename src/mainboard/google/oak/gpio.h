/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __MAINBOARD_GOOGLE_OAK_GPIO_H__
#define __MAINBOARD_GOOGLE_OAK_GPIO_H__
#include <soc/pinmux.h>

enum {
	LID		= PAD_EINT12,
	/* Write Protect */
	WRITE_PROTECT	= PAD_EINT4,
	/* Power button */
	POWER_BUTTON	= PAD_EINT14,
	/* EC Interrupt */
	EC_IRQ          = PAD_EINT0,
	/* EC in RW signal */
	EC_IN_RW	= PAD_DAIPCMIN,
	/* EC AP suspend */
	EC_SUSPEND_L	= PAD_KPROW1,
};

void setup_chromeos_gpios(void);

#endif /* __MAINBOARD_GOOGLE_OAK_GPIO_H__ */
