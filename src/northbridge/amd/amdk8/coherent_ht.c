#if 0
static void setup_coherent_ht_domain(void)
{
	static const unsigned int register_values[] = {
	/* Routing Table Node i 
	 * F0:0x40 i = 0, 
	 * F0:0x44 i = 1,
	 * F0:0x48 i = 2, 
	 * F0:0x4c i = 3,
	 * F0:0x50 i = 4, 
	 * F0:0x54 i = 5,
	 * F0:0x58 i = 6, 
	 * F0:0x5c i = 7
	 * [ 0: 3] Request Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [11: 8] Response Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [19:16] Broadcast route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 */
	PCI_ADDR(0, 0x18, 0, 0x40), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x44), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x48), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x4c), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x50), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x54), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x58), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x5c), 0xfff0f0f0, 0x00010101,

	/* Hypetransport Transaction Control Register 
	 * F0:0x68
	 * [ 0: 0] Disable read byte probe
	 *         0 = Probes issues
	 *         1 = Probes not issued
	 * [ 1: 1] Disable Read Doubleword probe
	 *         0 = Probes issued
	 *         1 = Probes not issued
	 * [ 2: 2] Disable write byte probes
	 *         0 = Probes issued
	 *         1 = Probes not issued
	 * [ 3: 3] Disable Write Doubleword Probes
	 *         0 = Probes issued
	 *         1 = Probes not issued.
	 * [ 4: 4] Disable Memroy Controller Target Start
	 *         0 = TgtStart packets are generated
	 *         1 = TgtStart packets are not generated.
	 * [ 5: 5] CPU1 Enable
	 *         0 = Second CPU disabled or not present
	 *         1 = Second CPU enabled.
	 * [ 6: 6] CPU Request PassPW
	 *         0 = CPU requests do not pass posted writes
	 *         1 = CPU requests pass posted writes.
	 * [ 7: 7] CPU read Respons PassPW
	 *         0 = CPU Responses do not pass posted writes
	 *         1 = CPU responses pass posted writes.
	 * [ 8: 8] Disable Probe Memory Cancel
	 *         0 = Probes may generate MemCancels
	 *         1 = Probes may not generate MemCancels
	 * [ 9: 9] Disable Remote Probe Memory Cancel.
	 *         0 = Probes hitting dirty blocks generate memory cancel packets
	 *         1 = Only probed caches on the same node as the memory controller
	 *              generate cancel packets.
	 * [10:10] Disable Fill Probe
	 *         0 = Probes issued for cache fills
	 *         1 = Probes not issued for cache fills.
	 * [11:11] Response PassPw
	 *         0 = Downstream response PassPW based on original request
	 *         1 = Downstream response PassPW set to 1
	 * [12:12] Change ISOC to Ordered
	 *         0 = Bit 1 of coherent HT RdSz/WrSz command used for iosynchronous prioritization
	 *         1 = Bit 1 of coherent HT RdSz/WrSz command used for ordering.
	 * [14:13] Buffer Release Priority select 
	 *         00 = 64
	 *         01 = 16
	 *         10 = 8
	 *         11 = 2
	 * [15:15] Limit Coherent HT Configuration Space Range
	 *         0 = No coherent HT configuration space restrictions
	 *         1 = Limit coherent HT configuration space based on node count
	 * [16:16] Local Interrupt Conversion Enable.
	 *         0 = ExtInt/NMI interrups unaffected.
	 *         1 = ExtInt/NMI broadcat interrupts converted to LINT0/1
	 * [17:17] APIC Extended Broadcast Enable.
	 *         0 = APIC broadcast is 0F
	 *         1 = APIC broadcast is FF
	 * [18:18] APIC Extended ID Enable
	 *         0 = APIC ID is 4 bits.
	 *         1 = APIC ID is 8 bits.
	 * [19:19] APIC Extended Spurious Vector Enable
	 *         0 = Lower 4 bits of spurious vector are read-only 1111
	 *         1 = Lower 4 bits of spurious vecotr are writeable.
	 * [20:20] Sequence ID Source Node Enable
	 *         0 = Normal operation
	 *         1 = Keep SeqID on routed packets for debugging.
	 * [22:21] Downstream non-posted request limit
	 *         00 = No limit
	 *         01 = Limited to 1
	 *         10 = Limited to 4
	 *         11 = Limited to 8
	 * [23:23] RESERVED
	 * [25:24] Medium-Priority Bypass Count
	 *         - Maximum # of times a medium priority access can pass a low
	 *           priority access before Medium-Priority mode is disabled for one access.
	 * [27:26] High-Priority Bypass Count
	 *         - Maximum # of times a high prioirty access can pass a medium or low
	 *           priority access before High-prioirty mode is disabled for one access.
	 * [28:28] Enable High Priority CPU Reads
	 *         0 = Cpu reads are medium prioirty
	 *         1 = Cpu reads are high prioirty
	 * [29:29] Disable Low Priority Writes
	 *         0 = Non-isochronous writes are low priority
	 *         1 = Non-isochronous writes are medium prioirty
	 * [30:30] Disable High Priority Isochronous writes
	 *         0 = Isochronous writes are high priority
	 *         1 = Isochronous writes are medium priority
	 * [31:31] Disable Medium Priority Isochronous writes
	 *         0 = Isochronous writes are medium are high
	 *         1 = With bit 30 set makes Isochrouns writes low priority.
	 */
	PCI_ADDR(0, 0x18, 0, 0x68), 0x00800000, 0x0f00840f,
	/* HT Initialization Control Register
	 * F0:0x6C ok...
	 * [ 0: 0] Routing Table Disable
	 *         0 = Packets are routed according to routing tables
	 *         1 = Packets are routed according to the default link field
	 * [ 1: 1] Request Disable (BSP should clear this)
	 *         0 = Request packets may be generated
	 *         1 = Request packets may not be generated.
	 * [ 3: 2] Default Link (Read-only)
	 *         00 = LDT0
	 *         01 = LDT1
	 *         10 = LDT2
	 *         11 = CPU on same node
	 * [ 4: 4] Cold Reset
	 *         - Scratch bit cleared by a cold reset
	 * [ 5: 5] BIOS Reset Detect
	 *         - Scratch bit cleared by a cold reset
	 * [ 6: 6] INIT Detect
	 *         - Scratch bit cleared by a warm or cold reset not by an INIT
	 *
	 */
	PCI_ADDR(0, 0x18, 0, 0x6C), 0xffffff8c, 0x00000000 | (1 << 6) |(1 << 5)| (1 << 4),
	/* LDTi Capabilities Registers
	 * F0:0x80 i = 0,
	 * F0:0xA0 i = 1,
	 * F0:0xC0 i = 2,
	 */
	/* LDTi Link Control Registrs
	 * F0:0x84 i = 0,
	 * F0:0xA4 i = 1,
	 * F0:0xC4 i = 2,
	 * [ 1: 1] CRC Flood Enable
	 *         0 = Do not generate sync packets on CRC error
	 *         1 = Generate sync packets on CRC error
	 * [ 2: 2] CRC Start Test (Read-Only)
	 * [ 3: 3] CRC Force Frame Error
	 *         0 = Do not generate bad CRC
	 *         1 = Generate bad CRC
	 * [ 4: 4] Link Failure
	 *         0 = No link failure detected
	 *         1 = Link failure detected
	 * [ 5: 5] Initialization Complete
	 *         0 = Initialization not complete
	 *         1 = Initialization complete
	 * [ 6: 6] Receiver off
	 *         0 = Recevier on
	 *         1 = Receiver off
	 * [ 7: 7] Transmitter Off
	 *         0 = Transmitter on
	 *         1 = Transmitter off
	 * [ 9: 8] CRC_Error
	 *         00 = No error
	 *         [0] = 1 Error on byte lane 0
	 *         [1] = 1 Error on byte lane 1
	 * [12:12] Isochrnous Enable  (Read-Only)
	 * [13:13] HT Stop Tristate Enable
	 *         0 = Driven during an LDTSTOP_L
	 *         1 = Tristated during and LDTSTOP_L
	 * [14:14] Extended CTL Time 
	 *         0 = CTL is asserted for 16 bit times during link initialization
	 *         1 = CTL is asserted for 50us during link initialization
	 * [18:16] Max Link Width In (Read-Only?)
	 *         000 = 8 bit link
	 *         001 = 16bit link
	 * [19:19] Doubleword Flow Control in (Read-Only)
	 *         0 = This link does not support doubleword flow control
	 *         1 = This link supports doubleword flow control
	 * [22:20] Max Link Width Out (Read-Only?)
	 *         000 = 8 bit link
	 *         001 = 16bit link
	 * [23:23] Doubleworld Flow Control out (Read-Only)
	 *         0 = This link does not support doubleword flow control
	 *         1 = This link supports doubleworkd flow control
	 * [26:24] Link Width In
	 *         000 = Use 8 bits
	 *         001 = Use 16 bits
	 *         010 = reserved
	 *         011 = Use 32 bits
	 *         100 = Use 2 bits
	 *         101 = Use 4 bits
	 *         110 = reserved
	 *         111 = Link physically not connected
	 * [27:27] Doubleword Flow Control In Enable
	 *         0 = Doubleword flow control disabled
	 *         1 = Doubleword flow control enabled (Not currently supported)
	 * [30:28] Link Width Out
	 *         000 = Use 8 bits
	 *         001 = Use 16 bits
	 *         010 = reserved
	 *         011 = Use 32 bits
	 *         100 = Use 2 bits
	 *         101 = Use 4 bits
	 *         110 = reserved
	 *         111 = Link physically not connected
	 * [31:31] Doubleworld Flow Control Out Enable
	 *         0 = Doubleworld flow control disabled
	 *         1 = Doubleword flow control enabled (Not currently supported)
	 */
	PCI_ADDR(0, 0x18, 0, 0x84), 0x00009c05, 0x11110020,
	/* LDTi Frequency/Revision Registers
	 * F0:0x88 i = 0,
	 * F0:0xA8 i = 1,
	 * F0:0xC8 i = 2,
	 * [ 4: 0] Minor Revision
	 *         Contains the HT Minor revision
	 * [ 7: 5] Major Revision
	 *         Contains the HT Major revision
	 * [11: 8] Link Frequency  (Takes effect the next time the link is reconnected)
	 *         0000 = 200Mhz
	 *         0001 = reserved
	 *         0010 = 400Mhz
	 *         0011 = reserved
	 *         0100 = 600Mhz
	 *         0101 = 800Mhz
	 *         0110 = 1000Mhz
	 *         0111 = reserved
	 *         1000 = reserved
	 *         1001 = reserved
	 *         1010 = reserved
	 *         1011 = reserved
	 *         1100 = reserved
	 *         1101 = reserved
	 *         1110 = reserved
	 *         1111 = 100 Mhz
	 * [15:12] Error (Not currently Implemented)
	 * [31:16] Indicates the frequency capabilities of the link
	 *         [16] = 1 encoding 0000 of freq supported
	 *         [17] = 1 encoding 0001 of freq supported
	 *         [18] = 1 encoding 0010 of freq supported
	 *         [19] = 1 encoding 0011 of freq supported
	 *         [20] = 1 encoding 0100 of freq supported
	 *         [21] = 1 encoding 0101 of freq supported
	 *         [22] = 1 encoding 0110 of freq supported
	 *         [23] = 1 encoding 0111 of freq supported
	 *         [24] = 1 encoding 1000 of freq supported
	 *         [25] = 1 encoding 1001 of freq supported
	 *         [26] = 1 encoding 1010 of freq supported
	 *         [27] = 1 encoding 1011 of freq supported
	 *         [28] = 1 encoding 1100 of freq supported
	 *         [29] = 1 encoding 1101 of freq supported
	 *         [30] = 1 encoding 1110 of freq supported
	 *         [31] = 1 encoding 1111 of freq supported
	 */
	PCI_ADDR(0, 0x18, 0, 0x88), 0xfffff0ff, 0x00000200,
	/* LDTi Feature Capability
	 * F0:0x8C i = 0,
	 * F0:0xAC i = 1,
	 * F0:0xCC i = 2,
	 */
	/* LDTi Buffer Count Registers
	 * F0:0x90 i = 0,
	 * F0:0xB0 i = 1,
	 * F0:0xD0 i = 2,
	 */
	/* LDTi Bus Number Registers
	 * F0:0x94 i = 0,
	 * F0:0xB4 i = 1,
	 * F0:0xD4 i = 2,
	 * For NonCoherent HT specifies the bus number downstream (behind the host bridge)
	 * [ 0: 7] Primary Bus Number
	 * [15: 8] Secondary Bus Number
	 * [23:15] Subordiante Bus Number
	 * [31:24] reserved
	 */
	PCI_ADDR(0, 0x18, 0, 0x94), 0xff000000, 0x00ff0000,
	/* LDTi Type Registers
	 * F0:0x98 i = 0,
	 * F0:0xB8 i = 1,
	 * F0:0xD8 i = 2,
	 */
	};
	int i;
	int max;
	print_debug("setting up coherent ht domain....\r\n");
	max = sizeof(register_values)/sizeof(register_values[0]);
	for(i = 0; i < max; i += 3) {
		device_t dev;
		unsigned where;
		unsigned long reg;
#if 0
		print_debug_hex32(register_values[i]);
		print_debug(" <-");
		print_debug_hex32(register_values[i+2]);
		print_debug("\r\n");
#endif
		dev = register_values[i] & ~0xff;
		where = register_values[i] & 0xff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
#if 0
		reg = pci_read_config32(register_values[i]);
		reg &= register_values[i+1];
		reg |= register_values[i+2] & ~register_values[i+1];
		pci_write_config32(register_values[i], reg);
#endif
	}
	print_debug("done.\r\n");
}
#else
/* coherent hypertransport initialization for AMD64 
 * written by Stefan Reinauer <stepan@openbios.info>
 * (c) 2003 by SuSE Linux AG
 *
 * This code is licensed under GPL.
 */

