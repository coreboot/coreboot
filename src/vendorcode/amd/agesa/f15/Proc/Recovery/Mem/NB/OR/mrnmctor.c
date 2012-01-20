/* $NoKeywords:$ */
/**
 * @file
 *
 * mrnmctor.c
 *
 * Northbridge OR MCT supporting functions Recovery
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
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mrnor.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_RECOVERY_MEM_NB_OR_MRNMCTOR_FILECODE
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

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is the Recovery memory configuration function for OR DDR3
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */

AGESA_STATUS
MemRecNMemInitOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  AGESA_STATUS Status;
  MEM_TECH_BLOCK *TechPtr;

  TechPtr = NBPtr->TechPtr;

  Status = AGESA_FATAL;
  if (TechPtr->DimmPresence (TechPtr)) {

    if (MemRecNAutoConfigNb (NBPtr)) {

      AGESA_TESTPOINT (TpProcMemPlatformSpecificConfig, &(NBPtr->MemPtr->StdHeader));
      if (MemRecNPlatformSpecOr (NBPtr)) {
        AgesaHookBeforeDramInitRecovery (0, NBPtr->MemPtr);
        AGESA_TESTPOINT (TpProcMemStartDcts, &(NBPtr->MemPtr->StdHeader));
        MemRecNStartupDCTOr (NBPtr);

        AGESA_TESTPOINT (TpProcMemMtrrConfiguration, &(NBPtr->MemPtr->StdHeader));
        MemRecNCPUMemRecTypingNb (NBPtr);

        AGESA_TESTPOINT (TpProcMemDramTraining, &(NBPtr->MemPtr->StdHeader));
        NBPtr->TrainingFlow (NBPtr);

        Status = AGESA_SUCCESS;
      }
    }
  }

  return Status;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
