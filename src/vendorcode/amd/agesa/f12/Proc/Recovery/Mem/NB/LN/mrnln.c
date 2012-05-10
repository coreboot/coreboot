/* $NoKeywords:$ */
/**
 * @file
 *
 * mrnln.c
 *
 * Common Northbridge  functions for Llano Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 48317 $ @e \$Date: 2011-03-08 01:38:14 +0800 (Tue, 08 Mar 2011) $
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
#include "amdlib.h"
#include "Ids.h"
#include "mrport.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "mm.h"
#include "mn.h"
#include "mrnln.h"
#include "heapManager.h"
#include "AdvancedApi.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_NB_LN_MRNLN_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

#define SPLIT_CHANNEL   0x20000000
#define CHANNEL_SELECT  0x10000000
#define MAX_DELAYS    9   /* 8 data bytes + 1 ECC byte */
#define MAX_DIMMS     4   /* 4 DIMMs per channel */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
CONST MEM_FREQ_CHANGE_PARAM RecFreqChangeParamLN = {0x0190, NULL, 8, 14, 3};

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

VOID
STATIC
MemRecNSwitchChannelLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Channel
  );

VOID
STATIC
MemRecNInitNBRegTableLN (
  IN OUT   TSEFO *NBRegTable
  );

UINT32
STATIC
MemRecNCmnGetSetFieldLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

UINT32
STATIC
MemRecNcmnGetSetTrainDlyLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

BOOLEAN
STATIC
MemRecNIsIdSupportedLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
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
MemRecConstructNBBlockLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  UINT8 i;
  UINT8 Dct;
  UINT8 Channel;
  DIE_STRUCT *MCTPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[NodeID].SocketId, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemRecNIsIdSupportedLN (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
    return FALSE;
  }

  //
  // Allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
  //
  MCTPtr = &MemPtr->DiesPerSystem[NodeID];
  AllocHeapParams.RequestedBufferSize = MAX_DCTS_PER_NODE_LN * (
                                          sizeof (DCT_STRUCT) + (
                                            MAX_CHANNELS_PER_DCT_LN * (
                                              sizeof (CH_DEF_STRUCT) + (
                                                MAX_DIMMS * MAX_DELAYS * NUMBER_OF_DELAY_TABLES
                                              )
                                            )
                                          )
                                        ) + (sizeof (TSEFO) * BFEndOfList);
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_DCT_STRUCT_HANDLE, NodeID, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) != AGESA_SUCCESS) {
    return FALSE;
  }

  MemPtr->DieCount = 1;
  MCTPtr->DctCount = MAX_DCTS_PER_NODE_LN;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += MAX_DCTS_PER_NODE_LN * sizeof (DCT_STRUCT);
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_LN; Dct++) {
    MCTPtr->DctData[Dct].Dct = Dct;
    MCTPtr->DctData[Dct].ChannelCount = MAX_CHANNELS_PER_DCT_LN;
    MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
    MCTPtr->DctData[Dct].ChData[0].ChannelID = Dct;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_LN * sizeof (CH_DEF_STRUCT);
    for (Channel = 0; Channel < MAX_CHANNELS_PER_DCT_LN; Channel++) {
      MCTPtr->DctData[Dct].ChData[Channel].RcvEnDlys = (UINT16 *) AllocHeapParams.BufferPtr;
      AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS) * 2;
      MCTPtr->DctData[Dct].ChData[Channel].WrDqsDlys = AllocHeapParams.BufferPtr;
      MCTPtr->DctData[Dct].ChData[Channel].Dct = Dct;
      AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
    }
  }

  //
  //  Initialize NB block's variables
  //
  NBPtr->NBRegTable = (TSEFO *) AllocHeapParams.BufferPtr;
  NBPtr->MemPtr = MemPtr;
  NBPtr->RefPtr = MemPtr->ParameterListPtr;
  NBPtr->MCTPtr = &MemPtr->DiesPerSystem[NodeID];
  NBPtr->SPDPtr = MemPtr->SpdDataStructure;
  NBPtr->AllNodeSPDPtr = MemPtr->SpdDataStructure;

  NBPtr->DctCachePtr = NBPtr->DctCache;

  MemRecNInitNBRegTableLN (NBPtr->NBRegTable);
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->VarMtrrHiMsk = MemRecGetVarMtrrHiMsk (&(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));
  NBPtr->FreqChangeParam = (MEM_FREQ_CHANGE_PARAM *) &RecFreqChangeParamLN;
  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);
  LibAmdMemFill (NBPtr->IsSupported, FALSE, sizeof (NBPtr->IsSupported), &NBPtr->MemPtr->StdHeader);
  for (i = 0; i < NumberOfHooks; i++) {
    NBPtr->FamilySpecificHook[i] = (BOOLEAN (*) (MEM_NB_BLOCK *, VOID *)) MemRecDefTrue;
  }

  NBPtr->InitRecovery = MemRecNMemInitLN;

  NBPtr->RecModeDefRegArray = NULL;

  NBPtr->SwitchNodeRec = (VOID (*) (MEM_NB_BLOCK *, UINT8)) MemRecDefRet;
  NBPtr->SwitchDCT = MemRecNSwitchDctLN;
  NBPtr->SwitchChannel = MemRecNSwitchChannelLN;
  NBPtr->SetMaxLatency = MemRecNSetMaxLatencyLN;
  NBPtr->GetSysAddrRec = MemRecNGetMCTSysAddrNb;
  NBPtr->SendMrsCmd = MemRecNSendMrsCmdNb;
  NBPtr->sendZQCmd = MemRecNSendZQCmdNb;
  NBPtr->SetDramOdtRec = MemRecNSetDramOdtLN;

  NBPtr->GetBitField = MemRecNGetBitFieldNb;
  NBPtr->SetBitField = MemRecNSetBitFieldNb;
  NBPtr->GetTrainDly = MemRecNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemRecNSetTrainDlyNb;

  NBPtr->MemRecNCmnGetSetFieldNb = MemRecNCmnGetSetFieldLN;
  NBPtr->MemRecNcmnGetSetTrainDlyNb = MemRecNcmnGetSetTrainDlyLN;
  NBPtr->MemRecNSwitchDctNb = MemRecNSwitchDctLN;
  NBPtr->MemRecNInitializeMctNb = MemRecNInitializeMctLN;
  NBPtr->MemRecNFinalizeMctNb = MemRecNFinalizeMctLN;
  NBPtr->IsSupported[DramModeAfterDimmPres] = TRUE;
  NBPtr->TrainingFlow = MemNRecTrainingFlowClientNb;
  NBPtr->ReadPattern = MemRecNContReadPatternClientNb;
  NBPtr->FamilySpecificHook[ReEnablePhyComp] = MemRecNReEnablePhyCompNb;
  MemRecNSwitchDctLN (NBPtr, 0);

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets the current DCT to work on.
 *   Should be called before accessing a certain DCT
 *   All data structures will be updated to point to the current DCT
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   Dct - ID of the target DCT
 *
 */

