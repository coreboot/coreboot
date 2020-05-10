/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_OAK_GPIO_H__
#define __MAINBOARD_GOOGLE_OAK_GPIO_H__
#include <soc/gpio.h>

#define LID		((board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 7) ? \
			GPIO(EINT12) : GPIO(SPI_CK))
#define RAM_ID_1	GPIO(RCN_A)
#define RAM_ID_2	GPIO(RCP_A)

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
