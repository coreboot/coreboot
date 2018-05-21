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

/*
 *  for rs780 internal graphics device
 *  device id of internal graphics:
 *	RS780:	0x9610
 *	RS780C:	0x9611
 *	RS780M:	0x9612
 *	RS780MC:0x9613
 *	RS780E: 0x9615
 *	RS785G: 0x9710 - just works, not much tested
 *	RS785C: 0x9711
 *	RS785M: 0x9712
 *	RS785MC:0x9713
 *	RS785D: 0x9714
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <delay.h>
#include <cpu/x86/msr.h>
#include "rs780.h"
extern int is_dev3_present(void);
void set_pcie_reset(void);
void set_pcie_dereset(void);

/* Trust the original resource allocation. Don't do it again. */
#undef DONT_TRUST_RESOURCE_ALLOCATION
//#define DONT_TRUST_RESOURCE_ALLOCATION

#define CLK_CNTL_INDEX	0x8
#define CLK_CNTL_DATA	0xC

/* The Integrated Info Table. */
ATOM_INTEGRATED_SYSTEM_INFO_V2 vgainfo;

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
	/* printk(BIOS_DEBUG, "gfx bar 2 %02x\n", gfx_bar2); */

	*(u32*)(gfx_bar2+CLK_CNTL_INDEX) = index | 1<<7;
	*(u32*)(gfx_bar2+CLK_CNTL_DATA)  = data;
}

/*
* pci_dev_read_resources thinks it is a IO type.
* We have to force it to mem type.
*/
static void rs780_gfx_read_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "rs780_gfx_read_resources.\n");

	/* The initial value of 0x24 is 0xFFFFFFFF, which is confusing.
	   Even if we write 0xFFFFFFFF into it, it will be 0xFFF00000,
	   which tells us it is a memory address base.
	 */
	pci_write_config32(dev, 0x24, 0x00000000);

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);
	compact_resources(dev);
}

typedef struct _MMIORANGE
{
	u32	Base;
	u32	Limit;
	u8	Attribute;
} MMIORANGE;

MMIORANGE MMIO[8], CreativeMMIO[8];

#define CIM_STATUS u32
#define CIM_SUCCESS 0x00000000
#define CIM_ERROR	0x80000000
#define CIM_UNSUPPORTED	0x80000001
#define CIM_DISABLEPORT 0x80000002

#define	MMIO_ATTRIB_NP_ONLY	1
#define MMIO_ATTRIB_BOTTOM_TO_TOP (1 << 1)
#define MMIO_ATTRIB_SKIP_ZERO (1 << 2)

#ifdef DONT_TRUST_RESOURCE_ALLOCATION
static MMIORANGE* AllocMMIO(MMIORANGE* pMMIO)
{
	int i;
	for (i = 0; i < 8; i++) {
		if (pMMIO[i].Limit == 0)
				return &pMMIO[i];
	}
	return 0;
}

static void FreeMMIO(MMIORANGE* pMMIO)
{
	pMMIO->Base = 0;
	pMMIO->Limit = 0;
}

static u32 SetMMIO(u32 Base, u32 Limit, u8 Attribute, MMIORANGE *pMMIO)
{
	int i;
	MMIORANGE * TempRange;
	for (i = 0; i < 8; i++) {
		if (pMMIO[i].Attribute != Attribute && Base >= pMMIO[i].Base && Limit <= pMMIO[i].Limit) {
			TempRange = AllocMMIO(pMMIO);
			if (TempRange == 0) return 0x80000000;
			TempRange->Base = Limit;
			TempRange->Limit = pMMIO[i].Limit;
			TempRange->Attribute = pMMIO[i].Attribute;
			pMMIO[i].Limit = Base;
		}
	}
	TempRange = AllocMMIO(pMMIO);
	if (TempRange == 0) return 0x80000000;
	TempRange->Base = Base;
	TempRange->Limit = Limit;
	TempRange->Attribute = Attribute;
	return 0;
}

static u8 FinalizeMMIO(MMIORANGE *pMMIO)
{
	int i, j, n = 0;
	for (i = 0; i < 8; i++) {
		if (pMMIO[i].Base == pMMIO[i].Limit) {
			FreeMMIO(&pMMIO[i]);
			continue;
		}
		for (j = 0; j < i; j++) {
			if (i!=j && pMMIO[i].Attribute == pMMIO[j].Attribute) {
				if (pMMIO[i].Base == pMMIO[j].Limit) {
					pMMIO[j].Limit = pMMIO[i].Limit;
					 FreeMMIO(&pMMIO[i]);
				}
				if (pMMIO[i].Limit == pMMIO[j].Base) {
					pMMIO[j].Base = pMMIO[i].Base;
				   FreeMMIO(&pMMIO[i]);
				}
			}
		}
	}
	for (i = 0; i < 8; i++) {
		if (pMMIO[i].Limit != 0) n++;
	}
	return n;
}

static CIM_STATUS GetCreativeMMIO(MMIORANGE *pMMIO)
{
	CIM_STATUS Status = CIM_UNSUPPORTED;
	u8 Bus, Dev, Reg, BusStart, BusEnd;
	u32	Value;
	struct device *dev0x14 = dev_find_slot(0, PCI_DEVFN(0x14, 4));
	struct device *tempdev;
	Value = pci_read_config32(dev0x14, 0x18);
	BusStart = (Value >> 8) & 0xFF;
	BusEnd = (Value >> 16) & 0xFF;
	for (Bus = BusStart; Bus <= BusEnd; Bus++) {
		for (Dev = 0; Dev <= 0x1f; Dev++) {
			tempdev = dev_find_slot(Bus, Dev << 3);
			Value = pci_read_config32(tempdev, 0);
			printk(BIOS_DEBUG, "Dev ID %x\n", Value);
			if ((Value & 0xffff) == 0x1102) {//Creative
				//Found Creative SB
			 	u32	MMIOStart = 0xffffffff;
				u32 MMIOLimit = 0;
				for (Reg = 0x10; Reg < 0x20; Reg+=4) {
					u32	BaseA, LimitA;
					BaseA = pci_read_config32(tempdev, Reg);
					Value = BaseA;
					if (!(Value & 0x01)) {
						Value = Value & 0xffffff00;
						if (Value !=  0) {
							if (MMIOStart > Value)
								MMIOStart = Value;
							LimitA = 0xffffffff;
							//WritePCI(PciAddress,AccWidthUint32,&LimitA);
							pci_write_config32(tempdev, Reg, LimitA);
							//ReadPCI(PciAddress,AccWidthUint32,&LimitA);
							LimitA = pci_read_config32(tempdev, Reg);
							LimitA = Value + (~LimitA + 1);
							//WritePCI(PciAddress,AccWidthUint32,&BaseA);
							pci_write_config32(tempdev, Reg, BaseA);
							if (LimitA > MMIOLimit)
								MMIOLimit = LimitA;
						}
					}
				}
				printk(BIOS_DEBUG, " MMIOStart %x MMIOLimit %x\n", MMIOStart, MMIOLimit);
				if (MMIOStart < MMIOLimit)
				{
					Status = SetMMIO(MMIOStart>>8, MMIOLimit>>8, 0x80, pMMIO);
					if (Status == CIM_ERROR) return Status;
				}
			}
		}
	}
	if (Status == CIM_SUCCESS) {
		//Lets optimize MMIO
		if (FinalizeMMIO(pMMIO) > 4) {
			Status = CIM_ERROR;
		}
	}

	return Status;
}

static void ProgramMMIO(MMIORANGE *pMMIO, u8 LinkID, u8 Attribute)
{
	int i, j, n = 7;
	struct device *k8_f1;

	k8_f1 = dev_find_slot(0, PCI_DEVFN(0x18, 1));

	for (i = 0; i < 8; i++) {
		int k = 0, MmioReg;
		u32 Base = 0;
		u32 Limit = 0;
		for (j = 0; j < 8; j++) {
			if (Base < pMMIO[j].Base) {
				Base = pMMIO[j].Base;
				k = j;
			}
		}
		if (pMMIO[k].Limit != 0) {
			if (Attribute & MMIO_ATTRIB_NP_ONLY && pMMIO[k].Attribute == 0 ) {
				Base = 0;
			}
			else
			{
				Base = pMMIO[k].Base | 0x3;
				Limit= ((pMMIO[k].Limit - 1) & 0xffffff00) | pMMIO[k].Attribute | (LinkID << 4);
			}
			FreeMMIO(&pMMIO[k]);
		}
		if (Attribute & MMIO_ATTRIB_SKIP_ZERO && Base == 0 && Limit == 0) continue;
		MmioReg = (Attribute & MMIO_ATTRIB_BOTTOM_TO_TOP)?n:(7-n);
		n--;
		//RWPCI(PCI_ADDRESS(0,CPU_DEV,CPU_F1,0x80+MmioReg*8),AccWidthUint32 |S3_SAVE,0x0,0x0);
		pci_write_config32(k8_f1, 0x80+MmioReg*8, 0);

		//WritePCI(PCI_ADDRESS(0,CPU_DEV,CPU_F1,0x84+MmioReg*8),AccWidthUint32 |S3_SAVE,&Limit);
		pci_write_config32(k8_f1, 0x84+MmioReg*8, Limit);

		//WritePCI(PCI_ADDRESS(0,CPU_DEV,CPU_F1,0x80+MmioReg*8),AccWidthUint32 |S3_SAVE,&Base);
		pci_write_config32(k8_f1, 0x80+MmioReg*8, Base);
	}
}
#endif

