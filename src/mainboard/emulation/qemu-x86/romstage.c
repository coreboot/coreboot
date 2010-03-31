#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "console/console.c"
#include "pc80/udelay_io.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"

static void main(void)
{
	/*	init_timer();*/
	post_code(0x05);
	
	uart_init();
	console_init();
	
	//print_pci_devices();
	//dump_pci_devices();
}

