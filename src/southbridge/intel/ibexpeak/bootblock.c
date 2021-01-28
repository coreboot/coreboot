/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include <southbridge/intel/common/early_spi.h>
#include "pch.h"
#include "chip.h"

static void enable_port80_on_lpc(void)
{
	RCBA32(GCS) &= ~4;
}

static void set_spi_speed(void)
{
	u32 fdod;
	u8 ssfc;

	/* Observe SPI Descriptor Component Section 0 */
	RCBA32(0x38b0) = 0x1000;

	/* Extract the Write/Erase SPI Frequency from descriptor */
	fdod = RCBA32(0x38b4);
	fdod >>= 24;
	fdod &= 7;

	/* Set Software Sequence frequency to match */
	ssfc = RCBA8(0x3893);
	ssfc &= ~7;
	ssfc |= fdod;
	RCBA8(0x3893) = ssfc;
}

static void early_lpc_init(void)
{
	const struct device *dev = pcidev_on_root(0x1f, 0);
	const struct southbridge_intel_ibexpeak_config *config = NULL;

	/*
	 * Enable some common LPC IO ranges:
	 * - 0x2e/0x2f, 0x4e/0x4f often SuperIO
	 * - 0x60/0x64, 0x62/0x66 often KBC/EC
	 * - 0x3f0-0x3f5/0x3f7 FDD
	 * - 0x378-0x37f and 0x778-0x77f LPT
	 * - 0x2f8-0x2ff COMB
	 * - 0x3f8-0x3ff COMA
	 * - 0x208-0x20f GAMEH
	 * - 0x200-0x207 GAMEL
	 */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF2_LPC_EN | CNF1_LPC_EN
			   | MC_LPC_EN | KBC_LPC_EN | GAMEH_LPC_EN
			   | GAMEL_LPC_EN | FDD_LPC_EN | LPT_LPC_EN
			   | COMB_LPC_EN | COMA_LPC_EN);
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);

	/* Clear PWR_FLR */
	pci_write_config8(PCH_LPC_DEV, GEN_PMCON_3,
			  (pci_read_config8(PCH_LPC_DEV, GEN_PMCON_3) & ~2) | 1);

	pci_write_config32(PCH_LPC_DEV, ETR3,
			   pci_read_config32(PCH_LPC_DEV, ETR3) & ~ETR3_CF9GR);

	/* Set up generic decode ranges */
	if (!dev)
		return;
	if (dev->chip_info)
		config = dev->chip_info;
	if (!config)
		return;

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, config->gen4_dec);
}

void bootblock_early_southbridge_init(void)
{
	enable_spi_prefetching_and_caching();

	/* Enable RCBA */
	pci_devfn_t lpc_dev = PCI_DEV(0, 0x1f, 0);
	pci_write_config32(lpc_dev, RCBA, CONFIG_FIXED_RCBA_MMIO_BASE | 1);

	enable_port80_on_lpc();
	set_spi_speed();

	/* Enable upper 128bytes of CMOS */
	RCBA32(RC) = (1 << 2);

	early_lpc_init();
}
