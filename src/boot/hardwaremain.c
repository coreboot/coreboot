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
#include <reset.h>
#include <boot/tables.h>
#include <boot/elf.h>
#include <cbfs.h>
#if CONFIG_HAVE_ACPI_RESUME
#include <arch/acpi.h>
#endif
#if CONFIG_WRITE_HIGH_TABLES
#include <cbmem.h>
#endif

/**
 * @brief Main function of the RAM part of coreboot.
 *
 * Coreboot is divided into Pre-RAM part and RAM part.
 *
 * Device Enumeration:
 *	In the dev_enumerate() phase,
 */

void hardwaremain(int boot_complete);

void hardwaremain(int boot_complete)
{
	struct lb_memory *lb_mem;

	post_code(POST_ENTRY_RAMSTAGE);

	/* console_init() MUST PRECEDE ALL printk()! */
	console_init();

	post_code(POST_CONSOLE_READY);

	printk(BIOS_NOTICE, "coreboot-%s%s %s %s...\n",
		      coreboot_version, coreboot_extra_version, coreboot_build,
		      (boot_complete)?"rebooting":"booting");

	post_code(POST_CONSOLE_BOOT_MSG);

	/* If we have already booted attempt a hard reboot */
	if (boot_complete) {
		hard_reset();
	}

	/* FIXME: Is there a better way to handle this? */
	init_timer();

	/* Find the devices we don't have hard coded knowledge about. */
	dev_enumerate();
	post_code(POST_DEVICE_ENUMERATION_COMPLETE);
	/* Now compute and assign the bus resources. */
	dev_configure();
	post_code(POST_DEVICE_CONFIGURATION_COMPLETE);
	/* Now actually enable devices on the bus */
	dev_enable();
	/* And of course initialize devices on the bus */
	dev_initialize();
	post_code(POST_DEVICES_ENABLED);

#if CONFIG_WRITE_HIGH_TABLES == 1
	cbmem_initialize();
#endif
#if CONFIG_HAVE_ACPI_RESUME == 1
	suspend_resume();
	post_code(0x8a);
#endif

	/* Now that we have collected all of our information
	 * write our configuration tables.
	 */
	lb_mem = write_tables();
	cbfs_load_payload(lb_mem, CONFIG_CBFS_PREFIX "/payload");
	printk(BIOS_ERR, "Boot failed.\n");
}

