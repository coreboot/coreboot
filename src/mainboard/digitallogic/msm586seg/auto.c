#define ASSEMBLY 1
#define ASM_CONSOLE_LOGLEVEL 8
#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "cpu/amd/sc520/raminit.c"


static void hard_reset(void)
{
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

#include "sdram/generic_sdram.c"

static void main(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 0),
			.channel0 = { (0xa<<3)|0, 0 },
		},
	};

	if (bist == 0) {
		early_mtrr_init();
	}
        
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
