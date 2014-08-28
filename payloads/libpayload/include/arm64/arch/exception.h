/*
 * This file is part of the libpayload project.
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

#include <stdint.h>

void set_vbar(void* vbar);

struct exception_state
{
	uint64_t elr;
	uint64_t esr;
	uint64_t regs[31];
} __attribute__((packed));

extern struct exception_state *exception_state;

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

#endif
