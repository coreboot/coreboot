#define ASSEMBLY 1
#define __ROMCC__
 
#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"

#include "northbridge/amd/amdk8/cpu_rev.c"
#define K8_HT_FREQ_1G_SUPPORT 0
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "southbridge/amd/amd8111/amd8111_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"

#if CONFIG_USE_INIT == 0
#include "lib/memcpy.c"
#endif

#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"

#include "cpu/amd/mtrr/amd_earlymtrr.c"
#include "cpu/x86/bist.h"

#include "northbridge/amd/amdk8/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

static void hard_reset(void)
{
        set_bios_reset();

        /* enable cf9 */
        pci_write_config8(PCI_DEV(0, 0x04, 3), 0x41, 0xf1);
        /* reset */
        outb(0x0e, 0x0cf9);
}

static void soft_reset(void)
{
        set_bios_reset();
        pci_write_config8(PCI_DEV(0, 0x04, 0), 0x47, 1);
}

static void memreset_setup(void)
{
   if (is_cpu_pre_c0()) {
        outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 16);  //REVC_MEMRST_EN=0
   }
   else {
        outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 16);  //REVC_MEMRST_EN=1
   }
        outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 17);
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
   if (is_cpu_pre_c0()) {
        udelay(800);
        outb((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 17); //REVB_MEMRST_L=1
        udelay(90);
   }
}

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
	/* nothing to do */
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#define K8_4RANK_DIMM_SUPPORT 1

#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/resourcemap.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

#if CONFIG_LOGICAL_CPUS==1
#define SET_NB_CFG_54 1
#include "cpu/amd/dualcore/dualcore.c"
#endif

#define FIRST_CPU  1
#define SECOND_CPU 1
#define TOTAL_CPUS (FIRST_CPU + SECOND_CPU)

#include "cpu/amd/car/copy_and_run.c"

#if USE_FALLBACK_IMAGE == 1

#include "southbridge/amd/amd8111/amd8111_enable_rom.c"
#include "northbridge/amd/amdk8/early_ht.c"

void real_main(unsigned long bist);

