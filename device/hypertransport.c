/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003-2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2004 David Hendricks <sc@flagen.com>
 * Copyright (C) 2004 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005-2006 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 * Copyright (C) 2005-2006 Stefan Reinauer <stepan@openbios.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <bitops.h>
#include <console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <part/hard_reset.h>
#include <part/fallback_boot.h>

#define OPT_HT_LINK 0

#if OPT_HT_LINK == 1
#include <cpu/amd/model_fxx_rev.h>
#endif

static struct device *ht_scan_get_devs(struct device **old_devices)
{
	struct device *first, *last;
	first = *old_devices;
	last = first;

	/* Extract the chain of devices to (first through last)
	 * for the next hypertransport device.
	 */
	while (last && last->sibling &&
	       (last->sibling->path.type == DEVICE_PATH_PCI) &&
	       (last->sibling->path.pci.devfn > last->path.pci.devfn)) {
		last = last->sibling;
	}
	if (first) {
		struct device *child;
		/* Unlink the chain from the list of old devices. */
		*old_devices = last->sibling;
		last->sibling = 0;

		/* Now add the device to the list of devices on the bus. */
		/* Find the last child of our parent. */
		for (child = first->bus->children; child && child->sibling;) {
			child = child->sibling;
		}
		/* Place the chain on the list of children of their parent. */
		if (child) {
			child->sibling = first;
		} else {
			first->bus->children = first;
		}
	}
	return first;
}

#if OPT_HT_LINK == 1
static unsigned ht_read_freq_cap(struct device *dev, unsigned pos)
{
	/* Handle bugs in valid hypertransport frequency reporting. */
	unsigned freq_cap;

	freq_cap = pci_read_config16(dev, pos);
	freq_cap &= ~(1 << HT_FREQ_VENDOR); /* Ignore vendor HT frequencies. */

	/* AMD 8131 Errata 48. */
	if ((dev->vendor == PCI_VENDOR_ID_AMD) &&
	    (dev->device == PCI_DEVICE_ID_AMD_8131_PCIX)) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
	}
	/* AMD 8151 Errata 23. */
	if ((dev->vendor == PCI_VENDOR_ID_AMD) &&
	    (dev->device == PCI_DEVICE_ID_AMD_8151_SYSCTRL)) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
	}
	/* AMD K8 unsupported 1GHz? */
	if ((dev->vendor == PCI_VENDOR_ID_AMD) && (dev->device == 0x1100)) {
#ifndef K8_HT_FREQ_1G_SUPPORT
		freq_cap &= ~(1 << HT_FREQ_1000Mhz);
#endif

	}
	return freq_cap;
}
#endif

struct ht_link {
	struct device *dev;
	unsigned int pos;
	unsigned char ctrl_off, config_off, freq_off, freq_cap_off;
};

