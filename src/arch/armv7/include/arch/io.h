/*
 * Originally imported from linux/include/asm-arm/io.h. This file has changed
 * substantially since then.
 *
 *  Copyright (C) 2013 Google Inc.
 *  Copyright (C) 1996-2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Modifications:
 *  08-Apr-2013	G	Replaced several macros with inlines for type safety.
 *  16-Sep-1996	RMK	Inlined the inx/outx functions & optimised for both
 *			constant addresses and variable addresses.
 *  04-Dec-1997	RMK	Moved a lot of this stuff to the new architecture
 *			specific IO header files.
 *  27-Mar-1999	PJB	Second parameter of memcpy_toio is const..
 *  04-Apr-1999	PJB	Added check_signature.
 *  12-Dec-1999	RMK	More cleanups
 *  18-Jun-2000 RMK	Removed virt_to_* and friends definitions
 */
#ifndef __ASM_ARM_IO_H
#define __ASM_ARM_IO_H

#include <types.h>
#include <arch/cache.h>		/* for dmb() */
#include <arch/byteorder.h>

static inline uint8_t read8(const void *addr)
{
	uint8_t v = *(volatile uint8_t *)addr;
	dmb();
	return v;
}

static inline uint16_t read16(const void *addr)
{
	uint16_t v = *(volatile uint16_t *)addr;
	dmb();
	return v;
}

static inline uint32_t read32(const void *addr)
{
	uint32_t v = *(volatile uint32_t *)addr;
	dmb();
	return v;
}

static inline void write8(uint8_t val, const void *addr)
{
	dmb();
	*(volatile uint8_t *)addr = val;
}

static inline void write16(uint16_t val, const void *addr)
{
	dmb();
	*(volatile uint16_t *)addr = val;
}

static inline void write32(uint32_t val, const void *addr)
{
	dmb();
	*(volatile uint32_t *)addr = val;
}

/*
 * FIXME: These are to avoid breaking existing ARM code. We should eventually
 * re-factor all code to specify the data length intended.
  */
#define readb(a)	read8(a)
#define writeb(v,a)	write8(v,a)
#define readl(a)	read32(a)
#define writel(v,a)	write32(v,a)

/*
 * Clear and set bits in one shot. These macros can be used to clear and
 * set multiple bits in a register using a single call. These macros can
 * also be used to set a multiple-bit bit pattern using a mask, by
 * specifying the mask in the 'clear' parameter and the new bit pattern
 * in the 'set' parameter.
 */

#define out_arch(type,endian,a,v)	write##type(cpu_to_##endian(v),a)
#define in_arch(type,endian,a)		endian##_to_cpu(read##type(a))

#define out_le32(a,v)	out_arch(l,le32,a,v)
#define out_le16(a,v)	out_arch(w,le16,a,v)

#define in_le32(a)	in_arch(l,le32,a)
#define in_le16(a)	in_arch(w,le16,a)

#define out_be32(a,v)	out_arch(l,be32,a,v)
#define out_be16(a,v)	out_arch(w,be16,a,v)

#define in_be32(a)	in_arch(l,be32,a)
#define in_be16(a)	in_arch(w,be16,a)

#define out_8(a,v)	writeb(v,a)
#define in_8(a)		readb(a)

#define clrbits(type, addr, clear) \
	out_##type((addr), in_##type(addr) & ~(clear))

#define setbits(type, addr, set) \
	out_##type((addr), in_##type(addr) | (set))

#define clrsetbits(type, addr, clear, set) \
	out_##type((addr), (in_##type(addr) & ~(clear)) | (set))

#define clrbits_be32(addr, clear) clrbits(be32, addr, clear)
#define setbits_be32(addr, set) setbits(be32, addr, set)
#define clrsetbits_be32(addr, clear, set) clrsetbits(be32, addr, clear, set)

#define clrbits_le32(addr, clear) clrbits(le32, addr, clear)
#define setbits_le32(addr, set) setbits(le32, addr, set)
#define clrsetbits_le32(addr, clear, set) clrsetbits(le32, addr, clear, set)

