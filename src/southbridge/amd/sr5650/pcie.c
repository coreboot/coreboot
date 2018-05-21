/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <delay.h>
#include "sr5650.h"
#include "cmn.h"

/*------------------------------------------------
* Global variable
------------------------------------------------*/
PCIE_CFG AtiPcieCfg = {
	PCIE_ENABLE_STATIC_DEV_REMAP,	/* Config */
	0,			/* ResetReleaseDelay */
	0,			/* Gfx0Width */
	0,			/* Gfx1Width */
	0,			/* GfxPayload */
	0,			/* GppPayload */
	0,			/* PortDetect, filled by GppSbInit */
	0,			/* PortHp */
	0,			/* DbgConfig */
	0,			/* DbgConfig2 */
	0,			/* GfxLx */
	0,			/* GppLx */
	0,			/* NBSBLx */
	0,			/* PortSlotInit */
	0,			/* Gfx0Pwr */
	0,			/* Gfx1Pwr */
	0			/* GppPwr */
};

static void ValidatePortEn(struct device *nb_dev);

static void ValidatePortEn(struct device *nb_dev)
{
}

/*****************************************************************
* Compliant with CIM_33's PCIEPowerOffGppPorts
* Power off unused GPP lines
*****************************************************************/
static void PciePowerOffGppPorts(struct device *nb_dev, struct device *dev, u32 port)
{
	printk(BIOS_DEBUG, "PciePowerOffGppPorts() port %d\n", port);
	u32 reg;
	u16 state_save;
	uint8_t i;
	struct southbridge_amd_sr5650_config *cfg =
		(struct southbridge_amd_sr5650_config *)nb_dev->chip_info;
	u16 state = cfg->port_enable;

	if (!(AtiPcieCfg.Config & PCIE_DISABLE_HIDE_UNUSED_PORTS))
		state &= AtiPcieCfg.PortDetect;
	state = ~state;
	state &= (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7);
	state_save = state << 17;
	/* Disable ports any that failed training */
	for (i = 9; i <= 13; i++) {
		if (!(AtiPcieCfg.PortDetect & 1 << i)) {
			if ((port >= 9) && (port <= 13)) {
				state |= (1 << (port + 7));
			}
			if (port == 9)
				state_save |= 1 << 25;
			if (port == 10)
				state_save |= 1 << 26;
			if (port == 11)
				state_save |= 1 << 6;
			if (port == 12)
				state_save |= 1 << 7;

			if (port == 13) {
				reg = nbmisc_read_index(nb_dev, 0x2a);
				reg |= 1 << 4;
				nbmisc_write_index(nb_dev, 0x2a, reg);
			}
		}
	}
	state &= !(AtiPcieCfg.PortHp);
	reg = nbmisc_read_index(nb_dev, 0x0c);
	reg |= state;
	nbmisc_write_index(nb_dev, 0x0c, reg);

	reg = nbmisc_read_index(nb_dev, 0x08);
	reg |= state_save;
	nbmisc_write_index(nb_dev, 0x08, reg);

	if ((AtiPcieCfg.Config & PCIE_OFF_UNUSED_GPP_LANES)
	    && !(AtiPcieCfg.
		 Config & (PCIE_DISABLE_HIDE_UNUSED_PORTS +
			   PCIE_GFX_COMPLIANCE))) {
	}
	/* step 3 Power Down Control for Southbridge */
	reg = nbpcie_p_read_index(dev, 0xa2);

	switch ((reg >> 4) & 0x7) {	/* get bit 4-6, LC_LINK_WIDTH_RD */
	case 1:
		nbpcie_ind_write_index(nb_dev, 0x65, 0x0e0e);
		break;
	case 2:
		nbpcie_ind_write_index(nb_dev, 0x65, 0x0c0c);
		break;
	default:
		break;
	}
}

/**********************************************************************
**********************************************************************/
static void switching_gpp1_configurations(struct device *nb_dev, struct device *sb_dev)
{
	u32 reg;
	struct southbridge_amd_sr5650_config *cfg =
	    (struct southbridge_amd_sr5650_config *)nb_dev->chip_info;

	/* 4.3.3.1.1.1.step1. Asserts PCIE-GPP1 global reset */
	reg = nbmisc_read_index(nb_dev, 0x8);
	reg |= 1 << 15;
	nbmisc_write_index(nb_dev, 0x8, reg);

	/* 4.3.3.1.1.1.step2. De-asserts STRAP_BIF_all_valid for PCIE-GPP1 core */
	reg = nbmisc_read_index(nb_dev, 0x26);
	reg |= 1 << 28;
	nbmisc_write_index(nb_dev, 0x26, reg);

	/* 4.3.3.1.1.1.step3. Programs PCIE-GPP1 to be desired port configuration 8:8 or 16:0. */
	reg = nbmisc_read_index(nb_dev, 0x8);
	reg &= ~(1 << 8);		/* clean */
	reg |= cfg->gpp1_configuration << 8;
	nbmisc_write_index(nb_dev, 0x8, reg);

	/* 4.3.3.1.1.1.step4. Wait for 2ms */
	mdelay(1);

	/* 4.3.3.1.1.1.step5. Asserts STRAP_BIF_all_valid for PCIE-GPP1 core */
	reg = nbmisc_read_index(nb_dev, 0x26);
	reg &= ~(1 << 28);
	nbmisc_write_index(nb_dev, 0x26, reg);

	/* 4.3.3.1.1.1.step6. De-asserts PCIE-GPP1 global reset */
	reg = nbmisc_read_index(nb_dev, 0x8);
	reg &= ~(1 << 15);
	nbmisc_write_index(nb_dev, 0x8, reg);

	/* Follow the procedure for PCIE-GPP1 common initialization and
	 * link training sequence. */
}

