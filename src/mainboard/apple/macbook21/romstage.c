/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* __PRE_RAM__ means: use "unsigned" for device, not a struct. */

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/intel/romstage.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <timestamp.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <halt.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

static void ich7_enable_lpc(void)
{
	/* Enable Serial IRQ */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), SERIRQ_CNTL, 0xd0);

	/* I/O Decode Ranges
	 * X60:       0x0210 == 00000010 00010000
	 * Macbook21: 0x0010 == 00000000 00010000
	 * Bit 9:8    LPT Decode Range. This field determines which range to
	 *            decode for the LPT Port.
	 *            00 = 378h - 37Fh and 778h - 77Fh
	 *            10 = 3BCh - 3BEh and 7BCh - 7BEh
	 */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0010);

	/* LPC_EN--LPC I/F Enables Register
	 * X60:       0x1f0d == 00011111 00001101
	 * Macbook21: 0x3807 == 00111000 00000111
	 * Bit 13     CNF2_LPC_EN -- R/W. Microcontroller Enable # 2.
	 *            0 = Disable.
	 *            1 = Enables the decoding of the I/O locations 4Eh and 4Fh
	 *            to the LPC interface. This range is used for a
	 *            microcontroller.
	 * Bit 12     CNF1_LPC_EN -- R/W. Super I/O Enable.
	 *            0 = Disable.
	 *            1 = Enables the decoding of the I/O locations 2Eh and 2Fh
	 *            to the LPC interface. This range is used for
	 *            Super I/O devices.
	 * Bit 11     MC_LPC_EN -- R/W. Microcontroller Enable # 1.
	 *            0 = Disable.
	 *            1 = Enables the decoding of the I/O locations 62h and 66h
	 *            to the LPC interface. This range is used for a
	 *            microcontroller.
	 * Bit 10     KBC_LPC_EN -- R/W. Keyboard Enable.
	 *            0 = Disable.
	 *            1 = Enables the decoding of the I/O locations 60h and 64h
	 *            to the LPC interface. This range is used for a
	 *            microcontroller.
	 * Bit 9      GAMEH_LPC_EN -- R/W. High Gameport Enable
	 *            0 = Disable.
	 *            1 = Enables the decoding of the I/O locations 208h to 20Fh
	 *            to the LPC interface. This range is used for a gameport.
	 * Bit 8      GAMEL_LPC_EN -- R/W. Low Gameport Enable
	 *            0 = Disable.
	 *            1 = Enables the decoding of the I/O locations 200h to 207h
	 *            to the LPC interface. This range is used for a gameport.
	 * Bit 3      FDD_LPC_EN -- R/W. Floppy Drive Enable
	 *            0 = Disable.
	 *            1 = Enables the decoding of the FDD range to the LPC
	 *            interface. This range is selected in the LPC_FDD/LPT
	 *            Decode Range Register (D31:F0:80h, bit 12).
	 * Bit 2      LPT_LPC_EN -- R/W. Parallel Port Enable
	 *            0 = Disable.
	 *            1 = Enables the decoding of the LPT range to the LPC
	 *            interface. This range is selected in the LPC_FDD/LPT
	 *            Decode Range Register (D31:F0:80h, bit 9:8).
	 * Bit 1      COMB_LPC_EN -- R/W. Com Port B Enable
	 *            0 = Disable.
	 *            1 = Enables the decoding of the COMB range to the LPC
	 *            interface. This range is selected in the LPC_COM Decode
	 *            Range Register (D31:F0:80h, bits 6:4).
	 * Bit 0      COMA_LPC_EN -- R/W. Com Port A Enable
	 *            0 = Disable.
	 *            1 = Enables the decoding of the COMA range to the LPC
	 *            interface. This range is selected in the LPC_COM Decode
	 *            Range Register (D31:F0:80h, bits 3:2).
	 */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, CNF2_LPC_EN
			| CNF1_LPC_EN | MC_LPC_EN | LPT_LPC_EN | COMB_LPC_EN
			| COMA_LPC_EN);

	/* GEN1_DEC, LPC Interface Generic Decode Range 1
	 * X60:       0x1601 0x007c == 00000000 01111100 00010110 00000001
	 * Macbook21: 0x0681 0x000c == 00000000 00001100 00000110 10000001
	 * Bit 31:24  Reserved.
	 * Bit 23:18  Generic I/O Decode Range Address[7:2] Mask: A `1' in any
	 *            bit position indicates that any value in the corresponding
	 *            address bit in a received cycle will be treated as a
	 *            match. The corresponding bit in the Address field, below,
	 *            is ignored. The mask is only provided for the lower 6 bits
	 *            of the DWord address, allowing for decoding blocks up to
	 *            256 bytes in size.
	 * Bit 17:16  Reserved.
	 * Bit 15:2   Generic I/O Decode Range 1 Base Address (GEN1_BASE). This
	 *            address is aligned on a 128-byte boundary, and must have
	 *            address lines 31:16 as 0. NOTE: The Intel ICH7 does not
	 *            provide decode down to the word or byte level.
	 * Bit 1      Reserved.
	 * Bit 0      Generic Decode Range 1 Enable (GEN1_EN) -- R/W.
	 *            0 = Disable.
	 *            1 = Enable the GEN1 I/O range to be forwarded to the LPC
	 *            I/F
	 */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN1_DEC, 0x000c0681);

	/* GEN2_DEC, LPC Interface Generic Decode Range 2
	 * X60:       0x15e1 0x000c == 00000000 00001100 00010101 11100001
	 * Macbook21: 0x1641 0x000c == 00000000 00001100 00010110 01000001
	 */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN2_DEC, 0x000c1641);

	/* GEN4_DEC, LPC Interface Generic Decode Range 4
	 * X60:       0x0000 0x0000
	 * Macbook21: 0x0301 0x001c == 00000000 00011100 00000011 00000001
	 */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN4_DEC, 0x001c0301);
}

