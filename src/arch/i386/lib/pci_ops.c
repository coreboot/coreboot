#include <console/console.h>
#include <arch/io.h>
#include <arch/pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static const struct pci_ops *conf;
struct pci_ops {
	uint8_t (*read8)   (uint8_t bus, int devfn, int where);
	uint16_t (*read16) (uint8_t bus, int devfn, int where);
	uint32_t (*read32) (uint8_t bus, int devfn, int where);
	void (*write8)  (uint8_t bus, int devfn, int where, uint8_t val);
	void (*write16) (uint8_t bus, int devfn, int where, uint16_t val);
	void (*write32) (uint8_t bus, int devfn, int where, uint32_t val);
};

/*
 * Direct access to PCI hardware...
 */


/*
 * Functions for accessing PCI configuration space with type 1 accesses
 */

#define CONFIG_CMD(bus,devfn, where)   (0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3))

static uint8_t pci_conf1_read_config8(unsigned char bus, int devfn, int where)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	return inb(0xCFC + (where & 3));
}

static uint16_t pci_conf1_read_config16(unsigned char bus, int devfn, int where)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	return inw(0xCFC + (where & 2));
}

static uint32_t pci_conf1_read_config32(unsigned char bus, int devfn, int where)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	return inl(0xCFC);
}

static void  pci_conf1_write_config8(unsigned char bus, int devfn, int where, uint8_t value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outb(value, 0xCFC + (where & 3));
}

static void pci_conf1_write_config16(unsigned char bus, int devfn, int where, uint16_t value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outw(value, 0xCFC + (where & 2));
}

static void pci_conf1_write_config32(unsigned char bus, int devfn, int where, uint32_t value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outl(value, 0xCFC);
}

#undef CONFIG_CMD

static const struct pci_ops pci_direct_conf1 =
{
	.read8  = pci_conf1_read_config8,
	.read16 = pci_conf1_read_config16,
	.read32 = pci_conf1_read_config32,
	.write8  = pci_conf1_write_config8,
	.write16 = pci_conf1_write_config16,
	.write32 = pci_conf1_write_config32,
};

/*
 * Functions for accessing PCI configuration space with type 2 accesses
 */

#define IOADDR(devfn, where)	((0xC000 | ((devfn & 0x78) << 5)) + where)
#define FUNC(devfn)		(((devfn & 7) << 1) | 0xf0)
#define SET(bus,devfn)		outb(FUNC(devfn), 0xCF8); outb(bus, 0xCFA);

static uint8_t pci_conf2_read_config8(unsigned char bus, int devfn, int where)
{
	uint8_t value;
	SET(bus, devfn);
	value = inb(IOADDR(devfn, where));
	outb(0, 0xCF8);
	return value;
}

static uint16_t pci_conf2_read_config16(unsigned char bus, int devfn, int where)
{
	uint16_t value;
	SET(bus, devfn);
	value = inw(IOADDR(devfn, where));
	outb(0, 0xCF8);
	return value;
}

static uint32_t pci_conf2_read_config32(unsigned char bus, int devfn, int where)
{
	uint32_t value;
	SET(bus, devfn);
	value = inl(IOADDR(devfn, where));
	outb(0, 0xCF8);
	return value;
}

static void pci_conf2_write_config8(unsigned char bus, int devfn, int where, uint8_t value)
{
	SET(bus, devfn);
	outb(value, IOADDR(devfn, where));
	outb(0, 0xCF8);
}

static void pci_conf2_write_config16(unsigned char bus, int devfn, int where, uint16_t value)
{
	SET(bus, devfn);
	outw(value, IOADDR(devfn, where));
	outb(0, 0xCF8);
}

static void pci_conf2_write_config32(unsigned char bus, int devfn, int where, uint32_t value)
{
	SET(bus, devfn);
	outl(value, IOADDR(devfn, where));
	outb(0, 0xCF8);
}

#undef SET
#undef IOADDR
#undef FUNC

static const struct pci_ops pci_direct_conf2 =
{
	.read8  = pci_conf2_read_config8,
	.read16 = pci_conf2_read_config16,
	.read32 = pci_conf2_read_config32,
	.write8  = pci_conf2_write_config8,
	.write16 = pci_conf2_write_config16,
	.write32 = pci_conf2_write_config32,
};

