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

#include <x86emu/x86emu.h>
#include "../x86emu/prim_ops.h"
#include <string.h>

#include "biosemu.h"
#include "pmm.h"
#include "debug.h"
#include "device.h"

/* this struct is used to remember which PMM spaces
 * have been assigned. MAX_PMM_AREAS defines how many
 * PMM areas we can assign.
 * All areas are assigned in PMM_CONV_SEGMENT
 */
typedef struct {
	u32 handle;		/* handle that is returned to PMM caller */
	u32 offset;		/* in PMM_CONV_SEGMENT */
	u32 length;		/* length of this area */
} pmm_allocation_t;

#define MAX_PMM_AREAS 10

/* array to store the above structs */
static pmm_allocation_t pmm_allocation_array[MAX_PMM_AREAS];

/* index into pmm_allocation_array */
static u32 curr_pmm_allocation_index = 0;

/* This function is used to setup the PMM struct in virtual memory
 * at a certain offset, the length of the PMM struct is returned */
u8 pmm_setup(u16 segment, u16 offset)
{
	/* setup the PMM structure */
	pmm_information_t *pis =
	    (pmm_information_t *) (M.mem_base + (((u32) segment) << 4) +
				   offset);
	memset(pis, 0, sizeof(pmm_information_t));
	/* set signature to $PMM */
	pis->signature[0] = '$';
	pis->signature[1] = 'P';
	pis->signature[2] = 'M';
	pis->signature[3] = 'M';
	/* revision as specified */
	pis->struct_rev = 0x01;
	/* internal length, excluding code */
	pis->length = ((void *)&(pis->code) - (void *)&(pis->signature));
	/* the code to be executed, pointed to by entry_point_offset */
	pis->code[0] = 0xCD;	/* INT */
	pis->code[1] = PMM_INT_NUM;	/* my selfdefined PMM INT number */
	pis->code[2] = 0xCB;	/* RETF */
	/* set the entry_point_offset, it should point to pis->code, segment is the segment of
	 * this struct. Since pis->length is the length of the struct excluding code, offset+pis->length
	 * points to the code... it's that simple ;-)
	 */
	out32le(&(pis->entry_point_offset),
		(u32) segment << 16 | (u32) (offset + pis->length));
	/* checksum calculation */
	u8 i;
	u8 checksum = 0;
	for (i = 0; i < pis->length; i++) {
		checksum += *(((u8 *) pis) + i);
	}
	pis->checksum = ((u8) 0) - checksum;
	CHECK_DBG(DEBUG_PMM) {
		DEBUG_PRINTF_PMM("PMM Structure:\n");
		dump((void *)pis, sizeof(pmm_information_t));
	}
	return sizeof(pmm_information_t);
}

/* handle the selfdefined interrupt, this is executed, when the PMM Entry Point
 * is executed, it must handle all PMM requests
 */
