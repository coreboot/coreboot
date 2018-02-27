/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2017 Raptor Engineering, LLC
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

#include <stdint.h>
#include <string.h>
#include <reset.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <romstage_handoff.h>
#include <timestamp.h>
#include <lib.h>
#include <spd.h>
#include <cpu/amd/model_10xxx_rev.h>
#include <delay.h>
#include <cpu/x86/lapic.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83667hg-a/w83667hg-a.h>
#include <cpu/x86/bist.h>
#include <cpu/amd/car.h>
#include <smp/spinlock.h>
#include <southbridge/amd/sb700/sb700.h>
#include <southbridge/amd/sb700/smbus.h>
#include <southbridge/amd/sr5650/sr5650.h>
#include <northbridge/amd/amdfam10/raminit.h>
#include <northbridge/amd/amdht/ht_wrapper.h>
#include <cpu/amd/family_10h-family_15h/init_cpus.h>
#include <arch/early_variables.h>
#include <cbmem.h>

#include "resourcemap.c"
#include "cpu/amd/quadcore/quadcore.c"

#define SERIAL_0_DEV PNP_DEV(0x2e, W83667HG_A_SP1)
#define SERIAL_1_DEV PNP_DEV(0x2e, W83667HG_A_SP2)

void activate_spd_rom(const struct mem_controller *ctrl);
int spd_read_byte(unsigned device, unsigned address);
extern struct sys_info sysinfo_car;

int spd_read_byte(unsigned device, unsigned address)
{
	return do_smbus_read_byte(SMBUS_AUX_IO_BASE, device, address);
}

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

static const uint8_t spd_addr_fam15[] = {
	// Socket 0 Node 0 ("Node 0")
	RC00, DIMM0, DIMM1, 0, 0, DIMM2, DIMM3, 0, 0,
	// Socket 0 Node 1 ("Node 1")
	RC00, DIMM4, DIMM5, 0, 0, DIMM6, DIMM7, 0, 0,
	// Socket 1 Node 0 ("Node 2")
	RC01, DIMM0, DIMM1, 0, 0, DIMM2, DIMM3, 0, 0,
	// Socket 1 Node 1 ("Node 3")
	RC01, DIMM4, DIMM5, 0, 0, DIMM6, DIMM7, 0, 0,
};

static const uint8_t spd_addr_fam10[] = {
	// Socket 0 Node 0 ("Node 0")
	RC00, DIMM0, DIMM1, 0, 0, DIMM2, DIMM3, 0, 0,
	// Socket 0 Node 1 ("Node 1")
	RC00, DIMM4, DIMM5, 0, 0, DIMM6, DIMM7, 0, 0,
	// Socket 1 Node 1 ("Node 2")
	RC01, DIMM4, DIMM5, 0, 0, DIMM6, DIMM7, 0, 0,
	// Socket 1 Node 0 ("Node 3")
	RC01, DIMM0, DIMM1, 0, 0, DIMM2, DIMM3, 0, 0,
};

void activate_spd_rom(const struct mem_controller *ctrl) {
	struct sys_info *sysinfo = &sysinfo_car;
	printk(BIOS_DEBUG, "activate_spd_rom() for node %02x\n", ctrl->node_id);
	if (ctrl->node_id == 0) {
		printk(BIOS_DEBUG, "enable_spd_node0()\n");
		switch_spd_mux(0x2);
	} else if (ctrl->node_id == 1) {
		printk(BIOS_DEBUG, "enable_spd_node1()\n");
		switch_spd_mux((is_fam15h() || (sysinfo->nodes <= 2))?0x2:0x3);
	} else if (ctrl->node_id == 2) {
		printk(BIOS_DEBUG, "enable_spd_node2()\n");
		switch_spd_mux((is_fam15h() || (sysinfo->nodes <= 2))?0x3:0x2);
	} else if (ctrl->node_id == 3) {
		printk(BIOS_DEBUG, "enable_spd_node3()\n");
		switch_spd_mux(0x3);
	}
}

/* Voltages are specified by index
 * Valid indices for this platform are:
 * 0: 1.5V
 * 1: 1.35V
 * 2: 1.25V
 * 3: 1.15V
 */
static void set_ddr3_voltage(uint8_t node, uint8_t index) {
	uint8_t byte;
	uint8_t value = 0;

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

	printk(BIOS_DEBUG, "Node %02d DIMM voltage set to index %02x\n", node, index);
}

