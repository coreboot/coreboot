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

void
mainboard_fixup(void)
{
	struct mem_range *mem;
	unsigned long ramsize;
	int errors = 0;
	struct pci_dev *pcidev;

	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	pci_read_config_word(pcidev, 0x74, &acpibase);

	led_on(10);

	beep_on();
	/* FIXME this is not how sizeram is intened to be called! */
	mem = sizeram();
	ramsize = (mem[1].sizek + mem[1].basek) * 1024;
	if (ramcheck(0x00100000, ramsize, 20) != 0) {
		error("DRAM Test Error");
	}
	beep_off();

	led_off(10);
	led_on(9);
}

void
final_mainboard_fixup(void)
{
	void final_southbridge_fixup(void);

	printk_info("SiS 550 (and similar)...");

	final_southbridge_fixup();
}
