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
/* Copyright 2007 coresystems GmbH */

// 2005.9 yhlu    serengeti support
// 2005.9 yhlu    modify that to more dynamic for AMD Opteron Based MB
// 2007.9 stepan  improve code documentation

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


/**
 * Why we need the pci1234[] array
 *
 * It will keep the sequence of HT devices in the HT link registers even when a
 * given HT I/O card is not installed.
 *
 * The final result for pci1234[] will be
 *
 *     pci1234[0] will record the south bridge link and bus range
 *     pci1234[i] will record HT chain i.
 *
 * For example, on the Tyan S2885 coreboot_ram will put the AMD8151 chain (HT
 * link 0) into the register 0xE0, and the AMD8131/8111 HT chain into the
 * register 0xE4.
 *
 * So we need to make sure that the south bridge link will always be on
 * pci1234[0].
 *
 * Imagine a scenario with multiple HT I/O cards, where you don't install HT I/O 1, 
 * but you only install HT I/O 2 and HT I/O 3. The HT I/Os  will end up in registers 
 * 0xE4 and 0xE8.
 *
 * But we want to leave pci1234[1] to HT I/O 1 (even though it is disabled),
 * and let HT I/O 2 and HT I/O 3 still use pci1234[2] and pci1234[3].
 *
 * So we keep the sequence. You need to preset the pci1234[1], pci1234[2],
 * pci1234[3] for this purpose.
 *
 * For this example you need to set
 *
 *     unsigned pci1234[] = {
 *             0x0000ff0,
 *             0x0000f10, // HT IO 1 card always on node 1
 *             0x0000f20, // HT IO 2 card always on node 2
 *             0x0000f30  // HT IO 3 card always on node 3
 *     };
 *
 * For 2P + htio(n1) + htio(n0_1) + htio(n1_1), 2P + htio(n1) + 2P + htio(n2) + htio(n3):
 * You need an array pci1234[6]:
 *
 *     unsigned pci1234[] = {
 *             0x0000ff0,
 *             0x0000010, // HT IO 1 card always on node 1
 *             0x0000f00, // HT IO 2 card always on node 0
 *             0x0000110, // HT IO 3 card always on node 1
 *             0x0000f20, // HT IO 4 card always on node 2
 *             0x0000f30  // HT IO 5 card always on node 3
 *     };
 *
 *
 * For 4p+htio(n1)+htio(n2)+htio(n3),4p+htio(n1)+4p+htio(n6)+htio(n7):  
 * You need an array pci1234[6]:
 *
 *     unsigned pci1234[] = {
 *             0x0000ff0,
 *             0x0000f10, // HT IO 1 card always on node 1
 *             0x0000f20, // HT IO 2 card always on node 2
 *             0x0000f30, // HT IO 3 card always on node 3
 *             0x0000f60, // HT IO 4 card always on node 6
 *             0x0000f70  // HT IO 5 card always on node 7
 *     };
 * 
 * 
 * For 2p + htio(n1) + htio(n0_1) + htio(n1_1), 2P + htio(n1) + 2P + 
 * htio(n2) + htio(n3), 2P + htio(n1) + 4P + htio(n4) + htio(n5), 
 * you need an array pci1234[8]:
 *
 *     unsigned pci1234[] = {
 *             0x0000ff0,
 *             0x0000010, // HT IO 1 card always on node 1
 *             0x0000f00, // HT IO 2 card always on node 0
 *             0x0000110, // HT IO 3 card always on node 1
 *             0x0000f20, // HT IO 4 card always on node 2
 *             0x0000f30  // HT IO 5 card always on node 3
 *             0x0000f40, // HT IO 6 card always on node 4
 *             0x0000f50  // HT IO 7 card always on node 5
 *     };
 * 
 * 
 * For 4P + htio(n1) + htio(n2) + htio(n3), 4p + htio(n1) + 2p + htio(n4) +
 * htio(n5), 4p + htio(n1) + 4p + htio(n6) + htio(n7), 
 * you need an array pci1234[8]:
 *
 *     unsigned pci1234[] = {
 *             0x0000ff0,
 *             0x0000f10, // HT IO 1 card always on node 1
 *             0x0000f20, // HT IO 2 card always on node 2
 *             0x0000f30, // HT IO 3 card always on node 3
 *             0x0000f40, // HT IO 4 card always on node 4
 *             0x0000f50  // HT IO 5 card always on node 5
 *             0x0000f60, // HT IO 6 card always on node 6
 *             0x0000f70  // HT IO 7 card always on node 7
 *     };
 * 
 * 
 * So the maximum posible value of HC_POSSIBLE_NUM is 8. (FIXME Why?)
 * 
 *     1n:       3
 *     2n: 2x2 - 1
 *     4n: 1x4 - 2 
 *     6n:       2 
 *     8n:       2 
 *  Total:      12 
 * 
 * Just put all the possible HT Node/link to the list tp pci1234[] in 
 * src/mainboard/<vendor>/<mainboard>get_bus_conf.c
 * 
 * Also don't forget to increase the ACPI_SSDTX_NUM etc (FIXME what else) if
 * you have too many SSDTs
 * 
 * What about co-processor in socket 1 on a 2 way system? Or socket 2 and
 * socket 3 on a 4 way system? Treat that as an HC, too!
 * 
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
	sysconf.hcid[0] = 0;

        /* About hardcoded numbering for HT_IO support
	 *
	 * Set the node_id and link_id that could have a HT chain in the one
	 * array, (FIXME: which one?) then check if is enabled. Then update
	 * final value 
         */

        /* Here we need to set hcdn
	 *
	 * 1. hypertransport.c needs to record hcdn_reg together with 0xe0,
	 *    0xe4, 0xe8, 0xec when are set (FIXME: when WHAT is set?)
	 *
	 * 2. So at the same time we need update hcdn with hcdn_reg here. FIXME: Why?
	 */

        dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));

        for(j=0;j<4;j++) {
                uint32_t dwordx;
                dwordx = pci_read_config32(dev, 0xe0 + j*4);
                dwordx &=0xffff0ff1; /* keep bus num, node_id, link_num, enable bits */
                if((dwordx & 0xff1) == dword) { /* SBLINK */
                        sysconf.pci1234[0] = dwordx;
			sysconf.hcdn[0] = sysconf.hcdn_reg[j];
                        continue;
                }

                if((dwordx & 1) == 1) {
                        /* We need to find out the number of HC
                         * for exact match
			 */
                        for(i=1;i<sysconf.hc_possible_num;i++) {
                                if((dwordx & 0xff0) == (sysconf.pci1234[i] & 0xff0)) {
                                        sysconf.pci1234[i] = dwordx;
					sysconf.hcdn[i] = sysconf.hcdn_reg[j];
                                        break;
                                }
                        }

                        /* For 0xff0 match or same node */
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
		sysconf.hcid[i] = 0;
        }

}

