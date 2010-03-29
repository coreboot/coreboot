#define ASSEMBLY 1

 
#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "lib/ramtest.c"

#if 0
static void post_code(uint8_t value) {
#if 1
        int i;
        for(i=0;i<0x80000;i++) {
                outb(value, 0x80);
        }
#endif
}
#endif

#include "southbridge/intel/i82801ex/i82801ex_early_smbus.c"
#include "northbridge/intel/e7501/raminit.h"

#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/intel/e7501/debug.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"

#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

// FIXME: There's another hard_reset() in reset.c. Why?
static void hard_reset(void)
{
        /* full reset */
	outb(0x0a, 0x0cf9);
        outb(0x0e, 0x0cf9);
}

static void soft_reset(void)
{
#if 1
        /* link reset */
	outb(0x02, 0x0cf9);
        outb(0x06, 0x0cf9);
#endif
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
#include "lib/generic_sdram.c"


#include "cpu/x86/car/copy_and_run.c"

void amd64_main(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
                {
                        .d0 = PCI_DEV(0, 0, 0),
                        .d0f1 = PCI_DEV(0, 0, 1),
                        .channel0 = { (0xa<<3)|0, (0xa<<3)|1, (0xa<<3)|2, 0 },
                        .channel1 = { (0xa<<3)|4, (0xa<<3)|5, (0xa<<3)|6, 0 },
                },
	};
	
	unsigned cpu_reset = 0;

       if (bist == 0) 
	{
//		early_mtrr_init();
                enable_lapic();

        }

//	post_code(0x32);
	
 	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
        uart_init();
        console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

//        setup_s2735_resource_map();

	if(bios_reset_detected()) {
		cpu_reset = 1;
		goto cpu_reset_x;
	}

	enable_smbus();
#if 0
	dump_spd_registers(&memctrl[0]);
#endif
#if 0
	dump_smbus_registers();
#endif

	memreset_setup();
	sdram_initialize(1, memctrl);

#if 0
	dump_pci_devices();
#endif

#if 1
        dump_pci_device(PCI_DEV(0, 0, 0));
#endif


#if 1
        {
        	/* Check value of esp to verify if we have enough rom for stack in Cache as RAM */
	        unsigned v_esp;
	        __asm__ volatile (
        	        "movl   %%esp, %0\n\t"
	                : "=a" (v_esp)
	        );
#if CONFIG_USE_INIT
	        printk(BIOS_DEBUG, "v_esp=%08x\r\n", v_esp);
#else
	        print_debug("v_esp="); print_debug_hex32(v_esp); print_debug("\r\n");
#endif
        }

#endif
#if 1

cpu_reset_x:

#if CONFIG_USE_INIT
        printk(BIOS_DEBUG, "cpu_reset = %08x\r\n",cpu_reset);
#else
        print_debug("cpu_reset = "); print_debug_hex32(cpu_reset); print_debug("\r\n");
#endif

	if(cpu_reset == 0) {
	        print_debug("Clearing initial memory region: ");
	}
	print_debug("No cache as ram now - ");

	/* store cpu_reset to ebx */
        __asm__ volatile (
                "movl %0, %%ebx\n\t"
                ::"a" (cpu_reset)
        );

	if(cpu_reset==0) {
#define CLEAR_FIRST_1M_RAM 1
#include "cpu/x86/car/cache_as_ram_post.c"
	}
	else {
#undef CLEAR_FIRST_1M_RAM 
#include "cpu/x86/car/cache_as_ram_post.c"
	}

	__asm__ volatile (
                /* set new esp */ /* before CONFIG_RAMBASE */
                "subl   %0, %%ebp\n\t"
                "subl   %0, %%esp\n\t"
                ::"a"( (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE)- CONFIG_RAMBASE )
	);

	{
		unsigned new_cpu_reset;

		/* get back cpu_reset from ebx */
		__asm__ volatile (
			"movl %%ebx, %0\n\t"
			:"=a" (new_cpu_reset)
		);

                /* We can not go back any more, we lost old stack data in cache as ram*/
                if(new_cpu_reset==0) {
                        print_debug("Use Ram as Stack now - done\r\n");
                } else
                {  
                        print_debug("Use Ram as Stack now - \r\n");
                }
#if CONFIG_USE_INIT
                printk(BIOS_DEBUG, "new_cpu_reset = %08x\r\n", new_cpu_reset);
#else
                print_debug("new_cpu_reset = "); print_debug_hex32(new_cpu_reset); print_debug("\r\n");
#endif
	
#ifdef DEACTIVATE_CAR
		print_debug("Deactivating CAR");
#include DEACTIVATE_CAR_FILE
		print_debug(" - Done.\r\n");
#endif
		/*copy and execute coreboot_ram */
		copy_and_run(new_cpu_reset);
		/* We will not return */
	}
#endif


	print_debug("should not be here -\r\n");

}
