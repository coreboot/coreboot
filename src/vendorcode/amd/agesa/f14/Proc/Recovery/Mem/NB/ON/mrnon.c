/* $NoKeywords:$ */
/**
 * @file
 *
 * mrnon.c
 *
 * Common Northbridge  functions for Ontario Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 48803 $ @e \$Date: 2011-03-10 20:18:28 -0700 (Thu, 10 Mar 2011) $
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
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mrport.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "mm.h"
#include "mn.h"
#include "mrnon.h"
#include "heapManager.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_NB_ON_MRNON_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

#define SPLIT_CHANNEL   0x20000000
#define CHANNEL_SELECT  0x10000000
#define MAX_DELAYS    9   /* 8 data bytes + 1 ECC byte */
#define MAX_DIMMS     4   /* 4 DIMMs per channel */

#define PHY_DIRECT_ADDRESS_MASK   0x0D000000

STATIC CONST UINT8 RecInstancesPerTypeON[8] = {8, 2, 1, 0, 2, 0, 1, 1};
/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
CONST MEM_FREQ_CHANGE_PARAM RecFreqChangeParamON = {0x1838, NULL, 3, 10, 2, 9, NULL, 1000};

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

VOID
STATIC
MemRecNInitNBRegTableON (
  IN OUT   TSEFO *NBRegTable
  );

UINT32
STATIC
MemRecNCmnGetSetFieldON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

BOOLEAN
STATIC
MemRecNIsIdSupportedON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
STATIC CONST UINT32 RecModeDefRegArrayON[] = {
  BFDramBankAddrReg, 0x00000011,
  BFDramTimingLoReg, 0x000A0092,
  BFDramTiming0,     0x0A000101,
  BFDramTiming1,     0x04100415,
  BFDramTimingHiReg, 0x02D218FF,
  BFDramMRSReg,      0x000400A5,
  BFDramControlReg,  0x04802A03,
  BFDramConfigLoReg, 0x06600000,
  BFDramConfigHiReg, 0x1E000000,
  BFPhyFence,        0x000056B5,
  NULL
};
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes the northbridge block
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *     @param[in]       NodeID   - Node ID for this NB block
 *
 *     @return          TRUE  - This node is a Llano and this NB block has been initialized
 *     @return          FALSE - This node is not a Llano
 */

