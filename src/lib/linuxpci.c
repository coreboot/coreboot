/*
 *    $Id$
 *
 *      PCI Bus Services, see include/linux/pci.h for further explanation.
 *
 *      Copyright 1993 -- 1997 Drew Eckhardt, Frederic Potter,
 *      David Mosberger-Tang
 *
 *      Copyright 1997 -- 1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 */

#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>
#include <printk.h>
#include <types.h>
#include <string.h>
#include <subr.h>



/**
 * This is the root of the PCI tree. A PCI tree always has 
 * one bus, bus 0. Bus 0 contains devices and bridges. 
 */
struct pci_bus pci_root;
/// Linked list of PCI devices. ALL devices are on this list 
struct pci_dev *pci_devices = 0;
/// pointer to the last device */
static struct pci_dev **pci_last_dev_p = &pci_devices;
/// We're going to probably delete this -- flag to add in reverse order */
static int pci_reverse = 0;

/**
 * Given a bus and a devfn number, find the device structure
 * @param bus The bus number
 * @param devfn a device/function number
 * @return pointer to the device structure
 */
struct pci_dev *pci_find_slot(unsigned int bus, unsigned int devfn)
{
	struct pci_dev *dev;

	for (dev = pci_devices; dev; dev = dev->next)
		if (dev->bus->number == bus && dev->devfn == devfn)
			break;
	return dev;
}

/** Find a device of a given vendor and type
 * @param vendor Vendor ID (e.g. 0x8086 for Intel)
 * @param device Device ID
 * @param from Pointer to the device structure, used as a starting point
 *        in the linked list of devices, which can be 0 to start at the 
 *        head of the list (i.e. pci_devices)
 * @return Pointer to the device struct 
 */
struct pci_dev *pci_find_device(unsigned int vendor, unsigned int device, struct pci_dev *from)
{
	if (!from)
		from = pci_devices;
	else
		from = from->next;
	while (from && (from->vendor != vendor || from->device != device))
		from = from->next;
	return from;
}

/** Find a device of a given class
 * @param class Class of the device
 * @param from Pointer to the device structure, used as a starting point
 *        in the linked list of devices, which can be 0 to start at the 
 *        head of the list (i.e. pci_devices)
 * @return Pointer to the device struct 
 */
struct pci_dev *pci_find_class(unsigned int class, struct pci_dev *from)
{
	if (!from)
		from = pci_devices;
	else
		from = from->next;
	while (from && from->class != class)
		from = from->next;
	return from;
}

/** Given a device, set the PCI_COMMAND_MASTER bit in the command register
 * @param dev Pointer to the device structure
 */
void pci_set_master(struct pci_dev *dev)
{
	u16 cmd;
	u8 lat;

	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	if (!(cmd & PCI_COMMAND_MASTER)) {
		printk_debug("PCI: Enabling bus mastering for device %02x:%02x\n",
		       dev->bus->number, dev->devfn);
		cmd |= PCI_COMMAND_MASTER;
		pci_write_config_word(dev, PCI_COMMAND, cmd);
	}
	pci_read_config_byte(dev, PCI_LATENCY_TIMER, &lat);
	if (lat < 16) {
		printk_debug("PCI: Increasing latency timer of device %02x:%02x to 64\n",
		       dev->bus->number, dev->devfn);
		pci_write_config_byte(dev, PCI_LATENCY_TIMER, 64);
	}
}

/** Given a device and register, read the size of the BAR for that register. 
 * @param dev Pointer to the device structure
 * @param reg Which register to use
 * @param addr Address to load into the register after size is found
 */