/*
 * This algorithm assumes a grid configuration as follows:
 *
 * nodes :  1    2    4    6    8
 * org.  :  1x1  2x1  2x2  2x3  2x4
 *
 */

#if 0
#include "compat.h"
#endif

#include <device/pci_def.h>
#include "arch/romcc_io.h"


/* when generating a temporary row configuration we
 * don't want broadcast to be enabled for that node.
 */

#define generate_temp_row(x...) ((generate_row(x)&(~0x0f0000))|0x010000)
#define clear_temp_row(x)       fill_row(x,7,DEFAULT)
#define enable_bsp_routing()	enable_routing(0)

#define NODE_HT(x) PCI_DEV(0,24+x,0)
#define NODE_MP(x) PCI_DEV(0,24+x,1)
#define NODE_MC(x) PCI_DEV(0,24+x,3)

#define DEFAULT 0x00010101	/* default row entry */

typedef uint8_t u8;
typedef uint32_t u32;
typedef int8_t bool;

#define TRUE  (-1)
#define FALSE (0)

static void disable_probes(void)
{
	/* disable read/write/fill probes for uniprocessor setup
	 * they don't make sense if only one cpu is available
	 */

	/* Hypetransport Transaction Control Register 
	 * F0:0x68
	 * [ 0: 0] Disable read byte probe
	 *         0 = Probes issues
	 *         1 = Probes not issued
	 * [ 1: 1] Disable Read Doubleword probe
	 *         0 = Probes issued
	 *         1 = Probes not issued
	 * [ 2: 2] Disable write byte probes
	 *         0 = Probes issued
	 *         1 = Probes not issued
	 * [ 3: 3] Disable Write Doubleword Probes
	 *         0 = Probes issued
	 *         1 = Probes not issued.
	 * [10:10] Disable Fill Probe
	 *         0 = Probes issued for cache fills
	 *         1 = Probes not issued for cache fills.
	 */

	u32 val;

	print_debug("Disabling read/write/fill probes for UP... ");

	val=pci_read_config32(NODE_HT(0), 0x68);
	val |= 0x0000040f;
	pci_write_config32(NODE_HT(0), 0x68, val);

	print_debug("done.\r\n");

}

