/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 *
 * Copyright (C) 2006 MSI
 * Written by Bingxun Shi <bingxunshi@gmail.com> for MSI.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/amd/model_fxx_rev.h>
#include "southbridge/nvidia/mcp55/early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "lib/delay.c"
#include "cpu/x86/lapic.h"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "superio/winbond/w83627ehg/early_serial.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include <spd.h>
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include <device/pci_ids.h>
#include "southbridge/nvidia/mcp55/early_ctrl.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627EHG_SP1)

static void memreset(int controllers, const struct mem_controller *ctrl) { }

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_SWITCH1 0x70
#define SMBUS_SWITCH2 0x72
        unsigned device=(ctrl->channel0[0])>>8;
        smbus_send_byte(SMBUS_SWITCH1, device);
        smbus_send_byte(SMBUS_SWITCH2, (device >> 4) & 0x0f);
}

#if 0
static inline void change_i2c_mux(unsigned device)
{
#define SMBUS_SWITCH1 0x70
#define SMBUS_SWITHC2 0x72
        smbus_send_byte(SMBUS_SWITCH1, device & 0x0f);
        smbus_send_byte(SMBUS_SWITCH2, (device >> 4) & 0x0f);
}
#endif

static inline int spd_read_byte(unsigned device, unsigned address)
{
       return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/f.h"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/raminit_f.c"
#include "lib/generic_sdram.c"
#include "resourcemap.c"
#include "cpu/amd/dualcore/dualcore.c"
#include "southbridge/nvidia/mcp55/early_setup_ss.h"

//set GPIO to input mode
#define MCP55_MB_SETUP \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+15, ~(0xff), ((0<<4)|(0<<2)|(0<<0)),/* M8,GPIO16, PCIXA_PRSNT2_L*/ \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+44, ~(0xff), ((0<<4)|(0<<2)|(0<<0)),/* P5,GPIO45, PCIXA_PRSNT1_L*/ \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+16, ~(0xff), ((0<<4)|(0<<2)|(0<<0)),/* K4,GPIO17, PCIXB_PRSNT1_L*/ \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+45, ~(0xff), ((0<<4)|(0<<2)|(0<<0)),/* P7,GPIO46, PCIXB_PRSNT2_L*/ \

#include "southbridge/nvidia/mcp55/early_setup_car.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"
// Disabled until it's actually used:
// #include "cpu/amd/model_fxx/fidvid.c"
#include "northbridge/amd/amdk8/early_ht.c"

static void sio_setup(void)
{
        uint32_t dword;
        uint8_t byte;

        byte = pci_read_config32(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0x7b);
        byte |= 0x20;
        pci_write_config8(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0x7b, byte);

        dword = pci_read_config32(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0xa0);
        dword |= (1<<0);
        pci_write_config32(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0xa0, dword);
}

//CPU 1 mem is on SMBUS_HUB channel 2, and CPU 2 mem is on channel 1.
#define RC0 (2<<8)
#define RC1 (1<<8)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr[] = {
		// Node 0
		RC0|DIMM0, RC0|DIMM2, RC0|DIMM4, RC0|DIMM6,
		RC0|DIMM1, RC0|DIMM3, RC0|DIMM5, RC0|DIMM7,
		// node 1
		RC1|DIMM0, RC1|DIMM2, RC1|DIMM4, RC1|DIMM6,
		RC1|DIMM1, RC1|DIMM3, RC1|DIMM5, RC1|DIMM7,
	};

	unsigned bsp_apicid = 0;
        int needs_reset;
	struct sys_info *sysinfo = &sysinfo_car;

        if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		enumerate_ht_chain();
		sio_setup();
        }

        if (bist == 0) {
               //init_cpus(cpu_init_detectedx);
               bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);
        }

	w83627ehg_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	setup_ms9282_resource_map();

	setup_coherent_ht_domain();

	wait_all_core0_started();

#if CONFIG_LOGICAL_CPUS
        // It is said that we should start core1 after all core0 launched
        start_other_cores();
        //wait_all_other_cores_started(bsp_apicid);
#endif
        ht_setup_chains_x(sysinfo); // it will init sblnk and sbbusn, nodes, sbdn

	init_timer(); /* Need to use TMICT to synconize FID/VID. */

	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);
	needs_reset |= mcp55_early_setup_x();
        if (needs_reset) {
        	print_info("ht reset -\n");
                soft_reset();
        }

        //It's the time to set ctrl now;
        fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

       enable_smbus();

#if 0
        int i;
        for(i=4;i<8;i++) {
                change_i2c_mux(i);
                dump_smbus_registers();
        }
#endif

       sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

       post_cache_as_ram();
}