void amd64_main(unsigned long bist)
{
#if CONFIG_LOGICAL_CPUS==1
        struct node_core_id id;
#else
        unsigned nodeid;
#endif
        /* Make cerain my local apic is useable */
//        enable_lapic();

#if CONFIG_LOGICAL_CPUS==1
        id = get_node_core_id_x();
        /* Is this a cpu only reset? */
        if (cpu_init_detected(id.nodeid)) {
#else
//        nodeid = lapicid();
	nodeid = get_node_id();
        /* Is this a cpu only reset? */
        if (cpu_init_detected(nodeid)) {
#endif
                if (last_boot_normal()) {
                        goto normal_image;
                } else {
                        goto cpu_reset;
                }
        }

        /* Is this a secondary cpu? */
        if (!boot_cpu()) {
                if (last_boot_normal()) {
                        goto normal_image;
                } else {
                        goto fallback_image;
                }
        }

        /* Nothing special needs to be done to find bus 0 */
        /* Allow the HT devices to be found */

        enumerate_ht_chain();

        /* Setup the ck804 */
        amd8111_enable_rom();

        /* Is this a deliberate reset by the bios */
        if (bios_reset_detected() && last_boot_normal()) {
                goto normal_image;
        }
        /* This is the primary cpu how should I boot? */
        else if (do_normal_boot()) {
                goto normal_image;
        }
        else {
                goto fallback_image;
        }
 normal_image:
        __asm__ volatile ("jmp __normal_image"
                : /* outputs */
                : "a" (bist) /* inputs */
                );
 cpu_reset:
#if 0
        //CPU reset will reset memtroller ???
        asm volatile ("jmp __cpu_reset" 
                : /* outputs */ 
                : "a"(bist) /* inputs */
                );
#endif

 fallback_image:
        real_main(bist);
}
void real_main(unsigned long bist)
#else
void amd64_main(unsigned long bist)
#endif
{
	static const struct mem_controller cpu[] = {
#if FIRST_CPU
		{
			.node_id = 0,
			.f0 = PCI_DEV(0, 0x18, 0),
			.f1 = PCI_DEV(0, 0x18, 1),
			.f2 = PCI_DEV(0, 0x18, 2),
			.f3 = PCI_DEV(0, 0x18, 3),
			.channel0 = { (0xa<<3)|0, (0xa<<3)|2, 0, 0 },
			.channel1 = { (0xa<<3)|1, (0xa<<3)|3, 0, 0 },
		},
#endif
#if SECOND_CPU
		{
			.node_id = 1,
			.f0 = PCI_DEV(0, 0x19, 0),
			.f1 = PCI_DEV(0, 0x19, 1),
			.f2 = PCI_DEV(0, 0x19, 2),
			.f3 = PCI_DEV(0, 0x19, 3),
			.channel0 = { (0xa<<3)|4, (0xa<<3)|6, 0, 0 },
			.channel1 = { (0xa<<3)|5, (0xa<<3)|7, 0, 0 },
		},
#endif
	};

        int needs_reset;
	unsigned cpu_reset = 0;

        if (bist == 0) {
#if CONFIG_LOGICAL_CPUS==1
        	struct node_core_id id;
#else
	        unsigned nodeid;
#endif
                /* Skip this if there was a built in self test failure */
//                amd_early_mtrr_init(); # don't need, already done in cache_as_ram

#if CONFIG_LOGICAL_CPUS==1
                set_apicid_cpuid_lo();
		id = get_node_core_id_x(); // that is initid
#else
                nodeid = get_node_id();
#endif

                enable_lapic();
                init_timer();


#if CONFIG_LOGICAL_CPUS==1
                if(id.coreid == 0) {
                        if (cpu_init_detected(id.nodeid)) {
				cpu_reset = 1;
				goto cpu_reset_x;
                        }
                        distinguish_cpu_resets(id.nodeid);
                }
#else
                if (cpu_init_detected(nodeid)) {
			cpu_reset = 1;
			goto cpu_reset_x;
                }
                distinguish_cpu_resets(nodeid);
#endif


                if (!boot_cpu()
#if CONFIG_LOGICAL_CPUS==1 
                        || (id.coreid != 0)
#endif
                ) {
			// We need stop the CACHE as RAM for this CPU too
			#include "cpu/amd/car/cache_as_ram_post.c"
                        stop_this_cpu(); // it will stop all cores except core0 of cpu0
                }
        }

	
 	w83627hf_enable_serial(SERIAL_DEV, TTYS0_BASE);
        uart_init();
        console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

        setup_default_resource_map();
#if 0
        dump_pci_device(PCI_DEV(0, 0x18, 0));
	dump_pci_device(PCI_DEV(0, 0x19, 0));
#endif

	needs_reset = setup_coherent_ht_domain();
	
#if CONFIG_LOGICAL_CPUS==1
        start_other_cores();
#endif
        needs_reset |= ht_setup_chains_x();

       	if (needs_reset) {
               	print_info("ht reset -\r\n");
               	soft_reset();
       	}

	enable_smbus();
#if 0
	dump_spd_registers(&cpu[0]);
#endif
#if 0
	dump_smbus_registers();
#endif

	memreset_setup();
	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);

#if 0
	dump_pci_devices();
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
        printk_debug("v_esp=%08x\r\n", v_esp);
#else           
        print_debug("v_esp="); print_debug_hex32(v_esp); print_debug("\r\n");
#endif    
        }
#endif



cpu_reset_x:

#if CONFIG_USE_INIT
        printk_debug("cpu_reset = %08x\r\n",cpu_reset);
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
#include "cpu/amd/car/cache_as_ram_post.c"
	}
	else {
#undef CLEAR_FIRST_1M_RAM 
#include "cpu/amd/car/cache_as_ram_post.c"
	}

	__asm__ volatile (
                /* set new esp */ /* before _RAMBASE */
                "subl   %0, %%ebp\n\t"
                "subl   %0, %%esp\n\t"
                ::"a"( (DCACHE_RAM_BASE + DCACHE_RAM_SIZE)- _RAMBASE )
	);

	{
		unsigned new_cpu_reset;

		/* get back cpu_reset from ebx */
		__asm__ volatile (
			"movl %%ebx, %0\n\t"
			:"=a" (new_cpu_reset)
		);

		print_debug("Use Ram as Stack now - "); /* but We can not go back any more, we lost old stack data in cache as ram*/
		if(new_cpu_reset==0) {        
		        print_debug("done\r\n");	
		} else 
		{	
			print_debug("\r\n");
		}

#if CONFIG_USE_INIT
                printk_debug("new_cpu_reset = %08x\r\n", new_cpu_reset);
#else
                print_debug("new_cpu_reset = "); print_debug_hex32(new_cpu_reset); print_debug("\r\n");
#endif
		/*copy and execute linuxbios_ram */
		copy_and_run(new_cpu_reset);
		/* We will not return */
	}


	print_debug("should not be here -\r\n");

}
