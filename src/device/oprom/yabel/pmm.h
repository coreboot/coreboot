/****************************************************************************
 * YABEL BIOS Emulator
 *
 * Copyright (c) 2008 Pattrick Hueper <phueper@hueper.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef _YABEL_PMM_H_
#define _YABEL_PMM_H_

#include <types.h>
#include <compiler.h>

/* PMM Structure see PMM Spec Version 1.01 Chapter 3.1.1
 * (search web for specspmm101.pdf)
 */
typedef struct {
	u8 signature[4];
	u8 struct_rev;
	u8 length;
	u8 checksum;
	u32 entry_point_offset;
	u8 reserved[5];
	/* Code is not part of the specced PMM struct, however, since I cannot
	 * put the handling of PMM in the virtual memory (I don't want to hack
	 * it together in x86 assembly ;-)) this code array is pointed to by
	 * entry_point_offset, in code there is only a INT call and a RETF,
	 * thus every PMM call will issue a PMM INT (only defined in YABEL,
	 * see interrupt.c) and the INT Handler will do the actual PMM work.
	 */
	u8 code[3];
} __packed pmm_information_t;

/* This function is used to setup the PMM struct in virtual memory
 * at a certain offset */
u8 pmm_setup(u16 segment, u16 offset);

/* This is the INT Handler mentioned above, called by my special PMM INT. */
void pmm_handleInt(void);

void pmm_test(void);

#endif /* _YABEL_PMM_H_ */
