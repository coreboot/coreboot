/* $NoKeywords:$ */
/**
 * @file
 *
 * mnregkb.c
 *
 * Common Northbridge register related functions for KB
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/KB)
 * @e \$Revision: 87696 $ @e \$Date: 2013-02-07 12:35:03 -0600 (Thu, 07 Feb 2013) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "Gnb.h"
#include "GnbPcie.h"
#include "GnbCommonLib.h"
#include "GnbRegistersKB.h"
#include "GnbRegisterAccKB.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mnkb.h"
#include "merrhdl.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_KB_MNREGKB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define PHY_DIRECT_ADDRESS_MASK   0x0D000000ul

STATIC CONST UINT8 InstancesPerTypeKB[8] = {9, 3, 1, 2, 2, 0, 1, 1};

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
 *     MemNIsIdSupportedKB
 *      This function matches the CPU_LOGICAL_ID with certain criteria to
 *      determine if it is supported by this NBBlock.
 *
 *     @param[in]       *LogicalIdPtr - Pointer to the CPU_LOGICAL_ID
 *
 *     @return          TRUE -  This node is a KB.
 *     @return          FALSE - This node is not a KB.
 *
 */
BOOLEAN
MemNIsIdSupportedKB (
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  )
{
  if (((LogicalIdPtr->Family & AMD_FAMILY_16_KB) != 0)
      && ((LogicalIdPtr->Revision & AMD_F16_ALL) != 0)) {
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
MemNCmnGetSetFieldKB (
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
  UINT8  IsMultipleMPstate;

  Value = 0;
  if (FieldName == BFDctAccessDone) {
    // No need to poll DctAccessDone for KB due to enhancement in phy
    Value = 1;
  } else if ((FieldName < BFEndOfList) && (FieldName >= 0)) {
    Address = NBPtr->NBRegTable[FieldName];
    if (Address) {
      Lowbit = TSEFO_END (Address);
      Highbit = TSEFO_START (Address);
      Type = (UINT8) TSEFO_TYPE (Address);
      IsLinked = (UINT8) TSEFO_LINKED (Address);
      IsPhyDirectAccess = (UINT8) TSEFO_DIRECT_EN (Address);
      IsWholeRegAccess = (UINT8) TSEFO_WHOLE_REG_ACCESS (Address);
      if (NBPtr->MemPstate == MEMORY_PSTATE0) {
        IsMultipleMPstate = TSEFO_MULTI_MPSTATE_COPY (Address);
      } else {
        // Do not write in both instances when context is already in MP1
        IsMultipleMPstate = 0;
      }

      Address = TSEFO_OFFSET (Address);

      // By default, a bit field has only one instance
      NumOfInstances = 1;

      if ((Type == DCT_PHY_ACCESS) && IsPhyDirectAccess) {
        Address |= PHY_DIRECT_ADDRESS_MASK;
        if (IsWholeRegAccess) {
          // In the case of whole register access (bit 0 to 15),
          // HW broadcast and nibble mask will be used.
          Address |= Lowbit << 16;
          Lowbit = 0;
          Highbit = 15;
        } else {
          // In the case only some bits on a register is accessed,
          // BIOS will do read-mod-write to all chiplets manually.
          // And nibble mask will be 1111b always.
          if ((Address & 0xFFFF) == 0xE008) {
          // Special case for PStateToAccess, which use nibble mask
            Address |= 0x00040000;
          } else {
            Address |= 0x000F0000;
          }
          Field >>= Lowbit;
          if ((Address & 0x0F00) == 0x0F00) {
            // Broadcast mode
            // Find out how many instances to write to
            NumOfInstances = InstancesPerTypeKB[(Address >> 13) & 0x7];
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
          Value = MemNGetBitFieldNb (NBPtr, BFDctAddlDataReg);
          IDS_HDT_CONSOLE (MEM_GETREG, "~Dev%x Dct%d Fn2_9C_%x = %x\n", NBPtr->PciAddr.Address.Device, NBPtr->Dct, Address & 0x0FFFFFFF, Value);
        } else {
          // DCT_EXTRA_ACCESS is not supported on KB
          ASSERT (FALSE);
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

          do {
            if (Type == NB_ACCESS) {
              PciAddr.AddressValue = Address;
              LibAmdPciWrite (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
              if ((FieldName != BFDctAddlDataReg) && (FieldName != BFDctAddlOffsetReg) && (FieldName != BFDctCfgSel)) {
                IDS_HDT_CONSOLE (MEM_SETREG, "~Dev%x Dct%d Fn%d_%03x [%d:%d] = %x\n",
                                 NBPtr->PciAddr.Address.Device, NBPtr->Dct,
                                 (Address >> 12) & 0xF, Address & 0xFFF, Highbit, Lowbit, Field);
              }
            } else if (Type == DCT_PHY_ACCESS) {
              ASSERT (!NBPtr->IsSupported[ScrubberEn]);   // Phy CSR write is not allowed after scrubber is enabled
              MemNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
              Address |= DCT_ACCESS_WRITE;
              MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
              IDS_HDT_CONSOLE (MEM_SETREG, "~Dev%x Dct%d Fn2_9C_%x [%d:%d] = %x\n",
                               NBPtr->PciAddr.Address.Device, NBPtr->Dct,
                               Address & 0x0FFFFFFF, Highbit, Lowbit, Field);
            } else {
              // DCT_EXTRA_ACCESS is not supported on KB
              ASSERT (FALSE);
            }
            if (IsLinked) {
              MemNCmnGetSetFieldKB (NBPtr, 1, FieldName + 1, Field >> (Highbit - Lowbit + 1));
            }
            if (IsMultipleMPstate && (NBPtr->MemPstateStage == MEMORY_PSTATE_1ST_STAGE)) {
              // if there are multiple Pstate register copies, program register in M1 context when the frequency is DDR667
              if (NBPtr->MemPstate == MEMORY_PSTATE0) {
                MemNChangeMemPStateContextNb (NBPtr, 1);
              } else {
                // Switch back to M0 context
                MemNChangeMemPStateContextNb (NBPtr, 0);
              }
            }
          } while ((NBPtr->MemPstate == MEMORY_PSTATE1) && IsMultipleMPstate && (NBPtr->MemPstateStage == MEMORY_PSTATE_1ST_STAGE));
        } else {
          Value = Value >> Lowbit;  // Shift
          // A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case
          if ((Highbit - Lowbit) != 31) {
            Value &= (((UINT32)1 << (Highbit - Lowbit + 1)) - 1);
          }
          if (IsLinked) {
            Value |= MemNCmnGetSetFieldKB (NBPtr, 0, FieldName + 1, 0) << (Highbit - Lowbit + 1);
          }
          // For direct phy access, shift the bit back for compatibility reason.
          if ((Type == DCT_PHY_ACCESS) && IsPhyDirectAccess) {
            Value <<= Lowbit;
          }
        }
      }
    } else {
      IDS_HDT_CONSOLE (MEM_UNDEF_BF, "\t\tUndefined BF enum: %x\n", FieldName);
    }
  } else {
    ASSERT (FALSE);   // Invalid bit field index
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
MemNInitNBRegTableKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  )
{
  UINT16 i;

  // Allocate heap for NB register table
  if (!MemNAllocateNBRegTableNb (NBPtr, NbRegTabKB)) {
    return;     // escape if fails
  }
  NBRegTable = NBPtr->NBRegTable;

  for (i = 0; i < BFEndOfList; i++) {
    NBRegTable[i] = 0;
  }

  // ---------------------------------------------------------------------------
  //
  // FUNCTION 1
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x40), 31, 0, BFDramBaseReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x44), 31, 0, BFDramLimitReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0), 31, 0, BFDramHoleAddrReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0), 31, 24, BFDramHoleBase);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0), 15,  7, BFDramHoleOffset);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0),  1,  1, BFDramMemHoistValid);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0xF0),  0,  0, BFDramHoleValid);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x10C),  5,  4, BFNbPsSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x10C),  3,  3, BFMemPsSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x10C),  2,  0, BFDctCfgSel);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x120), 20,  0, BFDramBaseAddr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x124), 20,  0, BFDramLimitAddr);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x200), 31,  0, BFDramCtrlBaseReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x200),  1,  1, BFLgcyMmioHoleEn0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x204), 31,  0, BFDramCtrlLimitReg0);
  // ---------------------------------------------------------------------------
  //
  // FUNCTION 2
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x40), 31,  0, BFCSBaseAddr0Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x44), 31,  0, BFCSBaseAddr1Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x48), 31,  0, BFCSBaseAddr2Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x4C), 31,  0, BFCSBaseAddr3Reg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x60), 31,  0, BFCSMask0Reg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x64), 31,  0, BFCSMask1Reg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 17, 17, BFAddrCmdTriEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31,  0, BFDramInitRegReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31, 31, BFEnDramInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 29, 29, BFSendZQCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 28, 28, BFAssertCke);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 27, 27, BFDeassertMemRstX);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 26, 26, BFSendMrsCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 23, 21, BFMrsChipSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 20, 18, BFMrsBank);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 17,  0, BFMrsAddress);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x80), 7,  0, BFDramBankAddrReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 31,  0, BFDramMRSReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 23, 23, BFPchgPDModeSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  1,  0, BFBurstCtrl);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 29, 24, BFMemClkDis);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 18, 18, BFDisAutoRefresh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 17, 16, BFTref);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 27, 27, BFDisDllShutdownSR);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 25, 25, BFPendRefPaybackS3En);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 24, 24, BFStagRefEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 23, 23, BFForceAutoPchg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 20, 20, BFDynPageCloseEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 19, 19, BFDimmEccEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 17, 17, BFEnterSelfRef);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 16, 16, BFUnBuffDimm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  1,  1, BFExitSelfRef);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31,  0, BFDramConfigHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31, 31, BFDphyMemPsSelEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 28, 24, BFDcqBypassMax);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 22, 22, BFBankSwizzleMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 21, 21, BFFreqChgInProg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 20, 20, BFSlowAccessMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 16, 16, BFPowerDownMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 15, 15, BFPowerDownEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 14, 14, BFDisDramInterface);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 11, 10, BFZqcsInterval);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  7,  7, BFMemClkFreqVal);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  4,  0, BFMemClkFreq);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31,  0, BFDctAddlOffsetReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x9C), 31,  0, BFDctAddlDataReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4),  14, 12, BFCmdThrottleMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4),  11, 11, BFBwCapEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA4),   8,  8, BFODTSEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 31, 31, BFPerRankTimingEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 28, 28, BFFastSelfRefEntryDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 22, 22, BFPrtlChPDEnhEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 21, 21, BFAggrPDEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 20, 20, BFBankSwap);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 17, 16, BFMemPhyPllPdMode);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110), 10, 10, BFMemCleared);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  9,  9, BFMemClrBusy);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  8,  8, BFDramEnabled);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  5,  5, BFDctDatIntLv);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  3,  3, BFMemClrInit);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x114),  9,  9, BFDctSelBankSwap);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x118), 19, 19, BFLockDramCfg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x118), 18, 18, BFCC6SaveEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 31,  0, BFMctCfgHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 13, 13, BFPrefIoDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 12, 12, BFPrefCpuDis);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x120), 23,  16, BFTrcBufAdrPtrHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x120), 15,   8, BFTrcBufDramLimitHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x120),  7,   0, BFTrcBufDramBaseHi);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B0), 31,  0, BFExtMctCfgLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B4), 31,  0, BFExtMctCfgHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B4), 27, 27, BFFlushWrOnS3StpGnt);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1B4), 26, 26, BFEnSplitMctDatBuffers);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1BC), 31,  0, BFCSMapCKE);

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

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x20C), 17, 16, BFWrDqDqsEarly);
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

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x248),  31, 31, BFRxChMntClkEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x248),  29, 24, BFAggrPDDelay);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x248),  21, 16, BFPchgPDEnDelay);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x248),  12,  8, BFTxpdll);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x248),   3,  0, BFTxp);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x24C),   29,  24, BFTcksrx);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x24C),   21,  16, BFTcksre);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x24C),   13,   8, BFTckesr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x24C),    3,   0, BFTpd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x24C),   31,   0, BFDramPwrMngm1Reg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 20,  18, BFDataPatGenSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 12,  12, BFCmdSendInProg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 11,  11, BFSendCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 10,  10, BFTestStatus);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  9,   8, BFCmdTgt);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  7,   5, BFCmdType);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  4,   4, BFStopOnErr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  3,   3, BFResetAllErr);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  2,   2, BFCmdTestEnable);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  13,   13, BFLfsrRollOver );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x254),  26,   24, BFTgtChipSelectA);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x254),  23,   21, BFTgtBankA);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x254),   9,    0, BFTgtAddressA);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x258),  26,   24, BFTgtChipSelectB);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x258),  23,   21, BFTgtBankB);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x258),   9,    0, BFTgtAddressB);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x25C),  31, 22, BFBubbleCnt2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x25C),  21, 12, BFBubbleCnt);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x25C),   7,  0, BFCmdStreamLen);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x260), 20,  0, BFCmdCount);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x268), 17,  0, BFNibbleErrSts);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x26C), 17,  0, BFNibbleErr180Sts);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x270), 18,   0, BFDataPrbsSeed);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x274), 31,   0, BFDramDqMaskLow );
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x278), 31,   0, BFDramDqMaskHigh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x27C),  7,   0, BFDramEccMask );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x288), 31, 24, BFXorPatOvr);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 31, 31, BFSendActCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 30, 30, BFSendPchgCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 29, 22, BFCmdChipSelect);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 21, 19, BFCmdBank);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 17,  0, BFCmdAddress);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x294), 31,  0, BFDQErrLow);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x298), 31,  0, BFDQErrHigh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x29C), 7,  0, BFEccErr );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2A8), 31,  0, BFDramUserDataPattern0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2AC), 31,  0, BFDramUserDataPattern1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2B0), 31,  0, BFDramUserDataPattern2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2B4), 31,  0, BFDramUserDataPattern3);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2E0), 30,  30, BFFastMstateDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2E0), 28, 24, BFM1MemClkFreq);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2E0), 22, 20, BFMxMrsEn);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2E8), 31, 16, BFMxMr1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2E8), 15, 0, BFMxMr0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2EC), 15, 0, BFMxMr2);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x2F0),  0,   0, BFEffArbDis);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x400), 11,  8, BFGmcTokenLimit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x400),  3,  0, BFMctTokenLimit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x404), 31,  0, BFGmcToDctControl1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x408), 28, 24, BFCpuElevPrioPeriod);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x408),  0,  0, BFCpuElevPrioDis);

  // ---------------------------------------------------------------------------
  //
  // DCT PHY REGISTERS
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 31,  0, BFODCControl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 22, 20, BFDqsDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 18, 16, BFDataDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 14, 12, BFClkDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 10,  8, BFAddrCmdDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  6,  4, BFCsOdtDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  2,  0, BFCkeDrvStren);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x04, 31,  0, BFAddrTmgControl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 30, 29, BFDisablePredriverCal);
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
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C, 15, 12, BFCKETri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C, 11,  8, BFODTTri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0C,  7,  0, BFChipSelTri);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 25, 24, BFRxDLLWakeupTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 22, 20, BFRxCPUpdPeriod);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 19, 16, BFRxMaxDurDllNoLock);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  9,  8, BFTxDLLWakeupTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  6,  4, BFTxCPUpdPeriod);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  3,  0, BFTxMaxDurDllNoLock);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x50, 31,  0, BFRstRcvFifo);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F13,  8,  8, BFRxSsbMntClkEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F13, 14, 14, BFProcOdtAdv);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F13,  7,  0, BFPhy0x0D0F0F13);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0830,  4,  4, BFEccDLLPwrDnConf);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F30,  6,  6, BFDataCtlPipePclkGateEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F30,  7,  7, BFPchgPdPclkGateEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F30,  9,  9, BFTxPclkGateEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE013, 15,  0, BFPllRegWaitTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE006, 15,  0, BFPllLockTime);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F02, 15,  0, BFDataByteTxPreDriverCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F04, 12,  12, BFPOdtOff);
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

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4001, 15,  8, BFTxP);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4001,  7,  0, BFTxN);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F0B, 31,  0, BFPhy0x0D0F0F0B);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F07, 31,  0, BFPhy0x0D0F0F07);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F03, 31,  0, BFPhy0x0D0F0F03);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F812F, 15,  0, BFAddrCmdTri);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F10, 12, 12, BFEnRxPadStandby);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D04E008,  8,  8, BFPStateToAccess);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2030,  4,  4, BFPhyClkConfig0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2130,  4,  4, BFPhyClkConfig1);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE040,  3,  0, BFRate);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F1E, 14, 12, BFDllCSRBiasTrim);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F1F,  4,  3, BFDataRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F1F,  7,  5, BFRxDqInsDly);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2F1F,  4,  3, BFClkRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4009,  3,  2, BFCsrComparator);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4009, 15, 14, BFCmpVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8F1F,  4,  3, BFCmdRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC01F,  4,  3, BFAddrRxVioLvl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F31,  9,  0, BFDataFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE019, 14,  0, BFFence2);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE01A, 15,  0, BFChAM1FenceSave);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F30,  8,  8, BFBlockRxDqsLock);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F001C, 15,  0, BFDataByteDllPowerMgnByte0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F011C, 15,  0, BFDataByteDllPowerMgnByte1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F021C, 15,  0, BFDataByteDllPowerMgnByte2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F031C, 15,  0, BFDataByteDllPowerMgnByte3);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F041C, 15,  0, BFDataByteDllPowerMgnByte4);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F051C, 15,  0, BFDataByteDllPowerMgnByte5);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F061C, 15,  0, BFDataByteDllPowerMgnByte6);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F071C, 15,  0, BFDataByteDllPowerMgnByte7);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F081C, 15,  0, BFDataByteDllPowerMgnByte8);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F1C, 15,  0, BFDataByteDllPowerMgnByteAll);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4006, 2, 0, BFObsrvVrefSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4007, 2, 0, BFVrefSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4007, 8, 3, BFVrefDAC);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F20, 15, 0, BFDataByteRxDqsDLLDimm0Broadcast);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F21, 15, 0, BFDataByteRxDqsDLLDimm1Broadcast);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F22, 15, 0, BFDataByteRxDqsDLLDimm2Broadcast);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F23, 15, 0, BFDataByteRxDqsDLLDimm3Broadcast);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F38, 14, 13, BFReducedLoop);

  // ---------------------------------------------------------------------------
  //
  // FUNCTION 3
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x40), 31,  0, BFMcaNbCtlReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x44), 22, 22, BFDramEccEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x44),  2,  2, BFSyncOnUcEccEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x180), 25, 25, BFEccSymbolSize);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x58),  4,  0, BFDramScrub);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x5C),  0,  0, BFScrubReDirEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x5C), 31,  0, BFScrubAddrLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x60), 31,  0, BFScrubAddrHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x88), 27, 27, BFDisDramScrub);

  // ---------------------------------------------------------------------------
  //
  // FUNCTION 4
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  //
  // FUNCTION 5
  //
  // ---------------------------------------------------------------------------
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x84),  20, 16, BFDdrMaxRate);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x160),  18, 18, BFMemPstate0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x164),  18, 18, BFMemPstate1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x168),  18, 18, BFMemPstate2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x16C),  18, 18, BFMemPstate3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x160),  16, 10, BFNbVid0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x164),  16, 10, BFNbVid1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x168),  16, 10, BFNbVid2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x16C),  16, 10, BFNbVid3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x160),  21, 21, BFNbVid0Hi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x164),  21, 21, BFNbVid1Hi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x168),  21, 21, BFNbVid2Hi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x16C),  21, 21, BFNbVid3Hi);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170), 31,  0, BFNbPstateCtlReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170), 31, 31, BFMemPstateDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170), 14, 14, BFSwNbPstateLoDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170),  7,  6, BFNbPstateHi);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170),  4,  3, BFNbPstateLo);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170),  1,  0, BFNbPstateMaxVal);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x174), 24, 24, BFCurMemPstate);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x174), 20, 19, BFCurNbPstate);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x174),  0,  0, BFNbPstateDis);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x188),  1,  0, BFNbOffsetTrim);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x240), 31,  0, BFECCExclusionBaseLow);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x244), 15,  0, BFECCExclusionBaseHigh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x248), 31,  0, BFECCExclusionLimitLow);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x24C), 15,  0, BFECCExclusionLimitHigh);

  // ---------------------------------------------------------------------------
  //
  // LINK BITFIELD
  //
  // ---------------------------------------------------------------------------
  LINK_TSEFO (NBRegTable, BFNbVid0, BFNbVid0Hi);
  LINK_TSEFO (NBRegTable, BFNbVid1, BFNbVid1Hi);
  LINK_TSEFO (NBRegTable, BFNbVid2, BFNbVid2Hi);
  LINK_TSEFO (NBRegTable, BFNbVid3, BFNbVid3Hi);

  // ---------------------------------------------------------------------------
  //
  // REGISTERS WITH MULTIPLE MEMORY PSTATE COPIES
  //
  // ---------------------------------------------------------------------------
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFODCControl);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFDqsDrvStren);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFDataDrvStren);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFClkDrvStren);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFAddrCmdDrvStren);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFCsOdtDrvStren);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFCkeDrvStren);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFAddrTmgControl);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFDisablePredriverCal);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFProcOdtAdv);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFPOdtOff);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFDllCSRBiasTrim);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFDataRxVioLvl);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFRxDqInsDly);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTras);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTrp);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTrcd);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTcl);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTrtp);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFFourActWindow);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTrrd);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTrc);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFWrDqDqsEarly);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTwtr);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTcwl);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFTwrDDR3);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFWrOdtOnDuration);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFWrOdtTrnOnDly);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFRdOdtOnDuration);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFRdOdtTrnOnDly);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFMxMr0);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFMxMr1);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFMxMr2);
  // These Phy fence registers don't have mutiple memory Pstate copies
  // But they need to be written again in M1 context
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFDataFence2);
  MULTI_MPSTATE_COPY_TSEFO (NBRegTable, BFFence2);

  IDS_OPTION_HOOK (IDS_INIT_MEM_REG_TABLE, NBPtr, &NBPtr->MemPtr->StdHeader);
}
