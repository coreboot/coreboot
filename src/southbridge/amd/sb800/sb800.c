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

#include <arch/io.h>

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "sb800.h"
#include "smbus.h"

static struct device *find_sm_dev(struct device *dev, u32 devfn)
{
	struct device *sm_dev;

	sm_dev = dev_find_slot(dev->bus->secondary, devfn);
	if (!sm_dev)
		return sm_dev;

	if ((sm_dev->vendor != PCI_VENDOR_ID_ATI) ||
	    ((sm_dev->device != PCI_DEVICE_ID_ATI_SB800_SM))) {
		u32 id;
		id = pci_read_config32(sm_dev, PCI_VENDOR_ID);
		if ((id !=
		     (PCI_VENDOR_ID_ATI | (PCI_DEVICE_ID_ATI_SB800_SM << 16))))
		{
			sm_dev = 0;
		}
	}

	return sm_dev;
}

void set_sm_enable_bits(struct device *sm_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = pci_read_config32(sm_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config32(sm_dev, reg_pos, reg);
	}
}

static void pmio_write_index(u16 port_base, u8 reg, u8 value)
{
	outb(reg, port_base);
	outb(value, port_base + 1);
}

static u8 pmio_read_index(u16 port_base, u8 reg)
{
	outb(reg, port_base);
	return inb(port_base + 1);
}

void pm_iowrite(u8 reg, u8 value)
{
	pmio_write_index(PM_INDEX, reg, value);
}

u8 pm_ioread(u8 reg)
{
	return pmio_read_index(PM_INDEX, reg);
}

void pm2_iowrite(u8 reg, u8 value)
{
	pmio_write_index(PM2_INDEX, reg, value);
}

u8 pm2_ioread(u8 reg)
{
	return pmio_read_index(PM2_INDEX, reg);
}

static void set_pmio_enable_bits(u32 reg_pos, u32 mask, u32 val)
{
	u8 reg_old, reg;
	reg = reg_old = pm_ioread(reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pm_iowrite(reg_pos, reg);
	}
}

u16 tx_convert_table[] = {
	[0x0] = 0xFFFF,
	[0x1] = 0xFFFE,
	[0x2] = 0xFFFC,
	[0x3] = 0xFFF8,
	[0x4] = 0xFFF0,
	[0x5] = 0xFFE0,
	[0x6] = 0xFFC0,
	[0x7] = 0xFF80,
	[0x8] = 0xFF00,
	[0x9] = 0xFE00,
	[0xA] = 0xFC00,
	[0xB] = 0xF800,
	[0xC] = 0xF000,
	[0xD] = 0xE000,
	[0xE] = 0xC000,
	[0xF] = 0x8000
};

u16 rx_convert_table[] = {
	[0x0] = 0x0001,
	[0x1] = 0x0003,
	[0x2] = 0x0007,
	[0x3] = 0x000F,
	[0x4] = 0x001F,
	[0x5] = 0x003F,
	[0x6] = 0x007F,
	[0x7] = 0x00FF,
	[0x8] = 0x01FF,
	[0x9] = 0x03FF,
	[0xA] = 0x07FF,
	[0xB] = 0x0FFF,
	[0xC] = 0x1FFF,
	[0xD] = 0x3FFF,
	[0xE] = 0x7FFF,
	[0xF] = 0xFFFF
};