/**********************************************************************
**********************************************************************/
static void switching_gpp2_configurations(struct device *nb_dev, struct device *sb_dev)
{
	u32 reg;
	struct southbridge_amd_sr5650_config *cfg =
	    (struct southbridge_amd_sr5650_config *)nb_dev->chip_info;

	/* 4.3.3.1.1.2.step1. Asserts PCIE-GPP2 global reset */
	reg = nbmisc_read_index(nb_dev, 0x8);
	reg |= 1 << 13;
	nbmisc_write_index(nb_dev, 0x8, reg);

	/* 4.3.3.1.1.2.step2. De-asserts STRAP_BIF_all_valid for PCIE-GPP2 core */
	reg = nbmisc_read_index(nb_dev, 0x26);
	reg |= 1 << 29;
	nbmisc_write_index(nb_dev, 0x26, reg);

	/* 4.3.3.1.1.2.step3. Programs PCIE-GPP2 to be desired port configuration 8:8 or 16:0. */
	reg = nbmisc_read_index(nb_dev, 0x8);
	reg &= ~(1 << 9);		/* clean */
	reg |= (cfg->gpp2_configuration & 1) << 9;
	nbmisc_write_index(nb_dev, 0x8, reg);

	/* 4.3.3.1.1.2.step4. Wait for 2ms */
	mdelay(2);

	/* 4.3.3.1.1.2.step5. Asserts STRAP_BIF_all_valid for PCIE-GPP2 core */
	reg = nbmisc_read_index(nb_dev, 0x26);
	reg &= ~(1 << 29);
	nbmisc_write_index(nb_dev, 0x26, reg);

	/* 4.3.3.1.1.2.step6. De-asserts PCIE-GPP2 global reset */
	reg = nbmisc_read_index(nb_dev, 0x8);
	reg &= ~(1 << 13);
	nbmisc_write_index(nb_dev, 0x8, reg);

	/* Follow the procedure for PCIE-GPP2 common initialization and
	 * link training sequence. */
}
static void switching_gpp3a_configurations(struct device *nb_dev, struct device *sb_dev)
{
	u32 reg;
	struct southbridge_amd_sr5650_config *cfg =
	    (struct southbridge_amd_sr5650_config *)nb_dev->chip_info;

	/* 4.3.3.2.3.2.step1. Asserts PCIE-GPP3a global reset. */
	reg = nbmisc_read_index(nb_dev, 0x8);
	reg |= 1 << 31;
	nbmisc_write_index(nb_dev, 0x8, reg);
	/* 4.3.3.2.3.2.step2. De-asserts STRAP_BIF_all_valid for PCIE-GPP3a core */
	reg = nbmisc_read_index(nb_dev, 0x26);
	reg |= 1 << 30;
	nbmisc_write_index(nb_dev, 0x26, reg);
	/* 4.3.3.2.3.2.step3. Programs the desired PCIE-GPP3a configuration. */
	reg = nbmisc_read_index(nb_dev, 0x67);
	reg &= ~0x1F;		/* clean */
	reg |= cfg->gpp3a_configuration;
	nbmisc_write_index(nb_dev, 0x67, reg);
	/* 4.3.3.2.3.2.step4. Programs PCIE-GPP3a Line Director. */
	reg = nbmisc_read_index(nb_dev, 0x26);
	reg &= 0xF0000000;	/* TODO:Lane reversed. */
	switch (cfg->gpp3a_configuration) {
	case 0xB:		/* 1:1:1:1:1:1 */
		reg |= 0x2AA3554;
		break;
	case 0x1:		/* 4:2:0:0:0:0 */
		reg |= 0x055B000;
		break;
	case 0x2:		/* 4:1:1:0:0:0 */
		reg |= 0x215B400;
		break;
	case 0xC:		/* 2:2:2:0:0:0 */
		reg |= 0xFF0BAA0;
		break;
	case 0xA:		/* 2:2:1:1:0:0 */
		reg |= 0x215B400;
		break;
	case 0x4:		/* 2:1:1:1:1:0 */
		reg |= 0xFF0BAA0;
		break;
	default:	/* shouldn't be here. */
		printk(BIOS_DEBUG, "Warning:gpp3a_configuration is not correct. Check your devicetree.cb\n");
		break;
	}
	nbmisc_write_index(nb_dev, 0x26, reg);
	/* 4.3.3.2.3.2.step5. De-asserts STRAP_BIF_all_valid for PCIE-GPP3a core */
	reg = nbmisc_read_index(nb_dev, 0x26);
	reg &= ~(1 << 30);
	nbmisc_write_index(nb_dev, 0x26, reg);
	/* 4.3.3.2.3.2.step6. De-asserts PCIE-GPP3a global reset. */
	reg = nbmisc_read_index(nb_dev, 0x8);
	reg &= ~(1 << 31);
	nbmisc_write_index(nb_dev, 0x8, reg);
}

