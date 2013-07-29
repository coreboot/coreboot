/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD binary block interface
 *
 * Contains the block entry function dispatcher
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Legacy
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*****************************************************************************
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
 */

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Dispatcher.h"
#include "Options.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LEGACY_PROC_DISPATCHER_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CONST DISPATCH_TABLE DispatchTable[];
extern AMD_MODULE_HEADER mCpuModuleID;

/*---------------------------------------------------------------------------------------*/
/**
 * The Dispatcher is the entry point into the AGESA software. It takes a function
 * number as entry parameter in order to invoke the published function
 *
 * @param[in,out]  ConfigPtr
 *
 * @return   AGESA Status.
 *
 */
AGESA_STATUS
CALLCONV
AmdAgesaDispatcher (
  IN OUT   VOID *ConfigPtr
  )
{
  AGESA_STATUS Status;
  IMAGE_ENTRY ImageEntry;
  MODULE_ENTRY  ModuleEntry;
  DISPATCH_TABLE *Entry;
  UINT32 ImageStart;
  UINT32 ImageEnd;
  CONST AMD_IMAGE_HEADER* AltImagePtr;

  Status = AGESA_UNSUPPORTED;
  ImageEntry = NULL;
  ModuleEntry = NULL;
  ImageStart = 0xFFF00000;
  ImageEnd = 0xFFFFFFFF;
  AltImagePtr = NULL;

  Entry = (DISPATCH_TABLE *) DispatchTable;
  while (Entry->FunctionId != 0) {
    if ((((AMD_CONFIG_PARAMS *) ConfigPtr)->Func) == Entry->FunctionId) {
      Status = Entry->EntryPoint (ConfigPtr);
      break;
    }
    Entry++;
  }

  // 2. Try next dispatcher if possible, and we have not already got status back
  if ((mCpuModuleID.NextBlock != NULL) && (Status == AGESA_UNSUPPORTED)) {
    ModuleEntry = (MODULE_ENTRY) (UINT32) mCpuModuleID.NextBlock->ModuleDispatcher;
    if (ModuleEntry != NULL) {
      Status = (*ModuleEntry) (ConfigPtr);
    }
  }

  // 3. If not this image specific function, see if we can find alternative image instead
  if (Status == AGESA_UNSUPPORTED) {
    if ((((AMD_CONFIG_PARAMS *)ConfigPtr)->AltImageBasePtr != 0xFFFFFFFF  ) || (((AMD_CONFIG_PARAMS *)ConfigPtr)->AltImageBasePtr != 0)) {
      ImageStart = ((AMD_CONFIG_PARAMS *)ConfigPtr)->AltImageBasePtr;
      ImageEnd = ImageStart + 4;
      // Locate/test image base that matches this component
      AltImagePtr = LibAmdLocateImage ((VOID *) (UINT32)ImageStart, (VOID *) (UINT32)ImageEnd, 4096, (CHAR8 *) AGESA_ID);
      if (AltImagePtr != NULL) {
        //Invoke alternative Image
        ImageEntry = (IMAGE_ENTRY) ((UINT32) AltImagePtr + AltImagePtr->EntryPointAddress);
        Status = (*ImageEntry) (ConfigPtr);
      }
    }
  }

  return (Status);
}

/*---------------------------------------------------------------------------------------*/
/**
 * The host environment interface of callout.
 *
 * @param[in]      Func
 * @param[in]      Data
 * @param[in,out]  ConfigPtr
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
CALLCONV
AmdAgesaCallout (
  IN       UINT32  Func,
  IN       UINT32  Data,
  IN OUT   VOID    *ConfigPtr
  )
{
  UINT32 Result;
  Result = AGESA_UNSUPPORTED;
  if (((AMD_CONFIG_PARAMS *) ConfigPtr)->CalloutPtr == NULL) {
    return Result;
  }

  Result = (((AMD_CONFIG_PARAMS *) ConfigPtr)->CalloutPtr) (Func, Data, ConfigPtr);
  return (Result);
}
