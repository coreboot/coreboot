#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <arch/cpu.h>
#include <stdlib.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "southbridge/amd/amd8111/amd8111_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include <cpu/amd/model_fxx_rev.h>
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"
#include "cpu/amd/mtrr/amd_earlymtrr.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)
/* Look up a which bus a given node/link combination is on.
 * return 0 when we can't find the answer.
 */
static unsigned node_link_to_bus(unsigned node, unsigned link)
{
        unsigned reg;
        
        for(reg = 0xE0; reg < 0xF0; reg += 0x04) {
                unsigned config_map;
                config_map = pci_read_config32(PCI_DEV(0, 0x18, 1), reg);
                if ((config_map & 3) != 3) {
                        continue; 
                }       
                if ((((config_map >> 4) & 7) == node) &&
                        (((config_map >> 8) & 3) == link))
                {       
                        return (config_map >> 16) & 0xff;
                }       
        }       
        return 0;
}       

static void hard_reset(void)
{
        device_t dev;

        /* Find the device */
        dev = PCI_DEV(node_link_to_bus(0, 0), 0x04, 3);

        set_bios_reset();

        /* enable cf9 */
        pci_write_config8(dev, 0x41, 0xf1);
        /* reset */
        outb(0x0e, 0x0cf9);
}

static void soft_reset(void)
{
        device_t dev;

        /* Find the device */
        dev = PCI_DEV(node_link_to_bus(0, 0), 0x04, 0);

        set_bios_reset();
        pci_write_config8(dev, 0x47, 1);
}

#define REV_B_RESET 0
static void memreset_setup(void)
{
	if (is_cpu_pre_c0()) {
		outb((0 << 7) | (0 << 6) | (0 << 5) | (0 << 4) | (1 << 2) | (0 << 0), SMBUS_IO_BASE + 0xc0 + 16);	//REVC_MEMRST_EN=0
	} else {
		outb((0 << 7) | (0 << 6) | (0 << 5) | (0 << 4) | (1 << 2) | (1 << 0), SMBUS_IO_BASE + 0xc0 + 16);	//REVC_MEMRST_EN=1
	}
	outb((0 << 7) | (0 << 6) | (0 << 5) | (0 << 4) | (1 << 2) |
	     (0 << 0), SMBUS_IO_BASE + 0xc0 + 17);
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
	if (is_cpu_pre_c0()) {
		udelay(800);
		outb((0 << 7) | (0 << 6) | (0 << 5) | (0 << 4) | (1 << 2) | (1 << 0), SMBUS_IO_BASE + 0xc0 + 17);	//REVB_MEMRST_L=1
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

#include "northbridge/amd/amdk8/setup_resource_map.c"
#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"
#include "northbridge/amd/amdk8/resourcemap.c"

#if CONFIG_LOGICAL_CPUS==1
#define SET_NB_CFG_54 1
#include "cpu/amd/dualcore/dualcore.c"
#endif

static void main(unsigned long bist)
{
	/*
	 * GPIO28 of 8111 will control H0_MEMRESET_L
	 * GPIO29 of 8111 will control H1_MEMRESET_L
	 */
	static const struct mem_controller cpu[] = {
		{
		 .node_id = 0,
		 .f0 = PCI_DEV(0, 0x18, 0),
		 .f1 = PCI_DEV(0, 0x18, 1),
		 .f2 = PCI_DEV(0, 0x18, 2),
		 .f3 = PCI_DEV(0, 0x18, 3),
		 .channel0 = {(0xa << 3) | 0, (0xa << 3) | 2, 0, 0},
		 .channel1 = {(0xa << 3) | 1, (0xa << 3) | 3, 0, 0},
		 },
	};

	int needs_reset;

#if CONFIG_LOGICAL_CPUS==1
        struct node_core_id id;
#else
        unsigned nodeid;
#endif

        if (bist == 0) {
                /* Skip this if there was a built in self test failure */
                amd_early_mtrr_init();

#if CONFIG_LOGICAL_CPUS==1
                set_apicid_cpuid_lo();
#endif

                enable_lapic();
                init_timer();

#if CONFIG_LOGICAL_CPUS==1
                id = get_node_core_id_x();
                if(id.coreid == 0) {
                        if (cpu_init_detected(id.nodeid)) {
                                asm volatile ("jmp __cpu_reset");
                        }
                        distinguish_cpu_resets(id.nodeid);
                }
#else
                nodeid = lapicid();
                if (cpu_init_detected(nodeid)) {
                        asm volatile ("jmp __cpu_reset");
                }
                distinguish_cpu_resets(nodeid);
#endif

                if (!boot_cpu()
#if CONFIG_LOGICAL_CPUS==1 
                        || (id.coreid != 0)
#endif
                ) {
                        stop_this_cpu(); 
                }
        }
                        
        w83627hf_enable_serial(SERIAL_DEV, TTYS0_BASE);
        uart_init();    
        console_init(); 
                
        /* Halt if there was a built in self test failure */
        report_bist_failure(bist);

	setup_default_resource_map();
	needs_reset = setup_coherent_ht_domain();
#if CONFIG_LOGICAL_CPUS==1
        start_other_cores();
#endif
	needs_reset |= ht_setup_chain(PCI_DEV(0, 0x18, 0), 0x80);
	if (needs_reset) {
		print_info("ht reset -\r\n");
		soft_reset();
	}
	enable_smbus();

	memreset_setup();
	sdram_initialize(ARRAY_SIZE(cpu), cpu);

}
