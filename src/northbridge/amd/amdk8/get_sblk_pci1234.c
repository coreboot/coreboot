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
//  2005.9 yhlu modify that to more dynamic for AMD Opteron Based MB

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>

#include <cpu/amd/amdk8_sysconf.h>


#if 0
unsigned node_link_to_bus(unsigned node, unsigned link)
{
        device_t dev;
        unsigned reg;

        dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));
        if (!dev) {
                return 0;
        }
        for(reg = 0xE0; reg < 0xF0; reg += 0x04) {
                uint32_t config_map;
                unsigned dst_node;
                unsigned dst_link;
                unsigned bus_base;
                config_map = pci_read_config32(dev, reg);
                if ((config_map & 3) != 3) {
                        continue;
                }
                dst_node = (config_map >> 4) & 7;
                dst_link = (config_map >> 8) & 3;
                bus_base = (config_map >> 16) & 0xff;
#if 0
                printk_debug("node.link=bus: %d.%d=%d 0x%2x->0x%08x\n",
                        dst_node, dst_link, bus_base,
                        reg, config_map);
#endif
                if ((dst_node == node) && (dst_link == link))
                {
                        return bus_base;
                }
        }
        return 0;
}
#endif


/* why we need pci1234 array
        final result for pci1234 will be
                pci1234[0] will record sblink and bus range
                pci1234[i] will record ht chain i.
        it will keep the sequence when some ht io card is not installed.

        for Tyan S2885 the linxbios_ram will put 8151 chain (link 0) to 0xE0 reg, and 8131/8111 on 0xe4 reg, So we need to make sure the sb link
                will always on pci1234[0]
        for multi ht-io cards, if you don't install htio1, and only installed htio2, htio3, the htio will be on 0xe4, and 0xe8.
                but we want to leave pci1234[1] to htio1 (even it is disabled) , and let htio2 and htio3 still use pci1234[2] and pci1234[3]
        So we keep the sequence. ---- you need to preset the pci1234[1], pci1234[2], pci1234[3] for this purpose
                                        for example you need set
                        unsigned pci1234[] = {
                                0x0000ff0,
                                0x0000f10, // HT IO 1 card always on node 1
                                0x0000f20, // HT IO 2 card always on node 2
                                0x0000f30  // HT IO 3 card always on node 3
                        };

        for 2p+htio(n1)+htio(n0_1)+htio(n1_1),2p+htio(n1)+2p+htio(n2)+htio(n3) :  need pci1234[6]
                        unsigned pci1234[] = {
                                0x0000ff0,
                                0x0000010, // HT IO 1 card always on node 1
                                0x0000f00, // HT IO 2 card always on node 0
                                0x0000110, // HT IO 3 card always on node 1
                                0x0000f20, // HT IO 4 card always on node 2
                                0x0000f30  // HT IO 5 card always on node 3
                        };

        for 4p+htio(n1)+htio(n2)+htio(n3),4p+htio(n1)+4p+htio(n6)+htio(n7) :  need pci1234[6]
                        unsigned pci1234[] = {
                                0x0000ff0,
                                0x0000f10, // HT IO 1 card always on node 1
                                0x0000f20, // HT IO 2 card always on node 2
                                0x0000f30, // HT IO 3 card always on node 3
                                0x0000f60, // HT IO 4 card always on node 6
                                0x0000f70  // HT IO 5 card always on node 7
                        };


        for 2p+htio(n1)+htio(n0_1)+htio(n1_1), 2p+htio(n1)+2p+htio(n2)+htio(n3), 2p+htio(n1)+4p+htio(n4)+htio(n5), need pci1234[8]
                        unsigned pci1234[] = {
                                0x0000ff0,
                                0x0000010, // HT IO 1 card always on node 1
                                0x0000f00, // HT IO 2 card always on node 0
                                0x0000110, // HT IO 3 card always on node 1
                                0x0000f20, // HT IO 4 card always on node 2
                                0x0000f30  // HT IO 5 card always on node 3
                                0x0000f40, // HT IO 6 card always on node 4
                                0x0000f50  // HT IO 7 card always on node 5
                        };


        for 4p+htio(n1)+htio(n2)+htio(n3), 4p+htio(n1)+2p+htio(n4)+htio(n5), 4p+htio(n1)+4p+htio(n6)+htio(n7), need pci1234[8]
                        unsigned pci1234[] = {
                                0x0000ff0,
                                0x0000f10, // HT IO 1 card always on node 1
                                0x0000f20, // HT IO 2 card always on node 2
                                0x0000f30, // HT IO 3 card always on node 3
                                0x0000f40, // HT IO 4 card always on node 4
                                0x0000f50  // HT IO 5 card always on node 5
                                0x0000f60, // HT IO 6 card always on node 6
                                0x0000f70  // HT IO 7 card always on node 7
                        };


        So Max HC_POSSIBLE_NUM is 8

        1n: 3
        2n: 2x2 - 1
        4n: 1x4 - 2 
        6n: 2 
        8n: 2 
	Total: 12 

        just put all the possible ht node/link to the list tp pci1234[] in  get_bus_conf.c on MB dir

	Also don't forget to increase the ACPI_SSDTX_NUM etc if you have too much SSDT

	How about co-processor on socket 1 on 2 way system. or socket 2, and socket3 on 4 way system....? treat that as one hc too!

*/
void get_sblk_pci1234(void)
{

        device_t dev;
        int i,j;
        uint32_t dword;

        /* read PCI_DEV(0,0x18,0) 0x64 bit [8:9] to find out SbLink m */
        dev = dev_find_slot(0, PCI_DEVFN(0x18,0));
        dword = pci_read_config32(dev, 0x64);
        sysconf.sblk = (dword>>8) & 0x3;

        dword &=0x0300;
        dword |= 1;
        sysconf.pci1234[0] = dword;

        /*about hardcode numbering for HT_IO support
                set the node_id and link_id that could have ht chain in the one array,
                then check if is enabled.... then update final value 
        */
        //here we need to set hcdn
        //1. hypertransport.c need to record hcdn_reg together with 0xe0, 0xe4, 0xe8, 0xec when are set
        //2. so at the same time we need update hsdn with hcdn_reg here

        dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));
        for(j=0;j<4;j++) {
                uint32_t dwordx;
                dwordx = pci_read_config32(dev, 0xe0+j*4);
                dwordx &=0xffff0ff1; //keep bus num, node_id, link_num, enable bits
                if((dwordx & 0xff1) == dword) { //SBLINK
                        sysconf.pci1234[0] = dwordx;
			sysconf.hcdn[0] = sysconf.hcdn_reg[j];
                        continue;
                }
                if((dwordx & 1) == 1) {
                        // We need to find out the number of HC
                        // for exact match
                        for(i=1;i<sysconf.hc_possible_num;i++) {
                                if((dwordx & 0xff0) == (sysconf.pci1234[i] & 0xff0)) {
                                        sysconf.pci1234[i] = dwordx;
					sysconf.hcdn[i] = sysconf.hcdn_reg[j];
                                        break;
                                }
                        }
                        // for 0xff0 match or same node
                        for(i=1;i<sysconf.hc_possible_num;i++) {
                                if((dwordx & 0xff0) == (dwordx & sysconf.pci1234[i] & 0xff0)) {
                                        sysconf.pci1234[i] = dwordx;
					sysconf.hcdn[i] = sysconf.hcdn_reg[j];
                                        break;
                                }
                        }
                }
        }

        for(i=1;i<sysconf.hc_possible_num;i++) {
                if((sysconf.pci1234[i] & 1) != 1) {
                        sysconf.pci1234[i] = 0;
			sysconf.hcdn[i] = 0x20202020;
                }
        }

}

