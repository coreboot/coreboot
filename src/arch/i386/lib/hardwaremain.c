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
 * C Bootstrap code for the INTEL 
 * $Id$
 *
 */

#define LINUXBIOS
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <cpu/p5/io.h>
#include <intel.h>
#include <pciconf.h>
#include <pci.h>
#include <cpu/p5/cpuid.h>
#include <cpu/p6/ioapic.h>
#include <subr.h>
#include <printk.h>

void intel_main()
{
	// These are only used here, so don't bother putting them in a .h
	void mainboard_fixup(void);
	void nvram_on(void);
	void keyboard_on(void);
	void framebuffer_on(void);
	void intel_copy_irq_routing_table(void);
	unsigned long sizeram(void);
	void intel_cache_on(unsigned long base, unsigned long totalram);
	void intel_zero_irq_settings(void);
	void intel_check_irq_routing_table(void);
	void intel_interrupts_on();

#ifdef FINAL_MAINBOARD_FIXUP
	void final_mainboard_fixup(void);
#endif /* FINAL_MAINBOARD_FIXUP */
#ifdef USE_NEW_SUPERIO_INTERFACE
	extern struct superio *all_superio;
	extern int nsuperio;
	extern void handle_superio(int pass, struct superio *s, int nsuperio);
#endif
#ifdef CONFIGURE_L2_CACHE
	int intel_l2_configure();
#endif /* CONFIGURE_L2_CACHE */

#ifdef CPU_FIXUP
	// some cpus need a fixup done. This is the hook for doing that
	// For now we call it after pci config and sizing. 
	// we may move it earlier for speed. 
	// Comment: the NEW_SUPERIO architecture is actually pretty good.
	// I think we need to move to the same sort of architecture for
	// everything: A config file generated sequence of calls 
	// for initializing all the chips. We stick with this 
	// for now -- rgm. 
	void cpufixup(unsigned long totalram);
#endif

#ifdef UPDATE_MICROCODE
	void intel_display_cpuid_microcode(void);
#endif /* UPDATE_MICROCODE */

	/* the order here is a bit tricky. We don't want to do much of 
	 * anything that uses config registers until after PciAllocateResources
	 * since that function also figures out what kind of config strategy
	 * to use (type 1 or type 2). 
	 * so we turn on cache, then worry about PCI setup, then do other 
	 * things, so that the other work can use the PciRead* and PciWrite*
	 * functions. 
	 */
	unsigned long totalram = 0;
	extern void linuxbiosmain(unsigned long membase, unsigned long totalram);

	/* displayinit MUST PRECEDE ALL PRINTK! */
	displayinit();
	
	post_code(0x39);

	printk(KERN_INFO "Reached intel_main().\n");

	post_code(0x40);

#ifdef UPDATE_MICROCODE
	post_code(0x41);
	printk(KERN_INFO "Updating microcode\n");
	intel_display_cpuid_microcode();
#endif /* UPDATE_MICROCODE */
	post_code(0x42);

	// pick how to scan the bus. This is first so we can get at memory size.
	printk(KERN_INFO "Finding PCI confiuration type...\n");
	pci_set_method();
	post_code(0x5f);
#ifdef USE_NEW_SUPERIO_INTERFACE
	handle_superio(0, all_superio, nsuperio);
#endif
	printk(KERN_INFO "Scanning PCI bus...");
	pci_enumerate();
	post_code(0x66);
	printk(KERN_INFO "done\n");

	// The framebuffer can change how much memory you have. 
	// So you really need to run this before you size ram. 
#ifdef HAVE_FRAMEBUFFER
	framebuffer_on();
#endif /* HAVE_FRAMEBUFFER */

	totalram = sizeram();
	post_code(0x70);
	printk(KERN_INFO "totalram: %ldM\n", totalram/1024);

	// can't size just yet ... 
	// mainboard totalram sizing may not be up yet. If it is not ready, 
	// take a default of 64M
	if (!totalram)
		totalram = 64 * 1024;

#ifdef CPU_FIXUP
	// cpu-dependent fixup. 
	cpufixup(totalram);
#endif
	// Turn on cache before configuring the bus. 
	printk(KERN_INFO "Enabling cache...");
	intel_cache_on(0, totalram);
	post_code(0x80);
	printk(KERN_INFO "done.\n");

	printk(KERN_INFO "Allocating PCI resources...");
	// Now do the real bus
	// we round the total ram up a lot for thing like the SISFB, which 
	// shares high memory with the CPU. 
	pci_configure();
	post_code(0x88);
	pci_enable();
	post_code(0x90);
	printk(KERN_INFO "done.\n");

#ifdef IOAPIC
	/* set up the IO-APIC for the clock interrupt. */
	post_code(0x92);
	setup_ioapic();
#endif /* IOAPIC */

	// generic mainboard fixup
	mainboard_fixup();

#ifndef MAINBOARD_FIXUP_IN_CHARGE

	nvram_on();

	intel_display_cpuid();
	intel_mtrr_check();


#ifndef NO_KEYBOARD

	keyboard_on();
#endif /* NO_KEYBOARD */

#ifndef USE_NEW_SUPERIO_INTERFACE
#ifdef MUST_ENABLE_FLOPPY
	enable_floppy();
	post_code(0x95);
#endif /* MUST_ENABLE_FLOPPY */
#endif
#ifdef USE_NEW_SUPERIO_INTERFACE
	handle_superio(1, all_superio, nsuperio);
#endif

#ifdef SMP
	/* copy the smp block to address 0 */
	intel_smpblock((void *)16);
	post_code(0x96);
#endif /* SMP */

	intel_zero_irq_settings();
	intel_check_irq_routing_table();
	intel_copy_irq_routing_table();

	post_code(0x9a);

	/* to do: intel_serial_on(); */
	intel_interrupts_on();


#ifdef FINAL_MAINBOARD_FIXUP
	printk(KERN_INFO "Final mainboard fixup for ");
	post_code(0xec);
	final_mainboard_fixup();
	printk(KERN_INFO "done.\n");
#endif /* FINAL_MAINBOARD_FIXUP */

#ifdef CONFIGURE_L2_CACHE
	/* now that everything is really up, enable the l2 cache if desired. 
	 * The enable can wait until this point, because linuxbios and it's
	 * data areas are tiny, easily fitting into the L1 cache. 
	 */
	printk(KERN_INFO "Configuring L2 cache...");
	intel_l2_configure();
	printk(KERN_INFO "done.\n");
#endif /* CONFIGURE_L2_CACHE */

#endif /* MAINBOARD_FIXUP_IN_CHARGE */

#ifdef USE_NEW_SUPERIO_INTERFACE
	handle_superio(2, all_superio, nsuperio);
#endif

#ifdef LINUXBIOS
	printk(KERN_INFO "Jumping to linuxbiosmain()...\n");
	// we could go to argc, argv, for main but it seems like overkill.
	post_code(0xed);
	linuxbiosmain(0, totalram);
#endif /* LINUXBIOS */
}


