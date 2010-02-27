#define ASSEMBLY 1
#define __PRE_RAM__
#define ASM_CONSOLE_LOGLEVEL 8
#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#if 0
#include <arch/smp/lapic.h>
#endif
#include <arch/hlt.h>
//#include "option_table.h"
#include <stdlib.h>
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "lib/ramtest.c"
#include "southbridge/intel/i82801dx/i82801dx.h"
#include "southbridge/intel/i82801dx/i82801dx_early_smbus.c"
#include "northbridge/intel/i855/raminit.h"

#if 0
#include "cpu/p6/apic_timer.c"
#include "lib/delay.c"
#endif

#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/intel/i855/debug.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c" 
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

static void hard_reset(void)
{
        outb(0x0e, 0x0cf9);
}

static void memreset_setup(void)
{
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
}



static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
        /* nothing to do */
}
 
static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/i855/raminit.c"
#include "northbridge/intel/i855/reset_test.c"
#include "lib/generic_sdram.c"

static void main(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 1),
			.channel0 = { (0xa<<3)|0, 0 },
		},
	};

	if (bist == 0) {
		early_mtrr_init();
#if 0
		enable_lapic();
		init_timer();
#endif
	}
        
        w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
        uart_init();
        console_init();


	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);
	

#if 0
	print_pci_devices();
#endif

	if(!bios_reset_detected()) {
        	enable_smbus();
#if 0
      	dump_spd_registers(&memctrl[0]);
	//        	dump_smbus_registers();
#endif


		memreset_setup();

		sdram_initialize(ARRAY_SIZE(memctrl), memctrl);

	} 
#if 0
	else {
		        /* clear memory 1meg */
        __asm__ volatile(
                "1: \n\t"
                "movl %0, %%fs:(%1)\n\t"
                "addl $4,%1\n\t"
                "subl $4,%2\n\t"
                "jnz 1b\n\t"
                :
                : "a" (0), "D" (0), "c" (1024*1024)
                ); 
	
	}
#endif

#if 0
	dump_pci_devices();
#endif
#if 0
	dump_pci_device(PCI_DEV(0, 0, 0));
#endif

/*
#if  0
	ram_check(0x00000000, msr.lo+(msr.hi<<32));
#else
#if 0
	// Check 16MB of memory @ 0
	ram_check(0x00000000, 0x01000000);
#else
	// Check 16MB of memory @ 2GB 
	ram_check(0x80000000, 0x81000000);
#endif
#endif
*/
}
