/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
#include "rs780.h"

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

static void PciePowerOffGppPorts(struct device *nb_dev, struct device *dev, u32 port);
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
	u32 reg;
	u16 state_save;
	struct southbridge_amd_rs780_config *cfg =
	    (struct southbridge_amd_rs780_config *)nb_dev->chip_info;
	u8 state = cfg->port_enable;

	if (!(AtiPcieCfg.Config & PCIE_DISABLE_HIDE_UNUSED_PORTS))
		state &= AtiPcieCfg.PortDetect;
	state = ~state;
	state &= (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7);
	state_save = state << 17;
	state &= ~(AtiPcieCfg.PortHp);
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
	if (port != 8)
		return;

	reg = nbpcie_p_read_index(dev, 0xa2);

	switch ((reg >> 4) & 0x7) {	/* get bit 4-6, LC_LINK_WIDTH_RD */
	case 1:
		set_pcie_enable_bits(nb_dev, 0x65 | PCIE_CORE_INDEX_GPPSB,
				     0x0f0f, 0x0e0e);
		break;
	case 2:
		set_pcie_enable_bits(nb_dev, 0x65 | PCIE_CORE_INDEX_GPPSB,
				     0x0f0f, 0x0c0c);
		break;
	default:
		break;
	}
}

/**********************************************************************
**********************************************************************/
static void switching_gppsb_configurations(struct device *nb_dev, struct device *sb_dev)
{
	u32 reg;
	struct southbridge_amd_rs780_config *cfg =
	    (struct southbridge_amd_rs780_config *)nb_dev->chip_info;

	/* 5.5.7.1-3 enables GPP reconfiguration */
	reg = nbmisc_read_index(nb_dev, PCIE_NBCFG_REG7);
	reg |=
	    (RECONFIG_GPPSB_EN + RECONFIG_GPPSB_LINK_CONFIG +
	     RECONFIG_GPPSB_ATOMIC_RESET);
	nbmisc_write_index(nb_dev, PCIE_NBCFG_REG7, reg);

	/* 5.5.7.4a. De-asserts STRAP_BIF_all_valid for PCIE-GPPSB core */
	reg = nbmisc_read_index(nb_dev, 0x66);
	reg |= 1 << 31;
	nbmisc_write_index(nb_dev, 0x66, reg);
	/* 5.5.7.4b. sets desired GPPSB configurations, bit4-7 */
	reg = nbmisc_read_index(nb_dev, 0x67);
	reg &= 0xFFFFff0f;		/* clean */
	reg |= cfg->gppsb_configuration << 4;
	nbmisc_write_index(nb_dev, 0x67, reg);

#if 1
	/* NOTE:
	 * In CIMx 4.5.0 and RPR, 4c is done before 5 & 6. But in this way,
	 * a x4 device in port B (dev 4) of Configuration B can only be detected
	 * as x1, instead of x4. When the port B is being trained, the
	 * LC_CURRENT_STATE is 6 and the LC_LINK_WIDTH_RD is 1. We have
	 * to set the PCIEIND:0x65 as 0xE0E0 and reset the slot. Then the card
	 * seems to work in x1 mode.
	 * In the 2nd way below, we do the 5 & 6 before 4c. it conforms the
	 * CIMx 4.3.0. It conflicts with RPR. But based on the test result I've
	 * made so far, I haven't found any mistake.
	 */
	/* 5.5.7.4c. Asserts STRAP_BIF_all_valid for PCIE-GPPSB core */
	reg = nbmisc_read_index(nb_dev, 0x66);
	reg &= ~(1 << 31);
	nbmisc_write_index(nb_dev, 0x66, reg);

	/* 5.5.7.5-6. read bit14 and write back its inverted value */
	reg = nbmisc_read_index(nb_dev, PCIE_NBCFG_REG7);
	reg ^= RECONFIG_GPPSB_GPPSB;
	nbmisc_write_index(nb_dev, PCIE_NBCFG_REG7, reg);
#else
	/* 5.5.7.5-6. read bit14 and write back its inverted value */
	reg = nbmisc_read_index(nb_dev, PCIE_NBCFG_REG7);
	reg ^= RECONFIG_GPPSB_GPPSB;
	nbmisc_write_index(nb_dev, PCIE_NBCFG_REG7, reg);

	/* 5.5.7.4c. Asserts STRAP_BIF_all_valid for PCIE-GPPSB core */
	reg = nbmisc_read_index(nb_dev, 0x66);
	reg &= ~(1 << 31);
	nbmisc_write_index(nb_dev, 0x66, reg);
#endif
	/* 5.5.7.7. delay 1ms */
	mdelay(1);

	/* 5.5.7.8. waits until SB has trained to L0, poll for bit0-5 = 0x10 */
	do {
		reg = nbpcie_p_read_index(sb_dev, PCIE_LC_STATE0);
		reg &= 0x3f;	/* remain LSB [5:0] bits */
	} while (LC_STATE_RECONFIG_GPPSB != reg);

	/* 5.5.7.9.ensures that virtual channel negotiation is completed. poll for bit1 = 0 */
	do {
		reg =
		    pci_ext_read_config32(nb_dev, sb_dev,
					  PCIE_VC0_RESOURCE_STATUS);
	} while (reg & VC_NEGOTIATION_PENDING);
}

