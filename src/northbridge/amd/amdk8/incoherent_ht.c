/*
 * This file is part of the coreboot project.
 *
 * 2004.12 yhlu add multi ht chain dynamically support
 * 2005.11 yhlu add let real sb to use small unitid
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

#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/hypertransport_def.h>
#include <lib.h>
#include "amdk8.h"

// Do we need allocate MMIO? Current We direct last 64M to sblink only, We can not lose access to last 4M range to ROM
#ifndef K8_ALLOCATE_MMIO_RANGE
	#define K8_ALLOCATE_MMIO_RANGE 0
#endif

static inline void print_linkn_in (const char *strval, uint8_t byteval)
{
	printk(BIOS_DEBUG, "%s%02x\n", strval, byteval);
}

static uint8_t ht_lookup_capability(pci_devfn_t dev, uint16_t val)
{
	uint8_t pos;
	uint8_t hdr_type;

	hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
	pos = 0;
	hdr_type &= 0x7f;

	if ((hdr_type == PCI_HEADER_TYPE_NORMAL) ||
	    (hdr_type == PCI_HEADER_TYPE_BRIDGE)) {
		pos = PCI_CAPABILITY_LIST;
	}
	if (pos > PCI_CAP_LIST_NEXT) {
		pos = pci_read_config8(dev, pos);
	}
	while (pos != 0) { /* loop through the linked list */
		uint8_t cap;
		cap = pci_read_config8(dev, pos + PCI_CAP_LIST_ID);
		if (cap == PCI_CAP_ID_HT) {
			uint16_t flags;

			flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
			if ((flags >> 13) == val) {
				/* Entry is a slave or host , success... */
				break;
			}
		}
		pos = pci_read_config8(dev, pos + PCI_CAP_LIST_NEXT);
	}
	return pos;
}

static uint8_t ht_lookup_slave_capability(pci_devfn_t dev)
{
	return ht_lookup_capability(dev, 0); // Slave/Primary Interface Block Format
}

static void ht_collapse_previous_enumeration(uint8_t bus, unsigned offset_unitid)
{
	pci_devfn_t dev;

	//actually, only for one HT device HT chain, and unitid is 0
#if !CONFIG_HT_CHAIN_UNITID_BASE
	if (offset_unitid) {
		return;
	}
#endif

	/* Check if is already collapsed */
	if ((!offset_unitid) || (offset_unitid && (!((CONFIG_HT_CHAIN_END_UNITID_BASE == 0) && (CONFIG_HT_CHAIN_END_UNITID_BASE <CONFIG_HT_CHAIN_UNITID_BASE))))) {
		uint32_t id;
		dev = PCI_DEV(bus, 0, 0);
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (!((id == 0xffffffff) || (id == 0x00000000) ||
		      (id == 0x0000ffff) || (id == 0xffff0000))) {
			return;
		}
	}

	/* Spin through the devices and collapse any previous
	 * hypertransport enumeration.
	 */
	for (dev = PCI_DEV(bus, 1, 0); dev <= PCI_DEV(bus, 0x1f, 0x7); dev += PCI_DEV(0, 1, 0)) {
		uint32_t id;
		uint8_t pos;
		uint16_t flags;

		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if ((id == 0xffffffff) || (id == 0x00000000) ||
		    (id == 0x0000ffff) || (id == 0xffff0000)) {
			continue;
		}

		pos = ht_lookup_slave_capability(dev);
		if (!pos) {
			continue;
		}

		/* Clear the unitid */
		flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f;
		pci_write_config16(dev, pos + PCI_CAP_FLAGS, flags);
	}
}