#define GFX_CONFIG_DDI1		0x04
#define GFX_CONFIG_DDI2		0x08
#define GFX_CONFIG_DDI		(GFX_CONFIG_DDI1 | GFX_CONFIG_DDI2)

/**
 * Force poweron pads for lanes used for DDI
 * reference CIMx PCIEL_PowerOnDDILanes()
 *
 * Inactive    B_PRX_PDNB_FDIS  B_PTX_PDNB_FDIS
 *  Lanes
 * Lanes  0-1   Bit  8           Bit 0
 * Lanes  2-3   Bit  9           Bit 1
 * Lanes  4-5   Bit 10           Bit 2
 * Lanes  6-7   Bit 11           Bit 3
 * Lanes  8-9   Bit 12           Bit 4
 * Lanes 10-11  Bit 13           Bit 5
 * Lanes 12-13  Bit 14           Bit 6
 * Lanes 14-15  Bit 15           Bit 7
 */
static void poweron_ddi_lanes(struct device *nb_dev)
{
	u8 i;
	u32 gfx_cfg = 0;
	u32 ddi_pads = 0;

	ddi_pads = ~(nbpcie_ind_read_index(nb_dev, 0x65)); /* save original setting */
	gfx_cfg = nbmisc_read_index(nb_dev, 0x74);
	for (i = 0; i < 3; i++) {
		if (gfx_cfg & GFX_CONFIG_DDI) {
			ddi_pads |= (3 << (i * 2));
		}
		gfx_cfg >>= 8;
	}
	ddi_pads |= ddi_pads << 8; /* both TX and RX */
	nbpcie_ind_write_index(nb_dev, 0x65, ~ddi_pads);
}

