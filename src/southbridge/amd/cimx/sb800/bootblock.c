/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <arch/bootblock.h>
#include <device/pci_ops.h>

static void enable_rom(void)
{
	u16 word;
	u32 dword;
	const pci_devfn_t dev = PCI_DEV(0, 0x14, 0x03);
	/* SB800 LPC Bridge 0:20:3:44h.
	 * BIT6: Port Enable for serial port 0x3f8-0x3ff
	 * BIT29: Port Enable for KBC port 0x60 and 0x64
	 * BIT30: Port Enable for ACPI Micro-Controller port 0x66 and 0x62
	 */
	dword = pci_s_read_config32(dev, 0x44);
	//dword |= (1<<6) | (1<<29) | (1<<30);
	/* Turn on all of LPC IO Port decode enable */
	dword = 0xffffffff;
	pci_s_write_config32(dev, 0x44, dword);

	/* SB800 LPC Bridge 0:20:3:48h.
	 * BIT0: Port Enable for SuperIO 0x2E-0x2F
	 * BIT1: Port Enable for SuperIO 0x4E-0x4F
	 * BIT4: Port Enable for LPC ROM Address Arrage2 (0x68-0x6C)
	 * BIT6: Port Enable for RTC IO 0x70-0x73
	 * BIT21: Port Enable for Port 0x80
	 */
	dword = pci_s_read_config32(dev, 0x48);
	dword |= (1 << 0) | (1 << 1) | (1 << 4) | (1 << 6) | (1 << 21);
	pci_s_write_config32(dev, 0x48, dword);

	/* Enable ROM access */
	word = pci_s_read_config16(dev, 0x6c);
	word = 0x10000 - (CONFIG_COREBOOT_ROMSIZE_KB >> 6);
	pci_s_write_config16(dev, 0x6c, word);
}

static void enable_prefetch(void)
{
	u32 dword;
	const pci_devfn_t dev = PCI_DEV(0, 0x14, 0x03);

	/* Enable PrefetchEnSPIFromHost */
	dword = pci_s_read_config32(dev, 0xb8);
	pci_s_write_config32(dev, 0xb8, dword | (1 << 24));
}

static void enable_spi_fast_mode(void)
{
	u32 dword;
	const pci_devfn_t dev = PCI_DEV(0, 0x14, 0x03);

	// set temp MMIO base
	volatile u32 *spi_base = (void *)0xa0000000;
	u32 save = pci_s_read_config32(dev, 0xa0);
	pci_s_write_config32(dev, 0xa0, (u32)spi_base | 2);

	// early enable of SPI 33 MHz fast mode read
	dword = spi_base[3];
	spi_base[3] = (dword & ~(3 << 14)) | (1 << 14);
	spi_base[0] = spi_base[0] | (1 << 18);	// fast read enable

	pci_s_write_config32(dev, 0xa0, save);
}

static void enable_clocks(void)
{
	u32 reg32;

	// Program SB800 MiscClkCntrl register to configure clock output on the
	// 14M_25M_48M_OSC ball usually used for the Super-I/O.
	// Almost all SIOs need 48 MHz, only the SMSC SCH311x wants 14 MHz,
	// which is the SB800's power up default.  We could switch back to 14
	// in the mainboard's romstage.c, but then the clock frequency would
	// change twice.
	reg32 = misc_read32(0x40);
	reg32 &= ~((1 << 2) | (3 << 0)); // enable, 14 MHz (power up default)
#if !CONFIG(SUPERIO_WANTS_14MHZ_CLOCK)
	reg32 |= 2 << 0; // Device_CLK1_sel = 48 MHz
#endif
	misc_write32(0x40, reg32);
}

void bootblock_early_southbridge_init(void)
{
	/* Setup the ROM access for 2M */
	enable_rom();
	enable_prefetch();
	enable_spi_fast_mode();

	// Program AcpiMmioEn to enable MMIO access to MiscCntrl register
	enable_acpimmio_decode_pm24();
	enable_clocks();
}
