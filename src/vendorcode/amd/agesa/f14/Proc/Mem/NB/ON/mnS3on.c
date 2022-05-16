/* $NoKeywords:$ */
/**
 * @file
 *
 * mns3on.c
 *
 * ON memory specific function to support S3 resume
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
#include "S3.h"
#include "mfs3.h"
#include "mnon.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "GeneralServices.h"
#include "cpuCommonF14Utilities.h"
#include "mnS3on.h"
#include "heapManager.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_ON_MNS3ON_FILECODE

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
UINT16
STATIC
MemNS3GetRegLstPtrON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   DESCRIPTOR_GROUP *DescriptPtr
  );

AGESA_STATUS
STATIC
MemNS3GetDeviceRegLstON (
  IN       UINT32 RegisterLstID,
     OUT   VOID **RegisterHeader
  );

VOID
STATIC
MemNS3SetDfltPllLockTimeON (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

BOOLEAN
STATIC
MemNS3ChangeNbFrequencyWrapON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 NBPstate
  );

VOID
STATIC
MemNS3GetConPCIMaskON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   DESCRIPTOR_GROUP *DescriptPtr
  );

BOOLEAN
MemS3ResumeConstructNBBlockON (
  IN OUT   VOID *S3NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  );

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
CONST PCI_SPECIAL_CASE PciSpecialCaseFuncON[] = {
  {MemNS3GetCSRNb, MemNS3SetCSRNb},
  {MemNS3GetBitFieldNb, MemNS3SetBitFieldNb},
  {MemNS3DisNbPsDbgNb, MemNS3DisNbPsDbgNb},
  {MemNS3EnNbPsDbg1Nb, MemNS3EnNbPsDbg1Nb},
  { (VOID (*) (ACCESS_WIDTH, PCI_ADDR, VOID *, VOID *)) memDefRet, MemNS3SetDfltPllLockTimeON},
  { (VOID (*) (ACCESS_WIDTH, PCI_ADDR, VOID *, VOID *)) memDefRet, MemNS3SetDisAutoCompUnb},
  { (VOID (*) (ACCESS_WIDTH, PCI_ADDR, VOID *, VOID *)) memDefRet, MemNS3SetDynModeChangeNb},
  {MemNS3GetBitFieldNb, MemNS3SetPreDriverCalUnb}
};

CONST PCI_REG_DESCRIPTOR ROMDATA S3PciPreSelfRefDescriptorON[] = {
  {{0, 2, 0}, FUNC_2, 0x110, 0x00000708},
  {{0, 0, 0}, FUNC_1, 0x40,  0x0FFF0003},
  {{0, 0, 0}, FUNC_1, 0x44,  0xFFFF0000},
  {{0, 0, 0}, FUNC_1, 0xF0,  0xFF00FF81},
  {{0, 2, 0}, FUNC_2, 0x114, 0x00000200},
  {{0, 0, 0}, FUNC_2, 0x118, 0x0F00CFFF},
  {{0, 0, 0}, FUNC_2, 0x11C, 0x61CC507C},
  {{0, 0, 0}, FUNC_2, 0x40,  0x1FF83FED},
  {{0, 0, 0}, FUNC_2, 0x44,  0x1FF83FED},
  {{0, 0, 0}, FUNC_2, 0x48,  0x1FF83FED},
  {{0, 0, 0}, FUNC_2, 0x4C,  0x1FF83FED},
  {{0, 0, 0}, FUNC_2, 0x60,  0x1FF83FE0},
  {{0, 0, 0}, FUNC_2, 0x64,  0x1FF83FE0},
  {{0, 1, 0}, FUNC_2, 0x80,  0x000000FF},
  {{0, 0, 0}, FUNC_2, 0x84,  0x00FC2FFF},
  {{0, 0, 0}, FUNC_2, 0x88,  0xFF00000F},
  {{0, 0, 0}, FUNC_2, 0x8C,  0x03F7FCFF},
  {{0, 0, 0}, FUNC_2, 0x90,  0x0EF20003},
  {{0, 1, 0}, FUNC_2, 0xA4,  0x00000007},
  {{0, 0, 0}, FUNC_2, 0xA8,  0x0078FF1F},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x06), 0x00000F8F},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x16), 0x0000000F},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x40), 0x3F1F0F0F},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x41), 0x00070707},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x83), 0x00007177},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x180), 0x0F0F0F0F},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x182), 0x0F0F0F0F},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x200), 0x00001F0F},

  // Phy Initialization
  {{6, 3, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0B), 0},
  // 3. Phy voltage related
  {{1, 1, 1}, DCT0,   BFDataRxVioLvl, 0x00000018},
  {{1, 1, 1}, DCT0,   BFClkRxVioLvl, 0x00000018},
  {{1, 2, 1}, DCT0,   BFCmpVioLvl, 0x0000C000},
  {{1, 1, 1}, DCT0,   BFCmdRxVioLvl, 0x00000018},
  {{1, 1, 1}, DCT0,   BFAddrRxVioLvl, 0x00000018},
  // 4. Frequency Change
  {{4, 3, 1}, DCT0,   BFPllLockTime, 0},
  {{1, 2, 1}, DCT0,   BFDllCSRBisaTrimDByte, 0x7000},
  {{1, 2, 1}, DCT0,   BFDllCSRBisaTrimClk, 0x7000},
  {{1, 2, 1}, DCT0,   BFDllCSRBisaTrimCsOdt, 0x7000},
  {{1, 2, 1}, DCT0,   BFDllCSRBisaTrimAByte2, 0x7000},
  {{1, 2, 1}, DCT0,   BFReduceLoop, 0x6000},
  {{0, 0, 0}, FUNC_2, 0x94,  0xFFD1CC1F},
  //    NB Pstate Related Register for Pstate 0
  {{0, 0, 0}, FUNC_2, 0x78,  0xFFF63FCF},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x30), 0x00001FFF},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x31), 0x00001FFF},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x32), 0x00009F9F},
  //   Access NB Pstate 1
  {{3, 3, 1}, FUNC_6, 0x98, 0},
  //   NB Pstate Related Register for Pstate 1
  {{0, 0, 0}, FUNC_2, 0x78,  0xFFF63FCF},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x30), 0x00001FFF},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x31), 0x00001FFF},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_EXTRA_FLAG, 0, 0x32), 0x00009F9F},
  //   Disable Access to NB Pstate 1
  {{2, 3, 1}, FUNC_6, 0x98, 0},
  {{1, 1, 1}, DCT0,   BFMemClkFreqVal, 0},
  {{1, 2, 1}, DCT0,   BFPllLockTime, 0},
  // 5. Phy Fence
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0C),  0x7FFF3FFF},
  {{1, 2, 1}, DCT0,   BFDataFence2, 0x00007FFF},
  {{1, 1, 1}, DCT0,   BFClkFence2, 0x0000001F},
  {{1, 1, 1}, DCT0,   BFCmdFence2, 0x0000001F},
  {{1, 1, 1}, DCT0,   BFAddrFence2, 0x0000001F},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x00),  0x70777777},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x04),  0x003F3F3F},
  {{1, 1, 1}, DCT0,   BFDQOdt03, 0x70},
  {{1, 1, 1}, DCT0,   BFDQOdt47, 0x70},
  // 6. Phy Compensation Init
  {{5, 3, 1}, DCT0,   BFDisablePredriverCal, 0},
  {{1, 2, 1}, DCT0,   BFDataByteTxPreDriverCal2Pad1, 0},
  {{1, 2, 1}, DCT0,   BFDataByteTxPreDriverCal2Pad2, 0},
  {{7, 2, 1}, DCT0,   BFDataByteTxPreDriverCal, 0},
  {{1, 2, 1}, DCT0,   BFCmdAddr0TxPreDriverCal2Pad1, 0},
  {{1, 2, 1}, DCT0,   BFCmdAddr0TxPreDriverCal2Pad2, 0},
  {{1, 2, 1}, DCT0,   BFCmdAddr1TxPreDriverCal2Pad1, 0},
  {{1, 2, 1}, DCT0,   BFCmdAddr1TxPreDriverCal2Pad2, 0},
  {{1, 2, 1}, DCT0,   BFAddrTxPreDriverCal2Pad1, 0},
  {{1, 2, 1}, DCT0,   BFAddrTxPreDriverCal2Pad2, 0},
  {{1, 2, 1}, DCT0,   BFAddrTxPreDriverCal2Pad3, 0},
  {{1, 2, 1}, DCT0,   BFAddrTxPreDriverCal2Pad4, 0},
  {{7, 2, 1}, DCT0,   BFCmdAddr0TxPreDriverCalPad0, 0},
  {{7, 2, 1}, DCT0,   BFCmdAddr1TxPreDriverCalPad0, 0},
  {{7, 2, 1}, DCT0,   BFAddrTxPreDriverCalPad0, 0},
  {{7, 2, 1}, DCT0,   BFClock0TxPreDriverCalPad0, 0},
  {{7, 2, 1}, DCT0,   BFClock1TxPreDriverCalPad0, 0},

  {{1, 2, 1}, DCT0,   BFDisablePredriverCal, 0x00006000}
};

CONST PCI_REGISTER_BLOCK_HEADER ROMDATA S3PciPreSelfRefON = {
  0,
  ARRAY_SIZE(S3PciPreSelfRefDescriptorON),
  S3PciPreSelfRefDescriptorON,
  PciSpecialCaseFuncON
};

CONST CONDITIONAL_PCI_REG_DESCRIPTOR ROMDATA S3CPciPostSelfDescriptorON[] = {
  // DCT0
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x10),  0x01FF01FF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x11),  0x01FF01FF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x13),  0x01FF01FF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x14),  0x01FF01FF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x20),  0x01FF01FF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x21),  0x01FF01FF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x23),  0x01FF01FF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x24),  0x01FF01FF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x01),  0xFFFFFFFF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x02),  0xFFFFFFFF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x101), 0xFFFFFFFF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x102), 0xFFFFFFFF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x05),  0x3E3E3E3E, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x06),  0x3E3E3E3E, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x105), 0x3E3E3E3E, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x106), 0x3E3E3E3E, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x30),  0x00FF00FF, DCT0_DDR3_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x31),  0x00FF00FF, DCT0_DDR3_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x33),  0x00FF00FF, DCT0_DDR3_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x34),  0x00FF00FF, DCT0_DDR3_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x40),  0x00FF00FF, DCT0_DDR3_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x41),  0x00FF00FF, DCT0_DDR3_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x43),  0x00FF00FF, DCT0_DDR3_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x44),  0x00FF00FF, DCT0_DDR3_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0D),  0x037F037F, DCT0_MASK, ANY_DIMM_MASK},
  {{1, 1, 1}, DCT0,   BFPhyClkConfig0, 0x00000010, DCT0_MASK, ANY_DIMM_MASK},
  {{1, 1, 1}, DCT0,   BFPhyClkConfig1, 0x00000010, DCT0_MASK, ANY_DIMM_MASK},
  {{1, 1, 1}, DCT0,   BFPhy0x0D0F0F13Bit0to7, 0x00000083, DCT0_MASK, ANY_DIMM_MASK},
  {{1, 1, 1}, DCT0,   BFAddrCmdTri, 0x0000000B1, DCT0_MASK, ANY_DIMM_MASK},
  {{1, 2, 1}, DCT0,   BFLowPowerDrvStrengthEn, 0x00000100, DCT0_MASK, ANY_DIMM_MASK},
  {{1, 2, 1}, DCT0,   BFEnRxPadStandby, 0x000001000, DCT0_MASK, ANY_DIMM_MASK},
  {{1, 1, 1}, DCT0,   BFDisDllShutdownSR, 0x00000001, DCT0_MASK, ANY_DIMM_MASK},

  {{0, 0, 0}, FUNC_2, 0x1C0, 0x100000, DCT0_MASK, ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_3, 0x84, 0x00060006, DCT0_MASK, ANY_DIMM_MASK},
  {{0, 2, 0}, FUNC_4, 0x12C, 0x0000FFFF, DCT0_MASK, ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_4, 0x1A8, 0x3F000000, DCT0_MASK, ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_3, 0x188, 0x00400000, DCT0_MASK, ANY_DIMM_MASK},
  {{0, 2, 0}, FUNC_6, 0x78, 0x0000FF00, DCT0_MASK, ANY_DIMM_MASK},
  // Release NB P-state force
  {{0, 0, 0}, FUNC_6, 0x90, 0x50000000, DCT0_MASK, ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x118, 0x00080000, DCT0_MASK, ANY_DIMM_MASK}
};

CONST CPCI_REGISTER_BLOCK_HEADER ROMDATA S3CPciPostSelfRefON = {
  0,
  ARRAY_SIZE(S3CPciPostSelfDescriptorON),
  S3CPciPostSelfDescriptorON,
  PciSpecialCaseFuncON
};

CONST MSR_REG_DESCRIPTOR ROMDATA S3MSRPreSelfRefDescriptorON[] = {
  {{0, 0, 0}, 0xC0010010, 0x00000000007F0000},
  {{0, 0, 0}, 0xC001001A, 0x0000000FFF800000},
  {{0, 0, 0}, 0xC001001D, 0x0000000FFF800000},
  {{0, 0, 0}, 0xC001001F, 0x8480FC6A434243E0}
};

CONST MSR_REGISTER_BLOCK_HEADER ROMDATA S3MSRPreSelfRefON = {
  0,
  ARRAY_SIZE(S3MSRPreSelfRefDescriptorON),
  S3MSRPreSelfRefDescriptorON,
  NULL
};

VOID * CONST MemS3RegListON[] = {
  (VOID *)&S3PciPreSelfRefON,
  NULL,
  NULL,
  (VOID *)&S3CPciPostSelfRefON,
  (VOID *)&S3MSRPreSelfRefON,
  NULL,
  NULL,
  NULL
};

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the northbridge block for S3 resume
 *
 *     @param[in,out]   *S3NBPtr   - Pointer to MEM_NB_BLOCK.
 *     @param[in,out]   *MemPtr  - Pointer to MEM_DATA_STRUCT.
 *     @param[in]       NodeID   - Node ID of the target node.
 *
 *      @return         BOOLEAN
 *                         TRUE - This is the correct constructor for the targeted node.
 *                         FALSE - This isn't the correct constructor for the targeted node.
 */
