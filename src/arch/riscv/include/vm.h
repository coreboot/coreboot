/* SPDX-License-Identifier: BSD-4-Clause-UC */

#ifndef _VM_H
#define _VM_H

#include <stdint.h>
#include <arch/encoding.h>

#define EXTRACT_FIELD(val, which) (((val) & (which)) / ((which) & ~((which)-1)))
#define INSERT_FIELD(val, which, fieldval) (((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))

void mstatus_init(void); // need to setup mstatus so we know we have virtual memory

#define DEFINE_MPRV_READ_FLAGS(name, type, insn, flags)			\
	static inline type name(type *p);				\
	static inline type name(type *p)				\
	{								\
		size_t mprv = flags;		                        \
		type value;						\
		asm (							\
			"csrs		mstatus, %1\n"			\
			STRINGIFY(insn) " %0, 0(%2)\n"			\
			"csrc		mstatus, %1\n"			\
			: "=&r"(value) : "r"(mprv), "r"(p) : "memory"	\
		);							\
		return value;						\
	}

#define DEFINE_MPRV_READ(name, type, insn) \
	DEFINE_MPRV_READ_FLAGS(name, type, insn, MSTATUS_MPRV)

#define DEFINE_MPRV_READ_MXR(name, type, insn) \
	DEFINE_MPRV_READ_FLAGS(name, type, insn, MSTATUS_MPRV | MSTATUS_MXR)

#define DEFINE_MPRV_WRITE(name, type, insn)				\
	static inline void name(type *p, type value);			\
	static inline void name(type *p, type value)			\
	{								\
		size_t mprv = MSTATUS_MPRV;				\
		asm (							\
			"csrs		mstatus, %0\n"			\
			STRINGIFY(insn) " %1, 0(%2)\n"			\
			"csrc		mstatus, %0\n"			\
			:: "r"(mprv), "r"(value), "r"(p) : "memory"	\
		);							\
	}

/*
 * mprv_{read,write}_* - Modified privilege memory access functions.
 *
 * These inline functions perform a read or write memory operation with the
 * mstatus.MPRV bit set. This causes the memory protections and translation of
 * the previous mode (e.g. U-mode, if we're handling a trap from U-mode) to be
 * applied.
 *
 * The user of these functions must make sure to avoid trap loops through
 * unaligned memory accesses.
 */
DEFINE_MPRV_READ(mprv_read_u8, uint8_t, lbu)
DEFINE_MPRV_READ(mprv_read_u16, uint16_t, lhu)
DEFINE_MPRV_READ(mprv_read_u32, uint32_t, lwu)
DEFINE_MPRV_READ(mprv_read_u64, uint64_t, ld)
DEFINE_MPRV_READ(mprv_read_long, long, ld)
DEFINE_MPRV_READ(mprv_read_ulong, unsigned long, ld)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_u8, uint8_t, lbu)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_u16, uint16_t, lhu)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_u32, uint32_t, lwu)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_u64, uint64_t, ld)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_long, long, ld)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_ulong, unsigned long, ld)
DEFINE_MPRV_WRITE(mprv_write_u8, uint8_t, sb)
DEFINE_MPRV_WRITE(mprv_write_u16, uint16_t, sh)
DEFINE_MPRV_WRITE(mprv_write_u32, uint32_t, sw)
DEFINE_MPRV_WRITE(mprv_write_u64, uint64_t, sd)
DEFINE_MPRV_WRITE(mprv_write_long, long, sd)
DEFINE_MPRV_WRITE(mprv_write_ulong, unsigned long, sd)

#if __riscv_xlen == 32
	DEFINE_MPRV_READ(mprv_read_uintptr_t, uintptr_t, lw)
	DEFINE_MPRV_READ(mprv_write_uintptr_t, uintptr_t, sw)
#elif __riscv_xlen == 64
	DEFINE_MPRV_READ(mprv_read_uintptr_t, uintptr_t, ld)
	DEFINE_MPRV_READ(mprv_write_uintptr_t, uintptr_t, sd)
#endif

#undef DEFINE_MPRV_READ_FLAGS
#undef DEFINE_MPRV_READ
#undef DEFINE_MPRV_READ_MXR
#undef DEFINE_MPRV_WRITE

#endif
