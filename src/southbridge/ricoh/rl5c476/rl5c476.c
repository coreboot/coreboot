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



#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "rl5c476.h"
#include "chip.h"

static void udelay(int i){
	for(; i > 0 ; i--)
		inb(0x80);

}

static void
dump_south(void)
{
	device_t dev0;
	dev0 = dev_find_device(PCI_VENDOR_ID_RICOH, PCI_DEVICE_ID_RICOH_RL5C476, 0);
	dev0 = dev_find_device(PCI_VENDOR_ID_RICOH, PCI_DEVICE_ID_RICOH_RL5C476, dev0);
	int i,j;
	
	for(i = 0; i < 256; i += 16) {
		printk_debug("0x%x: ", i);
		for(j = 0; j < 16; j++) {
			printk_debug("%02x ", pci_read_config8(dev0, i+j));
		}
		printk_debug("\n");
	}
	printk_debug("Card32\n");
	for(i = 0 ; i < 256 ; i+=16){
		printk_debug("0x%x: ",i);
		for(j = 0 ; j < 16 ; j++){
			printk_debug(" %02x",*(unsigned char *)(0x80000000+i+j));
		}
		printk_debug("\n");
	}
	printk_debug("Card16\n");
	for(i = 0; i < 256; i += 16) {
		printk_debug("0x%x: ", i);
		for(j = 0; j < 16; j++) {
			printk_debug("%02x ", *(unsigned char *)(0x80000800+ i+j));
		}
		printk_debug("\n");
	}
	printk_debug("CF Config\n");
	for(i = 0 ; i < 256 ; i+=16){
		printk_debug("0x%x: ",i);
		for(j=0 ; j < 16 ; j++){
			printk_debug("%02x ",*(unsigned char *)(0x81000200 + i + j));
		}
		printk_debug("\n");
	}
}


static void rl5c476_init(device_t dev)
{
	//unsigned char enables;
	pc16reg_t *pc16;
	int i;

#error "FIXME implement carbus bridge support"
#error "FIXME this code is close to a but the conversion needs more work"
	/* cardbus controller function 1 for CF Socket */
	printk_debug("rl5c476 init\n");

	/* setup pci header manually because 'pci_device.c' doesn't know how to handle
         * pci to cardbus bridges - (header type 2 I think)
	 */


	/* initialize function zero - pcmcia socket so it behaves itself */
	/* FIXME - statically put control memory at 0xe0000000 for now
	 * one day the pci_device allocator might do this */
	pci_write_config32(dev,0x10,0xe0000000);
	pci_write_config8(dev,0x0d,0x20);
	pci_write_config8(dev,0x19,0x02);
	pci_write_config8(dev,0x1a,0x02);
	pci_write_config8(dev,0x1b,0x20);
	//pci_write_config8(dev,0x3c,0);
	pci_write_config8(dev,0x82,0x00a0);
	pci_write_config16(dev,0x04,0x07);

	
	/* get second function - i.e. compact flash socket */
	dev = dev_find_device(PCI_VENDOR_ID_RICOH, PCI_DEVICE_ID_RICOH_RL5C476, dev);


	/* FIXME - control structure statically declared at 0xe0008000 for now */
	pci_write_config32(dev,0x10,0xe0008000);
	pci_write_config8(dev,0x0d,0x20);
	pci_write_config8(dev,0x19,0x03);
	pci_write_config8(dev,0x1a,0x03);
	pci_write_config8(dev,0x1b,0x20);

	//pci_write_config8(dev,0x3c,0x0);
	pci_write_config16(dev,0x3e,0x0780);
	pci_write_config16(dev,0x82,0x00a0);

	pci_write_config16(dev,0x04,0x07);


	/* pick up where 16 bit card control structure is */
	pc16 = (pc16reg_t *)(0xe0008800);

	/* disable memory and io windows and turn off socket power */
	pc16->pwctrl = 0;

	/* disable irq lines */
	pc16->igctrl = 0;

	/* disable memory and I/O windows */
	pc16->awinen = 0;

	/* reset card, configure for I/O and set IRQ line */
	pc16->igctrl = 0x69;


	// set io window 0 for 1e8 - 1ef
	pc16->iostl0 = 0xe8;
	pc16->iosth0 = 1;

	pc16->iospl0 = 0xef;
	pc16->iosph0 = 1;

	// add io offset of 8 so that CF card will decode 0x1e8 as 0x1f0 i.e. the first byte of
	// a 16 byte aligned, 16 byte window etc
	pc16->ioffl0 = 0x8;
	pc16->ioffh0 = 0;

	// set io window 1 for 3ed - 3ee
	pc16->iostl1 = 0xed;
	pc16->iosth1 = 3;

	pc16->iospl1 = 0xee;
	pc16->iosph1 = 3;

	pc16->ioffl1 = 0x0;
	pc16->ioffh1 = 0;


	// FIXME statically declare CF config window at 0xe1000000
	pc16->smstl0 = 0;
	pc16->smsth0 = 0;
	pc16->smspl0 = 0;
	pc16->smsph0 = 0x80;
	pc16->moffl0 = 0;
	pc16->moffh0 = 0x40;
	pc16->smpga0 = 0xe1;

	// set I/O width for Auto Data width
	pc16->ioctrl = 0x22;


	// enable I/O window 0 and 1
	pc16->awinen = 0xc1;


	pc16->miscc1 = 1;

	// apply power and enable outputs
	pc16->pwctrl = 0xb0;
	

	// delay could be optimised, but this works
	udelay(100000);
	
	pc16->igctrl = 0x69;

	unsigned char *cptr;
	cptr = (unsigned char *)(0xe1000200);
	printk_debug("CF Config = %x\n",*cptr);

	// FIX Me 16 bit CF always have first config byte at 0x200 into Config structure,
        // but CF+ May Not according to spec - should locate through reading tuple data,
        // but this will do for now !!!


	// set CF to decode 16 IO bytes on any 16 byte boundary - rely on the io
	// windows of the bridge set up above to map those bytes into the 
	// addresses for ide controller 3 (0x1e8 - 0x1ef and 0x3ed - 0x3ee)
	*cptr = 0x41;


}

static struct device_operations ricoh_rl5c476_ops = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.inti             = rl5c476_init,
	.scan_bus         = pci_scan_bridge,
};

static struct pci_driver ricoh_rl5c476_driver __pci_driver = {
	.ops    = &ricoh_rl5c476_ops,
	.vendor = PCI_VENDOR_ID_RICOH,
	.device = PCI_DEVICE_ID_RICOH_RL5C476,
};

struct chip_operations southbridge_ricoh_rl5c476_control = {
	CHIP_NAME("RICOH RL5C476")
	.enable    = southbridge_init,
};
