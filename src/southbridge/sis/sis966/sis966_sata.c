/*
 * This file is part of the LinuxBIOS project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "sis966.h"
#include <arch/io.h>

#if 1
uint8_t	SiS_SiS1183_init[68][3]={
{0x04, 0x00, 0x05},					 
{0x09, 0x00, 0x05},					 
{0x2C, 0x00, 0x39},					 
{0x2D, 0x00, 0x10},					  
{0x2E, 0x00, 0x83},					 
{0x2F, 0x00, 0x11},					 
{0x90, 0x00, 0x40},					 
{0x91, 0x00, 0x00},	// set mode				 
{0x50, 0x00, 0xA2},					 
{0x52, 0x00, 0xA2},					 
{0x55, 0x00, 0x96},					 
{0x52, 0x00, 0xA2},					 
{0x55, 0xF7, 0x00},					 
{0x56, 0x00, 0xC0},	
{0x57, 0x00, 0x14},
{0x67, 0x00, 0x28},					 
{0x81, 0x00, 0xB3},					 
{0x82, 0x00, 0x72},					 
{0x83, 0x00, 0x40},					 
{0x85, 0x00, 0xB3},					 
{0x86, 0x00, 0x72},					 
{0x87, 0x00, 0x40},
{0x88, 0x00, 0xDE},      // after set mode					 
{0x89, 0x00, 0xB3},					 
{0x8A, 0x00, 0x72},					 
{0x8B, 0x00, 0x40},					 
{0x8C, 0x00, 0xDE},					 
{0x8D, 0x00, 0xB3},					 
{0x8E, 0x00, 0x92},					 
{0x8F, 0x00, 0x40},					 
{0x93, 0x00, 0x00},					 
{0x94, 0x00, 0x80},					 
{0x95, 0x00, 0x08},					 
{0x96, 0x00, 0x80},					 
{0x97, 0x00, 0x08},
{0x9C, 0x00, 0x80},					 
{0x9D, 0x00, 0x08},					 
{0x9E, 0x00, 0x80},					 
{0x9F, 0x00, 0x08},					 
{0xA0, 0x00, 0x15},					 
{0xA1, 0x00, 0x15},					 
{0xA2, 0x00, 0x15},					 
{0xA3, 0x00, 0x15},


{0xD8, 0xFE, 0x01},	// Com reset				 
{0xC8, 0xFE, 0x01},
{0xE8, 0xFE, 0x01},
{0xF8, 0xFE, 0x01},

{0xD8, 0xFE, 0x00},	// Com reset				 
{0xC8, 0xFE, 0x00},
{0xE8, 0xFE, 0x00},
{0xF8, 0xFE, 0x00},


{0xC4, 0xFF, 0xFF},    // Clear status					 
{0xC5, 0xFF, 0xFF},
{0xC6, 0xFF, 0xFF},
{0xC7, 0xFF, 0xFF},
{0xD4, 0xFF, 0xFF},
{0xD5, 0xFF, 0xFF},
{0xD6, 0xFF, 0xFF},
{0xD7, 0xFF, 0xFF},
{0xE4, 0xFF, 0xFF},    // Clear status					 
{0xE5, 0xFF, 0xFF},
{0xE6, 0xFF, 0xFF},
{0xE7, 0xFF, 0xFF},
{0xF4, 0xFF, 0xFF},
{0xF5, 0xFF, 0xFF},
{0xF6, 0xFF, 0xFF},
{0xF7, 0xFF, 0xFF},

{0x00, 0x00, 0x00}					//End of table
};


#else 
uint8_t	SiS_SiS1183_init[5][3]={

{0xD8, 0xFE, 0x01},	// Com reset				 
{0xC8, 0xFE, 0x01},
{0xE8, 0xFE, 0x01},
{0xF8, 0xFE, 0x01},

{0x00, 0x00, 0x00}
};					//End of table

uint8_t	SiS_SiS1183_init2[21][3]={
{0xD8, 0xFE, 0x00},			 
{0xC8, 0xFE, 0x00},
{0xE8, 0xFE, 0x00},
{0xF8, 0xFE, 0x00},


{0xC4, 0xFF, 0xFF},    // Clear status					 
{0xC5, 0xFF, 0xFF},
{0xC6, 0xFF, 0xFF},
{0xC7, 0xFF, 0xFF},
{0xD4, 0xFF, 0xFF},
{0xD5, 0xFF, 0xFF},
{0xD6, 0xFF, 0xFF},
{0xD7, 0xFF, 0xFF},
{0xE4, 0xFF, 0xFF},    // Clear status					 
{0xE5, 0xFF, 0xFF},
{0xE6, 0xFF, 0xFF},
{0xE7, 0xFF, 0xFF},
{0xF4, 0xFF, 0xFF},
{0xF5, 0xFF, 0xFF},
{0xF6, 0xFF, 0xFF},
{0xF7, 0xFF, 0xFF},


{0x00, 0x00, 0x00}					//End of table
};
#endif



static void sata_init(struct device *dev)
{
	uint32_t dword;       
	struct southbridge_sis_sis966_config *conf;

	
struct resource *res;
uint16_t base;
uint8_t temp8;
	
	conf = dev->chip_info;
printk_debug("SATA(SiS1183)_init-------->\r\n");

#if 1
//-------------- enable IDE (SiS5513) -------------------------
{    
       uint8_t  temp8;
       int i=0;
	while(SiS_SiS1183_init[i][0] != 0)
	{				temp8 = pci_read_config8(dev, SiS_SiS1183_init[i][0]);
					temp8 &= SiS_SiS1183_init[i][1];
					temp8 |= SiS_SiS1183_init[i][2];				
					pci_write_config8(dev, SiS_SiS1183_init[i][0], temp8);
					i++;
	};
}
/*
mdelay(5);
{    
       uint8_t  temp8;
       int i=0;
	while(SiS_SiS1183_init2[i][0] != 0)
	{				temp8 = pci_read_config8(dev, SiS_SiS1183_init2[i][0]);
					temp8 &= SiS_SiS1183_init2[i][1];
					temp8 |= SiS_SiS1183_init2[i][2];				
					pci_write_config8(dev, SiS_SiS1183_init2[i][0], temp8);
					i++;
	};
}
*/
//-----------------------------------------------------------
#endif