/*****************************************************************
* The sr5650 uses NBCONFIG:0x1c (BAR3) to map the PCIE Extended Configuration
* Space to a 256MB range within the first 4GB of addressable memory.
*****************************************************************/
void enable_pcie_bar3(struct device *nb_dev)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
	set_nbcfg_enable_bits(nb_dev, 0x7C, 1 << 30, 1 << 30);	/* Enables writes to the BAR3 register. */
	set_nbcfg_enable_bits(nb_dev, 0x84, 7 << 16, 0 << 16);

	pci_write_config32(nb_dev, 0x1C, EXT_CONF_BASE_ADDRESS);	/* PCIEMiscInit */
	pci_write_config32(nb_dev, 0x20, 0x00000000);
	set_htiu_enable_bits(nb_dev, 0x32, 1 << 28, 1 << 28);	/* PCIEMiscInit */
	ProgK8TempMmioBase(1, EXT_CONF_BASE_ADDRESS, TEMP_MMIO_BASE_ADDRESS);
}

/*****************************************************************
* We should disable bar3 when we want to exit sr5650_enable, because bar3 will be
* remapped in set_resource later.
*****************************************************************/
void disable_pcie_bar3(struct device *nb_dev)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
	pci_write_config32(nb_dev, 0x1C, 0);	/* clear BAR3 address */
	set_nbcfg_enable_bits(nb_dev, 0x7C, 1 << 30, 0 << 30);	/* Disable writes to the BAR3. */
	ProgK8TempMmioBase(0, EXT_CONF_BASE_ADDRESS, TEMP_MMIO_BASE_ADDRESS);
}

/*
 * GEN2 Software Compliance
 */
void init_gen2(struct device *nb_dev, struct device *dev, u8 port)
{
	u32 reg, val;

	/* for A11 (0x89 == 0) */
	reg = 0x34;
	if (port <= 3) {
		val = 1<<5;
	} else {
		val = 1<<31;
		if (port >= 9)
			reg = 0x39;
	}

	/* TODO: check for rev > a11 */
	switch (port) {
		case 2:
			reg = 0x34;
			val = 1<<5;
			break;
		case 3:
			reg = 0x22;
			val = 1<<6;
			break;
		case 4:
			reg = 0x34;
			val = 1<<31;
			break;
		case 5:
		case 6:
			reg = 0x39;
			val = 1<<31;
			break;
		case 7:
		case 8:
		case 9:
			reg = 0x37;
			val = 1<<port;
			break;
		case 10:
			reg = 0x22;
			val = 1<<5;
			break;
		default:
			reg = 0;
			break;
	}

	/* Enables GEN2 capability of the device */
	set_pcie_enable_bits(dev, 0xA4, 0x1, 0x1);
	/* Advertise the link speed to be Gen2 */
	pci_ext_write_config32(nb_dev, dev, 0x88, 0xF0, 1<<2); /* LINK_CRTL2 */
	set_nbmisc_enable_bits(nb_dev, reg, val, val);
}


/* Alternative to default CPL buffer count */
const u8 pGpp420000[] = {0x38, 0x1C};
const u8 pGpp411000[] = {0x38, 0x0E, 0x0E};
const u8 pGpp222000[] = {0x1C, 0x1C, 0x1C};
const u8 pGpp221100[] = {0x1C, 0x1C, 0x0E, 0x0E};
const u8 pGpp211110[] = {0x1C, 0x0E, 0x0E, 0x0E, 0, 0x0E, 0x0E};
const u8 pGpp111111[] = {0x0E, 0x0E, 0x0E, 0x0E, 0, 0x0E, 0x0E};

/*
 * Enabling Dynamic Slave CPL Buffer Allocation Feature for PCIE-GPP3a Ports
 * PcieLibCplBufferAllocation
 */
static void gpp3a_cpl_buf_alloc(struct device *nb_dev, struct device *dev)
{
	u8 dev_index;
	u8 *slave_cpl;
	u8 value;
	struct southbridge_amd_sr5650_config *cfg =
	    (struct southbridge_amd_sr5650_config *)nb_dev->chip_info;

	dev_index = dev->path.pci.devfn >> 3;
	if (dev_index < 4 || dev_index > 0xa) {
		return;
	}

	switch (cfg->gpp3a_configuration) {
	case 0x1: /* 4:2:0:0:0:0 */
		slave_cpl = (u8 *)&pGpp420000;
		break;
	case 0x2: /* 4:1:1:0:0:0 */
		slave_cpl = (u8 *)&pGpp411000;
		break;
	case 0xC: /* 2:2:2:0:0:0 */
		slave_cpl = (u8 *)&pGpp222000;
		break;
	case 0xA: /* 2:2:1:1:0:0 */
		slave_cpl = (u8 *)&pGpp221100;
		break;
	case 0x4: /* 2:1:1:1:1:0 */
		slave_cpl = (u8 *)&pGpp211110;
		break;
	case 0xB: /* 1:1:1:1:1:1 */
		slave_cpl = (u8 *)&pGpp111111;
		break;
	default:  /* shouldn't be here. */
		printk(BIOS_WARNING, "buggy gpp3a_configuration\n");
		return;
	}

	value = slave_cpl[dev_index - 4];
	if (value != 0) {
		set_pcie_enable_bits(dev, 0x10, 0x3f << 8, value << 8);
		set_pcie_enable_bits(dev, 0x20, 1 << 11, 1 << 11);
	}
}

