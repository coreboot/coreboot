/*
 * This file is part of the coreboot project.
 *
 * written by Stefan Reinauer <stepan@openbios.org>
 * (c) 2003-2004 by SuSE Linux AG
 *
 * (c) 2004 Tyan Computer
 *  2004.12 yhlu added support to create routing table dynamically.
 *          it also support 8 ways too. (8 ways ladder or 8 ways crossbar)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* coherent hypertransport initialization for AMD64
 */

/*
 * This algorithm assumes a grid configuration as follows:
 *
 * nodes :  1    2    4    6    8
 * org.  :  1x1  2x1  2x2  2x3  2x4
 Ladder:
			CPU7-------------CPU6
			|                |
			|                |
			|                |
			|                |
			|                |
			|                |
			CPU5-------------CPU4
			|                |
			|                |
			|                |
			|                |
			|                |
			|                |
			CPU3-------------CPU2
			|                |
			|                |
			|                |
			|                |
			|                |
			|                |
			CPU1-------------CPU0
 CROSS_BAR_47_56:
			CPU7-------------CPU6
			|  \____    ___/ |
			|       \  /     |
			|        \/      |
			|        /\      |
			|       /  \     |
			|  ____/    \___ |
			CPU5             CPU4
			|                |
			|                |
			|                |
			|                |
			|                |
			|                |
			CPU3-------------CPU2
			|                |
			|                |
			|                |
			|                |
			|                |
			|                |
			CPU1-------------CPU0
 */

#include <console/console.h>
#include <cpu/amd/model_fxx_rev.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/hypertransport_def.h>
#include <lib.h>
#include <stdlib.h>
#include <stdint.h>
#include <arch/io.h>
#include <pc80/mc146818rtc.h>
#if IS_ENABLED(CONFIG_HAVE_OPTION_TABLE)
#include "option_table.h"
#endif

#include "amdk8.h"

#define enable_bsp_routing()	enable_routing(0)

#define DEFAULT 0x00010101	/* default row entry */


#ifndef CROSS_BAR_47_56
	#define CROSS_BAR_47_56 0
#endif

#ifndef TRY_HIGH_FIRST
	#define TRY_HIGH_FIRST 0
#endif

#ifndef K8_HT_CHECK_PENDING_LINK
	#if CONFIG_MAX_PHYSICAL_CPUS >= 4
		#define K8_HT_CHECK_PENDING_LINK 1
	#else
		#define K8_HT_CHECK_PENDING_LINK 0
	#endif
#endif

#ifndef CONFIG_MAX_PHYSICAL_CPUS_4_BUT_MORE_INSTALLED
	#define CONFIG_MAX_PHYSICAL_CPUS_4_BUT_MORE_INSTALLED 0
#endif

static inline void print_linkn (const char *strval, uint8_t byteval)
{
	printk(BIOS_DEBUG, "%s%02x\n", strval, byteval);
}

static void disable_probes(void)
{
	/* disable read/write/fill probes for uniprocessor setup
	 * they don't make sense if only one CPU is available
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

	printk(BIOS_SPEW, "Disabling read/write/fill probes for UP... ");

	val = pci_read_config32(NODE_HT(0), HT_TRANSACTION_CONTROL);
	val |= HTTC_DIS_FILL_P | HTTC_DIS_RMT_MEM_C | HTTC_DIS_P_MEM_C |
		HTTC_DIS_MTS | HTTC_DIS_WR_DW_P | HTTC_DIS_WR_B_P |
		HTTC_DIS_RD_DW_P | HTTC_DIS_RD_B_P;
	pci_write_config32(NODE_HT(0), HT_TRANSACTION_CONTROL, val);

	printk(BIOS_SPEW, "done.\n");

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
	printk(BIOS_SPEW, "Enabling routing table for node %d", node);

	val = pci_read_config32(NODE_HT(node), 0x6c);
	val &= ~((1<<1)|(1<<0));
	pci_write_config32(NODE_HT(node), 0x6c, val);

	printk(BIOS_SPEW, " done.\n");
}

#if CONFIG_MAX_PHYSICAL_CPUS > 1
static void fill_row(u8 node, u8 row, u32 value)
{
	pci_write_config32(NODE_HT(node), 0x40+(row<<2), value);
}

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
	printk(BIOS_SPEW, "Unknown Link\n");
	return 0;
}

static u32 get_row(u8 node, u8 row)
{
	return pci_read_config32(NODE_HT(node), 0x40+(row<<2));
}

static int link_connection(u8 src, u8 dest)
{
	return get_row(src, dest) & 0x0f;
}

static void rename_temp_node(u8 node)
{
	uint32_t val;

	printk(BIOS_SPEW, "Renaming current temporary node to %d", node);

	val = pci_read_config32(NODE_HT(7), 0x60);
	val &= (~7); /* clear low bits. */
	val |= node; /* new node        */
	pci_write_config32(NODE_HT(7), 0x60, val);

	printk(BIOS_SPEW, " done.\n");
}

static int verify_connection(u8 dest)
{
	/* See if we have a valid connection to dest */
	u32 val;

	/* Verify that the coherent hypertransport link is
	 * established and actually working by reading the
	 * remode node's vendor/device id
	 */
	val = pci_read_config32(NODE_HT(dest),0);
	if (val != 0x11001022)
		return 0;

	return 1;
}

static uint16_t read_freq_cap(pci_devfn_t dev, uint8_t pos)
{
	/* Handle bugs in valid hypertransport frequency reporting */
	uint16_t freq_cap;
	uint32_t id;

	freq_cap = pci_read_config16(dev, pos);
	freq_cap &= ~(1 << HT_FREQ_VENDOR); /* Ignore Vendor HT frequencies */

#if IS_ENABLED(CONFIG_K8_HT_FREQ_1G_SUPPORT)
	#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
		if (!is_cpu_pre_e0())
	#endif
	{
		return freq_cap;
	}
#endif

	id = pci_read_config32(dev, 0);

	/* AMD K8 Unsupported 1GHz? */
	if (id == (PCI_VENDOR_ID_AMD | (0x1100 << 16))) {
		freq_cap &= ~(1 << HT_FREQ_1000Mhz);
	}

	return freq_cap;
}