VOID
MemRecNSwitchDctLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct
  )
{
  NBPtr->Dct = Dct & 1;
  NBPtr->SPDPtr = &(NBPtr->AllNodeSPDPtr[Dct * MAX_DIMMS_PER_CHANNEL]);
  NBPtr->DCTPtr = &(NBPtr->MCTPtr->DctData[NBPtr->Dct]);

  MemRecNSwitchChannelLN (NBPtr, NBPtr->Channel);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets the current channel to work on.
 *   Should be called before accessing a certain channel
 *   All data structures will be updated to point to the current channel
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   Channel - ID of the target channel
 *
 */

VOID
STATIC
MemRecNSwitchChannelLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Channel
  )
{
  NBPtr->Channel = Channel & 1;
  NBPtr->ChannelPtr = &(NBPtr->DCTPtr->ChData[NBPtr->Channel]);
}
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets or set DQS timing during training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   TrnDly - type of delay to be set
 *     @param[in]   DrbnVar - encoding of Dimm-Rank-Byte-Nibble to be accessed
 *                  (use either DIMM_BYTE_ACCESS(dimm,byte) or CS_NBBL_ACCESS(cs,nibble) to use this encoding
 *     @param[in]   Field - Value to be programmed
 *     @param[in]   IsSet - Indicates if the function will set or get
 *
 *     @return      value read, if the function is used as a "get"
 */

UINT32
STATIC
MemRecNcmnGetSetTrainDlyLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  )
{
  UINT16 Index;
  UINT16 Offset;
  UINT32 Value;
  UINT32 Address;
  UINT8 Dimm;
  UINT8 Byte;

  Dimm = DRBN_DIMM (DrbnVar);
  Byte = DRBN_BYTE (DrbnVar);

  ASSERT (Dimm < 1);
  ASSERT (Byte <= 8);

  switch (TrnDly) {
  case AccessRcvEnDly:
    Index = 0x10;
    break;
  case AccessWrDqsDly:
    Index = 0x30;
    break;
  case AccessWrDatDly:
    Index = 0x01;
    break;
  case AccessRdDqsDly:
    Index = 0x05;
    break;
  case AccessPhRecDly:
    Index = 0x50;
    break;
  default:
    Index = 0;
    IDS_ERROR_TRAP;
  }

  switch (TrnDly) {
  case AccessRcvEnDly:
  case AccessWrDqsDly:
    if ((Byte & 0x04) != 0) {
      // if byte 4,5,6,7
      Index += 0x10;
    }
    if ((Byte & 0x02) != 0) {
      // if byte 2,3,6,7
      Index++;
    }
    Offset = 16 * (Byte % 2);
    break;

  case AccessRdDqsDly:
    Field &= ~ 0x0001;
  case AccessWrDatDly:
    Index += (Dimm * 0x100);
    // break is not being used here because AccessRdDqsDly and AccessWrDatDly also need
    // to run AccessPhRecDly sequence.
  case AccessPhRecDly:
    Index += (Byte / 4);
    Offset = 8 * (Byte % 4);
    break;
  default:
    Offset = 0;
    IDS_ERROR_TRAP;
  }

  Address = Index;
  MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
  Value = MemRecNGetBitFieldNb (NBPtr, BFDctAddlDataReg);

  if (IsSet != 0) {
    if (TrnDly == AccessPhRecDly) {
      Value = NBPtr->DctCachePtr->PhRecReg[Index & 0x03];
    }

    Value = ((UINT32)Field << Offset) | (Value & (~((UINT32)0xFF << Offset)));
    MemRecNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
    Address |= DCT_ACCESS_WRITE;
    MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);

    if (TrnDly == AccessPhRecDly) {
      NBPtr->DctCachePtr->PhRecReg[Index & 0x03] = Value;
    }
  } else {
    Value = (Value >> Offset) & 0xFF;
  }

  return Value;
}

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
MemRecNCmnGetSetFieldLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  )
{
  SBDFO Address;
  PCI_ADDR PciAddr;
  UINT8 Type;
  UINT32 Value;
  UINT32 Highbit;
  UINT32 Lowbit;
  UINT32 Mask;

  Value = 0;
  if (FieldName < BFEndOfList) {
    Address = NBPtr->NBRegTable[FieldName];
    if (Address != 0) {
      Lowbit = TSEFO_END (Address);
      Highbit = TSEFO_START (Address);
      Type = TSEFO_TYPE (Address);

      // If Fn2 and DCT1 selected, set Address to be 1xx
      if (((Address & 0xF000) == 0x2000) && (NBPtr->Dct != 0)) {
        Address |= 0x0100;
      }
      if ((Address >> 29) == ((DCT_PHY_ACCESS << 1) | 1)) {
        // Special DCT Phy access
        Address &= 0x0FFFFFFF;
        Lowbit = 0;
        Highbit = 16;
      } else {
        // Normal DCT Phy access
        Address = TSEFO_OFFSET (Address);
      }


      if (Type == NB_ACCESS) {
        Address |= (((UINT32) (24 + 0)) << 15);
        PciAddr.AddressValue = Address;
        LibAmdPciRead (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
      } else if (Type == DCT_PHY_ACCESS) {
        MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
        Value = MemRecNGetBitFieldNb (NBPtr, BFDctAddlDataReg);
      } else if (Type == DCT_EXTRA) {
        MemRecNSetBitFieldNb (NBPtr, BFDctExtraOffsetReg, Address);
        Value = MemRecNGetBitFieldNb (NBPtr, BFDctExtraDataReg);
      } else {
        IDS_ERROR_TRAP;
      }

      if (IsSet != 0) {
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
        } else if (Type == DCT_PHY_ACCESS) {
          MemRecNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
          Address |= DCT_ACCESS_WRITE;
          MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
        } else if (Type == DCT_EXTRA) {
          MemRecNSetBitFieldNb (NBPtr, BFDctExtraDataReg, Value);
          Address |= DCT_ACCESS_WRITE;
          MemRecNSetBitFieldNb (NBPtr, BFDctExtraOffsetReg, Address);
        } else {
          IDS_ERROR_TRAP;
        }
      } else {
        Value = Value >> Lowbit;  // Shift
        // A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case
        if ((Highbit - Lowbit) != 31) {
          Value &= (((UINT32)1 << (Highbit - Lowbit + 1)) - 1);
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
MemRecNInitNBRegTableLN (
  IN OUT   TSEFO *NBRegTable
  )
{
  UINT16 i;
  for (i = 0; i <= BFEndOfList; i++) {
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

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x78), 18, 18, BFDqsRcvEnTrain);
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

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 22, 21, BFDbeGskMemClkAlignMode);
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
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0C, 20, 16, BFPhyFence);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE006, 15,  0, BFPllLockTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE013, 15,  0, BFPllRegWaitTime);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F1F,  4,  3, BFDataRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2F1F,  4,  3, BFClkRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F4009, 15, 14, BFCmpVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8F1F,  4,  3, BFCmdRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC01F,  4,  3, BFAddrRxVioLvl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F31, 14,  0, BFDataFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F2F31,  4,  0, BFClkFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F8F31,  4,  0, BFCmdFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FC031,  4,  0, BFAddrFence2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0F0F0F, 14, 12, BFAlwaysEnDllClks);

  MAKE_TSEFO (NBRegTable, DCT_PHY_DIRECT, 0x0D0FE003, 14, 13, BFDisablePredriverCal);
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

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x30, 12,  0, BFDbeGskFifoNumerator);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x31, 12,  0, BFDbeGskFifoDenominator);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32,  4,  0, BFDataTxFifoSchedDlySlot0);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32,  7,  7, BFDataTxFifoSchedDlyNegSlot0);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32, 12,  8, BFDataTxFifoSchedDlySlot1);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x32, 15, 15, BFDataTxFifoSchedDlyNegSlot1);

  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x40, 31,  0, BFDramTiming0);
  MAKE_TSEFO (NBRegTable, DCT_EXTRA, 0x41, 31,  0, BFDramTiming1);

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
MemRecNIsIdSupportedLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  )
{
  if ((LogicalIdPtr->Family & (AMD_FAMILY_12_LN)) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}