/*
 * Enabling Dynamic Slave CPL Buffer Allocation Feature for PCIE-GPP1/PCIE-GPP2 Ports
 * PcieLibCplBufferAllocation
 */
static void gpp12_cpl_buf_alloc(struct device *nb_dev, struct device *dev)
{
	u8 gpp_cfg;
	u8 value;
	u8 dev_index;

	dev_index = dev->path.pci.devfn >> 3;
	struct southbridge_amd_sr5650_config *cfg =
	    (struct southbridge_amd_sr5650_config *)nb_dev->chip_info;

	if (dev_index < 4) {
		gpp_cfg = cfg->gpp1_configuration;
	} else if (dev_index > 0xa) {
		gpp_cfg = cfg->gpp2_configuration;
	} else {
		return;
	}

	if (gpp_cfg == 0) {
		/* Configuration 16:0,  leave the default value */
	} else if (gpp_cfg == 1) {
		/* Configuration 8:8 */
		value = 0x60;
		set_pcie_enable_bits(dev, 0x10, 0x3f << 8, value << 8);
		set_pcie_enable_bits(dev, 0x20, 1 << 11, 1 << 11);
	} else {
		printk(BIOS_DEBUG, "buggy gpp configuration\n");
	}
}

#if 1				/* BTS report error without this function. But some board
				 * fail to boot. Leave it here for future debug. */

/*
 * Enable LCLK clock gating
 */
static void EnableLclkGating(struct device *dev)
{
	u8 port;
	u32 reg = 0;
	u32 mask = 0;
	u32 value = 0;
	struct device *nb_dev = dev_find_slot(0, 0);
	struct device *clk_f1= dev_find_slot(0, 1);

	reg = 0xE8;
	port = dev->path.pci.devfn >> 3;
	switch (port) {
 		//PCIE_CORE_INDEX_GPP1
		case 2:
		case 3:
			reg = 0x94;
			mask = 1 << 16;
			break;

 		//PCIE_CORE_INDEX_GPP2
		case 11:
		case 12:
			value = 1 << 28;
			break;

		//PCIE_CORE_INDEX_GPP3a
		case 4 ... 7:
		case 9:
		case 10:
			value = 1 << 31;
			break;

		//PCIE_CORE_INDEX_GPP3b;
		case 13:
			value = 1 << 25;
			break;

 		//PCIE_CORE_INDEX_SB;
		case 8:
			reg = 0x94;
			mask = 1 << 24;
			break;
		default:
			break;
	}
	/* enable access func1 */
	set_nbcfg_enable_bits(nb_dev, 0x4C, 1 << 0, 1 << 0);
	set_nbcfg_enable_bits(clk_f1, reg, mask, value);
}
#endif