static void enable_routing(u8 node)
{
	u32 val;

	/* HT Initialization Control Register
	 * F0:0x6C
	 * [ 0: 0] Routing Table Disable
	 *         0 = Packets are routed according to routing tables
	 *         1 = Packets are routed according to the default link field
	 * [ 1: 1] Request Disable (BSP should clear this)
	 *         0 = Request packets may be generated
	 *         1 = Request packets may not be generated.
	 * [ 3: 2] Default Link (Read-only)
	 *         00 = LDT0
	 *         01 = LDT1
	 *         10 = LDT2
	 *         11 = CPU on same node
	 * [ 4: 4] Cold Reset
	 *         - Scratch bit cleared by a cold reset
	 * [ 5: 5] BIOS Reset Detect
	 *         - Scratch bit cleared by a cold reset
	 * [ 6: 6] INIT Detect
	 *         - Scratch bit cleared by a warm or cold reset not by an INIT
	 *
	 */

	/* Enable routing table for BSP */
	print_debug("Enabling routing table for node ");
	print_debug_hex32(node);

	val=pci_read_config32(NODE_HT(node), 0x6c);
	val |= (1 << 6) | (1 << 5) | (1 << 4);
#if 0
	val &= ~((1<<1)|(1<<0));
#else
	/* Don't enable requests here as the indicated processor starts booting */
	val &= ~(1<<0);
#endif
	pci_write_config32(NODE_HT(node), 0x6c, val);

	print_debug(" done.\r\n");
}

