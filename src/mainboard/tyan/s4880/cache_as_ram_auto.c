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

#include <cpu/amd/model_fxx_rev.h>
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

#include "southbridge/amd/amd8111/amd8111_early_ctrl.c"

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
        int ret;
        unsigned device=(ctrl->channel0[0])>>8;
        smbus_write_byte(SMBUS_HUB, 0x01, device);
        smbus_write_byte(SMBUS_HUB, 0x03, 0);
}
#if 0
static inline void change_i2c_mux(unsigned device)
{
#define SMBUS_HUB 0x18
        int ret;
        print_debug("change_i2c_mux i="); print_debug_hex8(device); print_debug("\r\n"); 
        ret = smbus_write_byte(SMBUS_HUB, 0x01, device);
        print_debug("change_i2c_mux 1 ret="); print_debug_hex32(ret); print_debug("\r\n");
        ret = smbus_write_byte(SMBUS_HUB, 0x03, 0);
        print_debug("change_i2c_mux 2 ret="); print_debug_hex32(ret); print_debug("\r\n");
}
#endif

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#define K8_4RANK_DIMM_SUPPORT 1

#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

 /* tyan does not want the default */
#include "resourcemap.c" 

#if CONFIG_LOGICAL_CPUS==1
#define SET_NB_CFG_54 1
#endif
#include "cpu/amd/dualcore/dualcore.c"

#define RC0 ((1<<2)<<8)
#define RC1 ((1<<1)<<8)
#define RC2 ((1<<4)<<8)
#define RC3 ((1<<3)<<8)

#define DIMM0 0x50
#define DIMM1 0x51
#define DIMM2 0x52
#define DIMM3 0x53

#include "cpu/amd/car/copy_and_run.c"

#include "cpu/amd/car/post_cache_as_ram.c"

#include "cpu/amd/model_fxx/init_cpus.c"


#if USE_FALLBACK_IMAGE == 1

#include "southbridge/amd/amd8111/amd8111_enable_rom.c"
#include "northbridge/amd/amdk8/early_ht.c"

void failover_process(unsigned long bist, unsigned long cpu_init_detectedx)
{
        unsigned last_boot_normal_x = last_boot_normal();

        /* Is this a cpu only reset? or Is this a secondary cpu? */
        if ((cpu_init_detectedx) || (!boot_cpu())) {
                if (last_boot_normal_x) {
                        goto normal_image;
                } else {
                        goto fallback_image;
                }
        }

        /* Nothing special needs to be done to find bus 0 */
        /* Allow the HT devices to be found */

        enumerate_ht_chain();

        amd8111_enable_rom();

        /* Is this a deliberate reset by the bios */
        if (bios_reset_detected() && last_boot_normal_x) {
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
                : "a" (bist), "b" (cpu_init_detectedx) /* inputs */
                );

 fallback_image:
	;
}
#endif

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
	static const struct mem_controller cpu[] = {
                {
                        .node_id = 0,
                        .f0 = PCI_DEV(0, 0x18, 0),
                        .f1 = PCI_DEV(0, 0x18, 1),
                        .f2 = PCI_DEV(0, 0x18, 2),
                        .f3 = PCI_DEV(0, 0x18, 3),
                        .channel0 = { RC0|DIMM0, RC0|DIMM2, 0, 0 },
                        .channel1 = { RC0|DIMM1, RC0|DIMM3, 0, 0 },
                },
#if CONFIG_MAX_PHYSICAL_CPUS > 1
                {
                        .node_id = 1,
                        .f0 = PCI_DEV(0, 0x19, 0),
                        .f1 = PCI_DEV(0, 0x19, 1),
                        .f2 = PCI_DEV(0, 0x19, 2),
                        .f3 = PCI_DEV(0, 0x19, 3),
                        .channel0 = { RC1|DIMM0, RC1|DIMM2 , 0, 0 },
                        .channel1 = { RC1|DIMM1, RC1|DIMM3 , 0, 0 },

                },
#endif

#if CONFIG_MAX_PHYSICAL_CPUS > 2
                {
                        .node_id = 2,
                        .f0 = PCI_DEV(0, 0x1a, 0),
                        .f1 = PCI_DEV(0, 0x1a, 1),
                        .f2 = PCI_DEV(0, 0x1a, 2),
                        .f3 = PCI_DEV(0, 0x1a, 3),
                        .channel0 = { RC2|DIMM0, RC2|DIMM2, 0, 0 },
                        .channel1 = { RC2|DIMM1, RC2|DIMM3, 0, 0 },

                },
                {
                        .node_id = 3,
                        .f0 = PCI_DEV(0, 0x1b, 0),
                        .f1 = PCI_DEV(0, 0x1b, 1),
                        .f2 = PCI_DEV(0, 0x1b, 2),
                        .f3 = PCI_DEV(0, 0x1b, 3),
                        .channel0 = { RC3|DIMM0, RC3|DIMM2, 0, 0 },
                        .channel1 = { RC3|DIMM1, RC3|DIMM3, 0, 0 },

                },
#endif
	};

        int needs_reset;

        if (bist == 0) {
		init_cpus(cpu_init_detectedx);
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
	// automatically set that for you, but you might meet tight space
        needs_reset |= ht_setup_chains_x();

       	if (needs_reset) {
               	print_info("ht reset -\r\n");
               	soft_reset();
       	}

	enable_smbus();

	memreset_setup();
	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);

	post_cache_as_ram();
}