/*****************************************
* Compliant with CIM_33's PCIEGPPInit
* nb_dev:
*	root bridge struct
* dev:
*	p2p bridge struct
* port:
*	p2p bridge number, 4-10
*****************************************/
void sr5650_gpp_sb_init(struct device *nb_dev, struct device *dev, u32 port)
{
	uint8_t training_ok = 1;

	u32 gpp_sb_sel = 0;
	struct southbridge_amd_sr5650_config *cfg =
	    (struct southbridge_amd_sr5650_config *)nb_dev->chip_info;

	printk(BIOS_DEBUG, "%s: nb_dev=0x%p, dev=0x%p, port=0x%x\n", __func__, nb_dev, dev, port);
	switch (port) {
	case 2:
	case 3:
		gpp_sb_sel = PCIE_CORE_INDEX_GPP1;
		break;
	case 4 ... 7:
	case 9:
	case 10:
		gpp_sb_sel = PCIE_CORE_INDEX_GPP3a;
		break;
	case 8:
		gpp_sb_sel = PCIE_CORE_INDEX_SB;
		break;
	case 11:
	case 12:
		gpp_sb_sel = PCIE_CORE_INDEX_GPP2;
		break;
	case 13:
		gpp_sb_sel = PCIE_CORE_INDEX_GPP3b;
		break;
	}

	/* Init common Core registers */
	set_pcie_enable_bits(dev, 0xB1, 1 << 28 | 1 << 23 | 1 << 20 | 1 << 19,
		1 << 28 | 1 << 23 | 1 << 20 | 1 << 19);
	if (gpp_sb_sel == PCIE_CORE_INDEX_GPP3a) {
		set_pcie_enable_bits(dev, 0xB1, 1 << 22, 1 << 22);
		/* 4.3.3.2.3 Step 10: Dynamic Slave CPL Buffer Allocation */
		gpp3a_cpl_buf_alloc(nb_dev, dev);
	}
	if (gpp_sb_sel == PCIE_CORE_INDEX_GPP1 || gpp_sb_sel == PCIE_CORE_INDEX_GPP2) {
		gpp12_cpl_buf_alloc(nb_dev, dev);
	}
	set_pcie_enable_bits(dev, 0xA1, (1 << 26) | (1 << 24) | (1 << 11), 1 << 11);
	set_pcie_enable_bits(dev, 0xA0, 0x0000FFF0, 0x6830);
	// PCIE should not ignore malformed packet error or ATS request
	set_pcie_enable_bits(dev, 0x70, 1 << 12, 0);
	//Step 14.1: Advertising Hot Plug Capabilities
	set_pcie_enable_bits(dev, 0x10, 1 << 4, 1 << 4); //Enable power fault

	set_pcie_enable_bits(nb_dev, 0xC1 | gpp_sb_sel, 1 << 0, 1 << 0);

	/* init GPP core */
	/* 4.4.2.step13.1. Sets RCB completion timeout to be 200ms */
	pci_ext_write_config32(nb_dev, dev, 0x80, 0xF << 0, 0x6 << 0);
	/* 4.4.2.step13.2. RCB completion timeout on link down to shorten enumeration time. */
	set_pcie_enable_bits(dev, 0x70, 1 << 19, 1 << 19);
	/* 4.4.2.step13.3. Enable slave ordering rules */
	set_pcie_enable_bits(nb_dev, 0x20 | gpp_sb_sel, 1 << 8, 0 << 8);
	/* 4.4.2.step13.4. Sets DMA payload size to 64 bytes. */
	set_pcie_enable_bits(nb_dev, 0x10 | gpp_sb_sel, 7 << 10, 4 << 10);
	/* 4.4.2.step13.5. Set REGS_DLP_IGNORE_IN_L1_EN to ignore DLLPs
	   during L1 so that Tx Clk can be turned off. */
	set_pcie_enable_bits(nb_dev, 0x02 | gpp_sb_sel, 1 << 0 | 1 << 8, 1 << 0 | 1 << 8); // add bit 8 from CIMx
	/* 4.4.2.step13.6. Set REGS_LC_ALLOW_TX_L1_CONTROL to allow TX to
	   prevent LC from going to L1 when there are outstanding completions.*/
	set_pcie_enable_bits(dev, 0x02, 1 << 15, 1 << 15);

	/* Enables the PLL power down when all lanes are inactive.
	 * It should be on in GPP.
	 */
	if (gpp_sb_sel == PCIE_CORE_INDEX_GPP3a || gpp_sb_sel == PCIE_CORE_INDEX_GPP3b || gpp_sb_sel == PCIE_CORE_INDEX_SB) {
		set_pcie_enable_bits(nb_dev, 0x02 | gpp_sb_sel, 1 << 3, 1 << 3);
	}

	/* 4.4.2.step13.7. Set REGS_LC_DONT_GO_TO_L0S_IF_L1_ARMED to prevent
	   lc to go to from L0 to Rcv_L0s if L1 is armed. */
	set_pcie_enable_bits(dev, 0xA1, 1 << 11, 1 << 11);
	/* 4.4.2.step13.8. CMGOOD_OVERRIDE for all five PCIe cores. */
	set_nbmisc_enable_bits(nb_dev, 0x22, 1 << 27, 1 << 27);
	/* 4.4.2.step13.9. Prevents Electrical Idle from causing a
	   transition from Rcv_L0 to Rcv_L0s. */
	set_pcie_enable_bits(dev, 0xB1, 1 << 20, 1 << 20);
	/* 4.4.2.step13.10. Prevents the LTSSM from going to Rcv_L0s if
	   it has already acknowledged a request to go
	   to L1 but it has not transitioned there yet. */
	/* seems the same as step13.7 */
	set_pcie_enable_bits(dev, 0xA1, 1 << 11, 1 << 11);
	/* 4.4.2.step13.11. Transmits FTS before Recovery. */
	set_pcie_enable_bits(dev, 0xA3, 1 << 9, 1 << 9);
	/* 4.4.2.step13.12. Sets TX arbitration algorithm to round robin
	   for PCIE-GPP1, PCIE-GPP2, PCIE-GPP3a and PCIE-GPP3b cores only. */
	//if (gpp_sb_sel != PCIE_CORE_INDEX_SB) /* RPR NOT set SB_CORE, BTS set SB_CORE, we comply with BTS */
		set_pcie_enable_bits(nb_dev, 0x1C | gpp_sb_sel, 0x7FF, 0x109);
	/* 4.4.2.step13.13. Sets number of TX Clocks to drain TX Pipe to 0x3.*/
	set_pcie_enable_bits(dev, 0xA0, 0xF << 4, 0x3 << 4);
	/* 4.4.2.step13.14. Lets PI use Electrical Idle from PHY when
	   turning off PLL in L1 at Gen 2 speed instead of Inferred Electrical
	   Idle.
	   NOTE: LC still uses Inferred Electrical Idle. */
	set_pcie_enable_bits(nb_dev, 0x40 | gpp_sb_sel, 3 << 14, 2 << 14);
	/* 4.4.2.step13.15. Turn on rx_fronten_en for all active lanes upon
	   exit from Electrical Idle, rather than being tied to PLL_PDNB. */
	set_pcie_enable_bits(nb_dev, 0xC2 | gpp_sb_sel, 1 << 25, 1 << 25);

	/* 4.4.2.step13.16. Advertises TX L0s and L1 exit latency.
	   TX L0s exit latency to be 100b: 512ns to less than 1us;
	   L1 exit latency to be 011b: 4us to less than 8us.
	   For Hot-Plug Slots: Advertise TX L0s and L1 exit latency.
	   TX L0s exit latency to be 110b: 2us to 4us.
	   L1 exit latency to be 111b: more than 64us.*/
	//set_pcie_enable_bits(dev, 0xC1, 0xF << 0, 0xC << 0); /* 0xF for hotplug. */
	set_pcie_enable_bits(dev, 0xC1, 0xF << 0, 0xF << 0); /* 0xF for hotplug. */
	/* 4.4.2.step13.17. Always ACK an ASPM L1 entry DLLP to
	   workaround credit control issue on PM_NAK
	   message of SB700 and SB800. */
	/* 4.4.4.step13.18. To allow advertising Gen 2 capabilities to Southbridge. */
	if (port == 8) {
		set_pcie_enable_bits(dev, 0xA0, 1 << 23, 1 << 23);
		set_pcie_enable_bits(nb_dev, 0xC1 | gpp_sb_sel, 1 << 1, 1 << 1);
	}
	/* 4.4.2.step13.19. CMOS Option (Gen 2 AUTO-Part 1 - Enabled by Default) */
	/* 4.4.2.step13.20. CMOS Option (RC Advertised Gen 2-Part1 - Disabled by Default)*/
	set_nbcfg_enable_bits(dev, 0x88, 0xF << 0, 0x2 << 0);
	/* Disables GEN2 capability of the device.
	 * RPR typo- it says enable but the bit setting says disable.
	 * Disable it here and we enable it later. */
	set_pcie_enable_bits(dev, 0xA4, 1 << 0, 1 << 0);

	/* 4.4.2.step13.21. Legacy Hot Plug  -CMOS Option */
	/* NOTE: This feature can be enabled only for Hot-Plug slots implemented on SR5690 platform. */

	/* 4.4.2.step13.22. Native PCIe Mode -CMOS Option */
	/* Enable native PME. */
	set_pcie_enable_bits(dev, 0x10, 1 << 3, 1 < 3);
	/* This bit when set indicates that the PCIe Link associated with this port
	   is connected to a slot. */
	pci_ext_write_config32(nb_dev, dev, 0x5a, 1 << 8, 1 << 8);
	/* This bit when set indicates that this slot is capable of supporting
	   Hot-Plug operations. */
	set_nbcfg_enable_bits(dev, 0x6C, 1 << 6, 1 << 6);
	/* Enables flushing of TLPs when Data Link is down. */
	set_pcie_enable_bits(dev, 0x20, 1 << 19, 0 << 19);

	/* 4.4.2.step14. Server Class Hot Plug Feature. NOTE: This feature is not supported on SR5670 and SR5650 */
	/* 4.4.2 step14.1: Advertising Hot Plug Capabilities */
	/* 4.4.2.step14.2: Firmware Upload */
	/* 4.4.2.Step14.3: SBIOS Acknowledgment to Firmware of Successful Firmware Upload */
	/* step14.4 */
	/* step14.5 */
	/* skip */

	/* CIMx LPC Deadlock workaround - Enable Memory Write Map*/
	if (gpp_sb_sel == PCIE_CORE_INDEX_SB) {
		set_pcie_enable_bits(nb_dev, 0x10 | gpp_sb_sel, 1 << 9, 1 << 9);
		set_htiu_enable_bits(nb_dev, 0x06, 1 << 26, 1 << 26);
	}

	/* This CPL setup requires more than this one register and should be done in gpp_core.
	 * The additional setup is for the different revisions. */

	/* CIMx CommonPortInit settings that are not set above. */
	pci_ext_write_config32(nb_dev, dev, 0x88, 0xF0, 1 << 0); /* LINK_CRTL2 */

	if ( port == 8 )
		set_pcie_enable_bits(dev, 0xA0, 0, 1 << 23);

#if 0 //SR56x0 pcie Gen2 code is not tested yet, we should enable it again when test finished.
	/* set automatic Gen2 support, needs mainboard config option as Gen2 can cause issues on some platforms. */
	init_gen2(nb_dev, dev, port);
	set_pcie_enable_bits(dev, 0xA4, 1 << 29, 1 << 29);
	set_pcie_enable_bits(dev, 0xC0, 1 << 15, 0);
	set_pcie_enable_bits(dev, 0xA2, 1 << 13, 0);
#endif

	/* Hotplug Support - bit5 + bit6  capable and surprise */
	pci_ext_write_config32(nb_dev, dev, 0x6c, 0x60, 0x60);

	/* Set interrupt pin info 0x3d */
	pci_ext_write_config32(nb_dev, dev, 0x3c, 1 << 8, 1 << 8);

	/* 5.12.9.3 Hotplug step 1 - NB_PCIE_ROOT_CTRL - enable pm irq
	The RPR is wrong - this is not a PCIEND_P register */
	pci_ext_write_config32(nb_dev, dev, 0x74, 1 << 3, 1 << 3);

	/* 5.12.9.3 step 2 - PCIEP_PORT_CNTL - enable hotplug messages */
	if ( port != 8)
		set_pcie_enable_bits(dev, 0x10, 1 << 2, 1 << 2);

	/* Not sure about this PME setup */
	/* Native PME */
	set_pcie_enable_bits(dev, 0x10, 1 << 3, 1 << 3); /* Not set in CIMx */

	/* PME Enable */
	pci_ext_write_config32(nb_dev, dev, 0x54, 1 << 8, 1 << 8); /* Not in CIMx */

	/* 4.4.3 Training for GPP devices */
	/* init GPP */
	switch (port) {
	case 2:
	case 3:
	case 4:	/* GPP_SB */
	case 5:
	case 6:
	case 7:
	case 9:	/*GPP*/
	case 10:
	case 11:
	case 12:
	case 13:
		/* 4.4.2.step13.5. Blocks DMA traffic during C3 state */
		set_pcie_enable_bits(dev, 0x10, 1 << 0, 0 << 0);
		/* Enables TLP flushing */
		set_pcie_enable_bits(dev, 0x20, 1 << 19, 0 << 19);

		/* check port enable */
		if (cfg->port_enable & (1 << port)) {
			uint32_t hw_port = port;
			switch (cfg->gpp3a_configuration) {
			case 0x1: /* 4:2:0:0:0:0 */
				if (hw_port == 9)
					hw_port = 4 + 1;
				break;
			case 0x2: /* 4:1:1:0:0:0 */
				if (hw_port == 9)
					hw_port = 4 + 1;
				else if (hw_port == 10)
					hw_port = 4 + 2;
				break;
			case 0xc: /* 2:2:2:0:0:0 */
				if (hw_port == 6)
					hw_port = 4 + 1;
				else if (hw_port == 9)
					hw_port = 4 + 2;
				break;
			case 0xa: /* 2:2:1:1:0:0 */
				if (hw_port == 6)
					hw_port = 4 + 1;
				else if (hw_port == 9)
					hw_port = 4 + 2;
				else if (hw_port == 10)
					hw_port = 4 + 3;
				break;
			case 0x4: /* 2:1:1:1:1:0 */
				if (hw_port == 6)
					hw_port = 4 + 1;
				else if (hw_port == 7)
					hw_port = 4 + 2;
				else if (hw_port == 9)
					hw_port = 4 + 3;
				else if (hw_port == 10)
					hw_port = 4 + 4;
				break;
			case 0xb: /* 1:1:1:1:1:1 */
				break;
			default:  /* shouldn't be here. */
				printk(BIOS_WARNING, "invalid gpp3a_configuration\n");
				return;
			}
			PcieReleasePortTraining(nb_dev, dev, hw_port);
			if (!(AtiPcieCfg.Config & PCIE_GPP_COMPLIANCE)) {
				u8 res = PcieTrainPort(nb_dev, dev, hw_port);
				printk(BIOS_DEBUG, "%s: port=0x%x hw_port=0x%x result=%d\n",
					__func__, port, hw_port, res);
				if (res) {
					AtiPcieCfg.PortDetect |= 1 << port;
				} else {
					/* Even though nothing is attached to this port
					 * the port needs to be "enabled" to obtain
					 * a bus number from the PCI resource allocator
					 */
					training_ok = 0;
					dev->enabled = 1;
				}
			}
		}
		break;
	case 8:		/* SB */
		break;
	default:
		break;
	}

	/* Re-enable RC ordering logic after training (from CIMx)*/
	set_pcie_enable_bits(nb_dev, 0x20 | gpp_sb_sel, 1 << 9, 0);

	/* Advertising Hot Plug Capabilities */
	pci_ext_write_config32(nb_dev, dev, 0x6c, 0x04001B, 0x00001B);

	/* PCIE Late Init (CIMx late init - Maybe move somewhere else? Later in the coreboot PCI device enum?) */
	/* Set Slot Number */
	pci_ext_write_config32(nb_dev, dev, 0x6c, 0x1FFF << 19, port << 19);

	/* Set Slot present 0x5A*/
	pci_ext_write_config32(nb_dev, dev, 0x58, 1 << 24, 1 << 24);

	//PCIE-GPP1 TXCLK Clock Gating In L1  Late Core setting - Maybe move somewhere else? */
	set_pcie_enable_bits(nb_dev, 0x11 | gpp_sb_sel, 0xF << 0, 0x0C << 0);
	/* Enable powering down PLLs in L1 or L23 Ready states.
	 * Turns off PHY`s RX FRONTEND during L1 when PLL power down is enabled */
	set_pcie_enable_bits(nb_dev, 0x40 | gpp_sb_sel, 0x1219, 0x1009);
	/* 4.4..7.1 TXCLK Gating in L1, Enables powering down TXCLK clock pads on the receive side. */
	set_pcie_enable_bits(nb_dev, 0x40 | gpp_sb_sel, 1 << 6, 1 << 6);

	/* Step 20: Disables immediate RCB timeout on link down */
	if (!((pci_read_config32(dev, 0x6C ) >> 6) & 0x01)) {
		set_pcie_enable_bits(dev, 0x70, 1 << 19, 0 << 19);
	}

	/* Step 27: LCLK Gating	*/
	EnableLclkGating(dev);

	/* Set Common Clock */
	/* If dev present, set PcieCapPtr+0x10, BIT6);
	 * set dev 0x68,bit 6
	 * retrain link, set dev, 0x68 bit 5;
	 * wait dev 0x6B bit3 clear
	 */

	if ((port == 8) || (!training_ok)) {
		PciePowerOffGppPorts(nb_dev, dev, port);	/* This is run for all ports that are not hotplug and don't detect devices */
	}
}

