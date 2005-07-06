#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <arch/cpu.h>
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
#include "northbridge/amd/amdk8/cpu_rev.c"
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
        dev = PCI_DEV(node_link_to_bus(0, 2), 0x04, 3);

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
        dev = PCI_DEV(node_link_to_bus(0, 2), 0x04, 0);

        set_bios_reset();
        pci_write_config8(dev, 0x47, 1);
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

#if 0
static unsigned int generate_row(uint8_t node, uint8_t row, uint8_t maxnodes)
{
	/* Routing Table Node i 
	 *
	 * F0: 0x40, 0x44, 0x48, 0x4c, 0x50, 0x54, 0x58, 0x5c 
	 *  i:    0,    1,    2,    3,    4,    5,    6,    7
	 *
	 * [ 0: 3] Request Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [11: 8] Response Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [19:16] Broadcast route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 */
        uint32_t ret=0x00010101; /* default row entry */

/*
            (L1)       (L2)     
        CPU3-------------CPU1
     (L0)|                |(L0)
         |                |
         |                |
         |                |
         |                |
     (L0)|                |(L0)
        CPU2-------------CPU0---------8131----------8111
            (L2)       (L1)  (L2)       
*/

        /* Link0 of CPU0 to Link0 of CPU1 */
        /* Link1 of CPU0 to Link2 of CPU2 */
        /* Link2 of CPU1 to Link1 of CPU3 */
        /* Link0 of CPU2 to Link0 of CPU3 */

        static const unsigned int rows_4p[4][4] = {
                { 0x00070101, 0x00010202, 0x00030404, 0x00010204 },
                { 0x00010202, 0x000b0101, 0x00010208, 0x00030808 },
                { 0x00030808, 0x00010208, 0x000b0101, 0x00010202 },
                { 0x00010204, 0x00030404, 0x00010202, 0x00070101 }
        };
        
        if (!(node>=maxnodes || row>=maxnodes)) {
		ret=rows_4p[node][row];
        }

        return ret;
}
#endif

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_HUB 0x18
        unsigned device=(ctrl->channel0[0])>>8;
        smbus_write_byte(SMBUS_HUB , 0x01, device);
        smbus_write_byte(SMBUS_HUB , 0x03, 0);
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/setup_resource_map.c"
#include "northbridge/amd/amdk8/raminit.c"

#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

 /* tyan does not want the default */
#include "resourcemap.c"

#if CONFIG_LOGICAL_CPUS==1
#define SET_NB_CFG_54 1
#include "cpu/amd/dualcore/dualcore.c"
#endif

#define FIRST_CPU  1
#define SECOND_CPU 1

#define THIRD_CPU  1 
#define FOURTH_CPU 1 

#define TOTAL_CPUS (FIRST_CPU + SECOND_CPU + THIRD_CPU + FOURTH_CPU)

#define RC0 ((1<<1)<<8)
#define RC1 ((1<<2)<<8)
#define RC2 ((1<<3)<<8)
#define RC3 ((1<<4)<<8)

#define DIMM0 0x50
#define DIMM1 0x51
#define DIMM2 0x52
#define DIMM3 0x53
        
static void main(unsigned long bist)
{
	static const struct mem_controller cpu[] = {
#if FIRST_CPU
                {
                        .node_id = 0,
                        .f0 = PCI_DEV(0, 0x18, 0),
                        .f1 = PCI_DEV(0, 0x18, 1),
                        .f2 = PCI_DEV(0, 0x18, 2),
                        .f3 = PCI_DEV(0, 0x18, 3),
                        .channel0 = { RC0|DIMM0, RC0|DIMM2, 0, 0 },
                        .channel1 = { RC0|DIMM1, RC0|DIMM3, 0, 0 },
                },
#endif
#if SECOND_CPU
                {
                        .node_id = 1,
                        .f0 = PCI_DEV(0, 0x19, 0),
                        .f1 = PCI_DEV(0, 0x19, 1),
                        .f2 = PCI_DEV(0, 0x19, 2),
                        .f3 = PCI_DEV(0, 0x19, 3),
                        .channel0 = { RC1|DIMM0, 0 , 0, 0 },
                        .channel1 = { RC1|DIMM1, 0, 0, 0 },

                },
#endif

#if THIRD_CPU
                {
                        .node_id = 2,
                        .f0 = PCI_DEV(0, 0x1a, 0),
                        .f1 = PCI_DEV(0, 0x1a, 1),
                        .f2 = PCI_DEV(0, 0x1a, 2),
                        .f3 = PCI_DEV(0, 0x1a, 3),
                        .channel0 = { RC2|DIMM0, 0, 0, 0 },
                        .channel1 = { RC2|DIMM1, 0, 0, 0 },

                },
#endif
#if FOURTH_CPU
                {
                        .node_id = 3,
                        .f0 = PCI_DEV(0, 0x1b, 0),
                        .f1 = PCI_DEV(0, 0x1b, 1),
                        .f2 = PCI_DEV(0, 0x1b, 2),
                        .f3 = PCI_DEV(0, 0x1b, 3),
                        .channel0 = { RC3|DIMM0, 0, 0, 0 },
                        .channel1 = { RC3|DIMM1, 0, 0, 0 },

                },
#endif
	};
	int i;
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
//                        start_other_core(id.nodeid);
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
                        stop_this_cpu(); // it will stop all cores except core0 of cpu0
                }
        }

        w83627hf_enable_serial(SERIAL_DEV, TTYS0_BASE);
        uart_init();    
        console_init(); 
                
        /* Halt if there was a built in self test failure */
	report_bist_failure(bist);

        setup_s4880_resource_map();

        needs_reset = setup_coherent_ht_domain();

#if CONFIG_LOGICAL_CPUS==1
        // It is said that we should start core1 after all core0 launched
        start_other_cores();
#endif

#if 0
        needs_reset |= ht_setup_chain(PCI_DEV(0, 0x18, 0), 0xc0);
#else
        // automatically set that for you, but you might meet tight space
        needs_reset |= ht_setup_chains_x();
#endif
	
        if (needs_reset) {
                print_info("ht reset -\r\n");
                soft_reset();
        }
	
	enable_smbus();

	memreset_setup();
	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);

}
