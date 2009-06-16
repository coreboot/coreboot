#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include "arch/romcc_io.h"
/* no code inclusion allowed */
//#include "pc80/mc146818rtc_early.c"
//#include "cpu/x86/lapic/boot_cpu.c"

static void main(void)
{
#if 0
	/* Is this a cpu reset? */
	if (cpu_init_detected()) {
		if (last_boot_normal()) {
			asm("jmp __normal_image");
		} else {
			asm("jmp __cpu_reset");
		}
	}

	/* This is the primary cpu how should I boot? */
	else if (do_normal_boot()) {
		asm("jmp __normal_image");
	}
#endif
}