BOOLEAN
MemS3ResumeConstructNBBlockON (
  IN OUT   VOID *S3NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  INT32 i;
  MEM_NB_BLOCK *NBPtr;

  NBPtr = ((S3_MEM_NB_BLOCK *)S3NBPtr)->NBPtr;

  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[NodeID].SocketId, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedON (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
    return FALSE;
  }

  NBPtr->MemPtr = MemPtr;
  NBPtr->MCTPtr = &(MemPtr->DiesPerSystem[NodeID]);
  NBPtr->PciAddr.AddressValue = MemPtr->DiesPerSystem[NodeID].PciAddr.AddressValue;
  MemNInitNBRegTableON (NBPtr, NBPtr->NBRegTable);
  NBPtr->Node = ((UINT8) NBPtr->PciAddr.Address.Device) - 24;
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->Ganged = FALSE;
  NBPtr->NodeCount = MAX_NODES_SUPPORTED_ON;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_ON;

  for (i = 0; i < EnumSize; i++) {
    NBPtr->IsSupported[i] = FALSE;
  }

  for (i = 0; i < NumberOfHooks; i++) {
    NBPtr->FamilySpecificHook[i] = (BOOLEAN (*) (MEM_NB_BLOCK *, VOID *)) memDefTrue;
  }

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &MemPtr->StdHeader);

  NBPtr->SwitchDCT = (VOID (*) (MEM_NB_BLOCK*, UINT8)) memDefRet;
  NBPtr->SwitchChannel = (VOID (*) (MEM_NB_BLOCK*, UINT8)) memDefRet;
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldON;
  NBPtr->GetBitField = MemNGetBitFieldNb;
  NBPtr->SetBitField = MemNSetBitFieldNb;
  NBPtr->MemNIsIdSupportedNb = MemNIsIdSupportedON;
  NBPtr->ChangeNbFrequencyWrap = MemNS3ChangeNbFrequencyWrapON;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3ExitSelfRefReg = (VOID (*) (MEM_NB_BLOCK *, AMD_CONFIG_PARAMS *)) memDefRet;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetConPCIMask = MemNS3GetConPCIMaskON;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetConMSRMask = (VOID (*) (MEM_NB_BLOCK *, DESCRIPTOR_GROUP *)) memDefRet;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3Resume = MemNS3ResumeClientNb;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3RestoreScrub = (VOID (*) (MEM_NB_BLOCK *, UINT8)) memDefRet;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetRegLstPtr = MemNS3GetRegLstPtrON;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetDeviceRegLst = MemNS3GetDeviceRegLstON;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3SpecialCaseHeapSize = 0;

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
 *   This function returns the conditional PCI device mask
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in, out]  *DescriptPtr - Pointer to DESCRIPTOR_GROUP
 *      @return         none
 */