static uint16_t ht_read_freq_cap(pci_devfn_t dev, uint8_t pos)
{
	/* Handle bugs in valid hypertransport frequency reporting */
	uint16_t freq_cap;
	uint32_t id;

	freq_cap = pci_read_config16(dev, pos);
	printk(BIOS_SPEW, "pos=0x%x, unfiltered freq_cap=0x%x\n", pos, freq_cap);
	freq_cap &= ~(1 << HT_FREQ_VENDOR); /* Ignore Vendor HT frequencies */

	id = pci_read_config32(dev, 0);

	/* AMD 8131 Errata 48 */
	if (id == (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8131_PCIX << 16))) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
		return freq_cap;
	}

	/* AMD 8151 Errata 23 */
	if (id == (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8151_SYSCTRL << 16))) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
		return freq_cap;
	}

	/* AMD K8 Unsupported 1GHz? */
	if (id == (PCI_VENDOR_ID_AMD | (0x1100 << 16))) {
	#if IS_ENABLED(CONFIG_K8_HT_FREQ_1G_SUPPORT)
		#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
		if (is_cpu_pre_e0()) {  // only E0 later support 1GHz
			freq_cap &= ~(1 << HT_FREQ_1000Mhz);
		}
		#endif
	#else
		freq_cap &= ~(1 << HT_FREQ_1000Mhz);
	#endif
	}

	printk(BIOS_SPEW, "pos=0x%x, filtered freq_cap=0x%x\n", pos, freq_cap);

	return freq_cap;
}

static uint8_t ht_read_width_cap(pci_devfn_t dev, uint8_t pos)
{
	uint8_t width_cap = pci_read_config8(dev, pos);

	uint32_t id;

	id = pci_read_config32(dev, 0);

	/* netlogic micro cap doesn't support 16 bit yet */
	if (id == (0x184e | (0x0001 << 16))) {
		if ((width_cap & 0x77) == 0x11) {
			width_cap &= 0x88;
		}
	}

	return width_cap;

}

#define LINK_OFFS(CTRL, WIDTH,FREQ,FREQ_CAP) \
	(((CTRL & 0xff) << 24) | ((WIDTH & 0xff) << 16) | ((FREQ & 0xff) << 8) | (FREQ_CAP & 0xFF))

#define LINK_CTRL(OFFS)     ((OFFS >> 24) & 0xFF)
#define LINK_WIDTH(OFFS)    ((OFFS >> 16) & 0xFF)
#define LINK_FREQ(OFFS)     ((OFFS >> 8) & 0xFF)
#define LINK_FREQ_CAP(OFFS) ((OFFS) & 0xFF)

#define PCI_HT_HOST_OFFS LINK_OFFS(		\
		PCI_HT_CAP_HOST_CTRL,		\
		PCI_HT_CAP_HOST_WIDTH,		\
		PCI_HT_CAP_HOST_FREQ,		\
		PCI_HT_CAP_HOST_FREQ_CAP)

#define PCI_HT_SLAVE0_OFFS LINK_OFFS(		\
		PCI_HT_CAP_SLAVE_CTRL0,		\
		PCI_HT_CAP_SLAVE_WIDTH0,	\
		PCI_HT_CAP_SLAVE_FREQ0,		\
		PCI_HT_CAP_SLAVE_FREQ_CAP0)

#define PCI_HT_SLAVE1_OFFS LINK_OFFS(		\
		PCI_HT_CAP_SLAVE_CTRL1,		\
		PCI_HT_CAP_SLAVE_WIDTH1,	\
		PCI_HT_CAP_SLAVE_FREQ1,		\
		PCI_HT_CAP_SLAVE_FREQ_CAP1)

