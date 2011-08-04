/* $NoKeywords:$ */
/**
 * @file
 *
 * mnregon.c
 *
 * Common Northbridge register related functions for ON
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/ON)
 * @e \$Revision: 48511 $ @e \$Date: 2011-03-09 13:53:13 -0700 (Wed, 09 Mar 2011) $
 *
 **/
/*
 *****************************************************************************
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
#include "mnon.h"
#include "merrhdl.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_ON_MNREGON_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define PHY_DIRECT_ADDRESS_MASK   0x0D000000

STATIC CONST UINT8 InstancesPerTypeON[8] = {8, 2, 1, 0, 2, 0, 1, 1};

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
 *
 *      This function matches the CPU_LOGICAL_ID with certain criteria to
 *      determine if it is supported by this NBBlock.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *LogicalIdPtr - Pointer to the CPU_LOGICAL_ID
 *
 *     @return          TRUE -  This node is a Ontario.
 *     @return          FALSE - This node is not a Ontario.
 */
BOOLEAN
MemNIsIdSupportedON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  )
{
  if (((LogicalIdPtr->Family & AMD_FAMILY_14_ON) != 0)
      && ((LogicalIdPtr->Revision & AMD_F14_ALL) != 0)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Check if bitfields of all enabled DCTs on a die have the expected value. Ignore
 *  DCTs that are disabled.
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Bit Field name
 *     @param[in]   Field - Value to be checked
 *
 *     @return          TRUE -  All enabled DCTs have the expected value on the bitfield.
 *     @return          FALSE - Not all enabled DCTs have the expected value on the bitfield.
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNBrdcstCheckON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  )
{
  if (MemNGetBitFieldNb (NBPtr, FieldName) != Field) {
    return FALSE;
  }
  return TRUE;
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
MemNCmnGetSetFieldON (
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
            NumOfInstances = InstancesPerTypeON[(Address >> 13) & 0x7];
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
          if ((FieldName != BFDctAddlDataReg) && (FieldName != BFDctAddlOffsetReg) &&
              (FieldName != BFDctExtraDataReg) && (FieldName != BFDctExtraOffsetReg)) {
            IDS_HDT_CONSOLE (MEM_GETREG, "~Fn%d_%03x = %x\n", (Address >> 12) & 0xF, Address & 0xFFF, Value);
          }
        } else if (Type == DCT_PHY_ACCESS) {
          if (IsPhyDirectAccess && (NumOfInstances > 1)) {
            Address = (Address & 0x0FFFF0FF) | (((UINT32) Instance) << 8);
          }
          MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
          MemNPollBitFieldNb (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
          Value = MemNGetBitFieldNb (NBPtr, BFDctAddlDataReg);
          IDS_HDT_CONSOLE (MEM_GETREG, "~Fn2_%d9C_%x = %x\n", NBPtr->Dct, Address & 0x0FFFFFFF, Value);
        } else if (Type == DCT_EXTRA) {
          MemNSetBitFieldNb (NBPtr, BFDctExtraOffsetReg, Address);
          MemNPollBitFieldNb (NBPtr, BFDctExtraAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
          Value = MemNGetBitFieldNb (NBPtr, BFDctExtraDataReg);
          IDS_HDT_CONSOLE (MEM_GETREG, "~Fn2_%dF4_%x = %x\n", NBPtr->Dct, Address & 0x0FFFFFFF, Value);
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
          } else if (Type == DCT_EXTRA) {
            MemNSetBitFieldNb (NBPtr, BFDctExtraDataReg, Value);
            Address |= DCT_ACCESS_WRITE;
            MemNSetBitFieldNb (NBPtr, BFDctExtraOffsetReg, Address);
            MemNPollBitFieldNb (NBPtr, BFDctExtraAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
            IDS_HDT_CONSOLE (MEM_SETREG, "~Fn2_%dF4_%x [%d:%d] = %x\n", NBPtr->Dct, Address & 0x0FFFFFFF, Highbit, Lowbit, Field);
          } else {
            IDS_ERROR_TRAP;
          }
          if (IsLinked) {
            MemNCmnGetSetFieldON (NBPtr, 1, FieldName + 1, Field >> (Highbit - Lowbit + 1));
          }
        } else {
          Value = Value >> Lowbit;  // Shift
          // A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case
          if ((Highbit - Lowbit) != 31) {
            Value &= (((UINT32)1 << (Highbit - Lowbit + 1)) - 1);
          }
          if (IsLinked) {
            Value |= MemNCmnGetSetFieldON (NBPtr, 0, FieldName + 1, 0) << (Highbit - Lowbit + 1);
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
 *   This function initializes bit field translation table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   NBRegTable[]   - Pointer to the bit field data structure
 *
 */

VOID
MemNInitNBRegTableON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  )
{
  UINT16 i;
  for (i = 0; i < BFEndOfList; i++) {
    NBRegTable[i] = 0;
  }

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (0, 0x00), 31,  0, BFDevVendorIDReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x40), 31, 0, BFDramBaseReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x44), 31, 0, BFDramLimitReg0);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0), 31, 24, BFDramHoleBase);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0), 15,  7, BFDramHoleOffset);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0),  0,  0, BFDramHoleValid);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0), 31,  0, BFDramHoleAddrReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x40), 31,  0, BFCSBaseAddr0Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x44), 31,  0, BFCSBaseAddr1Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x48), 31,  0, BFCSBaseAddr2Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x4C), 31,  0, BFCSBaseAddr3Reg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x60), 31,  0, BFCSMask0Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x64), 31,  0, BFCSMask1Reg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 31,  0, BFDramControlReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31,  0, BFDramInitRegReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x80), 31,  0, BFDramBankAddrReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 31,  0, BFDramMRSReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 31,  0, BFDramTimingLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 31,  0, BFDramTimingHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 31,  0, BFDramConfigLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31,  0, BFDramConfigHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31,  0, BFDctAddlOffsetReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31, 31, BFDctAccessDone);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x9C), 31,  0, BFDctAddlDataReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA0), 31,  0, BFDramConfigMiscReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 31,  0, BFDramCtrlMiscReg2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xF0), 31,  0, BFDctExtraOffsetReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xF0), 31, 31, BFDctExtraAccessDone);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xF4), 31,  0, BFDctExtraDataReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 31,  0, BFMctCfgHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x118), 31,  0, BFMctCfgLoReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 13,  8, BFNonSPDHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78),  3,  0, BFRdPtrInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78),  6,  6, BFRxPtrInitReq);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78),  9,  8, BFTwrrdHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 11, 10, BFTwrwrHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 13, 12, BFTrdrdHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 17, 17, BFAddrCmdTriEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 20, 20, BFForceCasToSlot0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 21, 21, BFDisCutThroughMode);
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
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31, 31, BFEnDramInit);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  1,  0, BFBurstCtrl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  3,  2, BFDrvImpCtrl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  6,  4, BFTwrDDR3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 22, 20, BFTcwl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 23, 23, BFPchgPDModeSel);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88),  3,  0, BFTcl);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 31, 24, BFMemClkDis);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 15,  0, BFNonSPD);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C),  3,  0, BFTrwtWB);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C),  7,  4, BFTrwtTO);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 11, 10, BFTwrrd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 13, 12, BFTwrwr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 15, 14, BFTrdrd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 17, 16, BFTref);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 18, 18, BFDisAutoRefresh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 22, 20, BFTrfc0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 25, 23, BFTrfc1);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  0,  0, BFInitDram);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  1,  1, BFExitSelfRef);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 17, 17, BFEnterSelfRef);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 22, 21, BFIdleCycInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 25, 25, BFEnDispAutoPrecharge);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 26, 26, BFDbeSkidBufDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 27, 27, BFDisDllShutdownSR);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  4,  0, BFMemClkFreq);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  7,  7, BFMemClkFreqVal);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 11, 10, BFZqcsInterval);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 14, 14, BFDisDramInterface);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 15, 15, BFPowerDownEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 16, 16, BFPowerDownMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 20, 20, BFSlowAccessMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 22, 22, BFBankSwizzleMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 27, 24, BFDcqBypassMax);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31, 28, BFFourActWindow);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4),  0,  0, BFDoubleTrefRateEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4),  2,  1, BFThrottleEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 22, 21, BFDbeGskMemClkAlignMode);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xAC), 0, 0, BFMemTempHot);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xC0),  0,  0, BFTraceModeEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  3,  3, BFMemClrInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  8,  8, BFDramEnabled);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  9,  9, BFMemClrBusy);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110), 10, 10, BFMemCleared);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x114),  9,  9, BFDctSelBankSwap);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x118), 19, 19, BFC6DramLock);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 12, 12, BFPrefCpuDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C),  6,  2, BFDctWrLimit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0), 23, 23, BFRdTrainGo);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0), 22, 22, BFRdDramTrainMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0), 20, 20, BFDramTrainPdbDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0), 17,  2, BFTrainLength);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0),  1,  1, BFWrTrainGo);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0),  0,  0, BFWrDramTrainMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C8), 31,  0, BFWrTrainAdrPtrLo);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1CC), 17, 16, BFWrTrainAdrPtrHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1D0),  9,  0, BFWrTrainBufAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1D4), 31,  0, BFWrTrainBufDat);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1E8), 15,  8, BFTrainCmpSts2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1E8),  7,  0, BFTrainCmpSts);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xDC), 19, 19, BFNclkFreqDone);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xD4),  5,  0, BFMainPllOpFreqId);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xDC), 26, 20, BFNbPs0NclkDiv);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xE8),  7, 5, BFDdrMaxRate);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x188), 22, 22, BFEnCpuSerRdBehindNpIoWr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (6, 0x90),  6,  0, BFNbPs1NclkDiv);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (6, 0x90), 28, 28, BFNbPsForceReq);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (6, 0x90), 30, 30, BFNbPsCtrlDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (6, 0x98), 30, 30, BFNbPsCsrAccSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (6, 0x98), 31, 31, BFNbPsDbgEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (4, 0x12C), 11,  0, BFC6Base);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (4, 0x164),  0,  0, BFFixedErrataSkipPorFreqCap);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (4, 0x1A8), 29, 29, BFDramSrHysEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (4, 0x1A8), 28, 26, BFDramSrHys);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (4, 0x1A8), 25, 25, BFMemTriStateEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (4, 0x1A8), 24, 24, BFDramSrEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x84), 31,  0, BFAcpiPwrStsCtrlHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x1FC), 2, 2,  BFLowPowerDefault);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  2,  0, BFCkeDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  6,  4, BFCsOdtDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 10,  8, BFAddrCmdDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 14, 12, BFClkDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 18, 16, BFDataDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 22, 20, BFDqsDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 30, 28, BFProcOdt);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 31,  0, BFODCControl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x04, 31,  0, BFAddrTmgControl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  0,  0, BFWrtLvTrEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  1,  1, BFWrtLvTrMode);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  3,  3, BFPhyFenceTrEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  4,  4, BFTrDimmSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  7,  6, BFFenceTrSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 11,  8, BFWrLvOdt);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 12, 12, BFWrLvOdtEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 13, 13, BFDqsRcvTrEn);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0B, 31,  0, BFDramPhyStatusReg);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0C, 20, 16, BFPhyFence);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0C, 13, 12, BFCKETri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0C, 11,  8, BFODTTri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0C,  7,  0, BFChipSelTri);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 25, 24, BFRxDLLWakeupTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 22, 20, BFRxCPUpdPeriod);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 19, 16, BFRxMaxDurDllNoLock);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  9,  8, BFTxDLLWakeupTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  6,  4, BFTxCPUpdPeriod);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  3,  0, BFTxMaxDurDllNoLock);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F10, 12, 12, BFEnRxPadStandby);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F13,  7,  0, BFPhy0x0D0F0F13Bit0to7);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE003, 14, 13, BFDisablePredriverCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE006, 15,  0, BFPllLockTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2030,  4,  4, BFPhyClkConfig0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2130,  4,  4, BFPhyClkConfig1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC000,  8,  8, BFLowPowerDrvStrengthEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F812F, 15,  0, BFAddrCmdTri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F0F, 14, 12, BFAlwaysEnDllClks);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F31, 14,  0, BFDataFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2F31,  4,  0, BFClkFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8F31,  4,  0, BFCmdFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC031,  4,  0, BFAddrFence2);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F02, 15,  0, BFDataByteTxPreDriverCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F06, 15,  0, BFDataByteTxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F0A, 15,  0, BFDataByteTxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8006, 15,  0, BFCmdAddr0TxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F800A, 15,  0, BFCmdAddr0TxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8106, 15,  0, BFCmdAddr1TxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F810A, 15,  0, BFCmdAddr1TxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC006, 15,  0, BFAddrTxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC00A, 15,  0, BFAddrTxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC00E, 15,  0, BFAddrTxPreDriverCal2Pad3);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC012, 15,  0, BFAddrTxPreDriverCal2Pad4);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8002, 15,  0, BFCmdAddr0TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8102, 15,  0, BFCmdAddr1TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC002, 15,  0, BFAddrTxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2002, 15,  0, BFClock0TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2102, 15,  0, BFClock1TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F1C00, 15,  0, BFPNOdtCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F1D00, 15,  0, BFPNDrvCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D081E00, 15,  0, BFCalVal);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F1F,  4,  3, BFDataRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2F1F,  4,  3, BFClkRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4009, 15, 14, BFCmpVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8F1F,  4,  3, BFCmdRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC01F,  4,  3, BFAddrRxVioLvl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F00,  6,  4, BFDQOdt03);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F08,  6,  4, BFDQOdt47);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F1E, 14, 12, BFDllCSRBisaTrimDByte);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2F1E, 14, 12, BFDllCSRBisaTrimClk);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8F1E, 14, 12, BFDllCSRBisaTrimCsOdt);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FCF1E, 14, 12, BFDllCSRBisaTrimAByte2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F38, 14, 13, BFReduceLoop);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x06, 11,  8, BFTwrrdSD);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x06,  3,  0, BFTrdrdSD);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x16,  3,  0, BFTwrwrSD);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x30, 12,  0, BFDbeGskFifoNumerator);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x31, 12,  0, BFDbeGskFifoDenominator);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32,  4,  0, BFDataTxFifoSchedDlySlot0);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32,  7,  7, BFDataTxFifoSchedDlyNegSlot0);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32, 12,  8, BFDataTxFifoSchedDlySlot1);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32, 15, 15, BFDataTxFifoSchedDlyNegSlot1);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x40,  3,  0, BFTrcd);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x40, 11,  8, BFTrp);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x40, 20, 16, BFTras);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x40, 29, 24, BFTrc);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x41,  2,  0, BFTrtp);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x41, 10,  8, BFTrrd);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x41, 18, 16, BFTwtr);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x83,  2,  0, BFRdOdtTrnOnDly);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x83,  6,  4, BFRdOdtOnDuration);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x83,  8,  8, BFWrOdtTrnOnDly);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x83, 14, 12, BFWrOdtOnDuration);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x180, 31, 0, BFRdOdtPatReg);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x182, 31, 0, BFWrOdtPatReg);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x200,  3, 0, BFTxp);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x200, 12, 8, BFTxpdll);

  LINK_TSEFO (NBRegTable, BFTwrrd, BFTwrrdHi);
  LINK_TSEFO (NBRegTable, BFTwrwr, BFTwrwrHi);
  LINK_TSEFO (NBRegTable, BFTrdrd, BFTrdrdHi);
}
