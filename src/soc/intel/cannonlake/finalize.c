/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <commonlib/console/post_codes.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/cse.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/tco.h>
#include <intelblocks/thermal.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/systemagent.h>
#include <spi-generic.h>

#include "chip.h"

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
	config_t *config;

	tco_lockdown();

	/*
	 * Set low maximum temp threshold value used for dynamic thermal sensor
	 * shutdown consideration.
	 *
	 * If Dynamic Thermal Shutdown is enabled then PMC logic shuts down the
	 * thermal sensor when CPU is in a C-state and DTS Temp <= LTT.
	 */
	pch_thermal_configuration();

	config = config_of_soc();
	pmcbase = pmc_mmio_regs();

	if (config->s0ix_enable && config->cppmvric2_adsposcdis) {
		/* Enable Audio DSP OSC qualification for S0ix */
		reg32 = read32(pmcbase + CPPMVRIC2);
		reg32 &= ~ADSPOSCDIS;
		write32(pmcbase + CPPMVRIC2, reg32);
	}

	pch_handle_sideband(config);

	pmc_clear_pmcon_sts();

	/*
	 * Lock chipset memory registers to protect SMM.
	 * When SkipMpInit=0, this is done by FSP.
	 */
	if (!CONFIG(USE_INTEL_FSP_MP_INIT))
		cpu_lt_lock_memory();

}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize();
	apm_control(APM_CNT_FINALIZE);
	if (CONFIG(DISABLE_HECI1_AT_PRE_BOOT) &&
			CONFIG(SOC_INTEL_COMMON_BLOCK_HECI1_DISABLE_USING_PMC_IPC))
		heci1_disable();

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
