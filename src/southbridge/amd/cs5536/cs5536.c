/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <device/smbus.h>
#include <console/console.h>
#include <stdint.h>
#include <pc80/isa-dma.h>
#include <pc80/mc146818rtc.h>
#include <pc80/i8259.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/vr.h>
#include <stdlib.h>
#include "chip.h"
#include "cs5536.h"
#include "smbus.h"

struct msrinit {
	u32 msrnum;
	msr_t msr;
};

/*	Master Configuration Register for Bus Masters.*/
static struct msrinit SB_MASTER_CONF_TABLE[] = {
	{USB2_SB_GLD_MSR_CONF, {.hi = 0,.lo = 0x00008f000}},
	{ATA_SB_GLD_MSR_CONF,  {.hi = 0,.lo = 0x00048f000}},
	{AC97_SB_GLD_MSR_CONF, {.hi = 0,.lo = 0x00008f000}},
	{MDD_SB_GLD_MSR_CONF,  {.hi = 0,.lo = 0x00000f000}},
	{0, {0, 0}}
};

/*	5536 Clock Gating*/
static struct msrinit CS5536_CLOCK_GATING_TABLE[] = {
	/* MSR		  Setting*/
	{GLIU_SB_GLD_MSR_PM,  {.hi = 0,.lo = 0x000000004}},
	{GLPCI_SB_GLD_MSR_PM, {.hi = 0,.lo = 0x000000005}},
	{GLCP_SB_GLD_MSR_PM,  {.hi = 0,.lo = 0x000000004}},
	{MDD_SB_GLD_MSR_PM,   {.hi = 0,.lo = 0x050554111}},	/*  SMBus clock gating errata (PBZ 2226 & SiBZ 3977) */
	{ATA_SB_GLD_MSR_PM,   {.hi = 0,.lo = 0x000000005}},
	{AC97_SB_GLD_MSR_PM,  {.hi = 0,.lo = 0x000000005}},
	{0, {0, 0}}
};

struct acpiinit {
	u16 ioreg;
	u32 regdata;
};

static struct acpiinit acpi_init_table[] = {
	{ACPI_IO_BASE + 0x00, 0x01000000},
	{ACPI_IO_BASE + 0x08, 0},
	{ACPI_IO_BASE + 0x0C, 0},
	{ACPI_IO_BASE + 0x1C, 0},
	{ACPI_IO_BASE + 0x18, 0x0FFFFFFFF},
	{ACPI_IO_BASE + 0x00, 0x0000FFFF},
	{PMS_IO_BASE + PM_SCLK, 0x000000E00},
	{PMS_IO_BASE + PM_SED, 0x000004601},
	{PMS_IO_BASE + PM_SIDD, 0x000008C02},
	{PMS_IO_BASE + PM_WKD, 0x0000000A0},
	{PMS_IO_BASE + PM_WKXD, 0x0000000A0},
	{0, 0}
};

struct FLASH_DEVICE {
	unsigned char fType;	/* Flash type: NOR or NAND */
	unsigned char fInterface;	/* Flash interface: I/O or Memory */
	unsigned long fMask;	/* Flash size/mask */
};

static struct FLASH_DEVICE FlashInitTable[] = {
	{FLASH_TYPE_NAND, FLASH_IF_MEM, FLASH_MEM_4K},	/* CS0, or Flash Device 0 */
	{FLASH_TYPE_NONE, 0, 0},	/* CS1, or Flash Device 1 */
	{FLASH_TYPE_NONE, 0, 0},	/* CS2, or Flash Device 2 */
	{FLASH_TYPE_NONE, 0, 0},	/* CS3, or Flash Device 3 */
};

#define FlashInitTableLen (ARRAY_SIZE(FlashInitTable))

static u32 FlashPort[] = {
	MDD_LBAR_FLSH0,
	MDD_LBAR_FLSH1,
	MDD_LBAR_FLSH2,
	MDD_LBAR_FLSH3
};