static int ht_setup_link(struct ht_link *prev, struct device *dev,
			 unsigned int pos)
{
	static const u8 link_width_to_pow2[] = { 3, 4, 0, 5, 1, 2, 0, 0 };
	static const u8 pow2_to_link_width[] = { 0x7, 4, 5, 0, 1, 3 };
	struct ht_link cur[1];
	unsigned int present_width_cap, upstream_width_cap;
	unsigned int present_freq_cap, upstream_freq_cap;
	unsigned int ln_present_width_in, ln_upstream_width_in;
	unsigned int ln_present_width_out, ln_upstream_width_out;
	unsigned int freq, old_freq;
	unsigned int present_width, upstream_width, old_width;
	int reset_needed;
	int linkb_to_host;

	/* Set the hypertransport link width and frequency. */
	reset_needed = 0;
	/* See which side of the device our previous write to 
	 * set the unitid came from.
	 */
	cur->dev = dev;
	cur->pos = pos;
	linkb_to_host =
	    (pci_read_config16(cur->dev, cur->pos + PCI_CAP_FLAGS) >> 10) & 1;
	if (!linkb_to_host) {
		cur->ctrl_off = PCI_HT_CAP_SLAVE_CTRL0;
		cur->config_off = PCI_HT_CAP_SLAVE_WIDTH0;
		cur->freq_off = PCI_HT_CAP_SLAVE_FREQ0;
		cur->freq_cap_off = PCI_HT_CAP_SLAVE_FREQ_CAP0;
	} else {
		cur->ctrl_off = PCI_HT_CAP_SLAVE_CTRL1;
		cur->config_off = PCI_HT_CAP_SLAVE_WIDTH1;
		cur->freq_off = PCI_HT_CAP_SLAVE_FREQ1;
		cur->freq_cap_off = PCI_HT_CAP_SLAVE_FREQ_CAP1;
	}
#if OPT_HT_LINK == 1
	/* Read the capabilities. */
	present_freq_cap =
	    ht_read_freq_cap(cur->dev, cur->pos + cur->freq_cap_off);
	upstream_freq_cap =
	    ht_read_freq_cap(prev->dev, prev->pos + prev->freq_cap_off);
	present_width_cap =
	    pci_read_config8(cur->dev, cur->pos + cur->config_off);
	upstream_width_cap =
	    pci_read_config8(prev->dev, prev->pos + prev->config_off);

	/* Calculate the highest useable frequency. */
	freq = log2(present_freq_cap & upstream_freq_cap);

	/* Calculate the highest width. */
	ln_upstream_width_in = link_width_to_pow2[upstream_width_cap & 7];
	ln_present_width_out = link_width_to_pow2[(present_width_cap >> 4) & 7];
	if (ln_upstream_width_in > ln_present_width_out) {
		ln_upstream_width_in = ln_present_width_out;
	}
	upstream_width = pow2_to_link_width[ln_upstream_width_in];
	present_width = pow2_to_link_width[ln_upstream_width_in] << 4;

	ln_upstream_width_out =
	    link_width_to_pow2[(upstream_width_cap >> 4) & 7];
	ln_present_width_in = link_width_to_pow2[present_width_cap & 7];
	if (ln_upstream_width_out > ln_present_width_in) {
		ln_upstream_width_out = ln_present_width_in;
	}
	upstream_width |= pow2_to_link_width[ln_upstream_width_out] << 4;
	present_width |= pow2_to_link_width[ln_upstream_width_out];

	/* Set the current device. */
	old_freq = pci_read_config8(cur->dev, cur->pos + cur->freq_off);
	old_freq &= 0x0f;
	if (freq != old_freq) {
		unsigned int new_freq;
		pci_write_config8(cur->dev, cur->pos + cur->freq_off, freq);
		reset_needed = 1;
		printk(BIOS_SPEW, "HyperT FreqP old %x new %x\n", old_freq, freq);
		new_freq = pci_read_config8(cur->dev, cur->pos + cur->freq_off);
		new_freq &= 0x0f;
		if (new_freq != freq) {
			printk(BIOS_ERR, "%s Hypertransport frequency would not set wanted: %x got: %x\n",
			     dev_path(dev), freq, new_freq);
		}
	}
	old_width = pci_read_config8(cur->dev, cur->pos + cur->config_off + 1);
	if (present_width != old_width) {
		unsigned int new_width;
		pci_write_config8(cur->dev, cur->pos + cur->config_off + 1,
				  present_width);
		reset_needed = 1;
		printk(BIOS_SPEW, "HyperT widthP old %x new %x\n", old_width,
			    present_width);
		new_width =
		    pci_read_config8(cur->dev, cur->pos + cur->config_off + 1);
		if (new_width != present_width) {
			printk(BIOS_ERR, "%s Hypertransport width would not set wanted: %x got: %x\n",
			     dev_path(dev), present_width, new_width);
		}
	}

	/* Set the upstream device. */
	old_freq = pci_read_config8(prev->dev, prev->pos + prev->freq_off);
	old_freq &= 0x0f;
	if (freq != old_freq) {
		unsigned int new_freq;
		pci_write_config8(prev->dev, prev->pos + prev->freq_off, freq);
		reset_needed = 1;
		printk(BIOS_SPEW, "HyperT freqU old %x new %x\n", old_freq, freq);
		new_freq =
		    pci_read_config8(prev->dev, prev->pos + prev->freq_off);
		new_freq &= 0x0f;
		if (new_freq != freq) {
			printk(BIOS_ERR, "%s Hypertransport frequency would not set wanted: %x got: %x\n",
			     dev_path(prev->dev), freq, new_freq);
		}
	}
	old_width =
	    pci_read_config8(prev->dev, prev->pos + prev->config_off + 1);
	if (upstream_width != old_width) {
		unsigned int new_width;
		pci_write_config8(prev->dev, prev->pos + prev->config_off + 1,
				  upstream_width);
		reset_needed = 1;
		printk(BIOS_SPEW, "HyperT widthU old %x new %x\n", old_width,
			    upstream_width);
		new_width =
		    pci_read_config8(prev->dev,
				     prev->pos + prev->config_off + 1);
		if (new_width != upstream_width) {
			printk(BIOS_ERR, "%s Hypertransport width would not set wanted: %x got: %x\n",
			     dev_path(prev->dev), upstream_width, new_width);
		}
	}
#endif

	/* Remember the current link as the previous link,
	 * but look at the other offsets.
	 */
	prev->dev = cur->dev;
	prev->pos = cur->pos;
	if (cur->ctrl_off == PCI_HT_CAP_SLAVE_CTRL0) {
		prev->ctrl_off = PCI_HT_CAP_SLAVE_CTRL1;
		prev->config_off = PCI_HT_CAP_SLAVE_WIDTH1;
		prev->freq_off = PCI_HT_CAP_SLAVE_FREQ1;
		prev->freq_cap_off = PCI_HT_CAP_SLAVE_FREQ_CAP1;
	} else {
		prev->ctrl_off = PCI_HT_CAP_SLAVE_CTRL0;
		prev->config_off = PCI_HT_CAP_SLAVE_WIDTH0;
		prev->freq_off = PCI_HT_CAP_SLAVE_FREQ0;
		prev->freq_cap_off = PCI_HT_CAP_SLAVE_FREQ_CAP0;
	}

	return reset_needed;
}

