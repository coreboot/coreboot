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
 * C Bootstrap code for the LinuxBIOS
 */


#include <console/console.h>
#include <version.h>
#include <boot/tables.h>
#include <device/device.h>
#include <device/pci.h>
#include <delay.h>
#include <stdlib.h>
#include <part/hard_reset.h>
#include <part/init_timer.h>
#include <boot/elf.h>

void hardwaremain(int boot_complete)
{
	/* the order here is a bit tricky. We don't want to do much of 
	 * anything that uses config registers until after PciAllocateResources
	 * since that function also figures out what kind of config strategy
	 * to use (type 1 or type 2). 
	 * so we turn on cache, then worry about PCI setup, then do other 
	 * things, so that the other work can use the PciRead* and PciWrite*
	 * functions. 
	 */
	struct lb_memory *lb_mem;

	post_code(0x80);

	/* displayinit MUST PRECEDE ALL PRINTK! */
	console_init();
	
	post_code(0x39);
	printk_notice("LinuxBIOS-%s%s %s %s...\n", 
		linuxbios_version, linuxbios_extra_version, linuxbios_build,
		(boot_complete)?"rebooting":"booting");

	post_code(0x40);

	/* If we have already booted attempt a hard reboot */
	if (boot_complete) {
		hard_reset();
	}

	/* FIXME: Is there a better way to handle this? */
	init_timer(); 

	/* pick how to scan the bus. This is first so we can get at memory size. */
	printk_info("Finding PCI configuration type.\n");
	pci_set_method();
	post_code(0x5f);
	dev_enumerate();
	post_code(0x66);
	/* Now do the real bus.
	 * We round the total ram up a lot for thing like the SISFB, which 
	 * shares high memory with the CPU. 
	 */
	dev_configure();
	post_code(0x88);

	dev_enable();

	dev_initialize();
	post_code(0x89);

	/* Now that we have collected all of our information
	 * write our configuration tables.
	 */
	lb_mem = write_tables();

#if CONFIG_FS_STREAM == 1
	filo(lb_mem);
#else
	elfboot(lb_mem);
#endif
}

