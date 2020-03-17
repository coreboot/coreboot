/*
 * This file is part of the coreboot project.
 *
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

#include <device/device.h>
#include <soc/pci_devs.h>
#include <soc/reg_access.h>
#include <soc/romstage.h>

/* Minimum time in microseconds for assertion of PERST# signal */
#define PCIEXP_PERST_MIN_ASSERT_US		100

/* Microsecond delay post issuing common lane reset */
#define PCIEXP_DELAY_US_POST_CMNRESET_RESET	1

/* Microsecond delay to wait for PLL to lock */
#define PCIEXP_DELAY_US_WAIT_PLL_LOCK		80

/* Microsecond delay post issuing sideband interface reset */
#define PCIEXP_DELAY_US_POST_SBI_RESET		20

/* Microsecond delay post deasserting PERST# */
#define PCIEXP_DELAY_US_POST_PERST_DEASSERT	10

const struct reg_script pcie_init_script[] = {
	/* Assert PCIe reset# */
	MAINBOARD_PCIE_RESET(0),

	/* PHY Common lane reset */
	REG_SOC_UNIT_OR(QUARK_SCSS_SOC_UNIT_SOCCLKEN_CONFIG,
		SOCCLKEN_CONFIG_PHY_I_CMNRESET_L),

	/* Wait post common lane reset */
	TIME_DELAY_USEC(PCIEXP_DELAY_US_POST_CMNRESET_RESET),

	/* PHY Sideband interface reset.
	 * Controller main reset
	 */
	REG_SOC_UNIT_OR(QUARK_SCSS_SOC_UNIT_SOCCLKEN_CONFIG,
		SOCCLKEN_CONFIG_SBI_RST_100_CORE_B
		| SOCCLKEN_CONFIG_PHY_I_SIDE_RST_L),
	TIME_DELAY_USEC(PCIEXP_DELAY_US_WAIT_PLL_LOCK),

	/* Controller sideband interface reset */
	REG_SOC_UNIT_OR(QUARK_SCSS_SOC_UNIT_SOCCLKEN_CONFIG,
		SOCCLKEN_CONFIG_SBI_BB_RST_B),

	/* Wait post sideband interface reset */
	TIME_DELAY_USEC(PCIEXP_DELAY_US_POST_SBI_RESET),

	/* Deassert PCIe reset# */
	MAINBOARD_PCIE_RESET(1),

	/* Wait post de assert PERST#. */
	TIME_DELAY_USEC(PCIEXP_DELAY_US_POST_PERST_DEASSERT),

	/* Controller primary interface reset */
	REG_SOC_UNIT_OR(QUARK_SCSS_SOC_UNIT_SOCCLKEN_CONFIG,
		SOCCLKEN_CONFIG_BB_RST_B),

	/* Set the mixer load resistance */
	REG_PCIE_AFE_AND(QUARK_PCIE_AFE_PCIE_RXPICTRL0_L0,
		OCFGPIMIXLOAD_1_0_MASK),
	REG_PCIE_AFE_AND(QUARK_PCIE_AFE_PCIE_RXPICTRL0_L1,
		OCFGPIMIXLOAD_1_0_MASK),
	REG_SCRIPT_END
};

static const struct reg_script pcie_bus_init_script[] = {
	/* Setup Message Bus Idle Counter (SBIC) values */
	REG_PCI_RMW8(R_QNC_PCIE_IOSFSBCTL, ~B_QNC_PCIE_IOSFSBCTL_SBIC_MASK,
		V_PCIE_ROOT_PORT_SBIC_VALUE),
	REG_PCI_READ8(R_QNC_PCIE_IOSFSBCTL),

	/* Set the IPF bit in MCR2 */
	REG_PCI_OR32(R_QNC_PCIE_MPC2, B_QNC_PCIE_MPC2_IPF),
	REG_PCI_READ32(R_QNC_PCIE_MPC2),

	/* Set up the Posted and Non Posted Request sizes for PCIe */
	REG_PCI_RMW32(R_QNC_PCIE_CCFG, ~B_QNC_PCIE_CCFG_UPSD,
		(B_QNC_PCIE_CCFG_UNRS | B_QNC_PCIE_CCFG_UPRS)),
	REG_PCI_READ32(R_QNC_PCIE_CCFG),
	REG_SCRIPT_END
};

void pcie_init(void)
{
	/* Initialize the PCIe bridges */
	reg_script_run(pcie_init_script);
	reg_script_run_on_dev(PCIE_PORT0_BDF, pcie_bus_init_script);
	reg_script_run_on_dev(PCIE_PORT1_BDF, pcie_bus_init_script);
}
