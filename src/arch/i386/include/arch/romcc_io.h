#ifndef ARCH_ROMCC_IO_H
#define ARCH_ROMCC_IO_H 1

#include <stdint.h>

// arch/io.h is pulled in in many places but it could 
// also be pulled in here for all romcc/romstage code.
// #include <arch/io.h>

#if CONFIG_MMCONF_SUPPORT

#include <arch/mmio_conf.h>

#endif

static inline int log2(int value)
{
        unsigned int r = 0;
        __asm__ volatile (
                "bsrl %1, %0\n\t"
                "jnz 1f\n\t"
                "movl $-1, %0\n\t"
                "1:\n\t"
                : "=r" (r) : "r" (value));
        return r;

}
static inline int log2f(int value)
{
        unsigned int r = 0;
        __asm__ volatile (
                "bsfl %1, %0\n\t"
                "jnz 1f\n\t"
                "movl $-1, %0\n\t"
                "1:\n\t"
                : "=r" (r) : "r" (value));
        return r;

}

#define PCI_ADDR(SEGBUS, DEV, FN, WHERE) ( \
        (((SEGBUS) & 0xFFF) << 20) | \
        (((DEV) & 0x1F) << 15) | \
        (((FN) & 0x07) << 12) | \
        ((WHERE) & 0xFFF))

#define PCI_DEV(SEGBUS, DEV, FN) ( \
        (((SEGBUS) & 0xFFF) << 20) | \
        (((DEV) & 0x1F) << 15) | \
        (((FN)  & 0x07) << 12))

#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))


#define PNP_DEV(PORT, FUNC) (((PORT) << 8) | (FUNC))

typedef unsigned device_t; /* pci and pci_mmio need to have different ways to have dev */

/* FIXME: We need to make the coreboot to run at 64bit mode, So when read/write memory above 4G,
 * We don't need to set %fs, and %gs anymore
 * Before that We need to use %gs, and leave %fs to other RAM access
 */

static inline __attribute__((always_inline)) uint8_t pci_io_read_config8(device_t dev, unsigned where)
{
	unsigned addr;
#if CONFIG_PCI_IO_CFG_EXT == 0
	addr = (dev>>4) | where;
#else
	addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16); //seg == 0
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inb(0xCFC + (addr & 3));
}

#if CONFIG_MMCONF_SUPPORT
static inline __attribute__((always_inline)) uint8_t pci_mmio_read_config8(device_t dev, unsigned where)
{
        unsigned addr;
        addr = CONFIG_MMCONF_BASE_ADDRESS | dev | where;
        return read8x(addr);
}
#endif
static inline __attribute__((always_inline)) uint8_t pci_read_config8(device_t dev, unsigned where)
{
#if CONFIG_MMCONF_SUPPORT_DEFAULT
	return pci_mmio_read_config8(dev, where);
#else
	return pci_io_read_config8(dev, where);
#endif
}

static inline __attribute__((always_inline)) uint16_t pci_io_read_config16(device_t dev, unsigned where)
{
	unsigned addr;
#if CONFIG_PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inw(0xCFC + (addr & 2));
}

#if CONFIG_MMCONF_SUPPORT
static inline __attribute__((always_inline)) uint16_t pci_mmio_read_config16(device_t dev, unsigned where)
{
        unsigned addr;
        addr = CONFIG_MMCONF_BASE_ADDRESS | dev | (where & ~1);
        return read16x(addr);
}
#endif

static inline __attribute__((always_inline)) uint16_t pci_read_config16(device_t dev, unsigned where)
{
#if CONFIG_MMCONF_SUPPORT_DEFAULT
	return pci_mmio_read_config16(dev, where);
#else
        return pci_io_read_config16(dev, where);
#endif
}


static inline __attribute__((always_inline)) uint32_t pci_io_read_config32(device_t dev, unsigned where)
{
	unsigned addr;
#if CONFIG_PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inl(0xCFC);
}

#if CONFIG_MMCONF_SUPPORT
static inline __attribute__((always_inline)) uint32_t pci_mmio_read_config32(device_t dev, unsigned where)
{
        unsigned addr;
        addr = CONFIG_MMCONF_BASE_ADDRESS | dev | (where & ~3);
        return read32x(addr);
}
#endif

static inline __attribute__((always_inline)) uint32_t pci_read_config32(device_t dev, unsigned where)
{
#if CONFIG_MMCONF_SUPPORT_DEFAULT
	return pci_mmio_read_config32(dev, where);
#else
        return pci_io_read_config32(dev, where);
#endif
}

static inline __attribute__((always_inline)) void pci_io_write_config8(device_t dev, unsigned where, uint8_t value)
{
	unsigned addr;
#if CONFIG_PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outb(value, 0xCFC + (addr & 3));
}

#if CONFIG_MMCONF_SUPPORT
static inline __attribute__((always_inline)) void pci_mmio_write_config8(device_t dev, unsigned where, uint8_t value)
{
        unsigned addr;
        addr = CONFIG_MMCONF_BASE_ADDRESS | dev | where;
        write8x(addr, value);
}
#endif

