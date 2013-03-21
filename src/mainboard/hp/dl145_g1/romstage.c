#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/amd/model_fxx_rev.h>
#include <delay.h>
#include "northbridge/amd/amdk8/amdk8.h"
#include "southbridge/amd/amd8111/early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "superio/winbond/w83627hf/early_serial.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "southbridge/amd/amd8111/early_ctrl.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

static void memreset_setup(void)
{
	if (is_cpu_pre_c0()) {
		/* Set the memreset low. */
		outb((1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 16);
		/* Ensure the BIOS has control of the memory lines. */
		outb((1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 17);
	} else {
		/* Ensure the CPU has control of the memory lines. */
		outb((1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 17);
	}
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
	if (is_cpu_pre_c0()) {
		udelay(800);
		/* Set memreset high. */
		outb((1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 16);
		udelay(90);
	}
}

#define SMBUS_HUB 0x18

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
	int ret,i;
	unsigned device=(ctrl->channel0[0])>>8;
	/* the very first write always get COL_STS=1 and ABRT_STS=1, so try another time*/
	i=2;
	do {
		ret = smbus_write_byte(SMBUS_HUB, 0x01, device);
	} while ((ret!=0) && (i-->0));
	smbus_write_byte(SMBUS_HUB, 0x03, 0);
}

static inline void change_i2c_mux(unsigned device)
{
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

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/raminit.c"
#include "resourcemap.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/dualcore/dualcore.c"
#include <spd.h>
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"

#define RC0 ((1<<1)<<8)
#define RC1 ((1<<2)<<8)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr [] = {
		//first node
		RC0|DIMM0, RC0|DIMM2, 0, 0,
		RC0|DIMM1, RC0|DIMM3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
		//second node
		RC1|DIMM0, RC1|DIMM2, 0, 0,
		RC1|DIMM1, RC1|DIMM3, 0, 0,
#endif
	};
	struct sys_info *sysinfo = &sysinfo_car;

	int needs_reset = 0;
	unsigned bsp_apicid = 0;

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx,sysinfo);

	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);

	setup_dl145g1_resource_map();
	//setup_default_resource_map();

#if CONFIG_MEM_TRAIN_SEQ == 1
	set_sysinfo_in_ram(0); // in BSP so could hold all ap until sysinfo is in ram
#endif
	setup_coherent_ht_domain();
	wait_all_core0_started();
#if CONFIG_LOGICAL_CPUS
	// It is said that we should start core1 after all core0 launched
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif

	ht_setup_chains_x(sysinfo);

	needs_reset |= optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);

	if (needs_reset) {
		print_info("ht reset -\n");
		soft_reset_x(sysinfo->sbbusn, sysinfo->sbdn);
	}

	enable_smbus();

	int i;
	for(i=0;i<2;i++) {
		activate_spd_rom(&sysinfo->ctrl[i]);
	}
	for(i=RC0;i<=RC1;i<<=1) {
		change_i2c_mux(i);
	}

	//dump_spd_registers(&sysinfo->ctrl[0]);
	//dump_spd_registers(&sysinfo->ctrl[1]);
	//dump_smbus_registers();

	allow_all_aps_stop(bsp_apicid);

	//It's the time to set ctrl now;
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	memreset_setup();
	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

	//dump_pci_devices();

	post_cache_as_ram();
}
