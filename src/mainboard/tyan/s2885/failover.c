#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include "arch/romcc_io.h"
#include "pc80/mc146818rtc_early.c"
#if 0
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#endif
#include "southbridge/amd/amd8111/amd8111_enable_rom.c"
#include "northbridge/amd/amdk8/early_ht.c"
#include "cpu/p6/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"

static void main(void)
{
	/* Nothing special needs to be done to find bus 0 */
	/* Allow the HT devices to be found */
#if 0
        uart_init();
        console_init();
#endif
        enumerate_ht_chain(0);

	/* Setup the 8111 */
	amd8111_enable_rom();

	/* Is this a cpu reset? */
	if (cpu_init_detected()) {
		if (last_boot_normal()) {
			asm("jmp __normal_image");
		} else {
			asm("jmp __cpu_reset");
		}
	}
	/* Is this a secondary cpu? */
	else if (!boot_cpu() && last_boot_normal()) {
		asm("jmp __normal_image");
	}
	/* This is the primary cpu how should I boot? */
	else if (do_normal_boot()) {
		asm("jmp __normal_image");
	}
}
