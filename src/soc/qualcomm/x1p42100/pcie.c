/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/qcom_qmp_phy.h>
#include <soc/pcie.h>

/* Enable PIPE clock */
int qcom_dw_pcie_enable_pipe_clock(void)
{
	/* Set pipe clock source */
	if (clock_configure_mux(PCIE_6A_PIPE_MUXR, PCIE_6A_PIPE_SRC_SEL)) {
		printk(BIOS_ERR, " %s(): Pipe clock enable failed\n", __func__);
		return -1;
	}

	return 0;
}

/* Enable controller specific clocks */
int32_t qcom_dw_pcie_enable_clock(void)
{
	int32_t ret, clk, gdsc;

	/* Enable gdsc before enable pcie clocks */
	for (gdsc = PCIE_6A_GDSC; gdsc < MAX_GDSC; gdsc++) {
		ret = clock_enable_gdsc(gdsc);
		if (ret) {
			printk(BIOS_ERR, "Failed to enable gdsc\n");
			return ret;
		}
	}

	/* Configure gcc_pcie*_phy_rchng_clk to 100mhz */
	clock_configure_pcie();

	/* Enable pcie and PHY clocks */
	for (clk = ANOC_PCIE_AT_CBCR; clk < PCIE_CLK_COUNT; clk++) {
		if (clk == PCIE_6A_PIPE_MUXR) {
			printk(BIOS_DEBUG, "Skipping pipe\n");
			continue;
		}
		ret = clock_enable_pcie(clk);
		if (ret) {
			printk(BIOS_ERR, "Failed to enable %d clock\n", clk);
			return ret;
		}
	}
	write32(TCSR_GCC_PCIE_4L_CLKREF_EN_PCIE_ENABLE, 0x1);
	return ret;
}

/* Turn on NVMe */
void gcom_pcie_power_on_ep(void)
{
	/* placeholder */
}

void gcom_pcie_get_config(struct qcom_pcie_cntlr_t *host_cfg)
{
	/* placeholder */
}
