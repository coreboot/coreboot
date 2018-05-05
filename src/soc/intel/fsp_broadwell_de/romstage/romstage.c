/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2017 Siemens AG
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

#include <stddef.h>
#include <arch/cpu.h>
#include <lib.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <timestamp.h>
#include <version.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <pc80/mc146818rtc.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/gpio.h>
#include <build.h>

static void init_rtc(void)
{
	u16 gen_pmcon3 = pci_read_config16(PCI_DEV(0, LPC_DEV, LPC_FUNC), GEN_PMCON_3);

	if (gen_pmcon3 & RTC_PWR_STS) {
		printk(BIOS_DEBUG, "RTC Failure detected.  Resetting Date to %s\n",
			coreboot_dmi_date);
	}
	cmos_init(gen_pmcon3 & RTC_PWR_STS);
}

/* Set up IO address range and enable it for the GPIO block. */
static void setup_gpio_io_address(void)
{
	pci_write_config32(PCI_DEV(0, LPC_DEV, LPC_FUNC), GPIO_BASE_ADR_OFFSET,
					GPIO_BASE_ADDRESS);
	pci_write_config8(PCI_DEV(0, LPC_DEV, LPC_FUNC), GPIO_CTRL_OFFSET,
					GPIO_DECODE_ENABLE);
}


/* Entry from cache-as-ram.inc. */
void *asmlinkage main(FSP_INFO_HEADER *fsp_info_header)
{
	post_code(0x40);
	if (!IS_ENABLED(CONFIG_INTEGRATED_UART)) {
	/* Enable decoding of I/O locations for Super I/O devices */
		pci_write_config16(PCI_DEV(0x0, LPC_DEV, LPC_FUNC),
					   LPC_IO_DEC, 0x0010);
		pci_write_config16(PCI_DEV(0x0, LPC_DEV, LPC_FUNC),
					   LPC_EN, 0x340f);
	}
	console_init();
	init_rtc();
	setup_gpio_io_address();

	/* Call into mainboard. */
	post_code(0x41);
	early_mainboard_romstage_entry();

	/*
	 * Call early init to initialize memory and chipset. This function returns
	 * to the romstage_main_continue function with a pointer to the HOB
	 * structure.
	 */
	post_code(0x48);
	printk(BIOS_DEBUG, "Starting the Intel FSP (early_init)\n");
	fsp_early_init(fsp_info_header);
	die("Uh Oh! fsp_early_init should not return here.\n");
}

/*******************************************************************************
 * The FSP early_init function returns to this function.
 * Memory is set up and the stack is set by the FSP.
 */
void romstage_main_continue(EFI_STATUS status, void *hob_list_ptr)
{
	int cbmem_was_initted;
	void *cbmem_hob_ptr;

	post_code(0x4a);
	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_AFTER_INITRAM);
	printk(BIOS_DEBUG, "%s status: %x  hob_list_ptr: %x\n",
		__func__, (u32) status, (u32) hob_list_ptr);

#if IS_ENABLED(CONFIG_USBDEBUG_IN_ROMSTAGE)
	/* FSP reconfigures USB, so reinit it to have debug */
	usbdebug_init();
#endif	/* IS_ENABLED(CONFIG_USBDEBUG_IN_ROMSTAGE) */

	printk(BIOS_DEBUG, "FSP Status: 0x%0x\n", (u32)status);

	post_code(0x4b);
	late_mainboard_romstage_entry();

	post_code(0x4c);
	quick_ram_check();

	post_code(0x4d);
	cbmem_was_initted = !cbmem_recovery(0);

	/* Save the HOB pointer in CBMEM to be used in ramstage*/
	cbmem_hob_ptr = cbmem_add(CBMEM_ID_HOB_POINTER, sizeof(*hob_list_ptr));
	if (cbmem_hob_ptr == NULL)
		die("Could not allocate cbmem for HOB pointer");
	*(u32 *)cbmem_hob_ptr = (u32)hob_list_ptr;

	/* Load the ramstage. */
	post_code(0x4e);
	run_ramstage();
	while (1);
}

uint64_t get_initial_timestamp(void)
{
	return 0;
}
