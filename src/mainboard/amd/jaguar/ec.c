/* SPDX-License-Identifier: GPL-2.0-only */

#include "ec.h"

#include <console/console.h>
#include <ec/acpi/ec.h>
#include <stdint.h>

/* Controls power and reset lines connected to EC */
static void configure_ec_gpio(void)
{
	uint8_t tmp;
	uint8_t olddata = ec_read(EC_PAGE_SELECT);

	/* select page c2 */
	ec_write(EC_PAGE_SELECT, EC_GPIO_PAGE);

	/* SLOT-0 Force power */
	if (CONFIG(ENABLE_EVAL_CARD) &&
	    CONFIG(ENABLE_FORCE_POWER_GPP0) &&
	    CONFIG(PCIE_SLOT0_2X4))  {
		tmp = EC_FORCE_PWR_SLOT0;
	} else {
		tmp = 0;
	}

	ec_write(EC_FORCE_PWR, tmp);
	printk(BIOS_SPEW, "EC: 0x%02x = %02x\n", EC_FORCE_PWR, tmp);

	/* Power on WLAN */
	tmp = 0;
	if (CONFIG(ENABLE_NVME_WLAN_2LANES))
		tmp |= EC_WLAN_POWER_PWR_EN | EC_WLAN_POWER_PERST_N | EC_WLAN_POWER_SDIO_RST_N;
	ec_write(EC_WLAN_POWER, tmp);
	printk(BIOS_SPEW, "EC: 0x%02x = %02x\n", EC_WLAN_POWER, tmp);

	/* Configure PCIe mux */
	if (CONFIG(ENABLE_NVME_PCIE_2LANES)) {
		tmp = EC_PCIE_MUX_M2_SLOT_2X2X;
	} else if (CONFIG(ENABLE_PCIE_4LANES)) {
		tmp = EC_PCIE_MUX_SLOT1X4;
	} else if (CONFIG(ENABLE_NVME_WLAN_2LANES)) {
		tmp = EC_PCIE_MUX_M2_WLAN_2X2X;
	} else {
		tmp = EC_PCIE_MUX_NVMEX4;
	}
	ec_write(EC_PCIE_MUX, tmp);
	printk(BIOS_SPEW, "EC: 0x%02x = %02x\n", EC_PCIE_MUX, tmp);

	tmp = 0;
	if (CONFIG(XGBE_LED_TURN_ON))
		tmp |= EC_XGBE_LED_ENABLE;

	if (CONFIG(XGBE_EN))
		tmp |= EC_XGBE_MDIO0_1_XGBE | EC_XGBE_SFPP_MUX_ENABLE;

	ec_write(EC_XGBE_CTRL, tmp);
	printk(BIOS_SPEW, "EC: 0x%02x = %02x\n", EC_XGBE_CTRL, tmp);

	/* Enable M.2 SSD0 power */
	if (CONFIG(ENABLE_NVME_4LANES) ||
	    CONFIG(ENABLE_NVME_PCIE_2LANES) ||
	    CONFIG(ENABLE_NVME_WLAN_2LANES))
		tmp = EC_M2_POWER_PWR_EN | EC_M2_POWER_PERST_N;
	else
		tmp = 0;
	ec_write(EC_M2_POWER, tmp);
	printk(BIOS_SPEW, "EC: 0x%02x = %02x\n", EC_M2_POWER, tmp);

	/* restore page */
	ec_write(EC_PAGE_SELECT, olddata);
}

void jaguar_ec_init(void)
{
	ec_set_ports(JAGUAR_EC_CMD, JAGUAR_EC_DATA);
	configure_ec_gpio();
}
