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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <amd_geodelx.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include "cs5536.h"

/* Master configuration register for bus masters */
static const struct msrinit SB_MASTER_CONF_TABLE[] = {
	{USB2_SB_GLD_MSR_CONF, {.hi = 0,.lo = 0x00008f000}},
	{ATA_SB_GLD_MSR_CONF,  {.hi = 0,.lo = 0x00048f000}},
	{AC97_SB_GLD_MSR_CONF, {.hi = 0,.lo = 0x00008f000}},
	{MDD_SB_GLD_MSR_CONF,  {.hi = 0,.lo = 0x00000f000}},
	{0, {0, 0}}
};

/* CS5536 clock gating */
static const struct msrinit CS5536_CLOCK_GATING_TABLE[] = {
	{GLIU_SB_GLD_MSR_PM,  {.hi = 0,.lo = 0x000000004}},
	{GLPCI_SB_GLD_MSR_PM, {.hi = 0,.lo = 0x000000005}},
	{GLCP_SB_GLD_MSR_PM,  {.hi = 0,.lo = 0x000000004}},
	{MDD_SB_GLD_MSR_PM,   {.hi = 0,.lo = 0x050554111}},	/* SMBus clock gating errata (PBZ 2226 & SiBZ 3977) */
	{ATA_SB_GLD_MSR_PM,   {.hi = 0,.lo = 0x000000005}},
	{AC97_SB_GLD_MSR_PM,  {.hi = 0,.lo = 0x000000005}},
	{0, {0, 0}}
};

struct acpi_init {
	u16 ioreg;
	u32 regdata;
};

static const struct acpi_init acpi_init_table[] = {
	{ACPI_IO_BASE + 0x00,   0x01000000},
	{ACPI_IO_BASE + 0x08,   0x00000000},
	{ACPI_IO_BASE + 0x0C,   0x00000000},
	{ACPI_IO_BASE + 0x1C,   0x00000000},
	{ACPI_IO_BASE + 0x18,   0xFFFFFFFF},
	{ACPI_IO_BASE + 0x00,   0x0000FFFF},
	{PMS_IO_BASE + PM_SCLK, 0x00000E00},
	{PMS_IO_BASE + PM_SED,  0x00004601},
	{PMS_IO_BASE + PM_SIDD, 0x00008C02},
	{PMS_IO_BASE + PM_WKD,  0x000000A0},
	{PMS_IO_BASE + PM_WKXD, 0x000000A0},
	{0, 0}
};

struct FLASH_DEVICE {
	unsigned char fType;		/* Flash type: NOR or NAND */
	unsigned char fInterface;	/* Flash interface: I/O or memory */
	unsigned long fMask;		/* Flash size/mask */
};

static const struct FLASH_DEVICE FlashInitTable[] = {
	{FLASH_TYPE_NAND, FLASH_IF_MEM, FLASH_MEM_4K},	/* CS0, or Flash Device 0 */
	{FLASH_TYPE_NONE, 0, 0},	/* CS1, or Flash Device 1 */
	{FLASH_TYPE_NONE, 0, 0},	/* CS2, or Flash Device 2 */
	{FLASH_TYPE_NONE, 0, 0},	/* CS3, or Flash Device 3 */
};

static const u32 FlashPort[] = {
	MDD_LBAR_FLSH0,
	MDD_LBAR_FLSH1,
	MDD_LBAR_FLSH2,
	MDD_LBAR_FLSH3
};

/**
 * Power button setup.
 *
 * Setup GPIO24, it is the external signal for CS5536 vsb_work_aux which
 * controls all voltage rails except Vstandby & Vmem. We need to enable
 * OUT_AUX1 and OUTPUT_ENABLE in this order.
 *
 * @param sb The southbridge config structure. 
 * If GPIO24 is not enabled then soft-off will not work.
 */
static void cs5536_setup_power_button(struct southbridge_amd_cs5536_dts_config *sb )
{
	if (!sb->power_button)
		return;
	/* TODO: Should be a #define? */
	outl(0x40020000, PMS_IO_BASE + 0x40);
	outl(GPIOH_24_SET, GPIO_IO_BASE + GPIOH_OUT_AUX1_SELECT);
	outl(GPIOH_24_SET, GPIO_IO_BASE + GPIOH_OUTPUT_ENABLE);
}

/**
 * Program ACPI LBAR and initialize ACPI registers.
 */
