
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
#include <northsouthbridge/sis/630/param.h>
#include <cpu/p5/io.h>

void keyboard_on()
{
	u8 regval;
	struct pci_dev *pcidev;

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

	/* turn on sis630 nvram. */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		/* Enable FFF80000 to FFFFFFFF decode. You have to also enable
		   PCI Posted write for devices on sourthbridge */
		pci_write_config_byte(pcidev, 0x40, 0x33);
		/* Flash can be flashed */
		pci_write_config_byte(pcidev, 0x45, 0x40);
		DBG("Enabled in SIS 503 regs 0x40 and 0x45\n");

	}
	DBG("Now try to turn off shadow\n");

#ifdef USE_DOC_MIL
	/* turn off nvram shadow in 0xc0000 ~ 0xfffff, i.e. accessing segment C - F
	   is actually to the DRAM not NVRAM. For 512KB NVRAM case, this one should be
	   disabled */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_630, (void *)NULL);
	DBG("device for SiS 630 is 0x%x\n", pcidev);
	if (pcidev != NULL) {
		/* read cycle goes to System Memory */
		pci_write_config_word(pcidev, 0x70, 0x1fff);
		/* write cycle goest to System Memory */
		pci_write_config_word(pcidev, 0x72, 0x1fff);
		DBG("Shadow memory disabled in SiS 630\n");

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
		pci_write_config_byte(pcidev, 0x70, 0x80); 
		// skip the SMI, inta, etc. stuff. 
		// enable IRQs 7 to 1
		pci_write_config_byte(pcidev, 0x72, 0xfd);
		// enable IRQs 15 to 8
		pci_write_config_byte(pcidev, 0x73, 0xff);
	}
}

/* apc_fixup: Fix up the Mux-ed GPIO Lines controlled by APC registers 
 *
 * For SiS630A/B Mainboards, the MAC address of the internal SiS900 is stored in EEPROM
 * on the board. The EEPROM interface lines are muxed with GPIO pins and are selected by
 * some APC registers.
 */
static void
apc_fixup(void)
{
	u8 regval, revision;
	struct pci_dev *host_bridge, *isa_bridge;

	host_bridge = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_630,
				      (void *) NULL);
	if (host_bridge == NULL)
		return;
	pci_read_config_byte(host_bridge, PCI_CLASS_REVISION, &revision);

	isa_bridge = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503,
				     (void *)NULL);
	if (isa_bridge != NULL) {
		/* Register 0x48, select APC control registers */
		pci_read_config_byte(isa_bridge, 0x48, &regval);
		pci_write_config_byte(isa_bridge, 0x48, regval | 0x40);

		/* Enable MAC Serial ROM Autoload */
		outb(0x01, 0x70);
		regval = inb(0x71);
		outb(regval | 0x80, 0x71);

#ifndef SIS630S
		/* Select Programmable LED0 and EEPROM Data Out for GPIO8 and GPIO3 */
		outb(0x02, 0x70);
		regval = inb(0x71);
		outb(regval | 0x19, 0x71);
#else /* SIS630S */
		/* Select Programmable MDC/RXCLK and RXEXR for GPIO[9:8] and GPIO3 */
		outb(0x02, 0x70);
		regval = inb(0x71);
		outb(regval | 0x09, 0x71);
#endif

		/* select Keyboard/Mouse function for GPIO Pin [14:10] */
		outb(0x02, 0x70);
		regval = inb(0x71);
		outb(regval | 0x40, 0x71);

		/* Register 0x48, select RTC registers */
		pci_read_config_byte(isa_bridge, 0x48, &regval);
		pci_write_config_byte(isa_bridge, 0x48, regval & ~0x40);
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
	outb_p(0x26, 0x71);

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
		pci_read_config_word(pcidev, 0x74, &temp);

		DBG("acpibase was 0x%x\n", temp);
		pci_write_config_word(pcidev, 0x74, acpibase);
		pci_read_config_word(pcidev, 0x74, &temp);
		DBG("acpibase is 0x%x\n", temp);

		// now enable acpi
		pci_read_config_byte(pcidev, 0x40, &val);
		DBG("acpi enable reg was 0x%x\n", val);
		val |= 0x80;
		pci_write_config_byte(pcidev, 0x40, val);
		pci_read_config_byte(pcidev, 0x40, &val);
		DBG("acpi enable reg after set is 0x%x\n", val);
		DBG("acpi status: word at 0x56 is 0x%x\n",
		       inw(acpibase+0x56));
		DBG("acpi status: byte at 0x4b is 0x%x\n", 
		       inb(acpibase + 0x4b));

		// now that it's on, get in there and call off the dogs. 
		// that's the recommended thing to do if MD40 iso on. 
		outw(0, acpibase + 0x56);
		// does this help too? 
		outb(0, acpibase + 0x4b);
		// ah ha! have to SET, NOT CLEAR!
		outb(0x40, acpibase + 0x56);
		DBG("acpibase + 0x56 is 0x%x\n", 
		       inb(acpibase+0x56));
		val &= (~0x80);
		pci_write_config_byte(pcidev, 0x40, val);
		pci_read_config_byte(pcidev, 0x40, &val);
		DBG("acpi disable reg after set is 0x%x\n", val);
	} else {
		printk(KERN_EMERG "Can't find south bridge!\n");
	}
	
}

// simple fixup (which we hope can leave soon) for the sis southbridge part
void
final_southbridge_fixup()
{
#ifdef OLD_KERNEL_HACK
	struct pci_dev *pcidev;

	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		printk("Remapping IRQ on southbridge for OLD_KERNEL_HACK\n");
               // remap IRQ for PCI -- this is exactly what the BIOS does now.
               pci_write_config_byte(pcidev, 0x42, 0xa);
               pci_write_config_byte(pcidev, 0x43, 0xb);
               pci_write_config_byte(pcidev, 0x44, 0xc);
	}
	// ethernet fixup. This should all work, and doesn't, yet. 
	// so we hack it for now. 
	// need a manifest constant for the enet device. 
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, 0x0900, (void *)NULL);
	if (pcidev != NULL) {
		u32 bar0 = 0xb001;
		// set the BAR 0 to 0xb000. Safe, high value, known good. 
		// pci config set doesn't work for reasons we don't understand. 
		pci_write_config_dword(pcidev, PCI_BASE_ADDRESS_0, bar0);

		// Make sure bus mastering is on. The tried-and-true probe in linuxpci.c 
		// doesn't set this for some reason. 
		pci_write_config_byte(pcidev, PCI_COMMAND, 
				      PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

		// set the interrupt to 'b'
		pci_write_config_byte(pcidev, PCI_INTERRUPT_LINE, 0xb);
	} else {
		printk(KERN_ERR "Can't find ethernet interface\n");
	}
#endif /* OLD_KERNEL_HACK */

	timer0_fixup();
	rtc_fixup();
	apc_fixup();
	serial_irq_fixedup();
	acpi_fixup();

	DBG("Southbridge fixup done for SIS 503\n");
}
