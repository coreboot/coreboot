/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/* __PRE_RAM__ means: use "unsigned" for device, not a struct.  */

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <ec/acpi/ec.h>
#include <delay.h>
#include <timestamp.h>
#include <cbmem.h>

#include "gpio.h"
#include "dock.h"
#include "arch/early_variables.h"
#include "southbridge/intel/ibexpeak/pch.h"
#include "northbridge/intel/nehalem/nehalem.h"

#include "northbridge/intel/nehalem/raminit.h"
#include "southbridge/intel/ibexpeak/me.h"

static void pch_enable_lpc(void)
{
	/* X201 EC Decode Range Port60/64, Port62/66 */
	/* Enable EC, PS/2 Keyboard/Mouse */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN |
			   COMA_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x7c1601);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, 0xc15e1);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN3_DEC, 0x1c1681);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, (0x68 & ~3) | 0x00040001);

	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);

	pci_write_config32(PCH_LPC_DEV, 0xd0, 0x0);
	pci_write_config32(PCH_LPC_DEV, 0xdc, 0x8);

	pci_write_config8(PCH_LPC_DEV, GEN_PMCON_3,
			  (pci_read_config8(PCH_LPC_DEV, GEN_PMCON_3) & ~2) | 1);

	pci_write_config32(PCH_LPC_DEV, ETR3,
			   pci_read_config32(PCH_LPC_DEV, ETR3) & ~ETR3_CF9GR);
}

static void rcba_config(void)
{
	static const u32 rcba_dump3[] = {
		/* 30fc */ 0x00000000,
		/* 3100 */ 0x04341200, 0x00000000, 0x40043214, 0x00014321,
		/* 3110 */ 0x00000002, 0x30003214, 0x00000001, 0x00000002,
		/* 3120 */ 0x00000000, 0x00002321, 0x00000000, 0x00000000,
		/* 3130 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3140 */ 0x00003107, 0x76543210, 0x00000010, 0x00007654,
		/* 3150 */ 0x00000004, 0x00000000, 0x00000000, 0x00003210,
		/* 3160 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3170 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3180 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3190 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 31a0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 31b0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 31c0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 31d0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 31e0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 31f0 */ 0x00000000, 0x00000000, 0x00000000, 0x03000000,
		/* 3200 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3210 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3220 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3230 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3240 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3250 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3260 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3270 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3280 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3290 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 32a0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 32b0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 32c0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 32d0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 32e0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 32f0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3300 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3310 */ 0x02060100, 0x0000000f, 0x01020000, 0x80000000,
		/* 3320 */ 0x00000000, 0x04000000, 0x00000000, 0x00000000,
		/* 3330 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3340 */ 0x000fffff, 0x00000000, 0x00000000, 0x00000000,
		/* 3350 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3360 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3370 */ 0x00000000, 0x00000000, 0x7f8fdfff, 0x00000000,
		/* 3380 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3390 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 33a0 */ 0x00003900, 0x00000000, 0x00000000, 0x00000000,
		/* 33b0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 33c0 */ 0x00010000, 0x00000000, 0x00000000, 0x0001004b,
		/* 33d0 */ 0x06000008, 0x00010000, 0x00000000, 0x00000000,
		/* 33e0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 33f0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3400 */ 0x0000001c, 0x00000080, 0x00000000, 0x00000000,
		/* 3410 */ 0x00000c61, 0x00000000, 0x16e61fe1, 0xbf4f001f,
		/* 3420 */ 0x00000000, 0x00060010, 0x0000001d, 0x00000000,
		/* 3430 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3440 */ 0xdeaddeed, 0x00000000, 0x00000000, 0x00000000,
		/* 3450 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3460 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3470 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3480 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3490 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 34a0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 34b0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 34c0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 34d0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 34e0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 34f0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3500 */ 0x20000557, 0x2000055f, 0x2000074b, 0x2000074b,
		/* 3510 */ 0x20000557, 0x2000014b, 0x2000074b, 0x2000074b,
		/* 3520 */ 0x2000074b, 0x2000074b, 0x2000055f, 0x2000055f,
		/* 3530 */ 0x20000557, 0x2000055f, 0x00000000, 0x00000000,
		/* 3540 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3550 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3560 */ 0x00000001, 0x000026a3, 0x00040002, 0x01000052,
		/* 3570 */ 0x02000772, 0x16000f8f, 0x1800ff4f, 0x0001d630,
		/* 3580 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3590 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 35a0 */ 0xfc000201, 0x3c000201, 0x00000000, 0x00000000,
		/* 35b0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 35c0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 35d0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 35e0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 35f0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3600 */ 0x0a001f00, 0x00000000, 0x00000000, 0x00000001,
		/* 3610 */ 0x00010000, 0x00000000, 0x00000000, 0x00000000,
		/* 3600 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3610 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3620 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3630 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3640 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3650 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3660 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3670 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3680 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3690 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 36a0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 36b0 */ 0x00000000, 0x089c0018, 0x00000000, 0x00000000,
		/* 36c0 */ 0x11111111, 0x00000000, 0x00000000, 0x00000000,
		/* 36d0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 36e0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 36f0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		/* 3710 */ 0x00000000, 0x4e564d49, 0x00000000, 0x00000000,
	};
	unsigned i;
	for (i = 0; i < sizeof(rcba_dump3) / 4; i++) {
		RCBA32(4 * i + 0x30fc) = rcba_dump3[i];
		(void)RCBA32(4 * i + 0x30fc);
	}
}

