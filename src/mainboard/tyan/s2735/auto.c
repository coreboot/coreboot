#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/smp/lapic.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "southbridge/intel/i82801er/i82801er_early_smbus.c"
#include "northbridge/intel/e7501/raminit.h"

#if 1
#include "cpu/p6/apic_timer.c"
#include "lib/delay.c"
#endif

#include "cpu/p6/boot_cpu.c"
#include "northbridge/intel/e7501/debug.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c" 

#include "cpu/p6/earlymtrr.c"

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

#include "northbridge/intel/e7501/raminit.c"
#include "northbridge/intel/e7501/reset_test.c"
#include "sdram/generic_sdram.c"

static void main(void)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 0),
			.d0f1 = PCI_DEV(0, 0, 1),
			.channel0 = { (0xa<<3)|0, (0xa<<3)|1, (0xa<<3)|2, 0 },
			.channel1 = { (0xa<<3)|4, (0xa<<3)|5, (0xa<<3)|6, 0 },
		},
	};

#if 1
        enable_lapic();
        init_timer();
#endif
        
        w83627hf_enable_serial(SERIAL_DEV, TTYS0_BASE);
        uart_init();
        console_init();
//        setup_default_resource_map();
#if 0
	print_pci_devices();
#endif
	if(!bios_reset_detected()) {
        	enable_smbus();
#if 0
//      	dump_spd_registers(&memctrl[0]);
        	dump_smbus_registers();
#endif

		memreset_setup();
		sdram_initialize(sizeof(memctrl)/sizeof(memctrl[0]), memctrl);
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

#if 0
	msr_t msr;
	msr = rdmsr(TOP_MEM2);
	print_debug("TOP_MEM2: ");
	print_debug_hex32(msr.hi);
	print_debug_hex32(msr.lo);
	print_debug("\r\n");
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
