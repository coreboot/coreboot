/* $NoKeywords:$ */
/**
 * @file
 *
 * mnregor.c
 *
 * Common Northbridge register related functions for Orochi
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/OR)
 * @e \$Revision: 58126 $ @e \$Date: 2011-08-21 23:38:29 -0600 (Sun, 21 Aug 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2012 Advanced Micro Devices, Inc.
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
*
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
#include "mnor.h"
#include "merrhdl.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_OR_MNREGOR_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define PHY_DIRECT_ADDRESS_MASK   0x0D000000

STATIC CONST UINT8 InstancesPerTypeOR[8] = {9, 3, 1, 0, 2, 0, 1, 1};

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
 *     MemNIsIdSupportedOr
 *      This function matches the CPU_LOGICAL_ID with certain criteria to
 *      determine if it is supported by this NBBlock.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *LogicalIdPtr - Pointer to the CPU_LOGICAL_ID
 *
 *     @return          TRUE -  This node is a Orochi.
 *     @return          FALSE - This node is not a Orochi.
 *
 */
BOOLEAN
MemNIsIdSupportedOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  )
{
  if (((LogicalIdPtr->Family & AMD_FAMILY_15_OR) != 0)
      && ((LogicalIdPtr->Revision & AMD_F15_ALL) != 0)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*-----------------------------------------------------------------------------*/
/**
 *      This function calculates the memory channel index relative to the
 *      socket, taking the Die number, the Dct, and the channel.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in] Dct
 *     @param[in] Channel
 *
 */
UINT8
MemNGetSocketRelativeChannelOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct,
  IN       UINT8 Channel
  )
{
  return ((NBPtr->MCTPtr->DieId * MAX_DCTS_PER_NODE_OR) + Dct);
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
MemNCmnGetSetFieldOr (
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
  UINT8  IsPhyDirectAccess;
  UINT8  IsWholeRegAccess;
  UINT8  NumOfInstances;
  UINT8  Instance;

  Value = 0;
  if ((FieldName < BFEndOfList) && (FieldName >= 0)) {
    Address = NBPtr->NBRegTable[FieldName];
    if (Address) {
      Lowbit = TSEFO_END (Address);
      Highbit = TSEFO_START (Address);
      Type = (UINT8) TSEFO_TYPE (Address);
      IsLinked = (UINT8) TSEFO_LINKED (Address);
      IsPhyDirectAccess = (UINT8) TSEFO_DIRECT_EN (Address);
      IsWholeRegAccess = (UINT8) TSEFO_WHOLE_REG_ACCESS (Address);

      ASSERT ((Address & ((UINT32) 1) << 29) == 0);   // Old Phy direct access method is not supported

      Address = TSEFO_OFFSET (Address);

      // By default, a bit field has only one instance
      NumOfInstances = 1;

      if ((Type == DCT_PHY_ACCESS) && IsPhyDirectAccess) {
        Address |= PHY_DIRECT_ADDRESS_MASK;
        if (IsWholeRegAccess) {
          // In the case of whole regiter access (bit 0 to 15),
          // HW broadcast and nibble mask will be used.
          Address |= Lowbit << 16;
          Lowbit = 0;
          Highbit = 15;
        } else {
          // In the case only some bits on a register is accessed,
          // BIOS will do read-mod-write to all chiplets manually.
          // And nibble mask will be 1111b always.
          Address |= 0x000F0000;
          Field >>= Lowbit;
          if ((Address & 0x0F00) == 0x0F00) {
            // Broadcast mode
            // Find out how many instances to write to
            NumOfInstances = InstancesPerTypeOR[(Address >> 13) & 0x7];
            if (!IsSet) {
              // For read, only read from instance 0 in broadcast mode
              NumOfInstances = 1;
            }
          }
        }
      }

      ASSERT (NumOfInstances > 0);

      for (Instance = 0; Instance < NumOfInstances; Instance++) {
        if (Type == NB_ACCESS) {
          PciAddr.AddressValue = Address;
          PciAddr.Address.Device = NBPtr->PciAddr.Address.Device;
          PciAddr.Address.Bus = NBPtr->PciAddr.Address.Bus;
          PciAddr.Address.Segment = NBPtr->PciAddr.Address.Segment;
          Address = PciAddr.AddressValue;
          LibAmdPciRead (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
          if ((FieldName != BFDctAddlDataReg) && (FieldName != BFDctAddlOffsetReg) && (FieldName != BFDctCfgSel)) {
            IDS_HDT_CONSOLE (MEM_GETREG, "~Dev%x Dct%d Fn%d_%03x = %x\n",
                             NBPtr->PciAddr.Address.Device, NBPtr->Dct,
                             (Address >> 12) & 0xF, Address & 0xFFF, Value);
          }
        } else if (Type == DCT_PHY_ACCESS) {
          if (IsPhyDirectAccess && (NumOfInstances > 1)) {
            Address = (Address & 0x0FFFF0FF) | (((UINT32) Instance) << 8);
          }
          MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
          MemNPollBitFieldNb (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
          Value = MemNGetBitFieldNb (NBPtr, BFDctAddlDataReg);
          IDS_HDT_CONSOLE (MEM_GETREG, "~Dev%x Dct%d Fn2_9C_%x = %x\n", NBPtr->PciAddr.Address.Device, NBPtr->Dct, Address & 0x0FFFFFFF, Value);
        } else if (Type == DCT_EXTRA) {
          MemNSetBitFieldNb (NBPtr, BFDctExtraOffsetReg, Address);
          // @attention: DctExtraAccessDone not implemented in Orochi
          //MemNPollBitFieldNb (NBPtr, BFDctExtraAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
          Value = MemNGetBitFieldNb (NBPtr, BFDctExtraDataReg);
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
            if ((FieldName != BFDctAddlDataReg) && (FieldName != BFDctAddlOffsetReg) && (FieldName != BFDctCfgSel)) {
              IDS_HDT_CONSOLE (MEM_SETREG, "~Dev%x Dct%d Fn%d_%03x [%d:%d] = %x\n",
                               NBPtr->PciAddr.Address.Device, NBPtr->Dct,
                               (Address >> 12) & 0xF, Address & 0xFFF, Highbit, Lowbit, Field);
            }
          } else if (Type == DCT_PHY_ACCESS) {
            MemNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
            Address |= DCT_ACCESS_WRITE;
            MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
            MemNPollBitFieldNb (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
            IDS_HDT_CONSOLE (MEM_SETREG, "~Dev%x Dct%d Fn2_9C_%x [%d:%d] = %x\n",
                             NBPtr->PciAddr.Address.Device, NBPtr->Dct,
                             Address & 0x0FFFFFFF, Highbit, Lowbit, Field);
          } else if (Type == DCT_EXTRA) {
            MemNSetBitFieldNb (NBPtr, BFDctExtraDataReg, Value);
            Address |= DCT_ACCESS_WRITE;
            MemNSetBitFieldNb (NBPtr, BFDctExtraOffsetReg, Address);
            // @attention: DctExtraAccessDone not implemented in Orochi
            //MemNPollBitFieldNb (NBPtr, BFDctExtraAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
          } else {
            IDS_ERROR_TRAP;
          }
          if (IsLinked) {
            MemNCmnGetSetFieldOr (NBPtr, 1, FieldName + 1, Field >> (Highbit - Lowbit + 1));
          }
        } else {
          Value = Value >> Lowbit;  // Shift
          // A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case
          if ((Highbit - Lowbit) != 31) {
            Value &= (((UINT32)1 << (Highbit - Lowbit + 1)) - 1);
          }
          if (IsLinked) {
            Value |= MemNCmnGetSetFieldOr (NBPtr, 0, FieldName + 1, 0) << (Highbit - Lowbit + 1);
          }
          // For direct phy access, shift the bit back for compatibility reason.
          if ((Type == DCT_PHY_ACCESS) && IsPhyDirectAccess) {
            Value <<= Lowbit;
          }
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
 *     @param[in,out]   NBRegTable[]  - Pointer to the bit field data structure
 *
 */

VOID
MemNInitNBRegTableOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  )
{
  UINT16 i;

  // Allocate heap for NB register table
  if (!MemNAllocateNBRegTableNb (NBPtr, NbRegTabOR)) {
    return;     // escape if fails
  }
  NBRegTable = NBPtr->NBRegTable;

  for (i = 0; i < BFEndOfList; i++) {
    NBRegTable[i] = 0;
  }
  // ---------------------------------------------------------------------------
  //
  // FUNCTION 0
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (0, 0x00), 31,  0, BFDevVendorIDReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (0, 0x60),  2,  0, BFNodeID);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (0, 0x60),  6,  4, BFNodeCnt);

  // ---------------------------------------------------------------------------
  //
  // FUNCTION 1
  //
  // ---------------------------------------------------------------------------
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
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0),  2,  2, BFDramHtHoleValid);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0),  1,  1, BFDramMemHoistValid);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0),  0,  0, BFDramHoleValid);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x10C),  5,  4, BFNbPsSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x10C),  0,  0, BFDctCfgSel);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x120), 23, 21, BFDramIntlvSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x120), 20,  0, BFDramBaseAddr);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x124), 23, 21, BFDramIntlvEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x124), 20,  0, BFDramLimitAddr);
  // ---------------------------------------------------------------------------
  //
  // FUNCTION 2
  //
  // ---------------------------------------------------------------------------
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

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x60), 1,  0, BFRankDef0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x64), 1,  0, BFRankDef1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x68), 1,  0, BFRankDef2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x6C), 1,  0, BFRankDef3);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 31,  0, BFDramControlReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 18, 18, BFDqsRcvEnTrain);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 17, 17, BFAddrCmdTriEn); ///< Orochi
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 16, 16, BFReserved001);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31,  0, BFDramInitRegReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31, 31, BFEnDramInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 30, 30, BFSendCtrlWord);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 29, 29, BFSendZQCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 28, 28, BFAssertCke);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 27, 27, BFDeassertMemRstX);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 26, 26, BFSendMrsCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 25, 25, BFSendAutoRefresh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 24, 24, BFReserved002);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 23, 21, BFMrsChipSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 20, 18, BFMrsBank);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 17,  0, BFMrsAddress);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 17, 13, BFMrsAddressHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 12, 12, BFMrsQoff);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C),  7,  7, BFMrsLevel);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x80), 31,  0, BFDramBankAddrReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 31,  0, BFDramMRSReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 23, 23, BFPchgPDModeSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 19, 19, BFSRT);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  9,  7, BFDramTerm_DDR3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 11, 10, BFDramTermDyn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  1,  0, BFBurstCtrl);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 31,  0, BFDramTimingLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 29, 24, BFMemClkDis);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 31,  0, BFDramTimingHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 18, 18, BFDisAutoRefresh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 17, 16, BFTref);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 31,  0, BFDramConfigLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 27, 27, BFDisDllShutdownSR);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 25, 25, BFPendRefPaybackS3En);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 24, 24, BFStagRefEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 23, 23, BFForceAutoPchg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 20, 20, BFDynPageCloseEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 19, 19, BFDimmEccEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 17, 17, BFEnterSelfRef);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 16, 16, BFUnBuffDimm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 15, 12, BFX4Dimm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  8,  8, BFParEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  1,  1, BFExitSelfRef);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  0,  0, BFInitDram);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31,  0, BFDramConfigHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 27, 24, BFDcqBypassMax);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 22, 22, BFBankSwizzleMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 21, 21, BFFreqChgInProg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 20, 20, BFSlowAccessMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 19, 19, BFDcqArbBypassEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 18, 18, BFFourRankRDimm0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 17, 17, BFFourRankRDimm1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 16, 16, BFPowerDownMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 15, 15, BFPowerDownEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 14, 14, BFDisDramInterface);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 13, 13, BFDisSimulRdWr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 12, 12, BFRDqsEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 11, 10, BFZqcsInterval);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  7,  7, BFMemClkFreqVal);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  4,  0, BFMemClkFreq);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31,  0, BFDctAddlOffsetReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x9C), 31,  0, BFDctAddlDataReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31, 31, BFDctAccessDone);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA0), 31,  0, BFDramConfigMiscReg); ///< Orochi Read Only
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA0), 31,  31, BFRcvParErr);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4), 14, 12, BFCmdThrottleMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4),  11,  11, BFBwCapEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4),  8,  8, BFODTSEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 31,  0, BFDramCtrlMiscReg2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 29,  29, BFRefChCmdMgtDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 28, 28, BFFastSelfRefEntryDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 27, 27, BFCSMux67);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 26, 26, BFCSMux45);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 25, 24, BFWrDqDqsEarly);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 22, 22, BFPrtlChPDEnhEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 21, 21, BFAggrPDEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 15,  8, BFCtrlWordCS);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  7,  7, BFLrDimmMrsCtrl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  6,  6, BFLrDimmErrOutMonEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  5,  5, BFSubMemclkRegDly);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  4,  4, BFExtendedParityEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  3,  3, BFLrDimmEnhRefEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  2,  2, BFCSTimingMux67);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xC0),  0,  0, BFTraceModeEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x10C),  0,  0, BFIntLvRgnSwapEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x10C),  9,  3, BFIntLvRgnBaseAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x10C),  17, 11, BFIntLvRgnLmtAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x10C),  26, 20, BFIntLvRgnSize);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110), 31, 11, BFDctSelBaseAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110), 10, 10, BFMemCleared);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  9,  9, BFMemClrBusy);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  8,  8, BFDramEnabled);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  7,  6, BFDctSelIntLvAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  5,  5, BFDctDatIntLv);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  3,  3, BFMemClrInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  2,  2, BFDctSelIntLvEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  1,  1, BFDctSelHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  0,  0, BFDctSelHiRngEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x114), 31, 10, BFDctSelBaseOffset);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x118), 31,  0, BFMctCfgLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x118), 19, 19, BFLockDramCfg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x118), 18, 18, BFCC6SaveEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 31,  0, BFMctCfgHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 30, 30, BFFlushWr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 29, 29, BFFlushWrOnStpGnt);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 28, 28, BFPrefDramTrainMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 13, 13, BFPrefIoDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 12, 12, BFPrefCpuDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 11,  7, BFMctPrefReqLimit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C),  6,  2, BFMctWrLimit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C),  1,  0, BFDctWrLimit);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0), 31,  0, BFExtMctCfgLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0), 10,  8, BFCohPrefPrbLmt);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0),  5,  4, BFAdapPrefNegStep);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0),  3,  2, BFAdapPrefPosStep);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0),  1,  0, BFAdapPrefMissRatio);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B4), 31,  0, BFExtMctCfgHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B4), 27, 27, BFFlushWrOnS3StpGnt);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x200), 29, 24, BFTras);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x200), 20, 16, BFTrp);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x200), 12,  8, BFTrcd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x200),  4,  0, BFTcl);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x204), 27, 24, BFTrtp);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x204), 21, 16, BFFourActWindow);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x204), 11,  8, BFTrrd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x204),  5,  0, BFTrc);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x208), 26, 24, BFTrfc3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x208), 18, 16, BFTrfc2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x208), 10,  8, BFTrfc1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x208),  2,  0, BFTrfc0);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x20C), 11,  8, BFTwtr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x20C),  4,  0, BFTcwl);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x210), 31, 22, BFMaxLatency);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x210), 18, 16, BFDataTxFifoWrDly);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x210),  3,  0, BFRdPtrInit);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x214), 19, 16, BFTwrwrSdSc);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x214), 11,  8, BFTwrwrSdDc);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x214),  3,  0, BFTwrwrDd);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x218), 27, 24, BFTrdrdSdSc);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x218), 19, 16, BFTrdrdSdDc);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x218), 11,  8, BFTwrrd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x218),  3,  0, BFTrdrdDd);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x21C), 20, 16, BFTrwtWB);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x21C), 12,  8, BFTrwtTO);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x220), 12,  8, BFTmod);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x220),  3,  0, BFTmrd);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x224), 10,  8, BFTzqcs);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x224),  3,  0, BFTzqoper);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x228), 31, 24, BFTstag3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x228), 23, 16, BFTstag2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x228), 15,  8, BFTstag1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x228),  7,  0, BFTstag0);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x22C), 4,  0, BFTwrDDR3);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x230), 31, 0, BFPhyRODTCSLow);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x234), 31, 0, BFPhyRODTCSHigh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x238), 31, 0, BFPhyWODTCSLow);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x23C), 31, 0, BFPhyWODTCSHigh);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x240), 14, 12, BFWrOdtOnDuration);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x240), 10,  8, BFWrOdtTrnOnDly);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x240),  7,  4, BFRdOdtOnDuration);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x240),  3,  0, BFRdOdtTrnOnDly);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x244),  3,  0, BFPrtlChPDDynDly);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x248),  29, 24, BFAggrPDDelay);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x248),  21, 16, BFPchgPDEnDelay);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x248),  12,  8, BFTxpdll);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x248),   3,  0, BFTxp);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x24C),  31,  0, BFDramPwrMngm1Reg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 12,  12, BFCmdSendInProg );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 11,  11, BFSendCmd );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 10,  10, BFTestStatus );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  9,   8, BFCmdTgt );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  7,   5, BFCmdType );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  4,   4, BFStopOnErr );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  3,   3, BFResetAllErr );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  2,   2, BFCmdTestEnable );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  13,   13, BFReserved003 );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  7,  3, BFReserved004 );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x254),  26,   24, BFTgtChipSelectA );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x254),  23,   21, BFTgtBankA );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x254),   9,    0, BFTgtAddressA );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x258),  26,   24, BFTgtChipSelectB );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x258),  23,   21, BFTgtBankB );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x258),   9,    0, BFTgtAddressB );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x25C),  31, 22, BFReserved008 );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x25C),  21, 12, BFReserved007 );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x25C),   7,  0, BFReserved006 );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x25C),   31,  0, BFReserved005 );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x260), 20,  0, BFCmdCount );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x264), 31,  25, BFErrDqNum );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x264), 24,  0, BFErrCnt );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x268), 17,  0, BFNibbleErrSts );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x26C), 17,  0, BFNibbleErr180Sts );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x270), 18,   0, BFDataPrbsSeed );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x274), 31,   0, BFDramDqMaskLow );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x278), 31,   0, BFDramDqMaskHigh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x27C),  7,   0, BFDramEccMask );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 31, 31, BFSendActCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 30, 30, BFSendPchgCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 29, 22, BFCmdChipSelect );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 21, 19, BFCmdBank );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 17,  0, BFCmdAddress );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 31,  0, BFDramCommand2 );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x290), 26,  24, BFErrBeatNum );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x290), 20,   0, BFErrCmdNum );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x294), 31,  0, BFDQErrLow );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x298), 31,  0, BFDQErrHigh );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x29C), 7,  0, BFEccErr );

  // ---------------------------------------------------------------------------
  //
  // DCT PHY REGISTERS
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 31,  0, BFODCControl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  2,  0, BFCkeDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  6,  4, BFCsOdtDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 10,  8, BFAddrCmdDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 14, 12, BFClkDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 18, 16, BFDataDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 22, 20, BFDqsDrvStren);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x04, 31,  0, BFAddrTmgControl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 31,  0, BFDramPhyCtlReg);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 13, 13, BFDqsRcvTrEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 12, 12, BFWrLvOdtEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 11,  8, BFWrLvOdt);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  7,  6, BFFenceTrSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  5,  4, BFTrDimmSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  3,  3, BFPhyFenceTrEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  2,  2, BFTrNibbleSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  0,  0, BFWrtLvTrEn);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0B, 31,  0, BFDramPhyStatusReg);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C, 20, 16, BFPhyFence);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C, 13, 12, BFCKETri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C, 11,  8, BFODTTri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C,  7,  0, BFChipSelTri);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 31,  0, BFDRAMPhyDLLControl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 25, 24, BFRxDLLWakeupTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 22, 20, BFRxCPUpdPeriod);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 19, 16, BFRxMaxDurDllNoLock);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  9,  8, BFTxDLLWakeupTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  6,  4, BFTxCPUpdPeriod);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  3,  0, BFTxMaxDurDllNoLock);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x50, 31,  0, BFRstRcvFifo);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x53,  8,  0, BFWrtLvErr);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F13, 14, 14, BFProcOdtAdv);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F13,  7,  0, BFPhy0x0D0F0F13);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0830,  4,  4, BFEccDLLPwrDnConf);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE013, 15,  0, BFPllRegWaitTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE006, 15,  0, BFPllLockTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F04, 13, 13, BFTriDM);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F0F, 14, 12, BFAlwaysEnDllClks);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F02, 15,  0, BFDataByteTxPreDriverCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F06, 11,  0, BFDataByteTxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F0A, 11,  0, BFDataByteTxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8006, 11,  0, BFCmdAddr0TxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F800A, 11,  0, BFCmdAddr0TxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8106, 11,  0, BFCmdAddr1TxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F810A, 11,  0, BFCmdAddr1TxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC006, 11,  0, BFAddrTxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC00A, 11,  0, BFAddrTxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC00E, 11,  0, BFAddrTxPreDriverCal2Pad3);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC012, 11,  0, BFAddrTxPreDriverCal2Pad4);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8002, 15,  15, BFCmdAddr0TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8102, 15,  15, BFCmdAddr1TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC002, 15,  15, BFAddrTxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2002, 15,  15, BFClock0TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2102, 15,  15, BFClock1TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2202, 15,  15, BFClock2TxPreDriverCalPad0);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F812F, 7,  0, BFAddrCmdTri);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F10, 12, 12, BFEnRxPadStandby);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE003, 14, 13, BFDisablePredriverCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE00A,  4,  4, BFSkewMemClk);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2030,  4,  4, BFPhyClkConfig0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2130,  4,  4, BFPhyClkConfig1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2230,  4,  4, BFPhyClkConfig2);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC000,  8,  8, BFReserved00C);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F1F,  4,  3, BFDataRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2F1F,  4,  3, BFClkRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4009,  3,  2, BFCsrComparator);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4009, 15, 14, BFCmpVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8F1F,  4,  3, BFCmdRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC01F,  4,  3, BFAddrRxVioLvl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F31, 14,  0, BFDataFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2F31,  4,  0, BFClkFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8F31,  4,  0, BFCmdFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC031,  4,  0, BFAddrFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F30,  8,  8, BFBlockRxDqsLock);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F04, 13, 13, BFDataByteDMConf);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4007, 1, 0, BFReserved8_0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4007, 6, 2, BFReserved8_1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0020, 4, 0, BFReserved4_0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0020, 12, 8, BFReserved4_1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0120, 4, 0, BFReserved4_2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0120, 12, 8, BFReserved4_3);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0220, 4, 0, BFReserved4_4);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0220, 12, 8, BFReserved4_5);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0320, 4, 0, BFReserved4_6);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0320, 12, 8, BFReserved4_7);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0420, 4, 0, BFReserved4_8);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0420, 12, 8, BFReserved4_9);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0520, 4, 0, BFReserved4_A);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0520, 12, 8, BFReserved4_B);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0620,  4, 0, BFReserved4_C);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0620, 12, 8, BFReserved4_D);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0720, 4, 0, BFReserved4_E);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0720, 12, 8, BFReserved4_F);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0820, 4, 0, BFReserved4_10);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0820, 12, 8, BFReserved4_11);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F20, 15, 0, BFReserved4_12);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0021, 4, 0, BFReserved5_0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0021, 12, 8, BFReserved5_1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0121, 4, 0, BFReserved5_2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0121, 12, 8, BFReserved5_3);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0221, 4, 0, BFReserved5_4);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0221, 12, 8, BFReserved5_5);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0321, 4, 0, BFReserved5_6);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0321, 12, 8, BFReserved5_7);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0421, 4, 0, BFReserved5_8);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0421, 12, 8, BFReserved5_9);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0521, 4, 0, BFReserved5_A);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0521, 12, 8, BFReserved5_B);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0621, 4, 0, BFReserved5_C);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0621, 12, 8, BFReserved5_D);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0721, 4, 0, BFReserved5_E);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0721, 12, 8, BFReserved5_F);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0821, 4, 0, BFReserved5_10);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0821, 12, 8, BFReserved5_11);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F21, 15, 0, BFReserved5_12);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0022, 4, 0, BFReserved6_0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0022, 12, 8, BFReserved6_1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0122, 4, 0, BFReserved6_2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0122, 12, 8, BFReserved6_3);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0222, 4, 0, BFReserved6_4);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0222, 12, 8, BFReserved6_5);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0322, 4, 0, BFReserved6_6);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0322, 12, 8, BFReserved6_7);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0422, 4, 0, BFReserved6_8);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0422, 12, 8, BFReserved6_9);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0522, 4, 0, BFReserved6_A);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0522, 12, 8, BFReserved6_B);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0622, 4, 0, BFReserved6_C);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0622, 12, 8, BFReserved6_D);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0722, 4, 0, BFReserved6_E);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0722, 12, 8, BFReserved6_F);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0822, 4, 0, BFReserved6_10);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0822, 12, 8, BFReserved6_11);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F22, 15, 0, BFReserved6_12);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0023, 4, 0, BFReserved7_0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0023, 12, 8, BFReserved7_1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0123, 4, 0, BFReserved7_2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0123, 12, 8, BFReserved7_3);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0223, 4, 0, BFReserved7_4);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0223, 12, 8, BFReserved7_5);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0323, 4, 0, BFReserved7_6);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0323, 12, 8, BFReserved7_7);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0423, 4, 0, BFReserved7_8);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0423, 12, 8, BFReserved7_9);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0523, 4, 0, BFReserved7_A);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0523, 12, 8, BFReserved7_B);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0623, 4, 0, BFReserved7_C);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0623, 12, 8, BFReserved7_D);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0723, 4, 0, BFReserved7_E);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0723, 12, 8, BFReserved7_F);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0823, 4, 0, BFReserved7_10);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0823, 12, 8, BFReserved7_11);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F23, 15, 0, BFReserved7_12);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F3E, 0, 0, BFReserved00A);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4010, 31, 0, BFReserved009);

  // ---------------------------------------------------------------------------
  //
  // FUNCTION 3
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x40), 31,  0, BFMcaNbCtlReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x44), 22, 22, BFDramEccEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x44),  2,  2, BFSyncOnUcEccEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x180), 25, 25, BFEccSymbolSize);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x48), 31,  0, BFMcaNbStatusLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x4C), 31,  0, BFMcaNbStatusHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x58),  4,  0, BFDramScrub);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x58), 28, 24, BFL3Scrub);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x58), 29, 29, BFMultiNodeCpu);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x5C),  0,  0, BFScrubReDirEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x5C), 31,  0, BFScrubAddrLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x60), 31,  0, BFScrubAddrHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x8C),  4,  4, BFDisDatMsk);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xB0), 31,  0, BFOnLineSpareControl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xD4), 13, 13, BFMTC1eEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xE8), 25, 25, BFL3Capable);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x188), 8,  8, BFReserved00B);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x1B8), 4,  4, BFL3ScrbRedirDis);

  // ---------------------------------------------------------------------------
  //
  // FUNCTION 4
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (4, 0x128), 17, 12, BFCoreStateSaveDestNode);

  // ---------------------------------------------------------------------------
  //
  // FUNCTION 5
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x84),  20, 16, BFDdrMaxRate);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170), 31,  0, BFNbPstateCtlReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170), 14, 14, BFSwNbPstateLoDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170),  7,  6, BFNbPstateHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170),  4,  3, BFNbPstateLo);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170),  1,  0, BFNbPstateMaxVal);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x174), 20, 19, BFCurNbPstate);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x174),  0,  0, BFNbPstateDis);  ///< Orochi Read Only

  IDS_OPTION_HOOK (IDS_INIT_MEM_REG_TABLE, NBPtr, &NBPtr->MemPtr->StdHeader);
}
