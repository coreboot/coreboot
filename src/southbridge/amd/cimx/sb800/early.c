/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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


//#include <config.h>
#include <stdint.h>
#include <device/pci_ids.h>
#include <arch/io.h>		/* inl, outl */
#include <arch/romcc_io.h>	/* device_t */
#include "SBPLATFORM.h"
#include "SbEarly.h"
#include "cfg.h"		/*sb800_cimx_config*/
#include <console/console.h>
#include <console/loglevel.h>
#include "smbus.h"


/**
 * @brief Get SouthBridge device number
 * @param[in] bus target bus number
 * @return southbridge device number
 */
u32 get_sbdn(u32 bus)
{
	device_t dev;

    printk(BIOS_DEBUG, "SB800 - Early.c - get_sbdn - Start.\n");
	//dev = PCI_DEV(bus, 0x14, 0);
	dev = pci_locate_device_on_bus(
			PCI_ID(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SB800_SM),
			bus);

    printk(BIOS_DEBUG, "SB800 - Early.c - get_sbdn - End.\n");
	return (dev >> 15) & 0x1f;
}


/**
 * @brief South Bridge CIMx romstage entry,
 *        wrapper of sbPowerOnInit entry point.
 */
void sb_poweron_init(void)
{
	AMDSBCFG sb_early_cfg;

    printk(BIOS_DEBUG, "SB800 - Early.c - sb_poweron_init - Start.\n");
	sb800_cimx_config(&sb_early_cfg);
	//sb_early_cfg.StdHeader.Func = SB_POWERON_INIT;
	//AmdSbDispatcher(&sb_early_cfg);
	//TODO
	//AMD_IMAGE_HEADER was missing, when using AmdSbDispatcher,
	// VerifyImage() will fail, LocateImage() take minitues to find the image.
	sbPowerOnInit(&sb_early_cfg);
    printk(BIOS_DEBUG, "SB800 - Early.c - sb_poweron_init - End.\n");
}


static void pmio_write(u8 reg, u8 value)
{
	outb(reg, PM_INDEX);
	outb(value, PM_INDEX + 1);
}


static u8 pmio_read(u8 reg)
{
	outb(reg, PM_INDEX);
	return inb(PM_INDEX + 1);
}


/* RPR 2.28 Get SB ASIC Revision.*/
static u8 get_sb800_revision(void)
{
	device_t dev;
	u8 rev_id;
	u8 rev = 0;

	/* if (rev != 0) return rev; */

	dev = PCI_DEV(0, 0x14, 0);//pci_locate_device(PCI_ID(0x1002, 0x4385), 0);

	if (dev == PCI_DEV_INVALID) {
		die("SMBUS controller not found\n");
		/* NOT REACHED */
	}
	rev_id =  pci_read_config8(dev, 0x08);

	if (rev_id == 0x40) {
		rev = REV_SB800_A11;
	} else if (rev_id == 0x41) {
		rev = REV_SB800_A12;
	} else {
		die("It is not SB800 or SB810\r\n");
	}

	return rev;
}


