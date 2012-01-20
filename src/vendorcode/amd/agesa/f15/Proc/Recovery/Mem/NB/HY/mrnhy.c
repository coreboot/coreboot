/* $NoKeywords:$ */
/**
 * @file
 *
 * mrnhy.c
 *
 * Common Northbridge  functions for Hydra Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 48317 $ @e \$Date: 2011-03-07 10:38:14 -0700 (Mon, 07 Mar 2011) $
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
#include "mrnhy.h"
#include "heapManager.h"
#include "AdvancedApi.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_RECOVERY_MEM_NB_HY_MRNHY_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_DIES_PER_SOCKET     2
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
MemRecNInitNBRegTableHy (
  IN OUT   TSEFO *NBRegTable
  );

UINT32
STATIC
MemRecNCmnGetSetFieldHy (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

UINT32
STATIC
MemRecNcmnGetSetTrainDlyHy (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

BOOLEAN
STATIC
MemRecNIsIdSupportedHy (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

VOID
STATIC
MemRecNSwitchNodeHy (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 NodeID
  );


/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
STATIC CONST UINT32 RecModeDefRegArrayHy[] = {
  BFDramControlReg,  0x320C2A06,
  BFDramBankAddrReg, 0x00001111,
  BFDramMRSReg,      0x000400A4,
  BFDramTimingLoReg, 0x000A0092,
  BFDramTimingHiReg, 0xB6D218FF,
  BFDramConfigLoReg, 0x00000000,
  BFDramConfigHiReg, 0x1F48010B,
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
 *     @return          TRUE  - This node is a Hydra and this NB block has been initialized
 *     @return          FALSE - This node is not a Hydra
 */

