/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <commonlib/console/post_codes.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/tco.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <spi-generic.h>

#define CAMERA1_CLK		0x8000 /* Camera 1 Clock */
#define CAMERA2_CLK		0x8080 /* Camera 2 Clock */
#define CAM_CLK_EN		(1 << 1)
#define MIPI_CLK		(1 << 0)
#define HDPLL_CLK		(0 << 0)

static void pch_enable_isclk(void)
{
	pcr_or32(PID_ISCLK, CAMERA1_CLK, CAM_CLK_EN | MIPI_CLK);
	pcr_or32(PID_ISCLK, CAMERA2_CLK, CAM_CLK_EN | MIPI_CLK);
}

static void pch_handle_sideband(config_t *config)
{
	if (config->pch_isclk)
		pch_enable_isclk();
}

static void pch_finalize(void)
{
	uint32_t reg32;
	uint8_t *pmcbase;
	config_t *config = config_of_soc();

	/* TCO Lock down */
	tco_lockdown();

	/* TODO: Add Thermal Configuration */

	pmcbase = pmc_mmio_regs();
	if (config->s0ix_enable) {
		/*
		 * Enable USBSUSPGQDIS qualification to ensure USB2 PHY SUS is power gated
		 * before entering s0ix.
		 */
		reg32 = read32(pmcbase + CPPMVRIC3);
		reg32 &= ~USBSUSPGQDIS;
		write32(pmcbase + CPPMVRIC3, reg32);

		if (config->cnvi_reduce_s0ix_pwr_usage) {
			setbits32(pmcbase + CPPMVRIC2, CNVIVNNAONREQQDIS);
			setbits32(pmcbase + CORE_SPARE_GCR_0, BIT(0));
		}
	}

	pch_handle_sideband(config);

	pmc_clear_pmcon_sts();
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize();
	apm_control(APM_CNT_FINALIZE);

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
