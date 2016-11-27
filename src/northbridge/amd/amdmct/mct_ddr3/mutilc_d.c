/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* This file contains functions for common utility functions */
#include <inttypes.h>
#include <console/console.h>
#include <string.h>
#include "mct_d.h"
#include "mct_d_gcc.h"
#include "mwlc_d.h"

static uint8_t is_fam15h(void)
{
	uint8_t fam15h = 0;
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	return fam15h;
}

void AmdMemPCIReadBits(SBDFO loc, u8 highbit, u8 lowbit, u32 *pValue)
{
	/* ASSERT(highbit < 32 && lowbit < 32 && highbit >= lowbit && (loc & 3) == 0); */

	AmdMemPCIRead(loc, pValue);
	*pValue = *pValue >> lowbit;  /* Shift */

	/* A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case */
	if ((highbit-lowbit) != 31)
		*pValue &= (((u32)1 << (highbit-lowbit+1))-1);
}

void AmdMemPCIWriteBits(SBDFO loc, u8 highbit, u8 lowbit, u32 *pValue)
{
	u32 temp, mask;

	/* ASSERT(highbit < 32 && lowbit < 32 && highbit >= lowbit && (loc & 3) == 0); */

	/* A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case */
	if ((highbit-lowbit) != 31)
		mask = (((u32)1 << (highbit-lowbit+1))-1);
	else
		mask = (u32)0xFFFFFFFF;

	AmdMemPCIRead(loc, &temp);
	temp &= ~(mask << lowbit);
	temp |= (*pValue & mask) << lowbit;
	AmdMemPCIWrite(loc, &temp);
}

/*-----------------------------------------------------------------------------
 * u32 bitTestSet(u32 csMask,u32 tempD)
 *
 * Description:
 *     This routine sets a bit in a u32
 *
 * Parameters:
 *     IN        csMask = Target value in which the bit will be set
 *     IN        tempD     =  Bit that will be set
 *     OUT    value     =  Target value with the bit set
 *-----------------------------------------------------------------------------
 */
u32 bitTestSet(u32 csMask,u32 tempD)
{
	u32 localTemp;
	/* ASSERT(tempD < 32); */
	localTemp = 1;
	csMask |= localTemp << tempD;
	return csMask;
}

/*-----------------------------------------------------------------------------
 * u32 bitTestReset(u32 csMask,u32 tempD)
 *
 * Description:
 *     This routine re-sets a bit in a u32
 *
 * Parameters:
 *     IN        csMask = Target value in which the bit will be re-set
 *     IN        tempD     =  Bit that will be re-set
 *     OUT    value     =  Target value with the bit re-set
 *-----------------------------------------------------------------------------
 */
u32 bitTestReset(u32 csMask,u32 tempD)
{
	u32 temp, localTemp;
	/* ASSERT(tempD < 32); */
	localTemp = 1;
	temp = localTemp << tempD;
	temp = ~temp;
	csMask &= temp;
	return csMask;
}

/*-----------------------------------------------------------------------------
 *  u32 get_Bits(DCTStruct *DCTData, u8 DCT, u8 Node, u8 func, u16 offset,
 *                 u8 low, u8 high)
 *
 * Description:
 *     This routine Gets the PCT bits from the specified Node, DCT and PCI address
 *
 * Parameters:
 *   IN  OUT *DCTData - Pointer to buffer with information about each DCT
 *     IN        DCT - DCT number
 *              - 1 indicates DCT 1
 *              - 0 indicates DCT 0
 *              - 2 both DCTs
 *          Node - Node number
 *          Func - PCI Function number
 *          Offset - PCI register number
 *          Low - Low bit of the bit field
 *          High - High bit of the bit field
 *
 *     OUT    value     =  Value read from PCI space
 *-----------------------------------------------------------------------------
 */