void pmm_handleInt()
{
	u32 rval = 0;
	u16 function, flags;
	u32 handle, length;
	u32 i, j;
	u32 buffer;
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 * according to the PMM Spec "the flags and all registers, except DX and AX
	 * are preserved across calls to PMM"
	 * so we save M.x86 and in :exit label we restore it, however, this means that no
	 * returns must be used in this function, any exit must use goto exit!
	 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 */
	X86EMU_regs backup_regs = M.x86;
	pop_long();		/* pop the return address, this is already saved in INT handler, we don't need
				   to remember this. */
	function = pop_word();
	switch (function) {
	case 0:
		/* function pmmAllocate */
		length = pop_long();
		length *= 16;	/* length is passed in "paragraphs" of 16 bytes each */
		handle = pop_long();
		flags = pop_word();
		DEBUG_PRINTF_PMM
		    ("%s: pmmAllocate: Length: %x, Handle: %x, Flags: %x\n",
		     __func__, length, handle, flags);
		if ((flags & 0x1) != 0) {
			/* request to allocate in  conventional memory */
			if (curr_pmm_allocation_index >= MAX_PMM_AREAS) {
				printf
				    ("%s: pmmAllocate: Maximum Number of allocatable areas reached (%d), cannot allocate more memory!\n",
				     __func__, MAX_PMM_AREAS);
				rval = 0;
				goto exit;
			}
			/* some ROMs seem to be confused by offset 0, so lets start at 0x100 */
			u32 next_offset = 0x100;
			pmm_allocation_t *pmm_alloc =
			    &(pmm_allocation_array[curr_pmm_allocation_index]);
			if (curr_pmm_allocation_index != 0) {
				/* we have already allocated... get the new next_offset
				 * from the previous pmm_allocation_t */
				next_offset =
				    pmm_allocation_array
				    [curr_pmm_allocation_index - 1].offset +
				    pmm_allocation_array
				    [curr_pmm_allocation_index - 1].length;
			}
			DEBUG_PRINTF_PMM("%s: next_offset: 0x%x\n",
					 __func__, next_offset);
			if (length == 0) {
				/* largest possible block size requested, we have on segment
				 * to allocate, so largest possible is segment size (0xFFFF)
				 * minus next_offset
				 */
				rval = 0xFFFF - next_offset;
				goto exit;
			}
			u32 align = 0;
			if (((flags & 0x4) != 0) && (length > 0)) {
				/* align to least significant bit set in length param */
				u8 lsb = 0;
				while (((length >> lsb) & 0x1) == 0) {
					lsb++;
				}
				align = 1 << lsb;
			}
			/* always align at least to paragraph (16byte) boundary
			 * hm... since the length is always in paragraphs, we cannot
			 * align outside of paragraphs anyway... so this check might
			 * be unnecessary...*/
			if (align < 0x10) {
				align = 0x10;
			}
			DEBUG_PRINTF_PMM("%s: align: 0x%x\n", __func__,
					 align);
			if ((next_offset & (align - 1)) != 0) {
				/* not yet aligned... align! */
				next_offset += align;
				next_offset &= ~(align - 1);
			}
			if ((next_offset + length) > 0xFFFF) {
				rval = 0;
				printf
				    ("%s: pmmAllocate: Not enough memory available for allocation!\n",
				     __func__);
				goto exit;
			}
			curr_pmm_allocation_index++;
			/* remember the values in pmm_allocation_array */
			pmm_alloc->handle = handle;
			pmm_alloc->offset = next_offset;
			pmm_alloc->length = length;
			/* return the 32bit "physical" address, i.e. combination of segment and offset */
			rval = ((u32) (PMM_CONV_SEGMENT << 16)) | next_offset;
			DEBUG_PRINTF_PMM
			    ("%s: pmmAllocate: allocated memory at %x\n",
			     __func__, rval);
		} else {
			rval = 0;
			printf
			    ("%s: pmmAllocate: allocation in extended memory not supported!\n",
			     __func__);
		}
		goto exit;
	case 1:
		/* function pmmFind */
		handle = pop_long();	/* the handle to lookup */
		DEBUG_PRINTF_PMM("%s: pmmFind: Handle: %x\n", __func__,
				 handle);
		i = 0;
		for (i = 0; i < curr_pmm_allocation_index; i++) {
			if (pmm_allocation_array[i].handle == handle) {
				DEBUG_PRINTF_PMM
				    ("%s: pmmFind: found allocated memory at %x\n",
				     __func__, rval);
				/* return the 32bit "physical" address, i.e. combination of segment and offset */
				rval =
				    ((u32) (PMM_CONV_SEGMENT << 16)) |
				    pmm_allocation_array[i].offset;
			}
		}
		if (rval == 0) {
			DEBUG_PRINTF_PMM
			    ("%s: pmmFind: handle (%x) not found!\n",
			     __func__, handle);
		}
		goto exit;
	case 2:
		/* function pmmDeallocate */
		buffer = pop_long();
		/* since argument is the address of the PMM block (including the segment,
		 * we need to remove the segment to get the offset
		 */
		buffer = buffer ^ ((u32) PMM_CONV_SEGMENT << 16);
		DEBUG_PRINTF_PMM("%s: pmmDeallocate: PMM segment offset: %x\n",
				 __func__, buffer);
		i = 0;
		/* rval = 0 means we deallocated the buffer, so set it to 1 in case we don't find it and
		 * thus cannot deallocate
		 */
		rval = 1;
		for (i = 0; i < curr_pmm_allocation_index; i++) {
			DEBUG_PRINTF_PMM("%d: %x\n", i,
					 pmm_allocation_array[i].handle);
			if (pmm_allocation_array[i].offset == buffer) {
				/* we found the requested buffer, rval = 0 */
				rval = 0;
				DEBUG_PRINTF_PMM
				    ("%s: pmmDeallocate: found allocated memory at index: %d\n",
				     __func__, i);
				/* copy the remaining elements in pmm_allocation_array one position up */
				j = i;
				for (; j < curr_pmm_allocation_index; j++) {
					pmm_allocation_array[j] =
					    pmm_allocation_array[j + 1];
				}
				/* move curr_pmm_allocation_index one up, too */
				curr_pmm_allocation_index--;
				/* finally clean last element */
				pmm_allocation_array[curr_pmm_allocation_index].
				    handle = 0;
				pmm_allocation_array[curr_pmm_allocation_index].
				    offset = 0;
				pmm_allocation_array[curr_pmm_allocation_index].
				    length = 0;
				break;
			}
		}
		if (rval != 0) {
			DEBUG_PRINTF_PMM
			    ("%s: pmmDeallocate: offset (%x) not found, cannot deallocate!\n",
			     __func__, buffer);
		}
		goto exit;
	default:
		/* invalid/unimplemented function */
		printf("%s: invalid PMM function (0x%04x) called!\n",
		       __func__, function);
		/* PMM spec says if function is invalid, return 0xFFFFFFFF */
		rval = 0xFFFFFFFF;
		goto exit;
	}
exit:
	/* exit handler of this function, restore registers, put return value in DX:AX */
	M.x86 = backup_regs;
	M.x86.R_DX = (u16) ((rval >> 16) & 0xFFFF);
	M.x86.R_AX = (u16) (rval & 0xFFFF);
	CHECK_DBG(DEBUG_PMM) {
		DEBUG_PRINTF_PMM("%s: dump of pmm_allocation_array:\n",
				 __func__);
		for (i = 0; i < MAX_PMM_AREAS; i++) {
			DEBUG_PRINTF_PMM
			    ("%d:\n\thandle: %x\n\toffset: %x\n\tlength: %x\n",
			     i, pmm_allocation_array[i].handle,
			     pmm_allocation_array[i].offset,
			     pmm_allocation_array[i].length);
		}
	}
	return;
}

