/* coherent hypertransport initialization for AMD64 
 * 
 * written by Stefan Reinauer <stepan@openbios.org>
 * (c) 2003-2004 by SuSE Linux AG
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
#include <device/pci_ids.h>
#include <device/hypertransport_def.h>
#include "arch/romcc_io.h"
#include "amdk8.h"

/* when generating a temporary row configuration we
 * don't want broadcast to be enabled for that node.
 */

#define generate_temp_row(...) ((generate_row(__VA_ARGS__)&(~0x0f0000))|0x010000)
#define enable_bsp_routing()	enable_routing(0)

#define NODE_HT(x) PCI_DEV(0,24+x,0)
#define NODE_MP(x) PCI_DEV(0,24+x,1)
#define NODE_MC(x) PCI_DEV(0,24+x,3)

#define DEFAULT 0x00010101	/* default row entry */

typedef uint8_t u8;
typedef uint32_t u32;
typedef int bool;

#define TRUE  (-1)
#define FALSE (0)

static u8 link_to_register(int ldt)
{
	/*
	 * [ 0: 3] Request Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 */

	if (ldt&0x08) return 0x40;
	if (ldt&0x04) return 0x20;
	if (ldt&0x02) return 0x00;
	
	/* we should never get here */
	print_spew("Unknown Link\n");
	return 0;
}

static int link_connection(int src, int dest)
{
	/* we generate the needed link information from the rows
	 * by taking the Request Route of the according row.
	 */
	
	return generate_row(src, dest, CONFIG_MAX_CPUS) & 0x0f;
}

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

	print_spew("Disabling read/write/fill probes for UP... ");

	val=pci_read_config32(NODE_HT(0), 0x68);
	val |= (1<<10)|(1<<9)|(1<<8)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1 << 0);
	pci_write_config32(NODE_HT(0), 0x68, val);

	print_spew("done.\r\n");

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

	/* Enable routing table */
	print_spew("Enabling routing table for node ");
	print_spew_hex8(node);

	val=pci_read_config32(NODE_HT(node), 0x6c);
	val &= ~((1<<1)|(1<<0));
	pci_write_config32(NODE_HT(node), 0x6c, val);

	print_spew(" done.\r\n");
}

static void fill_row(u8 node, u8 row, u32 value)
{
	pci_write_config32(NODE_HT(node), 0x40+(row<<2), value);
}


#if CONFIG_MAX_CPUS > 1

static void rename_temp_node(u8 node)
{
	uint32_t val;

	print_spew("Renaming current temporary node to ");
	print_spew_hex8(node);

	val=pci_read_config32(NODE_HT(7), 0x60);
	val &= (~7);  /* clear low bits. */
        val |= node;   /* new node        */
	pci_write_config32(NODE_HT(7), 0x60, val);

	print_spew(" done.\r\n");
}

static bool check_connection(u8 src, u8 dest, u8 link)
{
	/* See if we have a valid connection to dest */
	u32 val;
	
	/* Detect if the coherent HT link is connected. */
	val = pci_read_config32(NODE_HT(src), 0x98+link);
	if ( (val&0x17) != 0x03)
		return 0;

	/* Verify that the coherent hypertransport link is
	 * established and actually working by reading the
	 * remode node's vendor/device id
	 */
        val = pci_read_config32(NODE_HT(dest),0);
	if(val != 0x11001022)
		return 0;

	return 1;
}

static unsigned read_freq_cap(device_t dev, unsigned pos)
{
	/* Handle bugs in valid hypertransport frequency reporting */
	unsigned freq_cap;
	uint32_t id;

	freq_cap = pci_read_config16(dev, pos);
	freq_cap &= ~(1 << HT_FREQ_VENDOR); /* Ignore Vendor HT frequencies */

	id = pci_read_config32(dev, 0);

	/* AMD 8131 Errata 48 */
	if (id == (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8131_PCIX << 16))) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
	}
	/* AMD 8151 Errata 23 */
	if (id == (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8151_SYSCTRL << 16))) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
	}
	/* AMD K8 Unsupported 1Ghz? */
	if (id == (PCI_VENDOR_ID_AMD | (0x1100 << 16))) {
		freq_cap &= ~(1 << HT_FREQ_1000Mhz);
	}
	return freq_cap;
}