u32 get_Bits(sDCTStruct *pDCTData,
		u8 dct, u8 node, u8 func,
		u16 offset, u8 low, u8 high)
{
	u32 temp;
	uint32_t dword;

	/* ASSERT(node < MAX_NODES); */
	if (dct == BOTH_DCTS)
	{
		/* Registers exist on DCT0 only */
		if (is_fam15h())
		{
			/* Select DCT 0 */
			AmdMemPCIRead(MAKE_SBDFO(0,0,24+node,1,0x10c), &dword);
			dword &= ~0x1;
			AmdMemPCIWrite(MAKE_SBDFO(0,0,24+node,1,0x10c), &dword);
		}

		AmdMemPCIReadBits(MAKE_SBDFO(0,0,24+node,func,offset), high, low, &temp);
	}
	else
	{
		if (is_fam15h())
		{
			/* Select DCT */
			AmdMemPCIRead(MAKE_SBDFO(0,0,24+node,1,0x10c), &dword);
			dword &= ~0x1;
			dword |= (dct & 0x1);
			AmdMemPCIWrite(MAKE_SBDFO(0,0,24+node,1,0x10c), &dword);

			/* Read from the selected DCT */
			AmdMemPCIReadBits(MAKE_SBDFO(0,0,24+node,func,offset), high, low, &temp);
		}
		else
		{
			if (dct == 1)
			{
				/* Read from dct 1 */
				offset += 0x100;
				AmdMemPCIReadBits(MAKE_SBDFO(0,0,24+node,func,offset), high, low, &temp);
			}
			else
			{
				/* Read from dct 0 */
				AmdMemPCIReadBits(MAKE_SBDFO(0,0,24+node,func,offset), high, low, &temp);
			}
		}
	}
	return temp;
}

/*-----------------------------------------------------------------------------
 *  void set_Bits(DCTStruct *DCTData,u8 DCT,u8 Node,u8 func, u16 offset,
 *                u8 low, u8 high, u32 value)
 *
 * Description:
 *     This routine Sets the PCT bits from the specified Node, DCT and PCI address
 *
 * Parameters:
 *   IN  OUT *DCTData - Pointer to buffer with information about each DCT
 *     IN        DCT - DCT number
 *              - 1 indicates DCT 1
 *              - 0 indicates DCT 0
 *              - 2 both DCTs
 *          Node - Node number
 *          Func - PCI Function number
 *          Offset - PCI register number
 *          Low - Low bit of the bit field
 *          High - High bit of the bit field
 *
 *     OUT
 *-----------------------------------------------------------------------------
 */
void set_Bits(sDCTStruct *pDCTData,
		u8 dct, u8 node, u8 func,
		u16 offset, u8 low, u8 high, u32 value)
{
	u32 temp;
	uint32_t dword;

	temp = value;

	if (dct == BOTH_DCTS)
	{
		/* Registers exist on DCT0 only */
		if (is_fam15h())
		{
			/* Select DCT 0 */
			AmdMemPCIRead(MAKE_SBDFO(0,0,24+node,1,0x10c), &dword);
			dword &= ~0x1;
			AmdMemPCIWrite(MAKE_SBDFO(0,0,24+node,1,0x10c), &dword);
		}

		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+node,func,offset), high, low, &temp);
	}
	else
	{
		if (is_fam15h())
		{
			/* Select DCT */
			AmdMemPCIRead(MAKE_SBDFO(0,0,24+node,1,0x10c), &dword);
			dword &= ~0x1;
			dword |= (dct & 0x1);
			AmdMemPCIWrite(MAKE_SBDFO(0,0,24+node,1,0x10c), &dword);

			/* Write to the selected DCT */
			AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+node,func,offset), high, low, &temp);
		}
		else
		{
			if (dct == 1)
			{
				/* Write to dct 1 */
				offset += 0x100;
				AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+node,func,offset), high, low, &temp);
			}
			else
			{
				/* Write to dct 0 */
				AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+node,func,offset), high, low, &temp);
			}
		}
	}
}

/*-------------------------------------------------
 *  u32 get_ADD_DCT_Bits(DCTStruct *DCTData,u8 DCT,u8 Node,u8 func,
 *                         u16 offset,u8 low, u8 high)
 *
 * Description:
 *     This routine gets the Additional PCT register from Function 2 by specified
 *   Node, DCT and PCI address
 *
 * Parameters:
 *   IN  OUT *DCTData - Pointer to buffer with information about each DCT
 *     IN        DCT - DCT number
 *              - 1 indicates DCT 1
 *              - 0 indicates DCT 0
 *              - 2 both DCTs
 *          Node - Node number
 *          Func - PCI Function number
 *          Offset - Additional PCI register number
 *          Low - Low bit of the bit field
 *          High - High bit of the bit field
 *
 *     OUT
 *-------------------------------------------------
 */