static int optimize_connection(pci_devfn_t node1, uint8_t link1,
		pci_devfn_t node2, uint8_t link2)
{
	static const uint8_t link_width_to_pow2[]= { 3, 4, 0, 5, 1, 2, 0, 0 };
	static const uint8_t pow2_to_link_width[] = { 0x7, 4, 5, 0, 1, 3 };
	uint16_t freq_cap1, freq_cap2;
	uint8_t width_cap1, width_cap2, width, old_width, ln_width1, ln_width2;
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
	old_freq &= 0x0f;
	needs_reset |= old_freq != freq;
	old_freq = pci_read_config8(node2, link2 + PCI_HT_CAP_HOST_FREQ);
	old_freq &= 0x0f;
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
	old_width &= 0x77;
	needs_reset |= old_width != width;

	/* Set node1's widths */
	pci_write_config8(node1, link1 + PCI_HT_CAP_HOST_WIDTH + 1, width);

	// * Calculate node2's width */
	width = ((width & 0x70) >> 4) | ((width & 0x7) << 4);

	/* See if I am changing node2's width */
	old_width = pci_read_config8(node2, link2 + PCI_HT_CAP_HOST_WIDTH + 1);
	old_width &= 0x77;
	needs_reset |= old_width != width;

	/* Set node2's widths */
	pci_write_config8(node2, link2 + PCI_HT_CAP_HOST_WIDTH + 1, width);

	return needs_reset;
}

static uint8_t get_linkn_first(uint8_t byte)
{
	if (byte & 0x02) { byte = 0; }
	else if (byte & 0x04) { byte = 1; }
	else if (byte & 0x08) { byte = 2; }
	return byte;
}

#if TRY_HIGH_FIRST == 1
static uint8_t get_linkn_last(uint8_t byte)
{
	if (byte & 0x02) { byte &= 0x0f; byte |= 0x00; }
	if (byte & 0x04) { byte &= 0x0f; byte |= 0x10; }
	if (byte & 0x08) { byte &= 0x0f; byte |= 0x20; }
	return byte>>4;
}
#endif

#if (CONFIG_MAX_PHYSICAL_CPUS > 2) || CONFIG_MAX_PHYSICAL_CPUS_4_BUT_MORE_INSTALLED
static uint8_t get_linkn_last_count(uint8_t byte)
{
	byte &= 0x0f;
	if (byte & 0x02) { byte &= 0xcf; byte |= 0x00; byte+=0x40; }
	if (byte & 0x04) { byte &= 0xcf; byte |= 0x10; byte+=0x40; }
	if (byte & 0x08) { byte &= 0xcf; byte |= 0x20; byte+=0x40; }
	return byte>>4;
}
#endif

static void setup_row_local(u8 source, u8 row) /* source will be 7 when it is for temp use*/
{
	uint8_t linkn;
	uint32_t val;
	val = 1;
	for (linkn = 0; linkn < 3; linkn++) {
		uint8_t regpos;
		uint32_t reg;
		regpos = 0x98 + 0x20 * linkn;
		reg = pci_read_config32(NODE_HT(source), regpos);
		if ((reg & 0x17) != 3) continue; /* it is not conherent or not connected*/
		val |= 1<<(linkn+1);
	}
	val <<= 16;
	val |= 0x0101;
	fill_row(source,row, val);
}

static void setup_row_direct_x(u8 temp, u8 source, u8 dest, u8 linkn)
{
	uint32_t val;
	uint32_t val_s;
	val = 1<<(linkn+1);
	val |= 1<<(linkn+1+8); /*for direct connect response route should equal to request table*/

	if (((source &1)!=(dest &1))
#if CROSS_BAR_47_56
		&& ((source < 4)||(source > 5)) //(6,7) (7,6) should still be here
					      //(6,5) (7,4) should be here
#endif
	) {
		val |= (1<<16);
	} else {
		/*for CROSS_BAR_47_56  47, 56, should be here too
			and for 47, 56, 57, 75, 46, 64 we need to substract another link to
				6,  7,  6,  6,  7,  7
		*/
		val_s = get_row(temp, source);
		val |= ((val_s>>16) - (1<<(linkn+1)))<<16;
	}

	fill_row(temp,dest, val);
}

#if CROSS_BAR_47_56
static void opt_broadcast_rt(u8 source, u8 dest, u8 kickout)
{
	uint32_t val;
	val = get_row(source, dest);
	val -= link_connection(source, kickout)<<16;
	fill_row(source, dest, val);
}

static void opt_broadcast_rt_group(const u8 *conn, int num)
{
	int i;

	for (i = 0; i < num; i+=3) {
		opt_broadcast_rt(conn[i], conn[i+1],conn[i+2]);
	}
}

static void opt_broadcast_rt_plus(u8 source, u8 dest, u8 kickout)
{
	uint32_t val;
	val = get_row(source, dest);
	val += link_connection(source, kickout)<<16;
	fill_row(source, dest, val);
}

static void opt_broadcast_rt_plus_group(const u8 *conn, int num)
{
	int i;

	for (i = 0; i < num; i+=3) {
		opt_broadcast_rt_plus(conn[i], conn[i+1],conn[i+2]);
	}
}
#endif

static void setup_row_direct(u8 source, u8 dest, u8 linkn)
{
	setup_row_direct_x(source, source, dest, linkn);
}

static void setup_remote_row_direct(u8 source, u8 dest, u8 linkn)
{
	setup_row_direct_x(7, source, dest, linkn);
}

static void setup_temp_row(u8 source, u8 dest)
{
	/* copy value from (source, dest) to (source,7) */
	fill_row(source, 7, get_row(source, dest));
}

static void setup_remote_node(u8 node)
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
	int i;

	printk(BIOS_SPEW, "setup_remote_node: ");

	/* copy the default resource map from node 0 */
	for (i = 0; i < ARRAY_SIZE(pci_reg); i++) {
		uint32_t value;
		uint8_t reg;
		reg = pci_reg[i];
		value = pci_read_config32(NODE_MP(0), reg);
		pci_write_config32(NODE_MP(7), reg, value);

	}
	printk(BIOS_SPEW, "done\n");
}

#endif /* CONFIG_MAX_PHYSICAL_CPUS > 1*/