void DIMMSetVoltages(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA) {
	/* This mainboard allows the DIMM voltage to be set per-socket.
	 * Therefore, for each socket, iterate over all DIMMs to find the
	 * lowest supported voltage common to all DIMMs on that socket.
	 */
	uint8_t nvram;
	uint8_t dimm;
	uint8_t node;
	uint8_t socket;
	uint8_t allowed_voltages = 0xf;	/* The mainboard VRMs allow 1.15V, 1.25V, 1.35V, and 1.5V */
	uint8_t socket_allowed_voltages = allowed_voltages;
	uint32_t set_voltage = 0;

	if (get_option(&nvram, "minimum_memory_voltage") == CB_SUCCESS) {
		switch (nvram) {
		case 2:
			allowed_voltages = 0x7;	/* Allow 1.25V, 1.35V, and 1.5V */
			break;
		case 1:
			allowed_voltages = 0x3;	/* Allow 1.35V and 1.5V */
			break;
		case 0:
		default:
			allowed_voltages = 0x1;	/* Allow 1.5V only */
			break;
		}
	}

	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		socket = node / 2;
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + node;

		/* reset socket_allowed_voltages before processing each socket */
		if (!(node % 2))
			socket_allowed_voltages = allowed_voltages;

		if (pDCTstat->NodePresent) {
			for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++) {
				if (pDCTstat->DIMMValid & (1 << dimm)) {
					socket_allowed_voltages &= pDCTstat->DimmSupportedVoltages[dimm];
				}
			}
		}

		/* set voltage per socket after processing last contained node */
		if (pDCTstat->NodePresent && (node % 2)) {
			/* Set voltages */
			if (socket_allowed_voltages & 0x8) {
				set_voltage = 0x8;
				set_ddr3_voltage(socket, 3);
			} else if (socket_allowed_voltages & 0x4) {
				set_voltage = 0x4;
				set_ddr3_voltage(socket, 2);
			} else if (socket_allowed_voltages & 0x2) {
				set_voltage = 0x2;
				set_ddr3_voltage(socket, 1);
			} else {
				set_voltage = 0x1;
				set_ddr3_voltage(socket, 0);
			}

			/* Save final DIMM voltages for MCT and SMBIOS use */
			if (pDCTstat->NodePresent) {
				for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++) {
					pDCTstat->DimmConfiguredVoltage[dimm] = set_voltage;
				}
			}
			pDCTstat = pDCTstatA + (node - 1);
			if (pDCTstat->NodePresent) {
				for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++) {
					pDCTstat->DimmConfiguredVoltage[dimm] = set_voltage;
				}
			}
		}
	}

	/* Allow the DDR supply voltages to settle */
	udelay(100000);
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
	uint32_t v;
	uint32_t w;
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t *dataptr;
	uint32_t readback;
	uint32_t start = 0x300000;
	printk(BIOS_DEBUG, "Writing test pattern 1 to memory...\n");
	for (i = 0; i < 0x1000000; i = i + 8) {
		dataptr = (void *)(start + i);
		*dataptr = 0x55555555;
		dataptr = (void *)(start + i + 4);
		*dataptr = 0xaaaaaaaa;
	}
	printk(BIOS_DEBUG, "Done!\n");
	printk(BIOS_DEBUG, "Testing memory...\n");
	for (i = 0; i < 0x1000000; i = i + 8) {
		dataptr = (void *)(start + i);
		readback = *dataptr;
		if (readback != 0x55555555)
			printk(BIOS_DEBUG, "%p: INCORRECT VALUE %08x (should have been %08x)\n", dataptr, readback, 0x55555555);
		dataptr = (void *)(start + i + 4);
		readback = *dataptr;
		if (readback != 0xaaaaaaaa)
			printk(BIOS_DEBUG, "%p: INCORRECT VALUE %08x (should have been %08x)\n", dataptr, readback, 0xaaaaaaaa);
	}
	printk(BIOS_DEBUG, "Done!\n");
	printk(BIOS_DEBUG, "Writing test pattern 2 to memory...\n");
	/* Set up the PRNG seeds for initial write */
	w = 0x55555555;
	x = 0xaaaaaaaa;
	y = 0x12345678;
	z = 0x87654321;
	for (i = 0; i < 0x1000000; i = i + 4) {
		/* Use Xorshift as a PRNG to stress test the bus */
		v = x;
		v ^= v << 11;
		v ^= v >> 8;
		x = y;
		y = z;
		z = w;
		w ^= w >> 19;
		w ^= v;
		dataptr = (void *)(start + i);
		*dataptr = w;
	}
	printk(BIOS_DEBUG, "Done!\n");
	printk(BIOS_DEBUG, "Testing memory...\n");
	/* Reset the PRNG seeds for readback */
	w = 0x55555555;
	x = 0xaaaaaaaa;
	y = 0x12345678;
	z = 0x87654321;
	for (i = 0; i < 0x1000000; i = i + 4) {
		/* Use Xorshift as a PRNG to stress test the bus */
		v = x;
		v ^= v << 11;
		v ^= v >> 8;
		x = y;
		y = z;
		z = w;
		w ^= w >> 19;
		w ^= v;
		dataptr = (void *)(start + i);
		readback = *dataptr;
		if (readback != w)
			printk(BIOS_DEBUG, "%p: INCORRECT VALUE %08x (should have been %08x)\n", dataptr, readback, w);
	}
	printk(BIOS_DEBUG, "Done!\n");
}
#endif

