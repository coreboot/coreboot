#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <arch/cpu.h>
#include <stdlib.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>

#include "southbridge/intel/i82801dx/i82801dx.h"
#include "southbridge/intel/i82801dx/early_smbus.c"
#include "southbridge/intel/i82801dx/reset.c"
#include "northbridge/intel/e7505/raminit.h"
#include "northbridge/intel/e7505/debug.c"
#include "superio/smsc/lpc47m10x/early_serial.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47M10X2_SP1)

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/e7505/raminit.c"
#include "northbridge/intel/e7505/reset_test.c"
#include "lib/generic_sdram.c"
#include "lib/ramtest.c"


// This function MUST appear last (ROMCC limitation)
static void main(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 0),
			.d0f1 = PCI_DEV(0, 0, 1),
			.channel0 = { 0x50, 0x52, 0, 0 },
			.channel1 = { 0x51, 0x53, 0, 0 },
		},
	};

	if (bist == 0) 	{
		// Skip this if there was a built in self test failure
		early_mtrr_init();
	        enable_lapic();
	}

	// Get the serial port running and print a welcome banner
	lpc47m10x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
//	uart_init();
	console_init();

	// Halt if there was a built in self test failure
	report_bist_failure(bist);

	// print_pci_devices();

	// If this is a warm boot, some initialization can be skipped
	if (!bios_reset_detected()) {
		enable_smbus();
	//    	dump_spd_registers(&memctrl[0]);
	//      dump_smbus_registers();
	//	memreset_setup();		No-op for this chipset
		sdram_initialize(ARRAY_SIZE(memctrl), memctrl);
		
		ram_check(0x200000,0x800000);
	}


	// NOTE: ROMCC dies with an internal compiler error
	//		 if the following line is removed.
	print_debug("SDRAM is up.\r\n");

}
