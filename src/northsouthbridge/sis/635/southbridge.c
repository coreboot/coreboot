/*
 * southbridge.c:        Southbridge Initialization For SiS 635
 *
 * Copyright 2002 International Business Machines
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *      1. SiS 635 Programing Guide
 */

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
	{ BYTE, 0x0D, 0x00,	0x80   },  /* Latency Timer */
	/* We let the pci enumerator assign these. */
	// { DBLE, 0x10, 0x0,         0x1F1 },   /* Primary channel base addr. */
	// { DBLE, 0x14, 0x0,         0x3F5 },   /* Primary channel control base addr. */
	// { DBLE, 0x18, 0x0,         0x171 },   /* Secondary channel base addr. */
	// { DBLE, 0x1C, 0x0,         0x375 },   /* Secondary channel control base addr. */
	// { DBLE, 0x20, 0x0,         0x4001 },  /* Control register base addr. */
	{ WORD, 0x2C, 0x00,	0x1039 },  /* Subsystem vendor ID. */
	{ WORD, 0x2E, 0x00,	0x5513 },  /* Subsystem ID. */
	{ BYTE, 0x3c, 0x00,	0x0e   },  /* reserved don't do this, sets irq 14 */
	{ BYTE, 0x40, 0x00,	0x44   },  /* Primary master data active/recovery time. PIO Mode 2 */
	{ BYTE, 0x41, 0x00,	0x85   },  /* Primary master UDMA cycle time. UDMA Mode 2 */
	{ BYTE, 0x42, 0x00,	0x00   },  /* Primary slave data recovery time. */
	{ BYTE, 0x43, 0x00,	0x00   },  /* Primary slave data active time. */   
	{ BYTE, 0x44, 0x00,	0x00   },  /* Secondary master data recovery time. */
	{ BYTE, 0x45, 0x00,	0x00   },  /* Secondary master data active time. */  
	{ BYTE, 0x46, 0x00,	0x00   },  /* Secondary slave data recovery time. */ 
	{ BYTE, 0x47, 0x00,	0x00   },  /* Secondary slave data active time. */   
	{ BYTE, 0x48, 0x00,	0x08   },  /* Read/Write FIFO Threshold = 1/4 */
	{ BYTE, 0x4A, 0x40,	0xe6   },  /* Enable Bus Master, Fast postwrite, IDE 0,1 */
	{ BYTE, 0x4B, 0x00,	0x11   },  /* Enable Postwrite and Prefech for IDE0, Master */
	{ WORD, 0x4C, 0x00,	0x0200 },  /* Prefetch count of primary. */
	{ WORD, 0x4E, 0x00,	0x0200 },  /* Prefetch count of secondary. */
	{ BYTE, 0x52, 0xFF,	0x14   }   /* Miscelaneous control (Only Doc). */
};

void ide_fixup(void)
{

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

	/* turn on sis635 keyboard/mouse controller */
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
	u8 tmp;

	/* turn on sis635 nvram. */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	
	if (pcidev != NULL) {
		printk_info("Enabling nvram\n");
		/* Enable FFF80000 to FFFFFFFF decode. You have to also enable
		   PCI Posted write for devices on sourthbridge */
		pci_read_config_byte(pcidev, 0x40, &tmp);
		tmp |= 0x3;
		pci_write_config_byte(pcidev, 0x40, tmp);
	}

#if !defined(STD_FLASH)
	/* turn off nvram shadow in 0xc0000 ~ 0xfffff, i.e. accessing segment C - F
	   is actually to the DRAM not NVRAM. For 512KB NVRAM case, this one should be
	   disabled */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_635, (void *)NULL);
	if (pcidev != NULL) {
		/* read cycle goes to System Memory */
		pci_write_config_word(pcidev, 0x70, 0x1fff);
		/* write cycle goest to System Memory */
		pci_write_config_word(pcidev, 0x72, 0x1fff);
		printk_debug("Shadow memory disabled in SiS 635\n");

	}
#endif
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
		pci_write_config_byte(pcidev, 0x69, 0x80); 
		pci_write_config_byte(pcidev, 0x67, 0x10);
	}
}

/* timer0_fixup: Fix up the Timer 0 of 8254 Programmable Timer
 *
 * The timer 0 is used to generate the 18.2 Hz, IRQ 0 timer event and used by system
 * to update date/time.
 *
 * FixME: Should be program timer 1 for DRAM refresh too ??
 */
static void
timer0_fixup(void)
{
	/* select Timer 0, 16 Bit Access, Mode 3, Binary */
	outb_p(0x43, 0x36);

	/* Load LSB, 0x00 */
	outb_p(0x40, 0x00);

	/* Load MSB, 0x00 */
	outb_p(0x40, 0x00);
}

/* rtc_fixup: Fix up the Real Time Clock
 *
 * The Real Time Clock updates the day/time information in the CMOS RAM every second.
 * The clock diveder has to be programmed correctly or the RTC will update the CMOS
 * in incorrect time interval (i.e. more or less then 1Sec in "wall clock").
 * 
 * FixME: Where does the CMOS stroe Y2K information and how does Linux handle it ??
 */
static void
rtc_fixup(void)
{
	volatile u8 dummy;
	u8 regval;
	struct pci_dev *isa_bridge;

	isa_bridge = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503,
				     (void *)NULL);
	if (isa_bridge != NULL) {
		/* Register 0x48, Enable internal RTC */
		pci_read_config_byte(isa_bridge, 0x48, &regval);
		pci_write_config_byte(isa_bridge, 0x48, regval | 0x10);
	}

	/* Select Normal Colok Divider, 978 us interrupt */
	outb_p(0x0A, 0x70);
	outb_p(0x26, 0x71);

	/* Select 24 HR time */
	outb_p(0x0B, 0x70);
	outb_p(0x02, 0x71);

	/* Clear Checksum Error */
	outb_p(0x0D, 0x70);
	dummy = inb_p(0x71);
}

static void
acpi_fixup(void)
{
	struct pci_dev *pcidev;

	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		unsigned char val;
		unsigned short acpibase = 0xc000, temp;

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
	timer0_fixup();
	rtc_fixup();
	nvram_on();
	acpi_fixup();
	ide_fixup();

	printk_debug("Southbridge fixup done for SIS 635\n");
}

