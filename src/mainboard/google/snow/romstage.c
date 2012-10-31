/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#if 0
#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/io.h>
//#include <arch/romcc_io.h>
#if 0
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#endif
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#if 0
#include "northbridge/intel/sandybridge/sandybridge.h"
#include "northbridge/intel/sandybridge/raminit.h"
#include "southbridge/intel/bd82x6x/pch.h"
#include "southbridge/intel/bd82x6x/gpio.h"
#endif
#include <arch/cpu.h>
#if 0
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#endif
#include "option_table.h"
#include "gpio.h"
#if CONFIG_CONSOLE_SERIAL8250
#if 0
#include "superio/smsc/lpc47n207/lpc47n207.h"
#include "superio/smsc/lpc47n207/early_serial.c"
#endif
#endif
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#endif
#endif
#include <cache.h>

static void mmu_setup(void)
{
	dram_bank_mmu_setup(CONFIG_SYS_SDRAM_BASE, CONFIG_DRAM_SIZE_MB * 1024);
}

void main(unsigned long bist)
{
	mmu_setup();
}

#if 0
/* FIXME: This is based off lumpy's romstage.c main() and is gutted to
   resemble snow's eventually... */
void main(unsigned long bist)
{
	int boot_mode = 0;
	int cbmem_was_initted;
//	u32 pm1_cnt;
//	u16 pm1_sts;

#if CONFIG_COLLECT_TIMESTAMPS
#if 0
	tsc_t start_romstage_time;
	tsc_t before_dram_time;
	tsc_t after_dram_time;
	tsc_t base_time = {
		.lo = pci_read_config32(PCI_DEV(0, 0x00, 0), 0xdc),
		.hi = pci_read_config32(PCI_DEV(0, 0x1f, 2), 0xd0)
	};
#endif
#endif

#if 0
	typedef const uint8_t spd_blob[256];
	struct cbfs_file *spd_file;
	spd_blob *spd_data;
#endif


#if CONFIG_COLLECT_TIMESTAMPS
//	start_romstage_time = rdtsc();
#endif

#if 0
	if (bist == 0)
		enable_lapic();

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
	setup_pch_gpios(&lumpy_gpio_map);

	console_init();
#endif

#if CONFIG_CHROMEOS
	save_chromeos_gpios();
#endif

#if 0
	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);
#endif

#if 0
	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	sandybridge_early_initialization(SANDYBRIDGE_MOBILE);
	printk(BIOS_DEBUG, "Back from sandybridge_early_initialization()\n");

	/* Check PM1_STS[15] to see if we are waking from Sx */
	pm1_sts = inw(DEFAULT_PMBASE + PM1_STS);

	/* Read PM1_CNT[12:10] to determine which Sx state */
	pm1_cnt = inl(DEFAULT_PMBASE + PM1_CNT);
#endif
#if 0

	if ((pm1_sts & WAK_STS) && ((pm1_cnt >> 10) & 7) == 5) {
#if CONFIG_HAVE_ACPI_RESUME
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
		boot_mode = 2;
		/* Clear SLP_TYPE. This will break stage2 but
		 * we care for that when we get there.
		 */
		outl(pm1_cnt & ~(7 << 10), DEFAULT_PMBASE + PM1_CNT);
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
#endif
	}
#endif

//	post_code(0x38);
	/* Enable SPD ROMs and DDR-III DRAM */
//	enable_smbus();

