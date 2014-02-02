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
#include <northbridge/amd/amdk8/raminit.h>
#include "northbridge/amd/amdk8/reset_test.c"
#include <cpu/x86/bist.h>
#include <delay.h>
#include "northbridge/amd/amdk8/debug.c"
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627hf/w83627hf.h>
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include "southbridge/amd/amd8111/early_ctrl.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

/*
 * GPIO28 of 8111 will control H0_MEMRESET_L
 * GPIO29 of 8111 will control H1_MEMRESET_L
 */
static void memreset_setup(void)
{
	if (is_cpu_pre_c0()) {
		/* Set the memreset low. */
		outb((1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 28);
		/* Ensure the BIOS has control of the memory lines. */
		outb((1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 29);
	} else {
		/* Ensure the CPU has control of the memory lines. */
		outb((1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 29);
	}
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
	if (is_cpu_pre_c0()) {
		udelay(800);
		/* Set memreset high. */
		outb((1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 28);
		udelay(90);
	}
}

static void activate_spd_rom(const struct mem_controller *ctrl) { }

static inline int spd_read_byte(unsigned device, unsigned address)
{
        return smbus_read_byte(device, address);
}

#include <northbridge/amd/amdk8/amdk8.h>
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/raminit.c"
#include "lib/generic_sdram.c"
#include "northbridge/amd/amdk8/resourcemap.c"
#include "cpu/amd/dualcore/dualcore.c"
#include <spd.h>
#include "cpu/amd/model_fxx/init_cpus.c"
#include "cpu/amd/model_fxx/fidvid.c"

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr[] = {
		// first node
		DIMM0, DIMM2, 0, 0,
		DIMM1, DIMM3, 0, 0,

			// second node
		DIMM4, DIMM6, 0, 0,
		DIMM5, DIMM7, 0, 0,
	};

	struct sys_info *sysinfo = &sysinfo_car;
        int needs_reset;
        unsigned bsp_apicid = 0;

        if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);

        setup_default_resource_map();

	printk(BIOS_DEBUG, "bsp_apicid=%02x\n", bsp_apicid);

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

#if CONFIG_SET_FIDVID
        {
                msr_t msr;
	        msr=rdmsr(0xc0010042);
                printk(BIOS_DEBUG, "begin msr fid, vid %08x%08x\n", msr.hi, msr.lo);
        }
	enable_fid_change();
	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
        init_fidvid_bsp(bsp_apicid);
        // show final fid and vid
        {
                msr_t msr;
               	msr=rdmsr(0xc0010042);
               	printk(BIOS_DEBUG, "end   msr fid, vid %08x%08x\n", msr.hi, msr.lo);
        }
#endif

	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);

        // fidvid change will issue one LDTSTOP and the HT change will be effective too
        if (needs_reset) {
                printk(BIOS_INFO, "ht reset -\n");
                soft_reset_x(sysinfo->sbbusn, sysinfo->sbdn);
        }

	allow_all_aps_stop(bsp_apicid);

        //It's the time to set ctrl in sysinfo now;
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	enable_smbus();

#if 0
	dump_smbus_registers();
#endif

	memreset_setup();

	//do we need apci timer, tsc...., only debug need it for better output
        /* all ap stopped? */
        init_timer(); // Need to use TMICT to synchronize FID/VID
	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

#if 0
        dump_pci_devices();
#endif

        post_cache_as_ram(); // bsp swtich stack to ram and copy sysinfo ram now
}
