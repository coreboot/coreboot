/* $NoKeywords:$ */
/**
 * @file
 *
 * mnregdr.c
 *
 * Common Northbridge register related functions for DR
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/DR)
 * @e \$Revision: 47676 $ @e \$Date: 2011-02-25 06:29:57 +0800 (Fri, 25 Feb 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
 *       its contributors may be used to endorse or promote products derived 
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ***************************************************************************
*
*/
/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mndr.h"
#include "merrhdl.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_DR_MNREGDR_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------*/
/**
 *     MemNIsIdSupportedDr
 *      This function matches the CPU_LOGICAL_ID with certain criteria to
 *      determine if it is supported by this NBBlock.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *LogicalIdPtr - Pointer to the CPU_LOGICAL_ID
 *
 *     @return          TRUE -  This node is a RB.
 *     @return          FALSE - This node is not a RB.
 *
 */
BOOLEAN
MemNIsIdSupportedDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  )
{

  if (((LogicalIdPtr->Family & (AMD_FAMILY_10_RB | AMD_FAMILY_10_BL | AMD_FAMILY_10_DA)) != 0)
      && ((LogicalIdPtr->Revision & (AMD_F10_RB_ALL | AMD_F10_BL_ALL | AMD_F10_DA_ALL)) != 0)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets or sets a value to a bit field in a PCI register.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Bit Field to be programmed
 *     @param[in]   Field - Value to be programmed
 *     @param[in]   IsSet - Indicates if the function will set or get
 *
 *     @return      value read, if the function is used as a "get"
 */

UINT32
MemNCmnGetSetFieldDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  )
{
  TSEFO Address;
  PCI_ADDR PciAddr;
  UINT8 Type;
  UINT8 IsLinked;
  UINT32 Value;
  UINT32 Highbit;
  UINT32 Lowbit;
  UINT32 Mask;

  Value = 0;
  if ((FieldName < BFEndOfList) && (FieldName >= 0)) {
    Address = NBPtr->NBRegTable[FieldName];
    if (Address) {
      Lowbit = TSEFO_END (Address);
      Highbit = TSEFO_START (Address);
      Type = (UINT8) TSEFO_TYPE (Address);
      IsLinked = (UINT8) TSEFO_LINKED (Address);

      // If Fn2 and DCT1 selected, set Address to be 1xx
      if ((Type == NB_ACCESS) && ((Address & 0xF000) == 0x2000) && NBPtr->Dct) {
        if (!NBPtr->Ganged || (Address & 0xFF) == 0x98 || (Address & 0xFF) == 0x9C) {
          Address |= 0x0100;
        }
      }

      ASSERT ((Address & ((UINT32) 1) << 28) == 0);   // Phy direct access method is not supported

      if ((Address >> 29) == ((DCT_PHY_ACCESS << 1) | 1)) {
        // Special DCT Phy access
        Address &= 0x0FFFFFFF;
        Lowbit = 0;
        Highbit = 16;
        IsLinked = 0;
      } else {
        // Normal DCT Phy access
        Address = TSEFO_OFFSET (Address);
      }

      if (Type == NB_ACCESS) {
        PciAddr.AddressValue = Address;
        PciAddr.Address.Device = NBPtr->PciAddr.Address.Device;
        PciAddr.Address.Bus = NBPtr->PciAddr.Address.Bus;
        PciAddr.Address.Segment = NBPtr->PciAddr.Address.Segment;
        Address = PciAddr.AddressValue;
        LibAmdPciRead (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
        if ((FieldName != BFDctAddlDataReg) && (FieldName != BFDctAddlOffsetReg) &&
            (FieldName != BFDctExtraDataReg) && (FieldName != BFDctExtraOffsetReg)) {
          IDS_HDT_CONSOLE (MEM_GETREG, "~Fn%d_%03x = %x\n", (Address >> 12) & 0xF, Address & 0xFFF, Value);
        }
      } else if (Type == DCT_PHY_ACCESS) {
        MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
        MemNPollBitFieldNb (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
        Value = MemNGetBitFieldNb (NBPtr, BFDctAddlDataReg);
        IDS_HDT_CONSOLE (MEM_GETREG, "~Fn2_%d9C_%x = %x\n", NBPtr->Dct, Address & 0x0FFFFFFF, Value);
      } else {
        IDS_ERROR_TRAP;
      }

      if (IsSet) {
        // A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case
        if ((Highbit - Lowbit) != 31) {
          Mask = (((UINT32)1 << (Highbit - Lowbit + 1)) - 1);
        } else {
          Mask = (UINT32)0xFFFFFFFF;
        }
        Value &= ~(Mask << Lowbit);
        Value |= (Field & Mask) << Lowbit;

        if (Type == NB_ACCESS) {
          PciAddr.AddressValue = Address;
          LibAmdPciWrite (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
          if ((FieldName != BFDctAddlDataReg) && (FieldName != BFDctAddlOffsetReg) &&
              (FieldName != BFDctExtraDataReg) && (FieldName != BFDctExtraOffsetReg)) {
            IDS_HDT_CONSOLE (MEM_SETREG, "~Fn%d_%03x [%d:%d] = %x\n", (Address >> 12) & 0xF, Address & 0xFFF, Highbit, Lowbit, Field);
          }
        } else if (Type == DCT_PHY_ACCESS) {
          MemNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
          Address |= DCT_ACCESS_WRITE;
          MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
          MemNPollBitFieldNb (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
          IDS_HDT_CONSOLE (MEM_SETREG, "~Fn2_%d9C_%x [%d:%d] = %x\n", NBPtr->Dct, Address & 0x0FFFFFFF, Highbit, Lowbit, Field);
        } else {
          IDS_ERROR_TRAP;
        }
        if (IsLinked) {
          MemNCmnGetSetFieldDr (NBPtr, 1, FieldName + 1, Field >> (Highbit - Lowbit + 1));
        }
      } else {
        Value = Value >> Lowbit;  // Shift
        // A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case
        if ((Highbit - Lowbit) != 31) {
          Value &= (((UINT32)1 << (Highbit - Lowbit + 1)) - 1);
        }
        if (IsLinked) {
          Value |= MemNCmnGetSetFieldDr (NBPtr, 0, FieldName + 1, 0) << (Highbit - Lowbit + 1);
        }
      }
    }
  } else {
    IDS_ERROR_TRAP;   // Invalid bit field index
  }
  return Value;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes bit field translation table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   NBRegTable[]   - Pointer to the bit field data structure
 *
 */

VOID
InitNBRegTableDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  )
{
  UINT16 i;

  // Allocate heap for NB register table
  if (!MemNAllocateNBRegTableNb (NBPtr, NbRegTabDR)) {
    return;     // escape if fails
  }
  NBRegTable = NBPtr->NBRegTable;

  for (i = 0; i < BFEndOfList; i++) {
    NBRegTable[i] = 0;
  }

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (0, 0x00), 31,  0, BFDevVendorIDReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (0, 0x60),  2,  0, BFNodeID);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (0, 0x60),  6,  4, BFNodeCnt);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x40), 31, 0, BFDramBaseReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x44), 31, 0, BFDramLimitReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x48), 31, 0, BFDramBaseReg1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x4C), 31, 0, BFDramLimitReg1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x50), 31, 0, BFDramBaseReg2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x54), 31, 0, BFDramLimitReg2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x58), 31, 0, BFDramBaseReg3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x5C), 31, 0, BFDramLimitReg3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x60), 31, 0, BFDramBaseReg4);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x64), 31, 0, BFDramLimitReg4);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x68), 31, 0, BFDramBaseReg5);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x6C), 31, 0, BFDramLimitReg5);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x70), 31, 0, BFDramBaseReg6);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x74), 31, 0, BFDramLimitReg6);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x78), 31, 0, BFDramBaseReg7);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x7C), 31, 0, BFDramLimitReg7);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0), 31, 0, BFDramHoleAddrReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x140), 7, 0, BFDramBaseHiReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x144), 7, 0, BFDramLimitHiReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x148), 7, 0, BFDramBaseHiReg1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x14C), 7, 0, BFDramLimitHiReg1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x150), 7, 0, BFDramBaseHiReg2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x154), 7, 0, BFDramLimitHiReg2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x158), 7, 0, BFDramBaseHiReg3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x15C), 7, 0, BFDramLimitHiReg3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x160), 7, 0, BFDramBaseHiReg4);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x164), 7, 0, BFDramLimitHiReg4);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x168), 7, 0, BFDramBaseHiReg5);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x16C), 7, 0, BFDramLimitHiReg5);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x170), 7, 0, BFDramBaseHiReg6);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x174), 7, 0, BFDramLimitHiReg6);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x178), 7, 0, BFDramBaseHiReg7);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x17C), 7, 0, BFDramLimitHiReg7);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0), 31, 24, BFDramHoleBase);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0), 15,  7, BFDramHoleOffset);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0),  1,  1, BFDramMemHoistValid);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0),  0,  0, BFDramHoleValid);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x120), 20,  0, BFDramBaseAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x120), 23, 21, BFDramIntlvSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x124), 20,  0, BFDramLimitAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x124), 26, 21, BFDramIntlvEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x40), 31,  0, BFCSBaseAddr0Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x44), 31,  0, BFCSBaseAddr1Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x48), 31,  0, BFCSBaseAddr2Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x4C), 31,  0, BFCSBaseAddr3Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x50), 31,  0, BFCSBaseAddr4Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x54), 31,  0, BFCSBaseAddr5Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x58), 31,  0, BFCSBaseAddr6Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x5C), 31,  0, BFCSBaseAddr7Reg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x60), 31,  0, BFCSMask0Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x64), 31,  0, BFCSMask1Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x68), 31,  0, BFCSMask2Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x6C), 31,  0, BFCSMask3Reg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 31,  0, BFDramControlReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31,  0, BFDramInitRegReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x80), 31,  0, BFDramBankAddrReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 31,  0, BFDramMRSReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 31,  0, BFDramTimingLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 31,  0, BFDramTimingHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 31,  0, BFDramConfigLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31,  0, BFDramConfigHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31,  0, BFDctAddlOffsetReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x9C), 31,  0, BFDctAddlDataReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31, 31, BFDctAccessDone);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA0), 31,  0, BFDramConfigMiscReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 31,  0, BFDramCtrlMiscReg2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xF4), 31,  0, BFDctExtraDataReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 31,  0, BFMctCfgHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x118), 31,  0, BFMctCfgLoReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x40), 31,  0, BFMcaNbCtlReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x44), 22, 22, BFDramEccEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x44),  2,  2, BFSyncOnUcEccEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x180), 25, 25, BFEccSymbolSize);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x48), 31,  0, BFMcaNbStatusLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x4C), 31,  0, BFMcaNbStatusHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x58),  4,  0, BFDramScrub);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x58), 12,  8, BFL2Scrub);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x58), 20, 16, BFDcacheScrub);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x58), 28, 24, BFL3Scrub);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x5C),  0,  0, BFScrubReDirEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x5C), 31,  0, BFScrubAddrLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x60), 31,  0, BFScrubAddrHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x84), 31, 29, BFC1ClkDivisor);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x8C),  4,  4, BFDisDatMsk);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xD4),  4,  0, BFNbFid);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xE8), 25, 25, BFL3Capable);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x188), 8,  8, BFDisableL3);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 13,  8, BFNonSPDHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78),  3,  0, BFRdPtrInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78),  9,  8, BFTwrrdHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 11, 10, BFTwrwrHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 13, 12, BFTrdrdHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 16, 16, BFAltVidC3MemClkTriEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 18, 18, BFDqsRcvEnTrain);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 19, 19, BFEarlyArbEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 31, 22, BFMaxLatency);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 15,  0, BFMrsAddress);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 18, 16, BFMrsBank);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 22, 20, BFMrsChipSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 24, 24, BFSendPchgAll);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 25, 25, BFSendAutoRefresh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 26, 26, BFSendMrsCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 27, 27, BFDeassertMemRstX);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 28, 28, BFAssertCke);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 29, 29, BFSendZQCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 30, 30, BFSendCtrlWord);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31, 31, BFEnDramInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C),  7,  7, BFMrsLevel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 12, 12, BFMrsQoff);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  1,  0, BFBurstCtrl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  3,  2, BFDrvImpCtrl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  9,  7, BFDramTerm_DDR3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 11, 10, BFDramTermDyn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 13, 13, BFQoff);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 18, 18, BFASR);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 19, 19, BFSRT);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 22, 20, BFTcwl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 23, 23, BFPchgPDModeSel);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  6,  4, BFTwrDDR3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88),  3,  0, BFTcl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88),  6,  4, BFTrcd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88),  9,  7, BFTrp);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 11, 10, BFTrtp);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 15, 12, BFTras);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 20, 16, BFTrc);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 21, 20, BFTwr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 23, 22, BFTrrd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 31, 24, BFMemClkDis);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 15,  0, BFNonSPD);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C),  3,  0, BFTrwtWB);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C),  7,  4, BFTrwtTO);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C),  9,  8, BFTwtr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 11, 10, BFTwrrd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 13, 12, BFTwrwr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 15, 14, BFTrdrd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 17, 16, BFTref);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 18, 18, BFDisAutoRefresh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 22, 20, BFTrfc0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 25, 23, BFTrfc1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 28, 26, BFTrfc2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 31, 29, BFTrfc3);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  0,  0, BFInitDram);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  1,  1, BFExitSelfRef);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  5,  4, BFDramTerm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  8,  8, BFParEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 10, 10, BFBurstLength32);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 11, 11, BFWidth128);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 15, 12, BFX4Dimm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 16, 16, BFUnBuffDimm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 17, 17, BFEnterSelfRef);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 19, 19, BFDimmEccEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 21, 21, BFFreqChgInProg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 23, 23, BFForceAutoPchg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  2,  0, BFMemClkFreq);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  3,  3, BFMemClkFreqVal);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  8,  8, BFDdr3Mode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  9,  9, BFLegacyBiosMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 11, 10, BFZqcsInterval);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 12, 12, BFRDqsEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 14, 14, BFDisDramInterface);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 15, 15, BFPowerDownEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 16, 16, BFPowerDownMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 17, 17, BFFourRankSoDimm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 19, 19, BFDcqArbBypassEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 18, 18, BFFourRankRDimm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 20, 20, BFSlowAccessMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 22, 22, BFBankSwizzleMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 27, 24, BFDcqBypassMax);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31, 28, BFFourActWindow);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4),  8,  8, BFODTSEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4), 14, 12, BFCmdThrottleMode);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  2,  2, BFDdr3FourSocketCh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  5,  5, BFSubMemclkRegDly);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  6,  6, BFOdtSwizzle);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 15,  8, BFCtrlWordCS);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 18, 16, BFDataTxFifoWrDly);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  0,  0, BFDctSelHiRngEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  1,  1, BFDctSelHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  2,  2, BFDctSelIntLvEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  3,  3, BFMemClrInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  4,  4, BFDctGangEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  5,  5, BFDctDatIntLv);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  7,  6, BFDctSelIntLvAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  8,  8, BFDramEnabled);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  9,  9, BFMemClrBusy);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110), 10, 10, BFMemCleared);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110), 31, 11, BFDctSelBaseAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x114), 31, 10, BFDctSelBaseOffset);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C),  1,  0, BFDctWrLimit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C),  6,  2, BFMctWrLimit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 11,  7, BFMctPrefReqLimit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 12, 12, BFPrefCpuDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 13, 13, BFPrefIoDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 28, 28, BFPrefDramTrainMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 30, 30, BFFlushWr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 29, 29, BFFlushWrOnStpGnt);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0),  1,  0, BFAdapPrefMissRatio);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0),  3,  2, BFAdapPrefPosStep);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0),  5,  4, BFAdapPrefNegStep);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0), 10,  8, BFCohPrefPrbLmt);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xB0), 31, 0, BFOnLineSpareControl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xE8),  7, 5, BFDdrMaxRate);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  9,  8, BFAddrCmdDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 17, 16, BFDataDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 31,  0, BFODCControl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x04, 31,  0, BFAddrTmgControl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  0,  0, BFWrtLvTrEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  1,  1, BFWrtLvTrMode);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  3,  3, BFPhyFenceTrEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  5,  4, BFTrDimmSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 11,  8, BFWrLvOdt);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 12, 12, BFWrLvOdtEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 13, 13, BFDqsRcvTrEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 30, 30, BFDisAutoComp);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x53,  8,  0, BFWrtLvErr);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0A, 27, 25, BFD3Cmp2PCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0A, 22, 20, BFD3Cmp2NCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0A, 17, 15, BFD3Cmp1PCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0A, 12, 10, BFD3Cmp1NCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0A,  7,  5, BFD3Cmp0PCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0A,  2,  0, BFD3Cmp0NCal);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C, 20, 16, BFPhyFence);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C, 13, 12, BFCKETri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C, 11,  8, BFODTTri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C,  7,  0, BFChipSelTri);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D040F30, _NOT_USED_, _NOT_USED_, BFErr263);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D080F0C, _NOT_USED_, _NOT_USED_, BFErr350);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D08E000, _NOT_USED_, _NOT_USED_, BFErr322I);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D02E001, _NOT_USED_, _NOT_USED_, BFErr322II);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0F2030, _NOT_USED_, _NOT_USED_, BFPhyClkConfig0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0F2130, _NOT_USED_, _NOT_USED_, BFPhyClkConfig1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0F2230, _NOT_USED_, _NOT_USED_, BFPhyClkConfig2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0F2330, _NOT_USED_, _NOT_USED_, BFPhyClkConfig3);

  LINK_TSEFO (NBRegTable, BFTwrrd, BFTwrrdHi);
  LINK_TSEFO (NBRegTable, BFTwrwr, BFTwrwrHi);
  LINK_TSEFO (NBRegTable, BFTrdrd, BFTrdrdHi);

}