static int optimize_connection(device_t node1, uint8_t link1, device_t node2, uint8_t link2)
{
	static const uint8_t link_width_to_pow2[]= { 3, 4, 0, 5, 1, 2, 0, 0 };
	static const uint8_t pow2_to_link_width[] = { 0x7, 4, 5, 0, 1, 3 };
	uint16_t freq_cap1, freq_cap2, freq_cap, freq_mask;
	uint8_t width_cap1, width_cap2, width_cap, width, old_width, ln_width1, ln_width2;
	uint8_t freq, old_freq;
	int needs_reset;
	/* Set link width and frequency */

	/* Initially assume everything is already optimized and I don't need a reset */
	needs_reset = 0;

	/* Get the frequency capabilities */
	freq_cap1 = read_freq_cap(node1, link1 + PCI_HT_CAP_HOST_FREQ_CAP);
	freq_cap2 = read_freq_cap(node2, link2 + PCI_HT_CAP_HOST_FREQ_CAP);

	/* Calculate the highest possible frequency */
	freq = log2(freq_cap1 & freq_cap2);

	/* See if I am changing the link freqency */
	old_freq = pci_read_config8(node1, link1 + PCI_HT_CAP_HOST_FREQ);
	needs_reset |= old_freq != freq;
	old_freq = pci_read_config8(node2, link2 + PCI_HT_CAP_HOST_FREQ);
	needs_reset |= old_freq != freq;

	/* Set the Calulcated link frequency */
	pci_write_config8(node1, link1 + PCI_HT_CAP_HOST_FREQ, freq);
	pci_write_config8(node2, link2 + PCI_HT_CAP_HOST_FREQ, freq);

	/* Get the width capabilities */
	width_cap1 = pci_read_config8(node1, link1 + PCI_HT_CAP_HOST_WIDTH);
	width_cap2 = pci_read_config8(node2, link2 + PCI_HT_CAP_HOST_WIDTH);

	/* Calculate node1's input width */
	ln_width1 = link_width_to_pow2[width_cap1 & 7];
	ln_width2 = link_width_to_pow2[(width_cap2 >> 4) & 7];
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width = pow2_to_link_width[ln_width1];
	/* Calculate node1's output width */
	ln_width1 = link_width_to_pow2[(width_cap1 >> 4) & 7];
	ln_width2 = link_width_to_pow2[width_cap2 & 7];
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width |= pow2_to_link_width[ln_width1] << 4;
	
	/* See if I am changing node1's width */
	old_width = pci_read_config8(node1, link1 + PCI_HT_CAP_HOST_WIDTH + 1);
	needs_reset |= old_width != width;

	/* Set node1's widths */
	pci_write_config8(node1, link1 + PCI_HT_CAP_HOST_WIDTH + 1, width);

	/* Calculate node2's width */
	width = ((width & 0x70) >> 4) | ((width & 0x7) << 4);

	/* See if I am changing node2's width */
	old_width = pci_read_config8(node2, link2 + PCI_HT_CAP_HOST_WIDTH + 1);
	needs_reset |= old_width != width;

	/* Set node2's widths */
	pci_write_config8(node2, link2 + PCI_HT_CAP_HOST_WIDTH + 1, width);

	return needs_reset;
}

static void setup_row(u8 source, u8 dest, u8 nodes)
{
	fill_row(source,dest,generate_row(source,dest,nodes));
}

static void setup_temp_row(u8 source, u8 dest, u8 nodes)
{
	fill_row(source,7,generate_temp_row(source,dest,nodes));
}