BOOLEAN
MemRecConstructNBBlockON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  UINT8 i;
  DIE_STRUCT *MCTPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem->SocketId, &(MemPtr->DiesPerSystem->LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemRecNIsIdSupportedON (NBPtr, &(MemPtr->DiesPerSystem->LogicalCpuid))) {
    return FALSE;
  }

  NBPtr->MemPtr = MemPtr;
  NBPtr->RefPtr = MemPtr->ParameterListPtr;

  MCTPtr = MemPtr->DiesPerSystem;
  NBPtr->MCTPtr = MCTPtr;
  NBPtr->MCTPtr->NodeId = 0;
  NBPtr->PciAddr.AddressValue = MCTPtr->PciAddr.AddressValue;

  //
  // Allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
  //
  AllocHeapParams.RequestedBufferSize = (sizeof (DCT_STRUCT) + sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK)) + (MAX_DIMMS * MAX_DELAYS * NUMBER_OF_DELAY_TABLES);
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_DCT_STRUCT_HANDLE, NodeID, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) != AGESA_SUCCESS) {
    ASSERT(FALSE); // Could not allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
    return FALSE;
  }

  NBPtr->SPDPtr = MemPtr->SpdDataStructure;
  NBPtr->AllNodeSPDPtr = MemPtr->SpdDataStructure;

  MemPtr->DieCount = 1;
  MCTPtr->Dct = 0;
  MCTPtr->DctCount = 1;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += sizeof (DCT_STRUCT);
  MCTPtr->DctData->ChannelCount = 1;
  MCTPtr->DctData->ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += sizeof (CH_DEF_STRUCT);
  NBPtr->PSBlock = (MEM_PS_BLOCK *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += sizeof (MEM_PS_BLOCK);

  MCTPtr->DctData->ChData->RcvEnDlys = (UINT16 *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS) * 2;
  MCTPtr->DctData->ChData->WrDqsDlys = AllocHeapParams.BufferPtr;

  //
  //  Initialize NB block's variables
  //
  NBPtr->DCTPtr = NBPtr->MCTPtr->DctData;
  NBPtr->DctCachePtr = NBPtr->DctCache;
  NBPtr->PsPtr = NBPtr->PSBlock;
  NBPtr->ChannelPtr = NBPtr->DCTPtr->ChData;

  NBPtr->DctCachePtr = NBPtr->DctCache;

  MemRecNInitNBRegTableON (NBPtr->NBRegTable);
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->VarMtrrHiMsk = MemRecGetVarMtrrHiMsk (&(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));
  NBPtr->FreqChangeParam = (MEM_FREQ_CHANGE_PARAM *) &RecFreqChangeParamON;
  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);
  LibAmdMemFill (NBPtr->IsSupported, FALSE, sizeof (NBPtr->IsSupported), &NBPtr->MemPtr->StdHeader);
  for (i = 0; i < NumberOfHooks; i++) {
    NBPtr->FamilySpecificHook[i] = (BOOLEAN (*) (MEM_NB_BLOCK *, VOID *)) MemRecDefTrue;
  }

  NBPtr->InitRecovery = MemRecNMemInitON;

  NBPtr->RecModeDefRegArray = RecModeDefRegArrayON;

  NBPtr->SwitchNodeRec = (VOID (*) (MEM_NB_BLOCK *, UINT8)) MemRecDefRet;
  NBPtr->SwitchDCT = (VOID (*) (MEM_NB_BLOCK *, UINT8)) MemRecDefRet;
  NBPtr->SwitchChannel = (VOID (*) (MEM_NB_BLOCK *, UINT8)) MemRecDefRet;
  NBPtr->SetMaxLatency = MemRecNSetMaxLatencyON;
  NBPtr->GetSysAddrRec = MemRecNGetMCTSysAddrNb;
  NBPtr->SendMrsCmd = MemRecNSendMrsCmdNb;
  NBPtr->sendZQCmd = MemRecNSendZQCmdNb;
  NBPtr->SetDramOdtRec = MemRecNSetDramOdtON;

  NBPtr->GetBitField = MemRecNGetBitFieldNb;
  NBPtr->SetBitField = MemRecNSetBitFieldNb;
  NBPtr->GetTrainDly = MemRecNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemRecNSetTrainDlyNb;

  NBPtr->MemRecNCmnGetSetFieldNb = MemRecNCmnGetSetFieldON;
  NBPtr->MemRecNcmnGetSetTrainDlyNb = MemRecNcmnGetSetTrainDlyClientNb;
  NBPtr->MemRecNSwitchDctNb = (VOID (*) (MEM_NB_BLOCK *, UINT8)) MemRecDefRet;
  NBPtr->TrainingFlow = MemNRecTrainingFlowClientNb;
  NBPtr->ReadPattern = MemRecNContReadPatternClientNb;
  NBPtr->IsSupported[DramModeAfterDimmPres] = TRUE;
  NBPtr->FamilySpecificHook[OverrideRcvEnSeed] = MemRecNOverrideRcvEnSeedON;

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets or sets a value to a bit field in a PCI register.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Name of Field to be set *     @param[in]   Field - Value to be programmed
 *     @param[in]   IsSet - Indicates if the function will set or get
 *
 *     @return      value read, if the function is used as a "get"
 */

