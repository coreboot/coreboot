/*
 *
 * Copyright 2014 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#define EXCEPTION_STATE_ELR	0x0
#define EXCEPTION_STATE_ESR	0x8
#define EXCEPTION_STATE_SPSR	0x10
#define EXCEPTION_STATE_SP	0x18
#define EXCEPTION_STATE_REG(r)	(0x20 + r * 0x8)

#define ESR_EC_UNKNOWN		0b000000
#define ESR_EC_SVC_64		0b010101
#define ESR_EC_INSN_ABT_LOWER	0b100000
#define ESR_EC_INSN_ABT_SAME	0b100001
#define ESR_EC_DATA_ABT_LOWER	0b100100
#define ESR_EC_DATA_ABT_SAME	0b100101
#define ESR_EC_SERROR		0b101111
#define ESR_EC_SS_SAME		0b110011
#define ESR_EC_BKPT_64		0b111100

#define MDCR_TDE		(1 << 8)

#define MDSCR_SS		(1 << 0)
#define MDSCR_KDE		(1 << 13)
#define MDSCR_MDE		(1 << 15)

#ifndef __ASSEMBLER__

#include <stddef.h>
#include <stdint.h>

struct exception_state
{
	uint64_t elr;
	union {
		uint64_t esr;
		union {
			struct {
				uint64_t iss	: 25;
				uint64_t il	: 1;
				uint64_t ec	: 6;
				uint64_t _res0	: 32;
			};
			struct {
				uint64_t isfc	: 6;
				uint64_t _res0	: 1;
				uint64_t s1ptw	: 1;
				uint64_t _res1	: 1;
				uint64_t ea	: 1;
				uint64_t fnv	: 1;
				uint64_t _res2	: 53;
			} insn_abt;
		};
	};
	union {
		uint32_t spsr;
		struct {
			uint32_t sp	: 1;	/* M[0] */
			uint32_t _res0	: 1;	/* M[1] */
			uint32_t el	: 2;	/* M[3:2] */
			uint32_t arch	: 1;	/* M[4] */
			uint32_t _res1	: 1;
			uint32_t f	: 1;
			uint32_t i	: 1;
			uint32_t a	: 1;
			uint32_t d	: 1;
			uint32_t _res2	: 10;
			uint32_t il	: 1;
			uint32_t ss	: 1;
			uint32_t _res3	: 6;
			uint32_t v	: 1;
			uint32_t c	: 1;
			uint32_t z	: 1;
			uint32_t n	: 1;
		} pstate;
	};
	uint32_t spsr_high_unused;
	uint64_t sp;
	uint64_t regs[31];
} __packed;

#define CHECK_ES(field, constant) \
	_Static_assert(offsetof(struct exception_state, field) == constant, \
	"(struct exception_state)." #field " doesn't match constant " #constant)
CHECK_ES(elr, EXCEPTION_STATE_ELR);
CHECK_ES(esr, EXCEPTION_STATE_ESR);
CHECK_ES(spsr, EXCEPTION_STATE_SPSR);
CHECK_ES(sp, EXCEPTION_STATE_SP);
CHECK_ES(regs[0], EXCEPTION_STATE_REG(0));
CHECK_ES(regs[30], EXCEPTION_STATE_REG(30));

extern struct exception_state exception_state;
extern u64 exception_stack[];
extern u64 *exception_stack_end;

void exception_set_state_ptr(struct exception_state *exception_state_ptr);

enum {
	EXC_SYNC_SP0 = 0,
	EXC_IRQ_SP0,
	EXC_FIQ_SP0,
	EXC_SERROR_SP0,
	EXC_SYNC_SPX,
	EXC_IRQ_SPX,
	EXC_FIQ_SPX,
	EXC_SERROR_SPX,
	EXC_SYNC_ELX_64,
	EXC_IRQ_ELX_64,
	EXC_FIQ_ELX_64,
	EXC_SERROR_ELX_64,
	EXC_SYNC_ELX_32,
	EXC_IRQ_ELX_32,
	EXC_FIQ_ELX_32,
	EXC_SERROR_ELX_32,
	EXC_COUNT
};

#endif	/* !__ASSEMBLER__ */

#endif
