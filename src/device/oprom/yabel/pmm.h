/****************************************************************************
 * YABEL BIOS Emulator
 *
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Copyright (c) 2008 Pattrick Hueper <phueper@hueper.net>
 ****************************************************************************/

#ifndef _YABEL_PMM_H_
#define _YABEL_PMM_H_

#include <types.h>

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
} __attribute__ ((__packed__)) pmm_information_t;

/* This function is used to setup the PMM struct in virtual memory
 * at a certain offset */
u8 pmm_setup(u16 segment, u16 offset);

/* This is the INT Handler mentioned above, called by my special PMM INT. */
void pmm_handleInt(void);

void pmm_test(void);

#endif				// _YABEL_PMM_H
