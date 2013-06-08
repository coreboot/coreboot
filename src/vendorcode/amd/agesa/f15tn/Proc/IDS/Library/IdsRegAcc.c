/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug library Routines
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
 ******************************************************************************
 */
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Filecode.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "Ids.h"
#include "IdsLib.h"
#include "IdsRegAcc.h"

#define FILECODE PROC_IDS_LIBRARY_IDSREGACC_FILECODE

extern CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsRegAccessTbl[];

/**
 *
 *  set the MSR of AP
 *
 *  @param[in] PRegMsr  point to REG_MSR
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *
 **/
VOID
IdsRegSetMsrCmnTask (
  IN       IDS_REG_MSR  *PRegMsr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 value;

  LibAmdMsrRead (PRegMsr->MsrAddr, &value, StdHeader);
  value &= PRegMsr->AndMask;
  value |= PRegMsr->OrMask;
  LibAmdMsrWrite (PRegMsr->MsrAddr, &value, StdHeader);
}

/**
 *
 *
 *     IDS Common routine for RMW MSR for both ealry & later stage
 *     This routine can only be used when AP service have been established
 *
 *     @param[in]   PMsrReg     Point MSR reg structure, contain TimePoint, Socket, Core,address, andmask, ormask
 *     @param[in,out]   StdHeader    - The Pointer of AGESA Header
 *
 */
VOID
IdsRegSetMsr (
  IN       IDS_REG_MSR *PMsrReg,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AP_TASK ApTask;
  IDSAPLATETASK IdsLateTask;
  UINT32 NumberOfCores;
  UINT8 StartSocket;
  UINT8 EndSocket;
  UINT8 Socket;
  UINT8 StartCore;
  UINT8 EndCore;
  UINT8 Core;
  UINT32 BscCoreNum;
  UINT32 BscSocket;
  UINT32 IgnoredModule;
  UINT32 ApicIdOfCore;
  AGESA_STATUS IgnoredSts;
  IDS_REG_AP_SERVICE_TIMEPOINT TimePoint;

  TimePoint = PMsrReg->TimePoint;
  ASSERT ((TimePoint == IDS_REG_AP_SERVICE_EARLY) ||
          (TimePoint == IDS_REG_AP_SERVICE_POST) ||
          (TimePoint == IDS_REG_AP_SERVICE_LATE));

  IdentifyCore (StdHeader, &BscSocket, &IgnoredModule, &BscCoreNum, &IgnoredSts);
  IdsGetStartEndSocket (PMsrReg->Socket, &StartSocket, &EndSocket);
  //TaskPtr for both IDS_REG_AP_SERVICE_EARLY, IDS_REG_AP_SERVICE_POST
  ApTask.FuncAddress.PfApTaskI = (PF_AP_TASK_I)IdsRegSetMsrCmnTask;
  ApTask.ExeFlags = WAIT_FOR_CORE;
  ApTask.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (IDS_REG_MSR);
  ApTask.DataTransfer.DataPtr = PMsrReg;
  ApTask.DataTransfer.DataTransferFlags = 0;

  for (Socket = StartSocket; Socket <= EndSocket; Socket++) {
    if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
      if (PMsrReg->Core == IDS_ALL_CORE) {
        StartCore = 0;
        EndCore = (UINT8)NumberOfCores - 1;
      } else {
        StartCore = PMsrReg->Core;
        EndCore = PMsrReg->Core;
      }
      for (Core = StartCore; (Core <= EndCore) && (Core <= (NumberOfCores - 1)); Core++) {
        if ((Core == BscCoreNum) && (Socket == BscSocket)) {
          IdsRegSetMsrCmnTask (PMsrReg, StdHeader);
        } else {
          if (IsProcessorPresent (Socket, StdHeader)) {
            if (TimePoint == IDS_REG_AP_SERVICE_EARLY) {
              // At early stage, the AP's task has to be called by core 0, not by bsc
              IdsRunCodeOnCoreEarly (Socket, Core, &ApTask, StdHeader);
            } else if (TimePoint == IDS_REG_AP_SERVICE_POST) {
              ApUtilRunCodeOnSocketCore (Socket, Core, &ApTask, StdHeader);
            } else if (TimePoint == IDS_REG_AP_SERVICE_LATE) {
              IdsLateTask.ApTask = (PF_IDS_AP_TASK)IdsRegSetMsrCmnTask;
              IdsLateTask.ApTaskPara = PMsrReg;
              GetLocalApicIdForCore (Socket, Core, &ApicIdOfCore, StdHeader);
              IdsAgesaRunFcnOnApLate (ApicIdOfCore, &IdsLateTask, StdHeader);
            }
          }
        }
      }
    }
  }
}

/**
 *
 *
 *     IDS Common routine for set Memory bit field using memory sevice
 *
 *     @param[in]   PMemReg     Point to MEM reg structure
 *     @param[in,out]   NBPtr    - The Pointer of NBPtr
 *
 */