static int ht_optimize_link(
	pci_devfn_t dev1, uint8_t pos1, unsigned offs1,
	pci_devfn_t dev2, uint8_t pos2, unsigned offs2)
{
	static const uint8_t link_width_to_pow2[]= { 3, 4, 0, 5, 1, 2, 0, 0 };
	static const uint8_t pow2_to_link_width[] = { 0x7, 4, 5, 0, 1, 3 };
	uint16_t freq_cap1, freq_cap2;
	uint8_t width_cap1, width_cap2, width, old_width, ln_width1, ln_width2;
	uint8_t freq, old_freq;
	int needs_reset;
	/* Set link width and frequency */

	printk(BIOS_SPEW, "entering ht_optimize_link\n");
	/* Initially assume everything is already optimized and I don't need a reset */
	needs_reset = 0;

	/* Get the frequency capabilities */
	freq_cap1 = ht_read_freq_cap(dev1, pos1 + LINK_FREQ_CAP(offs1));
	freq_cap2 = ht_read_freq_cap(dev2, pos2 + LINK_FREQ_CAP(offs2));
	printk(BIOS_SPEW, "freq_cap1=0x%x, freq_cap2=0x%x\n", freq_cap1, freq_cap2);

	/* Calculate the highest possible frequency */
	freq = log2(freq_cap1 & freq_cap2);

	/* See if I am changing the link freqency */
	old_freq = pci_read_config8(dev1, pos1 + LINK_FREQ(offs1));
	old_freq &= 0x0f;
	needs_reset |= old_freq != freq;
	printk(BIOS_SPEW, "dev1 old_freq=0x%x, freq=0x%x, needs_reset=0x%0x\n", old_freq, freq, needs_reset);
	old_freq = pci_read_config8(dev2, pos2 + LINK_FREQ(offs2));
	old_freq &= 0x0f;
	needs_reset |= old_freq != freq;
	printk(BIOS_SPEW, "dev2 old_freq=0x%x, freq=0x%x, needs_reset=0x%0x\n", old_freq, freq, needs_reset);

	/* Set the Calculated link frequency */
	pci_write_config8(dev1, pos1 + LINK_FREQ(offs1), freq);
	pci_write_config8(dev2, pos2 + LINK_FREQ(offs2), freq);

	/* Get the width capabilities */
	width_cap1 = ht_read_width_cap(dev1, pos1 + LINK_WIDTH(offs1));
	width_cap2 = ht_read_width_cap(dev2, pos2 + LINK_WIDTH(offs2));
	printk(BIOS_SPEW, "width_cap1=0x%x, width_cap2=0x%x\n", width_cap1, width_cap2);

	/* Calculate dev1's input width */
	ln_width1 = link_width_to_pow2[width_cap1 & 7];
	ln_width2 = link_width_to_pow2[(width_cap2 >> 4) & 7];
	printk(BIOS_SPEW, "dev1 input ln_width1=0x%x, ln_width2=0x%x\n", ln_width1, ln_width2);
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width = pow2_to_link_width[ln_width1];
	printk(BIOS_SPEW, "dev1 input width=0x%x\n", width);
	/* Calculate dev1's output width */
	ln_width1 = link_width_to_pow2[(width_cap1 >> 4) & 7];
	ln_width2 = link_width_to_pow2[width_cap2 & 7];
	printk(BIOS_SPEW, "dev1 output ln_width1=0x%x, ln_width2=0x%x\n", ln_width1, ln_width2);
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width |= pow2_to_link_width[ln_width1] << 4;
	printk(BIOS_SPEW, "dev1 input|output width=0x%x\n", width);

	/* See if I am changing dev1's width */
	old_width = pci_read_config8(dev1, pos1 + LINK_WIDTH(offs1) + 1);
	old_width &= 0x77;
	needs_reset |= old_width != width;
	printk(BIOS_SPEW, "old dev1 input|output width=0x%x\n", width);

	/* Set dev1's widths */
	pci_write_config8(dev1, pos1 + LINK_WIDTH(offs1) + 1, width);

	/* Calculate dev2's width */
	width = ((width & 0x70) >> 4) | ((width & 0x7) << 4);
	printk(BIOS_SPEW, "dev2 input|output width=0x%x\n", width);

	/* See if I am changing dev2's width */
	old_width = pci_read_config8(dev2, pos2 + LINK_WIDTH(offs2) + 1);
	old_width &= 0x77;
	needs_reset |= old_width != width;
	printk(BIOS_SPEW, "old dev2 input|output width=0x%x\n", width);

	/* Set dev2's widths */
	pci_write_config8(dev2, pos2 + LINK_WIDTH(offs2) + 1, width);

	return needs_reset;
}

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
static void ht_setup_chainx(pci_devfn_t udev, uint8_t upos, uint8_t bus,
		unsigned offset_unitid, struct sys_info *sysinfo)
#else
static int ht_setup_chainx(pci_devfn_t udev, uint8_t upos, uint8_t bus,
		unsigned offset_unitid)
