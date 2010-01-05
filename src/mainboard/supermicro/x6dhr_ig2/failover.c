#define ASSEMBLY 1
#define __PRE_RAM__
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "pc80/mc146818rtc_early.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/intel/e7520/memory_initialized.c"

static unsigned long main(unsigned long bist)
{
	/* Did just the cpu reset? */
	if (memory_initialized()) {
	 	if (last_boot_normal()) {
			goto normal_image;
		} else {
			goto cpu_reset;
		}
	}

	/* This is the primary cpu how should I boot? */
	else if (do_normal_boot()) {
		goto normal_image;
	}
	else {
		goto fallback_image;
	}
 normal_image:
	asm volatile ("jmp __normal_image" 
		: /* outputs */ 
		: "a" (bist) /* inputs */
		: /* clobbers */
		);
 cpu_reset:
	asm volatile ("jmp __cpu_reset"
		: /* outputs */ 
		: "a"(bist) /* inputs */
		: /* clobbers */
		);
 fallback_image:
	return bist;
}