/*
 * Before we decide to use direct hardware access mechanisms, we try to do some
 * trivial checks to ensure it at least _seems_ to be working -- we just test
 * whether bus 00 contains a host bridge (this is similar to checking
 * techniques used in XFree86, but ours should be more reliable since we
 * attempt to make use of direct access hints provided by the PCI BIOS).
 *
 * This should be close to trivial, but it isn't, because there are buggy
 * chipsets (yes, you guessed it, by Intel and Compaq) that have no class ID.
 */
static int pci_sanity_check(const struct pci_ops *o)
{
	uint16_t class, vendor;
	uint8_t bus;
	int devfn;
#define PCI_CLASS_BRIDGE_HOST		0x0600
#define PCI_CLASS_DISPLAY_VGA		0x0300
#define PCI_VENDOR_ID_COMPAQ		0x0e11
#define PCI_VENDOR_ID_INTEL		0x8086
#define PCI_VENDOR_ID_MOTOROLA		0x1057

	for (bus = 0, devfn = 0; devfn < 0x100; devfn++) {
		class = o->read16(bus, devfn, PCI_CLASS_DEVICE);
		vendor = o->read16(bus, devfn, PCI_VENDOR_ID);
		if (((class == PCI_CLASS_BRIDGE_HOST) || (class == PCI_CLASS_DISPLAY_VGA)) ||
			((vendor == PCI_VENDOR_ID_INTEL) || (vendor == PCI_VENDOR_ID_COMPAQ) ||
				(vendor == PCI_VENDOR_ID_MOTOROLA))) { 
			return 1;
		}
	}
	printk_err("PCI: Sanity check failed\n");
	return 0;
}

static const struct pci_ops *pci_check_direct(void)
{
	unsigned int tmp;

	/*
	 * Check if configuration type 1 works.
	 */
	{
		outb(0x01, 0xCFB);
		tmp = inl(0xCF8);
		outl(0x80000000, 0xCF8);
		if (inl(0xCF8) == 0x80000000 &&
		    pci_sanity_check(&pci_direct_conf1)) {
			outl(tmp, 0xCF8);
			printk_debug("PCI: Using configuration type 1\n");
			return &pci_direct_conf1;
		}
		outl(tmp, 0xCF8);
	}

	/*
	 * Check if configuration type 2 works.
	 */
	{
		outb(0x00, 0xCFB);
		outb(0x00, 0xCF8);
		outb(0x00, 0xCFA);
		if (inb(0xCF8) == 0x00 && inb(0xCFA) == 0x00 &&
		    pci_sanity_check(&pci_direct_conf2)) {
			printk_debug("PCI: Using configuration type 2\n");
			return &pci_direct_conf2;
		}
	}

	printk_debug("pci_check_direct failed\n");
    
	return 0;
}

uint8_t pci_read_config8(device_t dev, unsigned where)
{
	uint8_t value;
	value = conf->read8(dev->bus->secondary, dev->path.u.pci.devfn, where);
	printk_spew("Read config 8 bus %d,devfn 0x%x,reg 0x%x,val 0x%x\n",
		    dev->bus->secondary, dev->path.u.pci.devfn, where, value);
	return value;
}

uint16_t pci_read_config16(device_t dev, unsigned where)
{
	uint16_t value;
	value = conf->read16(dev->bus->secondary, dev->path.u.pci.devfn, where);
	printk_spew( "Read config 16 bus %d,devfn 0x%x,reg 0x%x,val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, value);
	return value;
}

uint32_t pci_read_config32(device_t dev, unsigned where)
{
	uint32_t value;
	value = conf->read32(dev->bus->secondary, dev->path.u.pci.devfn, where);
	printk_spew( "Read config 32 bus %d,devfn 0x%x,reg 0x%x,val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, value);
	return value;
}

void pci_write_config8(device_t dev, unsigned where, uint8_t val)
{
	printk_spew( "Write config 8 bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, val);
	conf->write8(dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}

void pci_write_config16(device_t dev, unsigned where, uint16_t val)
{
	printk_spew( "Write config 16 bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, val);
	conf->write16(dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}

void pci_write_config32(device_t dev, unsigned where, uint32_t val)
{
	printk_spew( "Write config 32 bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, val);
	conf->write32(dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}

/** Set the method to be used for PCI, type I or type II
 */
void pci_set_method(void)
{
	conf = &pci_direct_conf1;
	conf = pci_check_direct();
}


