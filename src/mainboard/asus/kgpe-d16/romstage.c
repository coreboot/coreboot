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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

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
#include <cpu/amd/model_10xxx_rev.h>
#include <northbridge/amd/amdfam10/raminit.h>
#include <northbridge/amd/amdfam10/amdfam10.h>
#include "lib/delay.c"
#include <cpu/x86/lapic.h>
#include "northbridge/amd/amdfam10/reset_test.c"
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5572d/nct5572d.h>
#include <cpu/x86/bist.h>
// #include "northbridge/amd/amdk8/incoherent_ht.c"
#include <southbridge/amd/sb700/sb700.h>
#include <southbridge/amd/sb700/smbus.h>
#include <southbridge/amd/sr5650/sr5650.h>
#include "northbridge/amd/amdfam10/debug.c"
#include "northbridge/amd/amdfam10/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, NCT5572D_SP1)

static void activate_spd_rom(const struct mem_controller *ctrl);

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return do_smbus_read_byte(SMBUS_AUX_IO_BASE, device, address);
}

#include <northbridge/amd/amdfam10/amdfam10.h>
#include "northbridge/amd/amdfam10/raminit_sysinfo_in_ram.c"
#include "northbridge/amd/amdfam10/pci.c"
#include "resourcemap.c"
#include "cpu/amd/quadcore/quadcore.c"

#include <cpu/amd/microcode.h>

#include "cpu/amd/model_10xxx/init_cpus.c"
#include "northbridge/amd/amdfam10/early_ht.c"

/*
 * ASUS KGPE-D16 specific SPD enable/disable magic.
 *
 * Setting SP5100 GPIOs 59 and 60 controls an SPI mux with four settings:
 * 0: Disabled
 * 1: Normal SPI access
 * 2: CPU0 SPD
 * 3: CPU1 SPD
 *
 * Disable SPD access after RAM init to allow access to standard SMBus/I2C offsets
 * which is required e.g. by lm-sensors.
 */

/* Relevant GPIO register information is available in the
 * AMD SP5100 Register Reference Guide rev. 3.03, page 130
 */
static void switch_spd_mux(uint8_t channel)
{
	uint8_t byte;

	byte = pci_read_config8(PCI_DEV(0, 0x14, 0), 0x54);
	byte &= ~0xc;			/* Clear SPD mux GPIOs */
	byte &= ~0xc0;			/* Enable SPD mux GPIO output drivers */
	byte |= (channel << 2) & 0xc;	/* Set SPD mux GPIOs */
	pci_write_config8(PCI_DEV(0, 0x14, 0), 0x54, byte);
}

static const uint8_t spd_addr[] = {
	// Socket 0 Node 0 ("Node 0")
	RC00, DIMM0, DIMM1, 0, 0, DIMM2, DIMM3, 0, 0,
	// Socket 0 Node 1 ("Node 1")
	RC00, DIMM4, DIMM5, 0, 0, DIMM6, DIMM7, 0, 0,
	// Socket 1 Node 1 ("Node 2")
	RC01, DIMM4, DIMM5, 0, 0, DIMM6, DIMM7, 0, 0,
	// Socket 1 Node 0 ("Node 3")
	RC01, DIMM0, DIMM1, 0, 0, DIMM2, DIMM3, 0, 0,
};

static void activate_spd_rom(const struct mem_controller *ctrl) {
	struct sys_info *sysinfo = &sysinfo_car;

	printk(BIOS_DEBUG, "activate_spd_rom() for node %02x\n", ctrl->node_id);
	if (ctrl->node_id == 0) {
		printk(BIOS_DEBUG, "enable_spd_node0()\n");
		switch_spd_mux(0x2);
	} else if (ctrl->node_id == 1) {
		printk(BIOS_DEBUG, "enable_spd_node1()\n");
		switch_spd_mux((sysinfo->nodes <= 2)?0x2:0x3);
	} else if (ctrl->node_id == 2) {
		printk(BIOS_DEBUG, "enable_spd_node2()\n");
		switch_spd_mux((sysinfo->nodes <= 2)?0x3:0x2);
	} else if (ctrl->node_id == 3) {
		printk(BIOS_DEBUG, "enable_spd_node3()\n");
		switch_spd_mux(0x3);
	}
}