BOOLEAN
MemRecConstructNBBlockHY (
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
  if (!MemRecNIsIdSupportedHy (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
    return FALSE;
  }

  //
  // Allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
  //
  MCTPtr = &MemPtr->DiesPerSystem[NodeID];
  AllocHeapParams.RequestedBufferSize = MAX_DCTS_PER_NODE_HY * (
                                          sizeof (DCT_STRUCT) + (
                                            MAX_CHANNELS_PER_DCT_HY * (
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

  MemPtr->DieCount = MAX_DIES_PER_SOCKET;
  MCTPtr->DctCount = MAX_DCTS_PER_NODE_HY;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  MemPtr->DiesPerSystem[NodeID + 1].DctCount = MAX_DCTS_PER_NODE_HY;
  MemPtr->DiesPerSystem[NodeID + 1].DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += MAX_DCTS_PER_NODE_HY * sizeof (DCT_STRUCT);
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_HY; Dct++) {
    MCTPtr->DctData[Dct].Dct = Dct;
    MCTPtr->DctData[Dct].ChannelCount = MAX_CHANNELS_PER_DCT_HY;
    MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
    MCTPtr->DctData[Dct].ChData[0].ChannelID = (MCTPtr->DieId * 2) + Dct;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_HY * sizeof (CH_DEF_STRUCT);
    for (Channel = 0; Channel < MAX_CHANNELS_PER_DCT_HY; Channel++) {
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
  NBPtr->MCTPtr = MCTPtr;
  NBPtr->AllNodeMCTPtr = &MemPtr->DiesPerSystem[NodeID];
  NBPtr->SPDPtr = &MemPtr->SpdDataStructure[MemPtr->DiesPerSystem[NodeID].SocketId * MAX_CHANNELS_PER_SOCKET * MAX_DIMMS_PER_CHANNEL];
  NBPtr->AllNodeSPDPtr = &MemPtr->SpdDataStructure[MemPtr->DiesPerSystem[NodeID].SocketId * MAX_CHANNELS_PER_SOCKET * MAX_DIMMS_PER_CHANNEL];

  NBPtr->DctCachePtr = NBPtr->DctCache;

  MemRecNInitNBRegTableHy (NBPtr->NBRegTable);
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->VarMtrrHiMsk = MemRecGetVarMtrrHiMsk (&(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);
  LibAmdMemFill (NBPtr->IsSupported, FALSE, sizeof (NBPtr->IsSupported), &NBPtr->MemPtr->StdHeader);
  for (i = 0; i < NumberOfHooks; i++) {
    NBPtr->FamilySpecificHook[i] = (BOOLEAN (*) (MEM_NB_BLOCK *, VOID *)) MemRecDefTrue;
  }

  NBPtr->InitRecovery = MemRecNMemInitNb;

  NBPtr->RecModeDefRegArray = RecModeDefRegArrayHy;

  NBPtr->SwitchNodeRec = MemRecNSwitchNodeHy;
  NBPtr->SwitchDCT = MemRecNSwitchDctHy;
  NBPtr->SwitchChannel = MemRecNSwitchChannelHy;
  NBPtr->SetMaxLatency = MemRecNSetMaxLatencyNb;
  NBPtr->GetSysAddrRec = MemRecNGetMCTSysAddrNb;
  NBPtr->SendMrsCmd = MemRecNSendMrsCmdNb;
  NBPtr->sendZQCmd = MemRecNSendZQCmdNb;
  NBPtr->SetDramOdtRec = MemRecNSetDramOdtNb;

  NBPtr->GetBitField = MemRecNGetBitFieldNb;
  NBPtr->SetBitField = MemRecNSetBitFieldNb;
  NBPtr->GetTrainDly = MemRecNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemRecNSetTrainDlyNb;

  NBPtr->MemRecNCmnGetSetFieldNb = MemRecNCmnGetSetFieldHy;
  NBPtr->MemRecNcmnGetSetTrainDlyNb = MemRecNcmnGetSetTrainDlyHy;
  NBPtr->MemRecNSwitchDctNb = MemRecNSwitchDctHy;
  NBPtr->TrainingFlow = MemNRecTrainingFlowNb;
  NBPtr->MemRecNInitializeMctNb = MemRecNInitializeMctHy;
  NBPtr->MemRecNFinalizeMctNb = MemRecNFinalizeMctHy;
  NBPtr->IsSupported[DramModeAfterDimmPres] = TRUE;
  MemRecNSwitchDctHy (NBPtr, 0);

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
MemRecNSwitchNodeHy (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 NodeID
  )
{
  NBPtr->MCTPtr = &(NBPtr->AllNodeMCTPtr[NodeID]);
  NBPtr->Node = NodeID;
  NBPtr->MCTPtr->NodeId = NodeID;
  MemRecNSwitchDctHy (NBPtr, NBPtr->Dct);
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
MemRecNSwitchDctHy (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct
  )
{
  NBPtr->Dct = Dct & 1;
  NBPtr->SPDPtr = &(NBPtr->AllNodeSPDPtr[(NBPtr->MCTPtr->NodeId * MAX_DCTS_PER_DIE + Dct) * MAX_DIMMS_PER_CHANNEL]);
  NBPtr->DCTPtr = &(NBPtr->MCTPtr->DctData[NBPtr->Dct]);

  MemRecNSwitchChannelHy (NBPtr, 0);
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
MemRecNSwitchChannelHy (
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
MemRecNcmnGetSetTrainDlyHy (
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
  UINT8 Rank;
  UINT8 Byte;
  UINT8 Nibble;

  DIMM = DRBN_DIMM (DrbnVar);
  Rank = DRBN_RANK (DrbnVar);
  Byte = DRBN_BYTE (DrbnVar);
  Nibble = DRBN_NBBL (DrbnVar);

  switch (TrnDly) {
  case AccessRcvEnDly:
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
    if (Byte > 7) {
      index += 2;
    }
    offset = 16 * (Byte % 2);
    index |= (Rank << 8);
    index |= (Nibble << 9);
    break;

  case AccessRdDqsDly:
    Field &= ~ 0x0001;
  case AccessWrDatDly:
    index += (DIMM * 0x100);
    if (Nibble != 0) {
      if (Rank != 0) {
        index += 0xA0;
      } else {
        index += 0x70;
      }
    } else if (Rank != 0) {
      index += 0x60;
    }
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

    value = ((UINT32)Field << offset) | (value & (~((UINT32)0xFF << offset)));
    MemRecNSetBitFieldNb (NBPtr, BFDctAddlDataReg, value);
    address |= DCT_ACCESS_WRITE;
    MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, address);
    while (MemRecNGetBitFieldNb (NBPtr, BFDctAccessDone) == 0) {}

    if (TrnDly == AccessPhRecDly) {
      NBPtr->DctCachePtr->PhRecReg[index & 0x03] = value;
    }
  } else {
    value = (value >> offset) & 0xFF;
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
MemRecNCmnGetSetFieldHy (
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
        // Address |= (((UINT32) (24 + 0)) << 15);
        PciAddr.AddressValue = Address;
        // Fix for MCM
        PciAddr.Address.Device = NBPtr->MemPtr->DiesPerSystem[NBPtr->Node].PciAddr.Address.Device;
        PciAddr.Address.Bus = NBPtr->MemPtr->DiesPerSystem[NBPtr->Node].PciAddr.Address.Bus;
        PciAddr.Address.Segment = NBPtr->MemPtr->DiesPerSystem[NBPtr->Node].PciAddr.Address.Segment;
        Address = PciAddr.AddressValue;
        LibAmdPciRead (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
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
MemRecNInitNBRegTableHy (
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
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84),  9,  7, BFDramTerm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x84), 11, 10, BFDramTermDyn);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x88), 31,  0, BFDramTimingLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x8C), 31,  0, BFDramTimingHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 31,  0, BFDramConfigLoReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 31,  0, BFDramConfigHiReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31,  0, BFDctAddlOffsetReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x9C), 31,  0, BFDctAddlDataReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x98), 31, 31, BFDctAccessDone);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (3, 0xD4),  4,  0, BFNbFid);

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
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 16, 16, BFUnBuffDimm);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x90), 15, 12, BFX4Dimm);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  8,  8, BFDdr3Mode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94),  9,  9, BFLegacyBiosMode);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 11, 10, BFZqcsInterval);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x94), 14, 14, BFDisDramInterface);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  5,  5, BFSubMemclkRegDly);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8),  6,  6, BFOdtSwizzle);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0xA8), 15, 8, BFCtrlWordCS);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110),  8, 8, BFDramEnabled);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x110), 31, 0, BFDctSelBaseAddrReg);
  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x114), 31, 0, BFDctSelBaseOffsetReg);

  MAKE_TSEFO (NBRegTable, NB_ACCESS, _FN (2, 0x11C), 31,  0, BFMctCfgHiReg);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x00, 31,  0, BFODCControl);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x04, 31,  0, BFAddrTmgControl);

  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  0,  0, BFWrtLvTrEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  1,  1, BFWrtLvTrMode);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08,  5,  4, BFTrDimmSel);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 11,  8, BFWrLvOdt);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 12, 12, BFWrLvOdtEn);
  MAKE_TSEFO (NBRegTable, DCT_PHY_ACCESS, 0x08, 30, 30, BFDisAutoComp);

}

/*-----------------------------------------------------------------------------*/
/**
 *     MemRecNIsIdSupportedHy
 *      This function matches the CPU_LOGICAL_ID with certain criteria to
 *      determine if it is supported by this NBBlock.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *LogicalIdPtr - Pointer to the CPU_LOGICAL_ID
 *
 *     @return          TRUE -  This node is a Hydra.
 *     @return          FALSE - This node is not a Hydra.
 *
 */
BOOLEAN
STATIC
MemRecNIsIdSupportedHy (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  )
{
  if ((LogicalIdPtr->Revision & AMD_F10_HY_ALL) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}