/* PCIe General Purpose Ports */
/* v:1814, d:3090. cp421A */
static void set_sb800_gpp(struct device *dev)
{
	struct southbridge_amd_sb800_config *conf;
	u32 imp_rb, lc_status;
	u8 port;

	conf = dev->chip_info;
	port = dev->path.pci.devfn & 3;

	/* 5.1 GPP Lane Configuration */
	/* To support one of 4 legal configurations:
	 * 0000: PortA lanes[3:0]
	 * 0001: N/A
	 * 0010: PortA lanes[1:0], PortB lanes[3:2]
	 * 0011: PortA lanes[1:0], PortB lane2, PortC lane3
	 * 0100: PortA lane0, PortB lane1, PortC lane2, PortD lane3.
	 * Other combinations are not supported.
	 */
	/* CIMx: Set abcfg:0x90[20] = 1 to enable GPP bridge multi-function */
	abcfg_reg(0x90, 1 << 20, 1 << 20);

	printk(BIOS_DEBUG, "set_sb800_gpp() 1\n");
	//abcfg_reg(0xC0, 0xF << 0, 0x4 << 0); /* bimini:4; tarpon:3 */
	abcfg_reg(0xC0, 0xF << 0, (conf->gpp_configuration & 0xF) << 0); /* bimini:4; tarpon:3 */

	printk(BIOS_DEBUG, "set_sb800_gpp() 2,\n");
	/* 5.2 Enabling GPP Port A/B/C/D */
	//abcfg_reg(0xC0, 0xF << 4, 0x1 << 4);
	abcfg_reg(0xC0, 0xF << 4, dev->enabled ? 0x1 << (4 + port) : 0);

	printk(BIOS_DEBUG, "set_sb800_gpp() 3\n");
	/* 5.3 Releasing GPP Reset */
	abcfg_reg(0xC0, 0x1 << 8, 0x0 << 8);

	/* release training */
	abcfg_reg(0xC0, 0xF << 12, 0x0 << 12);
	/* 5.4 Power Saving Feature for GPP Lanes. Skip */
	/* 5.5 PCIe Ports De-emphasis Settings. Skip */
	abcfg_reg(0x340, 1 << 21, 0 << 21);
	abcfg_reg(0x344, 1 << 21, 0 << 21);
	abcfg_reg(0x348, 1 << 21, 0 << 21);
	abcfg_reg(0x34C, 1 << 21, 0 << 21);
	/* 5.6 PCIe PHY Calibration Impedance Value Setting */
	/* AXINDC_Reg 0x60: TX_IMP_RB */
	outl(0x30, 0xCD8);
	outl(0x60, 0xCDC);
	outl(0x34, 0xCD8);
	imp_rb = inl(0xCDC);

	printk(BIOS_DEBUG, "imp_rb 1=%x\n", imp_rb);
	/* tx */
	abcfg_reg(0xA4, 0x1FFF, (rx_convert_table[(imp_rb>>8)&0xF]) & 0x1FFF);
	abcfg_reg(0xA4, 0x1FFF<<19, ((tx_convert_table[(imp_rb>>4)&0xF] >> 3) & 0x1FFF) << 19);

	/* 5.4. */
	abcfg_reg(0xA0, 3 << 12, 3 << 12);

	axindxp_reg(0xa0, 0xf<<4, 3<<4);
	rcindxp_reg(0xA0, 0, 0xF << 4, 1 << 0);
	rcindxp_reg(0xA0, 1, 0xF << 4, 1 << 0);
	rcindxp_reg(0xA0, 2, 0xF << 4, 1 << 0);
	rcindxp_reg(0xA0, 3, 0xF << 4, 1 << 0);

	/* 5.8 Disabling Serial Number Capability */
	abcfg_reg(0x330, 1 << 26, 0 << 26);

	abcfg_reg(0x50, 0xFFFFFFFF, 0x04000004);
	abcfg_reg(0x54, 0xFFFFFFFF, 0x00040000);
	abcfg_reg(0x80, 0xFFFFFFFF, 0x03060001);
	abcfg_reg(0x90, 0xFFFFFFFF, 0x00300000);
	abcfg_reg(0x98, 0xFFFFFFFF, 0x03034700);

	pci_write_config32(dev, 0x80, 0x00000006);
	/* Set PCIEIND_P:PCIE_RX_CNTL[RX_RCB_CPL_TIMEOUT_MODE] (0x70:[19]) = 1 */
	rcindxp_reg(0x70, 0, 1 << 19, 1 << 19);
	//outl(3<<29|0<<24|0x70, 0xCD8);
	/* Set PCIEIND_P:PCIE_TX_CNTL[TX_FLUSH_TLP_DIS] (0x20:[19]) = 0 */
	rcindxp_reg(0x20, 0, 1 << 19, 0 << 19);
	printk(BIOS_DEBUG, "imp_rb 5=%x\n", imp_rb);
	outl(3<<29|0<<24|0xA5, 0xCD8);
	lc_status = inl(0xCDC);
	printk(BIOS_DEBUG, "lc_status=%x\n", lc_status);
}