/**
 * Step 21: Register Locking
 * Lock HWInit Register of each pcie core
 */
static void lock_hwinitreg(struct device *nb_dev)
{
	/* Step 21: Register Locking, Lock HWInit Register */
	set_pcie_enable_bits(nb_dev, 0x10 | PCIE_CORE_INDEX_GPP1, 1 << 0, 1 << 0);
	set_pcie_enable_bits(nb_dev, 0x10 | PCIE_CORE_INDEX_SB, 1 << 0, 1 << 0);
	set_pcie_enable_bits(nb_dev, 0x10 | PCIE_CORE_INDEX_GPP2, 1 << 0, 1 << 0);
	set_pcie_enable_bits(nb_dev, 0x10 | PCIE_CORE_INDEX_GPP3a, 1 << 0, 1 << 0);
	set_pcie_enable_bits(nb_dev, 0x10 | PCIE_CORE_INDEX_GPP3b, 1 << 0, 1 << 0);
}

/**
 * Lock HWInit Register
 */
void sr56x0_lock_hwinitreg(void)
{
	struct device *nb_dev = dev_find_slot(0, PCI_DEVFN(0, 0));

	/* Lock HWInit Register */
	lock_hwinitreg(nb_dev);

	/* Lock HWInit Register NBMISCIND:0x0 NBCNTL[7] HWINIT_WR_LOCK */
	set_nbmisc_enable_bits(nb_dev, 0x00, 1 << 7, 1 << 7);

	/* Hide clock configuration PCI device HIDE_CLKCFG_HEADER */
	set_nbmisc_enable_bits(nb_dev, 0x00, 0x00000100, 1 << 8);
}

