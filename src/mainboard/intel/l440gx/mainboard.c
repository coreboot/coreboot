#include <printk.h>
#include <pci.h>

#include <cpu/p5/io.h>

// this needs to be moved about a bit to northbridge.c etc.


void mainboard_fixup()
{
	struct pci_dev *pm_pcidev, *host_bridge_pcidev, *nic_pcidev;
	unsigned smbus_io, pm_io;
	unsigned int i, j;
	printk_debug("intel_mainboard_fixup()\n");

#if 1
	pm_pcidev = pci_find_device(0x8086, 0x7113, 0);
	nic_pcidev = pci_find_device(0x8086, 0x1229, 0);
	host_bridge_pcidev = pci_find_slot(0, PCI_DEVFN(0,0));
#endif
#if 1
	pci_write_config_byte(nic_pcidev, 0x3c, 21);
#endif
#if 1
	{
	u8 byte;
	u16 word;
	u32 dword;
	for(i = 0; i < 8; i++) {
		pci_read_config_byte(host_bridge_pcidev, 0x60 +i, &byte);
		printk_debug("DRB[i] = 0x%02x\n", byte);
	}
	pci_read_config_byte(host_bridge_pcidev, 0x57, &byte);
	printk_debug("DRAMC = 0x%02x\n", byte);
	pci_read_config_byte(host_bridge_pcidev, 0x74, &byte);
	printk_debug("RPS = 0x%02x\n", byte);
	pci_read_config_word(host_bridge_pcidev, 0x78, &word);
	printk_debug("PGPOL = 0x%04x\n", word);
	pci_read_config_dword(host_bridge_pcidev, 0x50, &dword);
	printk_debug("NBXCFG = 0x%04x\n", dword);
	}
	
#endif
#if 1

	printk_debug("Reset Control Register\n");
	outb(((inb(0xcf9) & 0x04) | 0x02), 0xcf9);

	printk_debug("port 92\n");
	outb((inb(0x92) & 0xFE), 0x92);

	printk_debug("Disable Nmi\n");
	outb(0, 0x70);

	printk_debug("enabling smbus\n");
#if 0
	smbus_io = NewPciIo(0x10);
#else
	smbus_io = 0xFFF0;
#endif
	pci_write_config_dword(pm_pcidev, 0x90, smbus_io | 1); /* iobase addr */
	pci_write_config_byte(pm_pcidev, 0xd2,  (0x4 << 1) | 1); /* smbus enable */
	pci_write_config_word(pm_pcidev, 0x4, 1); /* iospace enable */


	printk_debug("enable pm functions\n");
#if 0
	pm_io = NewPciIo(0x40);
#else
	pm_io = 0xFF80;
#endif
	pci_write_config_dword(pm_pcidev, 0x40, pm_io | 1); /* iobase addr */
	pci_write_config_byte(pm_pcidev, 0x80, 1);  /* enable pm io address */

	printk_debug("disabling smi\n");
	/* GLBEN */
	outw(0x00,      pm_io + 0x20);
	/* GLBCTL */
	outl((1 << 24), pm_io + 0x28);
	
	printk_debug("Disable more pm stuff\n");
	/* PMEN */
	outw((1 << 8), pm_io + 0x02);
	/* PMCNTRL */
	outw((0x5 << 10) , pm_io + 0x4);
	/* PMTMR */
	outl(0, pm_io + 0x08);
	/* GPEN */
	outw(0, pm_io + 0x0e);
	/* PCNTRL */
	outl(0, pm_io + 0x10);
	/* GLBSTS */
	/* DEVSTS */
	/* GLBEN see above */
	/* GLBCTL see above */
	/* DEVCTL */
	outl(0, pm_io + 0x2c);
	/* GPIREG */
	/* GPOREG */
	
	printk_debug("Set the subsystem vendor id\n");
	pci_write_config_word(host_bridge_pcidev, 0x2c, 0x8086);

	printk_debug("Disabling pm stuff in pci config space\n");

#define MAX_COUNTERS
#ifndef MAX_COUNTERS
	/* counters to 0 */
#define WHICH_COUNTERS(min,max) min
#else
	/* max out the counters */
#define WHICH_COUNTERS(min,max) max
#endif

	/* CNTA */
	pci_write_config_dword(pm_pcidev, 0x44, WHICH_COUNTERS(0x004000f0, 0xFFFFFFFF));

	/* CNTB */
	pci_write_config_dword(pm_pcidev, 0x48, WHICH_COUNTERS(0x00000400, 0x007c07df));

	/* GPICTL */
	pci_write_config_dword(pm_pcidev, 0x4c, 0);

	/* DEVRESD */
	pci_write_config_dword(pm_pcidev, 0x50, 0);
	
	/* DEVACTA */
	pci_write_config_dword(pm_pcidev, 0x54, 0);

	/* DEVACTB */
	pci_write_config_dword(pm_pcidev, 0x58, 0);

	/* DEVRESA */
	pci_write_config_dword(pm_pcidev, 0x5c, 0);

	/* DEVRESB */
	pci_write_config_dword(pm_pcidev, 0x60, 0);

	/* DEVRESC */
	pci_write_config_dword(pm_pcidev, 0x64, 0); /* might kill the serial port */

	/* DEVRESE */
	pci_write_config_dword(pm_pcidev, 0x68, 0);

	/* DEVRESF */
	pci_write_config_dword(pm_pcidev, 0x6c, 0);

	/* DEVRESG */
	pci_write_config_dword(pm_pcidev, 0x70, 0);

	/* DEVRESH */
	pci_write_config_dword(pm_pcidev, 0x74, 0);

	/* DEVRESI */
	pci_write_config_dword(pm_pcidev, 0x78, 0);

	/* DEVRESJ */
	pci_write_config_dword(pm_pcidev, 0x7c, 0);

#endif

#if 1
	
	/* Verify that smi is disabled */
	printk_debug("Testing SMI\r\n");
	{
	u32 value;
	pci_read_config_dword(pm_pcidev, 0x58, &value);
	pci_write_config_dword(pm_pcidev, 0x58, value | (1 << 25));
	}
	outb(inb(0xb2), 0xb2);
	printk_debug("SMI disabled\r\n");
#endif
#if 0
	
	for(i = 0; i < 255; i++) {
		printk_debug("%08x\r\n", i);
		__rdtsc_delay2(1000000000UL, pm_io);
	}
#endif
}
