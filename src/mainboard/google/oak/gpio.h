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
#include <soc/gpio.h>

#if IS_ENABLED(CONFIG_BOARD_GOOGLE_ROWAN)
#define LID		GPIO(KPROW1)
#define RAM_ID_1	GPIO(DSI_TE)
#define RAM_ID_2	GPIO(RDP1_A)
#else
#define LID		((board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 7) ? \
			GPIO(EINT12) : GPIO(SPI_CK))
#define RAM_ID_1	GPIO(RCN_A)
#define RAM_ID_2	GPIO(RCP_A)
#endif

/* Board ID related GPIOS. */
#define BOARD_ID_0	GPIO(RDN3_A)
#define BOARD_ID_1	GPIO(RDP3_A)
#define BOARD_ID_2	GPIO(RDN2_A)
/* RAM ID related GPIOS. */
#define RAM_ID_0	GPIO(RDP2_A)
#define RAM_ID_3	GPIO(RDN1_A)
/* Write Protect */
#define WRITE_PROTECT	GPIO(EINT4)
/* Power button */
#define POWER_BUTTON	GPIO(EINT14)
/* EC Interrupt */
#define EC_IRQ          GPIO(EINT0)
/* EC in RW signal */
#define EC_IN_RW	GPIO(DAIPCMIN)
/* EC AP suspend */
#define EC_SUSPEND_L	GPIO(KPROW1)
/* Cr50 interrupt */
#define CR50_IRQ	GPIO(EINT16)

void setup_chromeos_gpios(void);

#endif /* __MAINBOARD_GOOGLE_OAK_GPIO_H__ */
