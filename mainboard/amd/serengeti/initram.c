/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#define _MAINOBJECT

#include <mainboard.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <cpu.h>
#include <amd/k8/k8.h>
#include <mc146818rtc.h>
#include <spd.h>

#define RC0 ((1<<0)<<8)
#define RC1 ((1<<1)<<8)
#define RC2 ((1<<2)<<8)
#define RC3 ((1<<3)<<8)

#define DIMM0 0x50
#define DIMM1 0x51
#define DIMM2 0x52
#define DIMM3 0x53
#define DIMM4 0x54
#define DIMM5 0x55
#define DIMM6 0x56
#define DIMM7 0x57

# warning fix hard_reset
void hard_reset(void)
{
}
void memreset_setup(void)
{
}

void memreset(int controllers, const struct mem_controller *ctrl)
{
}

void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_HUB 0x18
	 int smbus_write_byte(u16 device, u16 address, u8 val);
       int ret,i;
        u16 device=(ctrl->channel0[0])>>8;
        /* the very first write always get COL_STS=1 and ABRT_STS=1, so try another time*/
        i=2;
        do {
                ret = smbus_write_byte(SMBUS_HUB, 0x01, device);
        } while ((ret!=0) && (i-->0));

        smbus_write_byte(SMBUS_HUB, 0x03, 0);
}

u8 spd_read_byte(u16 device, u8 address)
{
	int smbus_read_byte(u16 device, u16 address);
        return smbus_read_byte(device, address);
}


/** 
  * main for initram for the AMD Serengeti
 * @param init_detected Used to indicate that we have been started via init
 * @returns 0 on success
 * The purpose of this code is to not only get ram going, but get any other cpus/cores going. 
 * The two activities are very tightly connected and not really seperable. 
 * The BSP (boot strap processor) Core 0 (BSC) is responsible for all training or all sockets. Note that
 * this need not be socket 0; one great strength of coreboot, as opposed to other BIOSes, is that it could
 * always boot with with a CPU in any socket, and even with empty sockets (as opposed to, e.g., the BIOS
 * that came installed on the Sun Ultra 40, which would freeze if one CPU were not installed).
 * The bringup proceeds in several sections. The cool part is that this code is run by all CPUs, and
 * control flow is managed by seeing which CPU we are -- BSP or AP? 
 * 
  */
/* 
 * init_detected is used to determine if we did a soft reset as required by a reprogramming of the 
 * hypertransport links. If we did this kind of reset, bit 11 will be set in the MTRRdefType_MSR MSR. 
 * That may seem crazy, but there are not lots of places to hide a bit when the CPU does a reset. 
 * This value is picked up in assembly, or it should be. 
 */
int main(void)
{
	/* sure, we could put this in a .h. It's called precisely once, from this one 
	 * place. And it only relates to the initram stage. I think I'll leave it here. 
	 * That way we can see the definition without grepping the source tree. 
	 */
	void enable_smbus(void);
	u32 init_detected;
	static const u16 spd_addr[] = {
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

	struct sys_info *sysinfo;
        int needs_reset; int i;
        unsigned bsp_apicid = 0;
	printk(BIOS_DEBUG, "Hi there from stage1\n");
	post_code(POST_START_OF_MAIN);
	sysinfo = &(global_vars()->sys_info);

	/* well, here we are. For starters, we need to know if this is cpu0 core0. 
	 * cpu0 core 0 will do all the DRAM setup. 
	 */
	bsp_apicid = init_cpus(init_detected, sysinfo);

//	dump_mem(DCACHE_RAM_BASE+DCACHE_RAM_SIZE-0x200, DCACHE_RAM_BASE+DCACHE_RAM_SIZE);

#if 0
        dump_pci_device(PCI_DEV(0, 0x18, 0));
	dump_pci_device(PCI_DEV(0, 0x19, 0));
#endif

	printk(BIOS_DEBUG, "bsp_apicid=%02x\n", bsp_apicid);

#if MEM_TRAIN_SEQ == 1
        set_sysinfo_in_ram(0); // in BSP so could hold all ap until sysinfo is in ram 
#endif
	setup_coherent_ht_domain(); // routing table and start other core0

	wait_all_core0_started();
#if CONFIG_LOGICAL_CPUS==1
        // It is said that we should start core1 after all core0 launched
	/* becase optimize_link_coherent_ht is moved out from setup_coherent_ht_domain, 
	 * So here need to make sure last core0 is started, esp for two way system,
	 * (there may be apic id conflicts in that case) 
	 */
        start_all_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif
	
	/* it will set up chains and store link pair for optimization later */
        ht_setup_chains_x(sysinfo); // it will init sblnk and sbbusn, nodes, sbdn

#if 0
	//it your CPU min fid is 1G, you can change HT to 1G and FID to max one time.
        needs_reset = optimize_link_coherent_ht();
        needs_reset |= optimize_link_incoherent_ht(sysinfo);
#endif

#if K8_SET_FIDVID == 1

        {
                struct msr msr;
                msr=rdmsr(FIDVID_STATUS);
                printk(BIOS_DEBUG, "begin msr fid, vid %08x:%08x\n",  msr.hi ,msr.lo);

        }

	enable_fid_change();

	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);

        init_fidvid_bsp(bsp_apicid);

        // show final fid and vid
        {
                struct msr;
                msr=rdmsr(msr_t);
               printk(BIOS_DEBUG, "begin msr fid, vid %08x:%08x\n",  msr.hi ,msr.lo);

        }
#endif

#if 1
	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);

        // fidvid change will issue one LDTSTOP and the HT change will be effective too
        if (needs_reset) {
                printk(BIOS_INFO, "ht reset -\r\n");
#warning define soft_reset_x
//FIXME                soft_reset_x(sysinfo->sbbusn, sysinfo->sbdn);
        }
#endif
	allow_all_aps_stop(bsp_apicid);

        //It's the time to set ctrl in sysinfo now;
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	enable_smbus();

#if 0
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

#warning re-implement post_cache_as_ram
 //       post_cache_as_ram(); // bsp swtich stack to ram and copy sysinfo ram now

	printk(BIOS_DEBUG, "stage1 returns\n");
	return 0;
}