/* ***************************************************************************/
/* **/
/* *	pmChipsetInit*/
/* **/
/* *	Program ACPI LBAR and initialize ACPI registers.*/
/* **/
/* ***************************************************************************/
static void pmChipsetInit(void)
{
	u32 val = 0;
	u16 port;

	port = (PMS_IO_BASE + 0x010);
	val = 0x0E00;		/*  1ms */
	outl(val, port);

	/*      PM_WKXD */
	/*      Make sure bits[3:0]=0000b to clear the */
	/*      saved Sx state */
	port = (PMS_IO_BASE + 0x034);
	val = 0x0A0;		/*  5ms */
	outl(val, port);

	/*      PM_WKD */
	port = (PMS_IO_BASE + 0x030);
	outl(val, port);

	/*      PM_SED */
	port = (PMS_IO_BASE + 0x014);
	val = 0x04601;		/*  5ms, # of 3.57954MHz clock edges */
	outl(val, port);

	/*      PM_SIDD */
	port = (PMS_IO_BASE + 0x020);
	val = 0x08C02;		/*  10ms, # of 3.57954MHz clock edges */
	outl(val, port);
}

/***************************************************************************
 *
 *	ChipsetFlashSetup
 *
 *	Flash LBARs need to be setup before VSA init so the PCI BARs have
 *	correct size info.	Call this routine only if flash needs to be
 *	configured (don't call it if you want IDE).
 *
 **************************************************************************/
static void ChipsetFlashSetup(void)
{
	msr_t msr;
	int i;
	int numEnabled = 0;

	printk(BIOS_DEBUG, "ChipsetFlashSetup: Start\n");
	for (i = 0; i < FlashInitTableLen; i++) {
		if (FlashInitTable[i].fType != FLASH_TYPE_NONE) {
			printk(BIOS_DEBUG, "Enable CS%d\n", i);
			/* we need to configure the memory/IO mask */
			msr = rdmsr(FlashPort[i]);
			msr.hi = 0;	/* start with the "enabled" bit clear */
			if (FlashInitTable[i].fType == FLASH_TYPE_NAND)
				msr.hi |= 0x00000002;
			else
				msr.hi &= ~0x00000002;
			if (FlashInitTable[i].fInterface == FLASH_IF_MEM)
				msr.hi |= 0x00000004;
			else
				msr.hi &= ~0x00000004;
			msr.hi |= FlashInitTable[i].fMask;
			printk(BIOS_DEBUG, "MSR(0x%08X, %08X_%08X)\n", FlashPort[i],
				     msr.hi, msr.lo);
			wrmsr(FlashPort[i], msr);

			/* now write-enable the device */
			msr = rdmsr(MDD_NORF_CNTRL);
			msr.lo |= (1 << i);
			printk(BIOS_DEBUG, "MSR(0x%08X, %08X_%08X)\n", MDD_NORF_CNTRL,
				     msr.hi, msr.lo);
			wrmsr(MDD_NORF_CNTRL, msr);

			/* update the number enabled */
			numEnabled++;
		}
	}

	printk(BIOS_DEBUG, "ChipsetFlashSetup: Finish\n");

}

/* ***************************************************************************/
/* **/
/* *	enable_ide_nand_flash_header */
/*		Run after VSA init to enable the flash PCI device header */
/* **/
/* ***************************************************************************/
static void enable_ide_nand_flash_header(void)
{
	/* Tell VSA to use FLASH PCI header. Not IDE header. */
	outl(0x80007A40, 0xCF8);
	outl(0xDEADBEEF, 0xCFC);
}

#define RTC_CENTURY 0x32
#define RTC_DOMA	0x3D
#define RTC_MONA	0x3E

static void lpc_init(struct southbridge_amd_cs5536_config *sb)
{
	msr_t msr;

	if (sb->lpc_serirq_enable) {
		msr.lo = sb->lpc_serirq_enable;
		msr.hi = 0;
		wrmsr(MDD_IRQM_LPC, msr);
		if (sb->lpc_serirq_polarity) {
			msr.lo = sb->lpc_serirq_polarity << 16;
			msr.lo |= (sb->lpc_serirq_mode << 6) | (1 << 7);	/* enable */
			msr.hi = 0;
			wrmsr(MDD_LPC_SIRQ, msr);
		}
	}

	/* Allow DMA from LPC */
	msr = rdmsr(MDD_DMA_MAP);
	msr.lo = 0x7777;
	wrmsr(MDD_DMA_MAP, msr);

	/* enable the RTC/CMOS century byte at address 32h */
	msr = rdmsr(MDD_RTC_CENTURY_OFFSET);
	msr.lo = RTC_CENTURY;
	wrmsr(MDD_RTC_CENTURY_OFFSET, msr);

	/* enable the RTC/CMOS day of month and month alarms */
	msr = rdmsr(MDD_RTC_DOMA_IND);
	msr.lo = RTC_DOMA;
	wrmsr(MDD_RTC_DOMA_IND, msr);

	msr = rdmsr(MDD_RTC_MONA_IND);
	msr.lo = RTC_MONA;
	wrmsr(MDD_RTC_MONA_IND, msr);

	cmos_init(0);

	isa_dma_init();
}