static unsigned int ht_lookup_slave_capability(struct device *dev)
{
	unsigned int pos;
	pos = 0;
	do {
		pos = pci_find_next_capability(dev, PCI_CAP_ID_HT, pos);
		if (pos) {
			unsigned int flags;
			flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
			printk(BIOS_SPEW, "Flags: 0x%04x\n", flags);
			if ((flags >> 13) == 0) {
				/* Entry is a Slave secondary, success... */
				break;
			}
		}
	} while (pos);
	return pos;
}

static void ht_collapse_early_enumeration(struct bus *bus,
					  unsigned int offset_unitid)
{
	unsigned int devfn, ctrl;
	struct ht_link prev;

	/* Initialize the hypertransport enumeration state. */
	prev.dev = bus->dev;
	prev.pos = bus->cap;
	prev.ctrl_off = PCI_HT_CAP_HOST_CTRL;
	prev.config_off = PCI_HT_CAP_HOST_WIDTH;
	prev.freq_off = PCI_HT_CAP_HOST_FREQ;
	prev.freq_cap_off = PCI_HT_CAP_HOST_FREQ_CAP;

	/* Wait until the link initialization is complete. */
	do {
		ctrl = pci_read_config16(prev.dev, prev.pos + prev.ctrl_off);
		/* Is this the end of the hypertransport chain? */
		if (ctrl & (1 << 6)) {
			return;
		}
		/* Has the link failed? */
		if (ctrl & (1 << 4)) {
			/* Either the link has failed, or we have a CRC error.
			 * Sometimes this can happen due to link retrain, so
			 * lets knock it down and see if its transient.
			 */
			ctrl |= ((1 << 4) | (1 << 8));	// Link fail + CRC.
			pci_write_config16(prev.dev, prev.pos + prev.ctrl_off,
					   ctrl);
			ctrl = pci_read_config16(prev.dev,
					      prev.pos + prev.ctrl_off);
			if (ctrl & ((1 << 4) | (1 << 8))) {
				printk(BIOS_ALERT, "Detected error on Hypertransport Link\n");
				return;
			}
		}
	} while ((ctrl & (1 << 5)) == 0);

	/* Actually, only for one HT device HT chain, and unitid is 0. */
#if HT_CHAIN_UNITID_BASE == 0
	if (offset_unitid) {
		return;
	}
#endif

	/* Check if is already collapsed. */
	if ((!offset_unitid)
	    || (offset_unitid
		&&
		(!((HT_CHAIN_END_UNITID_BASE == 0)
		   && (HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE))))) {
		struct device dummy;
		u32 id;
		dummy.bus = bus;
		dummy.path.type = DEVICE_PATH_PCI;
		dummy.path.pci.devfn = PCI_DEVFN(0, 0);
		id = pci_read_config32(&dummy, PCI_VENDOR_ID);
		if (!((id == 0xffffffff) || (id == 0x00000000) ||
		      (id == 0x0000ffff) || (id == 0xffff0000))) {
			return;
		}
	}

	/* Spin through the devices and collapse any early
	 * hypertransport enumeration.
	 */
	for (devfn = PCI_DEVFN(1, 0); devfn <= 0xff; devfn += 8) {
		struct device dummy;
		u32 id;
		unsigned int pos, flags;
		dummy.bus = bus;
		dummy.path.type = DEVICE_PATH_PCI;
		dummy.path.pci.devfn = devfn;
		id = pci_read_config32(&dummy, PCI_VENDOR_ID);
		if ((id == 0xffffffff) || (id == 0x00000000) ||
		    (id == 0x0000ffff) || (id == 0xffff0000)) {
			continue;
		}
		dummy.vendor = id & 0xffff;
		dummy.device = (id >> 16) & 0xffff;
		dummy.hdr_type = pci_read_config8(&dummy, PCI_HEADER_TYPE);
		pos = ht_lookup_slave_capability(&dummy);
		if (!pos) {
			continue;
		}

		/* Clear the unitid. */
		flags = pci_read_config16(&dummy, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f;
		pci_write_config16(&dummy, pos + PCI_CAP_FLAGS, flags);
		printk(BIOS_SPEW, "Collapsing %s [%04x/%04x]\n",
			    dev_path(&dummy), dummy.vendor, dummy.device);
	}
}

unsigned int hypertransport_scan_chain(struct bus *bus, unsigned int min_devfn,
				       unsigned int max_devfn,
				       unsigned int max,
				       unsigned int *ht_unitid_base,
				       unsigned int offset_unitid)
{
	/* Even HT_CHAIN_UNITID_BASE == 0, we still can go through this
	 * function, because of end_of_chain check, also we need it to
	 * optimize link.
	 */
	unsigned int next_unitid, last_unitid;
	struct device *old_devices, *dev, *func;
	unsigned int min_unitid = (offset_unitid) ? HT_CHAIN_UNITID_BASE : 1;
	struct ht_link prev;
	struct device *last_func = 0;
	int ht_dev_num = 0;

#if HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE
	/* Let's record the device of last HT device, so we can set the
	 * unitid to HT_CHAIN_END_UNITID_BASE.
	 */
	unsigned int real_last_unitid;
	u8 real_last_pos;
	struct device *real_last_dev;
#endif

	/* Restore the hypertransport chain to its unitialized state. */
	ht_collapse_early_enumeration(bus, offset_unitid);

	/* See which static device nodes I have. */
	old_devices = bus->children;
	bus->children = 0;

	/* Initialize the hypertransport enumeration state. */
	prev.dev = bus->dev;
	prev.pos = bus->cap;
	prev.ctrl_off = PCI_HT_CAP_HOST_CTRL;
	prev.config_off = PCI_HT_CAP_HOST_WIDTH;
	prev.freq_off = PCI_HT_CAP_HOST_FREQ;
	prev.freq_cap_off = PCI_HT_CAP_HOST_FREQ_CAP;

	/* If present assign unitid to a hypertransport chain. */
	last_unitid = min_unitid - 1;
	next_unitid = min_unitid;
	do {
		u8 pos;
		u16 flags;
		unsigned int count, static_count, ctrl;

		last_unitid = next_unitid;

		/* Wait until the link initialization is complete. */
		do {
			ctrl = pci_read_config16(prev.dev,
					      prev.pos + prev.ctrl_off);

			if (ctrl & (1 << 6))
				goto end_of_chain;	// End of chain.

			if (ctrl & ((1 << 4) | (1 << 8))) {
				/* Either the link has failed, or we have a
				 * CRC error. Sometimes this can happen due to
				 * link retrain, so lets knock it down and see
				 * if its transient.
				 */
				ctrl |= ((1 << 4) | (1 << 8)); /* Link fail + CRC */
				pci_write_config16(prev.dev,
						   prev.pos + prev.ctrl_off,
						   ctrl);
				ctrl = pci_read_config16(prev.dev,
					      prev.pos + prev.ctrl_off);
				if (ctrl & ((1 << 4) | (1 << 8))) {
					printk(BIOS_ALERT, "Detected error on Hypertransport Link\n");
					goto end_of_chain;
				}
			}
		} while ((ctrl & (1 << 5)) == 0);

		/* Get and setup the device_structure. */
		dev = ht_scan_get_devs(&old_devices);

		/* See if a device is present and setup the device structure. */
		dev = pci_probe_dev(dev, bus, 0);
		if (!dev || !dev->enabled) {
			break;
		}

		/* Find the hypertransport link capability. */
		pos = ht_lookup_slave_capability(dev);
		if (pos == 0) {
			printk(BIOS_ERR, "%s Hypertransport link capability not found",
			     dev_path(dev));
			break;
		}

		/* Update the unitid of the current device. */
		flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);

		/* If the devices has a unitid set and is at devfn 0 we are
		 * done. This can happen with shadow hypertransport devices,
		 * or if we have reached the bottom of a hypertransport
		 * device chain.
		 */
		if (flags & 0x1f) {
			break;
		}

		flags &= ~0x1f;	/* Mask out base Unit ID. */
		flags |= next_unitid & 0x1f;
		pci_write_config16(dev, pos + PCI_CAP_FLAGS, flags);

		/* Update the unitid in the device structure. */
		static_count = 1;
		for (func = dev; func; func = func->sibling) {
			func->path.pci.devfn += (next_unitid << 3);
			static_count = (func->path.pci.devfn >> 3)
			    - (dev->path.pci.devfn >> 3) + 1;
			last_func = func;
		}

		/* Compute the number of unitids consumed. */
		count = (flags >> 5) & 0x1f;	/* Get unit count. */
		printk(BIOS_SPEW, "%s count: %04x static_count: %04x\n",
			    dev_path(dev), count, static_count);
		if (count < static_count) {
			count = static_count;
		}

		/* Update the unitid of the next device. */
		ht_unitid_base[ht_dev_num] = next_unitid;
		ht_dev_num++;
#if HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE
		if (offset_unitid) {
			real_last_unitid = next_unitid;
			real_last_pos = pos;
			real_last_dev = dev;
		}
#endif
		next_unitid += count;

		/* Setup the hypertransport link. */
		bus->reset_needed |= ht_setup_link(&prev, dev, pos);

		printk(BIOS_DEBUG, "%s [%04x/%04x] %s next_unitid: %04x\n",
			     dev_path(dev),
			     dev->vendor, dev->device,
			     (dev->enabled ? "enabled" : "disabled"),
			     next_unitid);

	} while ((last_unitid != next_unitid)
		 && (next_unitid <= (max_devfn >> 3)));
      end_of_chain:
#if OPT_HT_LINK == 1
	if (bus->reset_needed) {
		printk(BIOS_INFO, "HyperT reset needed\n");
	} else {
		printk(BIOS_DEBUG, "HyperT reset not needed\n");
	}
#endif

#if HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE
	if (offset_unitid && (ht_dev_num > 0)) {
		u16 flags;
		int i;
		struct device *last_func = 0;
		flags = pci_read_config16(real_last_dev,
			      real_last_pos + PCI_CAP_FLAGS);
		flags &= ~0x1f;
		flags |= HT_CHAIN_END_UNITID_BASE & 0x1f;
		pci_write_config16(real_last_dev, real_last_pos + PCI_CAP_FLAGS,
				   flags);

		for (func = real_last_dev; func; func = func->sibling) {
			func->path.pci.devfn -=
			    ((real_last_unitid -
			      HT_CHAIN_END_UNITID_BASE) << 3);
			last_func = func;
		}

		/* Update last one. */
		ht_unitid_base[ht_dev_num - 1] = HT_CHAIN_END_UNITID_BASE;

		next_unitid = real_last_unitid;
	}
#endif

	if (next_unitid > 0x1f) {
		next_unitid = 0x1f;
	}

	/* Die if any leftover static devices are found.
	 * There's probably a problem in the Config.lb.
	 * TODO: No more Config.lb in coreboot-v3.
	 */
	if (old_devices) {
		struct device *left;
		printk(BIOS_INFO, "HT: Left over static devices:\n");
		for (left = old_devices; left; left = left->sibling) {
			printk(BIOS_INFO, "%s\n", dev_path(left));
		}
		printk(BIOS_INFO, "HT: End of leftover list.\n");
		/* Put back the left over static device, and let
		 * pci_scan_bus() disable it.
		 */
		if (last_func && !last_func->sibling)
			last_func->sibling = old_devices;
	}

	/* Now that nothing is overlapping it is safe to scan the
	 * children.
	 */
	max = pci_scan_bus(bus, 0x00, (next_unitid << 3) | 7, max);
	return max;
}

