#include <pci/pci.h>

#define CARD8 unsigned char
#define CARD16 unsigned short
#define CARD32 unsigned long

#define PCITAG struct pci_filter *
#define pciVideoPtr struct pci_dev *

extern int pciNumBuses;

int pciInit(void);
int pciExit(void);


PCITAG findPci(unsigned short bx);
CARD32 pciSlotBX(pciVideoPtr pvp);

void pciWriteLong(PCITAG tag, CARD32 idx, CARD32 data);
void pciWriteWord(PCITAG tag, CARD32 idx, CARD16 data);
void pciWriteByte(PCITAG tag, CARD32 idx, CARD8 data);

CARD32 pciReadLong(PCITAG tag, CARD32 idx);
CARD16 pciReadWord(PCITAG tag, CARD32 idx);
CARD8  pciReadByte(PCITAG tag, CARD32 idx);


