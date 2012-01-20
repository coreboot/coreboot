/* $NoKeywords:$ */
/**
 * @file
 *
 * mrnor.c
 *
 * Common Northbridge  functions for Orochi Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 49896 $ @e \$Date: 2011-03-30 02:18:18 -0600 (Wed, 30 Mar 2011) $
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
#include "amdlib.h"
#include "Ids.h"
#include "mrport.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "mm.h"
#include "mn.h"
#include "mrnor.h"
#include "heapManager.h"
#include "AdvancedApi.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)
#define FILECODE PROC_RECOVERY_MEM_NB_OR_MRNOR_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

#define MAX_DCTS_PER_DIE 2
#define SPLIT_CHANNEL   0x20000000
#define CHANNEL_SELECT  0x10000000
#define MAX_DELAYS    9   /* 8 data bytes + 1 ECC byte */
#define MAX_DIMMS     4   /* 4 DIMMs per channel */

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

VOID
STATIC
MemRecNInitNBRegTableOr (
  IN OUT   TSEFO *NBRegTable
  );

UINT32
STATIC
MemRecNCmnGetSetFieldOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

UINT32
STATIC
MemRecNcmnGetSetTrainDlyOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

BOOLEAN
STATIC
MemRecNIsIdSupportedOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

VOID
STATIC
MemRecNSwitchNodeOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 NodeID
  );


