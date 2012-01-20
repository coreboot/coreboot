/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA Input Structure Creation
 *
 * Contains AGESA input structure creation support.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Common
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "GeneralServices.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_COMMON_CREATESTRUCT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CONST FUNCTION_PARAMS_INFO FuncParamsInfo[];
extern CONST UINTN InitializerCount;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;

/*---------------------------------------------------------------------------------------*/
/**
 * Allocate and initialize Config headers and Service Interface structures.
 *
 * This function will be called for each AGESA public APIs.
 * This function will do the following:
 * -# Locate the AGESA API structure parameters initializer function information.
 * -# Find the size of the structure that gets passed to each public APIs as
 *    the entry parameter. Allocate heap space using the size for PreMemHeap, callout for
 *    memory allocation for PostMemDram, and just set the config and service interface
 *    pointers for ByHost.
 * -# If the allocation is not ByHost, copy the AmdConfigParams into the newly created AmdConfigParams.
 *    For ByHost, we're using the caller's existing config params.
 * -# Call the initializer function, and pass a reference to the Config params and to
 *    the Service Interface struct.  On return the constructor will have filled the
 *    remaining structure with default values.
 * -# Fill the remaining info in the newly created structure on heap in AMD_CONFIG_PARAMS
 *    area (i.e. Fill *newStructPtr with the pointer to the newly created structure)
 * -# Set the appropriate AGESA function number in the StdHeader member of the input
 *    parameter structure.
 *
 * @param[in,out]  InterfaceParams         Pointer to structure containing the function call
 *                                         whose parameter structure is to be created, the
 *                                         allocation method, and a pointer to the newly
 *                                         created structure.
 *
 * @retval         AGESA_SUCCESS           The interface struct is allocated and initialized.
 * @retval         AGESA_UNSUPPORTED       The Service is not supported.
 *
 */
AGESA_STATUS
AmdCreateStruct (
  IN OUT   AMD_INTERFACE_PARAMS *InterfaceParams
  )
{
  UINTN                     ServiceIndex;
  ALLOCATE_HEAP_PARAMS      AllocHeapParams;
  AMD_CONFIG_PARAMS         *NewlyCreatedConfig;
  VOID                      *NewlyCreatedServiceInterface;
  AGESA_STATUS              AgesaStatus;
  AGESA_STATUS              TempStatus;
  AGESA_STATUS              IgnoredSts;
  CPU_SPECIFIC_SERVICES     *FamilySpecificServices;

  AgesaStatus = AGESA_SUCCESS;

  ASSERT (InterfaceParams != NULL);

  switch (InterfaceParams->AgesaFunctionName) {
  case AMD_INIT_RESET:
    if (!IsBsp (&InterfaceParams->StdHeader, &IgnoredSts)) {
      // APs must transfer their system core number from the mailbox to
      // a local register while it is still valid.
      GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &InterfaceParams->StdHeader);
      FamilySpecificServices->TransferApCoreNumber (FamilySpecificServices, &InterfaceParams->StdHeader);
    }
    InterfaceParams->StdHeader.HeapStatus = HEAP_DO_NOT_EXIST_YET;
    break;
  case AMD_INIT_EARLY:
  case AMD_INIT_RECOVERY:
  case AMD_INIT_RESUME:
  case AMD_INIT_POST:
    InterfaceParams->StdHeader.HeapStatus = HEAP_LOCAL_CACHE;
    break;
  case AMD_INIT_ENV:
    InterfaceParams->StdHeader.HeapStatus = HEAP_TEMP_MEM;
    break;
  case AMD_INIT_LATE:
  case AMD_INIT_MID:
  case AMD_S3_SAVE:
  case AMD_LATE_RUN_AP_TASK:
    InterfaceParams->StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
    break;
  case AMD_S3LATE_RESTORE:
    InterfaceParams->StdHeader.HeapStatus = HEAP_S3_RESUME;
    break;
  default:
    ASSERT (FALSE);
    InterfaceParams->StdHeader.HeapStatus = HEAP_LOCAL_CACHE;
    break;
  }

  InterfaceParams->StdHeader.HeapBasePtr = HeapGetBaseAddress (&InterfaceParams->StdHeader);

  if (InterfaceParams->AgesaFunctionName == AMD_INIT_RESET) {
    AgesaStatus = HeapManagerInit (&InterfaceParams->StdHeader);
  }

  // Step 1
  for (ServiceIndex = 0; ServiceIndex < InitializerCount; ServiceIndex++) {
    if (FuncParamsInfo[ServiceIndex].AgesaFunctionName == InterfaceParams->AgesaFunctionName) {
      break;
    }
  }
  if (ServiceIndex >= InitializerCount) {
    // A call was made to AGESA with an invalid function number.  This wrapper error may be due to the build target
    // not containing the desired entry point.
    return AGESA_UNSUPPORTED;
  }

  // Step 2
  LibAmdMemFill (&AllocHeapParams, 0, (UINTN) (sizeof (ALLOCATE_HEAP_PARAMS)), &InterfaceParams->StdHeader);

  if (InterfaceParams->AllocationMethod < ByHost) {
    // Allocate one buffer to contain the config params and the service struct.
    // The service struct begins immediately after the config params.
    AllocHeapParams.RequestedBufferSize = FuncParamsInfo[ServiceIndex].CreateStructSize + sizeof (AMD_CONFIG_PARAMS);
    AllocHeapParams.BufferHandle = FuncParamsInfo[ServiceIndex].BufferHandle;
    AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
    TempStatus = HeapAllocateBuffer (&AllocHeapParams, &(InterfaceParams->StdHeader));
    AgesaStatus = ((AgesaStatus > TempStatus) ? AgesaStatus : TempStatus);
    NewlyCreatedConfig = (AMD_CONFIG_PARAMS *)AllocHeapParams.BufferPtr;
    NewlyCreatedConfig++;
    NewlyCreatedServiceInterface =  NewlyCreatedConfig;
    NewlyCreatedConfig = (AMD_CONFIG_PARAMS *)AllocHeapParams.BufferPtr;
  } else {
    // The caller (example, agesa basic interface implementation) already has a buffer to use.
    NewlyCreatedConfig = (AMD_CONFIG_PARAMS *)InterfaceParams;
    NewlyCreatedServiceInterface = InterfaceParams->NewStructPtr;
    ASSERT (InterfaceParams->NewStructSize >= FuncParamsInfo[ServiceIndex].CreateStructSize);
  }
  ASSERT (NewlyCreatedConfig != NULL);
  ASSERT (NewlyCreatedServiceInterface != NULL);

  // Step 3
  if (InterfaceParams->AllocationMethod != ByHost) {
    *NewlyCreatedConfig = InterfaceParams->StdHeader;
  }

  // Step 4
  TempStatus = FuncParamsInfo[ServiceIndex].AgesaFunction (NewlyCreatedConfig, NewlyCreatedServiceInterface);
  AgesaStatus = ((AgesaStatus > TempStatus) ? AgesaStatus : TempStatus);

  // Step 5
  if (InterfaceParams->AllocationMethod != ByHost) {
    InterfaceParams->NewStructPtr = (VOID *) NewlyCreatedServiceInterface;
    InterfaceParams->NewStructSize = FuncParamsInfo[ServiceIndex].CreateStructSize;
  }

  // Step 6
  ((AMD_CONFIG_PARAMS *) InterfaceParams->NewStructPtr)->Func = InterfaceParams->AgesaFunctionName;
  return AgesaStatus;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Clears storage space from allocation for a parameter block of an
 *  AGESA software call entry.
 *
 *  @param[in,out]  InterfaceParams         Pointer to structure containing the function call
 *                                          whose parameter structure is to be deallocated.
 *
 *  @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