/**
 * Scan a PCI bridge and the buses behind the bridge.
 *
 * Determine the existence of buses behind the bridge. Set up the bridge
 * according to the result of the scan.
 *
 * This function is the default scan_bus() method for PCI bridge devices.
 *
 * TODO: This comment in copy+pasted from elsewhere and probably incorrect.
 *
 * @param dev Pointer to the bridge device.
 * @param max The highest bus number assigned up to now.
 * @return The maximum bus number found, after scanning all subordinate buses.
 */
unsigned int hypertransport_scan_chain_x(struct bus *bus,
					 unsigned int min_devfn,
					 unsigned int max_devfn,
					 unsigned int max)
{
	unsigned int ht_unitid_base[4];
	unsigned int offset_unitid = 1;
	return hypertransport_scan_chain(bus, min_devfn, max_devfn, max,
					 ht_unitid_base, offset_unitid);
}

unsigned int ht_scan_bridge(struct device *dev, unsigned int max)
{
	return do_pci_scan_bridge(dev, max, hypertransport_scan_chain_x);
}

/** Default device operations for hypertransport bridges. */
static const struct pci_operations ht_bus_ops_pci = {
	.set_subsystem = 0,
};

const struct device_operations default_ht_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = 0,
	.scan_bus         = ht_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &ht_bus_ops_pci,
};
