/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
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
 */

unsigned int get_sbdn(unsigned bus);

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <timestamp.h>
#include <cpu/amd/model_fxx_rev.h>
#include "southbridge/nvidia/ck804/early_smbus.h"
#include <reset.h>
#include <northbridge/amd/amdk8/raminit.h>
#include "northbridge/amd/amdk8/reset_test.c"
#include <cpu/x86/bist.h>
#include <delay.h>
#include "northbridge/amd/amdk8/debug.c"
#include <cpu/amd/mtrr.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627thg/w83627thg.h>
#include "northbridge/amd/amdk8/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627THG_SP1)

static void memreset(int controllers, const struct mem_controller *ctrl) { }

static void activate_spd_rom(const struct mem_controller *ctrl);

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

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
#include "northbridge/amd/amdk8/early_ht.c"

#define CK804_MB_SETUP \
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+33, ~(0x0f),(0x04 | 0x01),	/* -ENOINFO Proprietary BIOS sets this register; "When in Rome..."*/

#include <southbridge/nvidia/ck804/early_setup_ss.h>
#include "southbridge/nvidia/ck804/early_setup_car.c"
#include <cpu/amd/microcode.h>

#define GPIO3_DEV PNP_DEV(0x2e, W83627THG_GPIO3)

/**
 * @brief Get SouthBridge device number
 * @param[in] bus target bus number
 * @return southbridge device number
 */
unsigned int get_sbdn(unsigned bus)
{
	device_t dev;

	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_NVIDIA,
					PCI_DEVICE_ID_NVIDIA_CK804_PRO), bus);
	return (dev >> 15) & 0x1f;
}

/*
 * ASUS KFSN4-DRE specific SPD enable/disable magic.
 *
 * Setting CK804 GPIO43 and GPIO44 to 0 and 0 respectively will make the
 * board DIMMs accessible at SMBus/SPD offsets 0x50-0x53. Per default the SPD
 * offsets 0x50-0x53 are _not_ readable (all SPD reads will return 0xff) which
 * will make RAM init fail.
 *
 * Disable SPD access after RAM init to allow access to standard SMBus/I2C offsets
 * which is required e.g. by lm-sensors.
 */

#define CK804_BOARD_BOOT_BASE_UNIT_UID 1

static const unsigned int ctrl_conf_enable_spd_node0[] = {
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+42, ~(0x0f),(0x04 | 0x00),/* W2,GPIO43, U6 input S0*/
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+43, ~(0x0f),(0x04 | 0x00),/* W3,GPIO44, U6 input S1*/
};

static const unsigned int ctrl_conf_enable_spd_node1[] = {
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+42, ~(0x0f),(0x04 | 0x00),/* W2,GPIO43, U6 input S0*/
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+43, ~(0x0f),(0x04 | 0x01),/* W3,GPIO44, U6 input S1*/
};

static const unsigned int ctrl_conf_disable_spd[] = {
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+42, ~(0x0f),(0x04 | 0x01),/* W2,GPIO43, U6 input S0*/
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+43, ~(0x0f),(0x04 | 0x00),/* W3,GPIO44, U6 input S1*/
};

static const unsigned int ctrl_conf_fix_pci_numbering[] = {
	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x44), ~(0x00010000), 0x00000000,	/* Force CK804 to start its internal device numbering (Base Unit ID) at 0 instead of the power-on default of 1 */
};

static const unsigned int ctrl_conf_enable_msi_mapping[] = {
	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xe0), ~(0x00000000), 0x00010000,	/* Enable MSI mapping on host bridge -- without this Linux cannot use the network device MSI interrupts! */
};

static void ck804_control(const unsigned int* values, u32 size, uint8_t bus_unit_id)
{
	unsigned busn[4], io_base[4];
	int i, ck804_num = 0;

	for (i = 0; i < 4; i++) {
		u32 id;
		device_t dev;
		if (i == 0) /* SB chain */
			dev = PCI_DEV(i * 0x40, bus_unit_id, 0);
		else
			dev = 0;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (id == 0x005e10de) {
			busn[ck804_num] = i * 0x40;
			io_base[ck804_num] = i * 0x4000;
			ck804_num++;
		}
	}

	if (ck804_num < 1)
		printk(BIOS_WARNING, "CK804 not found at device base unit id %02x!\n", bus_unit_id);

	ck804_early_set_port(ck804_num, busn, io_base);

	setup_resource_map_x_offset(values,
		size,
		PCI_DEV(0, bus_unit_id, 0), io_base[0]);

	ck804_early_clear_port(ck804_num, busn, io_base);
}

