/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#ifndef __ACPI__
#include <gpio.h>

#if CONFIG(BOARD_GOOGLE_BASEBOARD_TREMBYLE)
#define EC_IN_RW_OD		GPIO_130
#else
#define EC_IN_RW_OD		GPIO_11
#endif

/* SPI Write protect */
#define CROS_WP_GPIO		GPIO_137
#define GPIO_EC_IN_RW		EC_IN_RW_OD

#endif /* _ACPI__ */

/* These define the GPE, not the GPIO. */
#define EC_SCI_GPI		GEVENT_3	/* AGPIO 22 -> GPE 3 */
#define EC_WAKE_GPI		GEVENT_15	/* AGPIO 24 -> GPE 15 */

/* EC sync irq */
#define EC_SYNC_IRQ		GPIO_31

#endif /* __BASEBOARD_GPIO_H__ */
