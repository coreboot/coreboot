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

#include <device/pci_def.h>
#include "arch/romcc_io.h"

/*
 * 
 */

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
	val |= (1<<10)|(1<<9)|(1<<8)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1 << 0);
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
	val &= ~((1<<1)|(1<<0));
	pci_write_config32(NODE_HT(node), 0x6c, val);

	print_debug(" done.\r\n");
}

#if MAX_CPUS > 1

static void rename_temp_node(u8 node)
{
	uint32_t val;

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

	/* 2) */
        val=pci_read_config32(NODE_HT(dest),0);
	if(val != 0x11001022)
		return 0;

	return 1;
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
#if 0
		print_debug("copying reg: ");
		print_debug_hex8(reg);
		print_debug("\r\n");
#endif
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
	rename_temp_node(1);	/* Rename Node 7 to Node 1  */
	enable_routing(1);	/* Enable routing on Node 1 */
	
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
	rename_temp_node(2);
	enable_routing(2);

	setup_temp_row(0,1,cpus);
	setup_temp_row(1,3,cpus);
	setup_temp_node(3,cpus);
	rename_temp_node(3);
	enable_routing(3);	/* enable routing on node 3 (temp.) */
	
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
	print_debug("done\r\n");
#endif
}

static int setup_coherent_ht_domain(void)
{
	unsigned cpus;
	int reset_needed = 0;

	enable_bsp_routing();

#if MAX_CPUS == 1
	cpus=setup_uniprocessor();
#else
	cpus=setup_smp();
	cpus=detect_mp_capabilities(cpus);
#endif
	coherent_ht_finalize(cpus);

	return reset_needed;
}
