/*
 * stage2 - LinuxBIOS ram based setup
 *
 * Copyright (C) 2000 Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 *
 */

/*
This software and ancillary information (herein called SOFTWARE)
called LinuxBIOS is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34.  Unless otherwise indicated, this SOFTWARE has
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


/*
 * C Bootstrap code for the LinuxBIOS
 */


#include <arch/types.h>
#include <string.h>
#include <lar.h>
#include <console/loglevel.h>


/**
 * @brief Main function of the DRAM part of LinuxBIOS.
 *
 * LinuxBIOS is divided into Pre-DRAM part and DRAM part. 
 * The phases before this part are phase 0 and phase 1.
 * This part contains phases x through y. 
 *
 * 
 * Device Enumeration:
 *	In the dev_enumerate() phase, 
 */
void stage2(void)
{
	struct lb_memory *lb_mem;

	post_code(0x20);
	dev_init();

	dev_phase1(); /* console init, also ANYTHING that has to be done before printk can be used */
	
	post_code(0x30);

//	printk_notice("LinuxBIOS-%s%s %s booting...\n", linuxbios_version, linuxbios_extra_version, linuxbios_build);

	post_code(0x40);

	/* here is where weird stuff like init_timer handling should be done. This is for ANYTHING
	  * that might have to happen before device enumeration but that needs a printk
	  */
	dev_phase2();
	/* walk physical devices and add any dynamic devices to the device tree */
	post_code(0x30);
	dev_root_phase3();
	post_code(0x40);
	/* Compute and assign the bus resources. */
	dev_phase4();
	post_code(0x50);
	/* Now actually enable devices on the bus */
	dev_root_phase5(); 
	post_code(0x60);
	/*initialize devices on the bus */
	dev_phase6(); 
	
	post_code(0x70);
#ifdef NOT
	/* Now that we have collected all of our information
	 * write our configuration tables.
	 */
	phase8(); /* all devices up, prepare for elfboot */

	elfboot(lb_mem);
#endif

}