#if CONFIG_MAX_PHYSICAL_CPUS > 2
#if !CROSS_BAR_47_56
static void setup_row_indirect_x(u8 temp, u8 source, u8 dest)
#else
static void setup_row_indirect_x(u8 temp, u8 source, u8 dest, u8 gateway, u8 diff)
#endif
{
	/*for indirect connection, we need to compute the val from val_s(source, source), and val_g(source, gateway) */
	uint32_t val_s;
	uint32_t val;
#if !CROSS_BAR_47_56
	u8 gateway;
	u8 diff;
	if (source < dest) {
		gateway = source + 2;
	} else {
		gateway = source - 2;
	}
#endif
	val_s = get_row(temp, source);
	val = get_row(temp, gateway);

	val &= 0xffff;
	val_s >>= 16;
	val_s &= 0xfe;

#if !CROSS_BAR_47_56
	diff = ((source&1)!=(dest &1));
#endif

	if (diff && (val_s!=(val&0xff))) { /* use another connect as response*/
		val_s -= val & 0xff;
#if (CONFIG_MAX_PHYSICAL_CPUS > 4) || CONFIG_MAX_PHYSICAL_CPUS_4_BUT_MORE_INSTALLED
		uint8_t byte;
		/* Some node have two links left
		 * don't worry we only have (2, (3 as source need to handle
		 */
		byte = val_s;
		byte = get_linkn_last_count(byte);
		if ((byte>>2)>1) { /* make sure not the corner*/
			if (source < dest) {
				val_s-=link_connection(temp, source-2); /* -down*/
			} else {
#if CROSS_BAR_47_56
				if (source < gateway) { // for 5, 4 via 7
					val_s-=link_connection(temp, source-2);
				} else
#endif
					val_s-=link_connection(temp, source+2); /* -up*/
			}
		}
#endif
		val &= 0xff;
		val |= (val_s<<8);
	}

	if (diff) { /* cross rung?*/
		val |= (1<<16);
	}
	else {
		val_s = get_row(temp, source);
		val |= ((val_s>>16) - link_connection(temp, gateway))<<16;
	}

	fill_row(temp, dest, val);

}

#if !CROSS_BAR_47_56
static void setup_row_indirect(u8 source, u8 dest)
{
	setup_row_indirect_x(source, source, dest);
}
#else
static void setup_row_indirect(u8 source, u8 dest, u8 gateway, u8 diff)
{
	setup_row_indirect_x(source, source, dest, gateway, diff);
}
#endif

