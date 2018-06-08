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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <delay.h>
#include "rs690.h"

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

static void PciePowerOffGppPorts(struct device *nb_dev, struct device *dev,
				 u32 port);
static void ValidatePortEn(struct device *nb_dev);

static void ValidatePortEn(struct device *nb_dev)
{
}


/*****************************************************************
* Compliant with CIM_33's PCIEPowerOffGppPorts
* Power off unused GPP lines
*****************************************************************/
static void PciePowerOffGppPorts(struct device *nb_dev, struct device *dev,
				 u32 port)
{
	u32 reg;
	u16 state_save;
	struct southbridge_amd_rs690_config *cfg =
	    (struct southbridge_amd_rs690_config *)nb_dev->chip_info;
	u8 state = cfg->port_enable;

	if (!(AtiPcieCfg.Config & PCIE_DISABLE_HIDE_UNUSED_PORTS))
		state &= AtiPcieCfg.PortDetect;
	state = ~state;
	state &= (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7);
	state_save = state << 17;
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

	if (!cfg->gfx_tmds){
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
}

#ifdef UNUSED_CODE
static void pcie_init(struct device *dev)
{
	/* Enable pci error detecting */
	u32 dword;

	printk(BIOS_DEBUG, "pcie_init in rs690_pcie.c\n");

	/* System error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1 << 8);	/* System error enable */
	dword |= (1 << 30);	/* Clear possible errors */
	pci_write_config32(dev, 0x04, dword);
}
#endif

/**********************************************************************
**********************************************************************/
static void switching_gpp_configurations(struct device *nb_dev,
					 struct device *sb_dev)
{
	u32 reg;
	struct southbridge_amd_rs690_config *cfg =
	    (struct southbridge_amd_rs690_config *)nb_dev->chip_info;

	/* enables GPP reconfiguration */
	reg = nbmisc_read_index(nb_dev, PCIE_NBCFG_REG7);
	reg |=
	    (RECONFIG_GPPSB_EN + RECONFIG_GPPSB_LINK_CONFIG +
	     RECONFIG_GPPSB_ATOMIC_RESET);
	nbmisc_write_index(nb_dev, PCIE_NBCFG_REG7, reg);

	/* sets desired GPPSB configurations, bit4-7 */
	reg = nbmisc_read_index(nb_dev, 0x67);
	reg &= 0xffffff0f;		/* clean */
	reg |= cfg->gpp_configuration << 4;
	nbmisc_write_index(nb_dev, 0x67, reg);

	/* read bit14 and write back its inverted value */
	reg = nbmisc_read_index(nb_dev, PCIE_NBCFG_REG7);
	reg ^= RECONFIG_GPPSB_GPPSB;
	nbmisc_write_index(nb_dev, PCIE_NBCFG_REG7, reg);

	/* delay 1ms */
	mdelay(1);

	/* waits until SB has trained to L0, poll for bit0-5 = 0x10 */
	do {
		reg = nbpcie_p_read_index(sb_dev, PCIE_LC_STATE0);
		reg &= 0x3f;	/* remain LSB [5:0] bits */
	} while (LC_STATE_RECONFIG_GPPSB != reg);

	/* ensures that virtual channel negotiation is completed. poll for bit1 = 0 */
	do {
		reg =
		    pci_ext_read_config32(nb_dev, sb_dev,
					  PCIE_VC0_RESOURCE_STATUS);
	} while (reg & VC_NEGOTIATION_PENDING);
}

/*****************************************************************
* The rs690 uses NBCONFIG:0x1c (BAR3) to map the PCIE Extended Configuration
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
* We should disable bar3 when we want to exit rs690_enable, because bar3 will be
* remapped in set_resource later.
*****************************************************************/
void disable_pcie_bar3(struct device *nb_dev)
{
	printk(BIOS_DEBUG, "disable_pcie_bar3()\n");
	set_nbcfg_enable_bits(nb_dev, 0x7C, 1 << 30, 0 << 30);	/* Disable writes to the BAR3. */
	pci_write_config32(nb_dev, 0x1C, 0);	/* clear BAR3 address */
	ProgK8TempMmioBase(0, EXT_CONF_BASE_ADDRESS, TEMP_MMIO_BASE_ADDRESS);
}

/*****************************************
* Compliant with CIM_33's PCIEGPPInit
* nb_dev:
*	root bridge struct
* dev:
*	p2p bridge struct
* port:
*	p2p bridge number, 4-8
*****************************************/
void rs690_gpp_sb_init(struct device *nb_dev, struct device *dev, u32 port)
{
	u8 reg8;
	u16 reg16;
	struct device *sb_dev;
	struct southbridge_amd_rs690_config *cfg =
	    (struct southbridge_amd_rs690_config *)nb_dev->chip_info;
	printk(BIOS_DEBUG, "gpp_sb_init nb_dev=0x%p, dev=0x%p, port=0x%x\n", nb_dev, dev, port);

	/* init GPP core */
	set_pcie_enable_bits(nb_dev, 0x20 | PCIE_CORE_INDEX_GPPSB, 1 << 8,
			     1 << 8);
	/* PCIE initialization 5.10.2: rpr 2.12*/
	set_pcie_enable_bits(nb_dev, 0x02 | PCIE_CORE_INDEX_GPPSB, 1 << 0, 1 << 0);	/* no description in datasheet. */

	/* init GPPSB port */
	/* Sets RCB timeout to be 100ms by setting bits[18:16] to 3 b101 and shortens the enumeration timer by setting bit[19] to 1*/
	set_pcie_enable_bits(dev, 0x70, 0xF << 16, 0xd << 16);
	/* PCIE initialization 5.10.2: rpr 2.4 */
	set_pcie_enable_bits(dev, 0x02, ~0xffffffff, 1 << 14);
	/* Do not gate the electrical idle from the PHY and enables the escape from L1L23 */
	set_pcie_enable_bits(dev, 0xA0, ~0xffffffbf, (3 << 30) | (3 << 12) | (3 << 4));
	/* PCIE initialization 5.10.2: rpr 2.13 */
	set_pcie_enable_bits(dev, 0x02, ~0xffffffff, 1 << 6);

	/* SLOT_IMPLEMENTED in pcieConfig space */
	reg8 = pci_read_config8(dev, 0x5b);
	reg8 |= 1 << 0;
	pci_write_config8(dev, 0x5b, reg8);

	reg16 = pci_read_config16(dev, 0x5a);
	reg16 |= 0x100;
	pci_write_config16(dev, 0x5a, reg16);
	nbmisc_write_index(nb_dev, 0x34, 0);

	/* check compliance rpr step 2.1*/
	if (AtiPcieCfg.Config & PCIE_GPP_COMPLIANCE) {
		u32 tmp;
		tmp = nbmisc_read_index(nb_dev, 0x67);
		tmp |= 1 << 3;
		nbmisc_write_index(nb_dev, 0x67, tmp);
	}

	/* step 5: dynamic slave CPL buffer allocation */
	set_pcie_enable_bits(nb_dev, 0x20 | PCIE_CORE_INDEX_GPPSB, 1 << 11, 1 << 11);

	/* step 5a: Training for GPP devices */
	/* init GPP */
	switch (port) {
	case 4:		/* GPP */
	case 5:
	case 6:
	case 7:
		/* Blocks DMA traffic during C3 state */
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

	/* step 5b: GFX devices in a GPP slot */

	/* step 6a: VCI */
	sb_dev = dev_find_slot(0, PCI_DEVFN(8, 0));
	if (port == 8) {
		/* The code below between #if and #endif causes a hang on HDA init.
		 * So we skip it. */
#if 0
		/* Clear bits 7:1 */
		pci_ext_write_config32(nb_dev, sb_dev, 0x114, 0x3f << 1, 0 << 1);
		/* Maps Traffic Class 1-7 to VC1 */
		pci_ext_write_config32(nb_dev, sb_dev, 0x120, 0x7f << 1, 0x7f << 1);
		/* Assigns VC ID to 1 */
		pci_ext_write_config32(nb_dev, sb_dev, 0x120, 7 << 24, 1 << 24);
		/* Enables VC1 */
		pci_ext_write_config32(nb_dev, sb_dev, 0x120, 1 << 31, 1 << 31);

		do {
			reg16 = pci_ext_read_config32(nb_dev, sb_dev, 0x124);
			reg16 &= 0x2;
		} while (reg16); /*bit[1] = 0 means VC1 flow control initialization is successful */
#endif
	}

	/* step 6b: L0s for the southbridge link */
	/* To enable L0s in the southbridge*/

	/* step 6c: L0s for the GPP link(s) */
	/* To enable L0s in the RS690 for the GPP port(s) */
	set_pcie_enable_bits(nb_dev, 0xf9, 3 << 13, 2 << 13);
	set_pcie_enable_bits(dev, 0xa0, 0xf << 8, 0x9 << 8);
	reg16 = pci_read_config16(dev, 0x68);
	reg16 |= 1 << 0;
	pci_write_config16(dev, 0x68, reg16);

	/* step 6d: ASPM L1 for the southbridge link */
	/* To enable L1s in the southbridge*/

	/* step 6e: ASPM L1 for GPP link(s) */
	set_pcie_enable_bits(nb_dev, 0xf9, 3 << 13, 2 << 13);
	set_pcie_enable_bits(dev, 0xa0, 3 << 12, 3 << 12);
	set_pcie_enable_bits(dev, 0xa0, 0xf << 4, 3 << 4);
	reg16 = pci_read_config16(dev, 0x68);
	reg16 &= ~0xff;
	reg16 |= 1 << 1;
	pci_write_config16(dev, 0x68, reg16);

	/* step 6f: Turning off PLL during L1/L23 */
	set_pcie_enable_bits(nb_dev, 0x40, 1 << 3, 1 << 3);
	set_pcie_enable_bits(nb_dev, 0x40, 1 << 9, 1 << 9);

	/* step 6g: TXCLK clock gating */
	set_nbmisc_enable_bits(nb_dev, 0x7, 3 << 4, 3 << 4);
	set_nbmisc_enable_bits(nb_dev, 0x7, 1 << 22, 1 << 22);
	set_pcie_enable_bits(nb_dev, 0x11, 0xf << 4, 0xc << 4);

	/* step 6h: LCLK clock gating, done in rs690_config_misc_clk() */
}

/*****************************************
* Compliant with CIM_33's PCIEConfigureGPPCore
*****************************************/
void config_gpp_core(struct device *nb_dev, struct device *sb_dev)
{
	u32 reg;
	struct southbridge_amd_rs690_config *cfg =
	    (struct southbridge_amd_rs690_config *)nb_dev->chip_info;

	reg = nbmisc_read_index(nb_dev, 0x20);
	if (AtiPcieCfg.Config & PCIE_ENABLE_STATIC_DEV_REMAP)
		reg &= 0xfffffffd;	/* set bit1 = 0 */
	else
		reg |= 0x2;	/* set bit1 = 1 */
	nbmisc_write_index(nb_dev, 0x20, reg);

	reg = nbmisc_read_index(nb_dev, 0x67);	/* get STRAP_BIF_LINK_CONFIG_GPPSB at bit 4-7 */
	if (cfg->gpp_configuration != ((reg >> 4) & 0xf))
		switching_gpp_configurations(nb_dev, sb_dev);
	ValidatePortEn(nb_dev);
}

#ifdef UNUSED_CODE
/*****************************************
* Compliant with CIM_33's PCIEMiscClkProg
*****************************************/
void pcie_config_misc_clk(struct device *nb_dev)
{
	u32 reg;
	struct bus pbus; /* fake bus for dev0 fun1 */

	reg = pci_read_config32(nb_dev, 0x4c);
	reg |= 1 << 0;
	pci_write_config32(nb_dev, 0x4c, reg);

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
		set_pcie_enable_bits(nb_dev, 0x11 | PCIE_CORE_INDEX_GPPSB, (3 << 6) | (~0xf), 3 << 6);

		/* LCLK Clock Gating */
		reg =  pci_cf8_conf1.read32(&pbus, 0, 1, 0x94);
		reg &= ~(1 << 24);
		pci_cf8_conf1.write32(&pbus, 0, 1, 0x94, reg);
	}

	reg = pci_read_config32(nb_dev, 0x4c);
	reg &= ~(1 << 0);
	pci_write_config32(nb_dev, 0x4c, reg);
}
#endif