#if 0

	dword = pci_read_config32(dev, 0x50);
	/* Ensure prefetch is disabled */
	dword &= ~((1 << 15) | (1 << 13));
	if(conf) {
		if (conf->sata1_enable) {
			/* Enable secondary SATA interface */
			dword |= (1<<0);
			printk_debug("SATA S \t");
		}
		if (conf->sata0_enable) {
			/* Enable primary SATA interface */
			dword |= (1<<1);
			printk_debug("SATA P \n");
		}
	} else {
		dword |= (1<<1) | (1<<0);
		printk_debug("SATA P and S \n");
	}


#if 1
	dword &= ~(0x1f<<24);
	dword |= (0x15<<24);
#endif
	pci_write_config32(dev, 0x50, dword);

	dword = pci_read_config32(dev, 0xf8);
	dword |= 2;
	pci_write_config32(dev, 0xf8, dword);

#endif

{
uint32_t i,j;
uint32_t temp32;

for (i=0;i<10;i++){
   temp32=0;
   temp32= pci_read_config32(dev, 0xC0);
   for ( j=0;j<0xFFFF;j++);
   printk_debug("status= %x",temp32);
   if (((temp32&0xF) == 0x3) || ((temp32&0xF) == 0x0)) break;
}
printk_debug("\n");
}

#if 0	

res = find_resource(dev, 0x10);	
base =(uint16_t ) res->base;
printk_debug("BASE ADDR %x\n",base);
base&=0xFFFE;
printk_debug("SATA status %x\n",inb(base+7));

{
int i;
for(i=0;i<0xFF;i+=4)
{
    if((i%16)==0)
    {
      print_debug("\r\n");print_debug_hex8(i);print_debug("  ");}
      print_debug_hex32(pci_read_config32(dev,i));
      print_debug("  ");
    }
}
#endif
printk_debug("sata_init <--------\r\n");

}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}
static struct pci_operations lops_pci = {
	.set_subsystem	= lpci_set_subsystem,
};

static struct device_operations sata_ops = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
//	.enable		= sis966_enable,
	.init		= sata_init,
	.scan_bus	= 0,
	.ops_pci	= &lops_pci,
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_SATA0,
};

static const struct pci_driver sata1_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_SATA1,
};
