#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#if CONFIG_LOGICAL_CPUS==1
#include <cpu/amd/dualcore.h>
#endif

unsigned sblk;
unsigned pci1234[] =
{        //Here you only need to set value in pci1234 for HT-IO that could be installed or not
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
unsigned hc_possible_num;
unsigned sbdn;
unsigned hcdn[] =
{ //HT Chain device num, actually it is unit id base of every ht device in chain, assume every chain only have 4 ht device at most
        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
//        0x20202020,
};

// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
//busnum is default
unsigned char bus_isa = 10;
unsigned char bus_bcm5780[7];
unsigned char bus_bcm5785_0 = 1;
unsigned char bus_bcm5785_1 = 8;
unsigned char bus_bcm5785_1_1 = 9;
unsigned apicid_bcm5785[3];

unsigned sbdn2;

extern void get_sblk_pci1234(void);

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{

	unsigned apicid_base;

        device_t dev;
        int i;

        if(get_bus_conf_done==1) return; //do it only once

        get_bus_conf_done = 1;

        hc_possible_num = sizeof(pci1234)/sizeof(pci1234[0]);

        get_sblk_pci1234();

        sbdn = (hcdn[0] >> 8) & 0xff;
        sbdn2 = hcdn[0] & 0xff; // bcm5780

	bus_bcm5785_0 = (pci1234[0] >> 16) & 0xff;
	bus_bcm5780[0] = bus_bcm5785_0;

                /* bcm5785 */
        dev = dev_find_slot(bus_bcm5785_0, PCI_DEVFN(sbdn,0));
        if (dev) {
                bus_bcm5785_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		dev = dev_find_slot(bus_bcm5785_1, PCI_DEVFN(0x0d,0));
		if(dev) {
			bus_bcm5785_1_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
#if HT_CHAIN_END_UNITID_BASE >= HT_CHAIN_UNITID_BASE
	                bus_isa    = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
	                bus_isa++;
//        	        printk_debug("bus_isa=%d\n",bus_isa);
#endif
		}
        }
	else {
                printk_debug("ERROR - could not find PCI %02x:07.0, using defaults\n", bus_bcm5785_0);
        }

		/* bcm5780 */
	for(i = 1; i < 7; i++) {
	        dev = dev_find_slot(bus_bcm5780[0], PCI_DEVFN(sbdn2 + i - 1,0));
	        if(dev) {
        	        bus_bcm5780[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
#if HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE
                        bus_isa    = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_isa++;
//                      printk_debug("bus_isa=%d\n",bus_isa);
#endif

		}
        	else {
                	printk_debug("ERROR - could not find PCI %02x:01.0, using defaults\n", bus_bcm5780[i]);
	        }
	}


/*I/O APICs:	APIC ID	Version	State		Address*/
#if CONFIG_LOGICAL_CPUS==1
	apicid_base = get_apicid_base(3);
#else 
	apicid_base = CONFIG_MAX_PHYSICAL_CPUS; 
#endif
	for(i=0;i<3;i++) 
		apicid_bcm5785[i] = apicid_base+i;
}
