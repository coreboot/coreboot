#define ASSEMBLY 1
 
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
#include "southbridge/amd/amd8111/amd8111_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "northbridge/amd/amdk8/incoherent_ht.c"
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
#define SMBUS_HUB 0x18
        int ret,i;
        unsigned device=(ctrl->channel0[0])>>8;
        /* the very first write always get COL_STS=1 and ABRT_STS=1, so try another time*/
        i=2;
        do {
                ret = smbus_write_byte(SMBUS_HUB, 0x01, device);
        } while ((ret!=0) && (i-->0));

        smbus_write_byte(SMBUS_HUB, 0x03, 0);
}
#if 0
static inline void change_i2c_mux(unsigned device)
{
#define SMBUS_HUB 0x18
        int ret, i;
        print_debug("change_i2c_mux i="); print_debug_hex8(device); print_debug("\r\n");
        i=2;
        do {
                ret = smbus_write_byte(SMBUS_HUB, 0x01, device);
                print_debug("change_i2c_mux 1 ret="); print_debug_hex32(ret); print_debug("\r\n");
        } while ((ret!=0) && (i-->0));
        ret = smbus_write_byte(SMBUS_HUB, 0x03, 0);
        print_debug("change_i2c_mux 2 ret="); print_debug_hex32(ret); print_debug("\r\n");
}
#endif

static inline int spd_read_byte(unsigned device, unsigned address)
{
        return smbus_read_byte(device, address);
}

//#include "northbridge/amd/amdk8/setup_resource_map.c"
#define QRANK_DIMM_SUPPORT 1
#include "northbridge/amd/amdk8/raminit.c"

#if 0
        #define ENABLE_APIC_EXT_ID 1
        #define APIC_ID_OFFSET 0x10
        #define LIFT_BSP_APIC_ID 0
#else                   
        #define ENABLE_APIC_EXT_ID 0
#endif
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

/* tyan does not want the default */
#include "resourcemap.c" 

#if CONFIG_LOGICAL_CPUS==1
#define SET_NB_CFG_54 1
#endif

#include "cpu/amd/dualcore/dualcore.c"

#define FIRST_CPU  1
#define SECOND_CPU 1
#define TOTAL_CPUS (FIRST_CPU + SECOND_CPU)

#define RC0 ((1<<0)<<8)
#define RC1 ((1<<1)<<8)

#define DIMM0 0x50
#define DIMM1 0x51

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
                        .channel0 = { RC0|DIMM0, 0, 0, 0 },
                        .channel1 = { RC0|DIMM1, 0, 0, 0 },
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
                        .channel1 = { RC1|DIMM1, 0 , 0, 0 },

                },
#endif

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
                
                id = get_node_core_id_x(); // that is initid
        #if ENABLE_APIC_EXT_ID == 1
                if(id.coreid == 0) {
                        enable_apic_ext_id(id.nodeid);
                }
        #endif
#else           
                nodeid = get_node_id();
        #if ENABLE_APIC_EXT_ID == 1
                enable_apic_ext_id(nodeid);
        #endif
#endif  

	        enable_lapic();
        	init_timer();

#if CONFIG_LOGICAL_CPUS==1
        #if ENABLE_APIC_EXT_ID == 1
            #if LIFT_BSP_APIC_ID == 0
                if( id.nodeid != 0 ) //all except cores in node0
            #endif
                        lapic_write(LAPIC_ID, ( lapic_read(LAPIC_ID) | (APIC_ID_OFFSET<<24) ) );
        #endif

                if(id.coreid == 0) {
                        if (cpu_init_detected(id.nodeid)) {
                                asm volatile ("jmp __cpu_reset");
                        }
                        distinguish_cpu_resets(id.nodeid);
//                        start_other_core(id.nodeid);
                }

#else
        #if ENABLE_APIC_EXT_ID == 1
            #if LIFT_BSP_APIC_ID == 0
                if(nodeid != 0)
            #endif
                        lapic_write(LAPIC_ID, ( lapic_read(LAPIC_ID) | (APIC_ID_OFFSET<<24) ) ); // CPU apicid is from 0x10

        #endif

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

        setup_serengeti_leopard_resource_map();
        needs_reset = setup_coherent_ht_domain();

#if CONFIG_LOGICAL_CPUS==1
	// It is said that we should start core1 after all core0 launched
        start_other_cores();
#endif

#if 0
	// You need to preset bus num in PCI_DEV(0, 0x18,1) 0xe0, 0xe4, 0xe8, 0xec
        needs_reset |= ht_setup_chains(2);
#else
	// automatically set that for you, but you might meet tight space
        needs_reset |= ht_setup_chains_x();
#endif

        if (needs_reset) {
                print_info("ht reset -\r\n");
                soft_reset();
        }

	enable_smbus();
#if 0
	dump_spd_registers(&cpu[0]);
#endif
	memreset_setup();
	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);

#if 0
        dump_pci_devices();
#endif

        /* Check all of memory */
#if 0
        msr_t msr;
        msr = rdmsr(TOP_MEM2);
        print_debug("TOP_MEM2: ");
        print_debug_hex32(msr.hi);
        print_debug_hex32(msr.lo);
        print_debug("\r\n");
#endif

#if  0
        ram_check(0x00000000, msr.lo+(msr.hi<<32));
#endif

#if 0
        // Check 16MB of memory @ 0
        ram_check(0x00000000, 0x00100000);
        // Check 16MB of memory @ 2GB 
        ram_check(0x80000000, 0x80100000);
#endif


}
