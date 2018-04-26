/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
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

#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <delay.h>
#include "sis966.h"


u8 SiS_SiS191_init[6][3]={
	{0x04, 0xFF, 0x07},
	{0x2C, 0xFF, 0x39},
	{0x2D, 0xFF, 0x10},
	{0x2E, 0xFF, 0x91},
	{0x2F, 0xFF, 0x01},
	{0x00, 0x00, 0x00}	//End of table
};


#define StatusReg	0x1
#define SMI_READ	0x0
#define SMI_REQUEST	0x10
#define TRUE	1
#define FALSE	0

u16 MacAddr[3];


static void writeApcByte(int addr, u8 value)
{
	outb(addr, 0x78);
	outb(value, 0x79);
}

static u8 readApcByte(int addr)
{
	u8 value;
	outb(addr, 0x78);
	value = inb(0x79);
	return(value);
}

static void readApcMacAddr(void)
{
	u8 i;

// enable APC in south bridge sis966 D2F0

	outl(0x80001048, 0xcf8);
	outl((inl(0xcfc) & 0xfffffffd), 0xcfc ); // enable IO78/79h for APC Index/Data

	printk(BIOS_DEBUG, "MAC addr in APC = ");
	for (i = 0x9; i <= 0xe; i++)
		printk(BIOS_DEBUG, "%2.2x",readApcByte(i));

	printk(BIOS_DEBUG, "\n");

	/* Set APC Reload */
	writeApcByte(0x7, readApcByte(0x7) & 0xf7);
	writeApcByte(0x7, readApcByte(0x7) | 0x0a);

	/* disable APC in south bridge */
	outl(0x80001048, 0xcf8);
	outl(inl(0xcfc) & 0xffffffbf, 0xcfc);
}

static void set_apc(struct device *dev)
{
	u16 addr;
	u16 i;
	u8 bTmp;

	/* enable APC in south bridge sis966 D2F0 */
	outl(0x80001048, 0xcf8);
	outl((inl(0xcfc) & 0xfffffffd), 0xcfc ); // enable IO78/79h for APC Index/Data

	for (i = 0; i < 3; i++) {
		addr = 0x9 + 2*i;
		writeApcByte(addr, (u8)(MacAddr[i] & 0xFF));
		writeApcByte(addr+1L, (u8)((MacAddr[i] >> 8) & 0xFF));
		// printf("%x - ", readMacAddrByte(0x59 + i));
	}

	/* Set APC Reload */
	writeApcByte(0x7, readApcByte(0x7) & 0xf7);
	writeApcByte(0x7, readApcByte(0x7) | 0x0a);

	/* disable APC in south bridge */
	outl(0x80001048, 0xcf8);
	outl(inl(0xcfc) & 0xffffffbf, 0xcfc);

	// CFG reg0x73 bit = 1, tell driver MAC Address load to APC
	bTmp = pci_read_config8(dev, 0x73);
	bTmp |= 0x1;
	pci_write_config8(dev, 0x73, bTmp);
}

/**
 * Read one word out of the serial EEPROM.
 *
 * @param dev TODO
 * @param base TODO
 * @param Reg EEPROM word to read.
 * @return Contents of EEPROM word (Reg).
 */
#define LoopNum 200
static unsigned long ReadEEprom(struct device *dev, u8 *base, u32 Reg)
{
	u32 data;
	u32 i;
	u32 ulValue;

	ulValue = (0x80 | (0x2 << 8) | (Reg << 10)); //BIT_7

	write32(base + 0x3c, ulValue);

	mdelay(10);

	for (i = 0; i <= LoopNum; i++) {
		ulValue = read32(base + 0x3c);

		if (!(ulValue & 0x0080)) //BIT_7
		break;

		mdelay(100);
	}

	mdelay(50);

	if (i == LoopNum)
		data = 0x10000;
	else {
		ulValue = read32(base + 0x3c);
		data = ((ulValue & 0xffff0000) >> 16);
	}

	return data;
}

static int phy_read(u8 *base, unsigned phy_addr, unsigned phy_reg)
{
	u32 ulValue;
	u32 Read_Cmd;
	u16 usData;

	Read_Cmd = ((phy_reg << 11) |
			(phy_addr << 6) |
			SMI_READ |
			SMI_REQUEST);

	// SmiMgtInterface Reg is the SMI management interface register(offset 44h) of MAC
	write32(base + 0x44, Read_Cmd);

	// Polling SMI_REQ bit to be deasserted indicated read command completed
	do {
		// Wait 20 usec before checking status
		mdelay(20);
		ulValue = read32(base + 0x44);
	} while ((ulValue & SMI_REQUEST) != 0);
	//printk(BIOS_DEBUG, "base %x cmd %lx ret val %lx\n", tmp,Read_Cmd,ulValue);
	usData = (ulValue >> 16);

	return usData;
}

