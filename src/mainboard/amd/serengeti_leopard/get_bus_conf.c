/*============================================================================
Copyright 2005 ADVANCED MICRO DEVICES, INC. All Rights Reserved.
This software and any related documentation (the "Materials") are the
confidential proprietary information of AMD. Unless otherwise provided in a
software agreement specifically licensing the Materials, the Materials are
provided in confidence and may not be distributed, modified, or reproduced in
whole or in part by any means.
LIMITATION OF LIABILITY: THE MATERIALS ARE PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO
WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY
PARTICULAR PURPOSE, OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR
USAGE OF TRADE. IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY
DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
BUSINESS INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF THE USE OF OR
INABILITY TO USE THE MATERIALS, EVEN IF AMD HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES. BECAUSE SOME JURISDICTIONS PROHIBIT THE EXCLUSION
OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE
LIMITATION MAY NOT APPLY TO YOU.
AMD does not assume any responsibility for any errors which may appear in the
Materials nor any responsibility to support or update the Materials. AMD
retains the right to modify the Materials at any time, without notice, and is
not obligated to provide such modified Materials to you.
NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
further information, software, technical information, know-how, or show-how
available to you.
U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to the
restrictions as set forth in FAR 52.227-14 and DFAR 252.227-7013, et seq., or
its successor. Use of the Materials by the Government constitutes
acknowledgement of AMD's proprietary rights in them.
============================================================================*/
// 2005.9 serengeti support
// by yhlu
//
//=

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
unsigned char bus_isa = 7 ;
unsigned char bus_8132_0 = 1;
unsigned char bus_8132_1 = 2;
unsigned char bus_8132_2 = 3;
unsigned char bus_8111_0 = 1;
unsigned char bus_8111_1 = 4;
unsigned char bus_8151_0 = 5;
unsigned char bus_8151_1 = 6;
unsigned apicid_8111 ;
unsigned apicid_8132_1;
unsigned apicid_8132_2;

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

static unsigned get_sbdn(void)
{
        device_t dev;
        unsigned sbdn = 3;// 8111 unit id base is 3 if 8131 before it
        dev = dev_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_PCI , 0); //FIXME: if 8111 PCI is disabled?
        if(dev)  {
                sbdn = (dev->path.u.pci.devfn >> 3) & 0x1f;
        }

        return sbdn;
}

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
	
	sbdn = get_sbdn();

//	bus_8132_0 = node_link_to_bus(0, sblk);
	bus_8132_0 = (pci1234[0] >> 16) & 0xff;
	bus_8111_0 = bus_8132_0;

                /* 8111 */
        dev = dev_find_slot(bus_8111_0, PCI_DEVFN(sbdn,0));
        if (dev) {
                bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                bus_isa    = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                bus_isa++;
//		printk_debug("bus_isa=%d\n",bus_isa);
        }
	else {
                printk_debug("ERROR - could not find PCI %02x:03.0, using defaults\n", bus_8111_0);
        }

        /* 8132-1 */
        dev = dev_find_slot(bus_8132_0, PCI_DEVFN(0x01,0));
        if (dev) {
                bus_8132_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
        }
        else {
                printk_debug("ERROR - could not find PCI %02x:01.0, using defaults\n", bus_8132_0);
        }

        /* 8132-2 */
        dev = dev_find_slot(bus_8132_0, PCI_DEVFN(0x02,0));
        if (dev) {
                bus_8132_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
        }
        else {
                printk_debug("ERROR - could not find PCI %02x:02.0, using defaults\n", bus_8132_0);
        }

        /* HT chain 1 */
	if((pci1234[1] & 0x1) == 1) {
//                bus_8151_0 = node_link_to_bus( (pci1234[1]>>4) & 0xf, (pci1234[1]>>8) & 0xf);
		bus_8151_0 = (pci1234[1] >> 16) & 0xff;
                /* 8151 */
        	dev = dev_find_slot(bus_8151_0, PCI_DEVFN(0x02,0));
              	if (dev) {
                       	bus_8151_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
//                        printk_debug("bus_8151_1=%d\n",bus_8151_1);
       	                bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
              	        bus_isa++;
              	}
        }

/*I/O APICs:	APIC ID	Version	State		Address*/
#if CONFIG_LOGICAL_CPUS==1
	apicid_base = get_apicid_base(3);
#else 
	apicid_base = CONFIG_MAX_PHYSICAL_CPUS; 
#endif
	apicid_8111 = apicid_base+0;
	apicid_8132_1 = apicid_base+1;
	apicid_8132_2 = apicid_base+2;
}
