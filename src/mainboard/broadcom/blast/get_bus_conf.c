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
unsigned char bus_bcm5780[7];
unsigned char bus_bcm5785_0 = 1;
unsigned char bus_bcm5785_1 = 8;
unsigned char bus_bcm5785_1_1 = 9;
unsigned apicid_bcm5785[3];

unsigned pci1234x[] = {		//Here you only need to set value in pci1234 for HT-IO that could be installed or not
	//You may need to preset pci1234 for HTIO board, please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
	0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0
};

unsigned hcdnx[] = {		//HT Chain device num, actually it is unit id base of every ht device in chain, assume every chain only have 4 ht device at most
	0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
};

unsigned sbdn2;

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{

	unsigned apicid_base;

	struct device *dev;
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

	sysconf.sbdn = (sysconf.hcdn[0] >> 8) & 0xff;
	sbdn2 = sysconf.hcdn[0] & 0xff;	// bcm5780

	bus_bcm5785_0 = (sysconf.pci1234[0] >> 16) & 0xff;
	bus_bcm5780[0] = bus_bcm5785_0;

	/* bcm5785 */
	dev = dev_find_slot(bus_bcm5785_0, PCI_DEVFN(sysconf.sbdn, 0));
	if (dev) {
		bus_bcm5785_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		dev = dev_find_slot(bus_bcm5785_1, PCI_DEVFN(0x0d, 0));
		if (dev) {
			bus_bcm5785_1_1 =
			    pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
	} else {
		printk(BIOS_DEBUG,
		       "ERROR - could not find PCI %02x:07.0, using defaults\n",
		       bus_bcm5785_0);
	}

	/* bcm5780 */
	for (i = 1; i < 7; i++) {
		dev =
		    dev_find_slot(bus_bcm5780[0], PCI_DEVFN(sbdn2 + i - 1, 0));
		if (dev) {
			bus_bcm5780[i] =
			    pci_read_config8(dev, PCI_SECONDARY_BUS);
		} else {
			printk(BIOS_DEBUG,
			       "ERROR - could not find PCI %02x:01.0, using defaults\n",
			       bus_bcm5780[i]);
		}
	}

/*I/O APICs:	APIC ID	Version	State		Address*/
	if (IS_ENABLED(CONFIG_LOGICAL_CPUS))
		apicid_base = get_apicid_base(3);
	else
		apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
	for (i = 0; i < 3; i++)
		apicid_bcm5785[i] = apicid_base + i;
}
