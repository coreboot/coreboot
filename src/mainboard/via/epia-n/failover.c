#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include "arch/romcc_io.h"
#include "pc80/mc146818rtc_early.c"

static unsigned long main(unsigned long bist)
{
#if 0
	/* This is the primary cpu how should I boot? */
	if (do_normal_boot()) {
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
#endif
	return bist;
}
