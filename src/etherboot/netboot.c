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

int test_display_tftp_callback(char *data, int block, int length, int eof)
{
	int i;

#ifdef DEBUG	
	printk_info("RECD block %u, length = %u:\n",block, length);
#endif
	for (i = 0; i < length; i++)
		if (!data[i])
			printk_info("|");
		else
			printk_info("%c",data[i]);
#ifdef DEBUG
	if (eof)
		printk_info("\nEND OF FILE\n");
	
	printk_info("====================\n");
#endif
	return(-1);
}

char buffer[1024];

void netboot_init() 
{
	struct pci_dev *pcidev;
	struct nic *result;
	unsigned short iobase;

#ifdef DEBUG
	int rc = 512;
	int i;
#endif

	nic.node_addr = eth_addr;

	/* first, init the sis900 ethernet! */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_900, (void *)NULL);
	iobase = pcidev->base_address[0] & PCI_BASE_ADDRESS_IO_MASK;

	result = sis900_probe(&nic, &iobase, pcidev);

	printk_info("netboot_init : sis900_probe = %04x\n", (unsigned int) result);

	memcpy(arptable[ARP_CLIENT].node, eth_addr, ETH_ALEN);
	
	printk_info("doing rarp:\n");
	rarp();

	//printk_info("doing DHCP:\n");
	//bootp();

	printk_info("My IP address is: %04x\n", arptable[ARP_CLIENT].ipaddr.s_addr);
	printk_info("My server's IP address is: %04x\n", arptable[ARP_SERVER].ipaddr.s_addr);

#ifdef DEBUG
	printk_info("Now testing tftp, transferring cmdline\n");
	
	//tftp("cmdline", test_display_tftp_callback);
	tftp_init("cmdline");
	while(rc == 512) {
		rc = tftp_fetchone(buffer);
		for(i=0; i<rc; i++) 
			if(!buffer[i])
				printk_info("|");
			else
				printk_info("%c",buffer[i]);
	}
#endif
	
}