/* This function tests the pmm_handleInt() function above. */
void pmm_test(void)
{
	u32 handle, length, addr;
	u16 function, flags;
	/*-------------------- Test simple allocation/find/deallocation ----------------------------- */
	function = 0;		/* pmmAllocate */
	handle = 0xdeadbeef;
	length = 16;		/* in 16byte paragraphs, so we allocate 256 bytes... */
	flags = 0x1;		/* conventional memory, unaligned */
	/* setup stack for call to pmm_handleInt() */
	push_word(flags);
	push_long(handle);
	push_long(length);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	addr = ((u32) M.x86.R_DX << 16) | M.x86.R_AX;
	DEBUG_PRINTF_PMM("%s: allocated memory at: %04x:%04x\n", __func__,
			 M.x86.R_DX, M.x86.R_AX);
	function = 1;		/* pmmFind */
	push_long(handle);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	DEBUG_PRINTF_PMM("%s: found memory at: %04x:%04x (expected: %08x)\n",
			 __func__, M.x86.R_DX, M.x86.R_AX, addr);
	function = 2;		/* pmmDeallocate */
	push_long(addr);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	DEBUG_PRINTF_PMM
	    ("%s: freed memory rval: %04x:%04x (expected: 0000:0000)\n",
	     __func__, M.x86.R_DX, M.x86.R_AX);
	/*-------------------- Test aligned allocation/deallocation ----------------------------- */
	function = 0;		/* pmmAllocate */
	handle = 0xdeadbeef;
	length = 257;		/* in 16byte paragraphs, so we allocate 4KB + 16 bytes... */
	flags = 0x1;		/* conventional memory, unaligned */
	/* setup stack for call to pmm_handleInt() */
	push_word(flags);
	push_long(handle);
	push_long(length);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	addr = ((u32) M.x86.R_DX << 16) | M.x86.R_AX;
	DEBUG_PRINTF_PMM("%s: allocated memory at: %04x:%04x\n", __func__,
			 M.x86.R_DX, M.x86.R_AX);
	function = 0;		/* pmmAllocate */
	handle = 0xf00d4b0b;
	length = 128;		/* in 16byte paragraphs, so we allocate 2KB... */
	flags = 0x5;		/* conventional memory, aligned */
	/* setup stack for call to pmm_handleInt() */
	push_word(flags);
	push_long(handle);
	push_long(length);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	/* the address should be aligned to 0x800, so probably it is at offset 0x1800... */
	addr = ((u32) M.x86.R_DX << 16) | M.x86.R_AX;
	DEBUG_PRINTF_PMM("%s: allocated memory at: %04x:%04x\n", __func__,
			 M.x86.R_DX, M.x86.R_AX);
	function = 1;		/* pmmFind */
	push_long(handle);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	addr = ((u32) M.x86.R_DX << 16) | M.x86.R_AX;
	function = 2;		/* pmmDeallocate */
	push_long(addr);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	DEBUG_PRINTF_PMM
	    ("%s: freed memory rval: %04x:%04x (expected: 0000:0000)\n",
	     __func__, M.x86.R_DX, M.x86.R_AX);
	handle = 0xdeadbeef;
	function = 1;		/* pmmFind */
	push_long(handle);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	addr = ((u32) M.x86.R_DX << 16) | M.x86.R_AX;
	function = 2;		/* pmmDeallocate */
	push_long(addr);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	DEBUG_PRINTF_PMM
	    ("%s: freed memory rval: %04x:%04x (expected: 0000:0000)\n",
	     __func__, M.x86.R_DX, M.x86.R_AX);
	/*-------------------- Test out of memory allocation ----------------------------- */
	function = 0;		/* pmmAllocate */
	handle = 0xdeadbeef;
	length = 0;		/* length zero means, give me the largest possible block */
	flags = 0x1;		/* conventional memory, unaligned */
	/* setup stack for call to pmm_handleInt() */
	push_word(flags);
	push_long(handle);
	push_long(length);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	length = ((u32) M.x86.R_DX << 16) | M.x86.R_AX;
	length /= 16;		/* length in paragraphs */
	DEBUG_PRINTF_PMM("%s: largest possible length: %08x\n", __func__,
			 length);
	function = 0;		/* pmmAllocate */
	flags = 0x1;		/* conventional memory, aligned */
	/* setup stack for call to pmm_handleInt() */
	push_word(flags);
	push_long(handle);
	push_long(length);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	addr = ((u32) M.x86.R_DX << 16) | M.x86.R_AX;
	DEBUG_PRINTF_PMM("%s: allocated memory at: %04x:%04x\n", __func__,
			 M.x86.R_DX, M.x86.R_AX);
	function = 0;		/* pmmAllocate */
	length = 1;
	handle = 0xf00d4b0b;
	flags = 0x1;		/* conventional memory, aligned */
	/* setup stack for call to pmm_handleInt() */
	push_word(flags);
	push_long(handle);
	push_long(length);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	/* this should fail, so 0x0 should be returned */
	addr = ((u32) M.x86.R_DX << 16) | M.x86.R_AX;
	DEBUG_PRINTF_PMM
	    ("%s: allocated memory at: %04x:%04x expected: 0000:0000\n",
	     __func__, M.x86.R_DX, M.x86.R_AX);
	handle = 0xdeadbeef;
	function = 1;		/* pmmFind */
	push_long(handle);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	addr = ((u32) M.x86.R_DX << 16) | M.x86.R_AX;
	function = 2;		/* pmmDeallocate */
	push_long(addr);
	push_word(function);
	push_long(0);		/* This is the return address for the ABI, unused in this implementation */
	pmm_handleInt();
	DEBUG_PRINTF_PMM
	    ("%s: freed memory rval: %04x:%04x (expected: 0000:0000)\n",
	     __func__, M.x86.R_DX, M.x86.R_AX);
}