UINT32
STATIC
MemRecNCmnGetSetFieldON (
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
            NumOfInstances = RecInstancesPerTypeON[(Address >> 13) & 0x7];
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
        Address |= (((UINT32) (24 + 0)) << 15);
        PciAddr.AddressValue = Address;
        LibAmdPciRead (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
          if ((FieldName != BFDctAddlDataReg) && (FieldName != BFDctAddlOffsetReg) &&
              (FieldName != BFDctExtraDataReg) && (FieldName != BFDctExtraOffsetReg)) {
            IDS_HDT_CONSOLE (MEM_GETREG, "~Fn%d_%03x = %x\n", (Address >> 12) & 0xF, Address & 0xFFF, Value);
          }
      } else if (Type == DCT_PHY_ACCESS) {
          if (IsPhyDirectAccess && (NumOfInstances > 1)) {
            Address = (Address & 0x0FFFF0FF) | (((UINT32) Instance) << 8);
          }
        MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
        Value = MemRecNGetBitFieldNb (NBPtr, BFDctAddlDataReg);
          IDS_HDT_CONSOLE (MEM_GETREG, "~Fn2_%d9C_%x = %x\n", NBPtr->Dct, Address & 0x0FFFFFFF, Value);
      } else if (Type == DCT_EXTRA) {
        MemRecNSetBitFieldNb (NBPtr, BFDctExtraOffsetReg, Address);
        Value = MemRecNGetBitFieldNb (NBPtr, BFDctExtraDataReg);
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
          LibAmdPciWrite (AccessWidth32, PciAddr , &Value, &NBPtr->MemPtr->StdHeader);
            if ((FieldName != BFDctAddlDataReg) && (FieldName != BFDctAddlOffsetReg) &&
                (FieldName != BFDctExtraDataReg) && (FieldName != BFDctExtraOffsetReg)) {
              IDS_HDT_CONSOLE (MEM_SETREG, "~Fn%d_%03x [%d:%d] = %x\n", (Address >> 12) & 0xF, Address & 0xFFF, Highbit, Lowbit, Field);
            }
        } else if (Type == DCT_PHY_ACCESS) {
          MemRecNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
          Address |= DCT_ACCESS_WRITE;
          MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
            IDS_HDT_CONSOLE (MEM_SETREG, "~Fn2_%d9C_%x [%d:%d] = %x\n", NBPtr->Dct, Address & 0x0FFFFFFF, Highbit, Lowbit, Field);
        } else if (Type == DCT_EXTRA) {
          MemRecNSetBitFieldNb (NBPtr, BFDctExtraDataReg, Value);
          Address |= DCT_ACCESS_WRITE;
          MemRecNSetBitFieldNb (NBPtr, BFDctExtraOffsetReg, Address);
            IDS_HDT_CONSOLE (MEM_SETREG, "~Fn2_%dF4_%x [%d:%d] = %x\n", NBPtr->Dct, Address & 0x0FFFFFFF, Highbit, Lowbit, Field);
        } else {
          IDS_ERROR_TRAP;
        }
          if (IsLinked) {
            MemRecNCmnGetSetFieldON (NBPtr, 1, FieldName + 1, Field >> (Highbit - Lowbit + 1));
          }
      } else {
        Value = Value >> Lowbit;  // Shift
        // A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case
        if ((Highbit - Lowbit) != 31) {
          Value &= (((UINT32)1 << (Highbit - Lowbit + 1)) - 1);
        }
          if (IsLinked) {
            Value |= MemRecNCmnGetSetFieldON (NBPtr, 0, FieldName + 1, 0) << (Highbit - Lowbit + 1);
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
 *     @param[in,out]   *NBRegTable   - Pointer to the NB Table
 *
 */

VOID
STATIC
MemRecNInitNBRegTableON (
  IN OUT   TSEFO *NBRegTable
  )
{
  UINT16 i;
  for (i = 0; i < BFEndOfList; i++) {
    NBRegTable[i] = 0;
  }

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (0, 0x00), 31,  0, BFDevVendorIDReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (0, 0x60),  2,  0, BFNodeID);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x40), 31, 0, BFDramBaseReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x44), 31, 0, BFDramLimitReg0);

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
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  9,  7, BFDramTerm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 11, 10, BFDramTermDyn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 31,  0, BFDramTimingLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 31,  0, BFDramTimingHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 31,  0, BFDramConfigLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31,  0, BFDramConfigHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31,  0, BFDctAddlOffsetReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x9C), 31,  0, BFDctAddlDataReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78),  3,  0, BFRdPtrInit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78),  6,  6, BFRxPtrInitReq);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 18, 18, BFDqsRcvEnTrain);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 17, 17, BFAddrCmdTriEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 21, 21, BFDisCutThroughMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 31, 22, BFMaxLatency);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 15,  0, BFMrsAddress);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 18, 16, BFMrsBank);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 22, 20, BFMrsChipSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 26, 26, BFSendMrsCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 27, 27, BFDeassertMemRstX);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 28, 28, BFAssertCke);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 29, 29, BFSendZQCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 30, 30, BFSendCtrlWord);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31, 31, BFEnDramInit);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C),  7,  7, BFLevel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 12, 12, BFMrsQoff);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 18, 18, BFDisAutoRefresh);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  0,  0, BFInitDram);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90),  1,  1, BFExitSelfRef);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 17, 17, BFEnterSelfRef);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 27, 27, BFDisDllShutdownSR);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  4,  0, BFMemClkFreq);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  7,  7, BFMemClkFreqVal);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  8,  8, BFDdr3Mode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  9,  9, BFLegacyBiosMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 11, 10, BFZqcsInterval);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 14, 14, BFDisDramInterface);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 15, 15, BFPowerDownEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 22, 22, BFBankSwizzleMode);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 22, 21, BFDbeGskMemClkAlignMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xC0),  0,  0, BFTraceModeEn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xF0), 31,  0, BFDctExtraOffsetReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xF4), 31,  0, BFDctExtraDataReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  8, 8, BFDramEnabled);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110), 31, 0, BFDctSelBaseAddrReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x114), 31, 0, BFDctSelBaseOffsetReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C),  6,  2, BFDctWrLimit);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 12, 12, BFPrefCpuDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0), 17,  2, BFTrainLength);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0), 20, 20, BFDramTrainPdbDis);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0), 22, 22, BFRdDramTrainMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C0), 23, 23, BFRdTrainGo);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1C8), 31,  0, BFWrTrainAdrPtrLo);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x1D0),  9,  0, BFWrTrainBufAddr);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xD4),  5,  0, BFMainPllOpFreqId);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xDC), 26, 20, BFNbPs0NclkDiv);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0x188), 22, 22, BFEnCpuSerRdBehindNpIoWr);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (6, 0x90), 30, 30, BFNbPsCtrlDis);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  2,  0, BFCkeDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  6,  4, BFCsOdtDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 10,  8, BFAddrCmdDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 14, 12, BFClkDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 18, 16, BFDataDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 22, 20, BFDqsDrvStren);
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
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 21, 15, BFPllMult);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 27, 24, BFPllDiv);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0B, 31,  0, BFDramPhyStatusReg);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0C, 31, 16, BFPhyFence);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D, 19, 16, BFRxMaxDurDllNoLock);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D,  3,  0, BFTxMaxDurDllNoLock);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F10, 12, 12, BFEnRxPadStandby);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE003, 14, 13, BFDisablePredriverCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE006, 15,  0, BFPllLockTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F812F, 15,  0, BFAddrCmdTri);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F0F, 14, 12, BFAlwaysEnDllClks);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F1C00, 15,  0, BFPNOdtCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F1D00, 15,  0, BFPNDrvCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D081E00, 15,  0, BFCalVal);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F1F,  4,  3, BFDataRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2F1F,  4,  3, BFClkRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4009, 15, 14, BFCmpVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8F1F,  4,  3, BFCmdRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC01F,  4,  3, BFAddrRxVioLvl);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x06, 11,  8, BFTwrrdSD);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x06,  3,  0, BFTrdrdSD);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x16,  3,  0, BFTwrwrSD);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x30, 12,  0, BFDbeGskFifoNumerator);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x31, 12,  0, BFDbeGskFifoDenominator);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32,  4,  0, BFDataTxFifoSchedDlySlot0);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32,  7,  7, BFDataTxFifoSchedDlyNegSlot0);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32, 12,  8, BFDataTxFifoSchedDlySlot1);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32, 15, 15, BFDataTxFifoSchedDlyNegSlot1);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x40, 31,  0, BFDramTiming0);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x83,  2,  0, BFRdOdtTrnOnDly);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x83,  6,  4, BFRdOdtOnDuration);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x83,  8,  8, BFWrOdtTrnOnDly);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x83, 14, 12, BFWrOdtOnDuration);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x180, 31,  0, BFPhyRODTCSLow);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x182, 31,  0, BFPhyWODTCSLow);
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *      This function matches the CPU_LOGICAL_ID with certain criteria to
 *      determine if it is supported by this NBBlock.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *LogicalIdPtr - Pointer to the CPU_LOGICAL_ID
 *
 *     @return          TRUE -  This node is a Llano.
 *     @return          FALSE - This node is not a Llano.
 */
BOOLEAN
STATIC
MemRecNIsIdSupportedON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  )
{
  if ((LogicalIdPtr->Family & (AMD_FAMILY_14_ON)) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}
