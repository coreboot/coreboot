#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/hypertransport_def.h>

static unsigned ht_lookup_slave_capability(device_t dev)
{
	unsigned pos;
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
	while(pos != 0) { /* loop through the linked list */
		uint8_t cap;
		cap = pci_read_config8(dev, pos + PCI_CAP_LIST_ID);
		if (cap == PCI_CAP_ID_HT) {
			uint16_t flags;

			flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
			if ((flags >> 13) == 0) {
				/* Entry is a Slave secondary, success... */
				break;
			}
		}
		pos = pci_read_config8(dev, pos + PCI_CAP_LIST_NEXT);
	}
	return pos;
}

static void ht_collapse_previous_enumeration(unsigned bus)
{
	device_t dev;
	
	/* Spin through the devices and collapse any previous
	 * hypertransport enumeration.
	 */
	for(dev = PCI_DEV(bus, 0, 0); dev <= PCI_DEV(bus, 0x1f, 0x7); dev += PCI_DEV(0, 1, 0)) {
		uint32_t id;
		unsigned pos, flags;
		
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

static unsigned ht_read_freq_cap(device_t dev, unsigned pos)
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

#define LINK_OFFS(WIDTH,FREQ,FREQ_CAP)					\
	(((WIDTH & 0xff) << 16) | ((FREQ & 0xff) << 8) | (FREQ_CAP & 0xFF))

#define LINK_WIDTH(OFFS)    ((OFFS >> 16) & 0xFF)
#define LINK_FREQ(OFFS)     ((OFFS >> 8) & 0xFF)
#define LINK_FREQ_CAP(OFFS) ((OFFS) & 0xFF)

#define PCI_HT_HOST_OFFS LINK_OFFS(		\
		PCI_HT_CAP_HOST_WIDTH,		\
		PCI_HT_CAP_HOST_FREQ,		\
		PCI_HT_CAP_HOST_FREQ_CAP)

#define PCI_HT_SLAVE0_OFFS LINK_OFFS(		\
		PCI_HT_CAP_SLAVE_WIDTH0,	\
		PCI_HT_CAP_SLAVE_FREQ0,		\
		PCI_HT_CAP_SLAVE_FREQ_CAP0)

#define PCI_HT_SLAVE1_OFFS LINK_OFFS(		\
		PCI_HT_CAP_SLAVE_WIDTH1,	\
		PCI_HT_CAP_SLAVE_FREQ1,		\
		PCI_HT_CAP_SLAVE_FREQ_CAP1)

static int ht_optimize_link(
	device_t dev1, uint8_t pos1, unsigned offs1,
	device_t dev2, uint8_t pos2, unsigned offs2)
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
	freq_cap1 = ht_read_freq_cap(dev1, pos1 + LINK_FREQ_CAP(offs1));
	freq_cap2 = ht_read_freq_cap(dev2, pos2 + LINK_FREQ_CAP(offs2));

	/* Calculate the highest possible frequency */
	freq = log2(freq_cap1 & freq_cap2);

	/* See if I am changing the link freqency */
	old_freq = pci_read_config8(dev1, pos1 + LINK_FREQ(offs1));
	needs_reset |= old_freq != freq;
	old_freq = pci_read_config8(dev2, pos2 + LINK_FREQ(offs2));
	needs_reset |= old_freq != freq;

	/* Set the Calulcated link frequency */
	pci_write_config8(dev1, pos1 + LINK_FREQ(offs1), freq);
	pci_write_config8(dev2, pos2 + LINK_FREQ(offs2), freq);

	/* Get the width capabilities */
	width_cap1 = pci_read_config8(dev1, pos1 + LINK_WIDTH(offs1));
	width_cap2 = pci_read_config8(dev2, pos2 + LINK_WIDTH(offs2));

	/* Calculate dev1's input width */
	ln_width1 = link_width_to_pow2[width_cap1 & 7];
	ln_width2 = link_width_to_pow2[(width_cap2 >> 4) & 7];
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width = pow2_to_link_width[ln_width1];
	/* Calculate dev1's output width */
	ln_width1 = link_width_to_pow2[(width_cap1 >> 4) & 7];
	ln_width2 = link_width_to_pow2[width_cap2 & 7];
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width |= pow2_to_link_width[ln_width1] << 4;

	/* See if I am changing dev1's width */
	old_width = pci_read_config8(dev1, pos1 + LINK_WIDTH(offs1) + 1);
	needs_reset |= old_width != width;

	/* Set dev1's widths */
	pci_write_config8(dev1, pos1 + LINK_WIDTH(offs1) + 1, width);

	/* Calculate dev2's width */
	width = ((width & 0x70) >> 4) | ((width & 0x7) << 4);

	/* See if I am changing dev2's width */
	old_width = pci_read_config8(dev2, pos2 + LINK_WIDTH(offs2) + 1);
	needs_reset |= old_width != width;

	/* Set dev2's widths */
	pci_write_config8(dev2, pos2 + LINK_WIDTH(offs2) + 1, width);

	return needs_reset;
}

