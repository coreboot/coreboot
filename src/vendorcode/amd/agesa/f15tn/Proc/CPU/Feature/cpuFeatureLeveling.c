/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Feature Leveling Function.
 *
 * Contains code to Level the Feature in a multi-socket system
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
 *----------------------------------------------------------------------------
 */


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuPostInit.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUFEATURELEVELING_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
STATIC
SaveFeatures (
  IN OUT   VOID *cpuFeatureListPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
WriteFeatures (
  IN OUT   VOID *cpuFeatureListPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
GetGlobalCpuFeatureListAddress (
     OUT   UINT64            **Address,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          P U B L I C     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  FeatureLeveling
 *
 *    CPU feature leveling. Set least common features set of all CPUs
 *
 *    @param[in,out]   StdHeader   - Pointer to AMD_CONFIG_PARAMS struct.
 *
 */
VOID
FeatureLeveling (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 BscSocket;
  UINT32 Ignored;
  UINT32 BscCoreNum;
  UINT32 Socket;
  UINT32 Core;
  UINT32 NumberOfSockets;
  UINT32 NumberOfCores;
  BOOLEAN *FirstTime;
  BOOLEAN *NeedLeveling;
  AGESA_STATUS IgnoredSts;
  CPU_FEATURES_LIST *globalCpuFeatureList;
  AP_TASK  TaskPtr;

  ASSERT (IsBsp (StdHeader, &IgnoredSts));

  GetGlobalCpuFeatureListAddress ((UINT64 **) &globalCpuFeatureList, StdHeader);
  FirstTime = (BOOLEAN *) ((UINT8 *) globalCpuFeatureList + sizeof (CPU_FEATURES_LIST));
  NeedLeveling = (BOOLEAN *) ((UINT8 *) globalCpuFeatureList + sizeof (CPU_FEATURES_LIST) + sizeof (BOOLEAN));

  *FirstTime = TRUE;
  *NeedLeveling = FALSE;

  LibAmdMemFill (globalCpuFeatureList, 0xFF, sizeof (CPU_FEATURES_LIST), StdHeader);
  IdentifyCore (StdHeader, &BscSocket, &Ignored, &BscCoreNum, &IgnoredSts);
  NumberOfSockets = GetPlatformNumberOfSockets ();

  TaskPtr.FuncAddress.PfApTaskI = SaveFeatures;
  TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (CPU_FEATURES_LIST);
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  TaskPtr.DataTransfer.DataPtr = globalCpuFeatureList;
  TaskPtr.DataTransfer.DataTransferFlags = DATA_IN_MEMORY;

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      if (Socket !=  BscSocket) {
        ApUtilRunCodeOnSocketCore ((UINT8)Socket, 0, &TaskPtr, StdHeader);
      }
    }
  }
  ApUtilTaskOnExecutingCore (&TaskPtr, StdHeader, NULL);

  if (*NeedLeveling) {
    TaskPtr.FuncAddress.PfApTaskI  = WriteFeatures;
    for (Socket = 0; Socket < NumberOfSockets; Socket++) {
      if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
        for (Core = 0; Core < NumberOfCores; Core++) {
          if ((Socket != BscSocket) || (Core != BscCoreNum)) {
            ApUtilRunCodeOnSocketCore ((UINT8)Socket, (UINT8)Core, &TaskPtr, StdHeader);
          }
        }
      }
    }
    ApUtilTaskOnExecutingCore (&TaskPtr, StdHeader, NULL);
  }
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L      F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  SaveFeatures
 *
 *    save least common features set of all CPUs
 *
 *    @param[in,out]  cpuFeatureListPtr  - Pointer to CPU Feature List.
 *    @param[in,out]  StdHeader          - Pointer to AMD_CONFIG_PARAMS struct.
 *
 */
VOID
STATIC
SaveFeatures (
  IN OUT   VOID *cpuFeatureListPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  FamilySpecificServices = NULL;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->SaveFeatures (FamilySpecificServices, cpuFeatureListPtr, StdHeader);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  WriteFeatures
 *
 *    Write out least common features set of all CPUs
 *
 *    @param[in,out]  cpuFeatureListPtr  - Pointer to CPU Feature List.
 *    @param[in,out]  StdHeader          - Pointer to AMD_CONFIG_PARAMS struct.
 *
 */
VOID
STATIC
WriteFeatures (
  IN OUT   VOID *cpuFeatureListPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  FamilySpecificServices = NULL;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->WriteFeatures (FamilySpecificServices, cpuFeatureListPtr, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 *  GetGlobalCpuFeatureListAddress
 *
 *   Determines the address in system DRAM that should be used for CPU feature leveling.
 *
 * @param[out] Address           Address to utilize
 * @param[in]  StdHeader         Config handle for library and services
 *
 *
 */
VOID
STATIC
GetGlobalCpuFeatureListAddress (
     OUT   UINT64            **Address,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 AddressValue;

  AddressValue = GLOBAL_CPU_FEATURE_LIST_TEMP_ADDR;

  *Address = (UINT64 *)(intptr_t)(AddressValue);
}