void pci_get_size(struct pci_dev *dev, unsigned long reg, unsigned long addr)
{
	u32 size;
	unsigned long type;

	/* FIXME: more consideration for 64-bit PCI devices */
	// get the size
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + (reg << 2), ~0);
	pci_read_config_dword(dev, PCI_BASE_ADDRESS_0 + (reg << 2), &size);

	// restore addr
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + (reg << 2), addr);

	// some broken hardware has read-only registers that do not 
	// really size correctly. You can tell this if addr == size
	// Example: the acer m7229 has BARs 1-4 normally read-only. 
	// so BAR1 at offset 0x10 reads 0x1f1. If you size that register
	// by writing 0xffffffff to it, it will read back as 0x1f1 -- a 
	// violation of the spec. 
	// We catch this case and ignore it by settting size and type to 0.
	// This incidentally catches the common case where registers 
	// read back as 0 for both address and size. 
	if (addr == size) {
		printk_spew(__FUNCTION__
			"dev_fn 0x%x, register %d, read-only"
			" SO, ignoring it\n",
			dev->devfn, reg);
		printk_spew("addr was 0x%x, size was 0x%x\n",addr,size); 
		type = 0;
         	size = 0;
         }
	// Now compute the actual size, See PCI Spec 6.2.5.1 ... 
         else if (size & PCI_BASE_ADDRESS_SPACE_IO) {
		type = size & (~PCI_BASE_ADDRESS_IO_MASK);
		size &= (PCI_BASE_ADDRESS_IO_MASK);
		// BUG! Top 16 bits can be zero (or not) 
		// So set them to 0xffff so they go away ...
		size |= 0xffff0000;
		size = ~size;
		size++;
	} else {
		type = size & (~PCI_BASE_ADDRESS_MEM_MASK);
		size &= (PCI_BASE_ADDRESS_MEM_MASK);
		size = ~size;
		size++;
	}
	dev->size[reg] = size | type;
}

/** Read the base address registers for a given device. 
 * @param dev Pointer to the dev structure
 * @param howmany How many registers to read (6 for device, 2 for bridge)
 */
void pci_read_bases(struct pci_dev *dev, unsigned int howmany)
{
	unsigned int reg;
	u32 /* unsigned long for 64 bits ?? */ addr;

	/* FIXME: to deal with 64-bits PCI */
	for (reg = 0; reg < howmany; reg++) {
		pci_read_config_dword(dev, PCI_BASE_ADDRESS_0 + (reg << 2), &addr);
		if (addr == 0xffffffff)
			continue;

		/* get address space size */
		pci_get_size(dev, reg, addr);

		addr &= (PCI_BASE_ADDRESS_SPACE | PCI_BASE_ADDRESS_MEM_TYPE_MASK);
		if (addr == (PCI_BASE_ADDRESS_SPACE_MEMORY | PCI_BASE_ADDRESS_MEM_TYPE_64)) {
			printk_debug("reg %d is 64-bit\n", reg);
			/* this is a 64-bit memory base address */
			reg++;
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_0 + (reg << 2), &addr);
			if (addr) {
#if BITS_PER_LONG == 64
				dev->base_address[reg - 1] |= ((unsigned long) addr) << 32;
#else
				printk_err("PCI: Unable to handle 64-bit address for device "
				       "%02x:%02x\n", dev->bus->number, dev->devfn);
				dev->base_address[reg - 1] = 0;
#endif
			}
		}
	}
}

/** Scan the bus, first for bridges and next for devices. 
 * @param pci_bus pointer to the bus structure
 * @return The maximum bus number found, after scanning all subordinate busses
 */