static void setup_node(u8 node, u8 nodes)
{
	u8 row;
	for(row=0; row<nodes; row++)
		setup_row(node, row, nodes);
}

static void setup_remote_row(u8 source, u8 dest, u8 nodes)
{
	fill_row(7, dest, generate_row(source, dest, nodes));
}

static void setup_remote_node(u8 node, u8 nodes)
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

	print_spew("setup_remote_node: ");
	for(row=0; row<nodes; row++)
		setup_remote_row(node, row, nodes);

	/* copy the default resource map from node 0 */
	for(i = 0; i < sizeof(pci_reg)/sizeof(pci_reg[0]); i++) {
		uint32_t value;
		uint8_t reg;
		reg = pci_reg[i];
		value = pci_read_config32(NODE_MP(0), reg);
		pci_write_config32(NODE_MP(7), reg, value);

	}
	print_spew("done\r\n");
}

#endif

#if CONFIG_MAX_CPUS > 2
static void setup_temp_node(u8 node, u8 nodes)
{
	u8 row;
	for(row=0; row<nodes; row++)
		fill_row(7,row,generate_row(node,row,nodes));
}
#endif

static void setup_uniprocessor(void)
{
	print_spew("Enabling UP settings\r\n");
	disable_probes();
}

struct setup_smp_result {
	int nodes;
	int needs_reset;
};

#if CONFIG_MAX_CPUS > 1
static struct setup_smp_result setup_smp(void)
{
	struct setup_smp_result result;
	result.nodes = 2;
	result.needs_reset = 0;

	print_spew("Enabling SMP settings\r\n");

	setup_row(0, 0, result.nodes);
	/* Setup and check a temporary connection to node 1 */
	setup_temp_row(0, 1, result.nodes);
	
	if (!check_connection(0, 7, link_to_register(link_connection(0,1)))) {
		print_spew("No connection to Node 1.\r\n");
		setup_uniprocessor();	/* and get up working     */
		result.nodes = 1;
		return result;
	}

	/* We found 2 nodes so far */
	
	setup_node(0, result.nodes);	/* Node 1 is there. Setup Node 0 correctly */
	setup_remote_node(1, result.nodes);  /* Setup the routes on the remote node */
        rename_temp_node(1);    /* Rename Node 7 to Node 1  */
        enable_routing(1);      /* Enable routing on Node 1 */
  	
	result.needs_reset = optimize_connection(
		NODE_HT(0), 0x80 + link_to_register(link_connection(0,1)),
		NODE_HT(1), 0x80 + link_to_register(link_connection(1,0)) );

#if CONFIG_MAX_CPUS > 2
	result.nodes=4;
	
	/* Setup and check temporary connection from Node 0 to Node 2 */
	setup_temp_row(0,2, result.nodes);

	if (!check_connection(0, 7, link_to_register(link_connection(0,2))) ) {
		print_spew("No connection to Node 2.\r\n");
		result.nodes = 2;
		return result;
	}

	/* We found 3 nodes so far. Now setup a temporary
	 * connection from node 0 to node 3 via node 1
	 */

	setup_temp_row(0,1, result.nodes); /* temp. link between nodes 0 and 1 */
	setup_temp_row(1,3, result.nodes); /* temp. link between nodes 1 and 3 */

	if (!check_connection(1, 7, link_to_register(link_connection(1,3)))) {
		print_spew("No connection to Node 3.\r\n");
		result.nodes = 2;
		return result;
	}

	/* We found 4 nodes so far. Now setup all nodes for 4p */

	setup_node(0, result.nodes);  /* The first 2 nodes are configured    */
	setup_node(1, result.nodes);  /* already. Just configure them for 4p */
	
	setup_temp_row(0,2, result.nodes);
	setup_temp_node(2, result.nodes);
        rename_temp_node(2);
        enable_routing(2);
  