#define clrbits_be16(addr, clear) clrbits(be16, addr, clear)
#define setbits_be16(addr, set) setbits(be16, addr, set)
#define clrsetbits_be16(addr, clear, set) clrsetbits(be16, addr, clear, set)

#define clrbits_le16(addr, clear) clrbits(le16, addr, clear)
#define setbits_le16(addr, set) setbits(le16, addr, set)
#define clrsetbits_le16(addr, clear, set) clrsetbits(le16, addr, clear, set)

#define clrbits_8(addr, clear) clrbits(8, addr, clear)
#define setbits_8(addr, set) setbits(8, addr, set)
#define clrsetbits_8(addr, clear, set) clrsetbits(8, addr, clear, set)

/*
 *  IO port access primitives
 *  -------------------------
 *
 * The ARM doesn't have special IO access instructions; all IO is memory
 * mapped.  Note that these are defined to perform little endian accesses
 * only.  Their primary purpose is to access PCI and ISA peripherals.
 *
 * Note that for a big endian machine, this implies that the following
 * big endian mode connectivity is in place, as described by numerous
 * ARM documents:
 *
 *    PCI:  D0-D7   D8-D15 D16-D23 D24-D31
 *    ARM: D24-D31 D16-D23  D8-D15  D0-D7
 *
 * The machine specific io.h include defines __io to translate an "IO"
 * address to a memory address.
 *
 * Note that we prevent GCC re-ordering or caching values in expressions
 * by introducing sequence points into the in*() definitions.
 *
 * The {in,out}[bwl] macros are for emulating x86-style PCI/ISA IO space.
 */
#ifdef __io
#define outb(v,p)			writeb(v,__io(p))
#define outw(v,p)			writew(cpu_to_le16(v),__io(p))
#define outl(v,p)			writel(cpu_to_le32(v),__io(p))

#define inb(p)	({ unsigned int __v = readb(__io(p)); __v; })
#define inw(p)	({ unsigned int __v = le16_to_cpu(readw(__io(p))); __v; })
#define inl(p)	({ unsigned int __v = le32_to_cpu(readl(__io(p))); __v; })

#define outsb(p,d,l)			writesb(__io(p),d,l)
#define outsw(p,d,l)			writesw(__io(p),d,l)
#define outsl(p,d,l)			writesl(__io(p),d,l)

#define insb(p,d,l)			readsb(__io(p),d,l)
#define insw(p,d,l)			readsw(__io(p),d,l)
#define insl(p,d,l)			readsl(__io(p),d,l)
#endif

#define outb_p(val,port)		outb((val),(port))
#define outw_p(val,port)		outw((val),(port))
#define outl_p(val,port)		outl((val),(port))
#define inb_p(port)			inb((port))
#define inw_p(port)			inw((port))
#define inl_p(port)			inl((port))

#define outsb_p(port,from,len)		outsb(port,from,len)
#define outsw_p(port,from,len)		outsw(port,from,len)
#define outsl_p(port,from,len)		outsl(port,from,len)
#define insb_p(port,to,len)		insb(port,to,len)
#define insw_p(port,to,len)		insw(port,to,len)
#define insl_p(port,to,len)		insl(port,to,len)

/*
 * ioremap and friends.
 *
 * ioremap takes a PCI memory address, as specified in
 * linux/Documentation/IO-mapping.txt.  If you want a
 * physical address, use __ioremap instead.
 */
extern void * __ioremap(unsigned long offset, size_t size, unsigned long flags);
extern void __iounmap(void *addr);

/*
 * Generic ioremap support.
 *
 * Define:
 *  iomem_valid_addr(off,size)
 *  iomem_to_phys(off)
 */
#ifdef iomem_valid_addr
#define __arch_ioremap(off,sz,nocache)					\
 ({									\
	unsigned long _off = (off), _size = (sz);			\
	void *_ret = (void *)0;						\
	if (iomem_valid_addr(_off, _size))				\
		_ret = __ioremap(iomem_to_phys(_off),_size,nocache);	\
	_ret;								\
 })

#define __arch_iounmap __iounmap
#endif

/*
 * String version of IO memory access ops:
 */