static void switching_gpp_configurations(struct device *nb_dev, struct device *sb_dev)
{
	u32 reg;
	struct southbridge_amd_rs780_config *cfg =
	    (struct southbridge_amd_rs780_config *)nb_dev->chip_info;

	/* 5.6.2.1. De-asserts STRAP_BIF_all_valid for PCIE-GPP core */
	reg = nbmisc_read_index(nb_dev, 0x22);
	reg |= 1 << 14;
	nbmisc_write_index(nb_dev, 0x22, reg);
	/* 5.6.2.2. sets desired GPP configurations, bit7-10 */
	reg = nbmisc_read_index(nb_dev, 0x2D);
	reg &= ~(0xF << 7);		/* clean */
	reg |= cfg->gpp_configuration << 7;
	nbmisc_write_index(nb_dev, 0x2D, reg);
	/* 5.6.2.3. Asserts STRAP_BIF_all_valid for PCIE-GPP core */
	reg = nbmisc_read_index(nb_dev, 0x22);
	reg &= ~(1 << 14);
	nbmisc_write_index(nb_dev, 0x22, reg);
}

/*****************************************************************
* The rs780 uses NBCONFIG:0x1c (BAR3) to map the PCIE Extended Configuration
* Space to a 256MB range within the first 4GB of addressable memory.
*****************************************************************/
void enable_pcie_bar3(struct device *nb_dev)
{
	printk(BIOS_DEBUG, "enable_pcie_bar3()\n");
	set_nbcfg_enable_bits(nb_dev, 0x7C, 1 << 30, 1 << 30);	/* Enables writes to the BAR3 register. */
	set_nbcfg_enable_bits(nb_dev, 0x84, 7 << 16, 0 << 16);

	pci_write_config32(nb_dev, 0x1C, EXT_CONF_BASE_ADDRESS);	/* PCIEMiscInit */
	pci_write_config32(nb_dev, 0x20, 0x00000000);
	set_htiu_enable_bits(nb_dev, 0x32, 1 << 28, 1 << 28);	/* PCIEMiscInit */
	ProgK8TempMmioBase(1, EXT_CONF_BASE_ADDRESS, TEMP_MMIO_BASE_ADDRESS);
}