#endif
{
	//even CONFIG_HT_CHAIN_UNITID_BASE == 0, we still can go through this function, because of end_of_chain check, also We need it to optimize link

	uint8_t next_unitid, last_unitid;
	unsigned uoffs;

#if !IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
	int reset_needed = 0;
#endif

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
	//let't record the device of last ht device, So we can set the Unitid to CONFIG_HT_CHAIN_END_UNITID_BASE
	unsigned real_last_unitid = 0;
	uint8_t real_last_pos = 0;
	int ht_dev_num = 0;
	uint8_t end_used = 0;
#endif

	uoffs = PCI_HT_HOST_OFFS;
	next_unitid = (offset_unitid) ? CONFIG_HT_CHAIN_UNITID_BASE:1;

	do {
		uint32_t id;
		uint8_t pos;
		uint16_t flags, ctrl;
		uint8_t count;
		unsigned offs;

		/* Wait until the link initialization is complete */
		do {
			ctrl = pci_read_config16(udev, upos + LINK_CTRL(uoffs));
			/* Is this the end of the hypertransport chain? */
			if (ctrl & (1 << 6)) {
				goto end_of_chain;
			}

			if (ctrl & ((1 << 4) | (1 << 8))) {
				/*
				 * Either the link has failed, or we have
				 * a CRC error.
				 * Sometimes this can happen due to link
				 * retrain, so lets knock it down and see
				 * if its transient
				 */
				ctrl |= ((1 << 4) | (1 <<8)); // Link fail + Crc
				pci_write_config16(udev, upos + LINK_CTRL(uoffs), ctrl);
				ctrl = pci_read_config16(udev, upos + LINK_CTRL(uoffs));
				if (ctrl & ((1 << 4) | (1 << 8))) {
					printk(BIOS_ERR, "Detected error on Hypertransport Link\n");
					break;
				}
			}
		} while ((ctrl & (1 << 5)) == 0);

		pci_devfn_t dev = PCI_DEV(bus, 0, 0);
		last_unitid = next_unitid;

		id = pci_read_config32(dev, PCI_VENDOR_ID);

		/* If the chain is enumerated quit */
		if ((id == 0xffffffff) || (id == 0x00000000) ||
		    (id == 0x0000ffff) || (id == 0xffff0000))
		{
			break;
		}

		pos = ht_lookup_slave_capability(dev);
		if (!pos) {
			printk(BIOS_ERR, "udev=%08x", udev);
			printk(BIOS_ERR, "\tupos=%08x", upos);
			printk(BIOS_ERR, "\tuoffs=%08x", uoffs);
			printk(BIOS_ERR, "\tHT link capability not found\n");
			break;
		}


#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
		if (offset_unitid) {
			if (next_unitid >= (bus ? 0x20:0x18)) {
				if (!end_used) {
					next_unitid = CONFIG_HT_CHAIN_END_UNITID_BASE;
					end_used = 1;
				} else {
					goto out;
				}

			}
			real_last_pos = pos;
			real_last_unitid = next_unitid;
			ht_dev_num++;
		}
#endif
		/* Update the Unitid of the current device */
		flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f; /* mask out the base Unit ID */
		flags |= next_unitid & 0x1f;
		pci_write_config16(dev, pos + PCI_CAP_FLAGS, flags);

		/* Compute the number of unitids consumed */
		count = (flags >> 5) & 0x1f;

		/* Note the change in device number */
		dev = PCI_DEV(bus, next_unitid, 0);

		next_unitid += count;

		/* Find which side of the ht link we are on,
		 * by reading which direction our last write to PCI_CAP_FLAGS
		 * came from.
		 */
		flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
		offs = ((flags>>10) & 1) ? PCI_HT_SLAVE1_OFFS : PCI_HT_SLAVE0_OFFS;

		#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
		/* store the link pair here and we will Setup the Hypertransport link later, after we get final FID/VID */
		{
			struct link_pair_st *link_pair = &sysinfo->link_pair[sysinfo->link_pair_num];
			link_pair->udev = udev;
			link_pair->upos = upos;
			link_pair->uoffs = uoffs;
			link_pair->dev = dev;
			link_pair->pos = pos;
			link_pair->offs = offs;
			sysinfo->link_pair_num++;
		}
		#else
		reset_needed |= ht_optimize_link(udev, upos, uoffs, dev, pos, offs);
		#endif

		/* Remeber the location of the last device */
		udev = dev;
		upos = pos;
		uoffs = (offs != PCI_HT_SLAVE0_OFFS) ? PCI_HT_SLAVE0_OFFS : PCI_HT_SLAVE1_OFFS;

	} while (last_unitid != next_unitid);

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
out:
#endif
end_of_chain:;

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
	if (offset_unitid && (ht_dev_num > 1) && (real_last_unitid != CONFIG_HT_CHAIN_END_UNITID_BASE) && !end_used) {
		uint16_t flags;
		flags = pci_read_config16(PCI_DEV(bus,real_last_unitid,0), real_last_pos + PCI_CAP_FLAGS);
		flags &= ~0x1f;
		flags |= CONFIG_HT_CHAIN_END_UNITID_BASE & 0x1f;
		pci_write_config16(PCI_DEV(bus, real_last_unitid, 0), real_last_pos + PCI_CAP_FLAGS, flags);

		#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
		// Here need to change the dev in the array
		int i;
		for (i = 0; i < sysinfo->link_pair_num; i++)
		{
			struct link_pair_st *link_pair = &sysinfo->link_pair[i];
			if (link_pair->udev == PCI_DEV(bus, real_last_unitid, 0)) {
				link_pair->udev = PCI_DEV(bus, CONFIG_HT_CHAIN_END_UNITID_BASE, 0);
				continue;
			}
			if (link_pair->dev == PCI_DEV(bus, real_last_unitid, 0)) {
				link_pair->dev = PCI_DEV(bus, CONFIG_HT_CHAIN_END_UNITID_BASE, 0);
			}
		}
		#endif

	}
#endif

#if !IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
	return reset_needed;
#endif

}