static void internal_gfx_pci_dev_init(struct device *dev)
{
	unsigned char * bpointer;
	volatile u32 * GpuF0MMReg;
	volatile u32 * pointer;
	int i;
	u16 command;
	u32 value;
	u16 deviceid, vendorid;
	struct device *nb_dev = dev_find_slot(0, 0);
	struct device *k8_f2 = dev_find_slot(0, PCI_DEVFN(0x18, 2));
	struct device *k8_f0 = dev_find_slot(0, PCI_DEVFN(0x18, 0));
	static const u8 ht_freq_lookup [] = {2, 0, 4, 0, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 0, 0, 28, 30, 32};
	static const u8 ht_width_lookup [] = {8, 16, 0, 0, 2, 4, 0, 0};
	static const u16 memclk_lookup_fam0F [] = {100, 0, 133, 0, 0, 166, 0, 200};
	static const u16 memclk_lookup_fam10 [] = {200, 266, 333, 400, 533, 667, 800, 800};

	/* We definitely will use this in future. Just leave it here. */
	/*struct southbridge_amd_rs780_config *cfg =
	   (struct southbridge_amd_rs780_config *)dev->chip_info;*/

	deviceid = pci_read_config16(dev, PCI_DEVICE_ID);
	vendorid = pci_read_config16(dev, PCI_VENDOR_ID);
	printk(BIOS_DEBUG, "internal_gfx_pci_dev_init device=%x, vendor=%x.\n",
	     deviceid, vendorid);

	command = pci_read_config16(dev, 0x04);
	command |= 0x7;
	pci_write_config16(dev, 0x04, command);

	/* Clear vgainfo. */
	bpointer = (unsigned char *) &vgainfo;
	for (i = 0; i < sizeof(ATOM_INTEGRATED_SYSTEM_INFO_V2); i++) {
		*bpointer = 0;
		bpointer++;
	}

	GpuF0MMReg = (u32 *)pci_read_config32(dev, 0x18);

	/* GFX_InitFBAccess. */
	value = nbmc_read_index(nb_dev, 0x10);
	*(GpuF0MMReg + 0x2000/4) = 0x11;
	*(GpuF0MMReg + 0x2180/4) = ((value&0xff00)>>8)|((value&0xff000000)>>8);
	*(GpuF0MMReg + 0x2c04/4) = ((value&0xff00)<<8);
	*(GpuF0MMReg + 0x5428/4) = ((value&0xffff0000)+0x10000)-((value&0xffff)<<16);
	*(GpuF0MMReg + 0xF774/4) = 0xffffffff;
	*(GpuF0MMReg + 0xF770/4) = 0x00000001;
	*(GpuF0MMReg + 0x2000/4) = 0x00000011;
	*(GpuF0MMReg + 0x200c/4) = 0x00000020;
	*(GpuF0MMReg + 0x2010/4) = 0x10204810;
	*(GpuF0MMReg + 0x2010/4) = 0x00204810;
	*(GpuF0MMReg + 0x2014/4) = 0x10408810;
	*(GpuF0MMReg + 0x2014/4) = 0x00408810;
	*(GpuF0MMReg + 0x2414/4) = 0x00000080;
	*(GpuF0MMReg + 0x2418/4) = 0x84422415;
	*(GpuF0MMReg + 0x2418/4) = 0x04422415;
	*(GpuF0MMReg + 0x5490/4) = 0x00000001;
	*(GpuF0MMReg + 0x7de4/4) |= (1<<3) | (1<<4);
	/* Force allow LDT_STOP Cool'n'Quiet workaround. */
	*(GpuF0MMReg + 0x655c/4) |= 1<<4;

	// disable write combining, needed for stability
	// reference bios does this only for RS780 rev A11
	// need to figure out why we need it for all revs
	*(GpuF0MMReg + 0x2000/4) = 0x00000010;
	*(GpuF0MMReg + 0x2408/4) = 1 << 9;
	*(GpuF0MMReg + 0x2000/4) = 0x00000011;

	/* GFX_InitFBAccess finished. */

#if IS_ENABLED(CONFIG_GFXUMA) /* for UMA mode. */
	/* GFX_StartMC. */
	set_nbmc_enable_bits(nb_dev, 0x02, 0x00000000, 0x80000000);
	set_nbmc_enable_bits(nb_dev, 0x01, 0x00000000, 0x00000001);
	set_nbmc_enable_bits(nb_dev, 0x01, 0x00000000, 0x00000004);
	set_nbmc_enable_bits(nb_dev, 0x01, 0x00040000, 0x00000000);
	set_nbmc_enable_bits(nb_dev, 0xB1, 0xFFFF0000, 0x00000040);
	set_nbmc_enable_bits(nb_dev, 0xC3, 0x00000000, 0x00000001);
	set_nbmc_enable_bits(nb_dev, 0x07, 0xFFFFFFFF, 0x00000018);
	set_nbmc_enable_bits(nb_dev, 0x06, 0xFFFFFFFF, 0x00000102);
	set_nbmc_enable_bits(nb_dev, 0x09, 0xFFFFFFFF, 0x40000008);
	set_nbmc_enable_bits(nb_dev, 0x06, 0x00000000, 0x80000000);
	/* GFX_StartMC finished. */
#else
	/* for SP mode. */
	set_nbmc_enable_bits(nb_dev, 0xaa, 0xf0, 0x30);
	set_nbmc_enable_bits(nb_dev, 0xce, 0xf0, 0x30);
	set_nbmc_enable_bits(nb_dev, 0xca, 0xff000000, 0x47000000);
	set_nbmc_enable_bits(nb_dev, 0xcb, 0x3f000000, 0x01000000);
	set_nbmc_enable_bits(nb_dev, 0x01, 0, 1<<0);
	set_nbmc_enable_bits(nb_dev, 0x04, 0, 1<<31);
	set_nbmc_enable_bits(nb_dev, 0xb4, 0x3f, 0x3f);
	set_nbmc_enable_bits(nb_dev, 0xb4, 0, 1<<6);
	set_nbmc_enable_bits(nb_dev, 0xc3, 1<<11, 0);
	set_nbmc_enable_bits(nb_dev, 0xa0, 1<<29, 0);
	nbmc_write_index(nb_dev, 0xa4, 0x3484576f);
	nbmc_write_index(nb_dev, 0xa5, 0x222222df);
	nbmc_write_index(nb_dev, 0xa6, 0x00000000);
	nbmc_write_index(nb_dev, 0xa7, 0x00000000);
	set_nbmc_enable_bits(nb_dev, 0xc3, 1<<8, 0);
	udelay(10);
	set_nbmc_enable_bits(nb_dev, 0xc3, 1<<9, 0);
	udelay(10);
	set_nbmc_enable_bits(nb_dev, 0x01, 0, 1<<2);
	udelay(200);
	set_nbmc_enable_bits(nb_dev, 0x01, 0, 1<<3);
	set_nbmc_enable_bits(nb_dev, 0xa0, 0, 1<<31);
	udelay(500);
	set_nbmc_enable_bits(nb_dev, 0x02, 0, 1<<31);
	set_nbmc_enable_bits(nb_dev, 0xa0, 0, 1<<30);
	set_nbmc_enable_bits(nb_dev, 0xa0, 1<<31, 0);
	set_nbmc_enable_bits(nb_dev, 0xa0, 0, 1<<29);
	nbmc_write_index(nb_dev, 0xa4, 0x23484576);
	nbmc_write_index(nb_dev, 0xa5, 0x00000000);
	nbmc_write_index(nb_dev, 0xa6, 0x00000000);
	nbmc_write_index(nb_dev, 0xa7, 0x00000000);
	/* GFX_StartMC finished. */

	/* GFX_SPPowerManagment, don't care for new. */
	/* Post MC Init table programming. */
	set_nbmc_enable_bits(nb_dev, 0xac, ~(0xfffffff0), 0x0b);

	/* Do we need Write and Read Calibration? */
	/* GFX_Init finished. */
#endif

	/* GFX_InitIntegratedInfo. */
	/* fill the Integrated Info Table. */
	vgainfo.sHeader.usStructureSize = sizeof(ATOM_INTEGRATED_SYSTEM_INFO_V2);
	vgainfo.sHeader.ucTableFormatRevision = 1;
	vgainfo.sHeader.ucTableContentRevision = 2;

#if !IS_ENABLED(CONFIG_GFXUMA) /* SP mode. */
	// Side port support is incomplete, do not use it
	// These parameters must match the motherboard
	vgainfo.ulBootUpSidePortClock = 667*100;
	vgainfo.ucMemoryType = 3;  // 3 = ddr3 sp mem, 2 = ddr2 sp mem
	vgainfo.ulMinSidePortClock = 333*100;
#endif

	vgainfo.ulBootUpEngineClock = 500 * 100;	       	// setup option on reference BIOS, 500 is default

	// find the DDR memory frequency
	if (is_family10h()) {
		value = pci_read_config32(k8_f2, 0x94);		// read channel 0 DRAM Configuration High Register
		if (extractbit(value, 14))			// if channel 0 disabled, channel 1 must have memory
			value = pci_read_config32(k8_f2, 0x194);// read channel 1 DRAM Configuration High Register
		vgainfo.ulBootUpUMAClock = memclk_lookup_fam10 [extractbits (value, 0, 2)] * 100;
	}
	if (is_family0Fh()) {
		value = pci_read_config32(k8_f2, 0x94);
		vgainfo.ulBootUpUMAClock = memclk_lookup_fam0F [extractbits (value, 20, 22)] * 100;
	}

	/* UMA Channel Number: 1 or 2. */
	vgainfo.ucUMAChannelNumber = 1;
	if (is_family0Fh()) {
		value = pci_read_config32(k8_f2, 0x90);
	if (extractbit(value, 11))  // 128-bit mode
		vgainfo.ucUMAChannelNumber = 2;
	}
	if (is_family10h()) {
		u32 dch0 = pci_read_config32(k8_f2, 0x94);
		u32 dch1 = pci_read_config32(k8_f2, 0x194);
		if (extractbit(dch0, 14) == 0 && extractbit(dch1, 14) == 0) { // both channels enabled
			value = pci_read_config32(k8_f2, 0x110);
			if (extractbit(value, 4))  // ganged mode
			vgainfo.ucUMAChannelNumber = 2;
		}
	}

	// processor type
	if (is_family0Fh())
		vgainfo.ulCPUCapInfo = 3;
	if (is_family10h())
		vgainfo.ulCPUCapInfo = 2;

	/* HT speed */
	value = pci_read_config8(nb_dev, 0xd1);
	value = ht_freq_lookup [value] * 100;  // HT link frequency in MHz
	vgainfo.ulHTLinkFreq = value * 100;    // HT frequency in units of 100 MHz
	vgainfo.ulHighVoltageHTLinkFreq = vgainfo.ulHTLinkFreq;
	vgainfo.ulLowVoltageHTLinkFreq = vgainfo.ulHTLinkFreq;

	if (value <= 1800)
		vgainfo.ulLowVoltageHTLinkFreq = vgainfo.ulHTLinkFreq;
	else {
		int sblink, cpuLnkFreqCap, nbLnkFreqCap;
		value = pci_read_config32(k8_f0, 0x64);
		sblink = extractbits(value, 8, 10);
		cpuLnkFreqCap = pci_read_config16(k8_f0, 0x8a + sblink * 0x20);
		nbLnkFreqCap = pci_read_config16(nb_dev, 0xd2);
		if (cpuLnkFreqCap & nbLnkFreqCap & (1 << 10)) // if both 1800 MHz capable
		vgainfo.ulLowVoltageHTLinkFreq = 1800*100;
	}

	/* HT width. */
	value = pci_read_config8(nb_dev, 0xcb);
	vgainfo.usMinDownStreamHTLinkWidth =
	vgainfo.usMaxDownStreamHTLinkWidth =
	vgainfo.usMinUpStreamHTLinkWidth =
	vgainfo.usMaxUpStreamHTLinkWidth =
	vgainfo.usMinHTLinkWidth =
	vgainfo.usMaxHTLinkWidth = ht_width_lookup [extractbits(value, 0, 2)];

	if (is_family0Fh()) {
		vgainfo.usUMASyncStartDelay = 322;
		vgainfo.usUMADataReturnTime = 286;
	}

	if (is_family10h()) {
		static u16 t0mult_lookup [] = {10, 50, 200, 2000};
		int t0time, t0scale;
		value = pci_read_config32(k8_f0, 0x16c);
		t0time = extractbits(value, 0, 3);
		t0scale = extractbits(value, 4, 5);
		vgainfo.usLinkStatusZeroTime = t0mult_lookup [t0scale] * t0time;
		vgainfo.usUMASyncStartDelay = 100;
		if (vgainfo.ulHTLinkFreq < 1000 * 100) { // less than 1000 MHz
			vgainfo.usUMADataReturnTime = 300;
			vgainfo.usLinkStatusZeroTime = 6 * 100;   // 6us for GH in HT1 mode
		}
		else {
			int lssel;
			value = pci_read_config32(nb_dev, 0xac);
			lssel = extractbits (value, 7, 8);
			vgainfo.usUMADataReturnTime = 1300;
			if (lssel == 0) vgainfo.usUMADataReturnTime = 150;
		}
	}

	/* Poweron DDI Lanes */
	poweron_ddi_lanes(nb_dev);

	printk(BIOS_DEBUG,"vgainfo:\n"
			"  ulBootUpEngineClock:%lu\n"
			"  ulBootUpUMAClock:%lu\n"
			"  ulBootUpSidePortClock:%lu\n"
			"  ulMinSidePortClock:%lu\n"
			"  ulSystemConfig:%lu\n"
			"  ulBootUpReqDisplayVector:%lu\n"
			"  ulOtherDisplayMisc:%lu\n"
			"  ulDDISlot1Config:%lu\n"
			"  ulDDISlot2Config:%lu\n"

			"  ucMemoryType:%u\n"
			"  ucUMAChannelNumber:%u\n"
			"  ucDockingPinBit:%u\n"
			"  ucDockingPinPolarity:%u\n"

			"  ulDockingPinCFGInfo:%lu\n"
			"  ulCPUCapInfo: %lu\n"

			"  usNumberOfCyclesInPeriod:%hu\n"
			"  usMaxNBVoltage:%hu\n"
			"  usMinNBVoltage:%hu\n"
			"  usBootUpNBVoltage:%hu\n"

			"  ulHTLinkFreq:%lu\n"

			"  usMinHTLinkWidth:%hu\n"
			"  usMaxHTLinkWidth:%hu\n"
			"  usUMASyncStartDelay:%hu\n"
			"  usUMADataReturnTime:%hu\n"
			"  usLinkStatusZeroTime:%hu\n"

			"  ulHighVoltageHTLinkFreq:%lu\n"
			"  ulLowVoltageHTLinkFreq:%lu\n"

			"  usMaxUpStreamHTLinkWidth:%hu\n"
			"  usMaxDownStreamHTLinkWidth:%hu\n"
			"  usMinUpStreamHTLinkWidth:%hu\n"
			"  usMinDownStreamHTLinkWidth:%hu\n",

			(unsigned long)vgainfo.ulBootUpEngineClock,
			(unsigned long)vgainfo.ulBootUpUMAClock,
			(unsigned long)vgainfo.ulBootUpSidePortClock,
			(unsigned long)vgainfo.ulMinSidePortClock,
			(unsigned long)vgainfo.ulSystemConfig,
			(unsigned long)vgainfo.ulBootUpReqDisplayVector,
			(unsigned long)vgainfo.ulOtherDisplayMisc,
			(unsigned long)vgainfo.ulDDISlot1Config,
			(unsigned long)vgainfo.ulDDISlot2Config,

			vgainfo.ucMemoryType,
			vgainfo.ucUMAChannelNumber,
			vgainfo.ucDockingPinBit,
			vgainfo.ucDockingPinPolarity,

			(unsigned long)vgainfo.ulDockingPinCFGInfo,
			(unsigned long)vgainfo.ulCPUCapInfo,

			vgainfo.usNumberOfCyclesInPeriod,
			vgainfo.usMaxNBVoltage,
			vgainfo.usMinNBVoltage,
			vgainfo.usBootUpNBVoltage,

			(unsigned long)vgainfo.ulHTLinkFreq,

			vgainfo.usMinHTLinkWidth,
			vgainfo.usMaxHTLinkWidth,
			vgainfo.usUMASyncStartDelay,
			vgainfo.usUMADataReturnTime,
			vgainfo.usLinkStatusZeroTime,

			(unsigned long)vgainfo.ulHighVoltageHTLinkFreq,
			(unsigned long)vgainfo.ulLowVoltageHTLinkFreq,

			vgainfo.usMaxUpStreamHTLinkWidth,
			vgainfo.usMaxDownStreamHTLinkWidth,
			vgainfo.usMinUpStreamHTLinkWidth,
			vgainfo.usMinDownStreamHTLinkWidth);


	/* Transfer the Table to VBIOS. */
	pointer = (u32 *)&vgainfo;
	for (i = 0; i < sizeof(ATOM_INTEGRATED_SYSTEM_INFO_V2); i+=4) {
#if IS_ENABLED(CONFIG_GFXUMA)
		*GpuF0MMReg = 0x80000000 + uma_memory_size - 512 + i;
#else
		*GpuF0MMReg = 0x80000000 + 0x8000000 - 512 + i;
#endif
		*(GpuF0MMReg+1) = *pointer++;
	}

	/* GFX_InitLate. */
	{
		u32 temp;
		temp = pci_read_config8(dev, 0x4);
		//temp &= ~1; /* CIM clears this bit. Strangely, I can'd. */
		temp |= 1<<1|1<<2;
		pci_write_config8(dev, 0x4, temp);

		// if the GFX debug bar is writable, then it has
		// been programmed and can be safely enabled now
		temp = pci_read_config32(nb_dev, 0x8c);

		// if bits 1 (intgfx_enable) and 9 (gfx_debug_bar_enable)
		// then enable gfx debug bar (set gxf_debug_decode_enable)
		if (temp & 0x202)
			temp |= (1 << 10);
		pci_write_config32(nb_dev, 0x8c, temp);

	}

#ifdef DONT_TRUST_RESOURCE_ALLOCATION
	/* NB_SetupMGMMIO. */

	/* clear MMIO and CreativeMMIO. */
	bpointer = (unsigned char *)MMIO;
	for (i = 0; i < sizeof(MMIO); i++) {
		*bpointer = 0;
		bpointer++;
	}
	bpointer = (unsigned char *)CreativeMMIO;
	for (i = 0; i < sizeof(CreativeMMIO); i++) {
		*bpointer = 0;
		bpointer++;
	}

	/* Set MMIO ranges in K8. */
	/* Set MMIO TOM - 4G. */
	SetMMIO(0x400<<12, 0x1000000, 0x80, &MMIO[0]);
	/* Set MMIO for VGA Legacy FB. */
	SetMMIO(0xa00, 0xc00, 0x80, &MMIO[0]);

	/* Set MMIO for non prefetchable P2P. */
	temp = pci_read_config32(dev0x14, 0x20);
	Base32 = (temp & 0x0fff0) << 8;
	Limit32 = ((temp & 0x0fff00000) + 0x100000) >> 8;
	if (Base32 < Limit32) {
		Status = GetCreativeMMIO(&CreativeMMIO[0]);
		if (Status != CIM_ERROR)
			SetMMIO(Base32, Limit32, 0x0, &MMIO[0]);
	}
	/* Set MMIO for prefetchable P2P. */
	if (Status != CIM_ERROR) {
		temp = pci_read_config32(dev0x14, 0x24);

		Base32 = (temp & 0x0fff0) <<8;
		Limit32 = ((temp & 0x0fff00000) + 0x100000) >> 8;
		if (Base32 < Limit32)
			SetMMIO(Base32, Limit32, 0x0, &MMIO[0]);
	}

	FinalizeMMIO(&MMIO[0]);

	ProgramMMIO(&CreativeMMIO[0], 0, MMIO_ATTRIB_NP_ONLY);
	ProgramMMIO(&MMIO[0], 0, MMIO_ATTRIB_NP_ONLY | MMIO_ATTRIB_BOTTOM_TO_TOP | MMIO_ATTRIB_SKIP_ZERO);
#endif

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

/**
 * Set registers in RS780 and CPU to disable the internal GFX.
 * Please refer to `rs780_internal_gfx_enable()`.
 */
static void rs780_internal_gfx_disable(struct device *dev)
{
	u32 l_dword;
	struct device *nb_dev = dev_find_slot(0, 0);

	/* Disable internal GFX and enable external GFX. */
	l_dword = pci_read_config32(nb_dev, 0x8c);
	l_dword |= 1<<0;
	l_dword &= ~(1<<1);
	pci_write_config32(nb_dev, 0x8c, l_dword);

	dev->enabled = 0;
}

/*
* Set registers in RS780 and CPU to enable the internal GFX.
* Please refer to CIM source code and BKDG.
*/

static void rs780_internal_gfx_enable(struct device *dev)
{
	u32 l_dword;
	int i;
	struct device *nb_dev = dev_find_slot(0, 0);
	msr_t sysmem;

#if !IS_ENABLED(CONFIG_GFXUMA)
	u32 FB_Start, FB_End;
#endif

	printk(BIOS_DEBUG, "rs780_internal_gfx_enable dev = 0x%p, nb_dev = 0x%p.\n", dev, nb_dev);

	/* The system top memory in 780. */
	sysmem = rdmsr(0xc001001a);
	printk(BIOS_DEBUG, "Sysmem TOM = %x_%x\n", sysmem.hi, sysmem.lo);
	pci_write_config32(nb_dev, 0x90, sysmem.lo);

	sysmem = rdmsr(0xc001001D);
	printk(BIOS_DEBUG, "Sysmem TOM2 = %x_%x\n", sysmem.hi, sysmem.lo);
	htiu_write_index(nb_dev, 0x31, sysmem.hi);
	htiu_write_index(nb_dev, 0x30, sysmem.lo | 1);

	/* Disable external GFX and enable internal GFX. */
	l_dword = pci_read_config32(nb_dev, 0x8c);
	l_dword &= ~(1<<0);
	l_dword |= 1<<1;
	pci_write_config32(nb_dev, 0x8c, l_dword);

	/* NB_SetDefaultIndexes */
	pci_write_config32(nb_dev, 0x94, 0x7f);
	pci_write_config32(nb_dev, 0x60, 0x7f);
	pci_write_config32(nb_dev, 0xe0, 0);

	/* NB_InitEarlyNB finished. */

	/* LPC DMA Deadlock workaround? */
	/* GFX_InitCommon*/
	struct device *k8_f0 = dev_find_slot(0, PCI_DEVFN(0x18, 0));
	l_dword = pci_read_config32(k8_f0, 0x68);
	l_dword &= ~(3 << 21);
	l_dword |= (1 << 21);
	pci_write_config32(k8_f0, 0x68, l_dword);

	/* GFX_InitCommon. */
	nbmc_write_index(nb_dev, 0x23, 0x00c00010);
	set_nbmc_enable_bits(nb_dev, 0x16, 1<<15, 1<<15);
	set_nbmc_enable_bits(nb_dev, 0x25, 0xffffffff, 0x111f111f);
	set_htiu_enable_bits(nb_dev, 0x37, 1<<24, 1<<24);

#if IS_ENABLED(CONFIG_GFXUMA)
	/* GFX_InitUMA. */
	/* Copy CPU DDR Controller to NB MC. */
	struct device *k8_f1 = dev_find_slot(0, PCI_DEVFN(0x18, 1));
	struct device *k8_f2 = dev_find_slot(0, PCI_DEVFN(0x18, 2));
	struct device *k8_f4 = dev_find_slot(0, PCI_DEVFN(0x18, 4));
	for (i = 0; i < 12; i++) {
		l_dword = pci_read_config32(k8_f2, 0x40 + i * 4);
		nbmc_write_index(nb_dev, 0x30 + i, l_dword);
	}

	l_dword = pci_read_config32(k8_f2, 0x80);
	nbmc_write_index(nb_dev, 0x3c, l_dword);
	l_dword = pci_read_config32(k8_f2, 0x94);
	set_nbmc_enable_bits(nb_dev, 0x3c, 0, !!(l_dword & (1<<22))<<16);
	set_nbmc_enable_bits(nb_dev, 0x3c, 0, !!(l_dword & (1<< 8))<<17);
	l_dword = pci_read_config32(k8_f2, 0x90);
	set_nbmc_enable_bits(nb_dev, 0x3c, 0, !!(l_dword & (1<<10))<<18);
   if (is_family10h()) {
	   for (i = 0; i < 12; i++) {
		   l_dword = pci_read_config32(k8_f2, 0x140 + i * 4);
		   nbmc_write_index(nb_dev, 0x3d + i, l_dword);
	   }

	   l_dword = pci_read_config32(k8_f2, 0x180);
	   nbmc_write_index(nb_dev, 0x49, l_dword);
	   l_dword = pci_read_config32(k8_f2, 0x194);
	   set_nbmc_enable_bits(nb_dev, 0x49, 0, !!(l_dword & (1<<22))<<16);
	   set_nbmc_enable_bits(nb_dev, 0x49, 0, !!(l_dword & (1<< 8))<<17);
	   l_dword = pci_read_config32(k8_f2, 0x190);
	   set_nbmc_enable_bits(nb_dev, 0x49, 0, !!(l_dword & (1<<10))<<18);

	   l_dword = pci_read_config32(k8_f2, 0x110);
	   nbmc_write_index(nb_dev, 0x4a, l_dword);
	   l_dword = pci_read_config32(k8_f2, 0x114);
	   nbmc_write_index(nb_dev, 0x4b, l_dword);
	   l_dword = pci_read_config32(k8_f4, 0x44);
	   set_nbmc_enable_bits(nb_dev, 0x4a, 0, !!(l_dword & (1<<22))<<24);
	   l_dword = pci_read_config32(k8_f1, 0x40);
	   nbmc_write_index(nb_dev, 0x4c, l_dword);
	   l_dword = pci_read_config32(k8_f1, 0xf0);
	   nbmc_write_index(nb_dev, 0x4d, l_dword);
   }


	/* Set UMA in the 780 side. */
	/* UMA start address, size. */
	/* The UMA starts at 0xC0000000 of internal RS780 address space
	    [31:16] addr of last byte | [31:16] addr of first byte
	*/
	nbmc_write_index(nb_dev, 0x10, ((uma_memory_size - 1 + 0xC0000000) & (~0xffff)) | 0xc000);
	nbmc_write_index(nb_dev, 0x11, uma_memory_base);
	nbmc_write_index(nb_dev, 0x12, 0);
	nbmc_write_index(nb_dev, 0xf0, uma_memory_size >> 20);
	/* GFX_InitUMA finished. */
#else
	/* GFX_InitSP. */
	/* SP memory:Hynix HY5TQ1G631ZNFP. 128MB = 64M * 16. 667MHz. DDR3. */

	/* Enable Async mode. */
	set_nbmc_enable_bits(nb_dev, 0x06, 7<<8, 1<<8);
	set_nbmc_enable_bits(nb_dev, 0x08, 1<<10, 0);
	/* The last item in AsynchMclkTaskFileIndex. Why? */
	/* MC_MPLL_CONTROL2. */
	nbmc_write_index(nb_dev, 0x07, 0x40100028);
	/* MC_MPLL_DIV_CONTROL. */
	nbmc_write_index(nb_dev, 0x0b, 0x00000028);
	/* MC_MPLL_FREQ_CONTROL. */
	set_nbmc_enable_bits(nb_dev, 0x09, 3<<12|15<<16|15<<8, 1<<12|4<<16|0<<8);
	/* MC_MPLL_CONTROL3. For PM. */
	set_nbmc_enable_bits(nb_dev, 0x08, 0xff<<13, 1<<13|1<<18);
	/* MPLL_CAL_TRIGGER. */
	set_nbmc_enable_bits(nb_dev, 0x06, 0, 1<<0);
	udelay(200); /* time is long enough? */
	set_nbmc_enable_bits(nb_dev, 0x06, 0, 1<<1);
	set_nbmc_enable_bits(nb_dev, 0x06, 1<<0, 0);
	/* MCLK_SRC_USE_MPLL. */
	set_nbmc_enable_bits(nb_dev, 0x02, 0, 1<<20);

	/* Pre Init MC. */
	nbmc_write_index(nb_dev, 0x01, 0x88108280);
	set_nbmc_enable_bits(nb_dev, 0x02, ~(1<<20), 0x00030200);
	nbmc_write_index(nb_dev, 0x04, 0x08881018);
	nbmc_write_index(nb_dev, 0x05, 0x000000bb);
	nbmc_write_index(nb_dev, 0x0c, 0x0f00001f);
	nbmc_write_index(nb_dev, 0xa1, 0x01f10000);
	/* MCA_INIT_DLL_PM. */
	set_nbmc_enable_bits(nb_dev, 0xc9, 1<<24, 1<<24);
	nbmc_write_index(nb_dev, 0xa2, 0x74f20000);
	nbmc_write_index(nb_dev, 0xa3, 0x8af30000);
	nbmc_write_index(nb_dev, 0xaf, 0x47d0a41c);
	nbmc_write_index(nb_dev, 0xb0, 0x88800130);
	nbmc_write_index(nb_dev, 0xb1, 0x00000040);
	nbmc_write_index(nb_dev, 0xb4, 0x41247000);
	nbmc_write_index(nb_dev, 0xb5, 0x00066664);
	nbmc_write_index(nb_dev, 0xb6, 0x00000022);
	nbmc_write_index(nb_dev, 0xb7, 0x00000044);
	nbmc_write_index(nb_dev, 0xb8, 0xbbbbbbbb);
	nbmc_write_index(nb_dev, 0xb9, 0xbbbbbbbb);
	nbmc_write_index(nb_dev, 0xba, 0x55555555);
	nbmc_write_index(nb_dev, 0xc1, 0x00000000);
	nbmc_write_index(nb_dev, 0xc2, 0x00000000);
	nbmc_write_index(nb_dev, 0xc3, 0x80006b00);
	nbmc_write_index(nb_dev, 0xc4, 0x00066664);
	nbmc_write_index(nb_dev, 0xc5, 0x00000000);
	nbmc_write_index(nb_dev, 0xd2, 0x00000022);
	nbmc_write_index(nb_dev, 0xd3, 0x00000044);
	nbmc_write_index(nb_dev, 0xd6, 0x00050005);
	nbmc_write_index(nb_dev, 0xd7, 0x00000000);
	nbmc_write_index(nb_dev, 0xd8, 0x00700070);
	nbmc_write_index(nb_dev, 0xd9, 0x00700070);
	nbmc_write_index(nb_dev, 0xe0, 0x00200020);
	nbmc_write_index(nb_dev, 0xe1, 0x00200020);
	nbmc_write_index(nb_dev, 0xe8, 0x00200020);
	nbmc_write_index(nb_dev, 0xe9, 0x00200020);
	nbmc_write_index(nb_dev, 0xe0, 0x00180018);
	nbmc_write_index(nb_dev, 0xe1, 0x00180018);
	nbmc_write_index(nb_dev, 0xe8, 0x00180018);
	nbmc_write_index(nb_dev, 0xe9, 0x00180018);

	/* Misc options. */
	/* Memory Termination. */
	set_nbmc_enable_bits(nb_dev, 0xa1, 0x0ff, 0x044);
	set_nbmc_enable_bits(nb_dev, 0xb4, 0xf00, 0xb00);
#if 0
	/* Controller Termination. */
	set_nbmc_enable_bits(nb_dev, 0xb1, 0x77770000, 0x77770000);
#endif

	/* OEM Init MC. 667MHz. */
	nbmc_write_index(nb_dev, 0xa8, 0x7a5aaa78);
	nbmc_write_index(nb_dev, 0xa9, 0x514a2319);
	nbmc_write_index(nb_dev, 0xaa, 0x54400520);
	nbmc_write_index(nb_dev, 0xab, 0x441460ff);
	nbmc_write_index(nb_dev, 0xa0, 0x20f00a48);
	set_nbmc_enable_bits(nb_dev, 0xa2, ~(0xffffffc7), 0x10);
	nbmc_write_index(nb_dev, 0xb2, 0x00000303);
	set_nbmc_enable_bits(nb_dev, 0xb1, ~(0xffffff70), 0x45);
	/* Do it later. */
	/* set_nbmc_enable_bits(nb_dev, 0xac, ~(0xfffffff0), 0x0b); */

	/* Init PM timing. */
	for (i = 0; i < 4; i++) {
		l_dword = nbmc_read_index(nb_dev, 0xa0+i);
		nbmc_write_index(nb_dev, 0xc8+i, l_dword);
	}
	for (i = 0; i < 4; i++) {
		l_dword = nbmc_read_index(nb_dev, 0xa8+i);
		nbmc_write_index(nb_dev, 0xcc+i, l_dword);
	}
	l_dword = nbmc_read_index(nb_dev, 0xb1);
	set_nbmc_enable_bits(nb_dev, 0xc8, 0xff<<24, ((l_dword&0x0f)<<24)|((l_dword&0xf00)<<20));

	/* Init MC FB. */
	/* FB_Start = ; FB_End = ; iSpSize = 0x0080, 128MB. */
	nbmc_write_index(nb_dev, 0x11, 0x40000000);
	FB_Start = 0xc00 + 0x080;
	FB_End = 0xc00 + 0x080;
	nbmc_write_index(nb_dev, 0x10, (((FB_End&0xfff)<<20)-0x10000)|(((FB_Start&0xfff)-0x080)<<4));
	set_nbmc_enable_bits(nb_dev, 0x0d, ~0x000ffff0, (FB_Start&0xfff)<<20);
	nbmc_write_index(nb_dev, 0x0f, 0);
	nbmc_write_index(nb_dev, 0x0e, (FB_Start&0xfff)|(0xaaaa<<12));
#endif

	/* GFX_InitSP finished. */
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations pcie_ops = {
	.read_resources = rs780_gfx_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = internal_gfx_pci_dev_init,	/* The option ROM initializes the device. rs780_gfx_init, */
	.scan_bus = 0,
	.enable = rs780_internal_gfx_enable,
	.disable = rs780_internal_gfx_disable,
	.ops_pci = &lops_pci,
};

/*
 * We should list all of them here.
 * */
static const unsigned short pcie_780_ids[] = {
	PCI_DEVICE_ID_ATI_RS780_INT_GFX,
	PCI_DEVICE_ID_ATI_RS780C_INT_GFX,
	PCI_DEVICE_ID_ATI_RS780M_INT_GFX,
	PCI_DEVICE_ID_ATI_RS780MC_INT_GFX,
	PCI_DEVICE_ID_ATI_RS780E_INT_GFX,
	PCI_DEVICE_ID_ATI_RS785G_INT_GFX,
	PCI_DEVICE_ID_ATI_RS785C_INT_GFX,
	PCI_DEVICE_ID_ATI_RS785M_INT_GFX,
	PCI_DEVICE_ID_ATI_RS785MC_INT_GFX,
	PCI_DEVICE_ID_ATI_RS785D_INT_GFX,
	0
};

static const struct pci_driver pcie_driver_780 __pci_driver = {
	.ops = &pcie_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.devices = pcie_780_ids,
};

/* step 12 ~ step 14 from rpr */
static void single_port_configuration(struct device *nb_dev, struct device *dev)
{
	u8 result, width;
	u32 reg32;
	struct southbridge_amd_rs780_config *cfg =
	    (struct southbridge_amd_rs780_config *)nb_dev->chip_info;

	printk(BIOS_DEBUG, "rs780_gfx_init single_port_configuration.\n");

	/* step 12 training, releases hold training for GFX port 0 (device 2) */
	PcieReleasePortTraining(nb_dev, dev, 2);
	result = PcieTrainPort(nb_dev, dev, 2);
	printk(BIOS_DEBUG, "rs780_gfx_init single_port_configuration step12.\n");

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
		AtiPcieCfg.PortDetect |= 1 << 2; /* Port 2 */
		set_pcie_enable_bits(dev, 0xA2, 0xFF, 0x1);
		reg32 = nbpcie_p_read_index(dev, 0x29);
		width = reg32 & 0xFF;
		printk(BIOS_DEBUG, "GFX Inactive Lanes = 0x%x.\n", width);
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
	printk(BIOS_DEBUG, "rs780_gfx_init single_port_configuration step13.\n");

	/* step 14 Reset Enumeration Timer, disables the shortening of the enumeration timer */
	set_pcie_enable_bits(dev, 0x70, 1 << 19, 1 << 19);
	printk(BIOS_DEBUG, "rs780_gfx_init single_port_configuration step14.\n");
}

static void dual_port_configuration(struct device *nb_dev, struct device *dev)
{
	u8 result, width;
	u32 reg32, dev_ind = dev->path.pci.devfn >> 3;
	struct southbridge_amd_rs780_config *cfg =
		    (struct southbridge_amd_rs780_config *)nb_dev->chip_info;

	/* 5.4.1.2 Dual Port Configuration */
	set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 1 << 31);
	set_nbmisc_enable_bits(nb_dev, 0x08, 0xF << 8, 0x5 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 0 << 31);

	/* 5.7. Training for Device 2 */
	/* 5.7.1. Releases hold training for GFX port 0 (device 2) */
	PcieReleasePortTraining(nb_dev, dev, dev_ind);
	/* 5.7.2- 5.7.9. PCIE Link Training Sequence */
	result = PcieTrainPort(nb_dev, dev, dev_ind);

	/* Power Down Control for Device 2 */
	/* Link Training was NOT successful */
	if (!result) {
		/* Powers down all lanes for port A */
		/* nbpcie_ind_write_index(nb_dev, 0x65, 0x0f0f); */
		/* Note: I have to disable the slot where there isn't a device,
		 * otherwise the system will hang. I don't know why. */
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << dev_ind, 1 << dev_ind);

	} else {		/* step 16.b Link Training was successful */
		AtiPcieCfg.PortDetect |= 1 << dev_ind;
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
static void dynamic_link_width_control(struct device *nb_dev, struct device *dev, u8 width)
{
	u32 reg32;
	struct device *sb_dev;
	struct southbridge_amd_rs780_config *cfg =
	    (struct southbridge_amd_rs780_config *)nb_dev->chip_info;

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
void rs780_gfx_init(struct device *nb_dev, struct device *dev, u32 port)
{
	u32 reg32;
	struct southbridge_amd_rs780_config *cfg =
	    (struct southbridge_amd_rs780_config *)nb_dev->chip_info;

	printk(BIOS_DEBUG, "rs780_gfx_init, nb_dev=0x%p, dev=0x%p, port=0x%x.\n",
		    nb_dev, dev, port);

	/* GFX Core Initialization */
	//if (port == 2) return;

	/* step 2, TMDS, (only need if CMOS option is enabled) */
	if (cfg->gfx_tmds) {
		/**
		 * PCIe Initialization for DDI.
		 * The VBIOS/Driver is responsible for DDI programming sequence,
		 * The SBIOS is responsible for programming the lane and clock muxing specific to each case.
		 * Refer to RPR Chapter 7: "PCIe Initialization for DDI".
		 * Note: This programming must be done before hold training is released.
		 */
		switch (cfg->gfx_pcie_config) {
			case 1: /* 1x16 GFX -default case, no programming required */
				break;
			case 2: /* 1x8 GFX on Lanes 0-7 */
			case 5: /* 1x4 GPP on Lanes 0-3 */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 6, 0x1 << 6); /* Disables PCIe mode on PHY Lanes  8-11 */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 7, 0x1 << 7); /* Disables PCIe mode on PHY Lanes 12-15 */
				break;
			case 3: /* 1x8 on Lanes 8-15 */
			case 7: /* 1x4 GPP on Lanes 8-11 */
				/* TXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x07, 1 << 16, 1 << 16);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0xF << 12, 0xF << 12);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0x3 << 24, 0x2 << 24);
				set_nbmisc_enable_bits(nb_dev, 0x28, 0x3 << 0, 0x0 << 0);
				/* RXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 8, 0x2 << 8);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 10, 0x2 << 10);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 12, 0x2 << 12);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 14, 0x2 << 14);
				/* TX Lane Muxing */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 2, 0x1 << 2);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 3, 0x1 << 3);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 4, 0x1 << 4);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 5, 0x1 << 5);
				break;
			case 4:  /* 2x8 */
			case 10: /* 1x4 GPP on Lanes 0-3 and 1x4 GPP on Lanes 8-11 */
			case 14: /* 1x8 GFX on Lanes 0-7 and 1x4 GPP on Lanes 8-11 */
			case 17: /* 1x4 GPP on Lanes 0-3 and 1x8 GFX on Lanes 8-15 */
				/* Set dual slot configuration */
				set_nbmisc_enable_bits(nb_dev, 0x08, 0xF << 8, 0x5 << 8);
				break;
			case 9: /* PCIe 2x4 GPPs on Lanes 0-7 */
			case 6: /* PCIe 1x4 GPP on Lanes 4-7 */
				/* Set dual slot configuration */
				set_nbmisc_enable_bits(nb_dev, 0x08, 0xF << 8, 0x5 << 8);
				/* TXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x07, 1 << 16, 0 << 16);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0xF << 12, 0x0 << 12);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0x3 << 20, 0x0 << 20);
				set_nbmisc_enable_bits(nb_dev, 0x28, 0x1 << 0, 0x0 << 0);
				/* RXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 8, 0x0 << 8);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 10, 0x1 << 10);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 12, 0x3 << 12);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 14, 0x0 << 14);
				/* TX Lane Muxing */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 1, 0x1 << 1);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 6, 0x1 << 6);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 7, 0x1 << 7);
				break;
			case 13: /* 2x4 GPPs on Lanes 8-15 */
			case 8:  /* 1x4 GPP on Lanes 12-15 */
				/* Set dual slot configuration */
				set_nbmisc_enable_bits(nb_dev, 0x08, 0xF << 8, 0x5 << 8);
				/* TXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x07, 1 << 16, 1 << 16);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0xF << 12, 0xF << 12);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0x3 << 24, 0x2 << 24);
				set_nbmisc_enable_bits(nb_dev, 0x28, 0x3 << 0, 0x3 << 0);
				/* RXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 8, 0x2 << 8);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 10, 0x3 << 10);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 12, 0x1 << 12);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 14, 0x2 << 14);
				/* TX Lane Muxing */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 2, 0x1 << 2);
				set_nbmisc_enable_bits(nb_dev, 0x28, 0x1 << 14, 0x1 << 14);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 4, 0x1 << 4);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 5, 0x1 << 5);
				break;
			case 15: /* 1x8 GFX on Lanes 0-7 and 1x4 GPP on Lanes 12-15 */
			case 11: /* 1x4 GPP on Lanes 0-3 and 1x4 GPP on Lanes 12-15 */
				/* Set dual slot configuration */
				set_nbmisc_enable_bits(nb_dev, 0x08, 0xF << 8, 0x5 << 8);
				/* TXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x07, 1 << 16, 0 << 16);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0xF << 12, 0x0 << 12);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0x3 << 20, 0x0 << 20);
				set_nbmisc_enable_bits(nb_dev, 0x28, 0x3 << 0, 0x1 << 0);
				/* RXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 8, 0x0 << 8);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 10, 0x0 << 10);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 12, 0x1 << 12);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 14, 0x3 << 14);
				/* TX Lane Muxing */
				set_nbmisc_enable_bits(nb_dev, 0x28, 0x1 << 14, 0x1 << 14);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 6, 0x1 << 6);
				break;
			case 16: /* 1x8 GFX on Lanes 8-15 and 1x4 GPP on Lanes 4-7 */
			case 12: /* 1x4 GPP on Lanes 4-7 and 1x8 GFX on Lanes 8-15 */
				/* Set dual slot configuration */
				set_nbmisc_enable_bits(nb_dev, 0x08, 0xF << 8, 0x5 << 8);
				/* TXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x07, 1 << 16, 1 << 16);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0xF << 12, 0xF << 12);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0x3 << 24, 0x2 << 24);
				set_nbmisc_enable_bits(nb_dev, 0x07, 0x3 << 22, 0x2 << 22);
				set_nbmisc_enable_bits(nb_dev, 0x28, 0x3 << 0, 0x2 << 0);
				/* RXCLK */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 8, 0x2 << 8);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 10, 0x2 << 10);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 12, 0x3 << 12);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x3 << 14, 0x1 << 14);
				/* TX Lane Muxing */
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 2, 0x1 << 2);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 3, 0x1 << 3);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 1, 0x1 << 1);
				set_nbmisc_enable_bits(nb_dev, 0x27, 0x1 << 4, 0x1 << 4);
				break;
			default:
				printk(BIOS_INFO, "Incorrect configuration of external GFX slot.\n");
				break;
		}

		/* DDI Configuration */
		switch (cfg->gfx_ddi_config) {
			case 1: /* DDI_SL lanes0-3 */
				nbmisc_write_index(nb_dev, 0x74, GFX_CONFIG_DDI);
				break;
			case 2: /* DDI_SL lanes4-7 */
				nbmisc_write_index(nb_dev, 0x74, (GFX_CONFIG_DDI << 8));
				break;
			case 5: /* DDI_SL lanes0-4, lanes4-7 */
				nbmisc_write_index(nb_dev, 0x74, (GFX_CONFIG_DDI << 8) | GFX_CONFIG_DDI);
				break;
			case 6: /* DDI_DL lanes0-7 */
				nbmisc_write_index(nb_dev, 0x74, (GFX_CONFIG_DDI << 8) | GFX_CONFIG_DDI);
				break;
			default:
				printk(BIOS_INFO, "Incorrect configuration of external GFX slot.\n");
				break;
		}
	}

