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
        unsigned char bus_isa;
        unsigned char bus_ck804_0; //1
        unsigned char bus_ck804_1; //2
        unsigned char bus_ck804_2; //3
        unsigned char bus_ck804_3; //4
        unsigned char bus_ck804_4; //5
        unsigned char bus_ck804_5; //6
        unsigned char bus_8131_0;  //7
        unsigned char bus_8131_1;  //8
        unsigned char bus_8131_2;  //9
        unsigned apicid_ck804;
        unsigned apicid_8131_1;
        unsigned apicid_8131_2;

unsigned sblk;
unsigned pci1234[] = 
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
unsigned hc_possible_num;
unsigned sbdn;
unsigned hcdn[] = 
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

unsigned sbdn3;

extern void get_sblk_pci1234(void);

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{

	unsigned apicid_base;

        device_t dev;

	if(get_bus_conf_done==1) return; //do it only once

	get_bus_conf_done = 1;

	hc_possible_num = sizeof(pci1234)/sizeof(pci1234[0]);	
	
	get_sblk_pci1234();
	
	sbdn = (hcdn[0] & 0xff); // first byte of first chain

	sbdn3 = (hcdn[1] & 0xff); // first byte of second chain

//	bus_ck804_0 = node_link_to_bus(0, sblk);
	bus_ck804_0 = (pci1234[0] >> 16) & 0xff;


                /* CK804 */
                dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(sbdn + 0x09,0));
                if (dev) {
                        bus_ck804_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
#if 0
                        bus_ck804_2 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_ck804_2++;
#else
                        bus_ck804_4 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_ck804_4++;
#endif
                }
                else {
                        printk_debug("ERROR - could not find PCI 1:%02x.0, using defaults\n", sbdn + 0x09);

                        bus_ck804_1 = 2;
#if 0
                        bus_ck804_2 = 3;
#else
                        bus_ck804_4 = 3;
#endif

                }
#if 0
                dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(sbdn + 0x0b,0));
                if (dev) {
                        bus_ck804_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_ck804_3 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_ck804_3++;
                }
                else {
                        printk_debug("ERROR - could not find PCI 1:%02x.0, using defaults\n", sbdn + 0x0b);

                        bus_ck804_3 = bus_ck804_2+1;
                }

                dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(sbdn + 0x0c,0));
                if (dev) {
                        bus_ck804_3 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_ck804_4 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_ck804_4++;
                }
                else {
                        printk_debug("ERROR - could not find PCI 1:%02x.0, using defaults\n", sbdn + 0x0c);

                        bus_ck804_4 = bus_ck804_3+1;
                }

#endif

                dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(sbdn + 0x0d,0));
                if (dev) {
                        bus_ck804_4 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_ck804_5 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_ck804_5++;
                }
                else {
                        printk_debug("ERROR - could not find PCI 1:%02x.0, using defaults\n",sbdn + 0x0d);

                        bus_ck804_5 = bus_ck804_4+1;
                }

                dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(sbdn+ 0x0e,0));
                if (dev) {
                        bus_ck804_5 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_8131_0 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_8131_0++;
                        bus_isa = bus_8131_0; // incase only one installed
                }
                else {
                        printk_debug("ERROR - could not find PCI 1:%02x.0, using defaults\n", sbdn+ 0x0e);

                }

		bus_8131_0 = (pci1234[1] >> 16) & 0xff;

                /* 8131-1 */
                dev = dev_find_slot(bus_8131_0, PCI_DEVFN(sbdn3,0));
                if (dev) {
                        bus_8131_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_8131_2 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_8131_2++;
                }
                else {
                        printk_debug("ERROR - could not find PCI %02x:01.0, using defaults\n", bus_8131_0);

                        bus_8131_1 = bus_8131_0+1;
                        bus_8131_2 = bus_8131_0+2;
                }
                /* 8131-2 */
                dev = dev_find_slot(bus_8131_0, PCI_DEVFN(sbdn3+1,0));
                if (dev) {
                        bus_8131_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_isa++;
                }
                else {
                        printk_debug("ERROR - could not find PCI %02x:02.0, using defaults\n", bus_8131_0);

                        bus_8131_2 = bus_8131_1+1;
                        bus_isa = bus_8131_1+2;
                }



/*I/O APICs:	APIC ID	Version	State		Address*/
#if CONFIG_LOGICAL_CPUS==1
	apicid_base = get_apicid_base(3);
#else 
	apicid_base = CONFIG_MAX_PHYSICAL_CPUS; 
#endif
	apicid_ck804 = apicid_base+0;
        apicid_8131_1 = apicid_base+1;
        apicid_8131_2 = apicid_base+2;
}
