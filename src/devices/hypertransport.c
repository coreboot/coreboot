#include <bitops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <device/chip.h>
#include <part/hard_reset.h>
#include <part/fallback_boot.h>

static device_t ht_scan_get_devs(device_t *old_devices)
{
	device_t first, last;
	first = *old_devices;
	last = first;
	while (last && last->sibling && 
	       (last->sibling->path.type == DEVICE_PATH_PCI) &&
	       (last->sibling->path.u.pci.devfn > last->path.u.pci.devfn)) {
		last = last->sibling;
	}
	if (first) {
		*old_devices = last->sibling;
		last->sibling = 0;
	}
	return first;
}

static unsigned ht_read_freq_cap(device_t dev, unsigned pos)
{
	/* Handle bugs in valid hypertransport frequency reporting */
	unsigned freq_cap;

	freq_cap = pci_read_config16(dev, pos);
	freq_cap &= ~(1 << HT_FREQ_VENDOR); /* Ignore Vendor HT frequencies */

	/* AMD 8131 Errata 48 */
	if ((dev->vendor == PCI_VENDOR_ID_AMD) &&
		(dev->device == PCI_DEVICE_ID_AMD_8131_PCIX)) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
	}
	/* AMD 8151 Errata 23 */
	if ((dev->vendor == PCI_VENDOR_ID_AMD) &&
		(dev->device == PCI_DEVICE_ID_AMD_8151_SYSCTRL)) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
	}
	/* AMD K8 Unsupported 1Ghz? */
	if ((dev->vendor == PCI_VENDOR_ID_AMD) && (dev->device == 0x1100)) {
		freq_cap &= ~(1 << HT_FREQ_1000Mhz);
	}
	return freq_cap;
}

struct prev_link {
	struct device *dev;
	unsigned pos;
	unsigned char config_off, freq_off, freq_cap_off;
};

static int ht_setup_link(struct prev_link *prev, device_t dev, unsigned pos)
{
	static const uint8_t link_width_to_pow2[]= { 3, 4, 0, 5, 1, 2, 0, 0 };
	static const uint8_t pow2_to_link_width[] = { 0x7, 4, 5, 0, 1, 3 };
	unsigned present_width_cap,    upstream_width_cap;
	unsigned present_freq_cap,     upstream_freq_cap;
	unsigned ln_present_width_in,  ln_upstream_width_in; 
	unsigned ln_present_width_out, ln_upstream_width_out;
	unsigned freq, old_freq;
	unsigned present_width, upstream_width, old_width;
	int reset_needed;

	/* Set the hypertransport link width and frequency */
	reset_needed = 0;

	/* Read the capabilities */
	present_freq_cap   = ht_read_freq_cap(dev, pos + PCI_HT_CAP_SLAVE_FREQ_CAP0);
	upstream_freq_cap  = ht_read_freq_cap(prev->dev, prev->pos + prev->freq_cap_off);
	present_width_cap  = pci_read_config8(dev, pos + PCI_HT_CAP_SLAVE_WIDTH0);
	upstream_width_cap = pci_read_config8(prev->dev, prev->pos + prev->config_off);
	
	/* Calculate the highest useable frequency */
	freq = log2(present_freq_cap & upstream_freq_cap);

	/* Calculate the highest width */
	ln_upstream_width_in = link_width_to_pow2[upstream_width_cap & 7];
	ln_present_width_out = link_width_to_pow2[(present_width_cap >> 4) & 7];
	if (ln_upstream_width_in > ln_present_width_out) {
		ln_upstream_width_in = ln_present_width_out;
	}
	upstream_width = pow2_to_link_width[ln_upstream_width_in];
	present_width  = pow2_to_link_width[ln_upstream_width_in] << 4;

	ln_upstream_width_out = link_width_to_pow2[(upstream_width_cap >> 4) & 7];
	ln_present_width_in   = link_width_to_pow2[present_width_cap & 7];
	if (ln_upstream_width_out > ln_present_width_in) {
		ln_upstream_width_out = ln_present_width_in;
	}
	upstream_width |= pow2_to_link_width[ln_upstream_width_out] << 4;
	present_width  |= pow2_to_link_width[ln_upstream_width_out];

	/* Set the current device */
	old_freq = pci_read_config8(dev, pos + PCI_HT_CAP_SLAVE_FREQ0);
	if (freq != old_freq) {
		pci_write_config8(dev, pos + PCI_HT_CAP_SLAVE_FREQ0, freq);
		reset_needed = 1;
		printk_spew("HyperT FreqP old %x new %x\n",old_freq,freq);
	}
	old_width = pci_read_config8(dev, pos + PCI_HT_CAP_SLAVE_WIDTH0 + 1);
	if (present_width != old_width) {
		pci_write_config8(dev, pos + PCI_HT_CAP_SLAVE_WIDTH0 + 1, present_width);
		reset_needed = 1;
		printk_spew("HyperT widthP old %x new %x\n",old_width, present_width);
	}

	/* Set the upstream device */
	old_freq = pci_read_config8(prev->dev, prev->pos + prev->freq_off);
	old_freq &= 0x0f;
	if (freq != old_freq) {
		pci_write_config8(prev->dev, prev->pos + prev->freq_off, freq);
		reset_needed = 1;
		printk_spew("HyperT freqU old %x new %x\n", old_freq, freq);
	}
	old_width = pci_read_config8(prev->dev, prev->pos + prev->config_off + 1);
	if (upstream_width != old_width) {
		pci_write_config8(prev->dev, prev->pos + prev->config_off + 1, upstream_width);
		reset_needed = 1;
		printk_spew("HyperT widthU old %x new %x\n", old_width, upstream_width);
	}
	
	/* Remember the current link as the previous link */
	prev->dev = dev;
	prev->pos = pos;
	prev->config_off   = PCI_HT_CAP_SLAVE_WIDTH1;
	prev->freq_off     = PCI_HT_CAP_SLAVE_FREQ1;
	prev->freq_cap_off = PCI_HT_CAP_SLAVE_FREQ_CAP1;

	return reset_needed;
		
}

