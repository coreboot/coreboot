/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#include <libpayload.h>
#include <pci.h>

static int find_on_bus(int bus, unsigned short vid, unsigned short did,
		       pcidev_t * dev)
{
	int devfn;
	unsigned int val;
	unsigned char hdr;

	for (devfn = 0; devfn < 0x100; devfn++) {
		pci_read_dword(bus, devfn, REG_VENDOR_ID, &val);

		if (val == 0xffffffff || val == 0x00000000 ||
		    val == 0x0000ffff || val == 0xffff0000)
			continue;

		if (val == ((did << 16) | vid)) {
			*dev = PCIDEV(bus, devfn);
			return 1;
		}

		pci_read_byte(bus, devfn, REG_HEADER_TYPE, &hdr);

		hdr &= 0x7F;

		if (hdr == HEADER_TYPE_BRIDGE || hdr == HEADER_TYPE_CARDBUS) {
			unsigned int busses;
			pci_read_dword(bus, devfn, REG_PRIMARY_BUS, &busses);
			if (find_on_bus((busses >> 8) & 0xFF, vid, did, dev))
				return 1;
		}
	}

	return 0;
}

void pci_read_dword(unsigned int bus, unsigned int devfn,
		    unsigned int reg, unsigned int *val)
{
	outl(PCI_ADDR(bus, devfn, reg), 0xCF8);
	*val = inl(0xCFC);
}

void pci_read_byte(unsigned int bus, unsigned int devfn,
		   unsigned int reg, unsigned char *val)
{
	outl(PCI_ADDR(bus, devfn, reg), 0xCF8);
	*val = inb(0xCFC + (reg & 3));
}

int pci_find_device(unsigned short vid, unsigned short did, pcidev_t * dev)
{
	return find_on_bus(0, vid, did, dev);
}

unsigned int pci_read_resource(pcidev_t dev, int bar)
{
	unsigned int val;
	pci_read_dword(PCIDEV_BUS(dev), PCIDEV_DEVFN(dev), 0x10 + (bar * 4),
		       &val);
	return val;
}
