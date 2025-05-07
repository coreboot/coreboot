/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <commonlib/console/post_codes.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <spi-generic.h>
#include <soc/pci_devs.h>
#include <soc/lpc.h>
#include <soc/pch.h>
#include <soc/rcba.h>
#include <soc/spi.h>
#include <southbridge/intel/common/spi.h>

static void broadwell_pch_finalize(void)
{
	spi_finalize_ops();

	/* Lock SPIBAR */
	if (!CONFIG(EM100PRO_SPI_CONSOLE))
		RCBA32_OR(SPIBAR_HSFS, SPIBAR_HSFS_FLOCKDN);

	/* TC Lockdown */
	RCBA32_OR(0x0050, 1 << 31);

	/* BIOS Interface Lockdown */
	RCBA32_OR(GCS, 1 << 0);

	/* Function Disable SUS Well Lockdown */
	RCBA8(FDSW) |= 1 << 7;

	/* Global SMI Lock */
	pci_or_config16(PCH_DEV_LPC, GEN_PMCON_1, SMI_LOCK);

	/* GEN_PMCON Lock */
	pci_or_config8(PCH_DEV_LPC, GEN_PMCON_LOCK, SLP_STR_POL_LOCK | ACPI_BASE_LOCK);

	/* PMSYNC */
	RCBA32_OR(PMSYNC_CONFIG, 1 << 31);

	/* Lock */
	RCBA32_OR(0x3a6c, 0x00000001);

	/* Read+Write this R/WO register */
	RCBA32(LCAP) = RCBA32(LCAP);
}

static void broadwell_finalize(void *unused)
{
	broadwell_pch_finalize();
	apm_control(APM_CNT_FINALIZE);

	/* Indicate finalize step with post code */
	post_code(POSTCODE_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, broadwell_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, broadwell_finalize, NULL);
