/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
 
#define RC0 (6<<8)
#define RC1 (7<<8)

#define DIMM0 0x50
#define DIMM1 0x51

#define ICS951462_ADDRESS	0x69
#define SMBUS_HUB 0x71

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>

#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"

#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "superio/ite/it8712f/early_serial.c"

#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"

#include "northbridge/amd/amdk8/setup_resource_map.c"

#include "southbridge/amd/rs690/early_setup.c"
#include "southbridge/amd/sb600/early_setup.c"
#include "northbridge/amd/amdk8/debug.c" /* After sb600_early_setup.c! */

/* CAN'T BE REMOVED! crt0.S will use it. I don't know WHY!*/
static void memreset(int controllers, const struct mem_controller *ctrl)
{
}

/* called in raminit_f.c */
static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
}

/*called in raminit_f.c */
static inline int spd_read_byte(u32 device, u32 address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/amdk8.h"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/raminit_f.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "lib/generic_sdram.c"
#include "resourcemap.c"
#include "cpu/amd/dualcore/dualcore.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"
#include "cpu/amd/model_fxx/fidvid.c"
#include "northbridge/amd/amdk8/early_ht.c"

#define __WARNING__(fmt, arg...) do_printk(BIOS_WARNING ,fmt, ##arg)
#define __DEBUG__(fmt, arg...) do_printk(BIOS_DEBUG ,fmt, ##arg)
#define __INFO__(fmt, arg...) do_printk(BIOS_INFO ,fmt, ##arg)

#if CONFIG_USE_OPTION_TABLE
#define DUMP_CMOS_RAM 0
static inline int cmos_error(void) {
	unsigned char reg_d;
	/* See if the cmos error condition has been flagged */
	outb(0xd, 0x72);
	reg_d = inb(0x73);
#if DUMP_CMOS_RAM
	__DEBUG__("RTC_VRT = %x\n", reg_d & RTC_VRT);
#endif
	return (reg_d & RTC_VRT) == 0;
}

static inline void set_chksum(int range_start, int range_end, int cks_loc)
{
	int addr;
	unsigned sum;
	sum = 0;
	for( addr = range_start; addr <= range_end; addr++) {
		outb(addr, 0x72);
		sum += inb(0x73);
	}
	sum = ~(sum & 0x0ffff);
	outb(cks_loc, 0x72);
	outb(((sum >> 8) & 0x0ff),0x73);
	outb(cks_loc+1,0x72);
	outb((sum & 0x0ff),0x73);
}

static inline int cmos_chksum_valid(void) {
	unsigned char addr, val;
	unsigned long sum, old_sum;

#if DUMP_CMOS_RAM
	u8 i;
	/* Compute the cmos checksum */
	for (addr = 14, i = 0; addr < LB_CKS_RANGE_START; addr++,i++) {
		outb(addr, 0x72);
		val = inb(0x73);
		if( i%16 == 0 ) __DEBUG__("%02x:", addr);
		__DEBUG__(" %02x",val);
		if( i%16 == 15 ) __DEBUG__("\n");
	}
#endif
	sum = 0;
	for(addr = LB_CKS_RANGE_START; addr <= LB_CKS_RANGE_END; addr++) {
		outb(addr, 0x72);
		val = inb(0x73);
#if DUMP_CMOS_RAM
		if( i%16 == 0 ) __DEBUG__("%02x:", addr);
		__DEBUG__(" %02x",val);
		if( i%16 == 15 ) __DEBUG__("\n");
		i++;
#endif
		sum += val;
	}
#if DUMP_CMOS_RAM
	__DEBUG__("\n");
#endif
	sum = (sum & 0xffff) ^ 0xffff;

	/* Read the stored checksum */
	outb(LB_CKS_LOC, 0x72);
	old_sum = inb(0x73) << 8;
	outb(LB_CKS_LOC+1, 0x72);
	old_sum |=  inb(0x73);
#if DUMP_CMOS_RAM
	__DEBUG__("CMOS checksum: old = %lx, new = %lx\n", old_sum, sum);
#endif
	return sum == old_sum;
}

#include <cbfs.h>
static inline void check_cmos( void ) {

	char *cmos_default = NULL;
	int i;
#if DUMP_CMOS_RAM
	u8 c = 0;
#endif
	if (cmos_error() || !cmos_chksum_valid()) {
		cmos_default = cbfs_find_file("cmos.default", 0xaa);
		if (cmos_default) {
#if DUMP_CMOS_RAM
			__DEBUG__("Write cmos default ...\n");
#endif
			outb(0x0a,0x72);
			i = inb(0x73);
			i &= ~(1 << 4);
			outb(i,0x73);					

			for (i = 14; i < 128; i++) {
#if DUMP_CMOS_RAM
				if( c%16 == 0 ) __DEBUG__("%02x:", i);
				__DEBUG__(" %02x", (u8)cmos_default[i]);
				if( c%16 == 15 ) __DEBUG__("\n");
				c++;
#endif
				outb(i,0x72);
				outb(cmos_default[i],0x73);
			}

#if DUMP_CMOS_RAM
			__DEBUG__("\n");
#endif
			if( !cmos_chksum_valid() )
				__DEBUG__("CMOS CHECKSUM ERROR\n");
			/* Now reboot to run with default cmos. */
			outb(0x06, 0xcf9);
			for (;;) asm("hlt"); /* Wait for reset! */
		} 
	}

	// update altcentury
	outb(0x32, 0x72);
	i = inb(0x73);
	if ( i != 0x20 ) {
		outb(0x20,0x73);
		set_chksum(LB_CKS_RANGE_START, LB_CKS_RANGE_END, LB_CKS_LOC);
	}

}

#endif

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const u16 spd_addr[] = { DIMM0, 0, 0, 0, DIMM1, 0, 0, 0, };
	int needs_reset = 0;
	u32 bsp_apicid = 0;
	msr_t msr;
	struct cpuid_result cpuid1;
	struct sys_info *sysinfo = (struct sys_info *)(CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE);
			
	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		enumerate_ht_chain();

		/* sb600_lpc_port80(); */
		sb600_pci_port80();
	}

	if (bist == 0) {
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);
	}

	enable_rs690_dev8(); // enable CFG access to Dev8, which is the SB P2P Bridge
	sb600_lpc_init();
