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
#include "mb_sysconf.h"

// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
struct mb_sysconf_t mb_sysconf;

static unsigned pci1234x[] =
{        //Here you only need to set value in pci1234 for HT-IO that could be installed or not
	 //You may need to preset pci1234 for HTIO board, please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
        0x0000ff0, // SB chain m
        0x0000000, // HTX
        0x0000100, // co processor on socket 1
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0,
//        0x0000ff0
};
static unsigned hcdnx[] =
{ //HT Chain device num, actually it is unit id base of every ht device in chain, assume every chain only have 4 ht device at most
	0x20202020,
	0x20202020,
        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
};



static unsigned get_bus_conf_done = 0;

static unsigned get_hcid(unsigned i)
{
        unsigned id = 0;

        unsigned busn = (sysconf.pci1234[i] >> 16) & 0xff;

        unsigned devn = sysconf.hcdn[i] & 0xff;

        device_t dev;

        dev = dev_find_slot(busn, PCI_DEVFN(devn,0));

        switch (dev->device) {
        case 0x7458: //8132
                id = 1;
                break;
        case 0x7454: //8151
                id = 2;
		break;
        case 0x7450: //8131
                id = 3;
                break;
        }

        // we may need more way to find out hcid: subsystem id? GPIO read ?

        // we need use id for 1. bus num, 2. mptable, 3. acpi table

        return id;
}

void get_bus_conf(void)
{

	unsigned apicid_base;

        device_t dev;
	int i, j;
	struct mb_sysconf_t *m;

	if(get_bus_conf_done == 1) return; //do it only once

	get_bus_conf_done = 1;

	sysconf.mb = &mb_sysconf;

	m = sysconf.mb;

	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);
	for(i=0;i<sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}

	get_sblk_pci1234();

	sysconf.sbdn = (sysconf.hcdn[0] >> 8) & 0xff;
	m->sbdn3 = sysconf.hcdn[0] & 0xff;

	m->bus_8132_0 = (sysconf.pci1234[0] >> 16) & 0xff;
	m->bus_8111_0 = m->bus_8132_0;

                /* 8111 */
        dev = dev_find_slot(m->bus_8111_0, PCI_DEVFN(sysconf.sbdn,0));
        if (dev) {
                m->bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
        }
	else {
                printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8111_0, sysconf.sbdn);
        }

        /* 8132-1 */
        dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN(m->sbdn3,0));
        if (dev) {
                m->bus_8132_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
        }
        else {
                printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132_0, m->sbdn3);
        }

        /* 8132-2 */
        dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN(m->sbdn3+1,0));
        if (dev) {
                m->bus_8132_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
        }
        else {
                printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132_0, m->sbdn3+1);
        }

        /* HT chain 1 */
        j=0;
        for(i=1; i< sysconf.hc_possible_num; i++) {
                if(!(sysconf.pci1234[i] & 0x1) ) continue;

                // check hcid type here
                sysconf.hcid[i] = get_hcid(i);

                switch(sysconf.hcid[i]) {

                case 1: //8132
		case 3: //8131

                        m->bus_8132a[j][0] = (sysconf.pci1234[i] >> 16) & 0xff;

                        m->sbdn3a[j] = sysconf.hcdn[i] & 0xff;

                        /* 8132-1 */
                        dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j],0));
                        if (dev) {
                                m->bus_8132a[j][1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        }
                        else {
                        printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132a[j][0], m->sbdn3a[j]);
                        }

                        /* 8132-2 */
                        dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j]+1,0));
                        if (dev) {
                                m->bus_8132a[j][2] = pci_read_config8(dev, PCI_SECONDARY_BUS);
                                }
                        else {
                                printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132a[j][0], m->sbdn3a[j]+1);
                        }

                        break;

                case 2: //8151

                        m->bus_8151[j][0] = (sysconf.pci1234[i] >> 16) & 0xff;
                        m->sbdn5[j] = sysconf.hcdn[i] & 0xff;
                        /* 8151 */
                        dev = dev_find_slot(m->bus_8151[j][0], PCI_DEVFN(m->sbdn5[j]+1, 0));

                        if (dev) {
                                m->bus_8151[j][1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        }
                        else {
                                printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8151[j][0], m->sbdn5[j]+1);
                        }

                        break;
                }

                j++;
        }


/*I/O APICs:	APIC ID	Version	State		Address*/
#if CONFIG_LOGICAL_CPUS
	apicid_base = get_apicid_base(3);
#else
	apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
#endif
	m->apicid_8111 = apicid_base+0;
	m->apicid_8132_1 = apicid_base+1;
	m->apicid_8132_2 = apicid_base+2;
        for(i=0;i<j;i++) {
                m->apicid_8132a[i][0] = apicid_base + 3 + i*2;
                m->apicid_8132a[i][1] = apicid_base + 3 + i*2 + 1;
        }

}