	setup_temp_row(0,1, result.nodes);
	setup_temp_row(1,3, result.nodes);
	setup_temp_node(3, result.nodes);
        rename_temp_node(3);
        enable_routing(3);      /* enable routing on node 3 (temp.) */
	
	/* optimize physical connections - by LYH */
	result.needs_reset = optimize_connection(
		NODE_HT(0), 0x80 + link_to_register(link_connection(0,2)),
		NODE_HT(2), 0x80 + link_to_register(link_connection(2,0)) );

	result.needs_reset = optimize_connection(
		NODE_HT(1), 0x80 + link_to_register(link_connection(1,3)),
		NODE_HT(3), 0x80 + link_to_register(link_connection(3,1)) );

	result.needs_reset = optimize_connection(
		NODE_HT(2), 0x80 + link_to_register(link_connection(2,3)),
		NODE_HT(3), 0x80 + link_to_register(link_connection(3,2)) );

#endif /* CONFIG_MAX_CPUS > 2 */

	print_debug_hex8(result.nodes);
	print_debug(" nodes initialized.\r\n");
	return result;
}
#endif

static unsigned verify_mp_capabilities(unsigned nodes)
{
	unsigned node, row, mask;
	bool mp_cap=TRUE;

	if (nodes > 2) {
		mask=0x06;	/* BigMPCap */
	} else if (nodes == 2) {
		mask=0x02;	/* MPCap    */
	} else {
		mask=0x00;	/* Non SMP */
	}

	for (node=0; node<nodes; node++) {
		if ((pci_read_config32(NODE_MC(node), 0xe8) & mask) != mask) {
			mp_cap = FALSE;
		}
	}

	if (mp_cap) {
		return nodes;
	}

	/* one of our nodes is not mp capable */

	print_err("One of the CPUs is not MP capable. Going back to UP\r\n");
	return 1;
}

static void clear_dead_routes(unsigned nodes)
{
	int last_row;
	int node, row;
	last_row = nodes;
	if (nodes == 1) {
		last_row = 0;
	}
	for(node = 7; node >= 0; node--) {
		for(row = 7; row >= last_row; row--) {
			fill_row(node, row, DEFAULT);
		}
	}
}

static void coherent_ht_finalize(unsigned nodes)
{
	unsigned node;
	bool rev_a0;
	
	/* set up cpu count and node count and enable Limit
	 * Config Space Range for all available CPUs.
	 * Also clear non coherent hypertransport bus range
	 * registers on Hammer A0 revision.
	 */

	print_spew("coherent_ht_finalize\r\n");
	rev_a0 = is_cpu_rev_a0();
	for (node = 0; node < nodes; node++) {
		device_t dev;
		uint32_t val;
		dev = NODE_HT(node);

		/* Set the Total CPU and Node count in the system */
		val = pci_read_config32(dev, 0x60);
		val &= (~0x000F0070);
		val |= ((nodes-1)<<16)|((nodes-1)<<4);
		pci_write_config32(dev, 0x60, val);

		/* Only respond to real cpu pci configuration cycles
		 * and optimize the HT settings 
		 */
		val=pci_read_config32(dev, 0x68);
		val &= ~((HTTC_BUF_REL_PRI_MASK << HTTC_BUF_REL_PRI_SHIFT) |
			(HTTC_MED_PRI_BYP_CNT_MASK << HTTC_MED_PRI_BYP_CNT_SHIFT) |
			(HTTC_HI_PRI_BYP_CNT_MASK << HTTC_HI_PRI_BYP_CNT_SHIFT));
		val |= HTTC_LIMIT_CLDT_CFG | 
			(HTTC_BUF_REL_PRI_8 << HTTC_BUF_REL_PRI_SHIFT) |
			HTTC_RSP_PASS_PW |
			(3 << HTTC_MED_PRI_BYP_CNT_SHIFT) |
			(3 << HTTC_HI_PRI_BYP_CNT_SHIFT);
		pci_write_config32(dev, 0x68, val);

		if (rev_a0) {
			print_spew("shit it is an old cup\n");
			pci_write_config32(dev, 0x94, 0);
			pci_write_config32(dev, 0xb4, 0);
			pci_write_config32(dev, 0xd4, 0);
		}
	}

	print_spew("done\r\n");
}