#if MAX_CPUS > 1

static void rename_temp_node(u8 node)
{
	u32 val;

	print_debug("Renaming current temp node to ");
	print_debug_hex32(node);

	val=pci_read_config32(NODE_HT(7), 0x60);
	val &= (~7);  /* clear low bits. */
        val |= node;   /* new node        */
	pci_write_config32(NODE_HT(7), 0x60, val);

	print_debug(" done.\r\n");


}

static bool check_connection(u8 src, u8 dest, u8 link)
{
	/* this function does 2 things:
	 * 1) detect whether the coherent HT link is connected.
         *    After this step follows a small idle loop.
	 * 2) verify that the coherent hypertransport link
	 *    is established and actually working by reading the
	 *    remote node's vendor/device id
	 */

#define UP	0x00
#define ACROSS	0x20
#define DOWN	0x40

	u32 val;
	
	/* 1) */
	val=pci_read_config32(NODE_HT(src), 0x98+link);
	if ( (val&0x17) != 0x03)
		return 0;

	/* idle loop to make sure the link is established */
	for (val=0;val<16;val++);

	/* 2) */
        val=pci_read_config32(NODE_HT(dest),0);
	if(val != 0x11001022)
		return 0;

	return 1;
}

static unsigned int generate_row(u8 node, u8 row, u8 maxnodes)
{
	/* Routing Table Node i 
	 *
	 * F0: 0x40, 0x44, 0x48, 0x4c, 0x50, 0x54, 0x58, 0x5c 
	 *  i:    0,    1,    2,    3,    4,    5,    6,    7
	 *
	 * [ 0: 3] Request Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [11: 8] Response Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [19:16] Broadcast route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 */

	u32 ret=DEFAULT;

	static const unsigned int rows_2p[2][2] = {
		{ 0x00030101, 0x00010404 },
		{ 0x00010404, 0x00030101 }
	};

	static const unsigned int rows_4p[4][4] = {
		{ 0x00070101, 0x00010404, 0x00050202, 0x00010402 },
		{ 0x00010808, 0x000b0101, 0x00010802, 0x00090202 },
		{ 0x00090202, 0x00010802, 0x000b0101, 0x00010808 },
		{ 0x00010402, 0x00050202, 0x00010404, 0x00070101 }
	};

	if (!(node>=maxnodes || row>=maxnodes)) {
		if (maxnodes==2)
			ret=rows_2p[node][row];
		if (maxnodes==4)
			ret=rows_4p[node][row];
	}

#if 0
	printk_spew("generating entry n=%d, r=%d, max=%d - row=%x\n", 
		node,row,maxnodes,ret);
#endif
	
	return ret;
}