/*****************************************
* Compliant with CIM_33's PCIEConfigureGPPCore
*****************************************/
void config_gpp_core(struct device *nb_dev, struct device *sb_dev)
{
	u32 reg;

	reg = nbmisc_read_index(nb_dev, 0x20);
	if (AtiPcieCfg.Config & PCIE_ENABLE_STATIC_DEV_REMAP)
		reg &= 0xfffffffd;	/* set bit1 = 0 */
	else
		reg |= 0x2;	/* set bit1 = 1 */
	nbmisc_write_index(nb_dev, 0x20, reg);

	/* Must perform PCIE-GPP1, GPP2, GPP3a global reset anyway */
	reg = nbmisc_read_index(nb_dev, 0x8);
	reg |= (1 << 31) | (1 << 15) | (1 << 13);	//asserts
	nbmisc_write_index(nb_dev, 0x8, reg);
	reg &= ~((1 << 31) | (1 << 15) | (1 << 13));	//De-asserts
	nbmisc_write_index(nb_dev, 0x8, reg);

	switching_gpp3a_configurations(nb_dev, sb_dev);
	switching_gpp1_configurations(nb_dev, sb_dev);
	switching_gpp2_configurations(nb_dev, sb_dev);
	ValidatePortEn(nb_dev);
}