static int apply_cpu_errata_fixes(unsigned nodes, int needs_reset)
{
	unsigned node;
	for(node = 0; node < nodes; node++) {
		device_t dev;
		uint32_t cmd;
		dev = NODE_MC(node);
		if (is_cpu_pre_c0()) {

			/* Errata 66
			 * Limit the number of downstream posted requests to 1 
			 */
			cmd = pci_read_config32(dev, 0x70);
			if ((cmd & (3 << 0)) != 2) {
				cmd &= ~(3<<0);
				cmd |= (2<<0);
				pci_write_config32(dev, 0x70, cmd );
				needs_reset = 1;
			}
			cmd = pci_read_config32(dev, 0x7c);
			if ((cmd & (3 << 4)) != 0) {
				cmd &= ~(3<<4);
				cmd |= (0<<4);
				pci_write_config32(dev, 0x7c, cmd );
				needs_reset = 1;
			}
			/* Clock Power/Timing Low */
			cmd = pci_read_config32(dev, 0xd4);
			if (cmd != 0x000D0001) {
				cmd = 0x000D0001;
				pci_write_config32(dev, 0xd4, cmd);
				needs_reset = 1; /* Needed? */
			}

		}
		else {
			uint32_t cmd_ref;
			/* Errata 98 
			 * Set Clk Ramp Hystersis to 7
			 * Clock Power/Timing Low
			 */
			cmd_ref = 0x04e20707; /* Registered */
			cmd = pci_read_config32(dev, 0xd4);
			if(cmd != cmd_ref) {
				pci_write_config32(dev, 0xd4, cmd_ref );
				needs_reset = 1; /* Needed? */
			}
		}
	}
	return needs_reset;
}

static int optimize_link_read_pointers(unsigned nodes, int needs_reset)
{
	unsigned node;
	for(node = 0; node < nodes; node = node + 1) {
		device_t f0_dev, f3_dev;
		uint32_t cmd_ref, cmd;
		int link;
		f0_dev = NODE_HT(node);
		f3_dev = NODE_MC(node);
		cmd_ref = cmd = pci_read_config32(f3_dev, 0xdc);
		for(link = 0; link < 3; link = link + 1) {
			uint32_t link_type;
			unsigned reg;
			/* This works on an Athlon64 because unimplemented links return 0 */
			reg = 0x98 + (link * 0x20);
			link_type = pci_read_config32(f0_dev, reg);
			if (link_type & LinkConnected) {
				cmd &= 0xff << (link *8);
				/* FIXME this assumes the device on the other
				 * side is an AMD device 
				 */
				cmd |= 0x25 << (link *8);
			}
		}
		if (cmd != cmd_ref) {
			pci_write_config32(f3_dev, 0xdc, cmd);
			needs_reset = 1;
		}
	}
	return needs_reset;
}

static int setup_coherent_ht_domain(void)
{
	struct setup_smp_result result;
	result.nodes = 1;
	result.needs_reset = 0;

	enable_bsp_routing();

#if CONFIG_MAX_CPUS > 1
	result = setup_smp();
#endif
	result.nodes = verify_mp_capabilities(result.nodes);
	clear_dead_routes(result.nodes);
	if (result.nodes == 1) {
		setup_uniprocessor();
	}
	coherent_ht_finalize(result.nodes);
	result.needs_reset = apply_cpu_errata_fixes(result.nodes, result.needs_reset);
	result.needs_reset = optimize_link_read_pointers(result.nodes, result.needs_reset);
	return result.needs_reset;
}
