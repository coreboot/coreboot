/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2012 Rudolf Marek <r.marek@assembler.cz>
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

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <device/pnp_def.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <console/loglevel.h>
#include "agesawrapper.h"
#include "cpu/x86/bist.h"
#include "cpu/x86/lapic.h"
#include "southbridge/amd/agesa/hudson/hudson.h"
#include "southbridge/amd/agesa/hudson/smbus.h"
#include "superio/ite/it8712f/early_serial.c"
#include "cpu/amd/agesa/s3_resume.h"
#include "src/drivers/pc80/i8254.c"
#include "src/drivers/pc80/i8259.c"
#include "cbmem.h"

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx);
void disable_cache_as_ram(void);

#define MMIO_NON_POSTED_START 0xfed00000
#define MMIO_NON_POSTED_END   0xfedfffff
#define SB_MMIO 0xFED80000
#define SB_MMIO_MISC32(x) *(volatile u32 *)(SB_MMIO + 0xE00 + (x))

static void sbxxx_enable_48mhzout(void)
{
	/* most likely programming to 48MHz out signal */
	u32 reg32;
	reg32 = SB_MMIO_MISC32(0x28);
	reg32 &= 0xffc7ffff;
	reg32 |= 0x00100000;
	SB_MMIO_MISC32(0x28) = reg32;

	reg32 = SB_MMIO_MISC32(0x40);
	reg32 &= ~0x80u;
	SB_MMIO_MISC32(0x40) = reg32;
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;
	u8 byte;
	device_t dev;
#if CONFIG_HAVE_ACPI_RESUME
	void *resume_backup_memory;
#endif
	val = agesawrapper_amdinitmmio();

	if (!cpu_init_detectedx && boot_cpu()) {

		/* enable SIO decode */
		dev = PCI_DEV(0, 0x14, 3);
		byte = pci_read_config8(dev, 0x48);
		byte |= 3;		/* 2e, 2f */
		pci_write_config8(dev, 0x48, byte);

		post_code(0x30);

                /* enable SB MMIO space */
		outb(0x24, 0xcd6);
		outb(0x1, 0xcd7);

		/* enable SIO clock */
		sbxxx_enable_48mhzout();
		it8712f_kill_watchdog();
		it8712f_enable_serial(0, CONFIG_TTYS0_BASE);
		console_init();

		/* turn on secondary smbus at b20 */
		outb(0x28, 0xcd6);
		byte = inb(0xcd7);
		byte |= 1;
		outb(byte, 0xcd7);

		/* set DDR3 voltage */
		byte = CONFIG_BOARD_ASUS_F2A85_M_DDR3_VOLT_VAL;

		/* default is byte = 0x0, so no need to set it in this case */
		if (byte)
			do_smbus_write_byte(0xb20, 0x15, 0x3, byte);
	}

	/* Halt if there was a built in self test failure */
	post_code(0x34);
	report_bist_failure(bist);

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x \n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx \n", cpu_init_detectedx);

	post_code(0x37);
	printk(BIOS_DEBUG, "agesawrapper_amdinitreset ");
	val = agesawrapper_amdinitreset();
	if(val) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitreset failed: %x \n", val);
	}

	post_code(0x39);

	val = agesawrapper_amdinitearly ();
	if(val) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitearly failed: %x \n", val);
	}
	printk(BIOS_DEBUG, "Got past agesawrapper_amdinitearly\n");

#if CONFIG_HAVE_ACPI_RESUME
	if (!acpi_is_wakeup_early()) {		/* Check for S3 resume */
#endif
		post_code(0x40);
		val = agesawrapper_amdinitpost ();
		if(val) {
			printk(BIOS_DEBUG, "agesawrapper_amdinitpost failed: %x \n", val);
		}
		printk(BIOS_DEBUG, "Got past agesawrapper_amdinitpost\n");

		post_code(0x41);
		val = agesawrapper_amdinitenv ();
		if(val) {
			printk(BIOS_DEBUG, "agesawrapper_amdinitenv failed: %x \n", val);
		}
		printk(BIOS_DEBUG, "Got past agesawrapper_amdinitenv\n");
		disable_cache_as_ram();
#if CONFIG_HAVE_ACPI_RESUME
	} else {		/* S3 detect */
		printk(BIOS_INFO, "S3 detected\n");

		post_code(0x60);
		printk(BIOS_DEBUG, "agesawrapper_amdinitresume ");
		val = agesawrapper_amdinitresume();
		if (val)
			printk(BIOS_DEBUG, "error level: %x \n", val);
		else
			printk(BIOS_DEBUG, "passed.\n");

		printk(BIOS_DEBUG, "agesawrapper_amds3laterestore ");
		val = agesawrapper_amds3laterestore ();
		if (val)
			printk(BIOS_DEBUG, "error level: %x \n", val);
		else
			printk(BIOS_DEBUG, "passed.\n");

		post_code(0x61);
		printk(BIOS_DEBUG, "Find resume memory location\n");
		resume_backup_memory = (void *)backup_resume();

		post_code(0x62);
		printk(BIOS_DEBUG, "Move CAR stack.\n");
		move_stack_high_mem();
		printk(BIOS_DEBUG, "stack moved to: 0x%x\n", (u32) (resume_backup_memory + HIGH_MEMORY_SAVE));

		post_code(0x63);
		disable_cache_as_ram();
		printk(BIOS_DEBUG, "CAR disabled.\n");
		set_resume_cache();

		/*
		 * Copy the system memory that is in the ramstage area to the
		 * reserved area.
		 */
		if (resume_backup_memory)
			memcpy(resume_backup_memory, (void *)(CONFIG_RAMBASE), HIGH_MEMORY_SAVE);

		printk(BIOS_DEBUG, "System memory saved. OK to load ramstage.\n");
	}
#endif

	/* Initialize i8259 pic */
	post_code(0x41);
	setup_i8259 ();

	/* Initialize i8254 timers */
	post_code(0x42);
	setup_i8254 ();

	post_code(0x50);
	copy_and_run();

	post_code(0x54);  /* Should never see this post code. */
}
