/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#ifndef __ACPI__
#include <soc/gpio.h>
#include <platform_descriptors.h>

#define H1_PCH_INT		GPIO_3
#define PEN_DETECT_ODL		GPIO_4
#define PEN_POWER_EN		GPIO_5
#define TOUCHPAD_INT_ODL	GPIO_9
#define EC_FCH_WAKE_L		GPIO_24
#define WIFI_PCIE_RESET_L	GPIO_26
#define PCIE_RST1_L		GPIO_27
#define EN_PWR_WIFI		GPIO_29
#define NVME_AUX_RESET_L	GPIO_40
#define WIFI_AUX_RESET_L	GPIO_42
#define EN_PWR_CAMERA		GPIO_76
#define EN_PWR_TOUCHPAD_PS2	GPIO_67
#define PCIE_0_WIFI_CLKREQ_ODL	GPIO_92
#define PCIE_1_SD_CLKREQ_ODL	GPIO_115
#define BIOS_FLASH_WP_ODL	GPIO_137
#define SD_AUX_RESET_L		GPIO_142
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
#define FPMCU_INT_L		GPIO_6
#define FPMCU_RST_ODL		GPIO_11
#define EC_IN_RW_OD		GPIO_130
#define PCIE_4_NVME_CLKREQ_ODL	GPIO_132
#define NVME_CLKREQ		CLK_REQ4
#else
#define EC_IN_RW_OD		GPIO_11
#define PCIE_2_NVME_CLKREQ_ODL	GPIO_116
#define NVME_CLKREQ		CLK_REQ2
#endif

/* SPI Write protect */
#define CROS_WP_GPIO		BIOS_FLASH_WP_ODL
#define GPIO_EC_IN_RW		EC_IN_RW_OD

/* PCIe reset pins */
#define PCIE_0_RST		WIFI_AUX_RESET_L
#define PCIE_1_RST		SD_AUX_RESET_L
#define PCIE_2_RST		0
#define PCIE_3_RST		0
#define PCIE_4_RST		NVME_AUX_RESET_L

#endif /* _ACPI__ */

/* These define the GPE, not the GPIO. */
#define EC_SCI_GPI		3	/* eSPI system event -> GPE 3 */
#define EC_WAKE_GPI		15	/* AGPIO 24 -> GPE 15 */

/* EC sync irq */
#define EC_SYNC_IRQ		31

#endif /* __BASEBOARD_GPIO_H__ */
