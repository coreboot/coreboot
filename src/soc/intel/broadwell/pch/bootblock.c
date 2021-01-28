/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/rcba.h>
#include <soc/spi.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <southbridge/intel/common/early_spi.h>

static void map_rcba(void)
{
	pci_write_config32(PCH_DEV_LPC, RCBA, CONFIG_FIXED_RCBA_MMIO_BASE | 1);
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

static void pch_enable_bars(void)
{
	/* Set up southbridge BARs */
	pci_write_config32(PCH_DEV_LPC, RCBA, CONFIG_FIXED_RCBA_MMIO_BASE | 1);

	pci_write_config32(PCH_DEV_LPC, PMBASE, ACPI_BASE_ADDRESS | 1);

	pci_write_config8(PCH_DEV_LPC, ACPI_CNTL, ACPI_EN);

	pci_write_config32(PCH_DEV_LPC, GPIO_BASE, GPIO_BASE_ADDRESS | 1);

	/* Enable GPIO functionality. */
	pci_write_config8(PCH_DEV_LPC, GPIO_CNTL, GPIO_EN);
}

static void pch_early_lpc(void)
{
	pch_enable_bars();

	/* Set COM1/COM2 decode range */
	pci_write_config16(PCH_DEV_LPC, LPC_IO_DEC, 0x0010);

	/* Enable SuperIO + MC + COM1 + PS/2 Keyboard/Mouse */
	u16 lpc_config = CNF1_LPC_EN | CNF2_LPC_EN | GAMEL_LPC_EN |
		COMA_LPC_EN | KBC_LPC_EN | MC_LPC_EN;
	pci_write_config16(PCH_DEV_LPC, LPC_EN, lpc_config);

	/* Enable IOAPIC */
	RCBA16(OIC) = 0x0100;

	/* Read back for posted write */
	(void)RCBA16(OIC);

	/* Set HPET address and enable it */
	RCBA32_AND_OR(HPTC, ~3, 1 << 7);

	/*
	 * Reading the register back guarantees that the write is
	 * done before we use the configured base address below.
	 */
	(void)RCBA32(HPTC);

	/* Enable HPET to start counter */
	setbits32((void *)HPET_BASE_ADDRESS + 0x10, 1 << 0);

	/* Disable reset */
	RCBA32_OR(GCS, 1 << 5);

	/* TCO timer halt */
	u16 reg16 = inb(ACPI_BASE_ADDRESS + TCO1_CNT);
	reg16 |= TCO_TMR_HLT;
	outb(reg16, ACPI_BASE_ADDRESS + TCO1_CNT);

	/* Enable upper 128 bytes of CMOS */
	RCBA32_OR(RC, 1 << 2);

	/* Disable unused device (always) */
	RCBA32_OR(FD, PCH_DISABLE_ALWAYS);
}

void bootblock_early_southbridge_init(void)
{
	map_rcba();
	enable_spi_prefetching_and_caching();
	enable_port80_on_lpc();
	set_spi_speed();
	pch_early_lpc();
}
