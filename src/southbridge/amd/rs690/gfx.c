/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

/*
 *  for rs690 internal graphics device
 *  device id of internal graphics:
 *	RS690M/T: 0x791f
 *    RS690:	   0x791e
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <delay.h>
#include "rs690.h"

#define CLK_CNTL_INDEX	0x8
#define CLK_CNTL_DATA	0xC

#ifdef UNUSED_CODE
static u32 clkind_read(struct device *dev, u32 index)
{
	u32	gfx_bar2 = pci_read_config32(dev, 0x18) & ~0xF;

	*(u32*)(gfx_bar2+CLK_CNTL_INDEX) = index & 0x7F;
	return *(u32*)(gfx_bar2+CLK_CNTL_DATA);
}
#endif

static void clkind_write(struct device *dev, u32 index, u32 data)
{
	u32	gfx_bar2 = pci_read_config32(dev, 0x18) & ~0xF;
	/* printk(BIOS_INFO, "gfx bar 2 %02x\n", gfx_bar2); */

	*(u32*)(gfx_bar2+CLK_CNTL_INDEX) = index | 1<<7;
	*(u32*)(gfx_bar2+CLK_CNTL_DATA)  = data;
}

/*
* pci_dev_read_resources thinks it is a IO type.
* We have to force it to mem type.
*/
static void rs690_gfx_read_resources(struct device *dev)
{
	printk(BIOS_INFO, "rs690_gfx_read_resources.\n");

	/* The initial value of 0x24 is 0xFFFFFFFF, which is confusing.
	   Even if we write 0xFFFFFFFF into it, it will be 0xFFF00000,
	   which tells us it is a memory address base.
	 */
	pci_write_config32(dev, 0x24, 0x00000000);

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);
	compact_resources(dev);
}

static void internal_gfx_pci_dev_init(struct device *dev)
{
	u16 deviceid, vendorid;
	deviceid = pci_read_config16(dev, PCI_DEVICE_ID);
	vendorid = pci_read_config16(dev, PCI_VENDOR_ID);
	printk(BIOS_INFO, "internal_gfx_pci_dev_init device=%x, vendor=%x.\n",
	     deviceid, vendorid);

	pci_dev_init(dev);

	/* clk ind */
	clkind_write(dev, 0x08, 0x01);
	clkind_write(dev, 0x0C, 0x22);
	clkind_write(dev, 0x0F, 0x0);
	clkind_write(dev, 0x11, 0x0);
	clkind_write(dev, 0x12, 0x0);
	clkind_write(dev, 0x14, 0x0);
	clkind_write(dev, 0x15, 0x0);
	clkind_write(dev, 0x16, 0x0);
	clkind_write(dev, 0x17, 0x0);
	clkind_write(dev, 0x18, 0x0);
	clkind_write(dev, 0x19, 0x0);
	clkind_write(dev, 0x1A, 0x0);
	clkind_write(dev, 0x1B, 0x0);
	clkind_write(dev, 0x1C, 0x0);
	clkind_write(dev, 0x1D, 0x0);
	clkind_write(dev, 0x1E, 0x0);
	clkind_write(dev, 0x26, 0x0);
	clkind_write(dev, 0x27, 0x0);
	clkind_write(dev, 0x28, 0x0);
	clkind_write(dev, 0x5C, 0x0);
}


