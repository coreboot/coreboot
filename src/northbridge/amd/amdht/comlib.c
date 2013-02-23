/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#undef FILECODE
#define FILECODE 0xCCCC
#include "comlib.h"

/*
 *---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS
 *
 *---------------------------------------------------------------------------
 */

void CALLCONV AmdPCIReadBits(SBDFO loc, u8 highbit, u8 lowbit, u32 *pValue)
{
	ASSERT(highbit < 32 && lowbit < 32 && highbit >= lowbit && (loc & 3) == 0);

	AmdPCIRead(loc, pValue);
	*pValue = *pValue >> lowbit;  /* Shift */

	/* A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case */
	if ((highbit-lowbit) != 31)
		*pValue &= (((u32)1 << (highbit-lowbit+1))-1);
}


void CALLCONV AmdPCIWriteBits(SBDFO loc, u8 highbit, u8 lowbit, u32 *pValue)
{
	u32 temp, mask;

	ASSERT(highbit < 32 && lowbit < 32 && highbit >= lowbit && (loc & 3) == 0);

	/* A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case */
	if ((highbit-lowbit) != 31)
		mask = (((u32)1 << (highbit-lowbit+1))-1);
	else
		mask = (u32)0xFFFFFFFF;

	AmdPCIRead(loc, &temp);
	temp &= ~(mask << lowbit);
	temp |= (*pValue & mask) << lowbit;
	AmdPCIWrite(loc, &temp);
}


/*
 *  Given a SBDFO this routine will find the next PCI capabilities list entry.
 *   If the end of the list of reached, or if a problem is detected, then
 *   ILLEGAL_SBDFO is returned.
 *
 *   To start a new search from the beginning of head of the list, specify a
 *   SBDFO with a offset of zero.
 */
void CALLCONV AmdPCIFindNextCap(SBDFO *pCurrent)
{
	SBDFO base;
	u32 offset;
	u32 temp;

	if (*pCurrent == ILLEGAL_SBDFO)
		return;

	offset = SBDFO_OFF(*pCurrent);
	base = *pCurrent - offset;
	*pCurrent = ILLEGAL_SBDFO;

	/* Verify that the SBDFO points to a valid PCI device SANITY CHECK */
	AmdPCIRead(base, &temp);
	if (temp == 0xFFFFFFFF)
		return; /* There is no device at this address */

	/* Verify that the device supports a capability list */
	AmdPCIReadBits(base + 0x04, 20, 20, &temp);
	if (temp == 0)
		return; /* This PCI device does not support capability lists */

	if (offset != 0)
	{
		/* If we are continuing on an existing list */
		AmdPCIReadBits(base + offset, 15, 8, &temp);
	}
	else
	{
		/* We are starting on a new list */
		AmdPCIReadBits(base + 0x34, 7, 0, &temp);
	}

	if (temp == 0)
		return; /* We have reached the end of the capabilties list */

	/* Error detection and recovery- The statement below protects against
		PCI devices with broken PCI capabilities lists.	 Detect a pointer
		that is not u32 aligned, points into the first 64 reserved DWORDs
		or points back to itself.
	*/
	if (((temp & 3) != 0) || (temp == offset) || (temp < 0x40))
		return;

	*pCurrent = base + temp;
	return;
}


void CALLCONV Amdmemcpy(void *pDst, const void *pSrc, u32 length)
{
	ASSERT(length <= 32768);
	ASSERT(pDst != NULL);
	ASSERT(pSrc != NULL);

	while (length--){
	//	*(((u8*)pDst)++) = *(((u8*)pSrc)++);
		*((u8*)pDst) = *((u8*)pSrc);
		pDst++;
		pSrc++;
	}
}


void CALLCONV Amdmemset(void *pBuf, u8 val, u32 length)
{
	ASSERT(length <= 32768);
	ASSERT(pBuf != NULL);

	while (length--){
		//*(((u8*)pBuf)++) = val;
		*(((u8*)pBuf)) = val;
		pBuf++;
	}
}


u8 CALLCONV AmdBitScanReverse(u32 value)
{
	u8 i;

	for (i = 31; i != 0xFF; i--)
	{
		if (value & ((u32)1 << i))
			break;
	}

	return i;
}


u32 CALLCONV AmdRotateRight(u32 value, u8 size, u32 count)
{
	u32 msb, mask;
	ASSERT(size > 0 && size <= 32);

	msb = (u32)1 << (size-1);
	mask = ((msb-1) << 1) + 1;

	value = value & mask;

	while (count--)
	{
		if (value & 1)
			value = (value >> 1) | msb;
		else
			value = value >> 1;
	}

	return value;
}


u32 CALLCONV AmdRotateLeft(u32 value, u8 size, u32 count)
{
	u32 msb, mask;
	ASSERT(size > 0 && size <= 32);

	msb = (u32)1 << (size-1);
	mask = ((msb-1) << 1) + 1;

	value = value & mask;

	while (count--)
	{
		if (value & msb)
			value = ((value << 1) & mask) | (u32)1;
		else
			value = ((value << 1) & mask);
	}

	return value;
}


void CALLCONV AmdPCIRead(SBDFO loc, u32 *Value)
{
	/* Use coreboot PCI functions */
	*Value = pci_read_config32((loc & 0xFFFFF000), SBDFO_OFF(loc));
}


void CALLCONV AmdPCIWrite(SBDFO loc, u32 *Value)
{
	/* Use coreboot PCI functions */
	pci_write_config32((loc & 0xFFFFF000), SBDFO_OFF(loc), *Value);
}


void CALLCONV AmdMSRRead(uint32 Address, uint64 *Value)
{
	msr_t msr;

	msr = rdmsr(Address);
	Value->lo = msr.lo;
	Value->hi = msr.hi;
}


void CALLCONV AmdMSRWrite(uint32 Address, uint64 *Value)
{
	msr_t msr;

	msr.lo = Value->lo;
	msr.hi = Value->hi;
	wrmsr(Address, msr);
}


void ErrorStop(u32 value)
{
	printk(BIOS_DEBUG, "Error: %08x ", value);

}

/*;----------------------------------------------------------------------------
; void __pascal ErrorStop(DWORD Value);
;
; This implementation provides a rotating display of the error code on the
; a port 80h POST display card.  The rotation is used to make it easier to
; view the error on both a 16-bit as well as a 32-bit display card.
;
; For use with SimNow the unrotated error code is also written to port 84h
ErrorStop   PROC FAR PASCAL PUBLIC Value:DWORD
        pushad
        mov     eax, Value
        mov     bx, 0DEADh
        out     84h, eax

ErrorStopTop:
        out     80h, eax

        mov     cx, 4           ; Rotate the display by one nibble
@@:
        bt      bx, 15
        rcl     eax, 1
        rcl     bx, 1
        loop    @B


        push    eax             ; Delay a few hundred milliseconds
        push    ebx
        mov     ecx, 10h        ; TSC
        db      00Fh, 032h      ; RDMSR
        mov     ebx, eax
@@:
        db      00Fh, 032h      ; RDMSR
        sub     eax, ebx
        cmp     eax, 500000000
        jb      @B
        pop     ebx
        pop     eax

        jmp     ErrorStopTop

        popad
        ret
ErrorStop   ENDP
*/
