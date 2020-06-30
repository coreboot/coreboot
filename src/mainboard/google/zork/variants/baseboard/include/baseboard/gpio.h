/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#ifndef __ACPI__
#include <soc/gpio.h>
#include <platform_descriptors.h>

#define WLAN_CLKREQ		CLK_REQ0
#define SD_CLKREQ		CLK_REQ1

#if CONFIG(BOARD_GOOGLE_BASEBOARD_DALBOZ)
#define NVME_START_LANE 4
#define NVME_END_LANE 5
#define WLAN_START_LANE 0
#define WLAN_END_LANE 0
#define SD_START_LANE 1
#define SD_END_LANE 1
#else
#define NVME_START_LANE 0
#define NVME_END_LANE 1
#define WLAN_START_LANE 4
#define WLAN_END_LANE 4
#define SD_START_LANE 5
#define SD_END_LANE 5
#endif

#if CONFIG(BOARD_GOOGLE_BASEBOARD_TREMBYLE)
#define EC_IN_RW_OD		GPIO_130
#define NVME_CLKREQ		CLK_REQ4
#else
#define EC_IN_RW_OD		GPIO_11
#define NVME_CLKREQ		CLK_REQ2
#endif

/* SPI Write protect */
#define CROS_WP_GPIO		GPIO_137
#define GPIO_EC_IN_RW		EC_IN_RW_OD

#endif /* _ACPI__ */

/* These define the GPE, not the GPIO. */
#define EC_SCI_GPI		3	/* eSPI system event -> GPE 3 */
#define EC_WAKE_GPI		15	/* AGPIO 24 -> GPE 15 */

/* EC sync irq */
#define EC_SYNC_IRQ		31

#endif /* __BASEBOARD_GPIO_H__ */
