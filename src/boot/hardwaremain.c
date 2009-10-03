/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */


/*
 * C Bootstrap code for the coreboot
 */

#include <console/console.h>
#include <version.h>
#include <device/device.h>
#include <device/pci.h>
#include <delay.h>
#include <stdlib.h>
#include <part/hard_reset.h>
#include <part/init_timer.h>
#include <boot/tables.h>
#include <boot/elf.h>
#include <cbfs.h>
#if CONFIG_HAVE_ACPI_RESUME
#include <arch/acpi.h>
#endif

/**
 * @brief Main function of the DRAM part of coreboot.
 *
 * Coreboot is divided into Pre-DRAM part and DRAM part. 
 *
 * 
 * Device Enumeration:
 *	In the dev_enumerate() phase, 
 */

void hardwaremain(int boot_complete)
{
	struct lb_memory *lb_mem;

	post_code(0x80);

	/* displayinit MUST PRECEDE ALL PRINTK! */
	console_init();
	
	post_code(0x39);

	printk(BIOS_NOTICE, "coreboot-%s%s %s %s...\n", 
		      coreboot_version, coreboot_extra_version, coreboot_build,
		      (boot_complete)?"rebooting":"booting");

	post_code(0x40);

	/* If we have already booted attempt a hard reboot */
	if (boot_complete) {
		hard_reset();
	}

	/* FIXME: Is there a better way to handle this? */
	init_timer(); 

	/* Find the devices we don't have hard coded knowledge about. */
	dev_enumerate();
	post_code(0x66);
	/* Now compute and assign the bus resources. */
	dev_configure();
	post_code(0x88);
	/* Now actually enable devices on the bus */
	dev_enable();
	/* And of course initialize devices on the bus */
	dev_initialize();
	post_code(0x89);

#if CONFIG_HAVE_ACPI_RESUME == 1
	suspend_resume();
	post_code(0x8a);
#endif

	/* Now that we have collected all of our information
	 * write our configuration tables.
	 */
	lb_mem = write_tables();
#if CONFIG_USE_FALLBACK_IMAGE == 1
	cbfs_load_payload(lb_mem, "fallback/payload");
#else
	cbfs_load_payload(lb_mem, "normal/payload");
#endif
	printk(BIOS_ERR, "Boot failed.\n");
}