static spinlock_t printk_spinlock CAR_GLOBAL;

spinlock_t *romstage_console_lock(void)
{
	return car_get_var_ptr(&printk_spinlock);
}

void initialize_romstage_console_lock(void)
{
	spin_unlock(romstage_console_lock());
}

static spinlock_t nvram_cbfs_spinlock CAR_GLOBAL;

spinlock_t *romstage_nvram_cbfs_lock(void)
{
	return car_get_var_ptr(&nvram_cbfs_spinlock);
}

void initialize_romstage_nvram_cbfs_lock(void)
{
	spin_unlock(romstage_nvram_cbfs_lock());
}

static spinlock_t microcode_cbfs_spinlock CAR_GLOBAL;

spinlock_t *romstage_microcode_cbfs_lock(void)
{
	return car_get_var_ptr(&microcode_cbfs_spinlock);
}

void initialize_romstage_microcode_cbfs_lock(void)
{
	spin_unlock(romstage_microcode_cbfs_lock());
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	uint32_t esp;
	__asm__ volatile (
		"movl %%esp, %0"
		: "=r" (esp)
		);

	struct sys_info *sysinfo = &sysinfo_car;

	/* Limit the maximum HT speed to 2.6GHz to prevent lockups
	 * due to HT CPU <--> CPU wiring not being validated to 3.2GHz
	 */
	sysinfo->ht_link_cfg.ht_speed_limit = 2600;

	uint32_t bsp_apicid = 0, val;
	uint8_t byte;
	uint8_t power_on_reset = 0;
	msr_t msr;

	int s3resume = acpi_is_wakeup_s3();

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Initial timestamp */
		timestamp_init(timestamp_get());
		timestamp_add_now(TS_START_ROMSTAGE);

		/* Initialize the printk, nvram CBFS, and microcode CBFS spinlocks */
		initialize_romstage_console_lock();
		initialize_romstage_nvram_cbfs_lock();
		initialize_romstage_microcode_cbfs_lock();

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

		/* Configure secondary serial port pin mux */
		winbond_set_pinmux(SERIAL_1_DEV, 0x2a, W83667HG_SPI_PINMUX_GPIO4_SERIAL_B_MASK, W83667HG_SPI_PINMUX_SERIAL_B);

		/* Initialize early serial */
		winbond_enable_serial(SERIAL_0_DEV, CONFIG_TTYS0_BASE);
		console_init();

		/* Disable LPC legacy DMA support to prevent lockup */
		byte = pci_read_config8(PCI_DEV(0, 0x14, 3), 0x78);
		byte &= ~(1 << 0);
		pci_write_config8(PCI_DEV(0, 0x14, 3), 0x78, byte);
	}

	printk(BIOS_SPEW, "Initial stack pointer: %08x\n", esp);

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

	if (!sb7xx_51xx_decode_last_reset())
		power_on_reset = 1;

	initialize_mca(1, power_on_reset);
	update_microcode(val);

	post_code(0x33);

	cpuSetAMDMSR(0);
	post_code(0x34);

	amd_ht_init(sysinfo);
	amd_ht_fixup(sysinfo);
	post_code(0x35);

	/* Setup nodes PCI space and start core 0 AP init. */
	finalize_node_setup(sysinfo);

	/* Setup any mainboard PCI settings etc. */
	setup_mb_resource_map();
	initialize_mca(0, power_on_reset);
	post_code(0x36);

	/* Wait for all the APs core0 started by finalize_node_setup. */
	wait_all_core0_started();

	/* run _early_setup before soft-reset. */
	sr5650_early_setup();
	sb7xx_51xx_early_setup();

	if (IS_ENABLED(CONFIG_LOGICAL_CPUS)) {
		/* Core0 on each node is configured. Now setup any additional cores. */
		printk(BIOS_DEBUG, "start_other_cores()\n");
		start_other_cores(bsp_apicid);
		post_code(0x37);
		wait_all_other_cores_started(bsp_apicid);
	}

	if (IS_ENABLED(CONFIG_SET_FIDVID)) {
		msr = rdmsr(0xc0010071);
		printk(BIOS_DEBUG, "\nBegin FIDVID MSR 0xc0010071 0x%08x 0x%08x\n", msr.hi, msr.lo);

		/* FIXME: The sb fid change may survive the warm reset and only need to be done once */
		enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);

		post_code(0x39);

		#if IS_ENABLED(CONFIG_SET_FIDVID)
		if (!warm_reset_detect(0)) {			// BSP is node 0
			init_fidvid_bsp(bsp_apicid, sysinfo->nodes);
		} else {
			init_fidvid_stage2(bsp_apicid, 0);	// BSP is node 0
		}
		#endif

		post_code(0x3A);

		/* show final fid and vid */
		msr = rdmsr(0xc0010071);
		printk(BIOS_DEBUG, "End FIDVIDMSR 0xc0010071 0x%08x 0x%08x\n", msr.hi, msr.lo);
	}

	post_code(0x38);

	init_timer(); // Need to use TMICT to synconize FID/VID

	sr5650_htinit();

	/* Reset for HT, FIDVID, PLL and errata changes to take effect. */
	if (!warm_reset_detect(0)) {
		printk(BIOS_INFO, "...WARM RESET...\n\n\n");
		soft_reset();
		die("After soft_reset - shouldn't see this message!!!\n");
	}

	sr5650_htinit_dect_and_enable_isochronous_link();

	/* Set default DDR memory voltage
	 * This will be overridden later during RAM initialization
	 */
	set_lpc_sticky_ctl(1);	/* Retain LPC/IMC GPIO configuration during S3 sleep */
	if (!s3resume) {	/* Avoid supply voltage glitches while the DIMMs are retaining data */
		set_ddr3_voltage(0, 0);	/* Node 0 */
		set_ddr3_voltage(1, 0);	/* Node 1 */
	}

	/* Set up peripheral control lines */
	set_peripheral_control_lines();

	post_code(0x3B);

	/* Wait for all APs to be stopped, otherwise RAM initialization may hang */
	if (IS_ENABLED(CONFIG_LOGICAL_CPUS))
		wait_all_other_cores_stopped(bsp_apicid);

	/* It's the time to set ctrl in sysinfo now; */
	printk(BIOS_DEBUG, "fill_mem_ctrl() detected %d nodes\n", sysinfo->nodes);
	if (is_fam15h())
		fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr_fam15);
	else
		fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr_fam10);
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