/* Voltages are specified by index
 * Valid indicies for this platform are:
 * 0: 1.5V
 * 1: 1.35V
 * 2: 1.25V
 * 3: 1.15V
 */
static void set_ddr3_voltage(uint8_t node, uint8_t index) {
	uint8_t byte;
	uint8_t value;

	if (index == 0)
		value = 0x0;
	else if (index == 1)
		value = 0x1;
	else if (index == 2)
		value = 0x4;
	else if (index == 3)
		value = 0x5;
	if (node == 1)
		value <<= 1;

	/* Set GPIOs */
	byte = pci_read_config8(PCI_DEV(0, 0x14, 3), 0xd1);
	if (node == 0)
		byte &= ~0x5;
	if (node == 1)
		byte &= ~0xa;
	byte |= value;
	pci_write_config8(PCI_DEV(0, 0x14, 3), 0xd1, byte);

	/* Enable GPIO output drivers */
	byte = pci_read_config8(PCI_DEV(0, 0x14, 3), 0xd0);
	byte &= 0x0f;
	pci_write_config8(PCI_DEV(0, 0x14, 3), 0xd0, byte);
}

static void set_peripheral_control_lines(void) {
	uint8_t byte;
	uint8_t nvram;
	uint8_t enable_ieee1394;

	enable_ieee1394 = 1;

	if (get_option(&nvram, "ieee1394_controller") == CB_SUCCESS)
		enable_ieee1394 = nvram & 0x1;

	if (enable_ieee1394) {
		/* Enable PCICLK5 (onboard FireWire device) */
		outb(0x41, 0xcd6);
		outb(0x02, 0xcd7);
	} else {
		/* Disable PCICLK5 (onboard FireWire device) */
		outb(0x41, 0xcd6);
		outb(0x00, 0xcd7);
	}

	/* Enable the RTC AltCentury register */
	outb(0x41, 0xcd6);
	byte = inb(0xcd7);
	byte |= 0x10;
	outb(byte, 0xcd7);
}