/* sbDevicesPorInitTable */
static void sb800_devices_por_init(void)
{
	device_t dev;
	u8 byte;

	printk(BIOS_INFO, "sb800_devices_por_init()\n");
	/* SMBus Device, BDF:0-20-0 */
	printk(BIOS_INFO, "sb800_devices_por_init(): SMBus Device, BDF:0-20-0\n");
	dev = PCI_DEV(0, 0x14, 0);//pci_locate_device(PCI_ID(0x1002, 0x4385), 0);

	if (dev == PCI_DEV_INVALID) {
		die("SMBUS controller not found\n");
		/* NOT REACHED */
	}
	printk(BIOS_INFO, "SMBus controller enabled, sb revision is A%x\n",
		    get_sb800_revision());

	/* sbPorAtStartOfTblCfg */
	/* rpr 4.1.Set A-Link bridge access address.
	 * This is an I/O address. The I/O address must be on 16-byte boundry.  */
	//pci_write_config32(dev, 0xf0, AB_INDX);
	pmio_write(0xE0, AB_INDX & 0xFF);
	pmio_write(0xE1, (AB_INDX >> 8) & 0xFF);
	pmio_write(0xE2, (AB_INDX >> 16) & 0xFF);
	pmio_write(0xE3, (AB_INDX >> 24) & 0xFF);

	/* To enable AB/BIF DMA access, a specific register inside the BIF register space needs to be configured first. */
	/* 4.2:Enables the SB800 to send transactions upstream over A-Link Express interface. */
	axcfg_reg(0x04, 1 << 2, 1 << 2);
	//axindxc_reg(0x21, 0xff, 0);

	/* 4.15:Enabling Non-Posted Memory Write for the K8 Platform */
	axindxc_reg(0x10, 1 << 9, 1 << 9);
	/* END of sbPorAtStartOfTblCfg */

	/* sbDevicesPorInitTables */
	/* set smbus iobase */
	//pci_write_config32(dev, 0x90, SMBUS_IO_BASE | 1);
	/* The base address of SMBUS is set in a different way with sb700. */
	byte = (SMBUS_IO_BASE & 0xFF) | 1;
	pmio_write(0x2c, byte & 0xFF);
	pmio_write(0x2d, SMBUS_IO_BASE >> 8);

	/* AcpiMMioDecodeEn */
	byte = pmio_read(0x24);
	byte |= 1;
	byte &= ~(1 << 1);
	pmio_write(0x24, byte);
	/* enable smbus controller interface */
	//byte = pci_read_config8(dev, 0xd2);
	//byte |= (1 << 0);
	//pci_write_config8(dev, 0xd2, byte);

	/* KB2RstEnable */
	//pci_write_config8(dev, 0x40, 0x44);

	/* Enable ISA Address 0-960K decoding */
	//pci_write_config8(dev, 0x48, 0x0f);

	/* Enable ISA  Address 0xC0000-0xDFFFF decode */
	//pci_write_config8(dev, 0x49, 0xff);

	/* Enable decode cycles to IO C50, C51, C52 GPM controls. */
	//byte = pci_read_config8(dev, 0x41);
	//byte &= 0x80;
	//byte |= 0x33;
	//pci_write_config8(dev, 0x41, byte);

	/* Legacy DMA Prefetch Enhancement, CIM masked it. */
	/* pci_write_config8(dev, 0x43, 0x1); */

	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);

	/* IDE Device, BDF:0-20-1 */
	printk(BIOS_INFO, "sb800_devices_por_init(): IDE Device, BDF:0-20-1\n");
	dev = PCI_DEV(0, 0x14, 1);//pci_locate_device(PCI_ID(0x1002, 0x439C), 0);
	/* Disable prefetch */
	byte = pci_read_config8(dev, 0x63);
	byte |= 0x1;
	pci_write_config8(dev, 0x63, byte);

	/* LPC Device, BDF:0-20-3 */
	printk(BIOS_INFO, "sb800_devices_por_init(): LPC Device, BDF:0-20-3\n");
	dev = PCI_DEV(0, 0x14, 3);//pci_locate_device(PCI_ID(0x1002, 0x439D), 0);
	/* DMA enable */
	pci_write_config8(dev, 0x40, 0x04);

	/* LPC Sync Timeout */
	pci_write_config8(dev, 0x49, 0xFF);

	/* Set LPC ROM size, it has been done in sb800_lpc_init().
	 * enable LPC ROM range, 0xfff8: 512KB, 0xfff0: 1MB;
	 * enable LPC ROM range, 0xfff8: 512KB, 0xfff0: 1MB
	 * pci_write_config16(dev, 0x68, 0x000e)
	 * pci_write_config16(dev, 0x6c, 0xfff0);*/

	/* Enable Tpm12_en and Tpm_legacy. I don't know what is its usage and copied from CIM. */
	pci_write_config8(dev, 0x7C, 0x05);

	/* P2P Bridge, BDF:0-20-4, the configuration of the registers in this dev are copied from CIM,
	 */
	printk(BIOS_INFO, "sb800_devices_por_init(): P2P Bridge, BDF:0-20-4\n");
	dev = PCI_DEV(0, 0x14, 4);//pci_locate_device(PCI_ID(0x1002, 0x4384), 0);

	/* Arbiter enable. */
	pci_write_config8(dev, 0x43, 0xff);

	/* Set PCDMA request into hight priority list. */
	/* pci_write_config8(dev, 0x49, 0x1) */ ;

	pci_write_config8(dev, 0x40, 0x26);

	pci_write_config8(dev, 0x0d, 0x40);
	pci_write_config8(dev, 0x1b, 0x40);
	/* Enable PCIB_DUAL_EN_UP will fix potential problem with PCI cards. */
	pci_write_config8(dev, 0x50, 0x01);

	/* SATA Device, BDF:0-17-0, Non-Raid-5 SATA controller */
	printk(BIOS_INFO, "sb800_devices_por_init(): SATA Device, BDF:0-18-0\n");
	dev = PCI_DEV(0, 0x11, 0);//pci_locate_device(PCI_ID(0x1002, 0x4390), 0);

	/*PHY Global Control*/
	pci_write_config16(dev, 0x86, 0x2C00);
}


/*
*/
static void sb800_por_init(void)
{
	/* sbDevicesPorInitTable + sbK8PorInitTable */
	sb800_devices_por_init();

	/* sbPmioPorInitTable + sbK8PmioPorInitTable */
	//sb800_pmio_por_init();
}