static int optimize_link_read_pointer(uint8_t node, uint8_t linkn, uint8_t linkt, uint8_t val)
{
	uint32_t dword, dword_old;
	uint8_t link_type;

	/* This works on an Athlon64 because unimplemented links return 0 */
	dword = pci_read_config32(PCI_DEV(0,0x18+node,0), 0x98 + (linkn * 0x20));
	link_type = dword & 0xff;


	if ((link_type & 7) == linkt) { /* Coherent Link only linkt = 3, ncoherent = 7*/
		dword_old = dword = pci_read_config32(PCI_DEV(0,0x18+node,3), 0xdc);
		dword &= ~(0xff<<(linkn *8));
		dword |= val << (linkn *8);

		if (dword != dword_old) {
			pci_write_config32(PCI_DEV(0,0x18+node,3), 0xdc, dword);
			return 1;
		}
	}

	return 0;
}

static int optimize_link_read_pointers_chain(uint8_t ht_c_num)
{
	int reset_needed;
	uint8_t i;

	reset_needed = 0;

	/* First one is SB HT chain. */
	for (i = 0; i < ht_c_num; i++) {
		uint32_t reg;
		uint8_t nodeid, linkn;
		uint8_t busn;
		uint8_t val;
		unsigned devn;

		reg = pci_read_config32(PCI_DEV(0,0x18,1), 0xe0 + i * 4);

		nodeid = ((reg & 0xf0)>>4); // nodeid
		linkn = ((reg & 0xf00)>>8); // link n
		busn = (reg & 0xff0000)>>16; //busn

		devn = offset_unit_id(i == 0) ? CONFIG_HT_CHAIN_UNITID_BASE : 1;

		reg = pci_read_config32(PCI_DEV(busn, devn, 0), PCI_VENDOR_ID); // ? the chain dev maybe offseted
		if ((reg & 0xffff) == PCI_VENDOR_ID_AMD) {
			val = 0x25;
		} else if ((reg & 0xffff) == PCI_VENDOR_ID_NVIDIA) {
			val = 0x25;//???
		} else {
			continue;
		}

		reset_needed |= optimize_link_read_pointer(nodeid, linkn, 0x07, val);

	}

	return reset_needed;
}

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_NVIDIA_CK804)
static int set_ht_link_buffer_count(uint8_t node, uint8_t linkn, uint8_t linkt, unsigned val)
{
	uint32_t dword;
	uint8_t link_type;
	unsigned regpos;
	pci_devfn_t dev;

	/* This works on an Athlon64 because unimplemented links return 0 */
	regpos = 0x98 + (linkn * 0x20);
	dev = PCI_DEV(0,0x18+node,0);
	dword = pci_read_config32(dev, regpos);
	link_type = dword & 0xff;

	if ((link_type & 0x7) == linkt) { /* Coherent Link only linkt = 3, ncoherent = 7*/
		regpos = 0x90 + (linkn * 0x20);
		dword = pci_read_config32(dev, regpos);

		if (dword != val) {
			pci_write_config32(dev, regpos, val);
			return 1;
		}
	}

	return 0;
}