/**
 * Depending on settings in the config struct, enable COM1 or COM2 or both.
 *
 * If the enable is NOT set, the UARTs are explicitly disabled, which is
 * required if (e.g.) there is a Super I/O attached that does COM1 or COM2.
 *
 * @param sb Southbridge config structure.
 */
static void uarts_init(struct southbridge_amd_cs5536_config *sb)
{
	msr_t msr;
	u16 addr = 0;
	u32 gpio_addr;
	struct device *dev;

	dev = dev_find_device(PCI_VENDOR_ID_AMD,
			PCI_DEVICE_ID_AMD_CS5536_ISA, 0);
	gpio_addr = pci_read_config32(dev, PCI_BASE_ADDRESS_1);
	gpio_addr &= ~1;	/* Clear I/O bit */
	printk(BIOS_DEBUG, "GPIO_ADDR: %08X\n", gpio_addr);

	/* This could be extended to support IR modes. */

	/* COM1 */
	if (sb->com1_enable) {
		printk(BIOS_SPEW, "uarts_init: enable COM1\n");
		/* Set the address. */
		switch (sb->com1_address) {
		case 0x3F8:
			addr = 7;
			break;
		case 0x3E8:
			addr = 6;
			break;
		case 0x2F8:
			addr = 5;
			break;
		case 0x2E8:
			addr = 4;
			break;
		}
		msr = rdmsr(MDD_LEG_IO);
		msr.lo |= addr << 16;
		wrmsr(MDD_LEG_IO, msr);

		/* Set the IRQ. */
		msr = rdmsr(MDD_IRQM_YHIGH);
		msr.lo |= sb->com1_irq << 24;
		wrmsr(MDD_IRQM_YHIGH, msr);

		/* GPIO8 - UART1_TX */
		/* Set: Output Enable (0x4) */
		outl(GPIOL_8_SET, gpio_addr + GPIOL_OUTPUT_ENABLE);
		/* Set: OUTAUX1 Select (0x10) */
		outl(GPIOL_8_SET, gpio_addr + GPIOL_OUT_AUX1_SELECT);

		/* GPIO9 - UART1_RX */
		/* Set: Input Enable   (0x20) */
		outl(GPIOL_9_SET, gpio_addr + GPIOL_INPUT_ENABLE);
		/* Set: INAUX1 Select (0x34) */
		outl(GPIOL_9_SET, gpio_addr + GPIOL_IN_AUX1_SELECT);

		/* Set: GPIO 8 + 9 Pull Up (0x18) */
		outl(GPIOL_8_SET | GPIOL_9_SET,
		     gpio_addr + GPIOL_PULLUP_ENABLE);

		/* Enable COM1.
		 *
		 * Bit 1 = device enable
		 * Bit 4 = allow access to the upper banks
		 */
		msr.lo = (1 << 4) | (1 << 1);
		msr.hi = 0;
		wrmsr(MDD_UART1_CONF, msr);
	} else {
		/* Reset and disable COM1. */
		printk(BIOS_SPEW, "uarts_init: disable COM1\n");
		msr = rdmsr(MDD_UART1_CONF);
		msr.lo = 1;			/* Reset */
		wrmsr(MDD_UART1_CONF, msr);
		msr.lo = 0;			/* Disabled */
		wrmsr(MDD_UART1_CONF, msr);

		/* Disable the IRQ. */
		msr = rdmsr(MDD_LEG_IO);
		msr.lo &= ~(0xF << 16);
		wrmsr(MDD_LEG_IO, msr);
	}

	/* COM2 */
	if (sb->com2_enable) {
		printk(BIOS_SPEW, "uarts_init: enable COM2\n");
		switch (sb->com2_address) {
		case 0x3F8:
			addr = 7;
			break;
		case 0x3E8:
			addr = 6;
			break;
		case 0x2F8:
			addr = 5;
			break;
		case 0x2E8:
			addr = 4;
			break;
		}
		msr = rdmsr(MDD_LEG_IO);
		msr.lo |= addr << 20;
		wrmsr(MDD_LEG_IO, msr);
		printk(BIOS_SPEW, "uarts_init: wrote COM2 address 0x%x\n", sb->com2_address);

		/* Set the IRQ. */
		msr = rdmsr(MDD_IRQM_YHIGH);
		msr.lo |= sb->com2_irq << 28;
		wrmsr(MDD_IRQM_YHIGH, msr);
		printk(BIOS_SPEW, "uarts_init: set COM2 irq\n");

		/* GPIO3 - UART2_RX */
		/* Set: Input Enable (0x20) */
		outl(GPIOL_3_SET, gpio_addr + GPIOL_INPUT_ENABLE);
		/* Set: INAUX1 Select (0x34) */
		outl(GPIOL_3_SET, gpio_addr + GPIOL_IN_AUX1_SELECT);

		/* GPIO4 - UART2_TX */
		/* Set: Output Enable (0x4) */
		outl(GPIOL_4_SET, gpio_addr + GPIOL_OUTPUT_ENABLE);
		printk(BIOS_SPEW, "uarts_init: set output enable\n");
		/* Set: OUTAUX1 Select (0x10) */
		outl(GPIOL_4_SET, gpio_addr + GPIOL_OUT_AUX1_SELECT);
		printk(BIOS_SPEW, "uarts_init: set OUTAUX1\n");

		/* Set: GPIO 3 + 4 Pull Up (0x18) */
		outl(GPIOL_3_SET | GPIOL_4_SET,
		     gpio_addr + GPIOL_PULLUP_ENABLE);
		printk(BIOS_SPEW, "uarts_init: set pullup COM2\n");

		/* Enable COM2.
		 *
		 * Bit 1 = device enable
		 * Bit 4 = allow access to the upper banks
		 */
		msr.lo = (1 << 4) | (1 << 1);
		msr.hi = 0;
		wrmsr(MDD_UART2_CONF, msr);
		printk(BIOS_SPEW, "uarts_init: COM2 enabled\n");
	} else {
		printk(BIOS_SPEW, "uarts_init: disable COM2\n");
		/* Reset and disable COM2. */
		msr = rdmsr(MDD_UART2_CONF);
		msr.lo = 1;			/* Reset */
		wrmsr(MDD_UART2_CONF, msr);
		msr.lo = 0;			/* Disabled */
		wrmsr(MDD_UART2_CONF, msr);

		/* Disable the IRQ. */
		msr = rdmsr(MDD_LEG_IO);
		msr.lo &= ~(0xF << 20);
		wrmsr(MDD_LEG_IO, msr);
	}
}


