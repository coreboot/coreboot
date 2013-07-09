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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <lib.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>

/*
 * The hypertransport link is already optimized in pre-RAM code so don't do
 * it again.
 */
#define OPT_HT_LINK 0

#if OPT_HT_LINK == 1
#include <cpu/amd/model_fxx_rev.h>
#endif

static device_t ht_scan_get_devs(device_t *old_devices)
{
	device_t first, last;

	first = *old_devices;
	last = first;

	/*
	 * Extract the chain of devices to (first through last) for the next
	 * hypertransport device.
	 */
	while (last && last->sibling &&
	       (last->sibling->path.type == DEVICE_PATH_PCI) &&
	       (last->sibling->path.pci.devfn > last->path.pci.devfn))
	{
		last = last->sibling;
	}

	if (first) {
		device_t child;

		/* Unlink the chain from the list of old devices. */
		*old_devices = last->sibling;
		last->sibling = 0;

		/* Now add the device to the list of devices on the bus. */
		/* Find the last child of our parent. */
		for (child = first->bus->children; child && child->sibling; )
			child = child->sibling;

		/* Place the chain on the list of children of their parent. */
		if (child)
			child->sibling = first;
		else
			first->bus->children = first;
	}
	return first;
}

#if OPT_HT_LINK == 1
static unsigned ht_read_freq_cap(device_t dev, unsigned pos)
{
	/* Handle bugs in valid hypertransport frequency reporting. */
	unsigned freq_cap;

	freq_cap = pci_read_config16(dev, pos);
	freq_cap &= ~(1 << HT_FREQ_VENDOR); /* Ignore Vendor HT frequencies. */

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
#if CONFIG_K8_HT_FREQ_1G_SUPPORT

#if !CONFIG_K8_REV_F_SUPPORT
		/* Only e0 later support 1GHz HT. */
		if (is_cpu_pre_e0())
			freq_cap &= ~(1 << HT_FREQ_1000Mhz);
#endif

#else
		freq_cap &= ~(1 << HT_FREQ_1000Mhz);
#endif
	}

	return freq_cap;
}
#endif

struct ht_link {
	struct device *dev;
	unsigned pos;
	unsigned char ctrl_off, config_off, freq_off, freq_cap_off;
};