static void pm_chipset_init(void)
{
	outl(0x0E00, PMS_IO_BASE + 0x010);	/* 1ms */

	/* Make sure bits[3:0]=0000b to clear the saved Sx state. */
	outl(0x00A0, PMS_IO_BASE + PM_WKXD);	/* 5ms */

	outl(0x00A0, PMS_IO_BASE + PM_WKD);

	/* 5ms, # of 3.57954MHz clock edges */
	outl(0x4601, PMS_IO_BASE + PM_SED);

	/* 10ms, # of 3.57954MHz clock edges */
	outl(0x8C02, PMS_IO_BASE + PM_SIDD);
}

/**
 * Flash LBARs need to be setup before VSA init so the PCI BARs have
 * correct size info. Call this routine only if flash needs to be
 * configured (don't call it if you want IDE).
 */
static void chipset_flash_setup(void)
{
	int i;
	struct msr msr;

	printk(BIOS_DEBUG, "chipset_flash_setup: Start\n");
	for (i = 0; i < ARRAY_SIZE(FlashInitTable); i++) {
		if (FlashInitTable[i].fType != FLASH_TYPE_NONE) {
			printk(BIOS_DEBUG, "Enable CS%d\n", i);
			/* We need to configure the memory/IO mask. */
			msr = rdmsr(FlashPort[i]);
			msr.hi = 0;	/* Start with "enabled" bit clear. */
			if (FlashInitTable[i].fType == FLASH_TYPE_NAND)
				msr.hi |= 0x00000002;
			else
				msr.hi &= ~0x00000002;
			if (FlashInitTable[i].fInterface == FLASH_IF_MEM)
				msr.hi |= 0x00000004;
			else
				msr.hi &= ~0x00000004;
			msr.hi |= FlashInitTable[i].fMask;
			printk(BIOS_DEBUG, "MSR(0x%08X, %08X_%08X)\n",
			       FlashPort[i], msr.hi, msr.lo);
			wrmsr(FlashPort[i], msr);

			/* Now write-enable the device. */
			msr = rdmsr(MDD_NORF_CNTRL);
			msr.lo |= (1 << i);
			printk(BIOS_DEBUG, "MSR(0x%08X, %08X_%08X)\n",
			       MDD_NORF_CNTRL, msr.hi, msr.lo);
			wrmsr(MDD_NORF_CNTRL, msr);
		}
	}
	printk(BIOS_DEBUG, "chipset_flash_setup: Finish\n");
}

/**
 * Use this in the event that you have a FLASH part instead of an IDE drive.
 * Run after VSA init to enable the flash PCI device header.
 */
static void enable_ide_nand_flash_header(void)
{
	/* Tell VSA to use FLASH PCI header. Not IDE header. */
	outl(0x80007A40, 0xCF8);
	outl(0xDEADBEEF, 0xCFC);
}

#define RTC_CENTURY	0x32
#define RTC_DOMA	0x3D
#define RTC_MONA	0x3E

/**
 * Standard init function for the LPC bus.
 *
 * Sets up the "serial irq" interrupt, which is NOT the same as serial
 * interrupt, and also enables DMA from the LPC bus. Configures the PC clock,
 * enables RTC and ISA DMA.
 *
 * @param sb Southbridge config structure.
 */