// Detect a valid PHY
// If there exist a valid PHY then return TRUE, else return FALSE
static int phy_detect(u8 *base,u16 *PhyAddr) //BOOL PHY_Detect()
{
	int bFoundPhy = FALSE;
	u16 usData;
	int PhyAddress = 0;


	// Scan all PHY address(0 ~ 31) to find a valid PHY
	for (PhyAddress = 0; PhyAddress < 32; PhyAddress++) {
		// Status register is a PHY's register(offset 01h)
		usData = phy_read(base,PhyAddress,StatusReg);

		// Found a valid PHY
		if ((usData != 0x0) && (usData != 0xffff)) {
			bFoundPhy = TRUE;
			break;
		}
	}

	if (!bFoundPhy)
		printk(BIOS_DEBUG, "PHY not found !!!!\n");

	*PhyAddr = PhyAddress;

	return bFoundPhy;
}


static void nic_init(struct device *dev)
{
	int val;
	u16 PhyAddr;
	u8 *base;
	struct resource *res;

	printk(BIOS_DEBUG, "NIC_INIT:---------->\n");

//-------------- enable NIC (SiS19x) -------------------------
{
	u8 temp8;
	int i = 0;
	while (SiS_SiS191_init[i][0] != 0) {
		temp8 = pci_read_config8(dev, SiS_SiS191_init[i][0]);
		temp8 &= SiS_SiS191_init[i][1];
		temp8 |= SiS_SiS191_init[i][2];
		pci_write_config8(dev, SiS_SiS191_init[i][0], temp8);
		i++;
	};
}
//-----------------------------------------------------------

{
	unsigned long i;
	unsigned long ulValue;

	res = find_resource(dev, 0x10);

	if (!res) {
		printk(BIOS_DEBUG, "NIC Cannot find resource..\n");
		return;
	}
	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "NIC base address %p\n",base);

	if (!(val = phy_detect(base, &PhyAddr))) {
		printk(BIOS_DEBUG, "PHY detect fail !!!!\n");
		return;
	}

	ulValue = read32(base + 0x38L); // check EEPROM existing

	if (ulValue & 0x0002) {

	//	read MAC address from EEPROM at first

	//	if that is valid we will use that

	printk(BIOS_DEBUG, "EEPROM contents %lx\n",ReadEEprom(dev, base, 0LL));
		for (i = 0; i < 3; i++) {
			//status = smbus_read_byte(dev_eeprom, i);
			ulValue = ReadEEprom(dev, base, i + 3L);
			if (ulValue == 0x10000)
				break; // error

			MacAddr[i] = ulValue & 0xFFFF;

		}
	} else {
		// read MAC address from firmware
		printk(BIOS_DEBUG, "EEPROM invalid!!\nReg 0x38h=%.8lx\n",ulValue);
		MacAddr[0] = read16((u16 *)0xffffffc0); // mac address store at here
		MacAddr[1] = read16((u16 *)0xffffffc2);
		MacAddr[2] = read16((u16 *)0xffffffc4);
	}

	set_apc(dev);

	readApcMacAddr();

#if DEBUG_NIC
{
	int i;

	printk(BIOS_DEBUG, "****** NIC PCI config ******");
	printk(BIOS_DEBUG, "\n    03020100  07060504  0B0A0908  0F0E0D0C");

	for (i = 0; i < 0xff; i += 4) {
		if ((i%16) == 0)
			printk(BIOS_DEBUG, "\n%02x: ", i);
		printk(BIOS_DEBUG, "%08x  ", pci_read_config32(dev, i));
	}
	printk(BIOS_DEBUG, "\n");
}

#endif

}

printk(BIOS_DEBUG, "NIC_INIT:<----------\n");
return;

}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem	= lpci_set_subsystem,
};

static struct device_operations nic_ops = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init		= nic_init,
	.scan_bus	= 0,
//	.enable		= sis966_enable,
	.ops_pci	= &lops_pci,
};

static const struct pci_driver nic_driver __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_NIC,
};