u32 get_ADD_DCT_Bits(sDCTStruct *pDCTData,
		u8 dct, u8 node, u8 func,
		u16 offset, u8 low, u8 high)
{
	u32 tempD;
	tempD = offset;
	tempD = bitTestReset(tempD,DctAccessWrite);
	set_Bits(pDCTData, dct, node, FUN_DCT, DRAM_CONTROLLER_ADD_DATA_OFFSET_REG,
		PCI_MIN_LOW, PCI_MAX_HIGH, offset);
	while ((get_Bits(pDCTData,dct, node, FUN_DCT, DRAM_CONTROLLER_ADD_DATA_OFFSET_REG,
			DctAccessDone, DctAccessDone)) == 0);
	return (get_Bits(pDCTData, dct, node, FUN_DCT, DRAM_CONTROLLER_ADD_DATA_PORT_REG,
			low, high));
}

/*-------------------------------------------------
 *  void set_DCT_ADDR_Bits(DCTStruct *DCTData, u8 DCT,u8 Node,u8 func,
 *                         u16 offset,u8 low, u8 high, u32 value)
 *
 * Description:
 *     This routine sets the Additional PCT register from Function 2 by specified
 *   Node, DCT and PCI address
 *
 * Parameters:
 *   IN  OUT *DCTData - Pointer to buffer with information about each DCT
 *     IN        DCT - DCT number
 *              - 1 indicates DCT 1
 *              - 0 indicates DCT 0
 *              - 2 both DCTs
 *          Node - Node number
 *          Func - PCI Function number
 *          Offset - Additional PCI register number
 *          Low - Low bit of the bit field
 *          High - High bit of the bit field
 *
 *     OUT
 *-------------------------------------------------
 */
void set_DCT_ADDR_Bits(sDCTStruct *pDCTData,
		u8 dct, u8 node, u8 func,
		u16 offset, u8 low, u8 high, u32 value)
{
	u32 tempD;

	set_Bits(pDCTData, dct, node, FUN_DCT, DRAM_CONTROLLER_ADD_DATA_OFFSET_REG,
		PCI_MIN_LOW, PCI_MAX_HIGH, offset);
	while ((get_Bits(pDCTData,dct, node, FUN_DCT, DRAM_CONTROLLER_ADD_DATA_OFFSET_REG,
			DctAccessDone, DctAccessDone)) == 0);

	set_Bits(pDCTData, dct, node, FUN_DCT, DRAM_CONTROLLER_ADD_DATA_PORT_REG,
		low, high, value);
	tempD = offset;
	tempD = bitTestSet(tempD,DctAccessWrite);
	set_Bits(pDCTData, dct, node, FUN_DCT,DRAM_CONTROLLER_ADD_DATA_OFFSET_REG,
		PCI_MIN_LOW, PCI_MAX_HIGH, tempD);
	while ((get_Bits(pDCTData,dct, pDCTData->NodeId, FUN_DCT,
			DRAM_CONTROLLER_ADD_DATA_OFFSET_REG, DctAccessDone,
			DctAccessDone)) == 0);
}

/*-------------------------------------------------
 * BOOL bitTest(u32 value, u8 bitLoc)
 *
 * Description:
 *     This routine tests the value to determine if the bitLoc is set
 *
 * Parameters:
 *     IN        Value - value to be tested
 *          bitLoc - bit location to be tested
 *     OUT    TRUE - bit is set
 *          FALSE - bit is clear
 *-------------------------------------------------
 */
BOOL bitTest(u32 value, u8 bitLoc)
{
	u32 tempD, compD;
	tempD = value;
	compD = 0;
	compD = bitTestSet(compD,bitLoc);
	tempD &= compD;
	if (compD == tempD)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