#if 0
	/* Prepare USB controller early in S3 resume */
	if (boot_mode == 2)
		enable_usb_bar();

	u32 gp_lvl2 = inl(DEFAULT_GPIOBASE + 0x38);
	u8 gpio33, gpio41, gpio49;
	gpio33 = (gp_lvl2 >> (33-32)) & 1;
	gpio41 = (gp_lvl2 >> (41-32)) & 1;
	gpio49 = (gp_lvl2 >> (49-32)) & 1;
	printk(BIOS_DEBUG, "Memory Straps:\n");
	printk(BIOS_DEBUG, " - memory capacity %dGB\n",
		gpio33 ? 2 : 1);
	printk(BIOS_DEBUG, " - die revision %d\n",
		gpio41 ? 2 : 1);
	printk(BIOS_DEBUG, " - vendor %s\n",
		gpio49 ? "Samsung" : "Other");

	int spd_index = 0;

	switch ((gpio49 << 2) | (gpio41 << 1) | gpio33) {
	case 0: // Other 1G Rev 1
		spd_index = 0;
		break;
	case 2: // Other 1G Rev 2
		spd_index = 1;
		break;
	case 1: // Other 2G Rev 1
	case 3: // Other 2G Rev 2
		spd_index = 2;
		break;
	case 4: // Samsung 1G Rev 1
		spd_index = 3;
		break;
	case 6: // Samsung 1G Rev 2
		spd_index = 4;
		break;
	case 5: // Samsung 2G Rev 1
	case 7: // Samsung 2G Rev 2
		spd_index = 5;
		break;
	}

	spd_file = cbfs_find("spd.bin");
	if (!spd_file)
		die("SPD data not found.");
	if (spd_file->len < (spd_index + 1) * 256)
		die("Missing SPD data.");
	spd_data = (spd_blob *)CBFS_SUBHEADER(spd_file);
	// leave onboard dimm address at f0, and copy spd data there.
	memcpy(pei_data.spd_data[0], spd_data[spd_index], 256);

	post_code(0x39);
	pei_data.boot_mode = boot_mode;
#endif
#if CONFIG_COLLECT_TIMESTAMPS
	before_dram_time = rdtsc();
#endif
	sdram_initialize(&pei_data);

#if CONFIG_COLLECT_TIMESTAMPS
	after_dram_time = rdtsc();
#endif
#if 0
	post_code(0x3a);
	/* Perform some initialization that must run before stage2 */
	early_pch_init();
	post_code(0x3b);

	rcba_config();
	post_code(0x3c);

	quick_ram_check();
	post_code(0x3e);

	MCHBAR16(SSKPD) = 0xCAFE;
#endif
#if CONFIG_EARLY_CBMEM_INIT
	cbmem_was_initted = !cbmem_initialize();
#else
	cbmem_was_initted = cbmem_reinit((uint64_t) (get_top_of_ram()
						     - HIGH_MEMORY_SIZE));
#endif

#if 0
#if CONFIG_HAVE_ACPI_RESUME
	/* If there is no high memory area, we didn't boot before, so
	 * this is not a resume. In that case we just create the cbmem toc.
	 */

	*(u32 *)CBMEM_BOOT_MODE = 0;
	*(u32 *)CBMEM_RESUME_BACKUP = 0;

	if ((boot_mode == 2) && cbmem_was_initted) {
		void *resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
		if (resume_backup_memory) {
			*(u32 *)CBMEM_BOOT_MODE = boot_mode;
			*(u32 *)CBMEM_RESUME_BACKUP = (u32)resume_backup_memory;
		}
		/* Magic for S3 resume */
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafed00d);
	} else if (boot_mode == 2) {
		/* Failed S3 resume, reset to come up cleanly */
		outb(0x6, 0xcf9);
		hlt();
	} else {
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafebabe);
	}
#endif
#endif
//	post_code(0x3f);
#if CONFIG_CHROMEOS
	init_chromeos(boot_mode);
#endif
#if 0
#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_init(base_time);
	timestamp_add(TS_START_ROMSTAGE, start_romstage_time );
	timestamp_add(TS_BEFORE_INITRAM, before_dram_time );
	timestamp_add(TS_AFTER_INITRAM, after_dram_time );
	timestamp_add_now(TS_END_ROMSTAGE);
#endif
#endif
#if CONFIG_CONSOLE_CBMEM
	/* Keep this the last thing this function does. */
	cbmemc_reinit();
#endif
}
#endif