AGESA_STATUS
IdsRegSetMemBitField (
  IN       IDS_REG_MEM *PMemReg,
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 CurDct;
  UINT32 BfType;
  UINT32 BfIndex;
  UINT8 HighBit;
  UINT8 LowBit;
  UINT32 RegValue;
  UINT8 Dct;
  HighBit = 31;
  LowBit = 0;
  //Check if current module need set
  if ((PMemReg->Module == IDS_ALL_MODULE) || (PMemReg->Module == NBPtr->Node)) {
    //Save Current DCT
    CurDct = NBPtr->Dct;
    //Set BfType for MAKE_TSEFO
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      if ((PMemReg->Dct == IDS_ALL_DCT) || (PMemReg->Dct == Dct)) {
        if (PMemReg->Type == IDS_REG_MEM_NB) {
          BfType = NB_ACCESS;
          BfIndex = _FN (PMemReg->Addr.PciAddr.Func, PMemReg->Addr.PciAddr.Offset);
        } else if (PMemReg->Type == IDS_REG_MEM_PHY) {
          if ((PMemReg->Addr.Index & 0xFFF00000) == 0x0D000000) {
            BfType = DCT_PHY_DIRECT;
            //Dram debug PHY only support 16 bits access
            HighBit = 15;
            LowBit = 0;
          } else {
            BfType = DCT_PHY_ACCESS;
          }
          BfIndex = PMemReg->Addr.Index;
        } else if (PMemReg->Type == IDS_REG_MEM_EXTRA) {
          BfType = DCT_EXTRA;
          BfIndex = PMemReg->Addr.Index;
        } else {
          return AGESA_UNSUPPORTED;
        }
        MAKE_TSEFO (NBPtr->NBRegTable, BfType, BfIndex, HighBit, LowBit, BFIdsCmnMemReg);

        NBPtr->SwitchDCT (NBPtr, Dct);
        RegValue = NBPtr->GetBitField (NBPtr, BFIdsCmnMemReg);
        RegValue &= PMemReg->AndMask;
        RegValue |= PMemReg->OrMask;
        NBPtr->SetBitField (NBPtr, BFIdsCmnMemReg, RegValue);
      }
    }
    //Restore DCT
    NBPtr->SwitchDCT (NBPtr, CurDct);
  }
  return AGESA_SUCCESS;
}

/**
 *
 *
 *     IDS routine for set family specific register
 *
 *     @param[in]   PFamReg     Point Famreg structure, contain register define, and type
 *     @param[in,out]   StdHeader    - The Pointer of AGESA Header
 *
 */

VOID
IdsFamRegAccess (
  IN       IDS_FAM_REG *PFamReg,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_IDS_OPTION RegFamId;

  switch (PFamReg->Type) {
  case IDS_FAM_REG_TYPE_GMMX:
    RegFamId = IDS_FAM_REG_GMMX;
    break;
  default:
    RegFamId = IDS_OPTION_END;
  }
  if (RegFamId != IDS_OPTION_END) {
    IdsParseFeatTbl (RegFamId, IdsRegAccessTbl, &PFamReg->Reg, NULL, StdHeader);
  }
}

/**
 * Set PCI indirect registers
 *
 *
 *     @param[in]   PPciIndirectReg     Point to IDS_REG_PCI_INDIRECT structure
 *     @param[in,out]   StdHeader    - The Pointer of AGESA Header
 */

VOID
IdsRegSetPciIndirect (
  IN       IDS_REG_PCI_INDIRECT *PPciIndirectReg,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 IndexOffset;
  UINT32 IndexValue;
  UINT32 Value;
  PCI_ADDR PciIndexPortAddr;
  PCI_ADDR PciDataPortAddr;

  IndexOffset = LibAmdAccessWidth (PPciIndirectReg->Width);
  PciIndexPortAddr.AddressValue = PPciIndirectReg->PciAddr;
  PciDataPortAddr.AddressValue = PPciIndirectReg->PciAddr + IndexOffset;
  //Read
  LibAmdPciWrite (PPciIndirectReg->Width, PciIndexPortAddr, &PPciIndirectReg->IndirectRegOff, StdHeader);
  LibAmdPciRead (PPciIndirectReg->Width, PciDataPortAddr, &Value, StdHeader);
  //Modify
  IdsLibDataMaskSet32 (&Value, PPciIndirectReg->AndMask, PPciIndirectReg->OrMask);
  //Write
  IndexValue = PPciIndirectReg->IndirectRegOff | PPciIndirectReg->WriteEnBit;
  LibAmdPciWrite (PPciIndirectReg->Width, PciIndexPortAddr, &IndexValue, StdHeader);
  LibAmdPciWrite (PPciIndirectReg->Width, PciDataPortAddr, &Value, StdHeader);
}


