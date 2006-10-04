#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#if CONFIG_LOGICAL_CPUS==1
#include <cpu/amd/dualcore.h>
#endif

#include <cpu/amd/amdk8_sysconf.h>

#include "mb_sysconf.h"

// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
struct mb_sysconf_t mb_sysconf;

static unsigned pci1234x[] = 
{        //Here you only need to set value in pci1234 for HT-IO that could be installed or not
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
static unsigned hcdnx[] = 
{ //HT Chain device num, actually it is unit id base of every ht device in chain, assume every chain only have 4 ht device at most
	0x20202020,
	0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
};

extern void get_sblk_pci1234(void);

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{

	unsigned apicid_base;

        device_t dev;
	int i;
	struct mb_sysconf_t *m;

	if(get_bus_conf_done == 1) return; //do it only once

	get_bus_conf_done = 1;

	sysconf.mb = &mb_sysconf;
	
	m = sysconf.mb;

	sysconf.hc_possible_num = sizeof(pci1234x)/sizeof(pci1234x[0]);	
	for(i=0;i<sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}
	
	get_sblk_pci1234();
	
	sysconf.sbdn = (sysconf.hcdn[0] >> 8) & 0xff;
	m->sbdn3 = sysconf.hcdn[0] & 0xff;
	m->sbdn5 = sysconf.hcdn[1] & 0xff;

	m->bus_8132_0 = (sysconf.pci1234[0] >> 16) & 0xff;
	m->bus_8111_0 = m->bus_8132_0;

                /* 8111 */
        dev = dev_find_slot(m->bus_8111_0, PCI_DEVFN(sysconf.sbdn,0));
        if (dev) {
                m->bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
#if HT_CHAIN_END_UNITID_BASE >= HT_CHAIN_UNITID_BASE
                m->bus_isa    = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                m->bus_isa++;
//		printk_debug("bus_isa=%d\n",bus_isa);
#endif
        }
	else {
                printk_debug("ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8111_0, sysconf.sbdn);
        }

        /* 8132-1 */
        dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN(m->sbdn3,0));
        if (dev) {
                m->bus_8132_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
        }
        else {
                printk_debug("ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132_0, m->sbdn3);
        }

        /* 8132-2 */
        dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN(m->sbdn3+1,0));
        if (dev) {
                m->bus_8132_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
#if HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE
                m->bus_isa    = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                m->bus_isa++;
//              printk_debug("bus_isa=%d\n",bus_isa);
#endif
        }
        else {
                printk_debug("ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132_0, m->sbdn3+1);
        }

        /* HT chain 1 */
	if((sysconf.pci1234[1] & 0x1) == 1) {
		m->bus_8151_0 = (sysconf.pci1234[1] >> 16) & 0xff;
                /* 8151 */
		dev = dev_find_slot(m->bus_8151_0, PCI_DEVFN(m->sbdn5+1, 0));

              	if (dev) {
                       	m->bus_8151_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
//                        printk_debug("bus_8151_1=%d\n",bus_8151_1);
       	                m->bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
              	        m->bus_isa++;
              	}
       		else {
                	printk_debug("ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8151_0, m->sbdn5+1);
        	}
        }

/*I/O APICs:	APIC ID	Version	State		Address*/
#if CONFIG_LOGICAL_CPUS==1
	apicid_base = get_apicid_base(3);
#else 
	apicid_base = CONFIG_MAX_PHYSICAL_CPUS; 
#endif
	m->apicid_8111 = apicid_base+0;
	m->apicid_8132_1 = apicid_base+1;
	m->apicid_8132_2 = apicid_base+2;
}
