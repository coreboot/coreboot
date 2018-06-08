/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <pc80/mc146818rtc.h>
#include <cbmem.h>
#include <console/console.h>
#include <console/usb.h>
#include <halt.h>
#include <program_loading.h>
#include <reset.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <northbridge/intel/fsp_sandybridge/northbridge.h>
#include <northbridge/intel/fsp_sandybridge/raminit.h>
#include <superio/winbond/wpcd376i/wpcd376i.h>
#include <superio/intel/i8900/i8900.h>
#include "gpio.h"
#include "me.h"
#include "pch.h"
#include "romstage.h"

static inline void reset_system(void)
{
	hard_reset();
	halt();
}

static void pch_enable_lpc(void)
{
	pci_devfn_t dev = PCH_LPC_DEV;

	/* Set COM1/COM2 decode range */
	pci_write_config16(dev, LPC_IO_DEC, 0x0010);

	/* Enable Board specific settings */
	uint16_t lpc_config = get_lpc_setting();
	pci_write_config16(dev, LPC_EN, lpc_config);
}

static void rcba_config(void)
{
	uint32_t reg32;
	uint32_t fd_mask = 0, fd2_mask = 0;

	/* Enable IOAPIC (generic) */
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void) RCBA16(OIC);

	/* Board specific settings */
	get_func_disables(&fd_mask, &fd2_mask);
	reg32 = RCBA32(FD);
	printk(BIOS_DEBUG, "FD original value: 0x%08lx\n",(unsigned long)reg32);

	reg32 |= fd_mask;

	printk(BIOS_DEBUG, "FD updated value: 0x%08lx\n",(unsigned long)reg32);
	RCBA32(FD) = reg32;

	reg32 = RCBA32(FD2);
	printk(BIOS_DEBUG, "FD2 original value: 0x%08lx\n",(unsigned long)reg32);

	reg32 |= fd2_mask;

	printk(BIOS_DEBUG, "FD2 updated value: 0x%08lx\n",(unsigned long)reg32);
	RCBA32(FD2) = reg32;
}

void main(FSP_INFO_HEADER *fsp_info_header)
{
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	int32_t boot_mode = 0;
#endif
	uint32_t pm1_cnt;
	uint16_t pm1_sts;

	post_code(0x40);

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE | 1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);

	/* Call into mainboard. */
	early_mainboard_romstage_entry();

	console_init();

	post_code(0x41);

	post_code(0x42);
	sandybridge_sb_early_initialization();

	post_code(0x43);
	sandybridge_early_initialization(SANDYBRIDGE_MOBILE);
	printk(BIOS_DEBUG, "Back from sandybridge_early_initialization()\n");

	post_code(0x44);
	/* Wait for ME to be ready */
	intel_early_me_status();
	intel_early_me_init();
#if IS_ENABLED(CONFIG_GFXUMA)
	intel_early_me_uma_size();
#endif
	post_code(0x45);
	/* Check PM1_STS[15] to see if we are waking from Sx */
	pm1_sts = inw(DEFAULT_PMBASE + PM1_STS);

	/* Read PM1_CNT[12:10] to determine which Sx state */
	pm1_cnt = inl(DEFAULT_PMBASE + PM1_CNT);
	post_code(0x46);
	if ((pm1_sts & WAK_STS) && ((pm1_cnt >> 10) & 7) == 5) {
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
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

	post_code(0x48);

	timestamp_add_now(TS_BEFORE_INITRAM);

  /*
   * Call early init to initialize memory and chipset. This function returns
   * to the romstage_main_continue function with a pointer to the HOB
   * structure.
   */
	printk(BIOS_DEBUG, "Starting the Intel FSP (early_init)\n");
	fsp_early_init(fsp_info_header);
	die("Uh Oh! fsp_early_init should not return here.\n");
}

/*******************************************************************************
 * The FSP early_init function returns to this function.
 * Memory is setup and the stack is set by the FSP.
 ******************************************************************************/
void romstage_main_continue(EFI_STATUS status, VOID *HobListPtr) {
	int32_t cbmem_was_initted;
	void *cbmem_hob_ptr;

	timestamp_add_now(TS_AFTER_INITRAM);

	post_code(0x49);

	/* For reference print FSP version */
	uint32_t version = MCHBAR32(0x5034);
	printk(BIOS_DEBUG, "FSP Version %d.%d.%d Build %d\n",
			version >> 24 , (version >> 16) & 0xff,
			(version >> 8) & 0xff, version & 0xff);
	printk(BIOS_DEBUG, "FSP Status: 0x%0x\n", (uint32_t)status);

	intel_early_me_init_done(ME_INIT_STATUS_SUCCESS);

	report_memory_config();

	post_code(0x4b);

	late_mainboard_romstage_entry();

	early_pch_init();
	post_code(0x4c);

	rcba_config();
	post_code(0x4d);

	quick_ram_check();
	post_code(0x4e);

	cbmem_was_initted = !cbmem_recovery(0);

	if (cbmem_was_initted) {
		reset_system();
	}

	/* Save the HOB pointer in CBMEM to be used in ramstage. */
	cbmem_hob_ptr = cbmem_add(CBMEM_ID_HOB_POINTER, sizeof(HobListPtr));
	if (cbmem_hob_ptr == NULL)
		die("Could not allocate cbmem for HOB pointer");
	*(uint32_t*)cbmem_hob_ptr = (uint32_t)HobListPtr;
	post_code(0x4f);

	run_ramstage();
}

uint64_t get_initial_timestamp(void)
{
	return (uint64_t) pci_read_config32(PCI_DEV(0, 0x1f, 2), 0xd0) << 4;
}
