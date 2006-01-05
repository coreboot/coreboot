#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#if CONFIG_LOGICAL_CPUS==1
#include <cpu/amd/dualcore.h>
#endif


// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
//busnum is default
unsigned char bus_isa = 7;
unsigned char bus_8111_0 = 1;
unsigned char bus_8111_1 = 4;
unsigned char bus_8131[7][3];	// another 6 8131
unsigned apicid_8111;
unsigned apicid_8131[7][2];

unsigned sblk;
unsigned pci1234[] = {		//Here you only need to set value in pci1234 for HT-IO that could be installed or not
	//You may need to preset pci1234 for HTIO board, please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
	0x0000ff0,
	0x0000f10,
	0x0000f20,
	0x0000f30,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0
};
unsigned hc_possible_num;
unsigned sbdn;
unsigned hcdn[] = {		//HT Chain device num, actually it is unit id base of every ht device in chain, assume every chain only have 4 ht device at most
	0x20202020,
	0x20202020,
	0x20202020,
	0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
};

unsigned sbdnx[7];		// for all 8131

extern void get_sblk_pci1234(void);

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{

	unsigned apicid_base;

	device_t dev;

	int i;

	if (get_bus_conf_done == 1)
		return;		//do it only once

	get_bus_conf_done = 1;

	hc_possible_num = sizeof(pci1234) / sizeof(pci1234[0]);

	get_sblk_pci1234();


	sbdn = ((hcdn[0] >> 8) & 0xff);	// first byte of first chain
	sbdnx[0] = (hcdn[0] & 0xff);

	for (i = 0; i < hc_possible_num; i++) {
		sbdnx[i * 2 + 1] = hcdn[i] & 0xff;
		sbdnx[i * 2 + 2] = (hcdn[i] >> 8) & 0xff;
	}

	bus_8131[0][0] = (pci1234[0] >> 16) & 0xff;
	bus_8111_0 = bus_8131[0][0];

	/* 8111 */
	dev = dev_find_slot(bus_8111_0, PCI_DEVFN(sbdn, 0));
	if (dev) {
		bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
//              printk_debug("bus_isa=%d\n",bus_isa);
	} else {
		printk_debug
		    ("ERROR - could not find PCI %02x:03.0, using defaults\n",
		     bus_8111_0);
	}


	/* 8131-1 */
	bus_8131[0][0] = 1;
	dev = dev_find_slot(bus_8131[0][0], PCI_DEVFN(sbdnx[1], 0));
	if (dev) {
		bus_8131[0][1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
		printk_debug
		    ("ERROR - could not find PCI %02x:01.0, using defaults\n",
		     bus_8131[0][0]);
	}


	/* 8132-2 */
	dev = dev_find_slot(bus_8131[0][0], PCI_DEVFN(sbdnx[1] + 1, 0));
	if (dev) {
		bus_8131[0][2] = pci_read_config8(dev, PCI_SECONDARY_BUS);
		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
	} else {
		printk_debug
		    ("ERROR - could not find PCI %02x:02.0, using defaults\n",
		     bus_8131[0][0]);
	}

	apicid_base = get_apicid_base(15);

	apicid_8111 = apicid_base++;

	apicid_8131[0][0] = apicid_base++;

	apicid_8131[0][1] = apicid_base++;


	/* HT chain 1 */
	for (i = 1; i < 4; i++) {
		if (pci1234[i] & 0x1) {
			int j = (i - 1) * 2 + 1;
			bus_8131[j][0] = (pci1234[i] >> 16) & 0xff;
			/* 8131 */
			dev = dev_find_slot(bus_8131[j][0], PCI_DEVFN(sbdnx[j], 0));
			if (dev) {
				bus_8131[j][1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			}
			apicid_8131[j][0] = apicid_base++;
			dev = dev_find_slot(bus_8131[j][0], PCI_DEVFN(sbdnx[j] + 1, 0));
			if (dev) {
				bus_8131[j][2] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			}
			apicid_8131[j][1] = apicid_base++;

			bus_8131[j + 1][0] = bus_8131[j][0];
			/* 8131 */
			dev = dev_find_slot(bus_8131[j + 1][0], PCI_DEVFN(sbdnx[j + 1], 0));
			if (dev) {
				bus_8131[j + 1][1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			}
			apicid_8131[j + 1][0] = apicid_base++;

			dev = dev_find_slot(bus_8131[i + 1][0], PCI_DEVFN(sbdnx[j + 1] + 1, 0));
			if (dev) {
				bus_8131[j + 1][2] = pci_read_config8(dev, PCI_SECONDARY_BUS);
				bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
				bus_isa++;

			}
			apicid_8131[j + 1][0] = apicid_base++;
		}
	}
}
