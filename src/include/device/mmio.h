/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DEVICE_MMIO_H__
#define __DEVICE_MMIO_H__

#include <arch/mmio.h>
#include <commonlib/helpers.h>
#include <endian.h>
#include <types.h>

#define __clrsetbits_impl(bits, addr, clear, set) write##bits(addr, \
	(read##bits(addr) & ~((uint##bits##_t)(clear))) | (set))

#define clrsetbits8(addr, clear, set)	__clrsetbits_impl(8, addr, clear, set)
#define clrsetbits16(addr, clear, set)	__clrsetbits_impl(16, addr, clear, set)
#define clrsetbits32(addr, clear, set)	__clrsetbits_impl(32, addr, clear, set)
#define clrsetbits64(addr, clear, set)	__clrsetbits_impl(64, addr, clear, set)

#define setbits8(addr, set)		clrsetbits8(addr, 0, set)
#define setbits16(addr, set)		clrsetbits16(addr, 0, set)
#define setbits32(addr, set)		clrsetbits32(addr, 0, set)
#define setbits64(addr, set)		clrsetbits64(addr, 0, set)

#define clrbits8(addr, clear)		clrsetbits8(addr, clear, 0)
#define clrbits16(addr, clear)		clrsetbits16(addr, clear, 0)
#define clrbits32(addr, clear)		clrsetbits32(addr, clear, 0)
#define clrbits64(addr, clear)		clrsetbits64(addr, clear, 0)

/*
 * Reads a transfer buffer from 32-bit FIFO registers. fifo_stride is the
 * distance in bytes between registers (e.g. pass 4 for a normal array of 32-bit
 * registers or 0 to read everything from the same register). fifo_width is
 * the amount of bytes read per register (can be 1 through 4).
 */
void buffer_from_fifo32(void *buffer, size_t size, void *fifo,
			int fifo_stride, int fifo_width);

/*
 * Version of buffer_to_fifo32() that can prepend a prefix of up to fifo_width
 * size to the transfer. This is often useful for protocols where a command word
 * precedes the actual payload data. The prefix must be packed in the low-order
 * bytes of the 'prefix' u32 parameter and any high-order bytes exceeding prefsz
 * must be 0. Note that 'size' counts total bytes written, including 'prefsz'.
 */
void buffer_to_fifo32_prefix(const void *buffer, u32 prefix, int prefsz, size_t size,
			     void *fifo, int fifo_stride, int fifo_width);

/*
 * Writes a transfer buffer into 32-bit FIFO registers. fifo_stride is the
 * distance in bytes between registers (e.g. pass 4 for a normal array of 32-bit
 * registers or 0 to write everything into the same register). fifo_width is
 * the amount of bytes written per register (can be 1 through 4).
 */
static inline void buffer_to_fifo32(const void *buffer, size_t size, void *fifo,
				    int fifo_stride, int fifo_width)
{
	buffer_to_fifo32_prefix(buffer, 0, 0, size, fifo,
				fifo_stride, fifo_width);
}

/*
 * Utilities to help processing bit fields.
 *
 * To define a bit field (usually inside a register), do:
 *
 *  DEFINE_BITFIELD(name, high_bit, low_bit)
 *
 *  - name: Name of the field to access.
 *  - high_bit: highest bit that's part of the bit field.
 *  - low_bit: lowest bit in the bit field.
 *
 * To define a field with a single bit:
 *
 *  DEFINE_BIT(name, bit)
 *
 * To extract one field value from a raw reg value:
 *
 *  EXTRACT_BITFIELD(value, name);
 *
 * To read from an MMIO register and extract one field from it:
 *
 *  READ32_BITFIELD(&reg, name);
 *
 * To write into an MMIO register, set given fields (by names) to specified
 * values, and all other bits to zero (usually used for resetting a register):
 *
 *  WRITE32_BITFIELDS(&reg, name, value, [name, value, ...])
 *
 * To write into an MMIO register, set given fields (by names) to specified
 * values, and leaving all others "unchanged" (usually used for updating some
 * settings):
 *
 *  SET32_BITFIELDS(&reg, name, value, [name, value, ...])
 *
 * Examples:
 *
 *  DEFINE_BITFIELD(DISP_TYPE, 2, 1)
 *  DEFINE_BIT(DISP_EN, 0)
 *
 *  SET32_BITFIELDS(&disp_regs.ctrl, DISP_TYPE, 2);
 *  SET32_BITFIELDS(&disp_regs.ctrl, DISP_EN, 0);
 *
 *  SET32_BITFIELDS(&disp_regs.ctrl, DISP_TYPE, 1, DISP_EN, 1);
 *  WRITE32_BITFIELDS(&disp_regs.ctrl, DISP_TYPE, 1, DISP_EN, 1);
 *
 *  READ32_BITFIELD(&reg, DISP_TYPE)
 *  EXTRACT_BITFIELD(value, DISP_TYPE)
 *
 * These will be translated to:
 *
 *  clrsetbits32(&disp_regs.ctrl, 0x6, 0x4);
 *  clrsetbits32(&disp_regs.ctrl, 0x1, 0x0);
 *
 *  clrsetbits32(&disp_regs.ctrl, 0x7, 0x3);
 *  write32(&disp_regs.ctrl, 0x3);
 *
 *  (read32(&reg) & 0x6) >> 1
 *  (value & 0x6) >> 1
 *
 * The {WRITE,SET}32_BITFIELDS currently only allows setting up to 8 fields at
 * one invocation.
 */

#define DEFINE_BITFIELD(name, high_bit, low_bit) \
	_Static_assert(high_bit >= low_bit, "invalid bit field range"); \
	enum { \
		name##_BITFIELD_SHIFT = (low_bit), \
		name##_BITFIELD_SIZE = (high_bit) - (low_bit) + 1, \
	};

#define DEFINE_BIT(name, bit) DEFINE_BITFIELD(name, bit, bit)

#define _BF_MASK(name, value) \
	((u32)GENMASK(name##_BITFIELD_SHIFT + name##_BITFIELD_SIZE - 1, \
		      name##_BITFIELD_SHIFT))

#define _BF_VALUE(name, value) \
	(((u32)(value) << name##_BITFIELD_SHIFT) & _BF_MASK(name, 0))

#define _BF_APPLY1(op, name, value, ...) (op(name, value))
#define _BF_APPLY2(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY1(op, __VA_ARGS__))
#define _BF_APPLY3(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY2(op, __VA_ARGS__))
#define _BF_APPLY4(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY3(op, __VA_ARGS__))
#define _BF_APPLY5(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY4(op, __VA_ARGS__))
#define _BF_APPLY6(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY5(op, __VA_ARGS__))
#define _BF_APPLY7(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY6(op, __VA_ARGS__))
#define _BF_APPLY8(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY7(op, __VA_ARGS__))
#define _BF_APPLY9(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY8(op, __VA_ARGS__))
#define _BF_APPLY10(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY9(op, __VA_ARGS__))
#define _BF_APPLY11(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY10(op, __VA_ARGS__))
#define _BF_APPLY12(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY11(op, __VA_ARGS__))
#define _BF_APPLY13(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY12(op, __VA_ARGS__))
#define _BF_APPLY14(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY13(op, __VA_ARGS__))
#define _BF_APPLY15(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY14(op, __VA_ARGS__))
#define _BF_APPLY16(op, name, value, ...) ((op(name, value)) | \
		_BF_APPLY15(op, __VA_ARGS__))
#define _BF_APPLYINVALID(...) \
		_Static_assert(0, "Invalid arguments for {WRITE,SET}*_BITFIELDS")

#define _BF_IMPL2(op, addr, \
	n1, v1, n2, v2, n3, v3, n4, v4, n5, v5, n6, v6, n7, v7, n8, v8, \
	n9, v9, n10, v10, n11, v11, n12, v12, n13, v13, n14, v14, n15, v15, n16, v16, \
	NARGS, ...) \
	\
	op(addr, \
	   _BF_APPLY##NARGS(_BF_MASK, n1, v1, n2, v2, n3, v3, n4, v4, \
			    n5, v5, n6, v6, n7, v7, n8, v8, \
			    n9, v9, n10, v10, n11, v11, n12, v12, \
			    n13, v13, n14, v14, n15, v15, n16, v16), \
	   _BF_APPLY##NARGS(_BF_VALUE, n1, v1, n2, v2, n3, v3, n4, v4, \
			    n5, v5, n6, v6, n7, v7, n8, v8,\
			    n9, v9, n10, v10, n11, v11, n12, v12, \
			    n13, v13, n14, v14, n15, v15, n16, v16))

#define _BF_IMPL(op, addr, ...) \
	_BF_IMPL2(op, addr, __VA_ARGS__, \
		16, INVALID, 15, INVALID, 14, INVALID, 13, INVALID, \
		12, INVALID, 11, INVALID, 10, INVALID, 9, INVALID, \
		  8, INVALID, 7, INVALID, 6, INVALID, 5, INVALID, \
		  4, INVALID, 3, INVALID, 2, INVALID, 1, INVALID)

#define _WRITE32_BITFIELDS_IMPL(addr, masks, values) write32(addr, values)

#define WRITE32_BITFIELDS(addr, ...)  \
	_BF_IMPL(_WRITE32_BITFIELDS_IMPL, addr, __VA_ARGS__)

#define SET32_BITFIELDS(addr, ...) \
	_BF_IMPL(clrsetbits32, addr, __VA_ARGS__)

#define EXTRACT_BITFIELD(value, name) \
	(((value) & _BF_MASK(name, 0)) >> name##_BITFIELD_SHIFT)

#define READ32_BITFIELD(addr, name) \
	EXTRACT_BITFIELD(read32(addr), name)

static __always_inline uint8_t read8p(const uintptr_t addr)
{
	return read8((void *)addr);
}

static __always_inline uint16_t read16p(const uintptr_t addr)
{
	return read16((void *)addr);
}

static __always_inline uint32_t read32p(const uintptr_t addr)
{
	return read32((void *)addr);
}

static __always_inline uint64_t read64p(const uintptr_t addr)
{
	return read64((void *)addr);
}

static __always_inline void write8p(const uintptr_t addr, const uint8_t value)
{
	write8((void *)addr, value);
}

static __always_inline void write16p(const uintptr_t addr, const uint16_t value)
{
	write16((void *)addr, value);
}

static __always_inline void write32p(const uintptr_t addr, const uint32_t value)
{
	write32((void *)addr, value);
}

static __always_inline void write64p(const uintptr_t addr, const uint64_t value)
{
	write64((void *)addr, value);
}

#endif	/* __DEVICE_MMIO_H__ */