/*
* Set registers in RS690 and CPU to enable the internal GFX.
* Please refer to CIM source code and BKDG.
*/
static void rs690_internal_gfx_enable(struct device *dev)
{
	u32 l_dword;
	int i;
	struct device *k8_f2 = NULL;
	struct device *nb_dev = dev_find_slot(0, 0);

	printk(BIOS_INFO, "rs690_internal_gfx_enable dev=0x%p, nb_dev=0x%p.\n", dev,
		    nb_dev);

	/* set APERTURE_SIZE, 128M. */
	l_dword = pci_read_config32(nb_dev, 0x8c);
	printk(BIOS_INFO, "nb_dev, 0x8c=0x%x\n", l_dword);
	l_dword &= 0xffffff8f;
	pci_write_config32(nb_dev, 0x8c, l_dword);

	/* set TOM */
	rs690_set_tom(nb_dev);

	/* Enable 64bit mode. */
	set_nbmc_enable_bits(nb_dev, 0x5f, 0, 1 << 9);
	set_nbmc_enable_bits(nb_dev, 0xb0, 0, 1 << 8);

	/* 64bit Latency. */
	set_nbmc_enable_bits(nb_dev, 0x5f, 0x7c00, 0x800);

	/* UMA dual channel control register. */
	nbmc_write_index(nb_dev, 0x86, 0x3d);

	/* check the setting later!! */
	set_htiu_enable_bits(nb_dev, 0x07, 1 << 7, 0);

	/* UMA mode, powerdown memory PLL. */
	set_nbmc_enable_bits(nb_dev, 0x74, 0, 1 << 31);

	/* Copy CPU DDR Controller to NB MC. */
	/* Why K8_MC_REG80 is special? */
	k8_f2 = dev_find_slot(0, PCI_DEVFN(0x18, 2));
	for (i = 0; i <= (0x80 - 0x40) / 4; i++) {
		l_dword = pci_read_config32(k8_f2, 0x40 + i * 4);
		nbmc_write_index(nb_dev, 0x63 + i, l_dword);
	}

	/* Set K8 MC for UMA, Family F. */
	l_dword = pci_read_config32(k8_f2, 0xa0);
	l_dword |= 0x2c;
	pci_write_config32(k8_f2, 0xa0, l_dword);
	l_dword = pci_read_config32(k8_f2, 0x94);
	l_dword &= 0xf0ffffff;
	l_dword |= 0x07000000;
	pci_write_config32(k8_f2, 0x94, l_dword);

	/* set FB size and location. */
	nbmc_write_index(nb_dev, 0x1b, 0x00);
	l_dword = nbmc_read_index(nb_dev, 0x1c);
	l_dword &= 0xffff0;
	l_dword |= 0x400 << 20;
	l_dword |= 0x4;
	nbmc_write_index(nb_dev, 0x1c, l_dword);
	l_dword = nbmc_read_index(nb_dev, 0x1d);
	l_dword &= 0xfffff000;
	l_dword |= 0x0400;
	nbmc_write_index(nb_dev, 0x1d, l_dword);
	nbmc_write_index(nb_dev, 0x100, 0x3fff3800);

	/* Program MC table. */
	set_nbmc_enable_bits(nb_dev, 0x00, 0, 1 << 31);
	l_dword = nbmc_read_index(nb_dev, 0x91);
	l_dword |= 0x5;
	nbmc_write_index(nb_dev, 0x91, l_dword);
	set_nbmc_enable_bits(nb_dev, 0xb1, 0, 1 << 6);
	set_nbmc_enable_bits(nb_dev, 0xc3, 0, 1);

	/* TODO: the optimization of voltage and frequency */
}

