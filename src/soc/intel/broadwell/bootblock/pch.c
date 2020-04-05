/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/rcba.h>
#include <soc/spi.h>
#include <reg_script.h>
#include <soc/pm.h>
#include <soc/romstage.h>

/*
 * Enable Prefetching and Caching.
 */
static void enable_spi_prefetch(void)
{
	u8 reg8 = pci_read_config8(PCH_DEV_LPC, 0xdc);
	reg8 &= ~(3 << 2);
	reg8 |= (2 << 2); /* Prefetching and Caching Enabled */
	pci_write_config8(PCH_DEV_LPC, 0xdc, reg8);
}


static void map_rcba(void)
{
	pci_write_config32(PCH_DEV_LPC, RCBA, RCBA_BASE_ADDRESS | 1);
}

static void enable_port80_on_lpc(void)
{
	/* Enable port 80 POST on LPC. The chipset does this by default,
	 * but it doesn't appear to hurt anything. */
	u32 gcs = RCBA32(GCS);
	gcs = gcs & ~0x4;
	RCBA32(GCS) = gcs;
}

static void set_spi_speed(void)
{
	u32 fdod;
	u8 ssfc;

	/* Observe SPI Descriptor Component Section 0 */
	SPIBAR32(SPIBAR_FDOC) = 0x1000;

	/* Extract the Write/Erase SPI Frequency from descriptor */
	fdod = SPIBAR32(SPIBAR_FDOD);
	fdod >>= 24;
	fdod &= 7;

	/* Set Software Sequence frequency to match */
	ssfc = SPIBAR8(SPIBAR_SSFC + 2);
	ssfc &= ~7;
	ssfc |= fdod;
	SPIBAR8(SPIBAR_SSFC + 2) = ssfc;
}

const struct reg_script pch_early_init_script[] = {
	/* Setup southbridge BARs */
	REG_PCI_WRITE32(RCBA, RCBA_BASE_ADDRESS | 1),
	REG_PCI_WRITE32(PMBASE, ACPI_BASE_ADDRESS | 1),
	REG_PCI_WRITE8(ACPI_CNTL, ACPI_EN),
	REG_PCI_WRITE32(GPIO_BASE, GPIO_BASE_ADDRESS | 1),
	REG_PCI_WRITE8(GPIO_CNTL, GPIO_EN),

	/* Set COM1/COM2 decode range */
	REG_PCI_WRITE16(LPC_IO_DEC, 0x0010),
	/* Enable legacy decode ranges */
	REG_PCI_WRITE16(LPC_EN, CNF1_LPC_EN | CNF2_LPC_EN | GAMEL_LPC_EN |
			COMA_LPC_EN | KBC_LPC_EN | MC_LPC_EN),

	/* Enable IOAPIC */
	REG_MMIO_WRITE16(RCBA_BASE_ADDRESS + OIC, 0x0100),
	/* Read back for posted write */
	REG_MMIO_READ16(RCBA_BASE_ADDRESS + OIC),

	/* Set HPET address and enable it */
	REG_MMIO_RMW32(RCBA_BASE_ADDRESS + HPTC, ~3, (1 << 7)),
	/* Read back for posted write */
	REG_MMIO_READ32(RCBA_BASE_ADDRESS + HPTC),
	/* Enable HPET to start counter */
	REG_MMIO_OR32(HPET_BASE_ADDRESS + 0x10, (1 << 0)),

	/* Disable reset */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + GCS, (1 << 5)),
	/* TCO timer halt */
	REG_IO_OR16(ACPI_BASE_ADDRESS + TCO1_CNT, TCO_TMR_HLT),

	/* Enable upper 128 bytes of CMOS */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + RC, (1 << 2)),

	/* Disable unused device (always) */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + FD, PCH_DISABLE_ALWAYS),

	REG_SCRIPT_END
};

static void pch_early_lpc(void)
{
	reg_script_run_on_dev(PCH_DEV_LPC, pch_early_init_script);
}

void bootblock_early_southbridge_init(void)
{
	map_rcba();
	enable_spi_prefetch();
	enable_port80_on_lpc();
	set_spi_speed();
	pch_early_lpc();
}