static inline __attribute__((always_inline)) void pci_write_config8(device_t dev, unsigned where, uint8_t value)
{
#if CONFIG_MMCONF_SUPPORT_DEFAULT
	pci_mmio_write_config8(dev, where, value);
#else
        pci_io_write_config8(dev, where, value);
#endif
}


static inline __attribute__((always_inline)) void pci_io_write_config16(device_t dev, unsigned where, uint16_t value)
{
        unsigned addr;
#if CONFIG_PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outw(value, 0xCFC + (addr & 2));
}

#if CONFIG_MMCONF_SUPPORT
static inline __attribute__((always_inline)) void pci_mmio_write_config16(device_t dev, unsigned where, uint16_t value)
{
        unsigned addr;
        addr = CONFIG_MMCONF_BASE_ADDRESS | dev | (where & ~1);
        write16x(addr, value);
}
#endif

static inline __attribute__((always_inline)) void pci_write_config16(device_t dev, unsigned where, uint16_t value)
{
#if CONFIG_MMCONF_SUPPORT_DEFAULT
	pci_mmio_write_config16(dev, where, value);
#else
	pci_io_write_config16(dev, where, value);
#endif
}


static inline __attribute__((always_inline)) void pci_io_write_config32(device_t dev, unsigned where, uint32_t value)
{
	unsigned addr;
#if CONFIG_PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outl(value, 0xCFC);
}

#if CONFIG_MMCONF_SUPPORT
static inline __attribute__((always_inline)) void pci_mmio_write_config32(device_t dev, unsigned where, uint32_t value)
{
        unsigned addr;
        addr = CONFIG_MMCONF_BASE_ADDRESS | dev | (where & ~3);
        write32x(addr, value);
}
#endif

static inline __attribute__((always_inline)) void pci_write_config32(device_t dev, unsigned where, uint32_t value)
{
#if CONFIG_MMCONF_SUPPORT_DEFAULT
	pci_mmio_write_config32(dev, where, value);
#else
        pci_io_write_config32(dev, where, value);
#endif
}

#define PCI_DEV_INVALID (0xffffffffU)
static inline device_t pci_io_locate_device(unsigned pci_id, device_t dev)
{
        for(; dev <= PCI_DEV(255, 31, 7); dev += PCI_DEV(0,0,1)) {
                unsigned int id;
                id = pci_io_read_config32(dev, 0);
                if (id == pci_id) {
                        return dev;
                }
        }
        return PCI_DEV_INVALID;
}

static inline device_t pci_locate_device(unsigned pci_id, device_t dev)
{
	for(; dev <= PCI_DEV(255|(((1<<CONFIG_PCI_BUS_SEGN_BITS)-1)<<8), 31, 7); dev += PCI_DEV(0,0,1)) {
		unsigned int id;
		id = pci_read_config32(dev, 0);
		if (id == pci_id) {
			return dev;
		}
	}
	return PCI_DEV_INVALID;
}

static inline device_t pci_locate_device_on_bus(unsigned pci_id, unsigned bus)
{
	device_t dev, last;

        dev = PCI_DEV(bus, 0, 0);
        last = PCI_DEV(bus, 31, 7);

        for(; dev <=last; dev += PCI_DEV(0,0,1)) {
                unsigned int id;
                id = pci_read_config32(dev, 0);
                if (id == pci_id) {
                        return dev;
                }
        }
        return PCI_DEV_INVALID;
}

/* Generic functions for pnp devices */
static inline __attribute__((always_inline)) void pnp_write_config(device_t dev, uint8_t reg, uint8_t value)
{
	unsigned port = dev >> 8;
	outb(reg, port );
	outb(value, port +1);
}

static inline __attribute__((always_inline)) uint8_t pnp_read_config(device_t dev, uint8_t reg)
{
	unsigned port = dev >> 8;
	outb(reg, port);
	return inb(port +1);
}

static inline __attribute__((always_inline)) void pnp_set_logical_device(device_t dev)
{
	unsigned device = dev & 0xff;
	pnp_write_config(dev, 0x07, device);
}

static inline __attribute__((always_inline)) void pnp_set_enable(device_t dev, int enable)
{
	pnp_write_config(dev, 0x30, enable?0x1:0x0);
}

static inline __attribute__((always_inline)) int pnp_read_enable(device_t dev)
{
	return !!pnp_read_config(dev, 0x30);
}

static inline __attribute__((always_inline)) void pnp_set_iobase(device_t dev, unsigned index, unsigned iobase)
{
	pnp_write_config(dev, index + 0, (iobase >> 8) & 0xff);
	pnp_write_config(dev, index + 1, iobase & 0xff);
}

static inline __attribute__((always_inline)) uint16_t pnp_read_iobase(device_t dev, unsigned index)
{
	return ((uint16_t)(pnp_read_config(dev, index)) << 8) | pnp_read_config(dev, index + 1);
}

static inline __attribute__((always_inline)) void pnp_set_irq(device_t dev, unsigned index, unsigned irq)
{
	pnp_write_config(dev, index, irq);
}

static inline __attribute__((always_inline)) void pnp_set_drq(device_t dev, unsigned index, unsigned drq)
{
	pnp_write_config(dev, index, drq & 0xff);
}

#endif /* ARCH_ROMCC_IO_H */
