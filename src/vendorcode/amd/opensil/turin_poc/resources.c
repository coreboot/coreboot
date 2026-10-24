/* SPDX-License-Identifier: GPL-2.0-only */

#include <opensil_config.h>
#include <DF/DfClass-api.h>
#include <ProjSocConst.h>
#include <RcMgr/DfX/RcManager4-api.h>
#include <console/console.h>
#include <halt.h>
#include <cpu/cpu.h>
#include <xSIM-api.h>
#include <xPrfFabric.h>

#include "../opensil.h"

void opensil_setup_resources(void)
{
	DFCLASS_INPUT_BLK *df = SilFindStructure(SilId_DfClass, 0);
	DFX_RCMGR_INPUT_BLK *rc = SilFindStructure(SilId_RcManager, 0);

	if (!df || !rc)
		die("openSIL: Data Fabric or resource-manager block not found\n");

	/* Fabric queries use this input for PCI access, including socket discovery. */
	df->AmdPciExpressBaseAddress = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
	uint32_t sockets = 0;
	if (xPrfFabricGetSystemInfo(&sockets, NULL, NULL, NULL, NULL) != SilPass ||
	    !sockets || sockets > CONFIG_MAX_SOCKET) {
		printk(BIOS_WARNING, "openSIL: invalid socket count %u; assuming one\n", sockets);
		sockets = 1;
	}
	df->AmdNumberOfPhysicalSocket = sockets;

	/* Let openSIL distribute resources among the detected sockets' PCI roots. */
	rc->SetRcBasedOnNv = false;
	rc->SocketNumber = sockets;
	rc->RbsPerSocket = PROJ_MAX_RBS_PER_SOCKET;
	rc->McptEnable = true;
	rc->PciExpressBaseAddress = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
	rc->BottomMmioReservedForPrimaryRb = 4ull * GiB - 32 * MiB;
	rc->MmioSizePerRbForNonPciDevice = 16 * MiB;
	/* Keep MMIO in the identity-mapped range supported by four-level paging. */
	rc->MmioAbove4GLimit = POWER_OF_2(MIN(48, cpu_phys_address_size()));
	rc->Above4GMmioSizePerRbForNonPciDevice = 0;
}