#ifdef TEST_MEMORY
static void execute_memory_test(void)
{
	/* Test DRAM functionality */
	uint32_t i;
	uint32_t* dataptr;
	printk(BIOS_DEBUG, "Writing test patterns to memory...\n");
	for (i=0; i < 0x1000000; i = i + 8) {
		dataptr = (void *)(0x300000 + i);
		*dataptr = 0x55555555;
		dataptr = (void *)(0x300000 + i + 4);
		*dataptr = 0xaaaaaaaa;
	}
	printk(BIOS_DEBUG, "Done!\n");
	printk(BIOS_DEBUG, "Testing memory...\n");
	uint32_t readback;
	for (i=0; i < 0x1000000; i = i + 8) {
		dataptr = (void *)(0x300000 + i);
		readback = *dataptr;
		if (readback != 0x55555555)
			printk(BIOS_DEBUG, "%p: INCORRECT VALUE %08x (should have been %08x)\n", dataptr, readback, 0x55555555);
		dataptr = (void *)(0x300000 + i + 4);
		readback = *dataptr;
		if (readback != 0xaaaaaaaa)
			printk(BIOS_DEBUG, "%p: INCORRECT VALUE %08x (should have been %08x)\n", dataptr, readback, 0xaaaaaaaa);
	}
	printk(BIOS_DEBUG, "Done!\n");
}
#endif

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	struct sys_info *sysinfo = &sysinfo_car;

	u32 bsp_apicid = 0, val;
	msr_t msr;

	timestamp_init(timestamp_get());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		set_bsp_node_CHtExtNodeCfgEn();
		enumerate_ht_chain();

		/* SR56x0 pcie bridges block pci_locate_device() before pcie training.
		 * disable all pcie bridges on SR56x0 to work around it
		 */
		sr5650_disable_pcie_bridge();

		/* Initialize southbridge */
		sb7xx_51xx_pci_port80();

		/* Initialize early serial */
		nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
		console_init();
	}

	post_code(0x30);

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	post_code(0x32);

	enable_sr5650_dev8();
	sb7xx_51xx_lpc_init();

	if (CONFIG_MAX_PHYSICAL_CPUS != 4)
		printk(BIOS_WARNING, "CONFIG_MAX_PHYSICAL_CPUS is %d, but this is a dual socket AMD G34 board!\n", CONFIG_MAX_PHYSICAL_CPUS);

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

	cpuSetAMDMSR();
	post_code(0x34);

	amd_ht_init(sysinfo);
	amd_ht_fixup(sysinfo);
	post_code(0x35);

	/* Set DDR memory voltage
	 * FIXME
	 * This should be set based on the output of the DIMM SPDs
	 * For now it is locked to 1.5V
	 */
	set_ddr3_voltage(0, 0);	/* Node 0 */
	set_ddr3_voltage(1, 0);	/* Node 1 */

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

	/* run _early_setup before soft-reset. */
	sr5650_early_setup();
	sb7xx_51xx_early_setup();

	if (IS_ENABLED(CONFIG_SET_FIDVID)) {
		msr = rdmsr(0xc0010071);
		printk(BIOS_DEBUG, "\nBegin FIDVID MSR 0xc0010071 0x%08x 0x%08x\n", msr.hi, msr.lo);

		/* FIXME: The sb fid change may survive the warm reset and only need to be done once */
		enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);

		post_code(0x39);

		if (!warm_reset_detect(0)) {			// BSP is node 0
			init_fidvid_bsp(bsp_apicid, sysinfo->nodes);
		} else {
			init_fidvid_stage2(bsp_apicid, 0);	// BSP is node 0
		}

		post_code(0x3A);

		/* show final fid and vid */
		msr=rdmsr(0xc0010071);
		printk(BIOS_DEBUG, "End FIDVIDMSR 0xc0010071 0x%08x 0x%08x\n", msr.hi, msr.lo);
	}

	if (IS_ENABLED(CONFIG_LOGICAL_CPUS)) {
		/* Core0 on each node is configured. Now setup any additional cores. */
		printk(BIOS_DEBUG, "start_other_cores()\n");
		start_other_cores();
		post_code(0x37);
		wait_all_other_cores_started(bsp_apicid);
	}

	post_code(0x38);

	init_timer(); // Need to use TMICT to synconize FID/VID

	sr5650_htinit();

	/* Reset for HT, FIDVID, PLL and errata changes to take affect. */
	if (!warm_reset_detect(0)) {
		printk(BIOS_INFO, "...WARM RESET...\n\n\n");
		soft_reset();
		die("After soft_reset_x - shouldn't see this message!!!\n");
	}

	/* Set up peripheral control lines */
	set_peripheral_control_lines();

	post_code(0x3B);

	/* It's the time to set ctrl in sysinfo now; */
	printk(BIOS_DEBUG, "fill_mem_ctrl() detected %d nodes\n", sysinfo->nodes);
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);
	post_code(0x3D);

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

#if !IS_ENABLED(CONFIG_LATE_CBMEM_INIT)
	cbmem_initialize_empty();
	post_code(0x41);

	amdmct_cbmem_store_info(sysinfo);
#endif

	printk(BIOS_DEBUG, "disable_spd()\n");
	switch_spd_mux(0x1);

	sr5650_before_pci_init();
	sb7xx_51xx_before_pci_init();

	/* Configure SP5100 GPIOs to match vendor settings */
	pci_write_config16(PCI_DEV(0, 0x14, 0), 0x50, 0x0170);
	pci_write_config16(PCI_DEV(0, 0x14, 0), 0x54, 0x0707);
	pci_write_config16(PCI_DEV(0, 0x14, 0), 0x56, 0x0bb0);
	pci_write_config16(PCI_DEV(0, 0x14, 0), 0x5a, 0x0ff0);

	timestamp_add_now(TS_END_ROMSTAGE);

#ifdef TEST_MEMORY
	execute_memory_test();
#endif

	post_cache_as_ram();	// BSP switch stack to ram, copy then execute LB.
	post_code(0x43);	// Should never see this post code.
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
