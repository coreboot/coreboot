#include <printk.h>
#include <pciconf.h>
#include <subr.h>
#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>


#include "nic.h"
#include "sis900.h"
#include "etherboot.h"

struct nic nic;
unsigned char eth_addr[6];

int acpi_base;

void init_HR_TIMER(void)
{
	int j;

	outl(0x80000840, 0x0cf8);
	acpi_base = inb(0x0cfc) | 0x80;
	outb(acpi_base, 0x0cfc);

	outl(0x80000874, 0x0cf8);
	acpi_base = inw(0x0cfc);

	printk("init_HR_TIMER: acpi_base = %04x\n", acpi_base);

	j = inw(acpi_base + 0x56) | 0x02;	// HR_TMR control
	outw(j, acpi_base + 0x56);		// activate HR_TMR
}

inline unsigned long currticks(void)
{
	unsigned long int j=0;

	j = inl(acpi_base + 0x4c);
	j /= 55555;	// HR_TMR runs at 1MHz, etherboot drivers expect 18.2Hz, but this will be close enough.

	return(j);
}

int test_display_tftp_callback(char *data, int block, int length, int eof)
{
	int i;

#ifdef DEBUG	
	printk("RECD block %u, length = %u:\n",block, length);
#endif
	for(i=0; i<length; i++) 
		if(!data[i])
			printk("|");
		else
			printk("%c",data[i]);
#ifdef DEBUG
	if(eof) 
		printk("\nEND OF FILE\n");
	
	printk("====================\n");
#endif
	return(-1);
}

char buffer[1024];

void netboot_init() 
{
	struct pci_dev *pcidev;
	struct nic *result;
	unsigned short iobase = 0x0b001;	// address = 0x0b00, I/O port space
	int rc=512;
	int i;

	nic.node_addr = eth_addr;
	/* first, init the sis900 ethernet! */

	pcidev = pci_find_device(PCI_VENDOR_ID_SI, 0x0900, (void *)NULL);
	pci_write_config_byte(pcidev, PCI_BASE_ADDRESS_0, iobase);

	printk("\ncalling init_HR_TIMER\n");
	init_HR_TIMER();

	result = sis900_probe(&nic, &iobase, pcidev);

	printk("netboot_init : sis900_probe = %04x\n", (unsigned int) result);

	memcpy(arptable[ARP_CLIENT].node, eth_addr, ETH_ALEN);
	
	printk("doing rarp:\n");
	rarp();

	printk("My IP address is: %04x\n",arptable[ARP_CLIENT].ipaddr.s_addr);
	printk("My server's IP address is: %04x\n",arptable[ARP_SERVER].ipaddr.s_addr);

#ifdef DEBUG
	printk("Now testing tftp, transferring cmdline\n");
	
//	tftp("cmdline", test_display_tftp_callback);
	tftp_init("cmdline");
	while(rc == 512) {
		rc = tftp_fetchone(buffer);
		for(i=0; i<rc; i++) 
			if(!buffer[i])
				printk("|");
			else
				printk("%c",buffer[i]);
	}
#endif
	
}