void sb800_enable(struct device *dev)
{
	struct device *sm_dev = NULL;
	struct device *bus_dev = NULL;
	int index = -1;
	u32 deviceid;
	u32 vendorid;

	/* struct southbridge_ati_sb800_config *conf; */
	/* conf = dev->chip_info; */
	int i;

	u32 devfn, dword;

	printk(BIOS_DEBUG, "sb800_enable()\n");

	/*
	 *	0:11.0  SATA		bit 8 of pmio 0xDA : 1 - enable
	 *	0:12.0  OHCI-USB1	bit 0 of pmio 0xEF
	 *	0:12.2  EHCI-USB1	bit 1 of pmio 0xEF
	 *	0:13.0  OHCI-USB2	bit 2 of pmio 0xEF
	 *	0:13.2  EHCI-USB2	bit 3 of pmio 0xEF
	 *	0:16.1  OHCI-USB3	bit 4 of pmio 0xEF
	 *	0:16.2  EHCI-USB3	bit 5 of pmio 0xEF
	 *	0:14.5  OHCI-USB4	bit 6 of pmio 0xEF
	 *	0:14.0  SMBUS							0
	 *	0:14.1  IDE							1
	 *	0:14.2  HDA	bit 0 of pm_io 0xEB : 1 - enable
	 *	0:14.3  LPC	bit 0 of pm_io 0xEC : 1 - enable
	 *	0:14.4  PCI	bit 0 of pm_io 0xEA : 0 - enable
	 *	0:14.6  GEC	bit 0 of pm_io 0xF6 : 0 - enable
	 */
	if (dev->device == 0x0000) {
		vendorid = pci_read_config32(dev, PCI_VENDOR_ID);
		deviceid = (vendorid >> 16) & 0xffff;
		vendorid &= 0xffff;
	} else {
		vendorid = dev->vendor;
		deviceid = dev->device;
	}
	bus_dev = dev->bus->dev;
	if ((bus_dev->vendor == PCI_VENDOR_ID_ATI) &&
	    (bus_dev->device == PCI_DEVICE_ID_ATI_SB800_PCI)) {
		devfn = (bus_dev->path.pci.devfn) & ~7;
		sm_dev = find_sm_dev(bus_dev, devfn);
		if (!sm_dev)
			return;

		/* something under 00:01.0 */
		switch (dev->path.pci.devfn) {
		case 5 << 3:
			;
		}

		return;
	}
	printk(BIOS_DEBUG, "sb800_enable() 1\n");

	i = (dev->path.pci.devfn) & ~7;
	i += (3 << 3);
	for (devfn = (0x14 << 3); devfn <= i; devfn += (1 << 3)) {
		sm_dev = find_sm_dev(dev, devfn);
		if (sm_dev)
			break;
	}
	if (!sm_dev)
		return;
	printk(BIOS_DEBUG, "sb800_enable() 2\n");

	switch (dev->path.pci.devfn - (devfn - (0x14 << 3))) {
	case (0x11 << 3) | 0:
		index = 8;
		set_pmio_enable_bits(0xDA, 1 << 0,
				     (dev->enabled ? 1 : 0) << 0);
		/* Set the device ID of SATA as 0x4390 to reduce the confusing. */
		dword = pci_read_config32(dev, 0x40);
		dword |= 1 << 0;
		pci_write_config32(dev, 0x40, dword);
		pci_write_config16(dev, 0x2, 0x4390);
		dword &= ~1;
		pci_write_config32(dev, 0x40, dword);//for (;;);
		index += 32 * 3;
		break;
	case (0x12 << 3) | 0:
	case (0x12 << 3) | 2:
		index = (dev->path.pci.devfn & 0x3) / 2;
		set_pmio_enable_bits(0xEF, 1 << index,
				     (dev->enabled ? 1 : 0) << index);
		break;
	case (0x13 << 3) | 0:
	case (0x13 << 3) | 2:
		index = (dev->path.pci.devfn & 0x3) / 2 + 2;
		set_pmio_enable_bits(0xEF, 1 << index,
				     (dev->enabled ? 1 : 0) << index);
		index += 32 * 2;
		break;
	case (0x14 << 3) | 0:
		index = 0;
		break;
	case (0x14 << 3) | 1:
		index = 1;
		set_pmio_enable_bits(0xDA, 1 << 3,
				     (dev->enabled ? 0 : 1) << 3);
		break;
	case (0x14 << 3) | 2:
		index = 0;
		set_pmio_enable_bits(0xEB, 1 << index,
				     (dev->enabled ? 1 : 0) << index);
		break;
	case (0x14 << 3) | 3:
		index = 0;
		set_pmio_enable_bits(0xEC, 1 << index,
				     (dev->enabled ? 1 : 0) << index);
		index += 32 * 1;
		break;
	case (0x14 << 3) | 4:
		index = 0;
		set_pmio_enable_bits(0xEA, 1 << index,
				     (dev->enabled ? 0 : 1) << index);
		break;
	case (0x14 << 3) | 5:
		index = 6;
		set_pmio_enable_bits(0xEF, 1 << index,
				     (dev->enabled ? 1 : 0) << index);
		break;
	case (0x14 << 3) | 6:
		index = 0;
		set_pmio_enable_bits(0xF6, 1 << index,
				     (dev->enabled ? 0 : 1) << index);
		break;
	case (0x15 << 3) | 0:
		set_sb800_gpp(dev);
		index = 4;
		break;
	case (0x15 << 3) | 1:
	case (0x15 << 3) | 2:
	case (0x15 << 3) | 3:
		break;
	case (0x16 << 3) | 0:
	case (0x16 << 3) | 2:
		index = (dev->path.pci.devfn & 0x3) / 2 + 4;
		set_pmio_enable_bits(0xEF, 1 << index,
				     (dev->enabled ? 1 : 0) << index);
		break;
	default:
		printk(BIOS_DEBUG, "unknown dev: %s deviceid=%4x\n", dev_path(dev),
			     deviceid);
	}
}

struct chip_operations southbridge_amd_sb800_ops = {
	CHIP_NAME("ATI SB800")
	.enable_dev = sb800_enable,
};
