#include <mem.h>
#include <part/sizeram.h>
#include <printk.h>
#include <arch/io.h>
#include <pciconf.h>
#include <subr.h>
#include <pci.h>
#include <pci_ids.h>

static unsigned short acpibase;

static void
beep_on(void)
{
	/* Timer 2, Mode 3, perodic square ware */
	outb(0xb6, 0x43);

	/* frequence = 440 Hz */
	outb(0x97, 0x42);
	outb(0x0a, 0x42);

	/* Enable PC speaker gate */
	outb(0x03, 0x61);
}

static void
beep_off(void)
{
	/* Disable PC speaker gate */
	outb(0x00, 0x61);
}

static void
led_on(int led)
{
	unsigned enable, level;

	enable = inw(acpibase + 0x64);
	enable &= ~(1 << led);
	outw(enable, acpibase + 0x64);

	level = inw(acpibase + 0x66);
	level &= ~(1 << led);
	outb(level, acpibase + 0x66);
}

static void
led_off(int led)
{
	unsigned enable, level;

	enable = inw(acpibase + 0x64);
	enable |= (1 << led);
	outw(enable, acpibase + 0x64);

	level = inw(acpibase + 0x66);
	level |= (1 << led);
	outb(level, acpibase + 0x66);
}

#define BASE 0x2e
#define BASE1 0x4e

void
winbond_83697_init(void)
{
	printk_info("Setting 2E ports.......\n");

        outb(0x87, BASE);
        outb(0x87, BASE);
	outb(0x2c, BASE);
	outb(0x55, BASE+1);

	/* URA */
	outb(0x07, BASE);
	outb(0x02, BASE+1);
	outb(0x30, BASE);
	outb(0x01, BASE+1);
	outb(0x60, BASE);
	outb(0x03, BASE+1);
	outb(0x61, BASE);
	outb(0xe8, BASE+1);
	outb(0x70, BASE);
	outb(0x05, BASE+1);
	outb(0xf0, BASE);
	outb(0x40, BASE+1);

/* URB */
	outb(0x07, BASE);
        outb(0x03, BASE+1);
        outb(0x30, BASE);
        outb(0x01, BASE+1);
        outb(0x60, BASE);
        outb(0x02, BASE+1);
        outb(0x61, BASE);
        outb(0xe8, BASE+1);
        outb(0x70, BASE);
        outb(0x06, BASE+1);
        outb(0xf0, BASE);
        outb(0x40, BASE+1); 
        outb(0xf1, BASE);
        outb(0x00, BASE+1); 

/*URC */
        outb(0x07, BASE);
        outb(0x0d, BASE+1);
        outb(0x30, BASE);
        outb(0x01, BASE+1);
	outb(0x60, BASE);
	outb(0x03, BASE+1);
	outb(0x61, BASE);
	outb(0xf8, BASE+1);
        outb(0x70, BASE);
        outb(0x03, BASE+1); 
        outb(0xf0, BASE);
        outb(0x40, BASE+1); 
/*URD*/
        outb(0x07, BASE);
        outb(0x0e, BASE+1);
        outb(0x30, BASE);
        outb(0x01, BASE+1); 
        outb(0x60, BASE);
        outb(0x02, BASE+1);
        outb(0x61, BASE);
        outb(0xf8, BASE+1);
        outb(0x62, BASE);
        outb(0x00, BASE+1);
        outb(0x63, BASE);
        outb(0x00, BASE+1);
        outb(0x70, BASE);
        outb(0x04, BASE+1); 
        outb(0xf0, BASE);
        outb(0x40, BASE+1); 

	outb(0xaa,BASE);

	/*==================4E================*/
	printk_info("Setting 4E ports.......\n");
	/* URE */
        // outb(0x40,0x26);
        outb(0x87, BASE1);
        outb(0x87, BASE1);
	outb(0x2c, BASE1);
	outb(0x55, BASE1+1);

        outb(0x07, BASE1);
        outb(0x02, BASE1+1);
        outb(0x30, BASE1);
        outb(0x01, BASE1+1);
        outb(0x60, BASE1);
        outb(0x03, BASE1+1);
        outb(0x61, BASE1);
        outb(0x18, BASE1+1);
        outb(0x70, BASE1);
        outb(0x05, BASE1+1);
        outb(0xf0, BASE1);
        outb(0x40, BASE1+1); 
  /* URF */
        outb(0x07, BASE1);
        outb(0x03, BASE1+1);
        outb(0x30, BASE1);
        outb(0x01, BASE1+1);
        outb(0x60, BASE1);
        outb(0x03, BASE1+1);
        outb(0x61, BASE1);
        outb(0x28, BASE1+1);
        outb(0x70, BASE1);
        outb(0x06, BASE1+1);
        outb(0xf0, BASE1);
        outb(0x40, BASE1+1);
        outb(0xf1, BASE1);
        outb(0x00, BASE1+1); 
 /*URG */
        outb(0x07, BASE1);
        outb(0x0d, BASE1+1);
        outb(0x30, BASE1);
        outb(0x01, BASE1+1);
        outb(0x60, BASE1);
        outb(0x02, BASE1+1);
        outb(0x61, BASE1);
        outb(0x18, BASE1+1);
        outb(0x70, BASE1);
        outb(0x03, BASE1+1); 
        outb(0xf0, BASE1);
        outb(0x40, BASE1+1);

 /*URH*/
        outb(0x07, BASE1);
        outb(0x0e, BASE1+1);
        outb(0x30, BASE1);
        outb(0x01, BASE1+1); 
        outb(0x60, BASE1);
        outb(0x02, BASE1+1);
        outb(0x61, BASE1);
        outb(0x28, BASE1+1);
        outb(0x62, BASE1);
        outb(0x00, BASE1+1);
        outb(0x63, BASE1);
        outb(0x00, BASE1+1);
        outb(0x70, BASE1);
        outb(0x04, BASE1+1); 
        outb(0xf0, BASE1);
        outb(0x40, BASE1+1); 

	outb(0xaa,BASE1);
	printk_info("completed setting ports\n");
}

void
mainboard_fixup(void)
{
	struct mem_range *mem;
	unsigned long ramsize;
	int errors = 0;
	struct  pci_dev *pcidev;
	u8 data;

	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	pci_read_config_word(pcidev, 0x74, &acpibase);
	/* Flash can not be flashed, enable USB device in undocumented Bit 6 */
	pci_write_config_byte(pcidev, 0x45, 0xA0);

	led_on(10);

	/* FIXME this is not how sizeram is intened to be called! */
	mem = sizeram();
	ramsize = (mem[1].sizek + mem[1].basek) * 1024;
	if (ramcheck(0x00100000, ramsize, 20) != 0) {
		beep_on();
		error("DRAM Test Error");
	}

	led_off(10);
	led_on(9);
}

void
final_mainboard_fixup(void)
{
	void final_southbridge_fixup(void);

	printk_info("SiS 550 (and similar)...");

	//winbond_83697_init();

	final_southbridge_fixup();
}

