#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#if CONFIG_LOGICAL_CPUS
#include <cpu/amd/multicore.h>
#endif

#include <cpu/amd/amdk8_sysconf.h>
#include <stdlib.h>

// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
//busnum is default
unsigned char bus_8131_0 = 1;
unsigned char bus_8131_1 = 2;
unsigned char bus_8131_2 = 3;
unsigned char bus_8111_0 = 1;
unsigned char bus_8111_1 = 4;
unsigned char bus_8151_0 = 5;
unsigned char bus_8151_1 = 6;
unsigned apicid_8111;
unsigned apicid_8131_1;
unsigned apicid_8131_2;

unsigned pci1234x[] = {		//Here you only need to set value in pci1234 for HT-IO that could be installed or not
	//You may need to preset pci1234 for HTIO board, please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
	0x0000ff0,
	0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0
};

unsigned hcdnx[] = {		//HT Chain device num, actually it is unit id base of every ht device in chain, assume every chain only have 4 ht device at most
	0x20202020,
	0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
};

unsigned sbdn3;
unsigned sbdn5;

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{

	unsigned apicid_base;

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

	sysconf.sbdn = (sysconf.hcdn[0] >> 8) & 0xff;
	sbdn3 = sysconf.hcdn[0] & 0xff;
	sbdn5 = sysconf.hcdn[1] & 0xff;

	bus_8131_0 = (sysconf.pci1234[0] >> 16) & 0xff;
	bus_8111_0 = bus_8131_0;

	/* 8111 */
	dev = dev_find_slot(bus_8111_0, PCI_DEVFN(sysconf.sbdn, 0));
	if (dev) {
		bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
		printk(BIOS_DEBUG,
		       "ERROR - could not find PCI %02x:03.0, using defaults\n",
		       bus_8111_0);
	}

	/* 8131-1 */
	dev = dev_find_slot(bus_8131_0, PCI_DEVFN(sbdn3, 0));
	if (dev) {
		bus_8131_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
		printk(BIOS_DEBUG,
		       "ERROR - could not find PCI %02x:01.0, using defaults\n",
		       bus_8131_0);
	}

	/* 8132-2 */
	dev = dev_find_slot(bus_8131_0, PCI_DEVFN(sbdn3 + 1, 0));
	if (dev) {
		bus_8131_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
		printk(BIOS_DEBUG,
		       "ERROR - could not find PCI %02x:02.0, using defaults\n",
		       bus_8131_0);
	}

	/* HT chain 1 */
	// it is on node0, so it must be there
	bus_8151_0 = (sysconf.pci1234[1] >> 16) & 0xff;
	/* 8151 */
	dev = dev_find_slot(bus_8151_0, PCI_DEVFN(sbdn5 + 1, 0));

	if (dev) {
		bus_8151_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
//              printk(BIOS_DEBUG, "bus_8151_1=%d\n",bus_8151_1);
	}

/*I/O APICs:	APIC ID	Version	State		Address*/
#if CONFIG_LOGICAL_CPUS
	apicid_base = get_apicid_base(3);
#else
	apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
#endif
	apicid_8111 = apicid_base + 0;
	apicid_8131_1 = apicid_base + 1;
	apicid_8131_2 = apicid_base + 2;
}