/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
STATIC CONST UINT32 RecModeDefRegArrayOR[] = {
  BFDramBankAddrReg, 0x00001111,
  BFTref,            0x00000002,
  BFDramTimingLoReg, 0x00000000,
  BFDramTiming0,     0x0F060606,
  BFDramTiming1,     0x04100415,
  BFDramTiming2,     0x05050505,
  BFDramTiming3,     0x00000405,
  BFDramTiming4,     0x000B0B0B,
  BFDramTiming5,     0x0B0B0B0B,
  BFDramTiming6,     0x00171600,
  BFDramTiming10,    0x00000006,
  BFDramNBP0,        0x32000006,
  BFDramConfigHiReg, 0x00180800,
  BFDramODTCtlReg,   0x00006061,
  BFPhyFence,        0x000056B5,
  BFSwNbPstateLoDis, 0x00000001,
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
 *     @return          TRUE  - This node is a Orochi and this NB block has been initialized
 *     @return          FALSE - This node is not a Orochi
 */

BOOLEAN
MemRecConstructNBBlockOr (
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
  if (!MemRecNIsIdSupportedOr (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
    return FALSE;
  }

  //
  // Allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
  //
  MCTPtr = &MemPtr->DiesPerSystem[NodeID];
  AllocHeapParams.RequestedBufferSize = MAX_DCTS_PER_NODE_OR * (
                                          sizeof (DCT_STRUCT) + (
                                            MAX_CHANNELS_PER_DCT_OR * (
                                              sizeof (CH_DEF_STRUCT) + (
                                                MAX_DIMMS * MAX_DELAYS * NUMBER_OF_DELAY_TABLES
                                              )
                                            )
                                          )
                                        ) + sizeof (MEM_PS_BLOCK) + (sizeof (TSEFO) * BFEndOfList);
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_DCT_STRUCT_HANDLE, NodeID, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) != AGESA_SUCCESS) {
    return FALSE;
  }

  MemPtr->DieCount = MAX_DIES_PER_SOCKET_OR;
  MCTPtr->DctCount = MAX_DCTS_PER_NODE_OR;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  MemPtr->DiesPerSystem[NodeID + 1].DctCount = MAX_DCTS_PER_NODE_OR;
  MemPtr->DiesPerSystem[NodeID + 1].DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += MAX_DCTS_PER_NODE_OR * sizeof (DCT_STRUCT);
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_OR; Dct++) {
    MCTPtr->DctData[Dct].Dct = Dct;
    MCTPtr->DctData[Dct].ChannelCount = MAX_CHANNELS_PER_DCT_OR;
    MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
    MCTPtr->DctData[Dct].ChData[0].ChannelID = (MCTPtr->DieId * 2) + Dct;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_OR * sizeof (CH_DEF_STRUCT);
    for (Channel = 0; Channel < MAX_CHANNELS_PER_DCT_OR; Channel++) {
      MCTPtr->DctData[Dct].ChData[Channel].RcvEnDlys = (UINT16 *) AllocHeapParams.BufferPtr;
      AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS) * 2;
      MCTPtr->DctData[Dct].ChData[Channel].WrDqsDlys = AllocHeapParams.BufferPtr;
      MCTPtr->DctData[Dct].ChData[Channel].Dct = Dct;
      AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
    }
  }
  NBPtr->PsPtr = (MEM_PS_BLOCK *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += sizeof (MEM_PS_BLOCK);

  //
  //  Initialize NB block's variables
  //
  NBPtr->NBRegTable = (TSEFO *) AllocHeapParams.BufferPtr;
  NBPtr->MemPtr = MemPtr;
  NBPtr->RefPtr = MemPtr->ParameterListPtr;
  NBPtr->MCTPtr = MCTPtr;
  NBPtr->AllNodeMCTPtr = &MemPtr->DiesPerSystem[NodeID];
  NBPtr->SPDPtr = &MemPtr->SpdDataStructure[MemPtr->DiesPerSystem[NodeID].SocketId * MAX_CHANNELS_PER_SOCKET * MAX_DIMMS_PER_CHANNEL];
  NBPtr->AllNodeSPDPtr = &MemPtr->SpdDataStructure[MemPtr->DiesPerSystem[NodeID].SocketId * MAX_CHANNELS_PER_SOCKET * MAX_DIMMS_PER_CHANNEL];

  NBPtr->DctCachePtr = NBPtr->DctCache;

  MemRecNInitNBRegTableOr (NBPtr->NBRegTable);
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->VarMtrrHiMsk = MemRecGetVarMtrrHiMsk (&(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);
  LibAmdMemFill (NBPtr->IsSupported, FALSE, sizeof (NBPtr->IsSupported), &NBPtr->MemPtr->StdHeader);
  for (i = 0; i < NumberOfHooks; i++) {
    NBPtr->FamilySpecificHook[i] = (BOOLEAN (*) (MEM_NB_BLOCK *, VOID *)) MemRecDefTrue;
  }

  NBPtr->InitRecovery = MemRecNMemInitOr;

  NBPtr->RecModeDefRegArray = RecModeDefRegArrayOR;

  NBPtr->SwitchNodeRec = MemRecNSwitchNodeOr;
  NBPtr->SwitchDCT = MemRecNSwitchDctOr;
  NBPtr->SwitchChannel = MemRecNSwitchChannelOr;
  NBPtr->SetMaxLatency = MemRecNSetMaxLatencyOr;
  NBPtr->GetSysAddrRec = MemRecNGetMCTSysAddrNb;
  NBPtr->SendMrsCmd = MemRecNSendMrsCmdNb;
  NBPtr->sendZQCmd = MemRecNSendZQCmdNb;
  NBPtr->SetDramOdtRec = MemRecNSetDramOdtOr;

  NBPtr->GetBitField = MemRecNGetBitFieldNb;
  NBPtr->SetBitField = MemRecNSetBitFieldNb;
  NBPtr->GetTrainDly = MemRecNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemRecNSetTrainDlyNb;

  NBPtr->MemRecNCmnGetSetFieldNb = MemRecNCmnGetSetFieldOr;
  NBPtr->MemRecNcmnGetSetTrainDlyNb = MemRecNcmnGetSetTrainDlyOr;
  NBPtr->MemRecNSwitchDctNb = MemRecNSwitchDctOr;
  NBPtr->TrainingFlow = MemNRecTrainingFlowUnb;
  NBPtr->ReadPattern = MemRecNContReadPatternUnb;
  NBPtr->WritePattern = MemRecNContWritePatternUnb;
  NBPtr->CompareTestPattern = MemRecNCompareTestPatternUnb;

  NBPtr->IsSupported[CheckDramTerm] = TRUE;
  NBPtr->IsSupported[CheckDramTermDyn] = TRUE;
  MemRecNSwitchDctOr (NBPtr, 0);

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets the current die to work on.
 *   Should be called before accessing a certain die
 *   All data structures will be updated to point to the current node
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   NodeID - ID of the target die
 *
 */

VOID
STATIC
MemRecNSwitchNodeOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 NodeID
  )
{
  NBPtr->MCTPtr = &(NBPtr->AllNodeMCTPtr[NodeID]);
  NBPtr->Node = NodeID;
  NBPtr->MCTPtr->NodeId = NodeID;
  MemRecNSwitchDctOr (NBPtr, NBPtr->Dct);
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
MemRecNSwitchDctOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct
  )
{
  NBPtr->Dct = Dct & 1;
  NBPtr->SPDPtr = &(NBPtr->AllNodeSPDPtr[(NBPtr->MCTPtr->NodeId * MAX_DCTS_PER_DIE + Dct) * MAX_DIMMS_PER_CHANNEL]);
  NBPtr->DCTPtr = &(NBPtr->MCTPtr->DctData[NBPtr->Dct]);

  MemRecNSwitchChannelOr (NBPtr, 0);
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
MemRecNSwitchChannelOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Channel
  )
{
  NBPtr->Channel = 0;
  NBPtr->ChannelPtr = &(NBPtr->DCTPtr->ChData[0]);
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
MemRecNcmnGetSetTrainDlyOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  )
{
  UINT16 index;
  UINT16 offset;
  UINT32 value;
  UINT32 address;
  UINT8 DIMM;
  UINT8 Byte;
  UINT32 mask;

  DIMM = DRBN_DIMM (DrbnVar);
  Byte = DRBN_BYTE (DrbnVar);

  mask = 0xFF;

  switch (TrnDly) {
  case AccessRcvEnDly:
    mask = 0x1FF;
    index = 0x10;
    break;
  case AccessWrDqsDly:
    index = 0x30;
    break;
  case AccessWrDatDly:
    index = 0x01;
    break;
  case AccessRdDqsDly:
    index = 0x05;
    break;
  case AccessPhRecDly:
    index = 0x50;
    break;
  default:
    index = 0;
    IDS_ERROR_TRAP;
  }

  switch (TrnDly) {
  case AccessRcvEnDly:
  case AccessWrDqsDly:
    index += (DIMM * 3);
    if ((Byte & 0x04) != 0) {
      // if byte 4,5,6,7
      index += 0x10;
    }
    if ((Byte & 0x02) != 0) {
      // if byte 2,3,6,7
      index++;
    }
    offset = 16 * (Byte % 2);
    break;

  case AccessRdDqsDly:
  case AccessWrDatDly:
    index += (DIMM * 0x100);
    // break is not being used here because AccessRdDqsDly and AccessWrDatDly also need
    // to run AccessPhRecDly sequence.
  case AccessPhRecDly:
    index += (Byte / 4);
    offset = 8 * (Byte % 4);
    break;
  default:
    offset = 0;
    IDS_ERROR_TRAP;
  }

  address = index;
  MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, address);
  while (MemRecNGetBitFieldNb (NBPtr, BFDctAccessDone) == 0) {}
  value = MemRecNGetBitFieldNb (NBPtr, BFDctAddlDataReg);

  if (IsSet != 0) {
    if (TrnDly == AccessPhRecDly) {
      value = NBPtr->DctCachePtr->PhRecReg[index & 0x03];
    }

    value = ((UINT32)Field << offset) | (value & (~(mask << offset)));
    MemRecNSetBitFieldNb (NBPtr, BFDctAddlDataReg, value);
    address |= DCT_ACCESS_WRITE;
    MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, address);
    while (MemRecNGetBitFieldNb (NBPtr, BFDctAccessDone) == 0) {}

    if (TrnDly == AccessPhRecDly) {
      NBPtr->DctCachePtr->PhRecReg[index & 0x03] = value;
    }
  } else {
    value = (value >> offset) & mask;
  }

  return value;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets or sets a value to a bit field in a PCI register.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Name of Field to be set
 *     @param[in]   Field - Value to be programmed
 *     @param[in]   IsSet - Indicates if the function will set or get
 *
 *     @return      value read, if the function is used as a "get"
 */

