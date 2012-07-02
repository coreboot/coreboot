/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD WHEA Table Creation API, and related functions.
 *
 * Contains code that produce the ACPI WHEA related information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "OptionWhea.h"
#include "cpuLateInit.h"
#include "heapManager.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FEATURE_CPUWHEA_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *---------------------------------------------------------------------------------------
 */

extern OPTION_WHEA_CONFIGURATION OptionWheaConfiguration;  // global user config record

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
CreateHestBank (
  IN       AMD_HEST_BANK *HestBankPtr,
  IN       UINT8 BankNum,
  IN       AMD_WHEA_INIT_DATA *WheaInitDataPtr
  );

AGESA_STATUS
GetAcpiWheaStub (
  IN OUT   AMD_CONFIG_PARAMS    *StdHeader,
  IN OUT   VOID                 **WheaMcePtr,
  IN OUT   VOID                 **WheaCmcPtr
  );

AGESA_STATUS
GetAcpiWheaMain (
  IN OUT   AMD_CONFIG_PARAMS    *StdHeader,
  IN OUT   VOID                 **WheaMcePtr,
  IN OUT   VOID                 **WheaCmcPtr
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*---------------------------------------------------------------------------------------*/
/**
 *
 * It will create the ACPI table of WHEA and return the pointer to the table.
 *
 *    @param[in, out]  StdHeader        Standard Head Pointer
 *    @param[in, out]  WheaMcePtr       Point to Whea Hest Mce table
 *    @param[in, out]  WheaCmcPtr       Point to Whea Hest Cmc table
 *
 *    @retval         AGESA_STATUS
 */
AGESA_STATUS
CreateAcpiWhea (
  IN OUT   AMD_CONFIG_PARAMS    *StdHeader,
  IN OUT   VOID                 **WheaMcePtr,
  IN OUT   VOID                 **WheaCmcPtr
  )
{
  AGESA_TESTPOINT (TpProcCpuEntryWhea, StdHeader);
  return ((*(OptionWheaConfiguration.WheaFeature)) (StdHeader, WheaMcePtr, WheaCmcPtr));
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This is the default routine for use when the WHEA option is NOT requested.
 *
 * The option install process will create and fill the transfer vector with
 * the address of the proper routine (Main or Stub). The link optimizer will
 * strip out of the .DLL the routine that is not used.
 *
 *    @param[in, out]  StdHeader        Standard Head Pointer
 *    @param[in, out]  WheaMcePtr       Point to Whea Hest Mce table
 *    @param[in, out]  WheaCmcPtr       Point to Whea Hest Cmc table
 *
 *    @retval         AGESA_STATUS
 */

AGESA_STATUS
GetAcpiWheaStub (
  IN OUT   AMD_CONFIG_PARAMS    *StdHeader,
  IN OUT   VOID                 **WheaMcePtr,
  IN OUT   VOID                 **WheaCmcPtr
  )
{
  return  AGESA_UNSUPPORTED;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * It will create the ACPI tale of WHEA and return the pointer to the table.
 *
 *    @param[in, out]  StdHeader        Standard Head Pointer
 *    @param[in, out]  WheaMcePtr       Point to Whea Hest Mce table
 *    @param[in, out]  WheaCmcPtr       Point to Whea Hest Cmc table
 *
 *    @retval         UINT32  AGESA_STATUS
 */
AGESA_STATUS
GetAcpiWheaMain (
  IN OUT   AMD_CONFIG_PARAMS    *StdHeader,
  IN OUT   VOID                 **WheaMcePtr,
  IN OUT   VOID                 **WheaCmcPtr
  )
{
  UINT8  BankNum;
  UINT8  Entries;
  UINT16 HestMceTableSize;
  UINT16 HestCmcTableSize;
  UINT64 MsrData;
  AMD_HEST_MCE_TABLE *HestMceTablePtr;
  AMD_HEST_CMC_TABLE *HestCmcTablePtr;
  AMD_HEST_BANK *HestBankPtr;
  AMD_WHEA_INIT_DATA *WheaInitDataPtr;
  ALLOCATE_HEAP_PARAMS AllocParams;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  FamilySpecificServices = NULL;

  IDS_HDT_CONSOLE (CPU_TRACE, "  WHEA is created\n");

  // step 1: calculate Hest table size
  LibAmdMsrRead (MSR_MCG_CAP, &MsrData, StdHeader);
  BankNum = (UINT8) (((MSR_MCG_CAP_STRUCT *) (&MsrData))->Count);
  if (BankNum == 0) {
    return AGESA_ERROR;
  }

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetWheaInitData (FamilySpecificServices, (CONST VOID **) &WheaInitDataPtr, &Entries, StdHeader);

  ASSERT (WheaInitDataPtr->HestBankNum <= BankNum);

  HestMceTableSize = sizeof (AMD_HEST_MCE_TABLE) + WheaInitDataPtr->HestBankNum * sizeof (AMD_HEST_BANK);
  HestCmcTableSize = sizeof (AMD_HEST_CMC_TABLE) + WheaInitDataPtr->HestBankNum * sizeof (AMD_HEST_BANK);

  HestMceTablePtr = (AMD_HEST_MCE_TABLE *) *WheaMcePtr;
  HestCmcTablePtr = (AMD_HEST_CMC_TABLE *) *WheaCmcPtr;

  // step 2: allocate a buffer by callback function
  if ((HestMceTablePtr == NULL) || (HestCmcTablePtr == NULL)) {
    AllocParams.RequestedBufferSize = (UINT32) (HestMceTableSize + HestCmcTableSize);
    AllocParams.BufferHandle = AMD_WHEA_BUFFER_HANDLE;
    AllocParams.Persist = HEAP_SYSTEM_MEM;

    AGESA_TESTPOINT (TpProcCpuBeforeAllocateWheaBuffer, StdHeader);
    if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
      return AGESA_ERROR;
    }
    AGESA_TESTPOINT (TpProcCpuAfterAllocateWheaBuffer, StdHeader);

    HestMceTablePtr = (AMD_HEST_MCE_TABLE *) AllocParams.BufferPtr;
    HestCmcTablePtr = (AMD_HEST_CMC_TABLE *) ((UINT8 *) (HestMceTablePtr + 1) + (WheaInitDataPtr->HestBankNum * sizeof (AMD_HEST_BANK)));
  }

  // step 3: fill in Hest MCE table
  HestMceTablePtr->TblLength = HestMceTableSize;
  HestMceTablePtr->GlobCapInitDataLSD = WheaInitDataPtr->GlobCapInitDataLSD;
  HestMceTablePtr->GlobCapInitDataMSD = WheaInitDataPtr->GlobCapInitDataMSD;
  HestMceTablePtr->GlobCtrlInitDataLSD = WheaInitDataPtr->GlobCtrlInitDataLSD;
  HestMceTablePtr->GlobCtrlInitDataMSD = WheaInitDataPtr->GlobCtrlInitDataMSD;
  HestMceTablePtr->NumHWBanks = WheaInitDataPtr->HestBankNum;

  HestBankPtr = (AMD_HEST_BANK *) (HestMceTablePtr + 1);
  CreateHestBank (HestBankPtr, WheaInitDataPtr->HestBankNum, WheaInitDataPtr);

  // step 4: fill in Hest CMC table
  HestCmcTablePtr->NumHWBanks = WheaInitDataPtr->HestBankNum;
  HestCmcTablePtr->TblLength = HestCmcTableSize;

  HestBankPtr = (AMD_HEST_BANK *) (HestCmcTablePtr + 1);
  CreateHestBank (HestBankPtr, WheaInitDataPtr->HestBankNum, WheaInitDataPtr);

  // step 5: fill in the incoming structure
  *WheaMcePtr = HestMceTablePtr;
  *WheaCmcPtr = HestCmcTablePtr;

  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *
 * It will create Bank structure for Hest table
 *
 *    @param[in]  HestBankPtr           Pointer to the Hest Back structure
 *    @param[in]  BankNum               The number of Bank
 *    @param[in]  WheaInitDataPtr     Pointer to the AMD_WHEA_INIT_DATA structure
 *
 */
VOID
STATIC
CreateHestBank (
  IN       AMD_HEST_BANK *HestBankPtr,
  IN       UINT8 BankNum,
  IN       AMD_WHEA_INIT_DATA *WheaInitDataPtr
  )
{
  UINT8 BankIndex;
  for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
    HestBankPtr->BankNum = BankIndex;
    HestBankPtr->ClrStatusOnInit = WheaInitDataPtr->ClrStatusOnInit;
    HestBankPtr->StatusDataFormat = WheaInitDataPtr->StatusDataFormat;
    HestBankPtr->ConfWriteEn = WheaInitDataPtr->ConfWriteEn;
    HestBankPtr->CtrlRegMSRAddr = WheaInitDataPtr->HestBankInitData[BankIndex].CtrlRegMSRAddr;
    HestBankPtr->CtrlInitDataLSD = WheaInitDataPtr->HestBankInitData[BankIndex].CtrlInitDataLSD;
    HestBankPtr->CtrlInitDataMSD = WheaInitDataPtr->HestBankInitData[BankIndex].CtrlInitDataMSD;
    HestBankPtr->StatRegMSRAddr = WheaInitDataPtr->HestBankInitData[BankIndex].StatRegMSRAddr;
    HestBankPtr->AddrRegMSRAddr = WheaInitDataPtr->HestBankInitData[BankIndex].AddrRegMSRAddr;
    HestBankPtr->MiscRegMSRAddr = WheaInitDataPtr->HestBankInitData[BankIndex].MiscRegMSRAddr;
    HestBankPtr++;
  }
}