static void sio_setup(void)
{
	u32 dword;
	u8 byte;

	/* Subject decoding */
	byte = pci_read_config8(PCI_DEV(0, CK804_BOARD_BOOT_BASE_UNIT_UID + 1, 0), 0x7b);
	byte |= 0x20;
	pci_write_config8(PCI_DEV(0, CK804_BOARD_BOOT_BASE_UNIT_UID + 1, 0), 0x7b, byte);

	/* LPC Positive Decode 0 */
	dword = pci_read_config32(PCI_DEV(0, CK804_BOARD_BOOT_BASE_UNIT_UID + 1, 0), 0xa0);
	/* Serial 0, Serial 1 */
	dword |= (1 << 0) | (1 << 1);
	pci_write_config32(PCI_DEV(0, CK804_BOARD_BOOT_BASE_UNIT_UID + 1, 0), 0xa0, dword);
}

static const uint16_t spd_addr[] = {
	// Node 0
	RC00 | DIMM0, RC00 | DIMM2, RC00 | DIMM4, RC00 | DIMM6, RC00 | DIMM1, RC00 | DIMM3, RC00 | DIMM5, RC00 | DIMM7,
	// Node 1
	RC01 | DIMM0, RC01 | DIMM2, RC01 | DIMM4, RC01 | DIMM6, RC01 | DIMM1, RC01 | DIMM3, RC01 | DIMM5, RC01 | DIMM7,
};

static void activate_spd_rom(const struct mem_controller *ctrl) {
	printk(BIOS_DEBUG, "activate_spd_rom() for node %02x\n", ctrl->node_id);
	if (ctrl->node_id == 0) {
		printk(BIOS_DEBUG, "enable_spd_node0()\n");
		ck804_control(ctrl_conf_enable_spd_node0, ARRAY_SIZE(ctrl_conf_enable_spd_node0), CK804_DEVN_BASE);
	}
	else if (ctrl->node_id == 1) {
		printk(BIOS_DEBUG, "enable_spd_node1()\n");
		ck804_control(ctrl_conf_enable_spd_node1, ARRAY_SIZE(ctrl_conf_enable_spd_node1), CK804_DEVN_BASE);
	}
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	struct sys_info *sysinfo = &sysinfo_car;

	uint32_t bsp_apicid = 0;
	uint32_t dword;
	uint8_t needs_reset = 0;
#if IS_ENABLED(CONFIG_SET_FIDVID)
	struct cpuid_result cpuid1;
#endif

	timestamp_init(timestamp_get());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		enumerate_ht_chain();
		sio_setup();
	}

	post_code(0x30);

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	post_code(0x32);

	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	if (CONFIG_MAX_PHYSICAL_CPUS != 2)
		printk(BIOS_WARNING, "CONFIG_MAX_PHYSICAL_CPUS is %d, but this is a dual socket board!\n", CONFIG_MAX_PHYSICAL_CPUS);

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	dword = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", dword);
	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);
	printk(BIOS_DEBUG, "bsp_apicid = %02x\n", bsp_apicid);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	/* Setup sysinfo defaults */
	set_sysinfo_in_ram(0);

	post_code(0x33);

	setup_coherent_ht_domain();
	post_code(0x35);

	/* Wait for all base cores to start */
	wait_all_core0_started();
	post_code(0x36);

	/* Setup any mainboard PCI settings etc. */
	setup_mb_resource_map();
	post_code(0x37);

	if (IS_ENABLED(CONFIG_LOGICAL_CPUS)) {
		/* Core0 on each node is configured. Now setup any additional cores. */
		printk(BIOS_DEBUG, "start_other_cores()\n");
		start_other_cores();
		post_code(0x38);
		wait_all_other_cores_started(bsp_apicid);
		post_code(0x39);
	}

	ht_setup_chains_x(sysinfo);