VOID
STATIC
MemNS3GetConPCIMaskON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   DESCRIPTOR_GROUP *DescriptPtr
  )
{
  BIT_FIELD_NAME bitfield;
  UINT32 RegVal;
  UINT8 DimmMask;

  DimmMask = 0;
  for (bitfield = BFCSBaseAddr0Reg; bitfield <= BFCSBaseAddr3Reg; bitfield ++) {
    RegVal = MemNGetBitFieldNb (NBPtr, bitfield);
    if (RegVal & 0x1) {
      DimmMask |= (UINT8) (1 << (((bitfield - BFCSBaseAddr0Reg) >> 1) << 1));
    }
  }

  // Set mask before exit self refresh
  DescriptPtr->CPCIDevice[PRESELFREF].Mask1 = 1;
  // Set mask after exit self refresh
  DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 = 1;
  // Set DDR3 mask if Dimms present are DDR3
  DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 |= (DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 << 4);

  // Set dimm mask
  DescriptPtr->CPCIDevice[PRESELFREF].Mask2 = DimmMask;
  DescriptPtr->CPCIDevice[POSTSELFREF].Mask2 = DimmMask;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function returns the register list for each device for LN
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in, out]  *DescriptPtr - Pointer to DESCRIPTOR_GROUP
 *     @return          UINT16 - size of the device descriptor on the target node.
 */
