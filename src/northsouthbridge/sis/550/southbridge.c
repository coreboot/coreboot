
/*
 * Bootstrap code for the INTEL 
 * $Id$
 *
 */

#ifndef lint
static char rcsid[] =
"$Id$";
#endif


#include <printk.h>
#include <pciconf.h>
#include <subr.h>
#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>

#include <types.h>

#define BYTE sizeof(u8)
#define WORD sizeof(u16)
#define DBLE sizeof(u32)
#define IDE_REG_EXTENDED_OFFSET (0x200u)
#define IDE_REG_SECTOR_COUNT(base)  ((base) + 2u)
#define IDE_REG_CONTROL(base)  ((base) + IDE_REG_EXTENDED_OFFSET + 6u)
#define IDE_BASE1  (0x1F0u)  /* primary controller */
#define ASIZE(x) (sizeof(x)/sizeof((x)[0]))

typedef struct {
	u8 size;
	u8 regno;
	u32 andval;
	u32 orval;
} initreg_t;

static const initreg_t ide_init[] = {
	/*
	 *  BUS(0), DEVICE(0), FUNCTION(1): IDE Interface. Bridge
	 */
	/* SIZE REG   AND-MASK	OR-MASK   */
	{ WORD, 0x04, 0x00,	0x0007 },  /* Command Register. sets mem+ */
	{ WORD, 0x06, 0x00,	0x3000 },  /* Status reg, clear aborts. */
	{ WORD, 0x09, 0x0A,	0x00   },  /* Programming interface =  Compatible mode. */
	{ BYTE, 0x0D, 0x00,	0x10   },  /* Latency Timer */
	/* We let the pci enumerator assign these. */
	// { DBLE, 0x10, 0x0,         0x1F1 },   /* Primary channel base addr. */
	// { DBLE, 0x14, 0x0,         0x3F5 },   /* Primary channel control base addr. */
	// { DBLE, 0x18, 0x0,         0x171 },   /* Secondary channel base addr. */
	// { DBLE, 0x1C, 0x0,         0x375 },   /* Secondary channel control base addr. */
	// { DBLE, 0x20, 0x0,         0x4001 },  /* Control register base addr. */
	{ WORD, 0x2C, 0x00,	0x1039 },  /* Subsystem vendor ID. */
	{ WORD, 0x2E, 0x00,	0x5513 },  /* Subsystem ID. */
	{ BYTE, 0x3c, 0x00,	0x0e   },  /* reserved don't do this, sets irq 14 */
	{ BYTE, 0x40, 0x00,	0x00   },  /* Primary master data recovery time. 4 PCICLK */
	{ BYTE, 0x41, 0x00,	0xb0   },  /* Primary master data active time. UDMA Mode 2 */
	{ BYTE, 0x42, 0x00,	0x00   },  /* Primary slave data recovery time. */
	{ BYTE, 0x43, 0x00,	0x00   },  /* Primary slave data active time. */   
	{ BYTE, 0x44, 0x00,	0x00   },  /* Secondary master data recovery time. */
	{ BYTE, 0x45, 0x00,	0x00   },  /* Secondary master data active time. */  
	{ BYTE, 0x46, 0x00,	0x00   },  /* Secondary slave data recovery time. */ 
	{ BYTE, 0x47, 0x00,	0x00   },  /* Secondary slave data active time. */   
	{ BYTE, 0x48, 0x00,	0x05   },  /* Read/Write FIFO Threshold = 1/4 */
	{ BYTE, 0x4A, 0x40,	0xe2   },  /* Enable Bus Master, Fast postwrite, IDE 0 */
	{ BYTE, 0x4B, 0x00,	0x11   },  /* Enable Postwrite and Prefech for IDE0, Master */
	{ WORD, 0x4C, 0x00,	0x0200 },  /* Prefetch count of primary. */
	{ WORD, 0x4E, 0x00,	0x0200 },  /* Prefetch count of secondary. */
	{ BYTE, 0x52, 0xFF,	0x14   }   /* Miscelaneous control (Only Doc). */
};

void ide_fixup(void)
{
	volatile int delay;
	struct pci_dev *dev  = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_5513,
					       (void *) NULL); 
	int i;
	u8 val8;
	u16 val16;
	u32 val32;

	int initlen = ASIZE(ide_init);
	const initreg_t * inittab = ide_init;

	printk_info ("Entering the initregs process\n");

	for (i = 0; i < initlen; ++i) {
		u16 regno = inittab[i].regno;
		switch (inittab[i].size) {
		case BYTE:
			pci_read_config_byte(dev, regno, &val8);
			val8 = (val8 & inittab[i].andval) | inittab[i].orval;
			pci_write_config_byte(dev, regno, val8);
			break;
		case WORD:
			pci_read_config_word(dev, regno, &val16);
			val16 = (val16 & inittab[i].andval) | inittab[i].orval;
			pci_write_config_word(dev, regno, val16);
			break;
		case DBLE:
			pci_read_config_dword(dev, regno, &val32);
			val32 = (val32 & inittab[i].andval) | inittab[i].orval;
			break;
			
		default:
			outb_p(0xEE,0x80);
			while(1);
			break;
		}
	}
}

