#include <console.h>
#include <device/device.h>
#include <pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <types.h>
#include <io.h>
/*
 * This file is part of the coreboot project.
 * It came from libpayload project. 
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 coresystems GmbH
 * Copyright (C) 2008 Ronald G. Minnich (conversion from libpayload to coreboot)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Functions for accessing PCI configuration space with type 1 accesses
 */

/* The EXT is for extended register sets, i.e. chipsets that have more than 8 bits of registers */
/* Here is one reason that overkill on types can bite you: Suppose we had insisted on u8 for 'where' for years. 
 * We would have to rewrite everything to grow 'where' to 12 bits! There is a reason that C uses 'int'.
 * It's a good handy type meaning 'value suitable to the native machine register size'
 * Also note these take a PCI_BDF -- meaning that the 'where' just needs to be 'or'ed in. 
 */

#define PCI_IO_CFG_EXT  0
#if PCI_IO_CFG_EXT
#define CONFIG_CMD(bdf, where)   (0x80000000 | (bdf) | (where & ~3))
#else
#define CONFIG_CMD(bdf, where)   (0x80000000 | (bdf) | ((where & 0xff) & ~3) | ((where & 0xf00)<<16) )
#endif

u8 pci_conf1_read_config8(u32 bdf, int where)
{
		outl(CONFIG_CMD(bdf, where), 0xCF8);
		return inb(0xCFC + (where & 3));
}

u16 pci_conf1_read_config16(u32 bdf, int where)
{
		outl(CONFIG_CMD(bdf, where), 0xCF8);
		return inw(0xCFC + (where & 2));
}

u32 pci_conf1_read_config32(u32 bdf, int where)
{
		outl(CONFIG_CMD(bdf, where), 0xCF8);
		return inl(0xCFC);
}

void  pci_conf1_write_config8(u32 bdf, int where, u8 value)
{
		outl(CONFIG_CMD(bdf, where), 0xCF8);
		outb(value, 0xCFC + (where & 3));
}

void pci_conf1_write_config16(u32 bdf, int where, u16 value)
{
		outl(CONFIG_CMD(bdf, where), 0xCF8);
		outw(value, 0xCFC + (where & 2));
}

void pci_conf1_write_config32(u32 bdf, int where, u32 value)
{
		outl(CONFIG_CMD(bdf, where), 0xCF8);
		outl(value, 0xCFC);
}

/**
 * find a device given a vendor id and a device id. 
 *  WARNING: the busdevfn is returned in a form suitable for use with the other functions in this file. 
 *  THIS MEANS THAT THE bus is the top 16 bits, the devfn is the high byte of the low 16 bits. 
 *   One can do this: 
 *  u32 slot; 
 *  find(vendor, device, &slot);
 * pci_conf1_write_config32(slot, this, that);
 * This is a low-level interface. If you are using the device tree, as in stage 2, you should not be 
 * calling this interface directly. 
 * We should consider a breadth-first search. The reason is that in almost all cases, 
 * the device you want in the coreboot context is on bus 0. 
 * 
 * @param bus Bus
 * @param vid vendor ID
 * @param did device ID
 * @param busdevfn pointer to a u32 in which the slot is returned. 
 * @return 1 if found, 0 otherwise
 */

int pci_conf1_find_on_bus(u16 bus, u16 vid, u16 did, u32 *busdevfn)
{
	u16 devfn;
	u32 val;
	u8 hdr;
	int bdf = bus << 16;
	printk(BIOS_SPEW, "pci_conf1_find_on_bus: bus %d, find 0x%04x:%04x\n", 
				bus, vid, did);
	/* skip over all the functions in a device -- 
	 * multifunction devices always have one vendor */
	for (devfn = 0; devfn < 0x100; devfn += 1) {
		u32 confaddr = bdf | (devfn << 8);
		val = pci_conf1_read_config32(confaddr, PCI_VENDOR_ID);

		if (val == 0xffffffff || val == 0x00000000 ||
		    val == 0x0000ffff || val == 0xffff0000)
			continue;
		if (val == ((did << 16) | vid)) {
			*busdevfn = confaddr;
			return 1;
		}

		hdr = pci_conf1_read_config8(confaddr, PCI_HEADER_TYPE);
		hdr &= 0x7F;
		if (hdr == PCI_HEADER_TYPE_BRIDGE || hdr == PCI_HEADER_TYPE_CARDBUS) {
			unsigned int busses;
			busses = pci_conf1_read_config32(confaddr, PCI_PRIMARY_BUS);
			/* We should never see a value of 0.
			 * this can happen if we run this before 
			 * things are set up (which we have to be able to do 
			 * in stage 0 
			 */
			if (!busses) {
				printk(BIOS_WARNING, "pci_conf1_find_on_bus: busses is 0!\n");
				continue;
			}
			if (pci_conf1_find_on_bus((busses >> 8) & 0xFF, vid, did, busdevfn))
				return 1;
		}
	}

	return 0;
}

int pci_conf1_find_device(u16 vid, u16 did, u32 * dev)
{
	return pci_conf1_find_on_bus(0, vid, did, dev);
}