UINT16
STATIC
MemNS3GetRegLstPtrON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   DESCRIPTOR_GROUP *DescriptPtr
  )
{
  UINT8 i;
  UINT16 Size;
  Size = 0;
  for (i = PRESELFREF; i <= POSTSELFREF; i ++) {
    DescriptPtr->PCIDevice[i].Type = (UINT8) (DEV_TYPE_PCI_PRE_ESR + i);
    DescriptPtr->PCIDevice[i].Node = NBPtr->Node;
    DescriptPtr->PCIDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((PCI_REGISTER_BLOCK_HEADER *) MemS3RegListON[PCI_LST_ESR_ON - PCI_LST_ESR_ON + i] != NULL) {
      DescriptPtr->PCIDevice[i].RegisterListID = PCI_LST_ESR_ON + i;
      Size += sizeof (PCI_DEVICE_DESCRIPTOR);
    }
    DescriptPtr->CPCIDevice[i].Type = (UINT8) (DEV_TYPE_CPCI_PRE_ESR + i);
    DescriptPtr->CPCIDevice[i].Node = NBPtr->Node;
    DescriptPtr->CPCIDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((CPCI_REGISTER_BLOCK_HEADER *) MemS3RegListON[CPCI_LST_ESR_ON - PCI_LST_ESR_ON + i] != NULL) {
      DescriptPtr->CPCIDevice[i].RegisterListID = CPCI_LST_ESR_ON + i;
      Size += sizeof (CONDITIONAL_PCI_DEVICE_DESCRIPTOR);
    }
    DescriptPtr->MSRDevice[i].Type = (UINT8)  (DEV_TYPE_MSR_PRE_ESR + i);
    DescriptPtr->MSRDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((MSR_REGISTER_BLOCK_HEADER *) MemS3RegListON[MSR_LST_ESR_ON - PCI_LST_ESR_ON + i] != NULL) {
      DescriptPtr->MSRDevice[i].RegisterListID = MSR_LST_ESR_ON + i;
      Size += sizeof (MSR_DEVICE_DESCRIPTOR);
    }
    DescriptPtr->CMSRDevice[i].Type = (UINT8) (DEV_TYPE_CMSR_PRE_ESR + i);
    DescriptPtr->CMSRDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((CMSR_REGISTER_BLOCK_HEADER *) MemS3RegListON[CMSR_LST_ESR_ON - PCI_LST_ESR_ON + i] != NULL) {
      DescriptPtr->CMSRDevice[i].RegisterListID = CMSR_LST_ESR_ON + i;
      Size += sizeof (CONDITIONAL_MSR_DEVICE_DESCRIPTOR);
    }
  }
  return Size;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function return the register list according to the register ID.
 *
 *     @param[in]   RegisterLstID - value of the Register list ID.
 *     @param[out]  **RegisterHeader - pointer to the address of the register list.
 *     @return      none
 */
AGESA_STATUS
STATIC
MemNS3GetDeviceRegLstON (
  IN       UINT32 RegisterLstID,
     OUT   VOID **RegisterHeader
  )
{
  if (RegisterLstID >= (sizeof (MemS3RegListON) / sizeof (VOID *))) {
    ASSERT(FALSE); // RegisterListID exceeded size of Register list
    return AGESA_FATAL;
  }
  if (MemS3RegListON[RegisterLstID] != NULL) {
    *RegisterHeader = MemS3RegListON[RegisterLstID];
    return AGESA_SUCCESS;
  }
  ASSERT(FALSE); // Device regiser list error
  return AGESA_FATAL;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function that set PllLockTime to default state.
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
STATIC
MemNS3SetDfltPllLockTimeON (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT16 RegValue;

  RegValue = 0x1838;
  MemNS3SetBitFieldNb (AccessS3SaveWidth16, Address, &RegValue, ConfigPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function is a wrapper to call a CPU routine to change NB P-state and
 *   update NB frequency.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]  *NBPstate - NB Pstate
 *
 *     @return          TRUE - Succeed
 *     @return          FALSE - Fail
 */

BOOLEAN
STATIC
MemNS3ChangeNbFrequencyWrapON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 NBPstate
  )
{
  BOOLEAN Status;
  UINT32  NBFreq;
  UINT32  Speed;

  Speed = MemNGetBitFieldNb (NBPtr, BFMemClkFreq);
  Status = F14NbPstateInit (((Speed + 6) * 3335) / 100,
                            Speed,
                            NBPstate,
                            &NBFreq,
                            &(NBPtr->MemPtr->StdHeader));

  return Status;
}