#define HCCPARAMS		0x08
#define IPREG04			0xA0
	#define USB_HCCPW_SET	(1 << 1)
#define UOCCAP			0x00
	#define APU_SET			(1 << 15)
#define UOCMUX			0x04
#define PMUX_HOST		0x02
#define PMUX_DEVICE		0x03
	#define PUEN_SET		(1 << 2)
#define UDCDEVCTL		0x404
	#define UDC_SD_SET		(1 << 10)
#define UOCCTL			0x0C
	#define PADEN_SET		(1 << 7)

static void enable_USB_port4(struct southbridge_amd_cs5536_config *sb)
{
	void *bar;
	msr_t msr;
	struct device *dev;

	dev = dev_find_device(PCI_VENDOR_ID_AMD,
			PCI_DEVICE_ID_AMD_CS5536_EHCI, 0);
	if (dev) {

		/* Serial Short Detect Enable */
		msr = rdmsr(USB2_SB_GLD_MSR_CONF);
		msr.hi |= USB2_UPPER_SSDEN_SET;
		wrmsr(USB2_SB_GLD_MSR_CONF, msr);

		/* write to clear diag register */
		wrmsr(USB2_SB_GLD_MSR_DIAG, rdmsr(USB2_SB_GLD_MSR_DIAG));

		bar = (void *)pci_read_config32(dev, PCI_BASE_ADDRESS_0);

		/* Make HCCPARAMS writable */
		write32(bar + IPREG04, read32(bar + IPREG04) | USB_HCCPW_SET);

		/* ; EECP=50h, IST=01h, ASPC=1 */
		write32(bar + HCCPARAMS, 0x00005012);
	}

	dev = dev_find_device(PCI_VENDOR_ID_AMD,
			PCI_DEVICE_ID_AMD_CS5536_OTG, 0);
	if (dev) {
		bar = (void *)pci_read_config32(dev, PCI_BASE_ADDRESS_0);

		write32(bar + UOCMUX, read32(bar + UOCMUX) & PUEN_SET);

		/* Host or Device? */
		if (sb->enable_USBP4_device) {
			write32(bar + UOCMUX, read32(bar + UOCMUX) | PMUX_DEVICE);
		} else {
			write32(bar + UOCMUX, read32(bar + UOCMUX) | PMUX_HOST);
		}

		/* Overcurrent configuration */
		if (sb->enable_USBP4_overcurrent) {
			write32(bar + UOCCAP, read32(bar + UOCCAP)
			       | sb->enable_USBP4_overcurrent);
		}
	}

	/* PBz#6466: If the UOC(OTG) device, port 4, is configured as a device,
	 *      then perform the following sequence:
	 *
	 * - set SD bit in DEVCTRL udc register
	 * - set PADEN (former OTGPADEN) bit in uoc register
	 * - set APU bit in uoc register */
	if (sb->enable_USBP4_device) {
		dev = dev_find_device(PCI_VENDOR_ID_AMD,
				PCI_DEVICE_ID_AMD_CS5536_UDC, 0);
		if (dev) {
			bar = (void *)pci_read_config32(dev,
							PCI_BASE_ADDRESS_0);
			write32(bar + UDCDEVCTL,
			       read32(bar + UDCDEVCTL) | UDC_SD_SET);

		}

		dev = dev_find_device(PCI_VENDOR_ID_AMD,
				PCI_DEVICE_ID_AMD_CS5536_OTG, 0);
		if (dev) {
			bar = (void *)pci_read_config32(dev,
							PCI_BASE_ADDRESS_0);
			write32(bar + UOCCTL, read32(bar + UOCCTL) | PADEN_SET);
			write32(bar + UOCCAP, read32(bar + UOCCAP) | APU_SET);
		}
	}

	/* Disable virtual PCI UDC and OTG headers */
	dev = dev_find_device(PCI_VENDOR_ID_AMD,
			PCI_DEVICE_ID_AMD_CS5536_UDC, 0);
	if (dev) {
		pci_write_config32(dev, 0x7C, 0xDEADBEEF);
	}

	dev = dev_find_device(PCI_VENDOR_ID_AMD,
			PCI_DEVICE_ID_AMD_CS5536_OTG, 0);
	if (dev) {
		pci_write_config32(dev, 0x7C, 0xDEADBEEF);
	}
}