#if defined(DUMP_CMOS_RAM) && (DUMP_CMOS_RAM == 0)	
	check_cmos();  // rebooting in case of corrupted cmos !!!!!	
#endif	
	/* it8712f_enable_serial does not use its 1st parameter. */
	it8712f_enable_serial(0, CONFIG_TTYS0_BASE);
	it8712f_kill_watchdog(); 

	console_init();
#if defined(DUMP_CMOS_RAM) && (DUMP_CMOS_RAM == 1)
	check_cmos();  // rebooting in case of corrupted cmos !!!!!		
#endif
	post_code(0x03);
	
	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);
	__DEBUG__("bsp_apicid=0x%x\n", bsp_apicid);

	setup_sitemp_resource_map();

	setup_coherent_ht_domain();

#if CONFIG_LOGICAL_CPUS==1
	/* It is said that we should start core1 after all core0 launched */
	wait_all_core0_started();
	start_other_cores();
#endif
	wait_all_aps_started(bsp_apicid);

	ht_setup_chains_x(sysinfo);

	/* run _early_setup before soft-reset. */
	rs690_early_setup();
	sb600_early_setup();
	
	post_code(0x04);
	
	/* Check to see if processor is capable of changing FIDVID  */
	/* otherwise it will throw a GP# when reading FIDVID_STATUS */
	cpuid1 = cpuid(0x80000007);
	if( (cpuid1.edx & 0x6) == 0x6 ) {

		/* Read FIDVID_STATUS */
		msr=rdmsr(0xc0010042);
		__DEBUG__("begin msr fid, vid: hi=0x%x, lo=0x%x\n", msr.hi, msr.lo);

		enable_fid_change();
		enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
		init_fidvid_bsp(bsp_apicid);

		/* show final fid and vid */
		msr=rdmsr(0xc0010042);
		__DEBUG__("end msr fid, vid: hi=0x%x, lo=0x%x\n", msr.hi, msr.lo);

	} else {
		__DEBUG__("Changing FIDVID not supported\n");
	}
	
	post_code(0x05);
	
	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);
	rs690_htinit();
	__DEBUG__("needs_reset=0x%x\n", needs_reset);
	
	post_code(0x06);
	
	if (needs_reset) {
		__INFO__("ht reset -\n");
		soft_reset();
	}

	allow_all_aps_stop(bsp_apicid);

	/* It's the time to set ctrl now; */
	__DEBUG__("sysinfo->nodes: %2x  sysinfo->ctrl: %p  spd_addr: %p\n",
		     sysinfo->nodes, sysinfo->ctrl, spd_addr);
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);
	
	post_code(0x07);
	
	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);
	
	post_code(0x08);
	
	rs690_before_pci_init(); // does nothing
	sb600_before_pci_init();
	
#if CONFIG_USE_OPTION_TABLE	
	if( read_option(cmos_defaults_loaded, 0) )
		__WARNING__("WARNING: CMOS DEFAULTS LOADED. PLEASE CHECK CMOS OPTION \"cmos_default_loaded\" !\n"); 
#endif

	post_cache_as_ram();
}