static void sb800_acpi_init(void)
{
	pmio_write(0x60, ACPI_PM_EVT_BLK & 0xFF);
	pmio_write(0x61, ACPI_PM_EVT_BLK >> 8);
	pmio_write(0x62, ACPI_PM1_CNT_BLK & 0xFF);
	pmio_write(0x63, ACPI_PM1_CNT_BLK >> 8);
	pmio_write(0x64, ACPI_PM_TMR_BLK & 0xFF);
	pmio_write(0x65, ACPI_PM_TMR_BLK >> 8);
	pmio_write(0x68, ACPI_GPE0_BLK & 0xFF);
	pmio_write(0x69, ACPI_GPE0_BLK >> 8);

	/* CpuControl is in \_PR.CPU0, 6 bytes */
	pmio_write(0x66, ACPI_CPU_CONTROL & 0xFF);
	pmio_write(0x67, ACPI_CPU_CONTROL >> 8);

	pmio_write(0x6A, 0xB0);	/* AcpiSmiCmdLo */
	pmio_write(0x6B, 0);	/* AcpiSmiCmdHi */

	pmio_write(0x6E, 0xB8);	/* AcpiPmaCntBlkLo */
	pmio_write(0x6F, 0);	/* AcpiPmaCntBlkHi */

	pmio_write(0x6C, ACPI_PMA_CNT_BLK & 0xFF);
	pmio_write(0x6D, ACPI_PMA_CNT_BLK >> 8);

	pmio_write(0x74, 1<<0 | 1<<1 | 1<<4 | 1<<2); /* AcpiDecodeEnable, When set, SB uses
					* the contents of the PM registers at
					* index 60-6B to decode ACPI I/O address.
					* AcpiSmiEn & SmiCmdEn*/
	/* RTC_En_En, TMR_En_En, GBL_EN_EN */
	outl(0x1, ACPI_PM1_CNT_BLK);		  /* set SCI_EN */
}


/*
* This function should be called after enable_sb800_smbus().
*/
void sb800_early_setup(void)
{
	printk(BIOS_INFO, "sb800_early_setup()\n");
	sb800_por_init();
	sb800_acpi_init();
}


/*
* Add any south bridge setting.
*/
static void sb800_pci_cfg(void)
{
	device_t dev;
	u8 byte;

	/* SMBus Device, BDF:0-20-0 */
	dev = PCI_DEV(0, 0x14, 0);//pci_locate_device(PCI_ID(0x1002, 0x4385), 0);
	/* Enable watchdog decode timer */
	byte = pci_read_config8(dev, 0x41);
	byte |= (1 << 3);
	pci_write_config8(dev, 0x41, byte);

	/* rpr 7.4. Set to 1 to reset USB on the software (such as IO-64 or IO-CF9 cycles)
	 * generated PCIRST#. */
	byte = pmio_read(0xF0);
	byte |= (1 << 2);
	pmio_write(0xF0, byte);

	/* IDE Device, BDF:0-20-1 */
	dev = PCI_DEV(0, 0x14, 1);//pci_locate_device(PCI_ID(0x1002, 0x439C), 0);
	/* Enable IDE Explicit prefetch, 0x63[0] clear */
	byte = pci_read_config8(dev, 0x63);
	byte &= 0xfe;
	pci_write_config8(dev, 0x63, byte);

	/* LPC Device, BDF:0-20-3 */
	/* The code below is ported from old chipset. It is not
	 * metioned in RPR. But I keep them. The registers and the
	 * comments are compatible. */
	dev = PCI_DEV(0, 0x14, 3);//pci_locate_device(PCI_ID(0x1002, 0x439D), 0);
	/* Enabling LPC DMA function. */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 2);
	pci_write_config8(dev, 0x40, byte);
	/* Disabling LPC TimeOut. 0x48[7] clear. */
	byte = pci_read_config8(dev, 0x48);
	byte &= 0x7f;
	pci_write_config8(dev, 0x48, byte);
	/* Disabling LPC MSI Capability, 0x78[1] clear. */
	byte = pci_read_config8(dev, 0x78);
	byte &= 0xfd;
	pci_write_config8(dev, 0x78, byte);

	/* SATA Device, BDF:0-17-0, Non-Raid-5 SATA controller */
	dev = PCI_DEV(0, 0x11, 0);//pci_locate_device(PCI_ID(0x1002, 0x4390), 0);
	/* rpr7.12 SATA MSI and D3 Power State Capability. */
	byte = pci_read_config8(dev, 0x40);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x40, byte);
	if (get_sb800_revision() <= 0x12)
		pci_write_config8(dev, 0x34, 0x70); /* set 0x61 to 0x70 if S1 is not supported. */
	else
		pci_write_config8(dev, 0x34, 0x50); /* set 0x61 to 0x50 if S1 is not supported. */
	byte &= ~(1 << 0);
	pci_write_config8(dev, 0x40, byte);
}


/*
* It should be called during early POST after memory detection and BIOS shadowing but before PCI bus enumeration.
*/
void sb800_before_pci_init(void)
{
	printk(BIOS_INFO, "sb800_before_pci_init()\n");
	sb800_pci_cfg();
}

