#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <lib.h>
#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "southbridge/amd/amd8111/early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "lib/delay.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "superio/winbond/w83627hf/early_serial.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include "southbridge/amd/amd8111/early_ctrl.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

static void memreset_setup(void)
{
   if (is_cpu_pre_c0())
        outb((1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 16);  //REVC_MEMRST_EN=0
   else
        outb((1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 16);  //REVC_MEMRST_EN=1
   outb((1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 17);
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
   if (is_cpu_pre_c0()) {
        udelay(800);
        outb((1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 17); //REVB_MEMRST_L=1
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
        print_debug("change_i2c_mux i="); print_debug_hex8(device); print_debug("\n");
        i=2;
        do {
                ret = smbus_write_byte(SMBUS_HUB, 0x01, device);
                print_debug("change_i2c_mux 1 ret="); print_debug_hex32(ret); print_debug("\n");
        } while ((ret!=0) && (i-->0));
        ret = smbus_write_byte(SMBUS_HUB, 0x03, 0);
        print_debug("change_i2c_mux 2 ret="); print_debug_hex32(ret); print_debug("\n");
}
#endif

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "lib/generic_sdram.c"
#include "resourcemap.c"
#include "cpu/amd/dualcore/dualcore.c"
#include <spd.h>
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"

#define RC0 ((1<<2)<<8)
#define RC1 ((1<<1)<<8)
#define RC2 ((1<<4)<<8)
#define RC3 ((1<<3)<<8)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr [] = {
                        RC0|DIMM0, RC0|DIMM2, 0, 0,
                        RC0|DIMM1, RC0|DIMM3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
                        RC1|DIMM0, RC1|DIMM2, 0, 0,
                        RC1|DIMM1, RC1|DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 2
                        RC2|DIMM0, RC2|DIMM2, 0, 0,
                        RC2|DIMM1, RC2|DIMM3, 0, 0,
                        RC3|DIMM0, RC3|DIMM2, 0, 0,
                        RC3|DIMM1, RC3|DIMM3, 0, 0,
#endif
	};

        int needs_reset;
        unsigned bsp_apicid = 0, nodes;
        struct mem_controller ctrl[8];

        if (bist == 0)
                bsp_apicid = init_cpus(cpu_init_detectedx);

 	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
        console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

        setup_s4882_resource_map();

	needs_reset = setup_coherent_ht_domain();

        wait_all_core0_started();
#if CONFIG_LOGICAL_CPUS
        // It is said that we should start core1 after all core0 launched
        start_other_cores();
        wait_all_other_cores_started(bsp_apicid);
#endif

	// automatically set that for you, but you might meet tight space
        needs_reset |= ht_setup_chains_x();

       	if (needs_reset) {
               	print_info("ht reset -\n");
               	soft_reset();
       	}

        allow_all_aps_stop(bsp_apicid);

        nodes = get_nodes();
        //It's the time to set ctrl now;
        fill_mem_ctrl(nodes, ctrl, spd_addr);

	enable_smbus();

	memreset_setup();
	sdram_initialize(nodes, ctrl);

	post_cache_as_ram();
}
