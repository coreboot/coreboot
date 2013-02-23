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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "sis966.h"

uint8_t	SiS_SiS7001_init[16][3]={
{0x04, 0x00, 0x07},
{0x0C, 0x00, 0x08},
{0x0D, 0x00, 0x20},

{0x2C, 0xFF, 0x39},
{0x2D, 0xFF, 0x10},
{0x2E, 0xFF, 0x01},
{0x2F, 0xFF, 0x70},

{0x44, 0x00, 0x54},
{0x45, 0x00, 0xAD},
{0x46, 0x00, 0x01},
{0x47, 0x00, 0x00},

{0x48, 0x00, 0x73},
{0x49, 0x00, 0x02},
{0x4A, 0x00, 0x00},
{0x4B, 0x00, 0x00},

{0x00, 0x00, 0x00}					//End of table
};

static void usb_init(struct device *dev)
{
        print_debug("USB 1.1 INIT:---------->\n");

//-------------- enable USB1.1 (SiS7001) -------------------------
{
        uint8_t  temp8;
        int i=0;

	 while(SiS_SiS7001_init[i][0] != 0)
	 {				temp8 = pci_read_config8(dev, SiS_SiS7001_init[i][0]);
					temp8 &= SiS_SiS7001_init[i][1];
					temp8 |= SiS_SiS7001_init[i][2];
					pci_write_config8(dev, SiS_SiS7001_init[i][0], temp8);
					i++;
	 };
}
//-----------------------------------------------------------

#if DEBUG_USB
{
        int i;

        print_debug("****** USB 1.1 PCI config ******");
        print_debug("\n    03020100  07060504  0B0A0908  0F0E0D0C");

        for(i=0;i<0xff;i+=4){
                if((i%16)==0){
                        print_debug("\n");
                        print_debug_hex8(i);
                        print_debug(": ");
                }
                print_debug_hex32(pci_read_config32(dev,i));
                print_debug("  ");
        }
        print_debug("\n");
}
#endif
        print_debug("USB 1.1 INIT:<----------\n");
}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}
static struct pci_operations lops_pci = {
	.set_subsystem	= lpci_set_subsystem,
};

static struct device_operations usb_ops = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init		= usb_init,
//	.enable		= sis966_enable,
	.scan_bus	= 0,
	.ops_pci	= &lops_pci,
};

static const struct pci_driver usb_driver __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_USB,
};