static void rcba_config(void)
{
	/* V0CTL Virtual Channel 0 Resource Control */
	RCBA32(0x0014) = 0x80000001;
	/* V1CAP Virtual Channel 1 Resource Capability */
	RCBA32(0x001c) = 0x03128010;

	/* Device 1f interrupt pin register */
	RCBA32(0x3100) = 0x00042210;
	RCBA32(0x3108) = 0x10004321;

	/* PCIe Interrupts */
	RCBA32(D28IP) = 0x00214321;
	/* HD Audio Interrupt */
	RCBA32(D27IP) = 0x00000001;

	/* dev irq route register */
	RCBA16(D31IR) = 0x0232;
	RCBA16(D30IR) = 0x3246;
	RCBA16(D29IR) = 0x0235;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x3216;

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;

	/* Disable unused devices */
	RCBA32(FD) = FD_PCIE6 | FD_PCIE5 | FD_PCIE4 | FD_PCIE3 | FD_INTLAN
		| FD_ACMOD | FD_ACAUD;
	RCBA32(FD) |= (1 << 0);	/* Required. */

	/* Set up I/O Trap #0 for 0xfe00 (SMIC) */

	/* Set up I/O Trap #3 for 0x800-0x80c (Trap) */
	RCBA32(0x1e9c) = 0x000200f0;
	RCBA32(0x1e98) = 0x000c0801;
}

static void early_ich7_init(void)
{
	uint8_t reg8;
	uint32_t reg32;

	/* program secondary mlt XXX byte? */
	pci_write_config8(PCI_DEV(0, 0x1e, 0), 0x1b, 0x20);

	/* reset rtc power status */
	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa4);
	reg8 &= ~(1 << 2);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa4, reg8);

	/* usb transient disconnect */
	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xad);
	reg8 |= (3 << 0);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xad, reg8);

	reg32 = pci_read_config32(PCI_DEV(0, 0x1d, 7), 0xfc);
	reg32 |= (1 << 29) | (1 << 17);
	pci_write_config32(PCI_DEV(0, 0x1d, 7), 0xfc, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x1d, 7), 0xdc);
	reg32 |= (1 << 31) | (1 << 27);
	pci_write_config32(PCI_DEV(0, 0x1d, 7), 0xdc, reg32);

	RCBA32(0x0088) = 0x0011d000;
	RCBA16(0x01fc) = 0x060f;
	RCBA32(0x01f4) = 0x86000040;
	RCBA32(0x0214) = 0x10030549;
	RCBA32(0x0218) = 0x00020504;
	RCBA8(0x0220) = 0xc5;
	reg32 = RCBA32(GCS);
	reg32 |= (1 << 6);
	RCBA32(GCS) = reg32;
	reg32 = RCBA32(0x3430);
	reg32 &= ~(3 << 0);
	reg32 |= (1 << 0);
	RCBA32(0x3430) = reg32;
	RCBA32(FD) |= (1 << 0);
	RCBA16(0x0200) = 0x2008;
	RCBA8(0x2027) = 0x0d;
	RCBA16(0x3e08) |= (1 << 7);
	RCBA16(0x3e48) |= (1 << 7);
	RCBA32(0x3e0e) |= (1 << 7);
	RCBA32(0x3e4e) |= (1 << 7);

	/* next step only on ich7m b0 and later: */
	reg32 = RCBA32(0x2034);
	reg32 &= ~(0x0f << 16);
	reg32 |= (5 << 16);
	RCBA32(0x2034) = reg32;
}

void mainboard_romstage_entry(unsigned long bist)
{
	int s3resume = 0;
	const u8 spd_addrmap[2 * DIMM_SOCKETS] = { 0x50, 0x51, 0x52, 0x53 };

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	ich7_enable_lpc();

	/* Set up the console */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG,
		       "Soft reset detected, rebooting properly.\n");
		outb(0x6, 0xcf9);
		halt();
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i945_early_initialization();

	s3resume = southbridge_detect_s3_resume();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
	dump_spd_registers();
#endif

	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(s3resume ? 2 : 0, spd_addrmap);
	timestamp_add_now(TS_AFTER_INITRAM);

	/* Perform some initialization that must run before stage2 */
	early_ich7_init();

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization(s3resume);
}