static int set_ht_link_buffer_counts_chain(uint8_t ht_c_num, unsigned vendorid,  unsigned val)
{
	int reset_needed;
	uint8_t i;

	reset_needed = 0;

	for (i = 0; i < ht_c_num; i++) {
		uint32_t reg;
		uint8_t nodeid, linkn;
		uint8_t busn;
		unsigned devn;

		reg = pci_read_config32(PCI_DEV(0,0x18,1), 0xe0 + i * 4);
		if ((reg & 3) != 3) continue; // not enabled

		nodeid = ((reg & 0xf0)>>4); // nodeid
		linkn = ((reg & 0xf00)>>8); // link n
		busn = (reg & 0xff0000)>>16; //busn

		for (devn = 0; devn < 0x20; devn++) {
			reg = pci_read_config32(PCI_DEV(busn, devn, 0), PCI_VENDOR_ID); //1?
			if ((reg & 0xffff) == vendorid) {
				reset_needed |= set_ht_link_buffer_count(nodeid, linkn, 0x07,val);
				break;
			}
		}
	}

	return reset_needed;
}
#endif

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
static void ht_setup_chains(uint8_t ht_c_num, struct sys_info *sysinfo)
#else
static int ht_setup_chains(uint8_t ht_c_num)
#endif
{
	/* Assumption the HT chain that is bus 0 has the HT I/O Hub on it.
	 * On most boards this just happens.  If a CPU has multiple
	 * non Coherent links the appropriate bus registers for the
	 * links needs to be programed to point at bus 0.
	 */
	uint8_t upos;
	pci_devfn_t udev;
	uint8_t i;

#if !IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
	int reset_needed = 0;
#else
	sysinfo->link_pair_num = 0;
#endif

	/* First one is SB HT chain. */
	for (i = 0; i < ht_c_num; i++) {
		uint32_t reg;
		uint8_t devpos;
		unsigned regpos;
		uint32_t dword;
		uint8_t busn;

		reg = pci_read_config32(PCI_DEV(0,0x18,1), 0xe0 + i * 4);

		//We need setup 0x94, 0xb4, and 0xd4 according to the reg
		devpos = ((reg & 0xf0)>>4)+0x18; // nodeid;it will decide 0x18 or 0x19
		regpos = ((reg & 0xf00)>>8) * 0x20 + 0x94; // link n;it will decide 0x94 or 0xb4, 0x0xd4;
		busn = (reg & 0xff0000)>>16;

		dword = pci_read_config32(PCI_DEV(0, devpos, 0), regpos);
		dword &= ~(0xffff<<8);
		dword |= (reg & 0xffff0000)>>8;
		pci_write_config32(PCI_DEV(0, devpos,0), regpos , dword);

		/* Make certain the HT bus is not enumerated */
		ht_collapse_previous_enumeration(busn, offset_unit_id(i == 0));

		upos = ((reg & 0xf00)>>8) * 0x20 + 0x80;
		udev =  PCI_DEV(0, devpos, 0);

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
		ht_setup_chainx(udev,upos,busn, offset_unit_id(i == 0), sysinfo); // all not
#else
		reset_needed |= ht_setup_chainx(udev,upos,busn, offset_unit_id(i == 0)); //all not
#endif

	}

#if !IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
	reset_needed |= optimize_link_read_pointers_chain(ht_c_num);

	return reset_needed;
#endif

}

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
static void ht_setup_chains_x(struct sys_info *sysinfo)
#else
static int ht_setup_chains_x(void)
#endif
{
	uint8_t nodeid;
	uint32_t reg;
	uint32_t tempreg;
	uint8_t next_busn;
	uint8_t ht_c_num;
	uint8_t nodes;
#if IS_ENABLED(CONFIG_K8_ALLOCATE_IO_RANGE)
	unsigned next_io_base;
#endif

	nodes = get_nodes();

	/* read PCI_DEV(0,0x18,0) 0x64 bit [8:9] to find out SbLink m */
	reg = pci_read_config32(PCI_DEV(0, 0x18, 0), 0x64);
	/* update PCI_DEV(0, 0x18, 1) 0xe0 to 0x05000m03, and next_busn = 0x3f+1 */
	print_linkn_in("SBLink=", ((reg>>8) & 3));
#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
	sysinfo->sblk = (reg>>8) & 3;
	sysinfo->sbbusn = 0;
	sysinfo->nodes = nodes;
#endif
	tempreg = 3 | (0<<4) | (((reg>>8) & 3)<<8) | (0<<16)| (0x3f<<24);
	pci_write_config32(PCI_DEV(0, 0x18, 1), 0xe0, tempreg);

	next_busn = 0x3f+1; /* 0 will be used ht chain with SB we need to keep SB in bus0 in auto stage*/

#if IS_ENABLED(CONFIG_K8_ALLOCATE_IO_RANGE)
	/* io range allocation */
	tempreg = 0 | (((reg>>8) & 0x3) << 4)|  (0x3<<12); //limit
	pci_write_config32(PCI_DEV(0, 0x18, 1), 0xC4, tempreg);
	tempreg = 3 | (3<<4) | (0<<12);	//base
	pci_write_config32(PCI_DEV(0, 0x18, 1), 0xC0, tempreg);
	next_io_base = 0x3+0x1;
#endif

	/* clean others */
	for (ht_c_num = 1;ht_c_num < 4; ht_c_num++) {
		pci_write_config32(PCI_DEV(0, 0x18, 1), 0xe0 + ht_c_num * 4, 0);

#if IS_ENABLED(CONFIG_K8_ALLOCATE_IO_RANGE)
		/* io range allocation */
		pci_write_config32(PCI_DEV(0, 0x18, 1), 0xc4 + ht_c_num * 8, 0);
		pci_write_config32(PCI_DEV(0, 0x18, 1), 0xc0 + ht_c_num * 8, 0);
#endif
	}

	for (nodeid = 0; nodeid < nodes; nodeid++) {
		pci_devfn_t dev;
		uint8_t linkn;
		dev = PCI_DEV(0, 0x18+nodeid,0);
		for (linkn = 0; linkn < 3; linkn++) {
			unsigned regpos;
			regpos = 0x98 + 0x20 * linkn;
			reg = pci_read_config32(dev, regpos);
			if ((reg & 0x17) != 7) continue; /* it is not non conherent or not connected*/
			print_linkn_in("NC node|link=", ((nodeid & 0xf)<<4)|(linkn & 0xf));
			tempreg = 3 | (nodeid <<4) | (linkn<<8);
			/*compare (temp & 0xffff), with (PCI(0, 0x18, 1) 0xe0 to 0xec & 0xfffff) */
			for (ht_c_num = 0;ht_c_num < 4; ht_c_num++) {
				reg = pci_read_config32(PCI_DEV(0, 0x18, 1), 0xe0 + ht_c_num * 4);
				if (((reg & 0xffff) == (tempreg & 0xffff)) || ((reg & 0xffff) == 0x0000)) {  /*we got it*/
					break;
				}
			}
			if (ht_c_num == 4) break; /*used up only 4 non conherent allowed*/
			/*update to 0xe0...*/
			if ((reg & 0xf) == 3) continue; /*SbLink so don't touch it */
			print_linkn_in("\tbusn=", next_busn);
			tempreg |= (next_busn<<16)|((next_busn+0x3f)<<24);
			pci_write_config32(PCI_DEV(0, 0x18, 1), 0xe0 + ht_c_num * 4, tempreg);
			next_busn+=0x3f+1;

#if IS_ENABLED(CONFIG_K8_ALLOCATE_IO_RANGE)
			/* io range allocation */
			tempreg = nodeid | (linkn<<4) |  ((next_io_base+0x3)<<12); //limit
			pci_write_config32(PCI_DEV(0, 0x18, 1), 0xC4 + ht_c_num * 8, tempreg);
			tempreg = 3 /*| (3<<4)*/ | (next_io_base<<12);	//base :ISA and VGA ?
			pci_write_config32(PCI_DEV(0, 0x18, 1), 0xC0 + ht_c_num * 8, tempreg);
			next_io_base += 0x3+0x1;
#endif

		}
	}
	/*update 0xe0, 0xe4, 0xe8, 0xec from PCI_DEV(0, 0x18,1) to PCI_DEV(0, 0x19,1) to PCI_DEV(0, 0x1f,1);*/

	for (nodeid = 1; nodeid < nodes; nodeid++) {
		int i;
		pci_devfn_t dev;
		dev = PCI_DEV(0, 0x18+nodeid,1);
		for (i = 0; i < 4; i++) {
			unsigned regpos;
			regpos = 0xe0 + i * 4;
			reg = pci_read_config32(PCI_DEV(0, 0x18, 1), regpos);
			pci_write_config32(dev, regpos, reg);
		}

#if IS_ENABLED(CONFIG_K8_ALLOCATE_IO_RANGE)
		/* io range allocation */
		for (i = 0; i < 4; i++) {
			unsigned regpos;
			regpos = 0xc4 + i * 8;
			reg = pci_read_config32(PCI_DEV(0, 0x18, 1), regpos);
			pci_write_config32(dev, regpos, reg);
		}
		for (i = 0; i < 4; i++) {
			unsigned regpos;
			regpos = 0xc0 + i * 8;
			reg = pci_read_config32(PCI_DEV(0, 0x18, 1), regpos);
			pci_write_config32(dev, regpos, reg);
		}
#endif
	}

	/* recount ht_c_num*/
	uint8_t i = 0;
	for (ht_c_num = 0;ht_c_num < 4; ht_c_num++) {
		reg = pci_read_config32(PCI_DEV(0, 0x18, 1), 0xe0 + ht_c_num * 4);
		if (((reg & 0xf) != 0x0)) {
			i++;
		}
	}

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
	sysinfo->ht_c_num = i;
	ht_setup_chains(i, sysinfo);
	sysinfo->sbdn = get_sbdn(sysinfo->sbbusn);
#else
	return ht_setup_chains(i);
#endif

}

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
static int optimize_link_incoherent_ht(struct sys_info *sysinfo)
{
	// We need to use recorded link pair info to optimize the link
	int i;
	int reset_needed = 0;

	unsigned link_pair_num = sysinfo->link_pair_num;

	printk(BIOS_SPEW, "entering optimize_link_incoherent_ht\n");
	printk(BIOS_SPEW, "sysinfo->link_pair_num=0x%x\n", link_pair_num);
	for (i = 0; i < link_pair_num; i++) {
		struct link_pair_st *link_pair= &sysinfo->link_pair[i];
		reset_needed |= ht_optimize_link(link_pair->udev, link_pair->upos, link_pair->uoffs, link_pair->dev, link_pair->pos, link_pair->offs);
		printk(BIOS_SPEW, "after ht_optimize_link for link pair %d, reset_needed=0x%x\n", i, reset_needed);
	}

	reset_needed |= optimize_link_read_pointers_chain(sysinfo->ht_c_num);
	printk(BIOS_SPEW, "after optimize_link_read_pointers_chain, reset_needed=0x%x\n", reset_needed);

	return reset_needed;

}
#endif