/*****************************************
* Compliant with CIM_33's PCIEMiscClkProg
*****************************************/
void pcie_config_misc_clk(struct device *nb_dev)
{
	u32 reg;
	//struct bus pbus; /* fake bus for dev0 fun1 */

	reg = pci_read_config32(nb_dev, 0x4c);
	reg |= 1 << 0;
	pci_write_config32(nb_dev, 0x4c, reg);

#if 0				/* TODO: Check the mics clock later. */
	if (AtiPcieCfg.Config & PCIE_GFX_CLK_GATING) {
		/* TXCLK Clock Gating */
		set_nbmisc_enable_bits(nb_dev, 0x07, 3 << 0, 3 << 0);
		set_nbmisc_enable_bits(nb_dev, 0x07, 1 << 22, 1 << 22);
		set_pcie_enable_bits(nb_dev, 0x11 | PCIE_CORE_INDEX_GFX, (3 << 6) | (~0xf), 3 << 6);

		/* LCLK Clock Gating */
		reg =  pci_cf8_conf1.read32(&pbus, 0, 1, 0x94);
		reg &= ~(1 << 16);
		pci_cf8_conf1.write32(&pbus, 0, 1, 0x94, reg);
	}

	if (AtiPcieCfg.Config & PCIE_GPP_CLK_GATING) {
		/* TXCLK Clock Gating */
		set_nbmisc_enable_bits(nb_dev, 0x07, 3 << 4, 3 << 4);
		set_nbmisc_enable_bits(nb_dev, 0x07, 1 << 22, 1 << 22);
		set_pcie_enable_bits(nb_dev, 0x11 | PCIE_CORE_INDEX_SB, (3 << 6) | (~0xf), 3 << 6);

		/* LCLK Clock Gating */
		reg =  pci_cf8_conf1.read32(&pbus, 0, 1, 0x94);
		reg &= ~(1 << 24);
		pci_cf8_conf1.write32(&pbus, 0, 1, 0x94, reg);
	}
#endif

	reg = pci_read_config32(nb_dev, 0x4c);
	reg &= ~(1 << 0);
	pci_write_config32(nb_dev, 0x4c, reg);
}