static void lpc_init(struct southbridge_amd_cs5536_dts_config *sb)
{
	struct msr msr;

	if (sb->lpc_serirq_enable) {
		msr.lo = sb->lpc_serirq_enable;
		msr.hi = 0;
		wrmsr(MDD_IRQM_LPC, msr);
		if (sb->lpc_serirq_polarity) {
			msr.lo = sb->lpc_serirq_polarity << 16;
			msr.lo |= (sb->lpc_serirq_mode << 6) | (1 << 7);	/* Enable */
			msr.hi = 0;
			wrmsr(MDD_LPC_SIRQ, msr);
		}
	}

	/* Allow DMA from LPC. */
	msr = rdmsr(MDD_DMA_MAP);
	msr.lo = 0x7777;
	wrmsr(MDD_DMA_MAP, msr);

	/* Enable the RTC/CMOS century byte at address 0x32. */
	msr = rdmsr(MDD_RTC_CENTURY_OFFSET);
	msr.lo = RTC_CENTURY;
	wrmsr(MDD_RTC_CENTURY_OFFSET, msr);

	/* Enable the RTC/CMOS day of month and month alarms. */
	msr = rdmsr(MDD_RTC_DOMA_IND);
	msr.lo = RTC_DOMA;
	wrmsr(MDD_RTC_DOMA_IND, msr);

	msr = rdmsr(MDD_RTC_MONA_IND);
	msr.lo = RTC_MONA;
	wrmsr(MDD_RTC_MONA_IND, msr);

	rtc_init(0);

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
static void uarts_init(struct southbridge_amd_cs5536_dts_config *sb)
{
	struct msr msr;
	u16 addr = 0;
	u32 gpio_addr;
	struct device *dev;

	dev = dev_find_pci_device(PCI_VENDOR_ID_AMD,
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

		/* GPIO8 - UART1_RX */
		/* Set: Input Enable (0x20) */
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

		/* Set the IRQ. */
		msr = rdmsr(MDD_IRQM_YHIGH);
		msr.lo |= sb->com2_irq << 28;
		wrmsr(MDD_IRQM_YHIGH, msr);

		/* GPIO3 - UART2_RX */
		/* Set: Output Enable (0x4) */
		outl(GPIOL_3_SET, gpio_addr + GPIOL_OUTPUT_ENABLE);

		/* Set: OUTAUX1 Select (0x10) */
		outl(GPIOL_3_SET, gpio_addr + GPIOL_OUT_AUX1_SELECT);

		/* GPIO4 - UART2_TX */
		/* Set: Input Enable (0x20) */
		outl(GPIOL_4_SET, gpio_addr + GPIOL_INPUT_ENABLE);

		/* Set: INAUX1 Select (0x34) */
		/* this totally disables com2 for serial, leave it out until we can
		 * figure it out
		 */
//		outl(GPIOL_4_SET, gpio_addr + GPIOL_IN_AUX2_SELECT);
//		printk(BIOS_SPEW, "uarts_init: set INAUX2 for COM2\n");

		/* Set: GPIO 3 + 3 Pull Up (0x18) */
		outl(GPIOL_3_SET | GPIOL_4_SET,
		     gpio_addr + GPIOL_PULLUP_ENABLE);

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
#define USB_HCCPW_SET		(1 << 1)
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

/**
 * Depending on settings in the config struct, manage USB setup.
 *
 * @param sb Southbridge config structure.
 */
static void enable_USB_port4(struct southbridge_amd_cs5536_dts_config *sb)
{
	u32 *bar;
	struct msr msr;
	struct device *dev;

	dev = dev_find_pci_device(PCI_VENDOR_ID_AMD,
			      PCI_DEVICE_ID_AMD_CS5536_EHCI, 0);
	if (dev) {
		/* Serial short detect enable */
		msr = rdmsr(USB2_SB_GLD_MSR_CONF);
		msr.hi |= USB2_UPPER_SSDEN_SET;
		wrmsr(USB2_SB_GLD_MSR_CONF, msr);

		/* Write to clear diag register. */
		wrmsr(USB2_SB_GLD_MSR_DIAG, rdmsr(USB2_SB_GLD_MSR_DIAG));

		bar = (u32 *) pci_read_config32(dev, PCI_BASE_ADDRESS_0);

		/* Make HCCPARAMS writable. */
		*(bar + IPREG04) |= USB_HCCPW_SET;

		/* EECP=50h, IST=01h, ASPC=1 */
		*(bar + HCCPARAMS) = 0x00005012;
	}

	dev = dev_find_pci_device(PCI_VENDOR_ID_AMD,
			      PCI_DEVICE_ID_AMD_CS5536_OTG, 0);
	if (dev) {
		bar = (u32 *) pci_read_config32(dev, PCI_BASE_ADDRESS_0);

		*(bar + UOCMUX) &= PUEN_SET;

		/* Host or Device? */
		if (sb->enable_USBP4_device)
			*(bar + UOCMUX) |= PMUX_DEVICE;
		else
			*(bar + UOCMUX) |= PMUX_HOST;

		/* Overcurrent configuration */
		printk(BIOS_DEBUG, "UOCCAP is %x\n", *(bar + UOCCAP));
		if (sb->enable_USBP4_overcurrent)
			*(bar + UOCCAP) |= sb->enable_USBP4_overcurrent;
		/* power control. see comment in the dts for these bits */
		if (sb->pph) {
			*(bar + UOCCAP) &= ~0xff;
			*(bar + UOCCAP) |= sb->pph;
		}
		printk(BIOS_DEBUG, "UOCCAP is %x\n", *(bar + UOCCAP));

	}

	/* PBz#6466: If the UOC(OTG) device, port 4, is configured as a
	 * device, then perform the following sequence:
	 *  - Set SD bit in DEVCTRL udc register
	 *  - Set PADEN (former OTGPADEN) bit in uoc register
	 *  - Set APU bit in uoc register
	 */
	if (sb->enable_USBP4_device) {
		dev = dev_find_pci_device(PCI_VENDOR_ID_AMD,
				      PCI_DEVICE_ID_AMD_CS5536_UDC, 0);
		if (dev) {
			bar = (u32 *)pci_read_config32(dev, PCI_BASE_ADDRESS_0);
			*(bar + UDCDEVCTL) |= UDC_SD_SET;
		}

		dev = dev_find_pci_device(PCI_VENDOR_ID_AMD,
				      PCI_DEVICE_ID_AMD_CS5536_OTG, 0);
		if (dev) {
			bar = (u32 *)pci_read_config32(dev, PCI_BASE_ADDRESS_0);
			*(bar + UOCCTL) |= PADEN_SET;
			*(bar + UOCCAP) |= APU_SET;
		}
	}

	/* Disable virtual PCI UDC and OTG headers.  The kernel never
	 * sees a header for this device.  It used to provide an OS
	 * visible device, but that was defeatured.  There are still
	 * some registers in the block that are useful for the firmware
	 * to setup, but nothing that a kernel level driver would need
	 * to consume.
	 *
	 * As you can see above, VSA does provide the header under
	 * device ID PCI_DEVICE_ID_AMD_CS5536_OTG, but it is hidden
	 * when 0xDEADBEEF is written to config space register 0x7C.
	 */
	dev = dev_find_pci_device(PCI_VENDOR_ID_AMD,
			      PCI_DEVICE_ID_AMD_CS5536_UDC, 0);
	if (dev)
		pci_write_config32(dev, 0x7C, 0xDEADBEEF);

	dev = dev_find_pci_device(PCI_VENDOR_ID_AMD,
			      PCI_DEVICE_ID_AMD_CS5536_OTG, 0);
	if (dev)
		pci_write_config32(dev, 0x7C, 0xDEADBEEF);
}

/** 
 * This function initializes a lot of nasty bits needed for phase 2.
 *
 * Can this function run before vsm is set up, or is it required for vsm?
 * The order here is a little hard to figure out.
 *
 * This function is in an odd place. We need to see about moving it to
 * geodelx.c. But for now, let's get things working and put a #warning in.
 */
void chipsetinit(void)
{
	struct device *dev;
	struct msr msr;
	struct southbridge_amd_cs5536_dts_config *sb;
	const struct msrinit *csi;

	post_code(P80_CHIPSET_INIT);
	dev = dev_find_pci_device(PCI_VENDOR_ID_AMD,
			      PCI_DEVICE_ID_AMD_CS5536_ISA, 0);
	if (!dev) {
		printk(BIOS_ERR, "%s: Could not find the south bridge!\n",
		       __FUNCTION__);
		return;
	}
	sb = (struct southbridge_amd_cs5536_dts_config *)dev->device_configuration;

#ifdef CONFIG_SUSPEND_TO_RAM
	if (!IsS3Resume())
#endif
	{
		struct acpi_init *aci = acpi_init_table;
		for (; aci->ioreg; aci++) {
			outl(aci->regdata, aci->ioreg);
			inl(aci->ioreg);
		}
		pm_chipset_init();
	}

	/* Set HD IRQ. */
	outl(GPIOL_2_SET, GPIO_IO_BASE + GPIOL_INPUT_ENABLE);
	outl(GPIOL_2_SET, GPIO_IO_BASE + GPIOL_IN_AUX1_SELECT);

	/* Allow I/O reads and writes during a ATA DMA operation. This could
	 * be done in the HD ROM but do it here for easier debugging.
	 */
	msr = rdmsr(ATA_SB_GLD_MSR_ERR);
	msr.lo &= ~0x100;
	wrmsr(ATA_SB_GLD_MSR_ERR, msr);

	/* Enable post primary IDE. */
	msr = rdmsr(GLPCI_SB_CTRL);
	msr.lo |= GLPCI_CRTL_PPIDE_SET;
	wrmsr(GLPCI_SB_CTRL, msr);

	csi = SB_MASTER_CONF_TABLE;
	for (; csi->msrnum; csi++) {
		msr.lo = csi->msr.lo;
		msr.hi = csi->msr.hi;
		wrmsr(csi->msrnum, msr);
	}

	/* Flash BAR size setup. */
	printk(BIOS_ERR, "%sDoing chipset_flash_setup()\n",
	       sb->enable_ide_nand_flash == 1 ? "" : "Not ");
	if (sb->enable_ide_nand_flash == 1)
		chipset_flash_setup();

	/* Set up hardware clock gating. */
	/* TODO: Why the extra block here? Can it be removed? */
	{
		csi = CS5536_CLOCK_GATING_TABLE;
		for (; csi->msrnum; csi++) {
			msr.lo = csi->msr.lo;
			msr.hi = csi->msr.hi;
			wrmsr(csi->msrnum, msr);
		}
	}
}

#define IDE_CFG	  0x40
	#define CHANEN  (1L <<  1)
	#define PWB	(1L << 14)
	#define CABLE	(1L << 16)
#define IDE_DTC	  0x48
#define IDE_CAST  0x4C
#define IDE_ETC	  0x50

/**
 * Enabled the IDE. This is code that is optionally run if the ide_enable is set
 * in the mainboard dts. 
 * 
 * @param dev The device 
 */
static void ide_init(struct device *dev)
{
	u32 ide_cfg;

	struct southbridge_amd_cs5536_ide_config *ide =
	    (struct southbridge_amd_cs5536_ide_config *)dev->device_configuration;
	if (!ide->enable_ide)
		return;

	printk(BIOS_DEBUG, "cs5536_ide: %s\n", __func__);
	/* GPIO and IRQ setup are handled in the main chipset code. */

	// Enable the channel and Post Write Buffer
	// NOTE: Only 32-bit writes to the data buffer are allowed when PWB is set
	ide_cfg = pci_read_config32(dev, IDE_CFG);
	ide_cfg |= CHANEN | PWB;
	pci_write_config32(dev, IDE_CFG, ide_cfg);
}


static void hide_vpci(u32 vpci_devid)
{
	/* Hide unwanted virtual PCI device. */
	printk(BIOS_DEBUG, "Hiding VPCI device: 0x%08X\n",
		vpci_devid);
	outl(vpci_devid + 0x7C, 0xCF8);
	outl(0xDEADBEEF, 0xCFC);
}

/**
 * TODO.
 *
 * @param dev The device to use.
 */
static void southbridge_init(struct device *dev)
{
	int i;
	struct southbridge_amd_cs5536_dts_config *sb =
	    (struct southbridge_amd_cs5536_dts_config *)dev->device_configuration;

	/*
	 * struct device *gpiodev;
	 * unsigned short gpiobase = MDD_GPIO;
	 */

	printk(BIOS_ERR, "cs5536: %s\n", __FUNCTION__);

	setup_i8259();
	lpc_init(sb);
	uarts_init(sb);

	if (sb->enable_gpio_int_route) {
		printk(BIOS_SPEW, "cs5536: call vr_write\n");
		vr_write((VRC_MISCELLANEOUS << 8) + PCI_INT_AB,
			 (sb->enable_gpio_int_route & 0xFFFF));
		printk(BIOS_SPEW, "cs5536: done first call vr_write\n");
		vr_write((VRC_MISCELLANEOUS << 8) + PCI_INT_CD,
			 (sb->enable_gpio_int_route >> 16));
		printk(BIOS_SPEW, "cs5536: done second call vr_write\n");
	}

	printk(BIOS_ERR, "cs5536: %s: enable_ide_nand_flash is %d\n",
	       __FUNCTION__, sb->enable_ide_nand_flash);
	if (sb->enable_ide_nand_flash == 1)
		enable_ide_nand_flash_header();

	enable_USB_port4(sb);

	/* disable unwanted virtual PCI devices */
	for (i = 0; 0 != sb->unwanted_vpci[i]; i++) {
		hide_vpci(sb->unwanted_vpci[i]);
	}

	cs5536_setup_power_button(sb);

	printk(BIOS_SPEW, "cs5536: %s() Exit\n", __FUNCTION__);
}

/**
 * A slightly different enable resources than the standard.
 * We grab control here as VSA has played in this chip as well.
 *
 * @param dev The device to use.
 */
static void cs5536_pci_dev_enable_resources(struct device *dev)
{
	printk(BIOS_SPEW, "cs5536: %s()\n", __FUNCTION__);
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
	printk(BIOS_SPEW, "cs5536: %s() Exit\n", __FUNCTION__);
}

struct device_operations cs5536_ops = {
	.id = {.type = DEVICE_ID_PCI,
		.u = {.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = PCI_DEVICE_ID_AMD_CS5536_ISA}}},
	.constructor			= default_device_constructor,
	.phase3_scan			= scan_static_bus,
	.phase4_read_resources		= pci_dev_read_resources,
	.phase4_set_resources		= pci_dev_set_resources,
	.phase5_enable_resources	= cs5536_pci_dev_enable_resources,
	.phase6_init			= southbridge_init,
};

struct device_operations cs5536_ide = {
	.id = {.type = DEVICE_ID_PCI,
		.u = {.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = PCI_DEVICE_ID_AMD_CS5536_B0_IDE}}},
	.constructor		 = default_device_constructor,
#warning FIXME: what has to go in phase3_scan?
	.phase3_scan		 = 0,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = ide_init,
	.ops_pci		 = &pci_dev_ops_pci,
};

