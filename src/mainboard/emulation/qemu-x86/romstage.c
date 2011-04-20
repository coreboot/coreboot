#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include "pc80/udelay_io.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"

static void main(void)
{
	/* init_timer(); */
	post_code(0x05);

	console_init();

	//print_pci_devices();
	//dump_pci_devices();
}
