#ifndef ARCH_ROMCC_IO_H
#define ARCH_ROMCC_IO_H 1

#include <stdint.h>


static inline uint8_t read8(unsigned long addr)
{
	return *((volatile uint8_t *)(addr));
}

static inline uint16_t read16(unsigned long addr)
{
	return *((volatile uint16_t *)(addr));
}

static inline uint32_t read32(unsigned long addr)
{
	return *((volatile uint32_t *)(addr));
}

static inline void write8(unsigned long addr, uint8_t value)
{
	*((volatile uint8_t *)(addr)) = value;
}

static inline void write16(unsigned long addr, uint16_t value)
{
	*((volatile uint16_t *)(addr)) = value;
}

static inline void write32(unsigned long addr, uint32_t value)
{
	*((volatile uint32_t *)(addr)) = value;
}
#if 0
typedef __builtin_div_t div_t;
typedef __builtin_ldiv_t ldiv_t;
typedef __builtin_udiv_t udiv_t;
typedef __builtin_uldiv_t uldiv_t;

static inline div_t div(int numer, int denom)
{
	return __builtin_div(numer, denom);
}

static inline ldiv_t ldiv(long numer, long denom)
{
	return __builtin_ldiv(numer, denom);
}

static inline udiv_t udiv(unsigned numer, unsigned denom)
{
	return __builtin_udiv(numer, denom);
}

static inline uldiv_t uldiv(unsigned long numer, unsigned long denom)
{
	return __builtin_uldiv(numer, denom);
}



inline int log2(int value)
{
	/* __builtin_bsr is a exactly equivalent to the x86 machine
	 * instruction with the exception that it returns -1  
	 * when the value presented to it is zero.
	 * Otherwise __builtin_bsr returns the zero based index of
	 * the highest bit set.
	 */
	return __builtin_bsr(value);
}
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

#define PCI_ADDR(BUS, DEV, FN, WHERE) ( \
	(((BUS) & 0xFF) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN) & 0x07) << 8) | \
	((WHERE) & 0xFF))

#define PCI_DEV(BUS, DEV, FN) ( \
	(((BUS) & 0xFF) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN)  & 0x7) << 8))

#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))


#define PNP_DEV(PORT, FUNC) (((PORT) << 8) | (FUNC))

typedef unsigned device_t;

static inline __attribute__((always_inline)) uint8_t pci_read_config8(device_t dev, unsigned where)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inb(0xCFC + (addr & 3));
}

static inline __attribute__((always_inline)) uint16_t pci_read_config16(device_t dev, unsigned where)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inw(0xCFC + (addr & 2));
}

static inline __attribute__((always_inline)) uint32_t pci_read_config32(device_t dev, unsigned where)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inl(0xCFC);
}

static inline __attribute__((always_inline)) void pci_write_config8(device_t dev, unsigned where, uint8_t value)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outb(value, 0xCFC + (addr & 3));
}

static inline __attribute__((always_inline)) void pci_write_config16(device_t dev, unsigned where, uint16_t value)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outw(value, 0xCFC + (addr & 2));
}

static inline __attribute__((always_inline)) void pci_write_config32(device_t dev, unsigned where, uint32_t value)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outl(value, 0xCFC);
}

#define PCI_DEV_INVALID (0xffffffffU)
static device_t pci_locate_device(unsigned pci_id, device_t dev)
{
	for(; dev <= PCI_DEV(255, 31, 7); dev += PCI_DEV(0,0,1)) {
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