unsigned int pci_scan_bus(struct pci_bus *bus)
{
	unsigned int devfn, max;
	struct pci_dev *dev, **bus_last;
	struct pci_bus *child;

	printk_debug("PCI: pci_scan_bus for bus %d\n", bus->number);

	bus_last = &bus->devices;
	max = bus->secondary;

	post_code(0x24);

	/* probe all devices on this bus with some optimization for non-existance and 
	   single funcion devices */
	for (devfn = 0; devfn < 0xff; devfn++) {
		u32 id, class, addr;
		u8 cmd, tmp, hdr_type;

		// gcc just went to hell. Don't test -- this always
		// returns 0 anyway. 
#if GCC_WORKS_ON_O2
                if (pcibios_read_config_dword(bus->number, devfn, PCI_VENDOR_ID, &id))	{
		    printk_spew("PCI: devfn 0x%x, read_config_dword fails\n", 
				devfn);
		    continue;
		}
#endif
                pcibios_read_config_dword(bus->number, devfn, PCI_VENDOR_ID, &id);

		/* some broken boards return 0 if a slot is empty: */
		if (id == 0xffffffff || id == 0x00000000 || id == 0x0000ffff || id == 0xffff0000) {
		    printk_spew("PCI: devfn 0x%x, bad id 0x%x\n", devfn, id);
			if (PCI_FUNC(devfn) == 0x00) {
				/* if this is a function 0 device and it is not present,
				   skip to next device */
				devfn += 0x07;
			}
			/* multi function device, skip to next function */
			continue;
		}

                if (pcibios_read_config_byte(bus->number, devfn, PCI_HEADER_TYPE, &hdr_type)){
		    printk_spew("PCI: devfn 0x%x, header type read fails\n", devfn);
		    continue;
		}

		if (pcibios_read_config_dword(bus->number, devfn, PCI_CLASS_REVISION, &class)) {
		    printk_spew("PCI: devfn 0x%x, class read fails\n", devfn);
			continue;
		}

		if ((dev = kmalloc(sizeof(*dev), GFP_ATOMIC)) == 0) {
			printk_err("PCI: out of memory.\n");
			continue;
		}

		memset(dev, 0, sizeof(*dev));
		dev->bus = bus;
		dev->devfn = devfn;
		dev->vendor = id & 0xffff;
		dev->device = (id >> 16) & 0xffff;
		dev->hdr_type = hdr_type;
		/* class code, the upper 3 bytes of PCI_CLASS_REVISION */
		dev->class = class >> 8;
		class >>= 16;

		/* non-destructively determine if device can be a master: */
		pcibios_read_config_byte(bus->number, devfn, PCI_COMMAND, &cmd);
		pcibios_write_config_byte(bus->number, devfn, PCI_COMMAND,
					  cmd | PCI_COMMAND_MASTER);
		pcibios_read_config_byte(bus->number, devfn, PCI_COMMAND, &tmp);
		dev->master = ((tmp & PCI_COMMAND_MASTER) != 0);
		pcibios_write_config_byte(bus->number, devfn, PCI_COMMAND, cmd);

		switch (hdr_type & 0x7f) {	/* header type */
		case PCI_HEADER_TYPE_NORMAL:	/* standard header */
			if (class == PCI_CLASS_BRIDGE_PCI)
				goto bad;
			/*  read base address registers, again pci_fixup() can tweak these */
			pci_read_bases(dev, 6);
			pcibios_read_config_dword(bus->number, devfn, PCI_ROM_ADDRESS, &addr);
			dev->rom_address = (addr == 0xffffffff) ? 0 : addr;
			break;
		case PCI_HEADER_TYPE_BRIDGE:	/* bridge header */
			if (class != PCI_CLASS_BRIDGE_PCI)
				goto bad;
			pci_read_bases(dev, 2);
			pcibios_read_config_dword(bus->number, devfn, PCI_ROM_ADDRESS1, &addr);
			dev->rom_address = (addr == 0xffffffff) ? 0 : addr;
			break;
		case PCI_HEADER_TYPE_CARDBUS:	/* CardBus bridge header */
			if (class != PCI_CLASS_BRIDGE_CARDBUS)
				goto bad;
			pci_read_bases(dev, 1);
			break;
		default:	/* unknown header */
		bad:
			printk_err("PCI: %02x:%02x [%04x/%04x/%06x] has unknown header "
			       "type %02x, ignoring.\n",
			       bus->number, dev->devfn, dev->vendor, dev->device, class,
			       hdr_type);
			continue;
		}

		printk_debug("PCI: %02x:%02x [%04x/%04x]\n", bus->number, dev->devfn,
		    dev->vendor, dev->device);

		/* Put it into the global PCI device chain. It's used to find devices once
		   everything is set up. */
		if (!pci_reverse) {
			*pci_last_dev_p = dev;
			pci_last_dev_p = &dev->next;
		} else {
			dev->next = pci_devices;
			pci_devices = dev;
		}

		/* Now insert it into the list of devices held by the parent bus. */
		*bus_last = dev;
		bus_last = &dev->sibling;

		if (PCI_FUNC(devfn) == 0x00 && (hdr_type & 0x80) != 0x80) {
			/* if this is not a multi function device, don't waste time probe
			   another function. Skip to next device. */
			devfn += 0x07;
		}
	}

	post_code(0x25);
	/*
	 * After performing arch-dependent fixup of the bus, look behind
	 * all PCI-to-PCI bridges on this bus.
	 */
	//pcibios_fixup_bus(bus);
	/*
	 * The fixup code may have just found some peer pci bridges on this
	 * machine.  Update the max variable if that happened so we don't
	 * get duplicate bus numbers.
	 */
	for (child = &pci_root; child; child = child->next)
		max = ((max > child->subordinate) ? max : child->subordinate);

	for (dev = bus->devices; dev; dev = dev->sibling)
		/* If it's a bridge, scan the bus behind it. */
		if ((dev->class >> 8) == PCI_CLASS_BRIDGE_PCI) {
			u32 buses;
			unsigned int devfn = dev->devfn;
			unsigned short cr;
#define NOTUSED
#ifdef NOTUSED
			/*
			 * Check for a duplicate bus.  If we already scanned
			 * this bus number as a peer bus, don't also scan it
			 * as a child bus
			 */
			if (((dev->vendor == PCI_VENDOR_ID_SERVERWORKS) &&
			     ((dev->device == PCI_DEVICE_ID_SERVERWORKS_HE) ||
			      (dev->device == PCI_DEVICE_ID_SERVERWORKS_LE))) ||
			    ((dev->vendor == PCI_VENDOR_ID_COMPAQ) &&
			     (dev->device == PCI_DEVICE_ID_COMPAQ_6010)) ||
			    ((dev->vendor == PCI_VENDOR_ID_INTEL) &&
			     ((dev->device == PCI_DEVICE_ID_INTEL_82454NX)||
			      (dev->device == PCI_DEVICE_ID_INTEL_82451NX))))
				goto skip_it;

			/* Read the existing primary/secondary/subordinate bus number
			   configuration to determine if the PCI bridge has already been
			   configured by the system.  If so, check to see if we've already
			   scanned this bus as a result of peer bus scanning, if so, skip this.
			   FIMXE: We are BIOS, is there anyone else doing this dirty job BEFORE us ?? */
			pcibios_read_config_dword(bus->number, devfn, PCI_PRIMARY_BUS, &buses);
			if ((buses & 0xFFFFFF) != 0) {
				for (child = pci_root.next; child; child = child->next)
					if (child->number == ((buses >> 8) & 0xff))
						goto skip_it;
			}
#endif
			/* Insert it into the tree of buses. */
			if ((child = kmalloc(sizeof(*child), GFP_ATOMIC)) == 0) {
				printk_err("PCI: out of memory for bridge.\n");
				continue;
			}
			memset(child, 0, sizeof(*child));
			child->next = bus->children;
			bus->children = child;
			child->self = dev;
			child->parent = bus;

			/* Set up the primary, secondary and subordinate bus numbers. We have
			   no idea how many buses are behind this bridge yet, so we set the
			   subordinate bus number to 0xff for the moment */
			child->number = child->secondary = ++max;
			child->primary = bus->secondary;
			child->subordinate = 0xff;

			/* Clear all status bits and turn off memory, I/O and master enables. */
			pcibios_read_config_word(bus->number, devfn, PCI_COMMAND, &cr);
			pcibios_write_config_word(bus->number, devfn, PCI_COMMAND, 0x0000);
			pcibios_write_config_word(bus->number, devfn, PCI_STATUS, 0xffff);

			/*
			 * Read the existing primary/secondary/subordinate bus
			 * number configuration to determine if the PCI bridge
			 * has already been configured by the system.  If so,
			 * do not modify the configuration, merely note it.
			 */
			pcibios_read_config_dword(bus->number, devfn, PCI_PRIMARY_BUS, &buses);

#ifdef BRIDGE_CONFIGURED_AT_POWERUP
			// There is some hardware (ALPHA) that configures bridges in hardware, at bootup. 
			// We need to take that into account at some point. 
			// At the same time, we're finding buggy bridge hardware that comes up 
			// with these registers non-zero (VIA VT8601). Hence this #ifdef -- in some cases, 
			// you should never check the buses; in other cases, you have no choice. 
			if ((buses & 0xFFFFFF) != 0) {
				unsigned int cmax;

				child->primary = buses & 0xFF;
				child->secondary = (buses >> 8) & 0xFF;
				child->subordinate = (buses >> 16) & 0xFF;
				child->number = child->secondary;
				cmax = pci_scan_bus(child);
				if (cmax > max)
					max = cmax;
			} else
#endif 
			{
				/* Configure the bus numbers for this bridge: the configuration
				   transactions will not be propagated by the bridge if it is not
				   correctly configured */
				buses &= 0xff000000;
				buses |= (((unsigned int) (child->primary) << 0) |
					  ((unsigned int) (child->secondary) << 8) |
					  ((unsigned int) (child->subordinate) << 16));
				pcibios_write_config_dword(bus->number, devfn,
							   PCI_PRIMARY_BUS, buses);

				/* Now we can scan all subordinate buses i.e. the bus hehind the bridge */
				max = pci_scan_bus(child);

				/* We know the number of buses behind this bridge. Set the subordinate
				   bus number to its real value */
				child->subordinate = max;
				buses = (buses & 0xff00ffff) |
					((unsigned int) (child->subordinate) << 16);
				pcibios_write_config_dword(bus->number, devfn,
							   PCI_PRIMARY_BUS, buses);
			}
			
			pcibios_write_config_word(bus->number, devfn, PCI_COMMAND, cr);
		skip_it:
		}
	/*
	 * We've scanned the bus and so we know all about what's on
	 * the other side of any bridges that may be on this bus plus
	 * any devices.
	 *
	 * Return how far we've got finding sub-buses.
	 */
	printk_debug("PCI: pci_scan_bus returning with max=%02x\n", max);
	post_code(0x55);
	return max;
}

/** Scan a peer bridge. 
 * First, scan all the bus structs for this bus number. If this bus
 * is found, that means it was configured, so just return the pointer. 
 * If the bus is not found, it needs to be configured. Allocate memory
 * for the struct, link it in to the linked list of bridges, and then 
 * scan it. 
 * @param bus The bus number supported by the peer bridge
 * @return Pointer to the bus struct for this bus number. 
 */
struct pci_bus *pci_scan_peer_bridge(int bus)
{
	struct pci_bus *b;

	b = &pci_root;
	while ((b != 0) && (bus != 0)) {
		if (b->number == bus)
			return (b);
		b = b->next;
	}
	b = kmalloc(sizeof(*b), GFP_KERNEL);
	memset(b, 0, sizeof(*b));
	b->next = pci_root.next;
	pci_root.next = b;
	b->number = b->secondary = bus;
	b->subordinate = pci_scan_bus(b);
	return b;
}

/** Initialize pci root struct, then scan starting at the root. 
 * Note that this function will recurse at each bridge. 
 */
void pci_init(void)
{
	memset(&pci_root, 0, sizeof(pci_root));
	pci_root.subordinate = pci_scan_bus(&pci_root);
}
