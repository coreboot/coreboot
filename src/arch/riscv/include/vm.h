/*
 * Copyright (c) 2013, The Regents of the University of California (Regents).
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Regents nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
 * SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 * HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#ifndef _VM_H
#define _VM_H

#include <string.h>
#include <stdint.h>
#include <arch/encoding.h>

#define SUPERPAGE_SIZE ((uintptr_t)(RISCV_PGSIZE << RISCV_PGLEVEL_BITS))
#define VM_CHOICE VM_SV39
#define VA_BITS 39
#define MEGAPAGE_SIZE (SUPERPAGE_SIZE << RISCV_PGLEVEL_BITS)

#define EXTRACT_FIELD(val, which) (((val) & (which)) / ((which) & ~((which)-1)))
#define INSERT_FIELD(val, which, fieldval) (((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))

#define supervisor_paddr_valid(start, length) \
  ((uintptr_t)(start) >= current.first_user_vaddr + current.bias \
   && (uintptr_t)(start) + (length) < mem_size \
   && (uintptr_t)(start) + (length) >= (uintptr_t)(start))

typedef uintptr_t pte_t;
extern pte_t* root_page_table;

void initVirtualMemory(void);

size_t pte_ppn(pte_t pte);
pte_t ptd_create(uintptr_t ppn);
pte_t pte_create(uintptr_t ppn, int prot, int user);

void print_page_table(void);

void init_vm(uintptr_t virtMemStart, uintptr_t physMemStart,
		pte_t *pageTableStart);
void mstatus_init(void); // need to setup mstatus so we know we have virtual memory

void flush_tlb(void);


#define DEFINE_MPRV_READ(name, type, insn)				\
	static inline type name(type *p);				\
	static inline type name(type *p)				\
	{								\
		int mprv = MSTATUS_MPRV;				\
		type value;						\
		asm (							\
			"csrs		mstatus, %1\n"			\
			STRINGIFY(insn) " %0, 0(%2)\n"			\
			"csrc		mstatus, %1\n"			\
			: "=r"(value) : "r"(mprv), "r"(p) : "memory"	\
		);							\
		return value;						\
	}

#define DEFINE_MPRV_WRITE(name, type, insn)				\
	static inline void name(type *p, type value);			\
	static inline void name(type *p, type value)			\
	{								\
		int mprv = MSTATUS_MPRV;				\
		asm (							\
			"csrs		mstatus, %0\n"			\
			STRINGIFY(insn) " %1, 0(%2)\n"			\
			"csrc		mstatus, %0\n"			\
			:: "r"(mprv), "r"(value), "r"(p) : "memory"	\
		);							\
	}

DEFINE_MPRV_READ(mprv_read_u8, uint8_t, lbu)
DEFINE_MPRV_READ(mprv_read_u16, uint16_t, lhu)
DEFINE_MPRV_READ(mprv_read_u32, uint32_t, lwu)
DEFINE_MPRV_READ(mprv_read_u64, uint32_t, ld)
DEFINE_MPRV_READ(mprv_read_long, long, ld)
DEFINE_MPRV_READ(mprv_read_ulong, unsigned long, ld)
DEFINE_MPRV_WRITE(mprv_write_u8, uint8_t, sb)
DEFINE_MPRV_WRITE(mprv_write_u16, uint16_t, sh)
DEFINE_MPRV_WRITE(mprv_write_u32, uint32_t, sw)
DEFINE_MPRV_WRITE(mprv_write_u64, uint64_t, sd)
DEFINE_MPRV_WRITE(mprv_write_long, long, sd)
DEFINE_MPRV_WRITE(mprv_write_ulong, unsigned long, sd)

#undef DEFINE_MPRV_READ
#undef DEFINE_MPRV_WRITE


#endif
