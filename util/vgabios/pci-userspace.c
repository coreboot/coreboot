#include <stdio.h>
#include "pci-userspace.h"

struct pci_access *pacc;
struct pci_dev *dev;

struct pci_filter ltag;


int pciNumBuses=0;

int pciInit(void)
{
	pacc = pci_alloc();

	pci_init(pacc);
	pci_scan_bus(pacc);

	return 0;
}

int pciExit(void)
{
	pci_cleanup(pacc);
	return 0;
}
#if 0
pciReadLong()
{
	int c;
	pci_get_dev(struct pci_access *acc, int bus, int dev, int func);
	pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES);
	c=pci_read_dword(dev, regnum);
}
#endif

PCITAG findPci(unsigned short bx)
{
    PCITAG tag=&ltag;
    pciVideoPtr dev;

    int bus  = (bx >> 8) & 0xFF;
    int slot = (bx >> 3) & 0x1F;
    int func = bx & 0x7;

    tag->bus=bus; tag->slot=slot; tag->func=func;

    if (pci_get_dev(pacc, bus, slot, func))
        return tag;

    return NULL;
}

CARD32 pciSlotBX(pciVideoPtr pvp)
{
    return (pvp->bus << 8) | (pvp->dev << 3) | (pvp->func);
}

CARD8 pciReadByte(PCITAG tag, CARD32 idx)
{
	printf("pciReadByte: idx=%x\n",idx);
}

CARD16 pciReadWord(PCITAG tag, CARD32 idx)
{
        printf("pciReadWord: idx=%x\n",idx);
}

CARD32 pciReadLong(PCITAG tag, CARD32 idx)
{
        printf("pciReadWord: idx=%x\n",idx);
}


void pciWriteLong(PCITAG tag, CARD32 idx, CARD32 data)
{
	printf("pciWriteLong: idx=%x, data=%x\n",idx,data);
}

void pciWriteWord(PCITAG tag, CARD32 idx, CARD16 data)
{
	printf("pciWriteWord: idx=%x, data=%x\n",idx,data);
}


void pciWriteByte(PCITAG tag, CARD32 idx, CARD8 data)
{
	printf("pciWriteByte: idx=%x, data=%x\n",idx,data);
}