AmdReleaseStruct (
  IN OUT   AMD_INTERFACE_PARAMS *InterfaceParams
  )
{
  UINT8 i;
  UINT8 *BufferPtr;
  VOID  *ServicePtr;
  AGESA_STATUS  AgesaStatus;
  AGESA_STATUS  TempStatus;
  LOCATE_HEAP_PTR LocHeap;

  AgesaStatus = AGESA_SUCCESS;

  switch (InterfaceParams->AgesaFunctionName) {
  case AMD_INIT_RESET:
  case AMD_INIT_EARLY:
  case AMD_INIT_RECOVERY:
  case AMD_INIT_RESUME:
    InterfaceParams->StdHeader.HeapStatus = HEAP_LOCAL_CACHE;
    break;
  case AMD_INIT_POST:
    InterfaceParams->StdHeader.HeapStatus = HEAP_TEMP_MEM;
    break;
  case AMD_INIT_ENV:
  case AMD_INIT_LATE:
  case AMD_INIT_MID:
  case AMD_S3_SAVE:
  case AMD_LATE_RUN_AP_TASK:
    InterfaceParams->StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
    break;
  case AMD_S3LATE_RESTORE:
    InterfaceParams->StdHeader.HeapStatus = HEAP_S3_RESUME;
    break;
  default:
    ASSERT (FALSE);
    InterfaceParams->StdHeader.HeapStatus = HEAP_LOCAL_CACHE;
    break;
  }

  InterfaceParams->StdHeader.HeapBasePtr = HeapGetBaseAddress (&InterfaceParams->StdHeader);

// Step 1
  for (i = 0; i < InitializerCount; i++) {
    if (FuncParamsInfo[i].AgesaFunctionName == InterfaceParams->AgesaFunctionName) {
      break;
    }
  }
  if (i >= InitializerCount) {
    return AGESA_BOUNDS_CHK;
  }

  // Step 2
  if (InterfaceParams->AllocationMethod < ByHost) {
    LocHeap.BufferHandle = FuncParamsInfo[i].BufferHandle;
    if (HeapLocateBuffer (&LocHeap, &(InterfaceParams->StdHeader)) == AGESA_SUCCESS) {
      BufferPtr = (UINT8 *) LocHeap.BufferPtr;
      ServicePtr = &BufferPtr[sizeof (AMD_CONFIG_PARAMS)];
      TempStatus = FuncParamsInfo[i].AgesaDestructor (&(InterfaceParams->StdHeader), ServicePtr);
      AgesaStatus = ((AgesaStatus > TempStatus) ? AgesaStatus : TempStatus);
    }
  }

  // Step 3
  if (InterfaceParams->AllocationMethod < ByHost) {
    TempStatus = HeapDeallocateBuffer (FuncParamsInfo[i].BufferHandle, &(InterfaceParams->StdHeader));
    AgesaStatus = ((AgesaStatus > TempStatus) ? AgesaStatus : TempStatus);
  } else {
    // Unless we define service specific destructors, nothing to do for ByHost.
    return AGESA_SUCCESS;
  }
  return AgesaStatus;
}
