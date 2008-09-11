/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 coresystems GmbH
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

#include <config.h>
#include <usb/usb.h>
#include "uhci.h"
#include <usb/usbdisk.h>

/**
 * Initializes USB controller attached to PCI
 *
 * @param bus PCI bus number
 * @param dev PCI device id at bus
 * @param func function id of the controller
 */
int
usb_controller_initialize (int bus, int dev, int func)
{
	u32 class;
	u32 devclass;
	u32 prog_if;
	pcidev_t addr;
	u32 pciid;

	addr = PCI_DEV (bus, dev, func);
	class = pci_read_config32 (addr, 8);
	pciid = pci_read_config32 (addr, 0);

	devclass = class >> 16;
	prog_if = (class >> 8) & 0xff;

	/* enable busmaster */
#define PCI_COMMAND 4
#define PCI_COMMAND_MASTER 4
	pci_write_config32 (addr, PCI_COMMAND,
			    pci_read_config32 (addr,
					       PCI_COMMAND) |
			    PCI_COMMAND_MASTER);

	if (devclass == 0xc03) {
		printf ("%02x:%02x.%x %04x:%04x.%d ", 0, dev, func,
			pciid >> 16, pciid & 0xFFFF, func);
		if (prog_if == 0) {
			printf ("UHCI controller\n");
#ifdef CONFIG_USB_UHCI
			uhci_init (addr);
			usb_poll ();
			usb_poll ();
#else
			printf ("Not supported.\n");
#endif
		}
		if (prog_if == 0x10) {
			printf ("OHCI controller\n");
#ifdef CONFIG_USB_OHCI
			// ohci_init(addr);
#else
			printf ("Not supported.\n");
#endif

		}
		if (prog_if == 0x20) {
			printf ("EHCI controller\n");
#ifdef CONFIG_USB_EHCI
			// ehci_init(addr);
#else
			printf ("Not supported.\n");
#endif

		}
	}

	return 0;
}

/**
 * Initialize all USB controllers attached to PCI.
 */
int
usb_initialize (void)
{
	int bus, dev, func;
	for (bus = 0; bus < 256; bus++)
		for (dev = 0; dev < 32; dev++)
			for (func = 0; func < 8; func++)
				usb_controller_initialize (bus, dev, func);
	return 0;
}

int
usb_exit (void)
{
	return 0;
}
