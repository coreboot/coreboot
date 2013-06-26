/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

/* Vortex86EX IDE to SD/STAT controller need to enable ATA decoder and
 * setup timing. */

/*
 * Primary ATA Timing Register (PATR) - Offset 40-41h
 * Secondary ATA Timing Register (PATR) - Offset 42-43h
 *
 * Bit     R/W     Default Description
 * 15      R/W     0h      ATA Decode Enable. Decode the I/O addressing ranges assigned to this controller.
 *                         1: Enabled.
 *                         0: Disabled.
 * 14      R/W     0b      Device 1 ATA Timing Register Enable
 *                         1: Enable the device 1 ATA timing.
 *                         0: Disable the device 1 ATA timing
 * 13-12   R/W     0h      IORDY Sample Mode. Sets the setup time before IORDY are sampled.
 *                         00: PIO-0
 *                         10: PIO-2, SW-2
 *                         10: PIO-3, PIO-4, MW-1, MW-2
 *                         11: Reserved
 * 11-10   RO      0h      Reserved
 * 9-8     R/W     0h      Recovery Mode. Sets the hold time after IORDY are sampled.
 *                         00: PIO-0, PIO-2, SW-2
 *                         10: PIO-3, MW-1
 *                         10: Reserved
 *                         11: PIO-4, MW-2
 * 7       R/W     0b      DMA Timing Enable Only Select 1
 *                         1: Enable the device timings for DMA operation for device 1
 *                         0: Disable the device timings for DMA operation for device 1
 * 6       R/W     0b      ATA/ATAPI Device Indicator 1
 *                         1: Indicate presence od an ATA device
 *                         0: Indicate presence od an ATAPI device
 * 5       R/W     0b      IORDY Sample Point Enabled Select 1
 *                         1: Enable IORDY sample for PIO transfers for device 1
 *                         0: Disable IORDY sample for PIO transfers for device 1
 * 4       R/W     0b      Fast Drive Timing Select 1
 *                         1: Enable faster than PIO-0 timing modes for device 1
 *                         0: Disable faster than PIO-0 timing modes for device 1
 * 3       R/W     0b      DMA Timing Enable Only Select 0
 *                         1: Enable the device timings for DMA operation for device 0
 *                         0: Disable the device timings for DMA operation for device 0
 * 2       R/W     0b      ATA/ATAPI Device Indicator 0
 *                         1: Indicate presence od an ATA device
 *                         0: Indicate presence od an ATAPI device
 * 1       R/W     0b      IORDY Sample Point Enabled Select 0
 *                         1: Enable IORDY sample for PIO transfers for device 0
 *                         0: Disable IORDY sample for PIO transfers for device 0
 * 0       R/W     0b      Fast Drive Timing Select 0
 *                         1: Enable faster than PIO-0 timing modes for device 0
 *                         0: Disable faster than PIO-0 timing modes for device 0
 * */

static void init_ide_ata_timing(struct device *dev)
{
	u16 ata_timing_pri, ata_timing_sec;
	u32 ata_timing_reg32;
	/* Primary channel is SD. */
#if CONFIG_IDE1_ENABLE
	ata_timing_pri = 0x8000;
#else
	ata_timing_pri = 0x0000;	// Disable this channel.
#endif
	/* Secondary channel is SATA. */
#if CONFIG_IDE2_ENABLE
	ata_timing_sec = 0xa30f;	// This setting value works well.
#else
	ata_timing_sec = 0x0000;	// Disable this channel.
#endif
	ata_timing_reg32 = (ata_timing_sec << 16) | ata_timing_pri;
	pci_write_config32(dev, 0x40, ata_timing_reg32);
#if CONFIG_IDE_NATIVE_MODE
	/* Set both IDE channels to native mode. */
	u8 prog_if;
	prog_if = pci_read_config8(dev, 0x09);
	prog_if |= 5;
	pci_write_config8(dev, 0x09, prog_if);
#endif
	/* MMC function enable. */
	u32 sd_ctrl_reg;
	sd_ctrl_reg = pci_read_config32(dev, 0x94);
	sd_ctrl_reg |= 0x0200;
	pci_write_config32(dev, 0x94, sd_ctrl_reg);
	printk(BIOS_INFO, "Vortex86EX IDE controller ATA TIMING reg = %08x\n", ata_timing_reg32);
}

static void setup_std_ide_compatible(struct device *dev)
{
#if CONFIG_IDE_STANDARD_COMPATIBLE
	// Misc Control Register (MCR) Offset 90h
	// bit 0 = Vendor ID Access, bit 1 = Device ID Access.
	u8 mcr;
	u16 vendor = (u16) (CONFIG_IDE_COMPATIBLE_SELECTION >> 16);
	u16 device = (u16) (CONFIG_IDE_COMPATIBLE_SELECTION & 0xffff);
	// unlock vendor/device ID access bits.
	mcr = pci_read_config8(dev, 0x90);
	pci_write_config8(dev, 0x90, mcr | 3);
	pci_write_config16(dev, 0x00, vendor);
	pci_write_config16(dev, 0x02, device);
	// restore lock bits.
	pci_write_config8(dev, 0x90, mcr);
#endif
}

static void vortex_ide_init(struct device *dev)
{
	if (dev->device == 0x1010) {
		// This is SX/old DX IDE controller.
		// Set IOCFG bit 15/13 : IDE Decoder Enable for Primary/Secondary channel.
		u16 iocfg = 0xa000;
		pci_write_config16(dev, 0x40, iocfg);
	} else if (dev->device == 0x1011 || dev->device == 0x1012) {
		// This is new DX/MX/MX+/DX2 IDE controller.
		init_ide_ata_timing(dev);
		setup_std_ide_compatible(dev);
	}
}

static struct device_operations vortex_ide_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = vortex_ide_init,
	.scan_bus         = 0,
};

static const struct pci_driver vortex_ide_driver_1010 __pci_driver = {
	.ops    = &vortex_ide_ops,
	.vendor = PCI_VENDOR_ID_RDC,
	.device = 0x1010,
};

static const struct pci_driver vortex_ide_driver_1011 __pci_driver = {
	.ops    = &vortex_ide_ops,
	.vendor = PCI_VENDOR_ID_RDC,
	.device = 0x1011,
};

static const struct pci_driver vortex_ide_driver_1012 __pci_driver = {
	.ops    = &vortex_ide_ops,
	.vendor = PCI_VENDOR_ID_RDC,
	.device = 0x1012,
};
