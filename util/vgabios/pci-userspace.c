/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <pci/pci.h>
#include "pci-userspace.h"

#define DEBUG_PCI 1

static struct pci_access *pacc;

int pci_initialize(void)
{
	struct pci_dev *dev;

	pacc = pci_alloc();

	pci_init(pacc);
	pci_scan_bus(pacc);
	for (dev = pacc->devices; dev; dev = dev->next) {
		pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES);
	}
	return 0;
}

int pci_exit(void)
{
	pci_cleanup(pacc);
	return 0;
}

u8 pci_read_config8(struct device *dev, unsigned int where)
{
	struct pci_dev *d;
	if ((d = pci_get_dev(pacc, 0, dev->busno, dev->slot, dev->func)))
		return pci_read_byte(d, where);
#ifdef DEBUG_PCI
	printf("PCI: device not found while read byte (%x:%x.%x)\n",
	       dev->busno, dev->slot, dev->func);
#endif
	return 0;
}

u16 pci_read_config16(struct device *dev, unsigned int where)
{
	struct pci_dev *d;
	if ((d = pci_get_dev(pacc, 0, dev->busno, dev->slot, dev->func)))
		return pci_read_word(d, where);
#ifdef DEBUG_PCI
	printf("PCI: device not found while read word (%x:%x.%x)\n",
	       dev->busno, dev->slot, dev->func);
#endif
	return 0;
}

u32 pci_read_config32(struct device *dev, unsigned int where)
{
	struct pci_dev *d;
	if ((d = pci_get_dev(pacc, 0, dev->busno, dev->slot, dev->func)))
		return pci_read_long(d, where);
#ifdef DEBUG_PCI
	printf("PCI: device not found while read dword (%x:%x.%x)\n",
	       dev->busno, dev->slot, dev->func);
#endif
	return 0;
}

void pci_write_config8(struct device *dev, unsigned int where, u8 val)
{
	struct pci_dev *d;
	if ((d = pci_get_dev(pacc, 0, dev->busno, dev->slot, dev->func)))
		pci_write_byte(d, where, val);
#ifdef DEBUG_PCI
	else
		printf("PCI: device not found while write byte (%x:%x.%x)\n",
		       dev->busno, dev->slot, dev->func);
#endif
}

void pci_write_config16(struct device *dev, unsigned int where, u16 val)
{
	struct pci_dev *d;
	if ((d = pci_get_dev(pacc, 0, dev->busno, dev->slot, dev->func)))
		pci_write_word(d, where, val);
#ifdef DEBUG_PCI
	else
		printf("PCI: device not found while write word (%x:%x.%x)\n",
		       dev->busno, dev->slot, dev->func);
#endif
}

void pci_write_config32(struct device *dev, unsigned int where, u32 val)
{
	struct pci_dev *d;
	if ((d = pci_get_dev(pacc, 0, dev->busno, dev->slot, dev->func)))
		pci_write_long(d, where, val);
#ifdef DEBUG_PCI
	else
		printf("PCI: device not found while write dword (%x:%x.%x)\n",
		       dev->busno, dev->slot, dev->func);
#endif
}
