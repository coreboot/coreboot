/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008-2010 coresystems GmbH
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

#include <libpayload-config.h>
#include <usb/usb.h>
#include "uhci.h"
#include "ohci.h"
#include "ehci.h"
#include "xhci.h"
#include <usb/usbdisk.h>

/**
 * Initializes USB controller attached to PCI
 *
 * @param bus PCI bus number
 * @param dev PCI device id at bus
 * @param func function id of the controller
 */
int usb_controller_initialize(int bus, int dev, int func)
{
	u32 class;
	u32 devclass;
	u32 prog_if;
	pcidev_t addr;
	u32 pciid;

	addr = PCI_DEV (bus, dev, func);
	class = pci_read_config32(addr, 8);
	pciid = pci_read_config32(addr, 0);

	devclass = class >> 16;
	prog_if = (class >> 8) & 0xff;

	/* enable busmaster */
#define PCI_COMMAND 4
#define PCI_COMMAND_MASTER 4
	if (devclass == 0xc03) {
		u32 pci_command;

		pci_command = pci_read_config32(addr, PCI_COMMAND);
		pci_command |= PCI_COMMAND_MASTER;
		pci_write_config32(addr, PCI_COMMAND, pci_command);

		printf("%02x:%02x.%x %04x:%04x.%d ", bus, dev, func,
			pciid >> 16, pciid & 0xFFFF, func);
		switch (prog_if) {
		case 0x00:
#ifdef CONFIG_USB_UHCI
			printf("UHCI controller\n");
			uhci_init (addr);
#else
			printf("UHCI controller (not supported)\n");
#endif
			break;

		case 0x10:
#ifdef CONFIG_USB_OHCI
			printf("OHCI controller\n");
			ohci_init(addr);
#else
			printf("OHCI controller (not supported)\n");
#endif
			break;

		case 0x20:
#ifdef CONFIG_USB_EHCI
			printf("EHCI controller\n");
			ehci_init(addr);
#else
			printf("EHCI controller (not supported)\n");
#endif
			break;

		case 0x30:
#ifdef CONFIG_USB_XHCI
			printf("xHCI controller\n");
			xhci_init(addr);
#else
			printf("xHCI controller (not supported)\n");
#endif
			break;

		default:
			printf("unknown controller %x not supported\n",
			       prog_if);
			break;
		}
	}

	return 0;
}

static void usb_scan_pci_bus(int bus)
{
	int dev, func;
	for (dev = 0; dev < 32; dev++) {
		u8 header_type;
		pcidev_t addr = PCI_DEV(bus, dev, 0);
		/* Check if there's a device here at all. */
		if (pci_read_config32(addr, REG_VENDOR_ID) == 0xffff)
			continue;
		header_type = pci_read_config8(addr, REG_HEADER_TYPE);
		/* If this is a bridge, scan the bus on the other side. */
		if ((header_type & ~HEADER_TYPE_MULTIFUNCTION) ==
				HEADER_TYPE_BRIDGE) {
			int sub_bus =
				pci_read_config8(addr, REG_SECONDARY_BUS);
			usb_scan_pci_bus(sub_bus);
			continue;
		}
		/*
		 * EHCI is defined by standards to be at a higher function
		 * than the USB1 controllers. We don't want to init USB1 +
		 * devices just to "steal" those for USB2, so make sure USB2
		 * comes first.
		 */
		/* Check for a multifunction device. */
		if (header_type & HEADER_TYPE_MULTIFUNCTION)
			for (func = 7; func > 0; func--)
				usb_controller_initialize(bus, dev, func);
		/* Initialize function 0. */
		usb_controller_initialize(bus, dev, 0);
	}
}

/**
 * Initialize all USB controllers attached to PCI.
 */
int usb_initialize(void)
{
	usb_scan_pci_bus(0);
	usb_poll();
	return 0;
}