static inline void write_acpi32(u32 addr, u32 val)
{
	outl(val, DEFAULT_PMBASE | addr);
}

static inline void write_acpi16(u32 addr, u16 val)
{
	outw(val, DEFAULT_PMBASE | addr);
}

static inline u32 read_acpi32(u32 addr)
{
	return inl(DEFAULT_PMBASE | addr);
}

static inline u16 read_acpi16(u32 addr)
{
	return inw(DEFAULT_PMBASE | addr);
}

void main(unsigned long bist)
{
	u32 reg32;
	int s3resume = 0;

	timestamp_init(rdtsc ());

	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	nehalem_early_initialization(NEHALEM_MOBILE);

	pch_enable_lpc();

	/* Enable USB Power. We need to do it early for usbdebug to work. */
	ec_set_bit(0x3b, 4);

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE | 1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);

	setup_pch_gpios(&x201_gpio_map);


	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* Read PM1_CNT */
	reg32 = inl(DEFAULT_PMBASE + 0x04);
	printk(BIOS_DEBUG, "PM1_CNT: %08x\n", reg32);
	if (((reg32 >> 10) & 7) == 5) {
		u8 reg8;
		reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa2);
		printk(BIOS_DEBUG, "a2: %02x\n", reg8);
		if (!(reg8 & 0x20)) {
			outl(reg32 & ~(7 << 10), DEFAULT_PMBASE + 0x04);
			printk(BIOS_DEBUG, "Bad resume from S3 detected.\n");
		} else {
#if CONFIG_HAVE_ACPI_RESUME
			printk(BIOS_DEBUG, "Resume from S3 detected.\n");
			s3resume = 1;
#else
			printk(BIOS_DEBUG,
			       "Resume from S3 detected, but disabled.\n");
#endif
		}
	}

	/* Enable SMBUS. */
	enable_smbus();

	outb((inb(DEFAULT_GPIOBASE | 0x3a) & ~0x2) | 0x20,
	     DEFAULT_GPIOBASE | 0x3a);
	outb(0x50, 0x15ec);
	outb(inb(0x15ee) & 0x70, 0x15ee);

	write_acpi16(0x2, 0x0);
	write_acpi32(0x28, 0x0);
	write_acpi32(0x2c, 0x0);
	if (!s3resume) {
		read_acpi32(0x4);
		read_acpi32(0x20);
		read_acpi32(0x34);
		write_acpi16(0x0, 0x900);
		write_acpi32(0x20, 0xffff7ffe);
		write_acpi32(0x34, 0x56974);
		pci_write_config8(PCH_LPC_DEV, GEN_PMCON_3,
				  pci_read_config8(PCH_LPC_DEV, GEN_PMCON_3) | 2);
	}

	early_thermal_init();

	timestamp_add_now(TS_BEFORE_INITRAM);

	raminit(s3resume);

	timestamp_add_now(TS_AFTER_INITRAM);

	intel_early_me_status();

	if (s3resume) {
		/* Clear SLP_TYPE. This will break stage2 but
		 * we care for that when we get there.
		 */
		reg32 = inl(DEFAULT_PMBASE + 0x04);
		outl(reg32 & ~(7 << 10), DEFAULT_PMBASE + 0x04);
	}

#if CONFIG_HAVE_ACPI_RESUME
	/* If there is no high memory area, we didn't boot before, so
	 * this is not a resume. In that case we just create the cbmem toc.
	 */
	if (s3resume) {
		void *resume_backup_memory;

		/* For non-S3-resume, CBMEM is inited in raminit code.  */
		if (cbmem_recovery(1)) {
			printk(BIOS_ERR, "Failed S3 resume.\n");
			ram_check(0x100000, 0x200000);

			/* Failed S3 resume, reset to come up cleanly */
			outb(0xe, 0xcf9);
			hlt();
		}

		resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);

		/* copy 1MB - 64K to high tables ram_base to prevent memory corruption
		 * through stage 2. We could keep stuff like stack and heap in high tables
		 * memory completely, but that's a wonderful clean up task for another
		 * day.
		 */
		if (resume_backup_memory)
			memcpy(resume_backup_memory, (void *)CONFIG_RAMBASE,
			       HIGH_MEMORY_SAVE);

		/* Magic for S3 resume */
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafed00d);
	} else {
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafebabe);
		quick_ram_check();
	}
#endif

	timestamp_add_now(TS_END_ROMSTAGE);
}