#if IS_ENABLED(CONFIG_SET_FIDVID)
	/* Check to see if processor is capable of changing FIDVID  */
	/* otherwise it will throw a GP# when reading FIDVID_STATUS */
	cpuid1 = cpuid(0x80000007);
	if ((cpuid1.edx & 0x6) == 0x6) {
		msr_t msr;

		/* Read FIDVID_STATUS */
		msr = rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "begin msr fid, vid %08x%08x\n", msr.hi, msr.lo);

		enable_fid_change();
		enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
		init_fidvid_bsp(bsp_apicid);

		/* Show final FID and VID */
		msr = rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "end   msr fid, vid %08x%08x\n", msr.hi, msr.lo);
	} else {
		printk(BIOS_DEBUG, "Changing FIDVID not supported\n");
	}
#endif

	init_timer(); /* Need to use TMICT to synchronize FID/VID. */

	printk(BIOS_DEBUG, "set_ck804_base_unit_id()\n");
	ck804_control(ctrl_conf_fix_pci_numbering, ARRAY_SIZE(ctrl_conf_fix_pci_numbering), CK804_BOARD_BOOT_BASE_UNIT_UID);

	post_code(0x3a);

	printk(BIOS_DEBUG, "optimize_link_coherent_ht()\n");
	needs_reset = optimize_link_coherent_ht();
	printk(BIOS_DEBUG, "optimize_link_incoherent_ht()\n");
	needs_reset |= optimize_link_incoherent_ht(sysinfo);
	printk(BIOS_DEBUG, "ck804_early_setup_x()\n");
	needs_reset |= ck804_early_setup_x();

        /* FIDVID change will issue one LDTSTOP and the HT change will be effective too */
        if (needs_reset) {
                printk(BIOS_INFO, "ht reset -\n");
                soft_reset();
        }

	post_code(0x3b);

	allow_all_aps_stop(bsp_apicid);

	/* It's the time to set ctrl in sysinfo now; */
	printk(BIOS_DEBUG, "fill_mem_ctrl()\n");
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);
	post_code(0x3d);

	printk(BIOS_DEBUG, "enable_smbus()\n");
	enable_smbus();

#if 0
	/* FIXME
	 * After the AMD K10 code has been converted to use
	 * IS_ENABLED(CONFIG_DEBUG_SMBUS) uncomment this block
	 */
	if (IS_ENABLED(CONFIG_DEBUG_SMBUS)) {
	        dump_spd_registers(&cpu[0]);
        	dump_smbus_registers();
	}
#endif

	post_code(0x40);

	timestamp_add_now(TS_BEFORE_INITRAM);
	printk(BIOS_DEBUG, "sdram_initialize()\n");
	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

	timestamp_add_now(TS_AFTER_INITRAM);

	cbmem_initialize_empty();
	post_code(0x41);

	printk(BIOS_DEBUG, "disable_spd()\n");
	ck804_control(ctrl_conf_disable_spd, ARRAY_SIZE(ctrl_conf_disable_spd), CK804_DEVN_BASE);

	printk(BIOS_DEBUG, "enable_msi_mapping()\n");
	ck804_control(ctrl_conf_enable_msi_mapping, ARRAY_SIZE(ctrl_conf_enable_msi_mapping), CK804_DEVN_BASE);

	/* Initialize GPIO */
	/* Access SuperIO GPI03 logical device */
	uint16_t port = GPIO3_DEV >> 8;
        outb(0x87, port);
        outb(0x87, port);
	pnp_set_logical_device(GPIO3_DEV);
	/* Set GP37 (power LED) to output */
	pnp_write_config(GPIO3_DEV, 0xf0, 0x7f);
	/* Set GP37 (power LED) on */
	pnp_write_config(GPIO3_DEV, 0xf1, 0x80);
	/* Set pin 64 multiplex to GP37 */
	uint8_t cr2c = pnp_read_config(GPIO3_DEV, 0x2c);
	pnp_write_config(GPIO3_DEV, 0x2c, (cr2c & 0xf3) | 0x04);
	/* Restore default SuperIO access */
	outb(0xaa, port);

	post_cache_as_ram();	// BSP switch stack to ram, copy then execute LB.
	post_code(0x43);	// Should never see this post code.
}