static void fill_row(u8 node, u8 row, u32 value)
{
#if 0
	print_debug("fill_row: pci_write_config32(");
	print_debug_hex32(NODE_HT(node));
	print_debug_char(',');
	print_debug_hex32(0x40 + (row << 2));
	print_debug_char(',');
	print_debug_hex32(value);
	print_debug(")\r\n");
#endif	
	pci_write_config32(NODE_HT(node), 0x40+(row<<2), value);
}

static void setup_row(u8 source, u8 dest, u8 cpus)
{
#if 0
	printk_spew("setting up link from node %d to %d (%d cpus)\r\n",
		source, dest, cpus);
#endif

	fill_row(source,dest,generate_row(source,dest,cpus));
}

static void setup_temp_row(u8 source, u8 dest, u8 cpus)
{
#if 0
	printk_spew("setting up temp. link from node %d to %d (%d cpus)\r\n",
		source, dest, cpus);
#endif

	fill_row(source,7,generate_temp_row(source,dest,cpus));
}

static void setup_node(u8 node, u8 cpus)
{
	u8 row;
	for(row=0; row<cpus; row++)
		setup_row(node, row, cpus);
}

static void setup_remote_row(u8 source, u8 dest, u8 cpus)
{
	fill_row(7, dest, generate_row(source, dest, cpus));
}