/*****************************************************************
* We should disable bar3 when we want to exit rs780_enable, because bar3 will be
* remapped in set_resource later.
*****************************************************************/
void disable_pcie_bar3(struct device *nb_dev)
{
	printk(BIOS_DEBUG, "disable_pcie_bar3()\n");
	pci_write_config32(nb_dev, 0x1C, 0);	/* clear BAR3 address */
	set_nbcfg_enable_bits(nb_dev, 0x7C, 1 << 30, 0 << 30);	/* Disable writes to the BAR3. */
	set_htiu_enable_bits(nb_dev, 0x32, 1 << 28, 0);	/* disable bar3 decode */
	ProgK8TempMmioBase(0, EXT_CONF_BASE_ADDRESS, TEMP_MMIO_BASE_ADDRESS);
}

/*****************************************
* Compliant with CIM_33's PCIEGPPInit
* nb_dev:
*	root bridge struct
* dev:
*	p2p bridge struct
* port:
*	p2p bridge number, 4-10
*****************************************/
void rs780_gpp_sb_init(struct device *nb_dev, struct device *dev, u32 port)
{
	u32 gfx_gpp_sb_sel;
	struct southbridge_amd_rs780_config *cfg =
	    (struct southbridge_amd_rs780_config *)nb_dev->chip_info;
	printk(BIOS_DEBUG, "gpp_sb_init nb_dev=0x%x, dev=0x%x, port=0x%x\n", nb_dev->path.pci.devfn, dev->path.pci.devfn, port);

	gfx_gpp_sb_sel = port >= 4 && port <= 8 ?
				PCIE_CORE_INDEX_GPPSB :		/* 4,5,6,7,8 */
				PCIE_CORE_INDEX_GPP;		/* 9,10 */
	/* init GPP core */
	/* 5.10.8.3. Disable slave ordering logic */
	set_pcie_enable_bits(nb_dev, 0x20 | gfx_gpp_sb_sel, 1 << 8,
			     1 << 8);
	/* 5.10.8.7. PCIE initialization 5.10.2: rpr 2.12*/
	set_pcie_enable_bits(nb_dev, 0x02 | gfx_gpp_sb_sel, 1 << 0, 1 << 0);	/* no description in datasheet. */

	/* init GPPSB port. rpr 5.10.8 */
	/* 5.10.8.1-5.10.8.2. Sets RCB timeout to be 100ms/4=25ms by setting bits[18:16] to 3 h4
	 * and shortens the enumeration timer by setting bit[19] to 1
	 */
	set_pcie_enable_bits(dev, 0x70, 0xF << 16, 0x4 << 16 | 1 << 19);
	/* 5.10.8.4. Sets DMA payload size to 64 bytes. */
	set_pcie_enable_bits(nb_dev, 0x10 | gfx_gpp_sb_sel, 7 << 10, 4 << 10);
	/* 5.10.8.6. Disable RC ordering logic */
	set_pcie_enable_bits(nb_dev, 0x20 | gfx_gpp_sb_sel, 1 << 9, 1 << 9);
	/* 5.10.8.7. Ignores DLLs druing L1 */
	set_pcie_enable_bits(nb_dev, 0x02 | gfx_gpp_sb_sel, 1 << 0, 1 << 0);
	/* 5.10.8.8. Prevents LCto go from L0 to Rcv_L0s if L1 is armed. */
	set_pcie_enable_bits(dev, 0xA1, 1 << 11, 1 << 11);
	/* 5.10.8.9. Sets timer in Config state from 20us to 1us.
	 * 5.10.8.10. De-asserts RX_EN in L0s
	 * 5.10.8.11. Enables de-assertion of PG2RX_CR_EN to lock clock recovery parameter when .. */
	set_pcie_enable_bits(dev, 0xB1, 1 << 23 | 1 << 19 | 1 << 28, 1 <<23 | 1 << 19 | 1 << 28);
	/* 5.10.8.12. Turns off offset calibration */
	/* 5.10.8.13. Enables Rx  Clock gating in CDR */
	if (gfx_gpp_sb_sel == PCIE_CORE_INDEX_GPPSB)
		set_nbmisc_enable_bits(nb_dev, 0x67, 1 << 14 | 1 << 26, 1 << 14 | 1 << 26); /* 4,5,6,7 */
	else
		set_nbmisc_enable_bits(nb_dev, 0x24, 1 << 29 | 1 << 28, 1 << 29 | 1 << 28); /* 9,10 */
	/* 5.10.8.14. Sets number of TX Clocks to drain TX Pipe to 3 */
	set_pcie_enable_bits(dev, 0xA0, 0xF << 4, 0x3 << 4);
	/* 5.10.8.15. empty */
	/* 5.10.8.16. P_ELEC_IDLE_MODE */
	set_pcie_enable_bits(nb_dev, 0x40 | gfx_gpp_sb_sel, 0x3 << 14, 0x2 << 14);
	/* 5.10.8.17. LC_BLOCK_EL_IDLE_IN_L0 */
	set_pcie_enable_bits(dev, 0xB1, 1 << 20, 1 << 20);
	/* 5.10.8.18. LC_DONT_GO_TO_L0S_IFL1_ARMED */
	set_pcie_enable_bits(dev, 0xA1, 1 << 11, 1 << 11);
	/* 5.10.8.19. RXP_REALIGN_ON_EACH_TSX_OR_SKP */
	set_pcie_enable_bits(nb_dev, 0x40 | gfx_gpp_sb_sel, 1 << 28, 0 << 28);
	/* 5.10.8.20. Bypass lane de-skew logic if in x1 */
	set_pcie_enable_bits(nb_dev, 0xC2 | gfx_gpp_sb_sel, 1 << 14, 1 << 14);
	/* 5.10.8.21. sets electrical idle threshold. */
	if (gfx_gpp_sb_sel == PCIE_CORE_INDEX_GPPSB)
		set_nbmisc_enable_bits(nb_dev, 0x6A, 3 << 22, 2 << 22);
	else
		set_nbmisc_enable_bits(nb_dev, 0x24, 3 << 16, 2 << 16);

	/* 5.10.8.22. Disable GEN2 */
	/* TODO: should be 2 separated cases. */
	set_nbmisc_enable_bits(nb_dev, 0x39, 1 << 31, 0 << 31);
	set_nbmisc_enable_bits(nb_dev, 0x22, 1 << 5, 0 << 5);
	set_nbmisc_enable_bits(nb_dev, 0x34, 1 << 31, 0 << 31);
	set_nbmisc_enable_bits(nb_dev, 0x37, 7 << 5, 0 << 5);
	/* 5.10.8.23. Disables GEN2 capability of the device. RPR says enable? No! */
	set_pcie_enable_bits(dev, 0xA4, 1 << 0, 0 << 0);
	/* 5.10.8.24. Disable advertising upconfigure support. */
	set_pcie_enable_bits(dev, 0xA2, 1 << 13, 1 << 13);
	/* 5.10.8.25-26. STRAP_BIF_DSN_EN */
	if (gfx_gpp_sb_sel == PCIE_CORE_INDEX_GPPSB)
		set_nbmisc_enable_bits(nb_dev, 0x68, 1 << 19, 0 << 19);
	else
		set_nbmisc_enable_bits(nb_dev, 0x22, 1 << 3, 0 << 3);
	/* 5.10.8.27-28. */
	set_pcie_enable_bits(nb_dev, 0xC1 | gfx_gpp_sb_sel, 1 << 0 | 1 << 2, 1 << 0 | 0 << 2);
	/* 5.10.8.29. Uses the bif_core de-emphasis strength by default. */
	if (gfx_gpp_sb_sel == PCIE_CORE_INDEX_GPPSB) {
		set_nbmisc_enable_bits(nb_dev, 0x67, 1 << 10, 1 << 10);
		set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 29, 1 << 29);
	}
	else {
		set_nbmisc_enable_bits(nb_dev, 0x39, 1 << 30, 1 << 30);
	}
	/* 5.10.8.30. Set TX arbitration algorithm to round robin. */
	set_pcie_enable_bits(nb_dev, 0x1C | gfx_gpp_sb_sel,
			     1 << 0 | 0x1F << 1 | 0x1F << 6,
			     1 << 0 | 0x04 << 1 | 0x04 << 6);

	/* check compliance rpr step 2.1*/
	if (AtiPcieCfg.Config & PCIE_GPP_COMPLIANCE) {
		u32 tmp;
		tmp = nbmisc_read_index(nb_dev, 0x67);
		tmp |= 1 << 3;
		nbmisc_write_index(nb_dev, 0x67, tmp);
	}

	/* step 5: dynamic slave CPL buffer allocation. Disable it, otherwise linux hangs. Why? */
	/* set_pcie_enable_bits(nb_dev, 0x20 | gfx_gpp_sb_sel, 1 << 11, 1 << 11); */

	/* step 5a: Training for GPP devices */
	/* init GPP */
	switch (port) {
	case 4:		/* GPP */
	case 5:
	case 6:
	case 7:
	case 9:
	case 10:
		/* 5.10.8.5. Blocks DMA traffic during C3 state */
		set_pcie_enable_bits(dev, 0x10, 1 << 0, 0 << 0);
		/* Enables TLP flushing */
		set_pcie_enable_bits(dev, 0x20, 1 << 19, 0 << 19);

		/* check port enable */
		if (cfg->port_enable & (1 << port)) {
			PcieReleasePortTraining(nb_dev, dev, port);
			if (!(AtiPcieCfg.Config & PCIE_GPP_COMPLIANCE)) {
				u8 res = PcieTrainPort(nb_dev, dev, port);
				printk(BIOS_DEBUG, "PcieTrainPort port=0x%x result=%d\n", port, res);
				if (res) {
					AtiPcieCfg.PortDetect |= 1 << port;
				}
			}
		}
		break;
	case 8:		/* SB */
		break;
	}
	PciePowerOffGppPorts(nb_dev, dev, port);
}

