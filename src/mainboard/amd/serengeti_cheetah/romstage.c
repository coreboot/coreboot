/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
#include <northbridge/amd/amdk8/raminit.h>
#include "northbridge/amd/amdk8/reset_test.c"
#include <cpu/x86/bist.h>
#include <delay.h>
#include "northbridge/amd/amdk8/debug.c"
#include <cpu/amd/mtrr.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627hf/w83627hf.h>
#include "northbridge/amd/amdk8/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

unsigned get_sbdn(unsigned bus);

static void memreset_setup(void)
{
	/* GPIO on amd8111 to enable MEMRST ???? */
	outb((1 << 2)|(1 << 0), SMBUS_IO_BASE + 0xc0 + 16);  /* REVC_MEMRST_EN = 1 */
	outb((1 << 2)|(0 << 0), SMBUS_IO_BASE + 0xc0 + 17);
}

static void memreset(int controllers, const struct mem_controller *ctrl) { }

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_HUB 0x18
	int ret,i;
	unsigned device=(ctrl->channel0[0])>>8;
	/* the very first write always get COL_STS = 1 and ABRT_STS = 1, so try another time */
	i = 2;
	do {
		ret = smbus_write_byte(SMBUS_HUB, 0x01, device);
	} while ((ret != 0) && (i-->0));

	smbus_write_byte(SMBUS_HUB, 0x03, 0);
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "southbridge/amd/amd8111/early_ctrl.c"
#include <northbridge/amd/amdk8/amdk8.h>
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/raminit_f.c"
#include "lib/generic_sdram.c"
#include "resourcemap.c"
#include "cpu/amd/dualcore/dualcore.c"
#include <spd.h>
#include "cpu/amd/model_fxx/init_cpus.c"
#include "cpu/amd/model_fxx/fidvid.c"

#define RC0 ((1 << 0)<<8)
#define RC1 ((1 << 1)<<8)
#define RC2 ((1 << 2)<<8)
#define RC3 ((1 << 3)<<8)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr[] = {
			/* first node */
			RC0|DIMM0, RC0|DIMM2, 0, 0,
			RC0|DIMM1, RC0|DIMM3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
			/* second node */
			RC1|DIMM0, RC1|DIMM2, RC1|DIMM4, RC1|DIMM6,
			RC1|DIMM1, RC1|DIMM3, RC1|DIMM5, RC1|DIMM7,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 2
			/*  third node */
			RC2|DIMM0, RC2|DIMM2, 0, 0,
			RC2|DIMM1, RC2|DIMM3, 0, 0,
			/*  four node */
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

	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);

	setup_mb_resource_map();

	printk(BIOS_DEBUG, "bsp_apicid=%02x\n", bsp_apicid);

	set_sysinfo_in_ram(0); /* in BSP so could hold all ap until sysinfo is in ram */
	setup_coherent_ht_domain(); /* routing table and start other core0 */

	wait_all_core0_started();
#if CONFIG_LOGICAL_CPUS
	/* It is said that we should start core1 after all core0 launched */
	/* becase optimize_link_coherent_ht is moved out from setup_coherent_ht_domain,
	 * So here need to make sure last core0 is started, esp for two way system,
	 * (there may be apic id conflicts in that case)
	 */
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif

	/* it will set up chains and store link pair for optimization later */
	ht_setup_chains_x(sysinfo); /* it will init sblnk and sbbusn, nodes, sbdn */

#if CONFIG_SET_FIDVID
	/* Check to see if processor is capable of changing FIDVID  */
	/* otherwise it will throw a GP# when reading FIDVID_STATUS */
	cpuid1 = cpuid(0x80000007);
	if ((cpuid1.edx & 0x6) == 0x6) {

	{
		/* Read FIDVID_STATUS */
		msr_t msr;
		msr = rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "begin msr fid, vid %08x%08x\n", msr.hi, msr.lo);
	}

	enable_fid_change();
	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
	init_fidvid_bsp(bsp_apicid);

	/* show final fid and vid */
	{
		msr_t msr;
		msr = rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "end   msr fid, vid %08x%08x\n", msr.hi, msr.lo);
	}

	} else {
		printk(BIOS_DEBUG, "Changing FIDVID not supported\n");
	}
#endif

#if 1
	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);

	/* fidvid change will issue one LDTSTOP and the HT change will be effective too */
	if (needs_reset) {
		printk(BIOS_INFO, "ht reset -\n");
		soft_reset_x(sysinfo->sbbusn, sysinfo->sbdn);
	}
#endif
	allow_all_aps_stop(bsp_apicid);

	/* It's the time to set ctrl in sysinfo now; */
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	enable_smbus();

	memreset_setup();

	/* do we need apci timer, tsc...., only debug need it for better output */
	/* all ap stopped? */
	/* Need to use TMICT to synchronize FID/VID */

	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

	post_cache_as_ram(); /* bsp swtich stack to RAM and copy sysinfo RAM now */
}