static void setup_row_indirect_group(const u8 *conn, int num)
{
	int i;

#if !CROSS_BAR_47_56
	for (i = 0; i < num; i+=2) {
		setup_row_indirect(conn[i], conn[i+1]);
#else
	for (i = 0; i < num; i+=4) {
		setup_row_indirect(conn[i], conn[i+1],conn[i+2], conn[i+3]);
#endif

	}
}

#if !CROSS_BAR_47_56
static void setup_remote_row_indirect(u8 source, u8 dest)
{
	setup_row_indirect_x(7, source, dest);
}
#else
static void setup_remote_row_indirect(u8 source, u8 dest, u8 gateway, u8 diff)
{
	setup_row_indirect_x(7, source, dest, gateway, diff);
}
#endif

static void setup_remote_row_indirect_group(const u8 *conn, int num)
{
	int i;

#if !CROSS_BAR_47_56
	for (i = 0; i < num; i+=2) {
		setup_remote_row_indirect(conn[i], conn[i+1]);
#else
	for (i = 0; i < num; i+=4) {
		setup_remote_row_indirect(conn[i], conn[i+1],conn[i+2], conn[i+3]);
#endif
	}
}

#endif /*CONFIG_MAX_PHYSICAL_CPUS > 2*/


static void setup_uniprocessor(void)
{
	printk(BIOS_SPEW, "Enabling UP settings\n");
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	unsigned tmp = (pci_read_config32(NODE_MC(0), 0xe8) >> 12) & 3;
	if (tmp > 0) return;
#endif
	disable_probes();
}

#if CONFIG_MAX_PHYSICAL_CPUS > 2
static int optimize_connection_group(const u8 *opt_conn, int num)
{
	int needs_reset = 0;
	int i;
	for (i = 0; i < num; i+=2) {
		needs_reset = optimize_connection(
			NODE_HT(opt_conn[i]), 0x80 + link_to_register(link_connection(opt_conn[i],opt_conn[i+1])),
			NODE_HT(opt_conn[i+1]), 0x80 + link_to_register(link_connection(opt_conn[i+1],opt_conn[i])));
	}
	return needs_reset;
}
#endif

#if CONFIG_MAX_PHYSICAL_CPUS > 1
static unsigned setup_smp2(void)
{
	unsigned nodes;
	u8 byte;
	uint32_t val;
	nodes = 2;

	setup_row_local(0, 0); /* it will update the broadcast RT*/

	val = get_row(0,0);
	byte = (val>>16) & 0xfe;
	if (byte < 0x2) { /* no coherent connection so get out.*/
		nodes = 1;
		return nodes;
	}

	/* Setup and check a temporary connection to node 1 */
#if TRY_HIGH_FIRST == 1
	byte = get_linkn_last(byte); /* Max Link to node1 */
#else
	byte = get_linkn_first(byte); /*Min Link to node1 --- according to AMD*/
#endif
	print_linkn("(0,1) link=", byte);
	setup_row_direct(0,1, byte);
	setup_temp_row(0, 1);

	verify_connection(7);

	/* We found 2 nodes so far */
	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /*get default link on node7 to node0*/
	print_linkn("(1,0) link=", byte);
	setup_row_local(7,1);
	setup_remote_row_direct(1, 0, byte);

#if (CONFIG_MAX_PHYSICAL_CPUS > 4) || CONFIG_MAX_PHYSICAL_CPUS_4_BUT_MORE_INSTALLED
	val = get_row(7,1);
	byte = (val>>16) & 0xfe;
	byte = get_linkn_last_count(byte);
	if ((byte>>2) == 3) { /* Oh! we need to treat it as node2. So use another link*/
		val = get_row(0,0);
		byte = (val>>16) & 0xfe;
#if TRY_HIGH_FIRST == 1
		byte = get_linkn_first(byte); /* Min link to Node1 */
#else
		byte = get_linkn_last(byte);  /* Max link to Node1*/
#endif
		print_linkn("\t-->(0,1) link=", byte);
		setup_row_direct(0,1, byte);
		setup_temp_row(0, 1);

		verify_connection(7);

		/* We found 2 nodes so far */
		val = pci_read_config32(NODE_HT(7), 0x6c);
		byte = (val>>2) & 0x3; /* get default link on node7 to node0*/
		print_linkn("\t-->(1,0) link=", byte);
		setup_row_local(7,1);
		setup_remote_row_direct(1, 0, byte);
	}
#endif

	setup_remote_node(1); /* Setup the regs on the remote node */
	rename_temp_node(1);  /* Rename Node 7 to Node 1  */
	enable_routing(1);    /* Enable routing on Node 1 */

	return nodes;
}
#endif /*CONFIG_MAX_PHYSICAL_CPUS > 1 */

#if CONFIG_MAX_PHYSICAL_CPUS > 2

static unsigned setup_smp4(void)
{
	unsigned nodes;
	u8 byte;
	uint32_t val;

	nodes = 4;

	/* Setup and check temporary connection from Node 0 to Node 2 */
	val = get_row(0,0);
	byte = ((val>>16) & 0xfe) - link_connection(0,1);
	byte = get_linkn_last_count(byte);

	if ((byte>>2) == 0) { /* We should have two coherent for 4p and above*/
		nodes = 2;
		return nodes;
	}

	byte &= 3; /* bit [3,2] is count-1*/
	print_linkn("(0,2) link=", byte);
	setup_row_direct(0, 2, byte); /*(0,2) direct link done*/

	/* We found 3 nodes so far. Now setup a temporary
	* connection from node 0 to node 3 via node 1
	*/
	setup_temp_row(0,1); /* temp. link between nodes 0 and 1 */
	/* here should setup_row_direct(1,3) at first, before that we should find the link in node 1 to 3*/
	val = get_row(1,1);
	byte = ((val>>16) & 0xfe) - link_connection(1,0);
	byte = get_linkn_first(byte);
	print_linkn("(1,3) link=", byte);
	setup_row_direct(1,3,byte); /* (1, 3) direct link done*/

	/* We found 4 nodes so far. Now setup all nodes for 4p */
	// We need to make sure 0,2 and 1,3 link is set already
#if !CROSS_BAR_47_56
	static const u8 conn4_1[] = {
		0,3,
		1,2,
	};
#else
	static const u8 conn4_1[] = {
		0,3,2,1,
		1,2,3,1,
	};
#endif

	setup_row_indirect_group(conn4_1, ARRAY_SIZE(conn4_1));

	setup_temp_row(0,2);
	verify_connection(7);
	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 0*/
	print_linkn("(2,0) link=", byte);

	setup_row_local(7,2);
	setup_remote_row_direct(2, 0, byte); /* node 2 to node 0 direct link done */
	setup_remote_node(2);  /* Setup the regs on the remote node */

	rename_temp_node(2);   /* Rename Node 7 to Node 2  */
	enable_routing(2);     /* Enable routing on Node 2 */

	setup_temp_row(0,1);
	setup_temp_row(1,3);
	verify_connection(7);

	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 1*/
	print_linkn("(3,1) link=", byte);

	setup_row_local(7,3);
	setup_remote_row_direct(3, 1, byte); /* node 3 to node 1 direct link done */
	setup_remote_node(3);  /* Setup the regs on the remote node */

	/* We need to init link between 2, and 3 direct link */
	val = get_row(2,2);
	byte = ((val>>16) & 0xfe) - link_connection(2,0);
	byte = get_linkn_last_count(byte);
	print_linkn("(2,3) link=", byte & 3);

	setup_row_direct(2,3, byte & 0x3);
	setup_temp_row(0,2);
	setup_temp_row(2,3);
	verify_connection(7); /* to 3*/

#if (CONFIG_MAX_PHYSICAL_CPUS > 4) || CONFIG_MAX_PHYSICAL_CPUS_4_BUT_MORE_INSTALLED
	/* We need to find out which link is to node3 */
	if ((byte>>2) == 2) { /* one to node3, one to node0, one to node4*/
		val = get_row(7,3);
		if ((val>>16) == 1) { /* that link is to node4, because via node1 it has been set, recompute it*/
			val = get_row(2,2);
			byte = ((val>>16) & 0xfe) - link_connection(2,0);
			byte = get_linkn_first(byte);
			print_linkn("\t-->(2,3) link=", byte);
			setup_row_direct(2,3,byte);
			setup_temp_row(2,3);
			verify_connection(7); /* to 3*/
		}
	}
#endif

	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 2*/
	print_linkn("(3,2) link=", byte);
	setup_remote_row_direct(3,2, byte);

#if (CONFIG_MAX_PHYSICAL_CPUS > 4) || CONFIG_MAX_PHYSICAL_CPUS_4_BUT_MORE_INSTALLED
	/* set link from 3 to 5 before enable it*/
	val = get_row(7,3);
	byte = ((val>>16) & 0xfe) - link_connection(7,2) - link_connection(7,1);
	byte = get_linkn_last_count(byte);
	if ((byte>>2) == 1) { /* We should have three coherent links on node 3 for 6p and above*/
		byte &= 3;  /*bit [3,2] is count-2*/
		print_linkn("(3,5) link=", byte);
		setup_remote_row_direct(3, 5, byte);
	}

	val = get_row(2,2);
	byte = ((val>>16) & 0xfe) - link_connection(2,3) - link_connection(2,0);
	byte = get_linkn_last_count(byte);

	if ((byte>>2) == 1) { /* We should have three coherent link on node 2 for 6p and above*/
		byte &= 3;  /* bit [3,2] is count-2*/
		print_linkn("(2,4) link=", byte);
		setup_row_direct(2, 4, byte);
	}
#endif

	//Beside 3, 1 is set, We need to make sure 3, 5 is set already in case has three link in 3
#if !CROSS_BAR_47_56
	static const u8 conn4_3[] = {
		3,0,
	};
#else
	static const u8 conn4_3[] = {
		3,0,1,1,
	};
#endif
	setup_remote_row_indirect_group(conn4_3, ARRAY_SIZE(conn4_3));

/* ready to enable RT for Node 3 */
	rename_temp_node(3);
	enable_routing(3);  /* enable routing on node 3 (temp.) */

	// beside 2, 0 is set, We need to make sure 2, 4 link is set already in case has three link in 2
#if !CROSS_BAR_47_56
	static const u8 conn4_2[] = {
		2,1,
	};
#else
	static const u8 conn4_2[] = {
		2,1,0,1,
	};
#endif
	setup_row_indirect_group(conn4_2, ARRAY_SIZE(conn4_2));

	return nodes;

}

#endif /* CONFIG_MAX_PHYSICAL_CPUS > 2 */

#if CONFIG_MAX_PHYSICAL_CPUS > 4

static unsigned setup_smp6(void)
{
	unsigned nodes;
	u8 byte;
	uint32_t val;

	nodes = 6;

	/* Setup and check temporary connection from Node 0 to Node 4 through 2*/
	val = get_row(2,2);
	byte = ((val>>16) & 0xfe) - link_connection(2,3) - link_connection(2,0);
	byte = get_linkn_last_count(byte);

	if ((byte>>2) == 0) { /* We should have three coherent link on node 2 for 6p and above*/
		nodes = 4;
		return nodes;
	}

	/* Setup and check temporary connection from Node 0 to Node 5 through 1, 3*/
	/* set link from 3 to 5 before enable it*/
	val = get_row(3,3);
	byte = ((val>>16) & 0xfe) - link_connection(3,2) - link_connection(3,1);
	byte = get_linkn_last_count(byte);
	if ((byte>>2) == 0) { /* We should have three coherent links on node 3 for 6p and above*/
		nodes = 4;
		return nodes;
	}

	/* We found 6 nodes so far. Now setup all nodes for 6p */
#warning "FIXME we need to find out the correct gateway for 6p"
	static const u8 conn6_1[] = {
#if !CROSS_BAR_47_56
		0, 4,
		0, 5,
		1, 4,
		1, 5,
		2, 5,
		3, 4,
#else
		0, 4, 2, 0,
		0, 5, 2, 1,
		1, 4, 3, 1,
		1, 5, 3, 0,
		2, 5, 3, 0,
		3, 4, 2, 0,
#endif
	};

	setup_row_indirect_group(conn6_1, ARRAY_SIZE(conn6_1));

	for (byte = 0; byte < 4; byte+=2) {
		setup_temp_row(byte,byte+2);
	}
	verify_connection(7);
	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /*get default link on 7 to 2*/
	print_linkn("(4,2) link=", byte);

	setup_row_local(7,4);
	setup_remote_row_direct(4, 2, byte);
	setup_remote_node(4);  /* Setup the regs on the remote node */

	/* Set indirect connection to 0, to 3 */
	//we only need to set 4,0 here
	static const u8 conn6_2[] = {
#if !CROSS_BAR_47_56
		4, 0,
#else
		4, 0, 2, 0,
#endif
	};

	setup_remote_row_indirect_group(conn6_2, ARRAY_SIZE(conn6_2));

	rename_temp_node(4);
	enable_routing(4);

	setup_temp_row(0,1);
	for (byte = 0; byte < 4; byte+=2) {
		setup_temp_row(byte+1,byte+3);
	}
	verify_connection(7);

	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 3*/
	print_linkn("(5,3) link=", byte);
	setup_row_local(7,5);
	setup_remote_row_direct(5, 3, byte);
	setup_remote_node(5);  /* Setup the regs on the remote node */

#if !CROSS_BAR_47_56
	/* We need to init link between 4, and 5 direct link */
	val = get_row(4,4);
	byte = ((val>>16) & 0xfe) - link_connection(4,2);
	byte = get_linkn_last_count(byte);
	print_linkn("(4,5) link=", byte & 3);

	setup_row_direct(4,5, byte & 0x3);
	setup_temp_row(0,2);
	setup_temp_row(2,4);
	setup_temp_row(4,5);
	verify_connection(7); /* to 5*/

#if CONFIG_MAX_PHYSICAL_CPUS > 6
	/* We need to find out which link is to node5 */

	if ((byte>>2) == 2) { /* one to node5, one to node2, one to node6*/
		val = get_row(7,5);
		if ((val>>16) == 1) { /* that link is to node6, because via node 3 node 5 has been set*/
			val = get_row(4,4);
			byte = ((val>>16) & 0xfe) - link_connection(4,2);
			byte = get_linkn_first(byte);
			print_linkn("\t-->(4,5) link=", byte);
			setup_row_direct(4,5,byte);
			setup_temp_row(4,5);
			verify_connection(7); /* to 5*/
		}
	}
#endif

	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 4*/
	print_linkn("(5,4) link=", byte);
	setup_remote_row_direct(5,4, byte);

	//init 5, 7 here
	val = get_row(7,5);
	byte = ((val>>16) & 0xfe) - link_connection(7,4) - link_connection(7,3);
	byte = get_linkn_last_count(byte);
	if ((byte>>2) == 1) { /* We should have three coherent links on node 5 for 6p and above*/
		byte &= 3; /*bit [3,2] is count-2*/
		print_linkn("(5,7) link=", byte);
		setup_remote_row_direct(5, 7, byte);
	}

	//init 4,6 here
	val = get_row(4,4);
	byte = ((val>>16) & 0xfe) - link_connection(4,5) - link_connection(4,2);
	byte = get_linkn_last_count(byte);

	if ((byte>>2) == 1) { /* We should have three coherent link on node 4 for 6p and above*/
		byte &= 3; /* bit [3,2] is count-2*/
		print_linkn("(4,6) link=", byte);
		setup_row_direct(4, 6, byte);
	}

#endif

	//We need to set 5,0 here only, We need to set up 5, 7 to make 5,0
	/* Set indirect connection to 0, to 3 for indirect we will use clockwise routing */
	static const u8 conn6_3[] = {
#if !CROSS_BAR_47_56
		5, 0,
#else
		5, 0, 3, 0,
#endif
	};

	setup_remote_row_indirect_group(conn6_3, ARRAY_SIZE(conn6_3));

/* ready to enable RT for 5 */
	rename_temp_node(5);
	enable_routing(5); /* enable routing on node 5 (temp.) */

	static const u8 conn6_4[] = {
#if !CROSS_BAR_47_56
		4, 1,
		4, 3,

		5, 2,
		5, 1,

#else
		4, 1, 2, 0,
		4, 3, 2, 0,
		4, 5, 2, 0,

		5, 2, 3, 0,
		5, 1, 3, 0,
		5, 4, 3, 0,

#endif
	};

	setup_row_indirect_group(conn6_4, ARRAY_SIZE(conn6_4));

	return nodes;

}

#endif /* CONFIG_MAX_PHYSICAL_CPUS > 4 */

#if CONFIG_MAX_PHYSICAL_CPUS > 6

static unsigned setup_smp8(void)
{
	unsigned nodes;
	u8 byte;
	uint32_t val;

	nodes = 8;

	/* Setup and check temporary connection from Node 0 to Node 6 via 2 and 4 to 7 */
	val = get_row(4,4);
#if CROSS_BAR_47_56
	byte = ((val>>16) & 0xfe) - link_connection(4,2);
#else
	byte = ((val>>16) & 0xfe) - link_connection(4,5) - link_connection(4,2);
	byte = get_linkn_last_count(byte); /* Max link to 6*/
	if ((byte>>2) == 0) { /* We should have two or three coherent links on node 4 for 8p*/
		nodes = 6;
		return nodes;
	}
#endif

#if CROSS_BAR_47_56
	byte = get_linkn_last_count(byte); /* Max link to 6*/
	if ((byte>>2)<2) { /* We should have two or three coherent links on node 4 for 8p*/
		nodes = 6;
		return nodes;
	}
#if TRY_HIGH_FIRST == 1
	byte = ((val>>16) & 0xfe) - link_connection(4,2);
	byte = get_linkn_first(byte); /*Min link to 6*/
#else
	byte &= 3; /* bit [3,2] is count-1 or 2*/
#endif
	print_linkn("(4,6) link=", byte);
	setup_row_direct(4, 6, byte);
#endif

#if !CROSS_BAR_47_56
	/* Setup and check temporary connection from Node 0 to Node 7 through 1, 3, 5*/
	val = get_row(5,5);
	byte = ((val>>16) & 0xfe) - link_connection(5,4) - link_connection(5,3);
	byte = get_linkn_last_count(byte);
	if ((byte>>2) == 0) { /* We should have three coherent links on node 5 for 6p and above*/
		nodes = 6;
		return nodes;
	}
#endif

	/* We found 8 nodes so far. Now setup all nodes for 8p */
	static const u8 conn8_1[] = {
#if !CROSS_BAR_47_56
		0, 6,
		/*0, 7,*/
		1, 6,
		/*1, 7,*/
		2, 6,
		/*2, 7,*/
		3, 6,
		/*3, 7,*/
		/*4, 7,*/
		5, 6,
#else
		0, 6, 2, 0,
		/*0, 7, 2, 0,*/
		1, 6, 3, 0,
		/*1, 7, 3, 0,*/
		2, 6, 4, 0,
		/*2, 7, 4, 0,*/
		3, 6, 5, 1,
		/*3, 7, 5, 0,*/
#endif
	};

	setup_row_indirect_group(conn8_1,ARRAY_SIZE(conn8_1));

	for (byte = 0; byte < 6; byte+=2) {
		setup_temp_row(byte,byte+2);
	}
	verify_connection(7);
	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 4*/
	print_linkn("(6,4) link=", byte);

	setup_row_local(7,6);
	setup_remote_row_direct(6, 4, byte);
	setup_remote_node(6); /* Setup the regs on the remote node */
	/* Set indirect connection to 0, to 3 */
#warning "FIXME we need to find out the correct gateway for 8p"
	static const u8 conn8_2[] = {
#if !CROSS_BAR_47_56
		6, 0,
#else
		6, 0, 4, 0,
#endif
	};

	setup_remote_row_indirect_group(conn8_2, ARRAY_SIZE(conn8_2));

#if CROSS_BAR_47_56
	//init 5, 6 here
	/* here init 5, 6 */
	/* Setup and check temporary connection from Node 0 to Node 5 through 1, 3, 5*/
	val = get_row(5,5);
	byte = ((val>>16) & 0xfe) - link_connection(5,3);
#if TRY_HIGH_FIRST == 1
	byte = get_linkn_first(byte);
#else
	byte = get_linkn_last(byte);
#endif
	print_linkn("(5,6) link=", byte);
	setup_row_direct(5, 6, byte);

	setup_temp_row(0,1); /* temp. link between nodes 0 and 1 */
	for (byte = 0; byte < 4; byte+=2) {
		setup_temp_row(byte+1,byte+3);
	}
	setup_temp_row(5,6);

	verify_connection(7);

	val = get_row(7,6); // to chect it if it is node6 before renaming
	if ((val>>16) == 1) { // it is real node 7 so swap it
		/* We need to recompute link to 6 */
		val = get_row(5,5);
		byte = ((val>>16) & 0xfe) - link_connection(5,3);
#if TRY_HIGH_FIRST == 1
		byte = get_linkn_first(byte);
#else
		byte = get_linkn_last(byte);
#endif
		print_linkn("\t-->(5,6) link=", byte);
		setup_row_direct(5, 6, byte);
		setup_temp_row(5,6);

		verify_connection(7);
	}
	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 5*/
	print_linkn("(6,5) link=", byte);
	setup_remote_row_direct(6, 5, byte);
	/*Till now 56, 65 done */
#endif

	rename_temp_node(6);
	enable_routing(6);

#if !CROSS_BAR_47_56
	setup_temp_row(0,1);
	for (byte = 0; byte < 6; byte+=2) {
		setup_temp_row(byte+1,byte+3);
	}

	verify_connection(7);

	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 5*/
	print_linkn("(7,5) link=", byte);
	setup_row_local(7,7);
	setup_remote_row_direct(7, 5, byte);

#else
	val = get_row(4,4);
	byte = ((val>>16) & 0xfe) - link_connection(4,2) - link_connection(4,6);
	byte = get_linkn_first(byte);
	print_linkn("(4,7) link=", byte);
	setup_row_direct(4, 7, byte);

	/* Setup and check temporary connection from Node 0 to Node 7 through 2, and 4*/
	for (byte = 0; byte < 4; byte+=2) {
		setup_temp_row(byte,byte+2);
	}

	verify_connection(7);

	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 4*/
	print_linkn("(7,4) link=", byte);
	setup_row_local(7,7);
	setup_remote_row_direct(7, 4, byte);
	/* till now 4-7, 7-4 done. */
#endif
	setup_remote_node(7); /* Setup the regs on the remote node */

#if CROSS_BAR_47_56
	/* here init 5, 7 */
	/* Setup and check temporary connection from Node 0 to Node 5 through 1, 3, 5*/
	val = get_row(5,5);
	byte = ((val>>16) & 0xfe) - link_connection(5,3) - link_connection(5,6);
	byte = get_linkn_first(byte);
	print_linkn("(5,7) link=", byte);
	setup_row_direct(5, 7, byte);

	setup_temp_row(0,1); /* temp. link between nodes 0 and 1 */
	for (byte = 0; byte < 4; byte+=2) {
		setup_temp_row(byte+1,byte+3);
	}

	verify_connection(7);

	val = pci_read_config32(NODE_HT(7), 0x6c);
	byte = (val>>2) & 0x3; /* get default link on 7 to 5*/
	print_linkn("(7,5) link=", byte);
	setup_remote_row_direct(7, 5, byte);
	/*Till now 57, 75 done */

#endif

	/* We need to init link between 6, and 7 direct link */
	val = get_row(6,6);
#if !CROSS_BAR_47_56
	byte = ((val>>16) & 0xfe) - link_connection(6,4);
#else
	byte = ((val>>16) & 0xfe) - link_connection(6,4) - link_connection(6,5);
#endif
	byte = get_linkn_first(byte);
	print_linkn("(6,7) link=", byte);
	setup_row_direct(6,7, byte);

	val = get_row(7,7);
#if !CROSS_BAR_47_56
	byte = ((val>>16) & 0xfe) - link_connection(7,5);
#else
	byte = ((val>>16) & 0xfe) - link_connection(7,5) - link_connection(7,4);
#endif
	byte = get_linkn_first(byte);
	print_linkn("(7,6) link=", byte);
	setup_row_direct(7,6, byte);

	/* Set indirect connection to 0, to 3 for indirect we will use clockwise routing */
	static const u8 conn8_3[] = {
#if !CROSS_BAR_47_56
		0, 7, /* restore it*/
		1, 7,
		2, 7,
		3, 7,
		4, 7,

		6, 1,
		6, 2,
		6, 3,
		6, 5,

		7, 0,
		7, 1,
		7, 2,
		7, 3,
		7, 4,
#else


		4, 5, 6, 1,
		5, 4, 7, 1,

		6, 1, 5, 0, // or 4, 1
		6, 2, 4, 0,
		6, 3, 5, 0, // or 4, 1

		7, 0, 4, 0, // or 5, 1
		7, 1, 5, 0,
		7, 2, 4, 0, // or 5, 1
		7, 3, 5, 0,

		0, 7, 2, 0, /* restore it*/
		1, 7, 3, 0,
		2, 7, 4, 1,
		3, 7, 5, 0,

		2, 5, 4, 1, /* reset it */
		3, 4, 5, 1,

		4, 1, 2, 1, /* reset it */
		4, 3, 2, 1,

		5, 2, 3, 1, /* reset it */
		5, 0, 3, 1,

#endif
	};

	setup_row_indirect_group(conn8_3, ARRAY_SIZE(conn8_3));

#if CROSS_BAR_47_56
	/* for 47, 56, 57, 75, 46, 64 we need to substract another link to
	       6,  7,  6,  6,  7,  7 */
	static const u8 conn8_4[] = {
//direct
		4, 7, 6,
		5, 6, 7,
		5, 7, 6,
		7, 5, 6,
		4, 6, 7,
		6, 4, 7,

//in direct
		0, 6, 1,
		0, 7, 1,

		1, 6, 0,
		1, 7, 0,

		2, 6, 3,
//		2, 7, 3, +

//		3, 6, 1, +
		3, 7, 2,

		6, 0, 7,
		6, 1, 7, // needed for via 5
			6, 1, 4, // ???
		6, 2, 7,
		6, 3, 7, // needed for via 5
			6, 3, 4, //???
		7, 0, 6, // needed for via 4
			7, 0, 5, //???
		7, 1, 6,
		7, 2, 6, // needed for via 4
			7, 2, 5, //???
		7, 3, 6,
	};

	opt_broadcast_rt_group(conn8_4, ARRAY_SIZE(conn8_4));

	static const u8 conn8_5[] = {
		2, 7, 0,

		3, 6, 1,
	};

	opt_broadcast_rt_plus_group(conn8_5, ARRAY_SIZE(conn8_5));
#endif



/* ready to enable RT for Node 7 */
	enable_routing(7); /* enable routing on node 7 (temp.) */

	return nodes;
}

#endif /* CONFIG_MAX_PHYSICAL_CPUS > 6 */


#if CONFIG_MAX_PHYSICAL_CPUS > 1

static unsigned setup_smp(void)
{
	unsigned nodes;

	printk(BIOS_SPEW, "Enabling SMP settings\n");

	nodes = setup_smp2();
#if CONFIG_MAX_PHYSICAL_CPUS > 2
	if (nodes == 2)
		nodes = setup_smp4();
#endif

#if CONFIG_MAX_PHYSICAL_CPUS > 4
	if (nodes == 4)
		nodes = setup_smp6();
#endif

#if CONFIG_MAX_PHYSICAL_CPUS > 6
	if (nodes == 6)
		nodes = setup_smp8();
#endif

	printk(BIOS_DEBUG, "%02x nodes initialized.\n", nodes);

	return nodes;
}

static unsigned verify_mp_capabilities(unsigned nodes)
{
	unsigned node, mask;

	mask = 0x06; /* BigMPCap */

	for (node = 0; node < nodes; node++) {
		mask &= pci_read_config32(NODE_MC(node), 0xe8);
	}

	switch(mask) {
#if CONFIG_MAX_PHYSICAL_CPUS > 2
	case 0x02: /* MPCap */
		if (nodes > 2) {
			printk(BIOS_ERR, "Going back to DP\n");
			return 2;
		}
		break;
#endif
	case 0x00: /* Non SMP */
		if (nodes >1) {
			printk(BIOS_ERR, "Going back to UP\n");
			return 1;
		}
		break;
	}

	return nodes;

}


static void clear_dead_routes(unsigned nodes)
{
	int last_row;
	int node, row;
#if CONFIG_MAX_PHYSICAL_CPUS == 8
	if (nodes == 8) return;/* don't touch (7,7)*/
#endif
	last_row = nodes;
	if (nodes == 1) {
		last_row = 0;
	}
	for (node = 7; node >= 0; node--) {
		for (row = 7; row >= last_row; row--) {
			fill_row(node, row, DEFAULT);
		}
	}

	/* Update the local row */
	for (node = 0; node < nodes; node++) {
		uint32_t val = 0;
		for (row =0; row < nodes; row++) {
			val |= get_row(node, row);
		}
		fill_row(node, node, (((val & 0xff) | ((val >> 8) & 0xff)) << 16) | 0x0101);
	}
}
#endif /* CONFIG_MAX_PHYSICAL_CPUS > 1 */

#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
static unsigned verify_dualcore(unsigned nodes)
{
	unsigned node, totalcpus, tmp;

	totalcpus = 0;
	for (node = 0; node < nodes; node++) {
		tmp = (pci_read_config32(NODE_MC(node), 0xe8) >> 12) & 3;
		totalcpus += (tmp + 1);
	}

	return totalcpus;

}
#endif

static void coherent_ht_finalize(unsigned nodes)
{
	unsigned node;
#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
	int rev_a0;
#endif
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	unsigned total_cpus;

	if (read_option(multi_core, 0) == 0) { /* multi_core */
		total_cpus = verify_dualcore(nodes);
	}
	else {
		total_cpus = nodes;
	}
#endif

	/* set up CPU count and node count and enable Limit
	 * Config Space Range for all available CPUs.
	 * Also clear non coherent hypertransport bus range
	 * registers on Hammer A0 revision.
	 */

	printk(BIOS_SPEW, "coherent_ht_finalize\n");
#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
	rev_a0 = is_cpu_rev_a0();
#endif
	for (node = 0; node < nodes; node++) {
		pci_devfn_t dev;
		uint32_t val;
		dev = NODE_HT(node);

		/* Set the Total CPU and Node count in the system */
		val = pci_read_config32(dev, 0x60);
		val &= (~0x000F0070);
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
		val |= ((total_cpus-1)<<16)|((nodes-1)<<4);
#else
		val |= ((nodes-1)<<16)|((nodes-1)<<4);
#endif
		pci_write_config32(dev, 0x60, val);

		/* Only respond to real CPU pci configuration cycles
		 * and optimize the HT settings
		 */
		val = pci_read_config32(dev, HT_TRANSACTION_CONTROL);
		val &= ~((HTTC_BUF_REL_PRI_MASK << HTTC_BUF_REL_PRI_SHIFT) |
			(HTTC_MED_PRI_BYP_CNT_MASK << HTTC_MED_PRI_BYP_CNT_SHIFT) |
			(HTTC_HI_PRI_BYP_CNT_MASK << HTTC_HI_PRI_BYP_CNT_SHIFT));
		val |= HTTC_LIMIT_CLDT_CFG |
			(HTTC_BUF_REL_PRI_8 << HTTC_BUF_REL_PRI_SHIFT) |
			(3 << HTTC_MED_PRI_BYP_CNT_SHIFT) |
			(3 << HTTC_HI_PRI_BYP_CNT_SHIFT);
		pci_write_config32(dev, HT_TRANSACTION_CONTROL, val);

#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
		if (rev_a0) {
			pci_write_config32(dev, 0x94, 0);
			pci_write_config32(dev, 0xb4, 0);
			pci_write_config32(dev, 0xd4, 0);
		}
#endif
	}

	printk(BIOS_SPEW, "done\n");
}

static int apply_cpu_errata_fixes(unsigned nodes)
{
	unsigned node;
	int needs_reset = 0;
	for (node = 0; node < nodes; node++) {
		pci_devfn_t dev;
		uint32_t cmd;
		dev = NODE_MC(node);
#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
		if (is_cpu_pre_c0()) {

			/* Errata 66
			 * Limit the number of downstream posted requests to 1
			 */
			cmd = pci_read_config32(dev, 0x70);
			if ((cmd & (3 << 0)) != 2) {
				cmd &= ~(3<<0);
				cmd |= (2<<0);
				pci_write_config32(dev, 0x70, cmd);
				needs_reset = 1;
			}
			cmd = pci_read_config32(dev, 0x7c);
			if ((cmd & (3 << 4)) != 0) {
				cmd &= ~(3<<4);
				cmd |= (0<<4);
				pci_write_config32(dev, 0x7c, cmd);
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
		else if (is_cpu_pre_d0()) { // d0 later don't need it
			uint32_t cmd_ref;
			/* Errata 98
			 * Set Clk Ramp Hystersis to 7
			 * Clock Power/Timing Low
			 */
			cmd_ref = 0x04e20707; /* Registered */
			cmd = pci_read_config32(dev, 0xd4);
			if (cmd != cmd_ref) {
				pci_write_config32(dev, 0xd4, cmd_ref);
				needs_reset = 1; /* Needed? */
			}
		}
#endif


#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
		/* I can't touch this msr on early buggy cpus, and cannot apply either 169 or 131 */
		if (!is_cpu_pre_b3())
#endif
		{
			/* Errata 169 */
			/* We also need to set some bits in NB_CFG_MSR, which is handled in src/cpu/amd/model_fxx/ */
			dev = NODE_HT(node);
			cmd = pci_read_config32(dev, 0x68);
			cmd &= ~(1 << 22);
			cmd |= (1 << 21);
			pci_write_config32(dev, 0x68, cmd);
		}
	}
	return needs_reset;
}

static int optimize_link_read_pointers(unsigned nodes)
{
	unsigned node;
	int needs_reset = 0;
	for (node = 0; node < nodes; node++) {
		pci_devfn_t f0_dev, f3_dev;
		uint32_t cmd_ref, cmd;
		int link;
		f0_dev = NODE_HT(node);
		f3_dev = NODE_MC(node);
		cmd_ref = cmd = pci_read_config32(f3_dev, 0xdc);
		for (link = 0; link < 3; link++) {
			uint32_t link_type;
			unsigned reg;
			/* This works on an Athlon64 because unimplemented links return 0 */
			reg = 0x98 + (link * 0x20);
			link_type = pci_read_config32(f0_dev, reg);
			/* Only handle coherent links */
			if ((link_type & (LinkConnected | InitComplete|NonCoherent)) ==
				(LinkConnected|InitComplete))
			{
				cmd &= ~(0xff << (link *8));
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

unsigned int get_nodes(void)
{
	return ((pci_read_config32(PCI_DEV(0, 0x18, 0), 0x60)>>4) & 7) + 1;
}

int optimize_link_coherent_ht(void)
{
	int needs_reset = 0;

	unsigned nodes;

	nodes = get_nodes();

#if CONFIG_MAX_PHYSICAL_CPUS > 1
	if (nodes > 1) {
		needs_reset |= optimize_connection(
			NODE_HT(0), 0x80 + link_to_register(link_connection(0,1)),
			NODE_HT(1), 0x80 + link_to_register(link_connection(1,0)));
	}

#if CONFIG_MAX_PHYSICAL_CPUS > 2
	if (nodes > 2) {
	/* optimize physical connections - by LYH */
		static const u8 opt_conn4[] = {
			0,2,
			1,3,
			2,3,
		};
		needs_reset |= optimize_connection_group(opt_conn4, ARRAY_SIZE(opt_conn4));
	}
#endif

#if CONFIG_MAX_PHYSICAL_CPUS > 4
	if (nodes > 4) {
		static const uint8_t opt_conn6[] ={
			2, 4,
			3, 5,
	#if !CROSS_BAR_47_56
			4, 5,
	#endif
		};
		needs_reset |= optimize_connection_group(opt_conn6, ARRAY_SIZE(opt_conn6));
	}
#endif

#if CONFIG_MAX_PHYSICAL_CPUS > 6
	if (nodes > 6) {
		static const uint8_t opt_conn8[] ={
			4, 6,
	#if CROSS_BAR_47_56
			4, 7,
			5, 6,
	#endif
			5, 7,
			6, 7,
		};
		needs_reset |= optimize_connection_group(opt_conn8, ARRAY_SIZE(opt_conn8));
	}
#endif

#endif

	needs_reset |= apply_cpu_errata_fixes(nodes);
	needs_reset |= optimize_link_read_pointers(nodes);

	return needs_reset;
}

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
void setup_coherent_ht_domain(void)
#else
int setup_coherent_ht_domain(void)
#endif
{
	unsigned nodes;
	nodes = 1;

	enable_bsp_routing();

#if CONFIG_MAX_PHYSICAL_CPUS > 1
	nodes = setup_smp();
	nodes = verify_mp_capabilities(nodes);
	clear_dead_routes(nodes);
#endif

	if (nodes == 1) {
		setup_uniprocessor();
	}
	coherent_ht_finalize(nodes);

#if !IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
	return optimize_link_coherent_ht();
#endif
}
