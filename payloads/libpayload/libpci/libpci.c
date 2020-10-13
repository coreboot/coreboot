/*
 *
 * Copyright (C) 2010 coresystems GmbH
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
#include <pci/pci.h>

/* libpci shim */
static pcidev_t libpci_to_lb(struct pci_dev *dev)
{
	return PCI_DEV(dev->bus, dev->dev, dev->func);
}

/* libpci interface */
u8 pci_read_byte(struct pci_dev *dev, int pos)
{
	return pci_read_config8(libpci_to_lb(dev), (uint16_t)pos);
}

u16 pci_read_word(struct pci_dev *dev, int pos)
{
	return pci_read_config16(libpci_to_lb(dev), (uint16_t)pos);
}

u32 pci_read_long(struct pci_dev *dev, int pos)
{
	return pci_read_config32(libpci_to_lb(dev), (uint16_t)pos);
}

int pci_write_byte(struct pci_dev *dev, int pos, u8 data)
{
	pci_write_config8(libpci_to_lb(dev), (uint16_t)pos, data);
	return 1; /* success */
}

int pci_write_word(struct pci_dev *dev, int pos, u16 data)
{
	pci_write_config16(libpci_to_lb(dev), (uint16_t)pos, data);
	return 1; /* success */
}

int pci_write_long(struct pci_dev *dev, int pos, u32 data)
{
	pci_write_config32(libpci_to_lb(dev), (uint16_t)pos, data);
	return 1; /* success */
}

struct pci_access *pci_alloc(void)
{
	return malloc(sizeof(struct pci_access));
}

void pci_init(struct pci_access *pacc)
{
	memset(pacc, 0, sizeof(*pacc));
}

void pci_cleanup(__attribute__((unused)) struct pci_access *pacc)
{
}

void pci_filter_init(struct pci_access* pacc, struct pci_filter* pf)
{
	pf->domain = -1;
	pf->bus = -1;
	pf->dev = -1;
	pf->func = -1;
	pf->vendor = -1;
	pf->device = -1;
}

static char invalid_pci_device_string[] = "invalid pci device string";

/* parse domain:bus:dev.func (with all components but "dev" optional)
 * into filter.
 * Returns NULL on success, a string pointer to the error message otherwise.
 */
char *pci_filter_parse_slot(struct pci_filter* filter, const char* id)
{
	char *endptr;

	filter->func = filter->dev = filter->bus = filter->domain = -1;

	char *funcp = strrchr(id, '.');
	if (funcp) {
		filter->func = strtol(funcp+1, &endptr, 0);
		if (endptr[0] != '\0') return invalid_pci_device_string;
	}

	char *devp = strrchr(id, ':');
	if (!devp) {
		filter->dev = strtol(id, &endptr, 0);
	} else {
		filter->dev = strtol(devp+1, &endptr, 0);
	}
	if (endptr != funcp) return invalid_pci_device_string;
	if (!devp) return NULL;

	char *busp = strchr(id, ':');
	if (busp == devp) {
		filter->bus = strtol(id, &endptr, 0);
	} else {
		filter->bus = strtol(busp+1, &endptr, 0);
	}
	if (endptr != funcp) return invalid_pci_device_string;
	if (busp == devp) return NULL;

	filter->domain = strtol(id, &endptr, 0);
	if (endptr != busp) return invalid_pci_device_string;

	return NULL;
}

int pci_filter_match(struct pci_filter* pf, struct pci_dev* dev)
{
	if ((pf->domain > -1) && (pf->domain != dev->domain))
		return 0;
	if ((pf->bus > -1) && (pf->bus != dev->bus))
		return 0;
	if ((pf->dev > -1) && (pf->dev != dev->dev))
		return 0;
	if ((pf->func > -1) && (pf->func != dev->func))
		return 0;
	if ((pf->vendor > -1) && (pf->vendor != dev->vendor_id))
		return 0;
	if ((pf->device > -1) && (pf->device != dev->device_id))
		return 0;
	return 1;
}

static struct pci_dev *pci_scan_single_bus(struct pci_dev *dev, uint8_t bus)
{
	int devfn;
	u32 val;
	unsigned char hdr;

	for (devfn = 0; devfn < 0x100; devfn++) {
		uint8_t func = devfn & 0x7;
		uint8_t slot = (devfn >> 3) & 0x1f;

		val = pci_read_config32(PCI_DEV(bus, slot, func),
					REG_VENDOR_ID);

		if (val == 0xffffffff || val == 0x00000000 ||
		    val == 0x0000ffff || val == 0xffff0000)
			continue;

		dev->next = malloc(sizeof(struct pci_dev));
		dev = dev->next;
		dev->domain = 0;
		dev->bus = bus;
		dev->dev = slot;
		dev->func = func;
		dev->vendor_id = val & 0xffff;
		dev->device_id = (uint16_t)(val >> 16);
		dev->device_class = pci_read_config16(PCI_DEV(bus, slot, func), PCI_CLASS_DEVICE);
		dev->next = 0;

		hdr = pci_read_config8(PCI_DEV(bus, slot, func),
				       REG_HEADER_TYPE);
		hdr &= 0x7F;

		if (hdr == HEADER_TYPE_BRIDGE || hdr == HEADER_TYPE_CARDBUS) {
			uint8_t busses;
			busses = (uint8_t)(pci_read_config32(
				PCI_DEV(bus, slot, func),
				REG_PRIMARY_BUS) >> 8);

			/* Avoid recursion if the new bus is the same as
			 * the old bus (insert lame The Who joke here) */

			if (busses != bus)
				dev = pci_scan_single_bus(dev, busses);
		}
	}

	return dev;
}

void pci_scan_bus(struct pci_access* pacc)
{
	struct pci_dev rootdev;
	pci_scan_single_bus(&rootdev, 0);
	pacc->devices = rootdev.next;
}

struct pci_dev *pci_get_dev(struct pci_access* pacc, u16 domain, u8 bus, u8 dev, u8 func)
{
	struct pci_dev *cur = malloc(sizeof(*cur));
	cur->domain = domain;
	cur->bus = bus;
	cur->dev = dev;
	cur->func = func;
	return cur;
}

void pci_free_dev(struct pci_dev *const dev)
{
	free(dev);
}
