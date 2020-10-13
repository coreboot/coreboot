/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <console/post_codes.h>
#include <device/pci_ops.h>
#include <reg_script.h>
#include <spi-generic.h>
#include <soc/pci_devs.h>
#include <soc/lpc.h>
#include <soc/me.h>
#include <soc/rcba.h>
#include <soc/spi.h>
#include <soc/systemagent.h>
#include <southbridge/intel/common/spi.h>

/*
 * 16.6 System Agent Configuration Locking
 * "5th Generation Intel Core Processor Family BIOS Specification"
 * Document Number 535094
 * Revision 2.2.0, August 2014
 *
 * To ease reading, first lock PCI registers, then MCHBAR registers.
 * Write the MC Lock register first, since more than one bit gets set.
 */
static void broadwell_systemagent_finalize(void)
{
	struct device *const host_bridge = pcidev_path_on_root(SA_DEVFN_ROOT);

	pci_or_config16(host_bridge, GGC,         1 << 0);
	pci_or_config32(host_bridge, DPR,         1 << 0);
	pci_or_config32(host_bridge, MESEG_LIMIT, 1 << 10);
	pci_or_config32(host_bridge, REMAPBASE,   1 << 0);
	pci_or_config32(host_bridge, REMAPLIMIT,  1 << 0);
	pci_or_config32(host_bridge, TOM,         1 << 0);
	pci_or_config32(host_bridge, TOUUD,       1 << 0);
	pci_or_config32(host_bridge, BDSM,        1 << 0);
	pci_or_config32(host_bridge, BGSM,        1 << 0);
	pci_or_config32(host_bridge, TSEG,        1 << 0);
	pci_or_config32(host_bridge, TOLUD,       1 << 0);

	MCHBAR32(0x50fc) |= 0x8f;	/* MC */
	MCHBAR32(0x5500) |= 1 << 0;	/* PAVP */
	MCHBAR32(0x5880) |= 1 << 5;	/* DDR PTM */
	MCHBAR32(0x7000) |= 1 << 31;
	MCHBAR32(0x77fc) |= 1 << 0;
	MCHBAR32(0x7ffc) |= 1 << 0;
	MCHBAR32(0x6800) |= 1 << 31;
	MCHBAR32(0x6020) |= 1 << 0;	/* UMA GFX */
	MCHBAR32(0x63fc) |= 1 << 0;	/* VTDTRK */

	/* Read+write the following */
	MCHBAR32(0x6030) = MCHBAR32(0x6030);
	MCHBAR32(0x6034) = MCHBAR32(0x6034);
	MCHBAR32(0x6008) = MCHBAR32(0x6008);
}

const struct reg_script pch_finalize_script[] = {
#if !CONFIG(EM100PRO_SPI_CONSOLE)
	/* Lock SPIBAR */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + SPIBAR_OFFSET + SPIBAR_HSFS,
		      SPIBAR_HSFS_FLOCKDN),
#endif

	/* TC Lockdown */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x0050, (1 << 31)),

	/* BIOS Interface Lockdown */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + GCS, (1 << 0)),

	/* Function Disable SUS Well Lockdown */
	REG_MMIO_OR8(RCBA_BASE_ADDRESS + FDSW, (1 << 7)),

	/* Global SMI Lock */
	REG_PCI_OR16(GEN_PMCON_1, SMI_LOCK),

	/* GEN_PMCON Lock */
	REG_PCI_OR8(GEN_PMCON_LOCK, SLP_STR_POL_LOCK | ACPI_BASE_LOCK),

	/* PMSYNC */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + PMSYNC_CONFIG, (1 << 31)),

	REG_SCRIPT_END
};

static void broadwell_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	broadwell_systemagent_finalize();

	spi_finalize_ops();
	reg_script_run_on_dev(PCH_DEV_LPC, pch_finalize_script);

	/* Lock */
	RCBA32_OR(0x3a6c, 0x00000001);

	/* Read+Write the following register */
	RCBA32(0x21a4) = RCBA32(0x21a4);

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, broadwell_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, broadwell_finalize, NULL);