extern void _memcpy_fromio(void *, unsigned long, size_t);
extern void _memcpy_toio(unsigned long, const void *, size_t);
extern void _memset_io(unsigned long, int, size_t);

extern void __readwrite_bug(const char *fn);

/*
 * If this architecture has PCI memory IO, then define the read/write
 * macros.  These should only be used with the cookie passed from
 * ioremap.
 */
#ifdef __mem_pci

#define readb(c) ({ unsigned int __v = readb(__mem_pci(c)); __v; })
#define readw(c) ({ unsigned int __v = le16_to_cpu(readw(__mem_pci(c))); __v; })
#define readl(c) ({ unsigned int __v = le32_to_cpu(readl(__mem_pci(c))); __v; })

#define writeb(v,c)		writeb(v,__mem_pci(c))
#define writew(v,c)		writew(cpu_to_le16(v),__mem_pci(c))
#define writel(v,c)		writel(cpu_to_le32(v),__mem_pci(c))

#define memset_io(c,v,l)		_memset_io(__mem_pci(c),(v),(l))
#define memcpy_fromio(a,c,l)		_memcpy_fromio((a),__mem_pci(c),(l))
#define memcpy_toio(c,a,l)		_memcpy_toio(__mem_pci(c),(a),(l))

#define eth_io_copy_and_sum(s,c,l,b) \
				eth_copy_and_sum((s),__mem_pci(c),(l),(b))

static inline int
check_signature(unsigned long io_addr, const unsigned char *signature,
		int length)
{
	int retval = 0;
	do {
		if (readb(io_addr) != *signature)
			goto out;
		io_addr++;
		signature++;
		length--;
	} while (length);
	retval = 1;
out:
	return retval;
}
#endif	/* __mem_pci */

/*
 * If this architecture has ISA IO, then define the isa_read/isa_write
 * macros.
 */
#ifdef __mem_isa

#define isa_readb(addr)			readb(__mem_isa(addr))
#define isa_readw(addr)			readw(__mem_isa(addr))
#define isa_readl(addr)			readl(__mem_isa(addr))
#define isa_writeb(val,addr)		writeb(val,__mem_isa(addr))
#define isa_writew(val,addr)		writew(val,__mem_isa(addr))
#define isa_writel(val,addr)		writel(val,__mem_isa(addr))
#define isa_memset_io(a,b,c)		_memset_io(__mem_isa(a),(b),(c))
#define isa_memcpy_fromio(a,b,c)	_memcpy_fromio((a),__mem_isa(b),(c))
#define isa_memcpy_toio(a,b,c)		_memcpy_toio(__mem_isa((a)),(b),(c))

#define isa_eth_io_copy_and_sum(a,b,c,d) \
				eth_copy_and_sum((a),__mem_isa(b),(c),(d))

static inline int
isa_check_signature(unsigned long io_addr, const unsigned char *signature,
		    int length)
{
	int retval = 0;
	do {
		if (isa_readb(io_addr) != *signature)
			goto out;
		io_addr++;
		signature++;
		length--;
	} while (length);
	retval = 1;
out:
	return retval;
}

#else	/* __mem_isa */

#define isa_readb(addr)			(__readwrite_bug("isa_readb"),0)
#define isa_readw(addr)			(__readwrite_bug("isa_readw"),0)
#define isa_readl(addr)			(__readwrite_bug("isa_readl"),0)
#define isa_writeb(val,addr)		__readwrite_bug("isa_writeb")
#define isa_writew(val,addr)		__readwrite_bug("isa_writew")
#define isa_writel(val,addr)		__readwrite_bug("isa_writel")
#define isa_memset_io(a,b,c)		__readwrite_bug("isa_memset_io")
#define isa_memcpy_fromio(a,b,c)	__readwrite_bug("isa_memcpy_fromio")
#define isa_memcpy_toio(a,b,c)		__readwrite_bug("isa_memcpy_toio")

#define isa_eth_io_copy_and_sum(a,b,c,d) \
				__readwrite_bug("isa_eth_io_copy_and_sum")

#define isa_check_signature(io,sig,len)	(0)

#endif	/* __mem_isa */
#endif	/* __ASM_ARM_IO_H */
