#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/multicore.h>

#include <cpu/amd/amdk8_sysconf.h>
#include <stdlib.h>

// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
//busnum is default
unsigned char bus_ck804_0;	//1
unsigned char bus_ck804_1;	//2
unsigned char bus_ck804_2;	//3
unsigned char bus_ck804_3;	//4
unsigned char bus_ck804_4;	//5
unsigned char bus_ck804_5;	//6
unsigned apicid_ck804;

unsigned pci1234x[] = {		//Here you only need to set value in pci1234 for HT-IO that could be installed or not
	//You may need to preset pci1234 for HTIO board, please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
	0x0000000,
};

unsigned hcdnx[] = {		//HT Chain device num, actually it is unit id base of every ht device in chain, assume every chain only have 4 ht device at most
	0x20202020,
};

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{

	unsigned apicid_base;
	unsigned sbdn;

	device_t dev;
	int i;

	if (get_bus_conf_done == 1)
		return;		//do it only once

	get_bus_conf_done = 1;

	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);
	for (i = 0; i < sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}

	get_sblk_pci1234();

	sysconf.sbdn = (sysconf.hcdn[0] & 0xff);	// first byte of first chain
	sbdn = sysconf.sbdn;

	bus_ck804_0 = (sysconf.pci1234[0] >> 16) & 0xff;

	/* CK804 */
	dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(sbdn + 0x09, 0));
	if (dev) {
		bus_ck804_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		bus_ck804_4 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_ck804_4++;
	} else {
		printk(BIOS_DEBUG,
		       "ERROR - could not find PCI 1:%02x.0, using defaults\n",
		       sbdn + 0x09);

		bus_ck804_1 = 2;
		bus_ck804_4 = 3;
	}

	dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(sbdn + 0x0d, 0));
	if (dev) {
		bus_ck804_4 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		bus_ck804_5 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_ck804_5++;
	} else {
		printk(BIOS_DEBUG,
		       "ERROR - could not find PCI 1:%02x.0, using defaults\n",
		       sbdn + 0x0d);

		bus_ck804_5 = bus_ck804_4 + 1;
	}

	dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(sbdn + 0x0e, 0));
	if (dev)
		bus_ck804_5 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	else
		printk(BIOS_DEBUG,
		       "ERROR - could not find PCI 1:%02x.0, using defaults\n",
		       sbdn + 0x0e);

/*I/O APICs:	APIC ID	Version	State		Address*/
	apicid_base = get_apicid_base(1);
	apicid_ck804 = apicid_base + 0;
}
