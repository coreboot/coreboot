/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>

static void enable_rom(void)
{
	u16 word;
	u32 dword;
	device_t dev;

	dev = PCI_DEV(0, 0x14, 0x03);
	/* SB800 LPC Bridge 0:20:3:44h.
	 * BIT6: Port Enable for serial port 0x3f8-0x3ff
	 * BIT29: Port Enable for KBC port 0x60 and 0x64
	 * BIT30: Port Enable for ACPI Micro-Controller port 0x66 and 0x62
	 */
	dword = pci_io_read_config32(dev, 0x44);
	//dword |= (1<<6) | (1<<29) | (1<<30) ;
	/* Turn on all of LPC IO Port decode enable */
	dword = 0xffffffff;
	pci_io_write_config32(dev, 0x44, dword);

	/* SB800 LPC Bridge 0:20:3:48h.
	 * BIT0: Port Enable for SuperIO 0x2E-0x2F
	 * BIT1: Port Enable for SuperIO 0x4E-0x4F
	 * BIT4: Port Enable for LPC ROM Address Arrage2 (0x68-0x6C)
	 * BIT6: Port Enable for RTC IO 0x70-0x73
	 * BIT21: Port Enable for Port 0x80
	 */
	dword = pci_io_read_config32(dev, 0x48);
	dword |= (1 << 0) | (1 << 1) | (1 << 4) | (1 << 6) | (1 << 21);
	pci_io_write_config32(dev, 0x48, dword);

	/* Enable rom access */
	word = pci_io_read_config16(dev, 0x6c);
	word = 0x10000 - (CONFIG_COREBOOT_ROMSIZE_KB >> 6);
	pci_io_write_config16(dev, 0x6c, word);
}

static void enable_prefetch(void)
{
	u32 dword;
	device_t dev = PCI_DEV(0, 0x14, 0x03);

	/* Enable PrefetchEnSPIFromHost */
	dword = pci_io_read_config32(dev, 0xb8);
	pci_io_write_config32(dev, 0xb8, dword | (1 << 24));
}

static void enable_spi_fast_mode(void)
{
	u8 byte;
	u32 dword;
	device_t dev = PCI_DEV(0, 0x14, 0x03);

	// set temp MMIO base
	volatile u32 *spi_base = (void *)0xa0000000;
	u32 save = pci_io_read_config32(dev, 0xa0);
	pci_io_write_config32(dev, 0xa0, (u32) spi_base | 2);

	// early enable of SPI 33 MHz fast mode read
	byte = spi_base[3];
	spi_base[3] = (byte & ~(3 << 14)) | (1 << 14);
	spi_base[0] = spi_base[0] | (1 << 18);	// fast read enable

	pci_io_write_config32(dev, 0xa0, save);
}

static void enable_clocks(void)
{
	u8 reg8;
	u32 reg32;
	volatile u32 *acpi_mmio = (void *) (0xFED80000 + 0xE00 + 0x40);

	// Program AcpiMmioEn to enable MMIO access to MiscCntrl register
	outb(0x24, 0xCD6);
	reg8 = inb(0xCD7);
	reg8 |= 1;
	reg8 &= ~(1 << 1);
	outb(reg8, 0xCD7);

	// Program SB800 MiscClkCntrl register to configure clock output on the
	// 14M_25M_48M_OSC ball usually used for the Super-I/O.
	// Almost all SIOs need 48 MHz, only the SMSC SCH311x wants 14 MHz,
	// which is the SB800's power up default.  We could switch back to 14
	// in the mainboard's romstage.c, but then the clock frequency would
	// change twice.
	reg32 = *acpi_mmio;
	reg32 &= ~((1 << 2) | (3 << 0)); // enable, 14 MHz (power up default)
#if !CONFIG_SUPERIO_WANTS_14MHZ_CLOCK
	reg32 |= 2 << 0; // Device_CLK1_sel = 48 MHz
#endif
	*acpi_mmio = reg32;
}

static void bootblock_southbridge_init(void)
{
	/* Setup the rom access for 2M */
	enable_rom();
	enable_prefetch();
	enable_spi_fast_mode();
	enable_clocks();
}