/*****************************************
* Compliant with CIM_33's PCIEConfigureGPPCore
*****************************************/
void config_gpp_core(struct device *nb_dev, struct device *sb_dev)
{
	u32 reg;
	struct southbridge_amd_rs780_config *cfg =
	    (struct southbridge_amd_rs780_config *)nb_dev->chip_info;

	reg = nbmisc_read_index(nb_dev, 0x20);
	if (AtiPcieCfg.Config & PCIE_ENABLE_STATIC_DEV_REMAP)
		reg &= 0xfffffffd;	/* set bit1 = 0 */
	else
		reg |= 0x2;	/* set bit1 = 1 */
	nbmisc_write_index(nb_dev, 0x20, reg);

	reg = nbmisc_read_index(nb_dev, 0x67);	/* get STRAP_BIF_LINK_CONFIG_GPPSB at bit 4-7 */
	if (cfg->gppsb_configuration != ((reg >> 4) & 0xf))
		switching_gppsb_configurations(nb_dev, sb_dev);
	reg = nbmisc_read_index(nb_dev, 0x2D);	/* get STRAP_BIF_LINK_CONFIG_GPP at bit 7-10 */
	if (cfg->gpp_configuration != ((reg >> 7) & 0xf))
		switching_gpp_configurations(nb_dev, sb_dev);
	ValidatePortEn(nb_dev);
}

/**
 * Hide unused Gpp port
 */
void pcie_hide_unused_ports(struct device *nb_dev)
{
	u16 hide = 0x6FC; /* skip port 0, 1, 8 */

	hide &= ~(AtiPcieCfg.PortDetect | AtiPcieCfg.PortHp);
	printk(BIOS_INFO, "rs780 unused GPP ports bitmap=0x%03x, force disabled\n", hide);
	set_nbmisc_enable_bits(nb_dev, 0x0C, 0xFC, (hide & 0xFC)); /* bridge 2-7 */
	set_nbmisc_enable_bits(nb_dev, 0x0C, 0x30000, ((hide >> 9) & 0x3) << 16); /* bridge 9-a */
}
