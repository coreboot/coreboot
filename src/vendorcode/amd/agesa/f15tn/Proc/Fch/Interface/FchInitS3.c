/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Initialization.
 *
 * Init IOAPIC/IOMMU/Misc NB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/

#include "FchPlatform.h"
#include "FchTaskLauncher.h"
#define FILECODE PROC_FCH_INTERFACE_FCHINITS3_FILECODE

extern FCH_TASK_ENTRY   *FchInitS3EarlyTaskTable[];
extern FCH_TASK_ENTRY   *FchInitS3LateTaskTable[];

VOID
FchInitS3EarlyRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  );

VOID
FchInitS3LateRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  );
/*----------------------------------------------------------------------------------------*/
/**
 * FchInitS3EarlyRestore - Config Fch before ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */

VOID
FchInitS3EarlyRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  )
{
  AGESA_STATUS    AgesaStatus;

  FchDataPtr->Misc.S3Resume = 1;
  AgesaStatus = FchTaskLauncher (&FchInitS3EarlyTaskTable[0], FchDataPtr, TpFchInitS3EarlyDispatching);
  FchDataPtr->Misc.S3Resume = 0;
}

/*----------------------------------------------------------------------------------------*/
/**
 * FchInitS3LateRestore - Config Fch after ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */

VOID
FchInitS3LateRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  )
{
  AGESA_STATUS    AgesaStatus;

  FchDataPtr->Misc.S3Resume = 1;
  AgesaStatus = FchTaskLauncher (&FchInitS3LateTaskTable[0], FchDataPtr, TpFchInitS3LateDispatching);
  FchDataPtr->Misc.S3Resume = 0;
}