#if 1				/* external clock mode */
	/* table 5-22, 5.9.1. REFCLK */
	/* 5.9.1.1. Disables the GFX REFCLK transmitter so that the GFX
	 * REFCLK PAD can be driven by an external source. */
	/* 5.9.1.2. Enables GFX REFCLK receiver to receive the REFCLK from an external source. */
	set_nbmisc_enable_bits(nb_dev, 0x38, 1 << 29 | 1 << 28 | 1 << 26, 1 << 28);

	/* 5.9.1.3 Selects the GFX REFCLK to be the source for PLL A. */
	/* 5.9.1.4 Selects the GFX REFCLK to be the source for PLL B. */
	/* 5.9.1.5 Selects the GFX REFCLK to be the source for PLL C. */
	reg32 = nbmisc_read_index(nb_dev, 0x28);
	printk(BIOS_DEBUG, "misc 28 = %x\n", reg32);

	/* 5.9.1.6.Selects the single ended GFX REFCLK to be the source for core logic. */
	set_nbmisc_enable_bits(nb_dev, 0x6C, 1 << 31, 1 << 31);
#else				/* internal clock mode */
	/* table 5-23, 5.9.1. REFCLK */
	/* 5.9.1.1. Enables the GFX REFCLK transmitter so that the GFX
	 * REFCLK PAD can be driven by the SB REFCLK. */
	/* 5.9.1.2. Disables GFX REFCLK receiver from receiving the
	 * REFCLK from an external source.*/
	set_nbmisc_enable_bits(nb_dev, 0x38, 1 << 29 | 1 << 28, 1 << 29 | 0 << 28);

	/* 5.9.1.3 Selects the GFX REFCLK to be the source for PLL A. */
	/* 5.9.1.4 Selects the GFX REFCLK to be the source for PLL B. */
	/* 5.9.1.5 Selects the GFX REFCLK to be the source for PLL C. */
	set_nbmisc_enable_bits(nb_dev, 0x28, 3 << 6 | 3 << 8 | 3 << 10,
			       0);
	reg32 = nbmisc_read_index(nb_dev, 0x28);
	printk(BIOS_DEBUG, "misc 28 = %x\n", reg32);

	/* 5.9.1.6.Selects the single ended GFX REFCLK to be the source for core logic. */
	set_nbmisc_enable_bits(nb_dev, 0x6C, 1 << 31, 0 << 31);