static int ht_setup_chain(device_t udev, unsigned upos)
{
	/* Assumption the HT chain that is bus 0 has the HT I/O Hub on it.
	 * On most boards this just happens.  If a cpu has multiple
	 * non Coherent links the appropriate bus registers for the
	 * links needs to be programed to point at bus 0.
	 */
	unsigned next_unitid, last_unitid;
	int reset_needed;
	unsigned uoffs;

#warning "FIXME handle multiple chains!"

	/* Make certain the HT bus is not enumerated */
	ht_collapse_previous_enumeration(0);

	reset_needed = 0;
	uoffs = PCI_HT_HOST_OFFS;
	next_unitid = 1;
	do {
		uint32_t id;
		uint8_t pos;
		unsigned flags, count;
		device_t dev = PCI_DEV(0, 0, 0);
		last_unitid = next_unitid;

		id = pci_read_config32(dev, PCI_VENDOR_ID);
		/* If the chain is enumerated quit */
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
		    (((id >> 16) & 0xffff) == 0xffff) ||
		    (((id >> 16) & 0xffff) == 0x0000)) {
			break;
		}

		pos = ht_lookup_slave_capability(dev);
		if (!pos) {
			print_err("HT link capability not found\r\n");
			break;
		}

		/* Setup the Hypertransport link */
		reset_needed |= ht_optimize_link(udev, upos, uoffs, dev, pos, PCI_HT_SLAVE0_OFFS);

		/* Update the Unitid of the current device */
		flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f; /* mask out the bse Unit ID */
		flags |= next_unitid & 0x1f;
		pci_write_config16(dev, pos + PCI_CAP_FLAGS, flags);

		/* Remeber the location of the last device */
		udev = PCI_DEV(0, next_unitid, 0);
		upos = pos;
		uoffs = PCI_HT_SLAVE1_OFFS;

		/* Compute the number of unitids consumed */
		count = (flags >> 5) & 0x1f;
		next_unitid += count;

	} while((last_unitid != next_unitid) && (next_unitid <= 0x1f));
	return reset_needed;
}

struct ht_chain {
	unsigned devreg; 
};
static int ht_setup_chainx(device_t udev, unsigned upos, unsigned bus)
{
	unsigned next_unitid, last_unitid;
	unsigned uoffs;
	int reset_needed=0;

	uoffs = PCI_HT_HOST_OFFS;
	next_unitid = 1;

	do {
		uint32_t id;
		uint8_t pos;
		unsigned flags, count;
		
		device_t dev = PCI_DEV(bus, 0, 0);
		last_unitid = next_unitid;

		id = pci_read_config32(dev, PCI_VENDOR_ID);
		/* If the chain is enumerated quit */
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
		    (((id >> 16) & 0xffff) == 0xffff) ||
		    (((id >> 16) & 0xffff) == 0x0000)) {
			break;
		}
#if 0
		print_debug("bus=");
		print_debug_hex8(bus);
		print_debug(" id =");
		print_debug_hex32(id);
		print_debug("\r\n");
#endif

		
		pos = ht_lookup_slave_capability(dev);
		if (!pos) {
			print_err("HT link capability not found\r\n");
			break;
		}

		/* Setup the Hypertransport link */
		reset_needed |= ht_optimize_link(udev, upos, uoffs, dev, pos, PCI_HT_SLAVE0_OFFS);

		/* Update the Unitid of the current device */
		flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f; /* mask out the bse Unit ID */
		flags |= next_unitid & 0x1f;
		pci_write_config16(dev, pos + PCI_CAP_FLAGS, flags);

		/* Remeber the location of the last device */
		udev = PCI_DEV(0, next_unitid, 0);
		upos = pos;
		uoffs = PCI_HT_SLAVE1_OFFS;

		/* Compute the number of unitids consumed */
		count = (flags >> 5) & 0x1f;
		next_unitid += count;

	} while((last_unitid != next_unitid) && (next_unitid <= 0x1f));
	return reset_needed;
}

static int ht_setup_chains(const struct ht_chain *ht_c, int ht_c_num)
{
	/* Assumption the HT chain that is bus 0 has the HT I/O Hub on it. 
	 * On most boards this just happens.  If a cpu has multiple
	 * non Coherent links the appropriate bus registers for the
	 * links needs to be programed to point at bus 0.
	 */
	int reset_needed; 
        unsigned upos;
        device_t udev;
	int i;

	reset_needed = 0;

	for (i = 0; i < ht_c_num; i++) {
		uint32_t reg;
		unsigned devpos;
		unsigned regpos;
		uint32_t dword;
		unsigned busn;
		
		reg = pci_read_config32(PCI_DEV(0,0x18,1), ht_c[i].devreg);

		//We need setup 0x94, 0xb4, and 0xd4 according to the reg
		devpos = ((reg & 0xf0)>>4)+0x18; // nodeid; it will decide 0x18 or 0x19
		regpos = ((reg & 0xf00)>>8) * 0x20 + 0x94; // link n; it will decide 0x94 or 0xb4, 0x0xd4;
		busn = (reg & 0xff0000)>>16;
		
		dword = pci_read_config32( PCI_DEV(0, devpos, 0), regpos) ;
		dword &= ~(0xffff<<8);
		dword |= (reg & 0xffff0000)>>8;
		pci_write_config32( PCI_DEV(0, devpos,0), regpos , dword);
#if 0
		print_debug("udev=(0,0x");
		print_debug_hex8(devpos);
		print_debug(",0) 0x");
		print_debug_hex8(regpos);
		print_debug("=");
		print_debug_hex32(dword);
#endif
		
	        /* Make certain the HT bus is not enumerated */
        	ht_collapse_previous_enumeration(busn);

		upos = ((reg & 0xf00)>>8) * 0x20 + 0x80;
		udev =  PCI_DEV(0, devpos, 0);
#if 0
                print_debug("\tupos=0x");
                print_debug_hex32(upos);
                print_debug("\r\n");
#endif		
		
		reset_needed |= ht_setup_chainx(udev,upos,busn );

	}

	return reset_needed;
}
