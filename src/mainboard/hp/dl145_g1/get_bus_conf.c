/*
 * Copyright (c) 2011,2014 Oskar Enoksson <enok@lysator.liu.se>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/multicore.h>

#include <cpu/amd/amdk8_sysconf.h>
#include <stdlib.h>

#include "mb_sysconf.h"

// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
struct mb_sysconf_t mb_sysconf;

static unsigned pci1234x[] =
{	//Here you only need to set value in pci1234 for HT-IO that could be installed or not
	 //You may need to preset pci1234 for HTIO board, please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
	0x0000ff0,
//	0x0000ff0,
//	0x0000ff0,
//	0x0000ff0,
//	0x0000ff0,
//	0x0000ff0,
//	0x0000ff0,
//	0x0000ff0
};
static unsigned hcdnx[] =
{ //HT Chain device num, actually it is unit id base of every ht device in chain, assume every chain only have 4 ht device at most
	0x20202020,
//	0x20202020,
//	0x20202020,
//	0x20202020,
//	0x20202020,
//	0x20202020,
//	0x20202020,
//	0x20202020,
};


static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{

	unsigned apicid_base;

	device_t dev;
	int i;

	if (get_bus_conf_done == 1)
		return; //do it only once

	get_bus_conf_done = 1;

	sysconf.mb = &mb_sysconf;
	struct mb_sysconf_t *m = sysconf.mb;

	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);
	for (i = 0; i < sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}

	get_sblk_pci1234();

	sysconf.sbdn = (sysconf.hcdn[0] >> 8) & 0xff;
	m->sbdn3 = sysconf.hcdn[0] & 0xff;

	m->bus_8131_0 = (sysconf.pci1234[0] >> 16) & 0xff;
	m->bus_8111_0 = m->bus_8131_0;

	/* 8111 */
	dev = dev_find_slot(m->bus_8111_0, PCI_DEVFN(sysconf.sbdn,0));
	if (dev)
		m->bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	else
		printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:03.0, using defaults\n", m->bus_8111_0);

	/* 8131-1 */
	dev = dev_find_slot(m->bus_8131_0, PCI_DEVFN(m->sbdn3,0));
	if (dev)
		m->bus_8131_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	else
		printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:01.0, using defaults\n", m->bus_8131_0);

	/* 8131-2 */
	dev = dev_find_slot(m->bus_8131_0, PCI_DEVFN(m->sbdn3+1,0));
	if (dev)
		m->bus_8131_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	else
		printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:02.0, using defaults\n", m->bus_8131_0);


/*I/O APICs:	APIC ID	Version	State		Address*/
	if (IS_ENABLED(CONFIG_LOGICAL_CPUS))
		apicid_base = get_apicid_base(3);
	else
		apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
	m->apicid_8111 = apicid_base+0;
	m->apicid_8131_1 = apicid_base+1;
	m->apicid_8131_2 = apicid_base+2;
}
