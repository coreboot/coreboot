/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <reg_script.h>
#include <spi-generic.h>
#include <soc/pci_devs.h>
#include <soc/lpc.h>
#include <soc/pch.h>
#include <soc/rcba.h>
#include <soc/spi.h>
#include <southbridge/intel/common/spi.h>

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

void broadwell_pch_finalize(void)
{
	spi_finalize_ops();
	reg_script_run_on_dev(PCH_DEV_LPC, pch_finalize_script);

	/* Lock */
	RCBA32_OR(0x3a6c, 0x00000001);

	/* Read+Write this R/WO register */
	RCBA32(LCAP) = RCBA32(LCAP);
}