/****************************************************************************
 *
 *	ChipsetInit
 *
 *	Called from northbridge init (Pre-VSA).
 *
 ****************************************************************************/
void chipsetinit(void)
{
	struct device *dev;
	msr_t msr;
	u32 msrnum;
	struct southbridge_amd_cs5536_config *sb;
	struct msrinit *csi;

	dev = dev_find_slot(0, PCI_DEVFN(0xf, 0));

	if (!dev) {
		printk(BIOS_ERR, "CS5536 not found.\n");
		return;
	}

	sb = (struct southbridge_amd_cs5536_config *)dev->chip_info;

	if (!sb) {
		printk(BIOS_ERR, "CS5536 configuration not found.\n");
		return;
	}

	post_code(P80_CHIPSET_INIT);

	/* we hope NEVER to be in coreboot when S3 resumes
	   if (! IsS3Resume()) */
	{
		struct acpiinit *aci = acpi_init_table;
		for (; aci->ioreg; aci++) {
			outl(aci->regdata, aci->ioreg);
			inl(aci->ioreg);
		}

		pmChipsetInit();
	}

	/* set hd IRQ */
	outl(GPIOL_2_SET, GPIO_IO_BASE + GPIOL_INPUT_ENABLE);
	outl(GPIOL_2_SET, GPIO_IO_BASE + GPIOL_IN_AUX1_SELECT);

	/*      Allow IO read and writes during a ATA DMA operation. */
	/*       This could be done in the HD ROM but do it here for easier debugging. */
	msrnum = ATA_SB_GLD_MSR_ERR;
	msr = rdmsr(msrnum);
	msr.lo &= ~0x100;
	wrmsr(msrnum, msr);

	/*      Enable Post Primary IDE. */
	msrnum = GLPCI_SB_CTRL;
	msr = rdmsr(msrnum);
	msr.lo |= GLPCI_CRTL_PPIDE_SET;
	wrmsr(msrnum, msr);

	csi = SB_MASTER_CONF_TABLE;
	for (; csi->msrnum; csi++) {
		msr.lo = csi->msr.lo;
		msr.hi = csi->msr.hi;
		wrmsr(csi->msrnum, msr);	// MSR - see table above
	}

	/*      Flash BAR size Setup */
	printk(BIOS_INFO, "%sDoing ChipsetFlashSetup()\n",
		   sb->enable_ide_nand_flash == 1 ? "" : "Not ");
	if (sb->enable_ide_nand_flash == 1)
		ChipsetFlashSetup();

	/* */
	/*      Set up Hardware Clock Gating */
	/* */
	{
		csi = CS5536_CLOCK_GATING_TABLE;
		for (; csi->msrnum; csi++) {
			msr.lo = csi->msr.lo;
			msr.hi = csi->msr.hi;
			wrmsr(csi->msrnum, msr);	// MSR - see table above
		}
	}
}