#ifdef TEST_MEMORY
	execute_memory_test();
#endif

	if (s3resume)
		cbmem_initialize();
	else
		cbmem_initialize_empty();

	romstage_handoff_init(s3resume);

	post_code(0x41);

	amdmct_cbmem_store_info(sysinfo);

	printk(BIOS_DEBUG, "disable_spd()\n");
	switch_spd_mux(0x1);

	sr5650_before_pci_init();
	sb7xx_51xx_before_pci_init();

	/* Configure SP5100 GPIOs to match vendor settings */
	pci_write_config16(PCI_DEV(0, 0x14, 0), 0x50, 0x0170);
	pci_write_config16(PCI_DEV(0, 0x14, 0), 0x54, 0x0707);
	pci_write_config16(PCI_DEV(0, 0x14, 0), 0x56, 0x0bb0);
	pci_write_config16(PCI_DEV(0, 0x14, 0), 0x5a, 0x0ff0);
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
	/* Force BUID to 0 */
	static const u8 swaplist[] = {0, 0, 0xFF, 0, 0xFF};
	if ((is_fam15h() && (node == 0) && (link == 1))			/* Family 15h BSP SB link */
		|| (!is_fam15h() && (node == 0) && (link == 3))) {	/* Family 10h BSP SB link */
		*List = swaplist;
		return 1;
	}

	return 0;
}
