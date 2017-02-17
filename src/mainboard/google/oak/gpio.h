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
 */

#ifndef __MAINBOARD_GOOGLE_OAK_GPIO_H__
#define __MAINBOARD_GOOGLE_OAK_GPIO_H__
#include <soc/pinmux.h>

#define LID ((IS_ENABLED(CONFIG_BOARD_GOOGLE_ROWAN)) ? PAD_KPROW1 \
	: ((board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 7) ? PAD_EINT12 \
	: PAD_SPI_CK))


enum {
	/* Board ID related GPIOS. */
	BOARD_ID_0	= PAD_RDN3_A,
	BOARD_ID_1	= PAD_RDP3_A,
	BOARD_ID_2	= PAD_RDN2_A,
	/* RAM ID related GPIOS. */
	RAM_ID_0	= PAD_RDP2_A,
	RAM_ID_1	= PAD_RCN_A,
	RAM_ID_2	= PAD_RCP_A,
	RAM_ID_3	= PAD_RDN1_A,
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
