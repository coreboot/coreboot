#include "pci.h"

typedef unsigned long pciaddr_t;
typedef u8 byte;
typedef u16 word;

struct pci_dev {
	struct pci_dev *next;	/* Next device in the chain */
	word bus;		/* Higher byte can select host bridges */
	byte dev, func;		/* Device and function */

	/* These fields are set by pci_fill_info() */
	int known_fields;	/* Set of info fields already known */
	word vendor_id, device_id;	/* Identity of the device */
	int irq;		/* IRQ number */
	pciaddr_t base_addr[6];	/* Base addresses */
	pciaddr_t size[6];	/* Region sizes */
	pciaddr_t rom_base_addr;	/* Expansion ROM base address */
	pciaddr_t rom_size;	/* Expansion ROM size */

	/* Fields used internally: */
	void *access;
	void *methods;
	byte *cache;		/* Cached information */
	int cache_len;
	int hdrtype;		/* Direct methods: header type */
	void *aux;		/* Auxillary data */
};


struct pci_filter {
	int bus, slot, func;	/* -1 = ANY */
	int vendor, device;
};


#define PCITAG struct pci_filter *
#define pciVideoPtr struct pci_dev *

extern int pciNumBuses;

int pciInit(void);
int pciExit(void);


PCITAG findPci(unsigned short bx);
u32 pciSlotBX(pciVideoPtr pvp);

void pciWriteLong(PCITAG tag, u32 idx, u32 data);
void pciWriteWord(PCITAG tag, u32 idx, u16 data);
void pciWriteByte(PCITAG tag, u32 idx, u8 data);

u32 pciReadLong(PCITAG tag, u32 idx);
u16 pciReadWord(PCITAG tag, u32 idx);
u8 pciReadByte(PCITAG tag, u32 idx);