static unsigned ht_lookup_slave_capability(struct device *dev)
{
	unsigned pos;
	pos = 0;
	switch(dev->hdr_type & 0x7f) {
	case PCI_HEADER_TYPE_NORMAL:
	case PCI_HEADER_TYPE_BRIDGE:
		pos = PCI_CAPABILITY_LIST;
		break;
	}
	if (pos > PCI_CAP_LIST_NEXT) {
		pos = pci_read_config8(dev, pos);
	}
	while(pos != 0) {   /* loop through the linked list */
		uint8_t cap;
		cap = pci_read_config8(dev, pos + PCI_CAP_LIST_ID);
		printk_spew("Capability: 0x%02x @ 0x%02x\n", cap, pos);
		if (cap == PCI_CAP_ID_HT) {
			unsigned flags;
			flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
			printk_spew("flags: 0x%04x\n", (unsigned)flags);
			if ((flags >> 13) == 0) {
				/* Entry is a Slave secondary, success...*/
				break;
			}
		}
		pos = pci_read_config8(dev, pos + PCI_CAP_LIST_NEXT);
	}
	return pos;
}

static void ht_collapse_early_enumeration(struct bus *bus)
{
	unsigned int devfn;

	/* Spin through the devices and collapse any early
	 * hypertransport enumeration.
	 */
	for(devfn = 0; devfn <= 0xff; devfn += 8) {
		struct device dummy;
		uint32_t id;
		unsigned pos, flags;
		dummy.bus              = bus;
		dummy.path.type        = DEVICE_PATH_PCI;
		dummy.path.u.pci.devfn = devfn;
		id = pci_read_config32(&dummy, PCI_VENDOR_ID);
		if (id == 0xffffffff || id == 0x00000000 || 
			id == 0x0000ffff || id == 0xffff0000) {
			continue;
		}
		dummy.vendor = id & 0xffff;
		dummy.device = (id >> 16) & 0xffff;
		dummy.hdr_type = pci_read_config8(&dummy, PCI_HEADER_TYPE);
		pos = ht_lookup_slave_capability(&dummy);
		if (!pos){
			continue;
		}

		/* Clear the unitid */
		flags = pci_read_config16(&dummy, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f;
		pci_write_config16(&dummy, pos + PCI_CAP_FLAGS, flags);
		printk_spew("Collapsing %s [%04x/%04x]\n", 
			dev_path(&dummy), dummy.vendor, dummy.device);
	}
}

unsigned int hypertransport_scan_chain(struct bus *bus, unsigned int max)
{
	unsigned next_unitid, last_unitid, previous_unitid;
	uint8_t previous_pos;
	device_t old_devices, dev, func, *chain_last;
	unsigned min_unitid = 1;
	int reset_needed;
	struct prev_link prev;

	/* Restore the hypertransport chain to it's unitialized state */
	ht_collapse_early_enumeration(bus);

	/* See which static device nodes I have */
	old_devices = bus->children;
	bus->children = 0;
	chain_last = &bus->children;

	/* Initialize the hypertransport enumeration state */
	reset_needed = 0;
	prev.dev = bus->dev;
	prev.pos = bus->cap;
	prev.config_off   = PCI_HT_CAP_HOST_WIDTH;
	prev.freq_off     = PCI_HT_CAP_HOST_FREQ;
	prev.freq_cap_off = PCI_HT_CAP_HOST_FREQ_CAP;
	
	/* If present assign unitid to a hypertransport chain */
	last_unitid = min_unitid -1;
	next_unitid = min_unitid;
	previous_pos = 0;
	do {
		uint32_t id, class;
		uint8_t hdr_type, pos;
		uint16_t flags;
		unsigned count, static_count;

		previous_unitid = last_unitid;
		last_unitid = next_unitid;

		/* Get setup the device_structure */
		dev = ht_scan_get_devs(&old_devices);

		if (!dev) {
			struct device dummy;
			dummy.bus              = bus;
			dummy.path.type        = DEVICE_PATH_PCI;
			dummy.path.u.pci.devfn = 0;
			id = pci_read_config32(&dummy, PCI_VENDOR_ID);
			/* If the chain is fully enumerated quit */
			if (id == 0xffffffff || id == 0x00000000 ||
				id == 0x0000ffff || id == 0xffff0000) {
				break;
			}
			dev = alloc_dev(bus, &dummy.path);
		}
		else {
			/* Add this device to the pci bus chain */
			*chain_last = dev;
			/* Run the magice enable sequence for the device */
			if (dev->chip && dev->chip->control && dev->chip->control->enable_dev) {
				int enable  = dev->enabled;
				dev->enabled = 1;
				dev->chip->control->enable_dev(dev);
				dev->enabled = enable;
			}
			/* Now read the vendor and device id */
			id = pci_read_config32(dev, PCI_VENDOR_ID);

			/* If the chain is fully enumerated quit */
			if (id == 0xffffffff || id == 0x00000000 ||
				id == 0x0000ffff || id == 0xffff0000) {
				printk_err("Missing static device: %s\n",
					dev_path(dev));
				break;
			}
		}
		/* Update the device chain tail */
		for(func = dev; func; func = func->sibling) {
			chain_last = &func->sibling;
		}
		
		/* Read the rest of the pci configuration information */
		hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
		class = pci_read_config32(dev, PCI_CLASS_REVISION);
		
		/* Store the interesting information in the device structure */
		dev->vendor = id & 0xffff;
		dev->device = (id >> 16) & 0xffff;
		dev->hdr_type = hdr_type;
		/* class code, the upper 3 bytes of PCI_CLASS_REVISION */
		dev->class = class >> 8;

		/* Find the hypertransport link capability */
		pos = ht_lookup_slave_capability(dev);
		if (pos == 0) {
			printk_err("%s Hypertransport link capability not found", 
				dev_path(dev));
			break;
		}
		
		/* Update the Unitid of the current device */
		flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f; /* mask out base Unit ID */
		flags |= next_unitid & 0x1f;
		pci_write_config16(dev, pos + PCI_CAP_FLAGS, flags);

		/* Update the Unitd id in the device structure */
		static_count = 1;
		for(func = dev; func; func = func->sibling) {
			func->path.u.pci.devfn += (next_unitid << 3);
			static_count = (func->path.u.pci.devfn >> 3) 
				- (dev->path.u.pci.devfn >> 3) + 1;
		}

		/* Compute the number of unitids consumed */
		count = (flags >> 5) & 0x1f; /* get unit count */
		printk_spew("%s count: %04x static_count: %04x\n", 
			dev_path(dev), count, static_count);
		if (count < static_count) {
			count = static_count;
		}

		/* Update the Unitid of the next device */
		next_unitid += count;

		/* Setup the hypetransport link */
		reset_needed |= ht_setup_link(&prev, dev, pos);

		printk_debug("%s [%04x/%04x] %s next_unitid: %04x\n",
			dev_path(dev),
			dev->vendor, dev->device, 
			(dev->enabled? "enabled": "disabled"), next_unitid);

	} while((last_unitid != next_unitid) && (next_unitid <= 0x1f));
#if HAVE_HARD_RESET == 1
	if(reset_needed) {
		printk_info("HyperT reset needed\n");
		hard_reset();
	}
	else {
		printk_debug("HyperT reset not needed\n");
	}
#endif
	if (next_unitid > 0x1f) {
		next_unitid = 0x1f;
	}
	return pci_scan_bus(bus, 0x00, (next_unitid << 3)|7, max);
}
