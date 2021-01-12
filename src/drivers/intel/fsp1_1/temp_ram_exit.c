/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbmem.h>
#include <cbfs.h>
#include <console/console.h>
#include <fsp/util.h>

asmlinkage void chipset_teardown_car_main(void)
{
	FSP_INFO_HEADER *fih;
	uint32_t status;
	FSP_TEMP_RAM_EXIT temp_ram_exit;

	/* CBMEM_ID_VBOOT_WORKBUF is used as vboot workbuffer.
	 * Init CBMEM before loading fsp, to have buffer available */
	cbmem_initialize();

	void *fsp = cbfs_map("fsp.bin", NULL);
	if (!fsp) {
		die("Unable to locate fsp.bin\n");
	} else {
		/* This leaks a mapping which this code assumes is benign as
		 * the flash is memory mapped CPU's address space. */

		/* FIXME: the implementation of find_fsp is utter garbage
		   as it casts error values to FSP_INFO_HEADER pointers.
		   Checking for return values can only be done sanely once
		   that is fixed. */
		fih = find_fsp((uintptr_t)fsp);
	}

	temp_ram_exit = (FSP_TEMP_RAM_EXIT)(fih->TempRamExitEntryOffset +
						fih->ImageBase);
	printk(BIOS_DEBUG, "Calling TempRamExit: %p\n", temp_ram_exit);
	status = temp_ram_exit(NULL);

	if (status != FSP_SUCCESS) {
		printk(BIOS_CRIT, "TempRamExit returned 0x%08x\n", status);
		die("TempRamExit returned an error!\n");
	}
}