#endif

	/* step 5.9.3, GFX overclocking, (only need if CMOS option is enabled) */
	/* 5.9.3.1. Increases PLL BW for 6G operation.*/
	/* set_nbmisc_enable_bits(nb_dev, 0x36, 0x3FF << 4, 0xB5 << 4); */
	/* skip */

	/* step 5.9.4, reset the GFX link */
	/* step 5.9.4.1 asserts both calibration reset and global reset */
	set_nbmisc_enable_bits(nb_dev, 0x8, 0x3 << 14, 0x3 << 14);

	/* step 5.9.4.2 de-asserts calibration reset */
	set_nbmisc_enable_bits(nb_dev, 0x8, 1 << 14, 0 << 14);

	/* step 5.9.4.3 wait for at least 200us */
	udelay(300);

	/* step 5.9.4.4 de-asserts global reset */
	set_nbmisc_enable_bits(nb_dev, 0x8, 1 << 15, 0 << 15);

	/* 5.9.5 Reset PCIE_GFX Slot */
	/* It is done in mainboard.c */
	set_pcie_reset();
	mdelay(1);
	set_pcie_dereset();

	/* step 5.9.8 program PCIE memory mapped configuration space */
	/* done by enable_pci_bar3() before */

	/* step 7 compliance state, (only need if CMOS option is enabled) */
	/* the compliance state is just for test. refer to 4.2.5.2 of PCIe specification */
	if (cfg->gfx_compliance) {
		/* force compliance */
		set_nbmisc_enable_bits(nb_dev, 0x32, 1 << 6, 1 << 6);
		/* release hold training for device 2. GFX initialization is done. */
		set_nbmisc_enable_bits(nb_dev, 0x8, 1 << 4, 0 << 4);
		dynamic_link_width_control(nb_dev, dev, cfg->gfx_link_width);
		printk(BIOS_DEBUG, "rs780_gfx_init step7.\n");
		return;
	}

	/* 5.9.12 Core Initialization. */
	/* 5.9.12.1 sets RCB timeout to be 25ms */
	/* 5.9.12.2. RCB Cpl timeout on link down. */
	set_pcie_enable_bits(dev, 0x70, 7 << 16 | 1 << 19, 4 << 16 | 1 << 19);
	printk(BIOS_DEBUG, "rs780_gfx_init step5.9.12.1.\n");

	/* step 5.9.12.3 disables slave ordering logic */
	set_pcie_enable_bits(nb_dev, 0x20, 1 << 8, 1 << 8);
	printk(BIOS_DEBUG, "rs780_gfx_init step5.9.12.3.\n");

	/* step 5.9.12.4 sets DMA payload size to 64 bytes */
	set_pcie_enable_bits(nb_dev, 0x10, 7 << 10, 4 << 10);
	/* 5.9.12.5. Blocks DMA traffic during C3 state. */
	set_pcie_enable_bits(dev, 0x10, 1 << 0, 0 << 0);

	/* 5.9.12.6. Disables RC ordering logic */
	set_pcie_enable_bits(nb_dev, 0x20, 1 << 9, 1 << 9);

	/* Enables TLP flushing. */
	/* Note: It is got from RS690. The system will hang without this action. */
	set_pcie_enable_bits(dev, 0x20, 1 << 19, 0 << 19);

	/* 5.9.12.7. Ignores DLLPs during L1 so that txclk can be turned off */
	set_pcie_enable_bits(nb_dev, 0x2, 1 << 0, 1 << 0);

	/* 5.9.12.8 Prevents LC to go from L0 to Rcv_L0s if L1 is armed. */
	set_pcie_enable_bits(dev, 0xA1, 1 << 11, 1 << 11);

	/* 5.9.12.9 CMGOOD_OVERRIDE for end point initiated lane degradation. */
	set_nbmisc_enable_bits(nb_dev, 0x6a, 1 << 17, 1 << 17);
	printk(BIOS_DEBUG, "rs780_gfx_init step5.9.12.9.\n");

	/* 5.9.12.10 Sets the timer in Config state from 20us to */
	/* 5.9.12.11 De-asserts RX_EN in L0s. */
	/* 5.9.12.12 Enables de-assertion of PG2RX_CR_EN to lock clock
	 * recovery parameter when lane is in electrical idle in L0s.*/
	set_pcie_enable_bits(dev, 0xB1, 1 << 23 | 1 << 19 | 1 << 28, 1 << 23 | 1 << 19 | 1 << 28);

	/* 5.9.12.13. Turns off offset calibration. */
	/* 5.9.12.14. Enables Rx Clock gating in CDR */
	set_nbmisc_enable_bits(nb_dev, 0x34, 1 << 10/* | 1 << 22 */, 1 << 10/* | 1 << 22 */);

	/* 5.9.12.15. Sets number of TX Clocks to drain TX Pipe to 3. */
	set_pcie_enable_bits(dev, 0xA0, 0xF << 4, 3 << 4);

	/* 5.9.12.16. Lets PI use Electrical Idle from PHY when
	 * turning off PLL in L1 at Gen2 speed instead Inferred Electrical Idle. */
	set_pcie_enable_bits(nb_dev, 0x40, 3 << 14, 2 << 14);

	/* 5.9.12.17. Prevents the Electrical Idle from causing a transition from Rcv_L0 to Rcv_L0s. */
	set_pcie_enable_bits(dev, 0xB1, 1 << 20, 1 << 20);

	/* 5.9.12.18. Prevents the LTSSM from going to Rcv_L0s if it has already
	 * acknowledged a request to go to L1. */
	set_pcie_enable_bits(dev, 0xA1, 1 << 11, 1 << 11);

	/* 5.9.12.19. LDSK only taking deskew on deskewing error detect */
	set_pcie_enable_bits(nb_dev, 0x40, 1 << 28, 0 << 28);

	/* 5.9.12.20. Bypasses lane de-skew logic if in x1 */
	set_pcie_enable_bits(nb_dev, 0xC2, 1 << 14, 1 << 14);

	/* 5.9.12.21. Sets Electrical Idle Threshold. */
	set_nbmisc_enable_bits(nb_dev, 0x35, 3 << 21, 2 << 21);

	/* 5.9.12.22. Advertises -6 dB de-emphasis value in TS1 Data Rate Identifier
	 * Only if CMOS Option in section. skip */

	/* 5.9.12.23. Disables GEN2 capability of the device. */
	set_pcie_enable_bits(dev, 0xA4, 1 << 0, 0 << 0);

	/* 5.9.12.24.Disables advertising Upconfigure Support. */
	set_pcie_enable_bits(dev, 0xA2, 1 << 13, 1 << 13);

	/* 5.9.12.25. No comment in RPR. */
	set_nbmisc_enable_bits(nb_dev, 0x39, 1 << 10, 0 << 10);

	/* 5.9.12.26. This capacity is required since links wider than x1 and/or multiple link
	 * speed are supported */
	set_pcie_enable_bits(nb_dev, 0xC1, 1 << 0, 1 << 0);

	/* 5.9.12.27. Enables NVG86 ECO. A13 above only. */
	if (get_nb_rev(nb_dev) == REV_RS780_A12)			/* A12 */
		set_pcie_enable_bits(dev, 0x02, 1 << 11, 1 << 11);

	/* 5.9.12.28 Hides and disables the completion timeout method. */
	set_pcie_enable_bits(nb_dev, 0xC1, 1 << 2, 0 << 2);

	/* 5.9.12.29. Use the bif_core de-emphasis strength by default. */
	/* set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 28, 1 << 28); */

	/* 5.9.12.30. Set TX arbitration algorithm to round robin */
	set_pcie_enable_bits(nb_dev, 0x1C,
			     1 << 0 | 0x1F << 1 | 0x1F << 6,
			     1 << 0 | 0x04 << 1 | 0x04 << 6);

	/* Single-port/Dual-port configuration. */
	switch (cfg->gfx_dual_slot) {
	case 0:
		/* step 1, lane reversal (only need if build config option is enabled) */
		if (cfg->gfx_lane_reversal) {
			set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 1 << 31);
			set_nbmisc_enable_bits(nb_dev, 0x33, 1 << 2, 1 << 2);
			set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 0 << 31);
		}
		printk(BIOS_DEBUG, "rs780_gfx_init step1.\n");

		printk(BIOS_DEBUG, "device = %x\n", dev->path.pci.devfn >> 3);
		if ((dev->path.pci.devfn >> 3) == 2) {
			single_port_configuration(nb_dev, dev);
		} else {
			set_nbmisc_enable_bits(nb_dev, 0xc, 0, 0x2 << 2); /* hide the GFX bridge. */
			printk(BIOS_INFO, "Single port. Do nothing.\n"); // If dev3
		}

		break;
	case 1:
		/* step 1, lane reversal (only need if build config option is enabled) */
		if (cfg->gfx_lane_reversal) {
			set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 1 << 31);
			set_nbmisc_enable_bits(nb_dev, 0x33, 1 << 2, 1 << 2);
			set_nbmisc_enable_bits(nb_dev, 0x33, 1 << 3, 1 << 3);
			set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 0 << 31);
		}
		printk(BIOS_DEBUG, "rs780_gfx_init step1.\n");
		/* step 1.1, dual-slot gfx configuration (only need if CMOS option is enabled) */
		/* AMD calls the configuration CrossFire */
		set_nbmisc_enable_bits(nb_dev, 0x0, 0xf << 8, 5 << 8);
		printk(BIOS_DEBUG, "rs780_gfx_init step2.\n");

		printk(BIOS_DEBUG, "device = %x\n", dev->path.pci.devfn >> 3);
		dual_port_configuration(nb_dev, dev);
		break;

	case 2:
		if (is_dev3_present()) {
			/* step 1, lane reversal (only need if CMOS option is enabled) */
			if (cfg->gfx_lane_reversal) {
				set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 1 << 31);
				set_nbmisc_enable_bits(nb_dev, 0x33, 1 << 2, 1 << 2);
				set_nbmisc_enable_bits(nb_dev, 0x33, 1 << 3, 1 << 3);
				set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 0 << 31);
			}
			printk(BIOS_DEBUG, "rs780_gfx_init step1.\n");
			/* step 1.1, dual-slot gfx configuration (only need if CMOS option is enabled) */
			/* AMD calls the configuration CrossFire */
			set_nbmisc_enable_bits(nb_dev, 0x0, 0xf << 8, 5 << 8);
			printk(BIOS_DEBUG, "rs780_gfx_init step2.\n");


			printk(BIOS_DEBUG, "device = %x\n", dev->path.pci.devfn >> 3);
			dual_port_configuration(nb_dev, dev);

		} else {
			if (cfg->gfx_lane_reversal) {
				set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 1 << 31);
				set_nbmisc_enable_bits(nb_dev, 0x33, 1 << 2, 1 << 2);
				set_nbmisc_enable_bits(nb_dev, 0x36, 1 << 31, 0 << 31);
			}
			printk(BIOS_DEBUG, "rs780_gfx_init step1.\n");

			if ((dev->path.pci.devfn >> 3) == 2)
				single_port_configuration(nb_dev, dev);
			else {
				set_nbmisc_enable_bits(nb_dev, 0xc, 0, 0x2 << 2); /* hide the GFX bridge. */
				printk(BIOS_DEBUG, "If dev3.., single port. Do nothing.\n");
			    }
		}
		break;

	default:
		printk(BIOS_INFO, "Incorrect configuration of external GFX slot.\n");
		break;
	}
}
