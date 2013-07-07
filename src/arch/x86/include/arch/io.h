#ifndef _ASM_IO_H
#define _ASM_IO_H

#include <stdint.h>

/*
 * This file contains the definitions for the x86 IO instructions
 * inb/inw/inl/outb/outw/outl and the "string versions" of the same
 * (insb/insw/insl/outsb/outsw/outsl).
 */
#if defined(__ROMCC__)
static inline void outb(uint8_t value, uint16_t port)
{
	__builtin_outb(value, port);
}

static inline void outw(uint16_t value, uint16_t port)
{
	__builtin_outw(value, port);
}

static inline void outl(uint32_t value, uint16_t port)
{
	__builtin_outl(value, port);
}


static inline uint8_t inb(uint16_t port)
{
	return __builtin_inb(port);
}


static inline uint16_t inw(uint16_t port)
{
	return __builtin_inw(port);
}

static inline uint32_t inl(uint16_t port)
{
	return __builtin_inl(port);
}
#else
static inline void outb(uint8_t value, uint16_t port)
{
	__asm__ __volatile__ ("outb %b0, %w1" : : "a" (value), "Nd" (port));
}

static inline void outw(uint16_t value, uint16_t port)
{
	__asm__ __volatile__ ("outw %w0, %w1" : : "a" (value), "Nd" (port));
}

static inline void outl(uint32_t value, uint16_t port)
{
	__asm__ __volatile__ ("outl %0, %w1" : : "a" (value), "Nd" (port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t value;
	__asm__ __volatile__ ("inb %w1, %b0" : "=a"(value) : "Nd" (port));
	return value;
}

static inline uint16_t inw(uint16_t port)
{
	uint16_t value;
	__asm__ __volatile__ ("inw %w1, %w0" : "=a"(value) : "Nd" (port));
	return value;
}

static inline uint32_t inl(uint16_t port)
{
	uint32_t value;
	__asm__ __volatile__ ("inl %w1, %0" : "=a"(value) : "Nd" (port));
	return value;
}
#endif /* __ROMCC__ */

static inline void outsb(uint16_t port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsb "
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void outsw(uint16_t port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsw "
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void outsl(uint16_t port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsl "
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}


static inline void insb(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insb "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void insw(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insw "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void insl(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insl "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline __attribute__((always_inline)) uint8_t read8(unsigned long addr)
{
	return *((volatile uint8_t *)(addr));
}

static inline __attribute__((always_inline)) uint16_t read16(unsigned long addr)
{
	return *((volatile uint16_t *)(addr));
}

static inline __attribute__((always_inline)) uint32_t read32(unsigned long addr)
{
	return *((volatile uint32_t *)(addr));
}

static inline __attribute__((always_inline)) void write8(unsigned long addr, uint8_t value)
{
	*((volatile uint8_t *)(addr)) = value;
}

static inline __attribute__((always_inline)) void write16(unsigned long addr, uint16_t value)
{
	*((volatile uint16_t *)(addr)) = value;
}

static inline __attribute__((always_inline)) void write32(unsigned long addr, uint32_t value)
{
	*((volatile uint32_t *)(addr)) = value;
}

#if defined(__PRE_RAM__) || defined(__SMM__)
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

#include <arch/pci_io_cfg.h>
#include <arch/pci_mmio_cfg.h>

static inline __attribute__((always_inline)) void pci_or_config8(device_t dev, unsigned where, uint8_t value)
{
	pci_write_config8(dev, where, pci_read_config8(dev, where) | value);
}

static inline __attribute__((always_inline)) void pci_or_config16(device_t dev, unsigned where, uint16_t value)
{
	pci_write_config16(dev, where, pci_read_config16(dev, where) | value);
}

static inline __attribute__((always_inline)) void pci_or_config32(device_t dev, unsigned where, uint32_t value)
{
	pci_write_config32(dev, where, pci_read_config32(dev, where) | value);
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

#endif /* __PRE_RAM__ */

#endif