void keyboard_on()
{
	u8 regval;
	struct pci_dev *pcidev;
	void pc_keyboard_init(void);

	/* turn on sis630 keyboard/mouse controller */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		/* Register 0x47, Keyboard Controller */
		pci_read_config_byte(pcidev, 0x47, &regval);
		/* enable both integrated keyboard and PS/2 mouse controller */
		regval |= 0x0c;
		pci_write_config_byte(pcidev, 0x47, regval);
	}
	pc_keyboard_init();
}

void nvram_on()
{
	struct pci_dev *pcidev;

	/* turn on sis550 nvram. */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		u8 data;
		/* Enable FFF80000 to FFFFFFFF decode. You have to also enable
		   PCI Posted write for devices on sourthbridge */
		pci_write_config_byte(pcidev, 0x40, 0x33);
		/* Enable flash write in bit 6 and USB device in undocumented Bit 5 */
		pci_write_config_byte(pcidev, 0x45, 0x60);
	}

	/* turn off nvram shadow in 0xc0000 ~ 0xfffff, i.e. accessing segment C - F
	   is actually to the DRAM not NVRAM. For 512KB NVRAM case, this one should be
	   disabled */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_550, (void *)NULL);
	if (pcidev != NULL) {
		pci_write_config_word(pcidev, 0x70, 0xFFFF);
		pci_write_config_word(pcidev, 0x72, 0xFFFF);
		pci_write_config_word(pcidev, 0x74, 0xFFFF);
		pci_write_config_byte(pcidev, 0x76, 0xFF);
		printk_debug("Shadow memory disabled in SiS 550\n");

	}
}

/* serial_irq_fixup: Enable Serial Interrupt. Serial interrupt is the IRQ line from SiS 950
 *	LPC to Host Controller. Serial IRQ is neceressary for devices on SiS 950
 *	ie.e floppy, COM, LPT etc
 */
static void
serial_irq_fixedup(void)
{
	struct pci_dev *pcidev;

	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		/* enable Serial Interrupt (SIRQ) */
		pci_write_config_byte(pcidev, 0x70, 0x80); 
		// skip the SMI, inta, etc. stuff. 
		// enable IRQs 7 to 1
		pci_write_config_byte(pcidev, 0x72, 0xfd);
		// enable IRQs 15 to 8
		pci_write_config_byte(pcidev, 0x73, 0xff);
	}
}

static void
south_fixup(void)
{
	struct pci_dev *pcidev;

	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		u8 reg;
		/* disable ROM delay transaction */
		pci_read_config_byte(pcidev, 0x77, &reg);
		pci_write_config_byte(pcidev, 0x77, reg & 0xEF);

		/* IO address for CIR */
		pci_write_config_byte(pcidev, 0x4A, 0x11);
		pci_write_config_byte(pcidev, 0x4B, 0x03);
		/* IRQ for CIR */
		pci_write_config_byte(pcidev, 0x6C, 0x05);
	}

	pcidev = pci_find_device(PCI_VENDOR_ID_SI, 0x7001, (void *)NULL);
	if (pcidev != NULL) {
		pci_read_config_byte(pcidev, 0x04, 0x17);
	}
}

static void
acpi_fixup(void)
{
	struct pci_dev *pcidev;

	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		unsigned char val;
		unsigned short acpibase = 0xc000;

		// the following is to turn off software watchdogs. 
		// we co-op the address space from c000-cfff here. Temporarily. 
		// Later, we need a better way to do this. 
		// But since Linux doesn't even understand this yet, no issue. 
		// Set a base address for ACPI of 0xc000
		pci_write_config_word(pcidev, 0x74, acpibase);

		// now enable acpi
		pci_read_config_byte(pcidev, 0x40, &val);
		val |= 0x80;
		pci_write_config_byte(pcidev, 0x40, val);

		/* Disable Auto-Reset */
		val = inb(acpibase + 0x56);
		outw(val | 0x40, acpibase + 0x56);
		/* Disable Software Watchdog */
		outb(0, acpibase + 0x4b);
	} else {
		printk_emerg("Can't find south bridge!\n");
	}
	
}

// simple fixup (which we hope can leave soon) for the sis southbridge part
void
final_southbridge_fixup()
{
	struct pci_dev *pcidev;

	serial_irq_fixedup();
	acpi_fixup();
	ide_fixup();

	/* Delay transaction, experimental */
	south_fixup();

	printk_debug("Southbridge fixup done for SIS 503\n");
}
