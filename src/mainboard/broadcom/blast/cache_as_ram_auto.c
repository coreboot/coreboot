#define ASSEMBLY 1
#define __ROMCC__


//#define K8_SCAN_PCI_BUS 1

#define K8_4RANK_DIMM_SUPPORT 1

#if CONFIG_LOGICAL_CPUS==1
#define SET_NB_CFG_54 1
#endif
 
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

#if 0
static void post_code(uint8_t value) {
#if 0
        int i;
        for(i=0;i<0x80000;i++) {
                outb(value, 0x80);
        }
#endif
}
#endif

#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "southbridge/broadcom/bcm5785/bcm5785_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"

#if CONFIG_USE_INIT == 0
#include "lib/memcpy.c"
#endif

#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "superio/NSC/pc87417/pc87417_early_serial.c"

#include "cpu/amd/mtrr/amd_earlymtrr.c"
#include "cpu/x86/bist.h"

#include "northbridge/amd/amdk8/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, PC87417_SP1)
#define RTC_DEV PNP_DEV(0x2e, PC87417_RTC)

#include "southbridge/broadcom/bcm5785/bcm5785_early_setup.c"

static void memreset_setup(void)
{
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
}

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_HUB 0x71
        int ret,i;
        unsigned device=(ctrl->channel0[0])>>8;
        smbus_send_byte(SMBUS_HUB, device);
}
#if 0
static inline void change_i2c_mux(unsigned device)
{
#define SMBUS_HUB 0x71
	int ret;
        print_debug("change_i2c_mux i="); print_debug_hex8(device); print_debug("\r\n");
        ret = smbus_send_byte(SMBUS_HUB, device);
        print_debug("change_i2c_mux ret="); print_debug_hex32(ret); print_debug("\r\n");
}
#endif

static inline int spd_read_byte(unsigned device, unsigned address)
{
        return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

 /* tyan does not want the default */
#include "resourcemap.c" 

#include "cpu/amd/dualcore/dualcore.c"

#define RC0 (6<<8)
#define RC1 (7<<8)

#define DIMM0 0x50
#define DIMM1 0x51
#define DIMM2 0x52
#define DIMM3 0x53

#include "cpu/amd/car/copy_and_run.c"
#include "cpu/amd/car/post_cache_as_ram.c"

#include "cpu/amd/model_fxx/init_cpus.c"


#if USE_FALLBACK_IMAGE == 1

#include "northbridge/amd/amdk8/early_ht.c"

void failover_process(unsigned long bist, unsigned long cpu_init_detectedx)
{


        /* Is this a cpu only reset? Is this a secondary cpu? */
        if ((cpu_init_detectedx) || (!boot_cpu())) {  
                if (last_boot_normal()) { // RTC already inited
                        goto normal_image;
                } else {
                        goto fallback_image;
                }
        }
        /* Nothing special needs to be done to find bus 0 */
        /* Allow the HT devices to be found */

        enumerate_ht_chain();

        bcm5785_enable_rom();

        bcm5785_enable_lpc();

        //enable RTC
        pc87417_enable_dev(RTC_DEV);

        /* Is this a deliberate reset by the bios */
//        post_code(0x22);
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
//        post_code(0x23);
        __asm__ volatile ("jmp __normal_image"
                : /* outputs */
                : "a" (bist), "b" (cpu_init_detectedx) /* inputs */
                );

 fallback_image:
//        post_code(0x25);
	;
}
#endif /* USE_FALLBACK_IMAGE == 1 */

void real_main(unsigned long bist, unsigned long cpu_init_detectedx);

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{

#if USE_FALLBACK_IMAGE == 1
        failover_process(bist, cpu_init_detectedx);
#endif
        real_main(bist, cpu_init_detectedx);

}

void real_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr[] = {
        	RC0|DIMM0, RC0|DIMM2, 0, 0,
                RC0|DIMM1, RC0|DIMM3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
                RC1|DIMM0, RC1|DIMM2, 0, 0,
                RC1|DIMM1, RC1|DIMM3, 0, 0,
#endif
	};

        int needs_reset;
	unsigned bsp_apicid = 0;

        struct mem_controller ctrl[8];
        unsigned nodes;

        if (bist == 0) {
		bsp_apicid = init_cpus(cpu_init_detectedx);
        }
//	post_code(0x32);

	pc87417_enable_serial(SERIAL_DEV, TTYS0_BASE);
//	post_code(0x33);
	
        uart_init();
//	post_code(0x34);

        console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	print_debug("bsp_apicid="); print_debug_hex8(bsp_apicid); print_debug("\r\n");

        setup_blast_resource_map();
	
#if 0
        dump_pci_device(PCI_DEV(0, 0x18, 0));
	dump_pci_device(PCI_DEV(0, 0x19, 0));
#endif

	needs_reset = setup_coherent_ht_domain();

#if CONFIG_LOGICAL_CPUS==1
        // It is said that we should start core1 after all core0 launched
        wait_all_core0_started();
        start_other_cores();
#endif
        wait_all_aps_started(bsp_apicid);

        needs_reset |= ht_setup_chains_x();

	bcm5785_early_setup();

       	if (needs_reset) {
               	print_info("ht reset -\r\n");
               	soft_reset();
       	}

	allow_all_aps_stop(bsp_apicid);

        nodes = get_nodes();
        //It's the time to set ctrl now;
        fill_mem_ctrl(nodes, ctrl, spd_addr);

	enable_smbus();

#if 0 
        int i;
        for(i=4;i<8;i++) {
                change_i2c_mux(i);
                dump_smbus_registers();
        }
#endif

	memreset_setup();

//	init_timer();

	sdram_initialize(nodes, ctrl);

#if 0
        print_pci_devices();
#endif

#if 0
	dump_pci_devices();
#endif

	post_cache_as_ram();

}