static void setup_remote_node(u8 node, u8 cpus)
{
	static const uint8_t pci_reg[] = { 
		0x44, 0x4c, 0x54, 0x5c, 0x64, 0x6c, 0x74, 0x7c, 
		0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78,
		0x84, 0x8c, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbc,
		0x80, 0x88, 0x90, 0x98, 0xa0, 0xa8, 0xb0, 0xb8,
		0xc4, 0xcc, 0xd4, 0xdc,
		0xc0, 0xc8, 0xd0, 0xd8,
		0xe0, 0xe4, 0xe8, 0xec,
	};
	uint8_t row;
	int i;
#if 1
	print_debug("setup_remote_node\r\n");
#endif
	for(row=0; row<cpus; row++)
		setup_remote_row(node, row, cpus);

	/* copy the default resource map from node 0 */
	for(i = 0; i < sizeof(pci_reg)/sizeof(pci_reg[0]); i++) {
		uint32_t value;
		uint8_t reg;
		reg = pci_reg[i];
		print_debug("copying reg: ");
		print_debug_hex8(reg);
		print_debug("\r\n");
		value = pci_read_config32(NODE_MP(0), reg);
		pci_write_config32(NODE_MP(7), reg, value);

	}
#if 1
	print_debug("setup_remote_done\r\n");
#endif
}

#endif

#if MAX_CPUS > 2
static void setup_temp_node(u8 node, u8 cpus)
{
	u8 row;
	for(row=0; row<cpus; row++)
		fill_row(7,row,generate_row(node,row,cpus));
}
#endif

static u8 setup_uniprocessor(void)
{
	print_debug("Enabling UP settings\r\n");
	disable_probes();
	return 1;
}