static void gfx_dev_set_subsystem(struct device *dev, unsigned vendor,
				  unsigned device)
{
	pci_write_config32(dev, 0x4c,  ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = gfx_dev_set_subsystem,
};

static struct device_operations pcie_ops = {
	.read_resources = rs690_gfx_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = internal_gfx_pci_dev_init,	/* The option ROM initializes the device. rs690_gfx_init, */
	.scan_bus = 0,
	.enable = rs690_internal_gfx_enable,
	.ops_pci = &lops_pci,
};

/*
 * The dev id of 690G is 791E, while the id of 690M, 690T is 791F.
 * We should list both of them here.
 * */
static const struct pci_driver pcie_driver_690t __pci_driver = {
	.ops = &pcie_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_RS690MT_INT_GFX,
};

static const struct pci_driver pcie_driver_690 __pci_driver = {
	.ops = &pcie_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_RS690_INT_GFX,
};

/* step 12 ~ step 14 from rpr */
static void single_port_configuration(struct device *nb_dev, struct device *dev)
{
	u8 result, width;
	u32 reg32;
	struct southbridge_amd_rs690_config *cfg =
	    (struct southbridge_amd_rs690_config *)nb_dev->chip_info;

	printk(BIOS_INFO, "rs690_gfx_init single_port_configuration.\n");

	/* step 12 training, releases hold training for GFX port 0 (device 2) */
	set_nbmisc_enable_bits(nb_dev, 0x8, 1 << 4, 0<<4);
	PcieReleasePortTraining(nb_dev, dev, 2);
	result = PcieTrainPort(nb_dev, dev, 2);
	printk(BIOS_INFO, "rs690_gfx_init single_port_configuration step12.\n");

	/* step 13 Power Down Control */
	/* step 13.1 Enables powering down transmitter and receiver pads along with PLL macros. */
	set_pcie_enable_bits(nb_dev, 0x40, 1 << 0, 1 << 0);

	/* step 13.a Link Training was NOT successful */
	if (!result) {
		set_nbmisc_enable_bits(nb_dev, 0x8, 0, 0x3 << 4); /* prevent from training. */
		set_nbmisc_enable_bits(nb_dev, 0xc, 0, 0x3 << 2); /* hide the GFX bridge. */
		if (cfg->gfx_tmds)
			nbpcie_ind_write_index(nb_dev, 0x65, 0xccf0f0);
		else {
			nbpcie_ind_write_index(nb_dev, 0x65, 0xffffffff);
			set_nbmisc_enable_bits(nb_dev, 0x7, 1 << 3, 1 << 3);
		}
	} else {		/* step 13.b Link Training was successful */

		reg32 = nbpcie_p_read_index(dev, 0xa2);
		width = (reg32 >> 4) & 0x7;
		printk(BIOS_DEBUG, "GFX LC_LINK_WIDTH = 0x%x.\n", width);
		switch (width) {
		case 1:
		case 2:
			nbpcie_ind_write_index(nb_dev, 0x65,
					       cfg->gfx_lane_reversal ? 0x7f7f : 0xccfefe);
			break;
		case 4:
			nbpcie_ind_write_index(nb_dev, 0x65,
					       cfg->gfx_lane_reversal ? 0x3f3f : 0xccfcfc);
			break;
		case 8:
			nbpcie_ind_write_index(nb_dev, 0x65,
					       cfg->gfx_lane_reversal ? 0x0f0f : 0xccf0f0);
			break;
		}
	}
	printk(BIOS_INFO, "rs690_gfx_init single_port_configuration step13.\n");

	/* step 14 Reset Enumeration Timer, disables the shortening of the enumeration timer */
	set_pcie_enable_bits(dev, 0x70, 1 << 19, 0 << 19);
	printk(BIOS_INFO, "rs690_gfx_init single_port_configuration step14.\n");
}

/* step 15 ~ step 18 from rpr */
static void dual_port_configuration(struct device *nb_dev, struct device *dev)
{
	u8 result, width;
	u32 reg32;
	struct southbridge_amd_rs690_config *cfg =
		    (struct southbridge_amd_rs690_config *)nb_dev->chip_info;

	/* step 15: Training for Device 2 */
	set_nbmisc_enable_bits(nb_dev, 0x8, 1 << 4, 0 << 4);
	/* Releases hold training for GFX port 0 (device 2) */
	PcieReleasePortTraining(nb_dev, dev, 2);
	/* PCIE Link Training Sequence */
	result = PcieTrainPort(nb_dev, dev, 2);

	/* step 16: Power Down Control for Device 2 */
	/* step 16.a Link Training was NOT successful */
	if (!result) {
		/* Powers down all lanes for port A */
		nbpcie_ind_write_index(nb_dev, 0x65, 0x0f0f);
	} else {		/* step 16.b Link Training was successful */

		reg32 = nbpcie_p_read_index(dev, 0xa2);
		width = (reg32 >> 4) & 0x7;
		printk(BIOS_DEBUG, "GFX LC_LINK_WIDTH = 0x%x.\n", width);
		switch (width) {
		case 1:
		case 2:
			nbpcie_ind_write_index(nb_dev, 0x65,
					       cfg->gfx_lane_reversal ? 0x0707 : 0x0e0e);
			break;
		case 4:
			nbpcie_ind_write_index(nb_dev, 0x65,
					       cfg->gfx_lane_reversal ? 0x0303 : 0x0c0c);
			break;
		}
	}

	/* step 17: Training for Device 3 */
	set_nbmisc_enable_bits(nb_dev, 0x8, 1 << 5, 0 << 5);
	/* Releases hold training for GFX port 0 (device 3) */
	PcieReleasePortTraining(nb_dev, dev, 3);
	/* PCIE Link Training Sequence */
	result = PcieTrainPort(nb_dev, dev, 3);

	/*step 18: Power Down Control for Device 3 */
	/* step 18.a Link Training was NOT successful */
	if (!result) {
		/* Powers down all lanes for port B and PLL1 */
		nbpcie_ind_write_index(nb_dev, 0x65, 0xccf0f0);
	} else {		/* step 18.b Link Training was successful */

		reg32 = nbpcie_p_read_index(dev, 0xa2);
		width = (reg32 >> 4) & 0x7;
		printk(BIOS_DEBUG, "GFX LC_LINK_WIDTH = 0x%x.\n", width);
		switch (width) {
		case 1:
		case 2:
			nbpcie_ind_write_index(nb_dev, 0x65,
					       cfg->gfx_lane_reversal ? 0x7070 : 0xe0e0);
			break;
		case 4:
			nbpcie_ind_write_index(nb_dev, 0x65,
					       cfg->gfx_lane_reversal ? 0x3030 : 0xc0c0);
			break;
		}
	}
}


/* For single port GFX configuration Only
* width:
* 	000 = x16
* 	001 = x1
*	010 = x2
*	011 = x4
*	100 = x8
*	101 = x12 (not supported)
*	110 = x16
*/
static void dynamic_link_width_control(struct device *nb_dev,
				       struct device *dev, u8 width)
{
	u32 reg32;
	struct device *sb_dev;
	struct southbridge_amd_rs690_config *cfg =
	    (struct southbridge_amd_rs690_config *)nb_dev->chip_info;

	/* step 5.9.1.1 */
	reg32 = nbpcie_p_read_index(dev, 0xa2);

	/* step 5.9.1.2 */
	set_pcie_enable_bits(nb_dev, 0x40, 1 << 0, 1 << 0);
	/* step 5.9.1.3 */
	set_pcie_enable_bits(dev, 0xa2, 3 << 0, width << 0);
	/* step 5.9.1.4 */
	set_pcie_enable_bits(dev, 0xa2, 1 << 8, 1 << 8);
	/* step 5.9.2.4 */
	if (0 == cfg->gfx_reconfiguration)
		set_pcie_enable_bits(dev, 0xa2, 1 << 11, 1 << 11);

	/* step 5.9.1.5 */
	do {
		reg32 = nbpcie_p_read_index(dev, 0xa2);
	}
	while (reg32 & 0x100);

	/* step 5.9.1.6 */
	sb_dev = dev_find_slot(0, PCI_DEVFN(8, 0));
	do {
		reg32 = pci_ext_read_config32(nb_dev, sb_dev,
					  PCIE_VC0_RESOURCE_STATUS);
	} while (reg32 & VC_NEGOTIATION_PENDING);

	/* step 5.9.1.7 */
	reg32 = nbpcie_p_read_index(dev, 0xa2);
	if (((reg32 & 0x70) >> 4) != 0x6) {
		/* the unused lanes should be powered off. */
	}

	/* step 5.9.1.8 */
	set_pcie_enable_bits(nb_dev, 0x40, 1 << 0, 0 << 0);
}

/*
* GFX Core initialization, dev2, dev3
*/
void rs690_gfx_init(struct device *nb_dev, struct device *dev, u32 port)
{
	u16 reg16;
	struct southbridge_amd_rs690_config *cfg =
	    (struct southbridge_amd_rs690_config *)nb_dev->chip_info;

	printk(BIOS_INFO, "rs690_gfx_init, nb_dev=0x%p, dev=0x%p, port=0x%x.\n",
		    nb_dev, dev, port);

	/* step 0, REFCLK_SEL, skip A11 revision */
	set_nbmisc_enable_bits(nb_dev, 0x6a, 1 << 9,
			       cfg->gfx_dev2_dev3 ? 1 << 9 : 0 << 9);
	printk(BIOS_INFO, "rs690_gfx_init step0.\n");

	/* step 1, lane reversal (only need if CMOS option is enabled) */
	if (cfg->gfx_lane_reversal) {
		set_nbmisc_enable_bits(nb_dev, 0x33, 1 << 2, 1 << 2);
		if (cfg->gfx_dual_slot)
			set_nbmisc_enable_bits(nb_dev, 0x33, 1 << 3, 1 << 3);
	}
	printk(BIOS_INFO, "rs690_gfx_init step1.\n");

	/* step 1.1, dual-slot gfx configuration (only need if CMOS option is enabled) */
	/* AMD calls the configuration CrossFire */
	if (cfg->gfx_dual_slot)
		set_nbmisc_enable_bits(nb_dev, 0x0, 0xf << 8, 5 << 8);
	printk(BIOS_INFO, "rs690_gfx_init step2.\n");

	/* step 2, TMDS, (only need if CMOS option is enabled) */
	if (cfg->gfx_tmds) {
	}

	/* step 3, GFX overclocking, (only need if CMOS option is enabled) */
	/* skip */

	/* step 4, reset the GFX link */
	/* step 4.1 asserts both calibration reset and global reset */
	set_nbmisc_enable_bits(nb_dev, 0x8, 0x3 << 14, 0x3 << 14);

	/* step 4.2 de-asserts calibration reset */
	set_nbmisc_enable_bits(nb_dev, 0x8, 1 << 14, 0 << 14);

	/* step 4.3 wait for at least 200us */
	udelay(200);

	/* step 4.4 de-asserts global reset */
	set_nbmisc_enable_bits(nb_dev, 0x8, 1 << 15, 0 << 15);

	/* step 4.5 asserts both calibration reset and global reset */
	/* a weird step in RPR, don't do that */
	/* set_nbmisc_enable_bits(nb_dev, 0x8, 0x3 << 14, 0x3 << 14); */

	/* step 4.6 bring external GFX device out of reset, wait for 1ms */
	mdelay(1);
	printk(BIOS_INFO, "rs690_gfx_init step4.\n");

	/* step 5 program PCIE memory mapped configuration space */
	/* done by enable_pci_bar3() before */

	/* step 6 SBIOS compile flags */
	if (cfg->gfx_tmds) {
		/* step 6.2.2 Clock-Muxing Control */
		/* step 6.2.2.1 */
		set_nbmisc_enable_bits(nb_dev, 0x7, 1 << 16, 1 << 16);

		/* step 6.2.2.2 */
		set_nbmisc_enable_bits(nb_dev, 0x7, 1 << 8, 1 << 8);
		set_nbmisc_enable_bits(nb_dev, 0x7, 1 << 10, 1 << 10);

		/* step 6.2.2.3 */
		set_nbmisc_enable_bits(nb_dev, 0x7, 1 << 26, 1 << 26);

		/* step 6.2.3 Lane-Muxing Control */
		/* step 6.2.3.1 */
		set_nbmisc_enable_bits(nb_dev, 0x37, 0x3 << 8, 0x2 << 8);

		/* step 6.2.4 Received Data Control */
		/* step 6.2.4.1 */
		set_pcie_enable_bits(nb_dev, 0x40, 0x3 << 16, 0x2 << 16);

		/* step 6.2.4.2 */
		set_pcie_enable_bits(nb_dev, 0x40, 0x3 << 18, 0x3 << 18);

		/* step 6.2.4.3 */
		set_pcie_enable_bits(nb_dev, 0x40, 0x3 << 20, 0x0 << 20);

		/* step 6.2.4.4 */
		set_pcie_enable_bits(nb_dev, 0x40, 0x3 << 22, 0x1 << 22);

		/* step 6.2.5 PLL Power Down Control */
		/* step 6.2.5.1 */
		set_nbmisc_enable_bits(nb_dev, 0x35, 0x3 << 6, 0x0 << 6);

		/* step 6.2.6 Driving Strength Control */
		/* step 6.2.6.1 */
		set_nbmisc_enable_bits(nb_dev, 0x34, 0x1 << 24, 0x0 << 24);

		/* step 6.2.6.2 */
		set_nbmisc_enable_bits(nb_dev, 0x35, 0x3 << 2, 0x3 << 2);
	}

	printk(BIOS_INFO, "rs690_gfx_init step6.\n");

	/* step 7 compliance state, (only need if CMOS option is enabled) */
	/* the compliance state is just for test. refer to 4.2.5.2 of PCIe specification */
	if (cfg->gfx_compliance) {
		/* force compliance */
		set_nbmisc_enable_bits(nb_dev, 0x32, 1 << 6, 1 << 6);
		/* release hold training for device 2. GFX initialization is done. */
		set_nbmisc_enable_bits(nb_dev, 0x8, 1 << 4, 0 << 4);
		dynamic_link_width_control(nb_dev, dev, cfg->gfx_link_width);
		printk(BIOS_INFO, "rs690_gfx_init step7.\n");
		return;
	}

	/* step 8 common initialization */
	/* step 8.1 sets RCB timeout to be 25ms */
	set_pcie_enable_bits(dev, 0x70, 7 << 16, 3 << 16);
	printk(BIOS_INFO, "rs690_gfx_init step8.1.\n");

	/* step 8.2 disables slave ordering logic */
	set_pcie_enable_bits(nb_dev, 0x20, 1 << 8, 1 << 8);
	printk(BIOS_INFO, "rs690_gfx_init step8.2.\n");

	/* step 8.3 sets DMA payload size to 64 bytes */
	set_pcie_enable_bits(nb_dev, 0x10, 7 << 10, 4 << 10);
	printk(BIOS_INFO, "rs690_gfx_init step8.3.\n");

	/* step 8.4 if the LTSSM could not see all 8 TS1 during Polling Active, it can still
	 * time out and go back to Detect Idle.*/
	set_pcie_enable_bits(dev, 0x02, 1 << 14, 1 << 14);
	printk(BIOS_INFO, "rs690_gfx_init step8.4.\n");

	/* step 8.5 shortens the enumeration timer */
	set_pcie_enable_bits(dev, 0x70, 1 << 19, 1 << 19);
	printk(BIOS_INFO, "rs690_gfx_init step8.5.\n");

	/* step 8.6 blocks DMA traffic during C3 state */
	set_pcie_enable_bits(dev, 0x10, 1 << 0, 0 << 0);
	printk(BIOS_INFO, "rs690_gfx_init step8.6.\n");

	/* step 8.7 Do not gate the electrical idle form the PHY
	 * step 8.8 Enables the escape from L1L23 */
	set_pcie_enable_bits(dev, 0xa0, 3 << 30, 3 << 30);
	printk(BIOS_INFO, "rs690_gfx_init step8.8.\n");

	/* step 8.9 Setting this register to 0x1 will workaround a PCI Compliance failure reported by Vista DTM.
	 * SLOT_IMPLEMENTED@PCIE_CAP */
	reg16 = pci_read_config16(dev, 0x5a);
	reg16 |= 0x100;
	pci_write_config16(dev, 0x5a, reg16);
	printk(BIOS_INFO, "rs690_gfx_init step8.9.\n");

	/* step 8.10 Setting this register to 0x1 will hide the Advanced Error Reporting Capabilities in the PCIE Bridge.
	 * This will workaround several failures reported by the PCI Compliance test under Vista DTM. */
	set_nbmisc_enable_bits(nb_dev, 0x33, 1 << 31, 0 << 31);
	printk(BIOS_INFO, "rs690_gfx_init step8.10.\n");

	/* step 8.11 Sets REGS_DLP_IGNORE_IN_L1_EN to ignore DLLPs during L1 so that txclk can be turned off. */
	set_pcie_enable_bits(nb_dev, 0x02, 1 << 0, 1 << 0);
	printk(BIOS_INFO, "rs690_gfx_init step8.11.\n");

	/* step 8.12 Sets REGS_LC_DONT_GO_TO_L0S_IF_L1_ARMED to prevent lc to go to from L0 to Rcv_L0s if L1 is armed. */
	set_pcie_enable_bits(nb_dev, 0x02, 1 << 6, 1 << 6);
	printk(BIOS_INFO, "rs690_gfx_init step8.12.\n");

	/* step 8.13 Sets CMGOOD_OVERRIDE. */
	set_nbmisc_enable_bits(nb_dev, 0x6a, 1 << 17, 1 << 17);
	printk(BIOS_INFO, "rs690_gfx_init step8.13.\n");

	/* step 9 Enable TLP Flushing, for non-AMD GFX devices and Hot-Plug devices only. */
	/* skip */

	/* step 10 Optional Features, only needed if CMOS option is enabled. */
	/* step 10.a: L0s */
	/* enabling L0s in the RS690 GFX port(s) */
	set_pcie_enable_bits(nb_dev, 0xF9, 3 << 13, 2 << 13);
	set_pcie_enable_bits(dev, 0xA0, 0xf << 8, 8 << 8);
	reg16 = pci_read_config16(dev, 0x68);
	reg16 |= 1 << 0;
	/* L0s is intended as a power saving state */
	/* pci_write_config16(dev, 0x68, reg16); */

	/* enabling L0s in the External GFX Device(s) */

	/* step 10.b: active state power management (ASPM L1) */
	/* TO DO */

	/* step 10.c: turning off PLL During L1/L23 */
	set_pcie_enable_bits(nb_dev, 0x40, 1 << 3, 1 << 3);
	set_pcie_enable_bits(nb_dev, 0x40, 1 << 9, 1 << 9);

	/* step 10.d: TXCLK clock gating */
	set_nbmisc_enable_bits(nb_dev, 0x7, 3, 3);
	set_nbmisc_enable_bits(nb_dev, 0x7, 1 << 22, 1 << 22);
	set_pcie_enable_bits(nb_dev, 0x11, 0xf << 4, 0xc << 4);

	/* step 10.e: LCLK clock gating, done in rs690_config_misc_clk() */

	/* step 11 Poll GPIO to determine whether it is single-port or dual-port configuration.
	 * While details will be added later in the document, for now assue the single-port configuration. */
	/* skip */

	/* Single-port/Dual-port configureation. */
	switch (cfg->gfx_dual_slot) {
	case 0:
		single_port_configuration(nb_dev, dev);
		break;
	case 1:
		dual_port_configuration(nb_dev, dev);
		break;
	default:
		printk(BIOS_INFO, "Incorrect configuration of external gfx slot.\n");
		break;
	}
}
