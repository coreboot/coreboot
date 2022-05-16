/* $NoKeywords:$ */
/**
 * @file
 *
 * NB services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "OptionGnb.h"
#include  "GnbSmuInitLibV7.h"
#include  "heapManager.h"
#include  "GnbFamServices.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBSCSLIBV1_GNBSCSLIBV1_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#ifndef TYPE_D0F0xBC
  #define TYPE_D0F0xBC 0x4
#endif

extern GNB_BUILD_OPTIONS GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
GnbSmuInitLibV7139_fun0 (
  IN       GNB_HANDLE           *GnbHandle,
  CONST IN       UINT8                *ScsDataPtr,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  DEV_OBJECT               DevObject;
  AGESA_STATUS             Status;

  IDS_PERF_TIMESTAMP (TP_BEGINGNBLOADSCSDATA, StdHeader);

  Status = AGESA_SUCCESS;
  if (GnbBuildOptions.CfgScsSupport == TRUE) {
    IDS_HDT_CONSOLE (GNB_TRACE, "GnbSmuInitLibV7139_fun0 Enter\n");
    //Check input parameters
    ASSERT ((ScsDataPtr != NULL) && (StdHeader != NULL));
    if ((ScsDataPtr == NULL) || (StdHeader == NULL)) {
      return AGESA_ERROR;
    }

    //Verify the SCS block signature
    ASSERT (*(UINT32 *)ScsDataPtr == GnbSmuInitLibV7136_macro0);
    if (*(UINT32 *)ScsDataPtr != GnbSmuInitLibV7136_macro0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Verify SCS Binary fail\n", ScsDataPtr);
      return AGESA_ERROR;
    }

    //Load SCS block
    IDS_HDT_CONSOLE (GNB_TRACE, "Load SCS @%08x\n", ScsDataPtr);
    DevObject.DevPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
    DevObject.GnbHandle = GnbHandle;
    DevObject.StdHeader = StdHeader;
    GnbSmuServiceRequestV7 (&DevObject, 0x50, (UINT32) (UINTN) ScsDataPtr, 0);

    //Get SCS result and save to Heap
    IDS_HDT_CONSOLE (GNB_TRACE, "Get SCS Result\n", ScsDataPtr);
    Status = GnbSmuInitLibV7139_fun1 (GnbHandle, StdHeader);

    IDS_HDT_CONSOLE (GNB_TRACE, "Get SCS Result %s\n", (Status == AGESA_SUCCESS) ? "Success" : "Fail");
  }

  IDS_PERF_TIMESTAMP (TP_ENDGNBLOADSCSDATA, StdHeader);
  return Status;
}

AGESA_STATUS
GnbSmuInitLibV7139_fun1 (
  IN       GNB_HANDLE           *GnbHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT32                      i;
  UINT32                      ScsResultAddr;
  UINT32                      *ScsResultBuf;
  UINT32                      NumApmWeights;
  AGESA_STATUS                Status;
  DEV_OBJECT                  DevObject;
  GNB_REGISTER_SERVICE        *GnbRegisterAccessProtocol;
  ALLOCATE_HEAP_PARAMS        AllocateHeapParams;

  Status = AGESA_SUCCESS;
  DevObject.GnbHandle = GnbHandle;
  DevObject.StdHeader = StdHeader;
  DevObject.DevPciAddress = GnbGetHostPciAddress (GnbHandle);

  Status = GnbLibLocateService (GnbRegisterAccessService, GnbGetSocketId (GnbHandle), (CONST VOID **)&GnbRegisterAccessProtocol, StdHeader);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }

  //Get the command result (A pointer to SMURAM)
  ScsResultAddr = 0;
  GnbUraGet (&DevObject, TRxSmuIntArgument, &ScsResultAddr);
  IDS_HDT_CONSOLE (GNB_TRACE, "SMURAMAddr %08x ", ScsResultAddr);
  ASSERT (ScsResultAddr != 0);


  Status = GnbRegisterAccessProtocol->Read (GnbHandle, 0x4, ScsResultAddr + offsetof (SMU_RAM_CPU_INFO, CountApmWeights), &NumApmWeights, 0, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "NumApmWeights %d\n", NumApmWeights);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }

  //Allocate heap for store the result
  AllocateHeapParams.BufferHandle = AMD_SCS_SMU_RAM_INFO;
  AllocateHeapParams.Persist = HEAP_LOCAL_CACHE;
  AllocateHeapParams.BufferPtr = NULL;
  AllocateHeapParams.RequestedBufferSize = sizeof (SMU_RAM_CPU_INFO) + SIZE_OF_APMWEIGHTS * NumApmWeights;
  Status = HeapAllocateBuffer (&AllocateHeapParams, StdHeader);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }

  ScsResultBuf = (UINT32 *)AllocateHeapParams.BufferPtr;
  //Get the result point by RxSmuIntArgument from SMURAM
  for (i = 0; i < (sizeof (SMU_RAM_CPU_INFO) / sizeof (UINT32) + NumApmWeights); i++) {
    Status = GnbRegisterAccessProtocol->Read (GnbHandle, 0x4, ScsResultAddr, &ScsResultBuf[i], 0, StdHeader);
    ASSERT (Status == AGESA_SUCCESS);
    if (Status != AGESA_SUCCESS) {
      break;
    }
    ScsResultAddr += sizeof (UINT32);
  }

  if (Status != AGESA_SUCCESS) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Fail on get SCS Result @%08x\n", ScsResultAddr);
    HeapDeallocateBuffer (AMD_SCS_SMU_RAM_INFO, StdHeader);
  } else {
  }

  return AGESA_SUCCESS;
}
