#if CONFIG_K8_REV_F_SUPPORT
#define K8_REV_F_SUPPORT_F0_F1_WORKAROUND 0
#endif

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/amd/model_fxx_rev.h>
#include "southbridge/amd/amd8111/early_smbus.c"
#include <reset.h>
#include "northbridge/amd/amdk8/raminit.h"
#include "northbridge/amd/amdk8/reset_test.c"
#include "cpu/x86/bist.h"
#include "lib/delay.c"
#include "northbridge/amd/amdk8/debug.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include <cpu/amd/mtrr.h>
#include "superio/winbond/w83627hf/early_serial.c"
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include "southbridge/amd/amd8111/early_ctrl.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

static void memreset_setup(void)
{
	//GPIO on amd8111 to enable MEMRST ????
        outb((1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 16);  //REVC_MEMRST_EN=1
        outb((1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 17);
}

static void memreset(int controllers, const struct mem_controller *ctrl) { }

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

#include "northbridge/amd/amdk8/amdk8.h"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/raminit_f.c"
#include "lib/generic_sdram.c"
#include "resourcemap.c"
#include "cpu/amd/dualcore/dualcore.c"
#include <spd.h>
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"
#include "cpu/amd/model_fxx/fidvid.c"

#define RC0 ((1<<0)<<8)
#define RC1 ((1<<1)<<8)
#define RC2 ((1<<2)<<8)
#define RC3 ((1<<3)<<8)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr[] = {
			//first node
                        RC0|DIMM0, RC0|DIMM2, 0, 0,
                        RC0|DIMM1, RC0|DIMM3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
			//second node
                        RC1|DIMM0, RC1|DIMM2, RC1|DIMM4, RC1|DIMM6,
                        RC1|DIMM1, RC1|DIMM3, RC1|DIMM5, RC1|DIMM7,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 2
                        // third node
                        RC2|DIMM0, RC2|DIMM2, 0, 0,
                        RC2|DIMM1, RC2|DIMM3, 0, 0,
                        // four node
                        RC3|DIMM0, RC3|DIMM2, RC3|DIMM4, RC3|DIMM6,
                        RC3|DIMM1, RC3|DIMM3, RC3|DIMM5, RC3|DIMM7,
#endif

	};

	struct sys_info *sysinfo = &sysinfo_car;
        int needs_reset;
        unsigned bsp_apicid = 0;
#if CONFIG_SET_FIDVID
	struct cpuid_result cpuid1;
#endif

        if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

 	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
        console_init();

//	dump_mem(CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE-0x200, CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE);

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);

        setup_mb_resource_map();
#if 0
        dump_pci_device(PCI_DEV(0, 0x18, 0));
	dump_pci_device(PCI_DEV(0, 0x19, 0));
#endif

	print_debug("bsp_apicid="); print_debug_hex8(bsp_apicid); print_debug("\n");

#if CONFIG_MEM_TRAIN_SEQ == 1
        set_sysinfo_in_ram(0); // in BSP so could hold all ap until sysinfo is in ram
#endif
	setup_coherent_ht_domain(); // routing table and start other core0

	wait_all_core0_started();
#if CONFIG_LOGICAL_CPUS
        // It is said that we should start core1 after all core0 launched
	/* becase optimize_link_coherent_ht is moved out from setup_coherent_ht_domain,
	 * So here need to make sure last core0 is started, esp for two way system,
	 * (there may be apic id conflicts in that case)
	 */
        start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif

	/* it will set up chains and store link pair for optimization later */
        ht_setup_chains_x(sysinfo); // it will init sblnk and sbbusn, nodes, sbdn

#if 0
	//it your CPU min fid is 1G, you can change HT to 1G and FID to max one time.
        needs_reset = optimize_link_coherent_ht();
        needs_reset |= optimize_link_incoherent_ht(sysinfo);
#endif

#if CONFIG_SET_FIDVID
	/* Check to see if processor is capable of changing FIDVID  */
	/* otherwise it will throw a GP# when reading FIDVID_STATUS */
	cpuid1 = cpuid(0x80000007);
	if ((cpuid1.edx & 0x6) == 0x6) {

        {
		/* Read FIDVID_STATUS */
                msr_t msr;
                msr=rdmsr(0xc0010042);
                print_debug("begin msr fid, vid "); print_debug_hex32( msr.hi ); print_debug_hex32(msr.lo); print_debug("\n");
        }

	enable_fid_change();
	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
        init_fidvid_bsp(bsp_apicid);

        // show final fid and vid
        {
                msr_t msr;
                msr=rdmsr(0xc0010042);
                print_debug("end   msr fid, vid "); print_debug_hex32( msr.hi ); print_debug_hex32(msr.lo); print_debug("\n");
        }

	} else {
		print_debug("Changing FIDVID not supported\n");
	}
#endif

#if 1
	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);

        // fidvid change will issue one LDTSTOP and the HT change will be effective too
        if (needs_reset) {
                print_info("ht reset -\n");
                soft_reset_x(sysinfo->sbbusn, sysinfo->sbdn);
        }
#endif
	allow_all_aps_stop(bsp_apicid);

        //It's the time to set ctrl in sysinfo now;
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	enable_smbus();

#if 0
	int i;
	for(i=0;i<4;i++) {
		activate_spd_rom(&cpu[i]);
		dump_smbus_registers();
	}
#endif

#if 0
        for(i=1;i<256;i<<=1) {
                change_i2c_mux(i);
                dump_smbus_registers();
        }
#endif

	memreset_setup();

	//do we need apci timer, tsc...., only debug need it for better output
        /* all ap stopped? */
//        init_timer(); // Need to use TMICT to synconize FID/VID

	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

#if 0
        print_pci_devices();
#endif

#if 0
//        dump_pci_devices();
        dump_pci_device_index_wait(PCI_DEV(0, 0x18, 2), 0x98);
	dump_pci_device_index_wait(PCI_DEV(0, 0x19, 2), 0x98);
#endif

        post_cache_as_ram(); // bsp swtich stack to ram and copy sysinfo ram now
}
