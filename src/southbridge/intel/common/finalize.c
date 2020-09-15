/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/common/pmutil.h>
#include <southbridge/intel/common/rcba.h>
#include <spi-generic.h>

#include "finalize.h"

void intel_pch_finalize_smm(void)
{
	const pci_devfn_t lpc_dev = PCI_DEV(0, 0x1f, 0);

	/* Lock SPIBAR */
	RCBA32_OR(0x3804, (1 << 15));

	if (CONFIG(SPI_FLASH_SMM))
		/* Re-init SPI driver to handle locked BAR */
		spi_init();

	/* TCLOCKDN: TC Lockdown */
	RCBA32_OR(0x0050, (1UL << 31));

	/* BIOS Interface Lockdown */
	RCBA32_OR(0x3410, (1 << 0));

	/* Function Disable SUS Well Lockdown */
	RCBA_AND_OR(8, 0x3420, ~0U, (1 << 7));

	pci_or_config16(lpc_dev, D31F0_GEN_PMCON_1, SMI_LOCK);

	pci_or_config8(lpc_dev, D31F0_GEN_PMCON_LOCK,
		       ACPI_BASE_LOCK | SLP_STR_POL_LOCK);

	pci_update_config32(lpc_dev, D31F0_ETR3, ~ETR3_CF9GR, ETR3_CF9LOCK);

	if (CONFIG(SOUTHBRIDGE_INTEL_LYNXPOINT))
		/* PMSYNC */
		RCBA32_OR(0x33c4, (1UL << 31));

	/* R/WO registers */
	RCBA32(0x21a4) = RCBA32(0x21a4);
	pci_write_config32(PCI_DEV(0, 27, 0), 0x74,
		    pci_read_config32(PCI_DEV(0, 27, 0), 0x74));

	write_pmbase16(TCO1_CNT, read_pmbase16(TCO1_CNT) | TCO_LOCK);

	post_code(POST_OS_BOOT);
}
