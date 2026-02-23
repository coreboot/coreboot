/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <commonlib/console/post_codes.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>

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

	mchbar_setbits32(0x50fc, 0x8f);		/* MC */
	mchbar_setbits32(0x5500, 1 << 0);	/* PAVP */
	mchbar_setbits32(0x5880, 1 << 5);	/* DDR PTM */
	mchbar_setbits32(0x7000, 1 << 31);
	mchbar_setbits32(0x77fc, 1 << 0);
	mchbar_setbits32(0x7ffc, 1 << 0);
	mchbar_setbits32(0x6800, 1 << 31);
	mchbar_setbits32(0x6020, 1 << 0);		/* UMA GFX */
	mchbar_setbits32(0x63fc, 1 << 0);		/* VTDTRK */

	/* Read+write the following */
	mchbar_setbits32(0x6030, 0);
	mchbar_setbits32(0x6034, 0);
	mchbar_setbits32(0x6008, 0);
}

static void broadwell_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	broadwell_systemagent_finalize();

	broadwell_pch_finalize();

	/* Indicate finalize step with post code */
	post_code(POSTCODE_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, broadwell_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, broadwell_finalize, NULL);
