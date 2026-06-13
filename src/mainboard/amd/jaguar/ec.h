/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef JAGUAR_EC_H
#define JAGUAR_EC_H

#ifndef __ACPI__
void jaguar_ec_init(void);
#endif

#define JAGUAR_EC_CMD	0x666
#define JAGUAR_EC_DATA	0x662

#define EC_PAGE_SELECT			0x31
#define EC_MODERN_STANDBY		0xB7
#define  EC_MODERN_STANDBY_SSD0_D3_EN	BIT(4)
#define  EC_MODERN_STANDBY_SSD1_D3_EN	BIT(5)
#define  EC_MODERN_STANDBY_S0ix		BIT(7)

#define EC_GPIO_PAGE			0xc2

/* Registers [0h-30h] on page 0xc2 */
#define EC_FORCE_PWR				0x11
#define  EC_FORCE_PWR_SLOT0			BIT(0)
#define  EC_FORCE_PWR_SLOT1			BIT(1)
#define  EC_FORCE_PWR_SLOT2			BIT(2)

#define EC_PCIE_MUX				0x13
#define  EC_PCIE_MUX_NVMEX4			0
#define  EC_PCIE_MUX_SLOT1X4			1
#define  EC_PCIE_MUX_M2_SLOT_2X2X		2
#define  EC_PCIE_MUX_M2_WLAN_2X2X		3
#define  EC_PCIE_MUX_M2_WLAN_SLOT_2X1X1X	4

#define EC_M2_POWER				0x15
#define  EC_M2_POWER_PWR_EN			BIT(0)
#define  EC_M2_POWER_PERST_N			BIT(4)

#define EC_WLAN_POWER				0x20
#define EC_WLAN_POWER_PWR_EN			BIT(0)
#define EC_WLAN_POWER_PERST_N			BIT(1)
#define EC_WLAN_POWER_SDIO_RST_N		BIT(2)

#define EC_XGBE_CTRL				0x22
#define  EC_XGBE_MDIO0_1_XGBE			BIT(0)
#define  EC_XGBE_MDIO2_3_XGBE			BIT(1)
#define  EC_XGBE_LED_ENABLE			BIT(2)
#define  EC_XGBE_SFPP_MUX_ENABLE		BIT(3)

#endif /* JAGUAR_EC_H */
