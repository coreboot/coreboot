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

#define FAM10_SCAN_PCI_BUS 0
#define FAM10_ALLOCATE_IO_RANGE 1

#include <stdint.h>
#include <string.h>
#include <reset.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <timestamp.h>
#include <lib.h>
#include <spd.h>
#include <cbmem.h>
#include <cpu/amd/model_10xxx_rev.h>
#include <cpu/amd/car.h>
#include <southbridge/nvidia/ck804/early_smbus.h>
#include <delay.h>
#include <cpu/x86/lapic.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627thg/w83627thg.h>
#include <cpu/x86/bist.h>
#include <northbridge/amd/amdht/ht_wrapper.h>
#include <northbridge/amd/amdfam10/raminit.h>
#include <cpu/amd/family_10h-family_15h/init_cpus.h>

#include "resourcemap.c"
#include "cpu/amd/quadcore/quadcore.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627THG_SP1)

#define CK804_MB_SETUP \
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+33, ~(0x0f),(0x04 | 0x01),	/* -ENOINFO Proprietary BIOS sets this register; "When in Rome..."*/

#include <southbridge/nvidia/ck804/early_setup_ss.h>
#include "southbridge/nvidia/ck804/early_setup_car.c"

#define GPIO3_DEV PNP_DEV(0x2e, W83627THG_GPIO3)

void activate_spd_rom(const struct mem_controller *ctrl);
int spd_read_byte(unsigned device, unsigned address);
extern struct sys_info sysinfo_car;

int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

/**
 * @brief Get SouthBridge device number
 * @param[in] bus target bus number
 * @return southbridge device number
 */
unsigned int get_sbdn(unsigned bus)
{
	pci_devfn_t dev;

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

static void ck804_control(const unsigned int *values, u32 size,
			  uint8_t bus_unit_id)
{
	unsigned busn[4], io_base[4];
	int i, ck804_num = 0;

	for (i = 0; i < 4; i++) {
		u32 id;
		pci_devfn_t dev;
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

static const uint8_t spd_addr[] = {
	// Node 0
	RC00, DIMM0, DIMM2, DIMM4, DIMM6, DIMM1, DIMM3, DIMM5, DIMM7,
	// Node 1
	RC01, DIMM0, DIMM2, DIMM4, DIMM6, DIMM1, DIMM3, DIMM5, DIMM7,
};

void activate_spd_rom(const struct mem_controller *ctrl) {
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

	u32 bsp_apicid = 0, val, wants_reset;
	msr_t msr;

	timestamp_init(timestamp_get());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		set_bsp_node_CHtExtNodeCfgEn();
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

	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);
	printk(BIOS_DEBUG, "bsp_apicid = %02x\n", bsp_apicid);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	/* Setup sysinfo defaults */
	set_sysinfo_in_ram(0);

	update_microcode(val);

	post_code(0x33);

	cpuSetAMDMSR(0);
	post_code(0x34);

	amd_ht_init(sysinfo);
	post_code(0x35);

	/* Setup nodes PCI space and start core 0 AP init. */
	finalize_node_setup(sysinfo);

	/* Setup any mainboard PCI settings etc. */
	setup_mb_resource_map();
	post_code(0x36);

	/* wait for all the APs core0 started by finalize_node_setup. */
	/* FIXME: A bunch of cores are going to start output to serial at once.
	 * It would be nice to fix up prink spinlocks for ROM XIP mode.
	 * I think it could be done by putting the spinlock flag in the cache
	 * of the BSP located right after sysinfo.
	 */
	wait_all_core0_started();

	if (IS_ENABLED(CONFIG_SET_FIDVID)) {
		msr = rdmsr(0xc0010071);
		printk(BIOS_DEBUG, "\nBegin FIDVID MSR 0xc0010071 0x%08x 0x%08x\n", msr.hi, msr.lo);

		post_code(0x39);

		if (!warm_reset_detect(0)) {			// BSP is node 0
			init_fidvid_bsp(bsp_apicid, sysinfo->nodes);
		} else {
			init_fidvid_stage2(bsp_apicid, 0);	// BSP is node 0
		}

		post_code(0x3A);

		/* show final fid and vid */
		msr = rdmsr(0xc0010071);
		printk(BIOS_DEBUG, "End FIDVIDMSR 0xc0010071 0x%08x 0x%08x\n", msr.hi, msr.lo);
	}

	if (IS_ENABLED(CONFIG_LOGICAL_CPUS)) {
		/* Core0 on each node is configured. Now setup any additional cores. */
		printk(BIOS_DEBUG, "start_other_cores()\n");
		start_other_cores(bsp_apicid);
		post_code(0x37);
		wait_all_other_cores_started(bsp_apicid);
	}

	printk(BIOS_DEBUG, "set_ck804_base_unit_id()\n");
	ck804_control(ctrl_conf_fix_pci_numbering, ARRAY_SIZE(ctrl_conf_fix_pci_numbering), CK804_BOARD_BOOT_BASE_UNIT_UID);

	post_code(0x38);

	init_timer(); // Need to use TMICT to synconize FID/VID

	wants_reset = ck804_early_setup_x();

	/* Reset for HT, FIDVID, PLL and errata changes to take affect. */
	if (!warm_reset_detect(0)) {
		printk(BIOS_INFO, "...WARM RESET...\n\n\n");
		soft_reset();
		die("After soft_reset - shouldn't see this message!!!\n");
	}

	if (wants_reset) {
		printk(BIOS_DEBUG, "ck804_early_setup_x wanted additional reset!\n");
	}

	post_code(0x3B);

	/* It's the time to set ctrl in sysinfo now; */
	printk(BIOS_DEBUG, "fill_mem_ctrl()\n");
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);
	post_code(0x3D);

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
	printk(BIOS_DEBUG, "raminit_amdmct()\n");
	raminit_amdmct(sysinfo);
	timestamp_add_now(TS_AFTER_INITRAM);

	cbmem_initialize_empty();
	post_code(0x41);

	amdmct_cbmem_store_info(sysinfo);

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
}

/**
 * BOOL AMD_CB_ManualBUIDSwapList(u8 Node, u8 Link, u8 **List)
 * Description:
 *	This routine is called every time a non-coherent chain is processed.
 *	BUID assignment may be controlled explicitly on a non-coherent chain. Provide a
 *	swap list. The first part of the list controls the BUID assignment and the
 *	second part of the list provides the device to device linking.  Device orientation
 *	can be detected automatically, or explicitly.  See documentation for more details.
 *
 *	Automatic non-coherent init assigns BUIDs starting at 1 and incrementing sequentially
 *	based on each device's unit count.
 *
 * Parameters:
 *	@param[in]  node   = The node on which this chain is located
 *	@param[in]  link   = The link on the host for this chain
 *	@param[out] List   = supply a pointer to a list
 */
BOOL AMD_CB_ManualBUIDSwapList (u8 node, u8 link, const u8 **List)
{
	return 0;
}