static int ht_setup_link(struct ht_link *prev, device_t dev, unsigned pos)
{
#if OPT_HT_LINK == 1
	static const u8 link_width_to_pow2[] = { 3, 4, 0, 5, 1, 2, 0, 0 };
	static const u8 pow2_to_link_width[] = { 7, 4, 5, 0, 1, 3 };
	unsigned present_width_cap, upstream_width_cap;
	unsigned present_freq_cap, upstream_freq_cap;
	unsigned ln_present_width_in, ln_upstream_width_in;
	unsigned ln_present_width_out, ln_upstream_width_out;
	unsigned freq, old_freq;
	unsigned present_width, upstream_width, old_width;
#endif
	struct ht_link cur[1];
	int reset_needed;
	int linkb_to_host;

	/* Set the hypertransport link width and frequency. */
	reset_needed = 0;
	/*
	 * See which side of the device our previous write to set the unitid
	 * came from.
	 */
	cur->dev = dev;
	cur->pos = pos;
	linkb_to_host =
	  (pci_read_config16(cur->dev, cur->pos + PCI_CAP_FLAGS) >> 10) & 1;

	if (!linkb_to_host) {
		cur->ctrl_off     = PCI_HT_CAP_SLAVE_CTRL0;
		cur->config_off   = PCI_HT_CAP_SLAVE_WIDTH0;
		cur->freq_off     = PCI_HT_CAP_SLAVE_FREQ0;
		cur->freq_cap_off = PCI_HT_CAP_SLAVE_FREQ_CAP0;
	} else {
		cur->ctrl_off     = PCI_HT_CAP_SLAVE_CTRL1;
		cur->config_off   = PCI_HT_CAP_SLAVE_WIDTH1;
		cur->freq_off     = PCI_HT_CAP_SLAVE_FREQ1;
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

	/* Calculate the highest usable frequency. */
	freq = log2(present_freq_cap & upstream_freq_cap);

	/* Calculate the highest width. */
	ln_upstream_width_in = link_width_to_pow2[upstream_width_cap & 7];
	ln_present_width_out = link_width_to_pow2[(present_width_cap >> 4) & 7];
	if (ln_upstream_width_in > ln_present_width_out)
		ln_upstream_width_in = ln_present_width_out;
	upstream_width = pow2_to_link_width[ln_upstream_width_in];
	present_width  = pow2_to_link_width[ln_upstream_width_in] << 4;

	ln_upstream_width_out =
		link_width_to_pow2[(upstream_width_cap >> 4) & 7];
	ln_present_width_in = link_width_to_pow2[present_width_cap & 7];
	if (ln_upstream_width_out > ln_present_width_in)
		ln_upstream_width_out = ln_present_width_in;
	upstream_width |= pow2_to_link_width[ln_upstream_width_out] << 4;
	present_width  |= pow2_to_link_width[ln_upstream_width_out];

	/* Set the current device. */
	old_freq = pci_read_config8(cur->dev, cur->pos + cur->freq_off);
	old_freq &= 0x0f;
	if (freq != old_freq) {
		unsigned new_freq;
		pci_write_config8(cur->dev, cur->pos + cur->freq_off, freq);
		reset_needed = 1;
		printk(BIOS_SPEW, "HyperT FreqP old %x new %x\n",old_freq,freq);
		new_freq = pci_read_config8(cur->dev, cur->pos + cur->freq_off);
		new_freq &= 0x0f;
		if (new_freq != freq) {
			printk(BIOS_ERR, "%s Hypertransport frequency would "
			       "not set. Wanted: %x, got: %x\n",
			       dev_path(dev), freq, new_freq);
		}
	}
	old_width = pci_read_config8(cur->dev, cur->pos + cur->config_off + 1);
	if (present_width != old_width) {
		unsigned new_width;
		pci_write_config8(cur->dev, cur->pos + cur->config_off + 1,
				  present_width);
		reset_needed = 1;
		printk(BIOS_SPEW, "HyperT widthP old %x new %x\n",
		       old_width, present_width);
		new_width = pci_read_config8(cur->dev,
					     cur->pos + cur->config_off + 1);
		if (new_width != present_width) {
			printk(BIOS_ERR, "%s Hypertransport width would not "
			       "set. Wanted: %x, got: %x\n",
			       dev_path(dev), present_width, new_width);
		}
	}

	/* Set the upstream device. */
	old_freq = pci_read_config8(prev->dev, prev->pos + prev->freq_off);
	old_freq &= 0x0f;
	if (freq != old_freq) {
		unsigned new_freq;
		pci_write_config8(prev->dev, prev->pos + prev->freq_off, freq);
		reset_needed = 1;
		printk(BIOS_SPEW, "HyperT freqU old %x new %x\n",
		       old_freq, freq);
		new_freq =
		  pci_read_config8(prev->dev, prev->pos + prev->freq_off);
		new_freq &= 0x0f;
		if (new_freq != freq) {
			printk(BIOS_ERR, "%s Hypertransport frequency would "
			       "not set. Wanted: %x, got: %x\n",
			       dev_path(prev->dev), freq, new_freq);
		}
	}
	old_width =
		pci_read_config8(prev->dev, prev->pos + prev->config_off + 1);
	if (upstream_width != old_width) {
		unsigned new_width;
		pci_write_config8(prev->dev, prev->pos + prev->config_off + 1,
				  upstream_width);
		reset_needed = 1;
		printk(BIOS_SPEW, "HyperT widthU old %x new %x\n", old_width,
		       upstream_width);
		new_width = pci_read_config8(prev->dev,
					     prev->pos + prev->config_off + 1);
		if (new_width != upstream_width) {
			printk(BIOS_ERR, "%s Hypertransport width would not "
			       "set. Wanted: %x, got: %x\n",
			       dev_path(prev->dev), upstream_width, new_width);
		}
	}
#endif

	/*
	 * Remember the current link as the previous link, but look at the
	 * other offsets.
	 */
	prev->dev = cur->dev;
	prev->pos = cur->pos;
	if (cur->ctrl_off == PCI_HT_CAP_SLAVE_CTRL0) {
		prev->ctrl_off     = PCI_HT_CAP_SLAVE_CTRL1;
		prev->config_off   = PCI_HT_CAP_SLAVE_WIDTH1;
		prev->freq_off     = PCI_HT_CAP_SLAVE_FREQ1;
		prev->freq_cap_off = PCI_HT_CAP_SLAVE_FREQ_CAP1;
	} else {
		prev->ctrl_off     = PCI_HT_CAP_SLAVE_CTRL0;
		prev->config_off   = PCI_HT_CAP_SLAVE_WIDTH0;
		prev->freq_off     = PCI_HT_CAP_SLAVE_FREQ0;
		prev->freq_cap_off = PCI_HT_CAP_SLAVE_FREQ_CAP0;
	}

	return reset_needed;
}

static unsigned ht_lookup_slave_capability(struct device *dev)
{
	unsigned pos;

	pos = 0;
	do {
		pos = pci_find_next_capability(dev, PCI_CAP_ID_HT, pos);
		if (pos) {
			u16 flags;
			flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
			printk(BIOS_SPEW, "flags: 0x%04x\n", flags);
			if ((flags >> 13) == 0) {
				/* Entry is a slave secondary, success... */
				break;
			}
		}
	} while (pos);

	return pos;
}

static void ht_collapse_early_enumeration(struct bus *bus,
					  unsigned offset_unitid)
{
	unsigned int devfn;
	struct ht_link prev;
	u16 ctrl;

	/* Initialize the hypertransport enumeration state. */
	prev.dev = bus->dev;
	prev.pos = bus->cap;
	prev.ctrl_off     = PCI_HT_CAP_HOST_CTRL;
	prev.config_off   = PCI_HT_CAP_HOST_WIDTH;
	prev.freq_off     = PCI_HT_CAP_HOST_FREQ;
	prev.freq_cap_off = PCI_HT_CAP_HOST_FREQ_CAP;

	/* Wait until the link initialization is complete. */
	do {
		ctrl = pci_read_config16(prev.dev, prev.pos + prev.ctrl_off);

		/* Is this the end of the hypertransport chain? */
		if (ctrl & (1 << 6))
			return;

		/* Has the link failed? */
		if (ctrl & (1 << 4)) {
			/*
			 * Either the link has failed, or we have a CRC error.
			 * Sometimes this can happen due to link retrain, so
			 * lets knock it down and see if its transient.
			 */
			ctrl |= ((1 << 4) | (1 << 8)); /* Link fail + CRC */
			pci_write_config16(prev.dev, prev.pos + prev.ctrl_off,
					   ctrl);
			ctrl = pci_read_config16(prev.dev,
						 prev.pos + prev.ctrl_off);
			if (ctrl & ((1 << 4) | (1 << 8))) {
				printk(BIOS_ALERT, "Detected error on "
				       "Hypertransport link\n");
				return;
			}
		}
	} while ((ctrl & (1 << 5)) == 0);

	/* Actually, only for one HT device HT chain, and unitid is 0. */
#if !CONFIG_HT_CHAIN_UNITID_BASE
	if (offset_unitid)
		return;
#endif

	/* Check if is already collapsed. */
	if ((!offset_unitid) || (offset_unitid
	    && (!((CONFIG_HT_CHAIN_END_UNITID_BASE == 0)
	    && (CONFIG_HT_CHAIN_END_UNITID_BASE
	    < CONFIG_HT_CHAIN_UNITID_BASE))))) {

		struct device dummy;
		u32 id;

		dummy.bus = bus;
		dummy.path.type = DEVICE_PATH_PCI;
		dummy.path.pci.devfn = PCI_DEVFN(0, 0);

		id = pci_read_config32(&dummy, PCI_VENDOR_ID);
		if (!((id == 0xffffffff) || (id == 0x00000000)
		    || (id == 0x0000ffff) || (id == 0xffff0000))) {
			return;
		}
	}

	/* Spin through the devices and collapse any early HT enumeration. */
	for (devfn = PCI_DEVFN(1, 0); devfn <= 0xff; devfn += 8) {
		struct device dummy;
		u32 id;
		unsigned pos, flags;

		dummy.bus = bus;
		dummy.path.type = DEVICE_PATH_PCI;
		dummy.path.pci.devfn = devfn;

		id = pci_read_config32(&dummy, PCI_VENDOR_ID);
		if ((id == 0xffffffff) || (id == 0x00000000)
		    || (id == 0x0000ffff) || (id == 0xffff0000)) {
			continue;
		}

		dummy.vendor = id & 0xffff;
		dummy.device = (id >> 16) & 0xffff;
		dummy.hdr_type = pci_read_config8(&dummy, PCI_HEADER_TYPE);

		pos = ht_lookup_slave_capability(&dummy);
		if (!pos)
			continue;

		/* Clear the unitid. */
		flags = pci_read_config16(&dummy, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f;
		pci_write_config16(&dummy, pos + PCI_CAP_FLAGS, flags);
		printk(BIOS_SPEW, "Collapsing %s [%04x/%04x]\n",
		       dev_path(&dummy), dummy.vendor, dummy.device);
	}
}

unsigned int hypertransport_scan_chain(struct bus *bus, unsigned min_devfn,
				       unsigned max_devfn, unsigned int max,
				       unsigned *ht_unitid_base,
				       unsigned offset_unitid)
{
	/*
	 * Even CONFIG_HT_CHAIN_UNITID_BASE == 0, we still can go through this
	 * function, because of end_of_chain check. Also, we need it to
	 * optimize link.
	 */
	unsigned int next_unitid, last_unitid, min_unitid, max_unitid;
	device_t old_devices, dev, func, last_func = 0;
	struct ht_link prev;
	int ht_dev_num = 0;

	min_unitid = (offset_unitid) ? CONFIG_HT_CHAIN_UNITID_BASE : 1;

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
        /*
	 * Let's record the device of last HT device, so we can set the unitid
	 * to CONFIG_HT_CHAIN_END_UNITID_BASE.
	 */
	unsigned int real_last_unitid = 0, end_used = 0;
	u8 real_last_pos = 0;
	device_t real_last_dev = NULL;
#endif

	/* Restore the hypertransport chain to it's uninitialized state. */
	ht_collapse_early_enumeration(bus, offset_unitid);

	/* See which static device nodes I have. */
	old_devices = bus->children;
	bus->children = 0;

	/* Initialize the hypertransport enumeration state. */
	prev.dev = bus->dev;
	prev.pos = bus->cap;

	prev.ctrl_off     = PCI_HT_CAP_HOST_CTRL;
	prev.config_off   = PCI_HT_CAP_HOST_WIDTH;
	prev.freq_off     = PCI_HT_CAP_HOST_FREQ;
	prev.freq_cap_off = PCI_HT_CAP_HOST_FREQ_CAP;

	/* If present, assign unitid to a hypertransport chain. */
	last_unitid = min_unitid -1;
	max_unitid = next_unitid = min_unitid;
	do {
		u8 pos;
		u16 flags, ctrl;
		unsigned int count, static_count;

		last_unitid = next_unitid;

		/* Wait until the link initialization is complete. */
		do {
			ctrl = pci_read_config16(prev.dev,
						 prev.pos + prev.ctrl_off);

			/* End of chain? */
			if (ctrl & (1 << 6))
				goto end_of_chain;

			if (ctrl & ((1 << 4) | (1 << 8))) {
				/*
				 * Either the link has failed, or we have a CRC
				 * error. Sometimes this can happen due to link
				 * retrain, so lets knock it down and see if
				 * it's transient.
				 */
				ctrl |= ((1 << 4) | (1 <<8)); // Link fail + CRC
				pci_write_config16(prev.dev,
					prev.pos + prev.ctrl_off, ctrl);
				ctrl = pci_read_config16(prev.dev,
						prev.pos + prev.ctrl_off);
				if (ctrl & ((1 << 4) | (1 << 8))) {
					printk(BIOS_ALERT, "Detected error on "
					       "hypertransport link\n");
					goto end_of_chain;
				}
			}
		} while ((ctrl & (1 << 5)) == 0);


		/* Get and setup the device_structure. */
		dev = ht_scan_get_devs(&old_devices);

		/* See if a device is present and setup the device structure. */
		dev = pci_probe_dev(dev, bus, 0);
		if (!dev || !dev->enabled)
			break;

		/* Find the hypertransport link capability. */
		pos = ht_lookup_slave_capability(dev);
		if (pos == 0) {
			printk(BIOS_ERR, "%s Hypertransport link capability "
			       "not found", dev_path(dev));
			break;
		}

		/* Update the unitid of the current device. */
		flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);

		/*
		 * If the devices has a unitid set and is at devfn 0 we are
		 * done. This can happen with shadow hypertransport devices,
		 * or if we have reached the bottom of a HT device chain.
		 */
		if (flags & 0x1f)
			break;

		flags &= ~0x1f; /* Mask out base Unit ID. */

		count = (flags >> 5) & 0x1f; /* Het unit count. */

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
		if (offset_unitid) {
			/* max_devfn will be (0x17<<3)|7 or (0x1f<<3)|7. */
			if (next_unitid > (max_devfn >> 3)) {
				if (!end_used) {
			                next_unitid =
					  CONFIG_HT_CHAIN_END_UNITID_BASE;
					end_used = 1;
				} else {
					goto end_of_chain;
				}
			}
		}
#endif

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
		printk(BIOS_SPEW, "%s count: %04x static_count: %04x\n",
		       dev_path(dev), count, static_count);
		if (count < static_count)
			count = static_count;

		/* Update the unitid of the next device. */
		ht_unitid_base[ht_dev_num] = next_unitid;
		ht_dev_num++;

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
		if (offset_unitid) {
			real_last_pos = pos;
			real_last_unitid = next_unitid;
			real_last_dev = dev;
		}
#endif
		next_unitid += count;
		if (next_unitid > max_unitid)
			max_unitid = next_unitid;

		/* Setup the hypertransport link. */
		bus->reset_needed |= ht_setup_link(&prev, dev, pos);

		printk(BIOS_DEBUG, "%s [%04x/%04x] %s next_unitid: %04x\n",
		       dev_path(dev), dev->vendor, dev->device,
		       (dev->enabled? "enabled" : "disabled"), next_unitid);

	} while (last_unitid != next_unitid);

end_of_chain:

#if OPT_HT_LINK == 1
	if (bus->reset_needed)
		printk(BIOS_INFO, "HyperT reset needed\n");
	else
		printk(BIOS_DEBUG, "HyperT reset not needed\n");
#endif

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
	if (offset_unitid && (ht_dev_num > 1)
	    && (real_last_unitid != CONFIG_HT_CHAIN_END_UNITID_BASE)
	    && !end_used) {
		u16 flags;
		flags = pci_read_config16(real_last_dev,
					  real_last_pos + PCI_CAP_FLAGS);
		flags &= ~0x1f;
		flags |= CONFIG_HT_CHAIN_END_UNITID_BASE & 0x1f;
		pci_write_config16(real_last_dev,
				   real_last_pos + PCI_CAP_FLAGS, flags);

		for (func = real_last_dev; func; func = func->sibling) {
			func->path.pci.devfn -= ((real_last_unitid
				- CONFIG_HT_CHAIN_END_UNITID_BASE) << 3);
			last_func = func;
		}

		/* Update last one. */
		ht_unitid_base[ht_dev_num-1] = CONFIG_HT_CHAIN_END_UNITID_BASE;

		printk(BIOS_DEBUG, " unitid: %04x --> %04x\n",
		       real_last_unitid, CONFIG_HT_CHAIN_END_UNITID_BASE);
	}
#endif
	next_unitid = max_unitid;

	if (next_unitid > 0x20)
		next_unitid = 0x20;

	if ((bus->secondary == 0) && (next_unitid > 0x18))
		next_unitid = 0x18; /* Avoid K8 on bus 0. */

	/*
	 * Die if any leftover static devices are are found. There's probably
	 * a problem in devicetree.cb.
	 */
	if (old_devices) {
		device_t left;
		for (left = old_devices; left; left = left->sibling)
			printk(BIOS_DEBUG, "%s\n", dev_path(left));

		printk(BIOS_ERR, "HT: Leftover static devices. "
		       "Check your devicetree.cb\n");

		/*
		 * Put back the leftover static device, and let pci_scan_bus()
		 * disable it.
		 */
		if (last_func && !last_func->sibling)
			last_func->sibling = old_devices;
	}

	/* Now that nothing is overlapping it is safe to scan the children. */
	max = pci_scan_bus(bus, 0x00, ((next_unitid - 1) << 3) | 7, max);
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
 * @param bus TODO
 * @param min_devfn TODO
 * @param max_devfn TODO
 * @param max The highest bus number assigned up to now.
 * @return The maximum bus number found, after scanning all subordinate busses.
 */
static unsigned int hypertransport_scan_chain_x(struct bus *bus,
	unsigned int min_devfn, unsigned int max_devfn, unsigned int max)
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

/** Default device operations for hypertransport bridges */
static struct pci_operations ht_bus_ops_pci = {
	.set_subsystem = 0,
};

struct device_operations default_ht_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = 0,
	.scan_bus         = ht_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &ht_bus_ops_pci,
};
