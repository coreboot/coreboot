/*
 * (C) Copyright 2004 Nick Barker <nick.barker9@btinternet.com>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

 /* vt1211 routines and defines*/


#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "vt1211.h"
#include "chip.h"


static unsigned char vt1211hwmonitorinits[]={
 0x10,0x3, 0x11,0x10, 0x12,0xd, 0x13,0x7f,
 0x14,0x21, 0x15,0x81, 0x16,0xbd, 0x17,0x8a,
 0x18,0x0, 0x19,0x0, 0x1a,0x0, 0x1b,0x0,
 0x1d,0xff, 0x1e,0x0, 0x1f,0x73, 0x20,0x67,
 0x21,0xc1, 0x22,0xca, 0x23,0x74, 0x24,0xc2,
 0x25,0xc7, 0x26,0xc9, 0x27,0x7f, 0x29,0x0,
 0x2a,0x0, 0x2b,0xff, 0x2c,0x0, 0x2d,0xff,
 0x2e,0x0, 0x2f,0xff, 0x30,0x0, 0x31,0xff,
 0x32,0x0, 0x33,0xff, 0x34,0x0, 0x39,0xff,
 0x3a,0x0, 0x3b,0xff, 0x3c,0xff, 0x3d,0xff,
 0x3e,0x0, 0x3f,0xb0, 0x43,0xff, 0x44,0xff,
 0x46,0xff, 0x47,0x50, 0x4a,0x3, 0x4b,0xc0,
 0x4c,0x0, 0x4d,0x0, 0x4e,0xf, 0x5d,0x77,
 0x5c,0x0, 0x5f,0x33, 0x40,0x1};

static void start_conf_pnp(int dev)
{
	outb(0x87,0x2e);
	outb(0x87,0x2e);
	outb(7,0x2e);
	outb(dev,0x2f);
}
static void write_pnp(int reg, int val)
{
	outb(reg,0x2e);
	outb(val,0x2f);
}
static void end_conf_pnp()
{
	outb(0xaa,0x2e);
}

static void vt1211_init(struct superio_via_vt1211_config *conf)
{

	int i;
 	// Activate the vt1211 hardware monitor
	if(conf->enable_hwmon){
		start_conf_pnp(0x0b);
		write_pnp(0x60,0xec); 
		write_pnp(0x30,1);
		end_conf_pnp();

	 	// initialize vt1211 hardware monitor registers, which are at 0xECXX
 		for(i=0;i<sizeof(vt1211hwmonitorinits);i+=2)
 			outb(vt1211hwmonitorinits[i+1],0xec00+vt1211hwmonitorinits[i]);
	}
	if( conf->enable_fdc){
		// activate FDC
		start_conf_pnp(0); // fdc is device 0
		write_pnp(0x60,0xfc); // io address
		write_pnp(0x70,0x06); // interupt
		write_pnp(0x74,0x02); // dma
		write_pnp(0x30,0x01); // activate it
		end_conf_pnp();
	}	

	if( conf->enable_com_ports ){
		// activate com2
		start_conf_pnp(3);
		write_pnp(0x60,0xbe);
		write_pnp(0x70,0x3);
		write_pnp(0xf0,0x02);
		write_pnp(0x30,0x01);
		end_conf_pnp();
	}

	if( conf->enable_lpt ){
		// activate lpt
		start_conf_pnp(1);
		write_pnp(0x60,0xde);
		write_pnp(0x70,0x07);
		write_pnp(0x74,0x3);
		write_pnp(0x30,0x01);
		end_conf_pnp();
	}

}

static void superio_init(struct chip *chip, enum chip_pass pass)
{

	struct superio_via_vt1211_config *conf = 
		(struct superio_via_vt1211_config *)chip->chip_info;

	switch (pass) {
	case CONF_PASS_PRE_PCI:
		break;
		
	case CONF_PASS_POST_PCI:
		vt1211_init(conf);
		break;

	case CONF_PASS_PRE_BOOT:
		break;
		
	default:
		/* nothing yet */
		break;
	}
}

static void enumerate(struct chip *chip)
{
	extern struct device_operations default_pci_ops_bus;
	chip_enumerate(chip);
	chip->dev->ops = &default_pci_ops_bus;
}

struct chip_operations superio_via_vt1211_control = {
	CHIP_NAME("VIA vt1211")
	.enumerate = enumerate,
	.enable    = superio_init,
};