static void southbridge_init(struct device *dev)
{
	struct southbridge_amd_cs5536_config *sb =
	    (struct southbridge_amd_cs5536_config *)dev->chip_info;
	int i;
	/*
	 * struct device *gpiodev;
	 * unsigned short gpiobase = MDD_GPIO;
	 */

	printk(BIOS_INFO, "cs5536: %s\n", __func__);

	if (!sb) {
		printk(BIOS_ERR, "CS5536 configuration not found.\n");
		return;
	}

	setup_i8259();
	lpc_init(sb);
	uarts_init(sb);

	if (sb->enable_gpio_int_route) {
		vrWrite((VRC_MISCELLANEOUS << 8) + PCI_INT_AB,
			(sb->enable_gpio_int_route & 0xFFFF));
		vrWrite((VRC_MISCELLANEOUS << 8) + PCI_INT_CD,
			(sb->enable_gpio_int_route >> 16));
	}

	printk(BIOS_DEBUG, "cs5536: %s: enable_ide_nand_flash is %d\n", __func__,
		   sb->enable_ide_nand_flash);
	if (sb->enable_ide_nand_flash == 1) {
		enable_ide_nand_flash_header();
	}

	enable_USB_port4(sb);

	/* disable unwanted virtual PCI devices */
	for (i = 0; (i < MAX_UNWANTED_VPCI) && (0 != sb->unwanted_vpci[i]); i++) {
		printk(BIOS_DEBUG, "Disabling VPCI device: 0x%08X\n",
			     sb->unwanted_vpci[i]);
		outl(sb->unwanted_vpci[i] + 0x7C, 0xCF8);
		outl(0xDEADBEEF, 0xCFC);
	}
}

static void cs5536_read_resources(struct device *dev)
{
	struct resource *res;

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x1000UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void southbridge_enable(struct device *dev)
{
	printk(BIOS_DEBUG, "cs5536: %s: dev is %p\n", __func__, dev);

}

static int lsmbus_read_byte(struct device *dev, u8 address)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, 0x10);

	return do_smbus_read_byte(res->base, device, address);
}

static struct smbus_bus_operations lops_smbus_bus = {
	.read_byte  = lsmbus_read_byte,
};

static void scan_lpc_smbus(struct device *dev)
{
	/* FIXME. Do we have mixed LPC/SMBus device node here. */
	scan_smbus(dev);
}

static struct device_operations southbridge_ops = {
	.read_resources = cs5536_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = southbridge_init,
	.scan_bus = scan_lpc_smbus,
	.ops_smbus_bus = &lops_smbus_bus,
};

static const struct pci_driver cs5536_pci_driver __pci_driver = {
	.ops = &southbridge_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_CS5536_ISA
};

struct chip_operations southbridge_amd_cs5536_ops = {
	CHIP_NAME("AMD Geode CS5536 Southbridge")
	    /* This is only called when this device is listed in the
	     * static device tree.
	     */
	    .enable_dev = southbridge_enable,
};
