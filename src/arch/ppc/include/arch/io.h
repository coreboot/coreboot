/*
 * BK Id: SCCS/s.io.h 1.14 10/16/01 15:58:42 trini
 */
#ifndef _PPC_IO_H
#define _PPC_IO_H

#include <stdint.h>

#define SIO_CONFIG_RA	0x398
#define SIO_CONFIG_RD	0x399

#define SLOW_DOWN_IO

#ifndef CONFIG_IO_BASE
#define CONFIG_IO_BASE	0
#endif

#define readb(addr) in_8((volatile uint8_t *)(addr))
#define writeb(b,addr) out_8((volatile uint8_t *)(addr), (b))
#define readw(addr) in_le16((volatile uint16_t *)(addr))
#define readl(addr) in_le32((volatile uint32_t *)(addr))
#define writew(b,addr) out_le16((volatile uint16_t *)(addr),(b))
#define writel(b,addr) out_le32((volatile uint32_t *)(addr),(b))


#define __raw_readb(addr)	(*(volatile unsigned char *)(addr))
#define __raw_readw(addr)	(*(volatile unsigned short *)(addr))
#define __raw_readl(addr)	(*(volatile unsigned int *)(addr))
#define __raw_writeb(v, addr)	(*(volatile unsigned char *)(addr) = (v))
#define __raw_writew(v, addr)	(*(volatile unsigned short *)(addr) = (v))
#define __raw_writel(v, addr)	(*(volatile unsigned int *)(addr) = (v))

/*
 * The insw/outsw/insl/outsl macros don't do byte-swapping.
 * They are only used in practice for transferring buffers which
 * are arrays of bytes, and byte-swapping is not appropriate in
 * that case.  - paulus
 */
#define insw(port, buf, ns)	_insw_ns((uint16_t *)((port)+CONFIG_IO_BASE), (buf), (ns))
#define outsw(port, buf, ns)	_outsw_ns((uint16_t *)((port)+CONFIG_IO_BASE), (buf), (ns))

#define inb(port)		in_8((uint8_t *)((port)+CONFIG_IO_BASE))
#define outb(val, port)		out_8((uint8_t *)((port)+CONFIG_IO_BASE), (val))
#define inw(port)		in_le16((uint16_t *)((port)+CONFIG_IO_BASE))
#define outw(val, port)		out_le16((uint16_t *)((port)+CONFIG_IO_BASE), (val))
#define inl(port)		in_le32((uint32_t *)((port)+CONFIG_IO_BASE))
#define outl(val, port)		out_le32((uint32_t *)((port)+CONFIG_IO_BASE), (val))

#define inb_p(port)		inb((port))
#define outb_p(val, port)	outb((val), (port))
#define inw_p(port)		inw((port))
#define outw_p(val, port)	outw((val), (port))
#define inl_p(port)		inl((port))
#define outl_p(val, port)	outl((val), (port))

/*
 * The *_ns versions below do byte-swapping.
 */
#define insw_ns(port, buf, ns)	_insw((uint16_t *)((port)+CONFIG_IO_BASE), (buf), (ns))
#define outsw_ns(port, buf, ns)	_outsw((uint16_t *)((port)+CONFIG_IO_BASE), (buf), (ns))


#define IO_SPACE_LIMIT ~0

#define memset_io(a,b,c)       memset((void *)(a),(b),(c))
#define memcpy_fromio(a,b,c)   memcpy((a),(void *)(b),(c))
#define memcpy_toio(a,b,c)	memcpy((void *)(a),(b),(c))

/*
 * Enforce In-order Execution of I/O:
 * Acts as a barrier to ensure all previous I/O accesses have
 * completed before any further ones are issued.
 */
static inline void eieio(void)
{
	__asm__ __volatile__ ("eieio" : : : "memory");
}

/* Enforce in-order execution of data I/O. 
 * No distinction between read/write on PPC; use eieio for all three.
 */
#define iobarrier_rw() eieio()
#define iobarrier_r()  eieio()
#define iobarrier_w()  eieio()

/*
 * 8, 16 and 32 bit, big and little endian I/O operations, with barrier.
 */
static inline int in_8(volatile unsigned char *addr)
{
	int ret;

	__asm__ __volatile__("lbz%U1%X1 %0,%1; eieio" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_8(volatile unsigned char *addr, int val)
{
	__asm__ __volatile__("stb%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}

static inline int in_le16(volatile unsigned short *addr)
{
	int ret;

	__asm__ __volatile__("lhbrx %0,0,%1; eieio" : "=r" (ret) :
			      "r" (addr), "m" (*addr));
	return ret;
}

static inline int in_be16(volatile unsigned short *addr)
{
	int ret;

	__asm__ __volatile__("lhz%U1%X1 %0,%1; eieio" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_le16(volatile unsigned short *addr, int val)
{
	__asm__ __volatile__("sthbrx %1,0,%2; eieio" : "=m" (*addr) :
			      "r" (val), "r" (addr));
}

static inline void out_be16(volatile unsigned short *addr, int val)
{
	__asm__ __volatile__("sth%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}

static inline unsigned in_le32(volatile unsigned *addr)
{
	unsigned ret;

	__asm__ __volatile__("lwbrx %0,0,%1; eieio" : "=r" (ret) :
			     "r" (addr), "m" (*addr));
	return ret;
}

static inline unsigned in_be32(volatile unsigned *addr)
{
	unsigned ret;

	__asm__ __volatile__("lwz%U1%X1 %0,%1; eieio" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_le32(volatile unsigned *addr, int val)
{
	__asm__ __volatile__("stwbrx %1,0,%2; eieio" : "=m" (*addr) :
			     "r" (val), "r" (addr));
}

static inline void out_be32(volatile unsigned *addr, int val)
{
	__asm__ __volatile__("stw%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}

static inline void _insw_ns(volatile uint16_t *port, void *buf, int ns)
{
	uint16_t *   b = (uint16_t *)buf;    

	while (ns > 0) {    
		*b++ = in_le16(port);
		ns--;
	}
}

static inline void _outsw_ns(volatile uint16_t *port, const void *buf, int ns)
{
	uint16_t *   b = (uint16_t *)buf;    

	while (ns > 0) {    
		out_le16(port, *b++);
		ns--;
	}
}

static inline void _insw(volatile uint16_t *port, void *buf, int ns)
{
	uint16_t *   b = (uint16_t *)buf;    

	while (ns > 0) {    
		*b++ = in_be16(port);
		ns--;
	}
}

static inline void _outsw(volatile uint16_t *port, const void *buf, int ns)
{
	uint16_t *   b = (uint16_t *)buf;    

	while (ns > 0) {    
		out_be16(port, *b++);
		ns--;
	}
}
#endif
