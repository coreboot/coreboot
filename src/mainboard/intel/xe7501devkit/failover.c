#define ASSEMBLY 1
#define __PRE_RAM__
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "pc80/mc146818rtc_early.c"
#include "southbridge/intel/i82801cx/cmos_failover.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/intel/e7501/reset_test.c"

static unsigned long main(unsigned long bist)
{
	/* Is this a deliberate reset by the bios */
	if (bios_reset_detected() && last_boot_normal()) {
		goto normal_image;
	}
	/* This is the primary cpu how should I boot? */
	else  {

		check_cmos_failed();		

		if (do_normal_boot()) {
			goto normal_image;
		}
		else {
			goto fallback_image;
		}
	}
 normal_image:
	asm volatile ("jmp __normal_image" 
		: /* outputs */ 
		: "a" (bist) /* inputs */
		: /* clobbers */
		);
#if 0
 cpu_reset:
	asm volatile ("jmp __cpu_reset"
		: /* outputs */ 
		: "a"(bist) /* inputs */
		: /* clobbers */
		);
#endif
 fallback_image:
	return bist;
}