#if MAX_CPUS > 1
static u8 setup_smp(void)
{
	u8 cpus=2;

	print_debug("Enabling SMP settings\r\n");

	setup_row(0,0,cpus);
	/* Setup and check a temporary connection to node 1 */
	setup_temp_row(0,1,cpus);
	
	if (!check_connection(0, 7, ACROSS)) {	// Link: ACROSS
		print_debug("No connection to Node 1.\r\n");
		clear_temp_row(0);	/* delete temp connection */
		setup_uniprocessor();	/* and get up working     */
		return 1;
	}

	/* We found 2 nodes so far */
	setup_node(0, cpus);	/* Node 1 is there. Setup Node 0 correctly */
	setup_remote_node(1, cpus);  /* Setup the routes on the remote node */
	enable_routing(7);	/* Enable routing on Node 1 */
	rename_temp_node(1);	/* Rename Node 7 to Node 1  */
	
	clear_temp_row(0);	/* delete temporary connection */
	
#if MAX_CPUS > 2
	cpus=4;
	
	/* Setup and check temporary connection from Node 0 to Node 2 */
	setup_temp_row(0,2,cpus);

	if (!check_connection(0, 7, UP)) {	// Link: UP
		print_debug("No connection to Node 2.\r\n");
		clear_temp_row(0);	 /* delete temp connection */
		return 2;
	}

	/* We found 3 nodes so far. Now setup a temporary
	 * connection from node 0 to node 3 via node 1
	 */

	setup_temp_row(0,1,cpus); /* temp. link between nodes 0 and 1 */
	setup_temp_row(1,3,cpus); /* temp. link between nodes 1 and 3 */

	if (!check_connection(1, 7, UP)) {	// Link: UP
		print_debug("No connection to Node 3.\r\n");
		clear_temp_row(0);	 /* delete temp connection */
		clear_temp_row(1);	 /* delete temp connection */
		return 2;
	}

	/* We found 4 nodes so far. Now setup all nodes for 4p */

	setup_node(0, cpus);  /* The first 2 nodes are configured    */
	setup_node(1, cpus);  /* already. Just configure them for 4p */
	
	setup_temp_row(0,2,cpus);
	setup_temp_node(2,cpus);
	enable_routing(7);
	rename_temp_node(2);

	setup_temp_row(0,1,cpus);
	setup_temp_row(1,3,cpus);
	setup_temp_node(3,cpus);
	enable_routing(7);	/* enable routing on node 3 (temp.) */
	rename_temp_node(3);
	
	clear_temp_row(0);
	clear_temp_row(1);
	clear_temp_row(2);
	clear_temp_row(3);

#endif
	print_debug_hex32(cpus);
	print_debug(" nodes initialized.\r\n");
	return cpus;
}
#endif

#if MAX_CPUS > 1
static unsigned detect_mp_capabilities(unsigned cpus)
{
	unsigned node, row, mask;
	bool mp_cap=TRUE;

#if 1
	print_debug("detect_mp_capabilities: ");
	print_debug_hex32(cpus);
	print_debug("\r\n");
#endif
	if (cpus>2)
		mask=0x06;	/* BigMPCap */
	else
		mask=0x02;	/* MPCap    */

	for (node=0; node<cpus; node++) {
		if ((pci_read_config32(NODE_MC(node), 0xe8) & mask)!=mask)
			mp_cap=FALSE;
	}

	if (mp_cap)
		return cpus;

	/* one of our cpus is not mp capable */

	print_debug("One of the CPUs is not MP capable. Going back to UP\r\n");

	for (node=cpus; node>0; node--)
	    for (row=cpus; row>0; row--)
		fill_row(NODE_HT(node-1), row-1, DEFAULT);
	
	return setup_uniprocessor();
}

#endif

/* this is a shrunken cpuid. */

static unsigned int cpuid(unsigned int op)
{
	unsigned int ret;

	asm volatile ( "cpuid" : "=a" (ret) : "a" (op));

	return ret;
}

static void coherent_ht_finalize(unsigned cpus)
{
	int node;
	bool rev_a0;
	
	/* set up cpu count and node count and enable Limit
	 * Config Space Range for all available CPUs.
	 * Also clear non coherent hypertransport bus range
	 * registers on Hammer A0 revision.
	 */

#if 1
	print_debug("coherent_ht_finalize\r\n");
#endif
	rev_a0=((cpuid(1)&0xffff)==0x0f10);

	for (node=0; node<cpus; node++) {
		u32 val;
		val=pci_read_config32(NODE_HT(node), 0x60);
		val &= (~0x000F0070);
		val |= ((cpus-1)<<16)|((cpus-1)<<4);
		pci_write_config32(NODE_HT(node),0x60,val);

		val=pci_read_config32(NODE_HT(node), 0x68);
		val |= 0x00008000;
		pci_write_config32(NODE_HT(node),0x68,val);

		if (rev_a0) {
			pci_write_config32(NODE_HT(node),0x94,0);
			pci_write_config32(NODE_HT(node),0xb4,0);
			pci_write_config32(NODE_HT(node),0xd4,0);
		}
	}

#if 1
	print_debug("done\n");
#endif
}

static void setup_coherent_ht_domain(void)
{
	unsigned cpus;

	enable_bsp_routing();

#if MAX_CPUS == 1
	cpus=setup_uniprocessor();
#else
	cpus=setup_smp();
	cpus=detect_mp_capabilities(cpus);
#endif
	coherent_ht_finalize(cpus);
}

#endif
