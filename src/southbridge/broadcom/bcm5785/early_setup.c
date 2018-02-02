/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <reset.h>
#include "bcm5785.h"

static void bcm5785_enable_lpc(void)
{
	uint8_t byte;
	pci_devfn_t dev;

	dev = pci_locate_device(PCI_ID(0x1166, 0x0234), 0);

	/* LPC Control 0 */
	byte = pci_read_config8(dev, 0x44);
	/* Serial 0 */
	byte |= 1 << 6;
	pci_write_config8(dev, 0x44, byte);

	/* LPC Control 4 */
	byte = pci_read_config8(dev, 0x48);
	/* superio port 0x2e/4e enable */
	byte |= (1 << 1) | (1 << 0);
	pci_write_config8(dev, 0x48, byte);
}

static void bcm5785_enable_wdt_port_cf9(void)
{
	pci_devfn_t dev;
	uint32_t dword;
	uint32_t dword_old;

	dev = pci_locate_device(PCI_ID(0x1166, 0x0205), 0);

	dword_old = pci_read_config32(dev, 0x4c);
	dword = dword_old | (1 << 4); //enable Timer Func
	if (dword != dword_old)
		pci_write_config32(dev, 0x4c, dword);

	dword_old = pci_read_config32(dev, 0x6c);
	dword = dword_old | (1 << 9); //unhide Timer Func in pci space
	if (dword != dword_old)
		pci_write_config32(dev, 0x6c, dword);

	dev = pci_locate_device(PCI_ID(0x1166, 0x0238), 0);

	/* enable cf9 */
	pci_write_config8(dev, 0x40, 1 << 2);
}

unsigned get_sbdn(unsigned bus)
{
	pci_devfn_t dev;

	/* Find the device.
	 * There can only be one bcm5785 on a hypertransport chain/bus.
	 */
	dev = pci_locate_device_on_bus(
		PCI_ID(0x1166, 0x0036),
		bus);

	return (dev >> 15) & 0x1f;

}

#define SB_VFSMAF 0

void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn)
{
	//ACPI Decode Enable
	outb(0x0e, 0xcd6);
	outb(1 << 3, 0xcd7);

	// set port to 0x2060
	outb(0x67, 0xcd6);
	outb(0x60, 0xcd7);
	outb(0x68, 0xcd6);
	outb(0x20, 0xcd7);

	outb(0x69, 0xcd6);
	outb(7, 0xcd7);

	outb(0x64, 0xcd6);
	outb(9, 0xcd7);
}

void ldtstop_sb(void)
{
	outb(1, 0x2060);
}


void do_hard_reset(void)
{
	bcm5785_enable_wdt_port_cf9();

	set_bios_reset();

	/* full reset */
	outb(0x0a, 0x0cf9);
	outb(0x0e, 0x0cf9);
}

void do_soft_reset(void)
{
	bcm5785_enable_wdt_port_cf9();

	set_bios_reset();
#if 1
	/* link reset */
//	outb(0x02, 0x0cf9);
	outb(0x06, 0x0cf9);
#endif
}

static void bcm5785_enable_msg(void)
{
	pci_devfn_t dev;
	uint32_t dword;
	uint32_t dword_old;
	uint8_t byte;

	dev = pci_locate_device(PCI_ID(0x1166, 0x0205), 0);

	byte = pci_read_config8(dev, 0x42);
	byte = 1 << 1; //enable a20
	pci_write_config8(dev, 0x42, byte);

	dword_old = pci_read_config32(dev, 0x6c);
	// bit 5: enable A20 Message
	// bit 4: enable interrupt messages
	// bit 3: enable reset init message
	// bit 2: enable keyboard init message
	// bit 1: enable upsteam messages
	// bit 0: enable shutdowm message to init generation

	/* bit 1 and bit 4 must be set, otherwise
	 * interrupt msg will not be delivered to the processor
	 */
	dword = dword_old | (1 << 5) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
	if (dword != dword_old)
		pci_write_config32(dev, 0x6c, dword);
}

static void bcm5785_early_setup(void)
{
	uint8_t byte;
	uint32_t dword;
	pci_devfn_t dev;

//F0
	// enable device on bcm5785 at first
	dev = pci_locate_device(PCI_ID(0x1166, 0x0205), 0);
	dword = pci_read_config32(dev, 0x64);
	dword |= (1 << 15) | (1 << 11) | (1 << 3); // ioapci enable
	dword |= 1 << 8; // USB enable
	dword |= /* (1 << 27)|*/ 1 << 14; // IDE enable
	pci_write_config32(dev, 0x64, dword);

	byte = pci_read_config8(dev, 0x84);
	byte |= 1 << 0; // SATA enable
	pci_write_config8(dev, 0x84, byte);

// WDT and cf9 for later in ramstage to call hard_reset
	bcm5785_enable_wdt_port_cf9();

	bcm5785_enable_msg();


// IDE related
	//F0
	byte = pci_read_config8(dev, 0x4e);
	byte |= 1 << 4; //enable IDE ext regs
	pci_write_config8(dev, 0x4e, byte);

	//F1
	dev = pci_locate_device(PCI_ID(0x1166, 0x0214), 0);
	byte = pci_read_config8(dev, 0x48);
	byte &= ~1; // disable pri channel
	pci_write_config8(dev, 0x48, byte);
	pci_write_config8(dev, 0xb0, 0x01);
	pci_write_config8(dev, 0xb2, 0x02);
	byte = pci_read_config8(dev, 0x06);
	byte |= 1 << 4; // so b0, b2 can not be changed from now
	pci_write_config8(dev, 0x06, byte);
	byte = pci_read_config8(dev, 0x49);
	byte |= 1; // enable second channel
	pci_write_config8(dev, 0x49, byte);

	//F2
	dev = pci_locate_device(PCI_ID(0x1166, 0x0234), 0);

	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 3) | (1 << 2); // LPC Retry, LPC to PCI DMA enable
	pci_write_config8(dev, 0x40, byte);

	pci_write_config32(dev, 0x60, 0x0000ffff); // LPC Memory hole start and end

// USB related
	pci_write_config8(dev, 0x90, 0x40);
	pci_write_config8(dev, 0x92, 0x06);
	pci_write_config8(dev, 0x9c, 0x7c); //PHY timinig register
	pci_write_config8(dev, 0xa4, 0x02); //mask reg - low/full speed func
	pci_write_config8(dev, 0xa5, 0x02); //mask reg - low/full speed func
	pci_write_config8(dev, 0xa6, 0x00); //mask reg - high speed func
	pci_write_config8(dev, 0xb4, 0x40);
}