UINT32
STATIC
MemRecNCmnGetSetFieldOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  )
{
  TSEFO Address;
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
      if ((Type == NB_ACCESS) && ((Address & 0xF000) == 0x2000)) {
        MemRecNSetBitFieldNb (NBPtr, BFDctCfgSel, NBPtr->Dct);
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
        // Address |= (((UINT32) (24 + 0)) << 15);
        PciAddr.AddressValue = Address;
        // Fix for MCM
        PciAddr.Address.Device = NBPtr->MemPtr->DiesPerSystem[NBPtr->Node].PciAddr.Address.Device;
        PciAddr.Address.Bus = NBPtr->MemPtr->DiesPerSystem[NBPtr->Node].PciAddr.Address.Bus;
        PciAddr.Address.Segment = NBPtr->MemPtr->DiesPerSystem[NBPtr->Node].PciAddr.Address.Segment;
        Address = PciAddr.AddressValue;
        LibAmdPciRead (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
        IDS_HDT_CONSOLE (MEM_GETREG, "~Dev%x Dct%d Fn%d_%03x = %x\n",
                         NBPtr->PciAddr.Address.Device, NBPtr->Dct,
                         (Address >> 12) & 0xF, Address & 0xFFF, Value);
      } else if (Type == DCT_PHY_ACCESS) {
        MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
        while (MemRecNGetBitFieldNb (NBPtr, BFDctAccessDone) == 0) {}

        Value = MemRecNGetBitFieldNb (NBPtr, BFDctAddlDataReg);
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
          IDS_HDT_CONSOLE (MEM_SETREG, "~Dev%x Dct%d Fn%d_%03x [%d:%d] = %x\n",
                           NBPtr->PciAddr.Address.Device, NBPtr->Dct,
                           (Address >> 12) & 0xF, Address & 0xFFF, Highbit, Lowbit, Field);
        } else if (Type == DCT_PHY_ACCESS) {
          MemRecNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
          Address |= DCT_ACCESS_WRITE;

          MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
          while (MemRecNGetBitFieldNb (NBPtr, BFDctAccessDone) == 0) {}
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
*     @param[in,out]   *NBRegTable   - Pointer to the NB Table *
 */

VOID
STATIC
MemRecNInitNBRegTableOr (
  IN OUT   TSEFO *NBRegTable
  )
{
  UINT16 i;
  for (i = 0; i <= BFEndOfList; i++) {
    NBRegTable[i] = 0;
  }

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x40), 31,  0, BFDramBaseReg0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x44), 31,  0, BFDramLimitReg0);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (1, 0x10C), 0,  0, BFDctCfgSel);

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

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31,  0, BFDramInitRegReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x80), 31,  0, BFDramBankAddrReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 31,  0, BFDramTimingLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 31,  0, BFDramConfigLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31,  0, BFDramConfigHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31,  0, BFDctAddlOffsetReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x9C), 31,  0, BFDctAddlDataReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31, 31, BFDctAccessDone);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 17,  0, BFMrsAddress);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 20, 18, BFMrsBank);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 23, 21, BFMrsChipSel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 26, 26, BFSendMrsCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 27, 27, BFDeassertMemRstX);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 28, 28, BFAssertCke);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 29, 29, BFSendZQCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 30, 30, BFSendCtrlWord);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 31, 31, BFEnDramInit);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C),  7,  7, BFLevel);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x7C), 12, 12, BFMrsQoff);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 17, 16, BFTref);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 18, 18, BFDisAutoRefresh);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 15, 12, BFX4Dimm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 16, 16, BFUnBuffDimm);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  4,  0, BFMemClkFreq);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  7,  7, BFMemClkFreqVal);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 11, 10, BFZqcsInterval);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 14, 14, BFDisDramInterface);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 21, 21, BFFreqChgInProg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  5,  5, BFSubMemclkRegDly);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 15,  8, BFCtrlWordCS);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110), 31, 0, BFDctSelBaseAddrReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x114), 31, 0, BFDctSelBaseOffsetReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x200), 31,  0, BFDramTiming0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x204), 31,  0, BFDramTiming1);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x208), 31,  0, BFDramTiming2);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x20C), 31,  0, BFDramTiming3);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x210), 31,  0, BFDramNBP0);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x210), 31, 22, BFMaxLatency);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x214), 31,  0, BFDramTiming4);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x218), 31,  0, BFDramTiming5);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x21C), 31,  0, BFDramTiming6);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x22C), 31,  0, BFDramTiming10);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x230), 31,  0, BFPhyRODTCSLow);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x234), 31,  0, BFPhyRODTCSHigh);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x238), 31,  0, BFPhyWODTCSLow);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x23C), 31,  0, BFPhyWODTCSHigh);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x240), 31,  0, BFDramODTCtlReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  3,  2, BFCmdTestEnable);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250),  7,  5, BFCmdType);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 10, 10, BFTestStatus);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 11, 11, BFSendCmd);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x250), 12, 12, BFCmdSendInProg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x254), 26, 24, BFTgtChipSelectA );

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x260), 20,  0, BFCmdCount);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x268), 17,  0, BFNibbleErrSts);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x270), 18,  0, BFDataPrbsSeed);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x28C), 31,  0, BFDramCmd2Reg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x160),  5,  1, BFNbFid);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x160),  7,  7, BFNbDid);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (5, 0x170), 14, 14, BFSwNbPstateLoDis);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 31,  0, BFODCControl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x04, 31,  0, BFAddrTmgControl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  0,  0, BFWrtLvTrEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  1,  1, BFWrtLvTrMode);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  5,  4, BFTrDimmSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 11,  8, BFWrLvOdt);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 12, 12, BFWrLvOdtEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 13, 13, BFDqsRcvTrEn);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0B, 31,  0, BFDramPhyStatusReg);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0C, 31, 16, BFPhyFence);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0FE013, _NOT_USED_, _NOT_USED_, BFPllRegWaitTime);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0FE006, _NOT_USED_, _NOT_USED_, BFPllLockTime);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0F0F1F, _NOT_USED_, _NOT_USED_, BFDataRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0F2F1F, _NOT_USED_, _NOT_USED_, BFClkRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0F4009, _NOT_USED_, _NOT_USED_, BFCmpVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0F8F1F, _NOT_USED_, _NOT_USED_, BFCmdRxVioLvl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0FC01F, _NOT_USED_, _NOT_USED_, BFAddrRxVioLvl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0F0F30, _NOT_USED_, _NOT_USED_, BFBlockRxDqsLock);
//DMACH
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS,  0x0D0FE003, _NOT_USED_, _NOT_USED_, BFDisablePredriverCal);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  2,  0, BFCkeDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00,  6,  4, BFCsOdtDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 10,  8, BFAddrCmdDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 14, 12, BFClkDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 18, 16, BFDataDrvStren);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 22, 20, BFDqsDrvStren);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D08E003, _NOT_USED_, _NOT_USED_, BFDisablePredriverCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F0F02, _NOT_USED_, _NOT_USED_, BFDataByteTxPreDriverCal);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F0F06, _NOT_USED_, _NOT_USED_, BFDataByteTxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F0F0A, _NOT_USED_, _NOT_USED_, BFDataByteTxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F8006, _NOT_USED_, _NOT_USED_, BFCmdAddr0TxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F800A, _NOT_USED_, _NOT_USED_, BFCmdAddr0TxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F8106, _NOT_USED_, _NOT_USED_, BFCmdAddr1TxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F810A, _NOT_USED_, _NOT_USED_, BFCmdAddr1TxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0FC006, _NOT_USED_, _NOT_USED_, BFAddrTxPreDriverCal2Pad1);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0FC00A, _NOT_USED_, _NOT_USED_, BFAddrTxPreDriverCal2Pad2);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0FC00E, _NOT_USED_, _NOT_USED_, BFAddrTxPreDriverCal2Pad3);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0FC012, _NOT_USED_, _NOT_USED_, BFAddrTxPreDriverCal2Pad4);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F8002, _NOT_USED_, _NOT_USED_, BFCmdAddr0TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F8102, _NOT_USED_, _NOT_USED_, BFCmdAddr1TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0FC002, _NOT_USED_, _NOT_USED_, BFAddrTxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F2002, _NOT_USED_, _NOT_USED_, BFClock0TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F2102, _NOT_USED_, _NOT_USED_, BFClock1TxPreDriverCalPad0);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x0D0F2202, _NOT_USED_, _NOT_USED_, BFClock2TxPreDriverCalPad0);
}

/*-----------------------------------------------------------------------------*/
/**
 *     MemRecNIsIdSupportedOr
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
STATIC
MemRecNIsIdSupportedOr (
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

