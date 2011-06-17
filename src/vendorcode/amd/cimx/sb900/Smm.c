/**
 * @file
 *
 * Southbridge SMM service function
 *
 * Prepare SMM service module for IBV call Southbridge SMI service routine.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*;********************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;       its contributors may be used to endorse or promote products derived 
;       from this software without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
;*********************************************************************************/

#include "SbPlatform.h"
#include "cbtypes.h"

//
// Declaration of local functions
//

/**
 * Southbridge SMI service module
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sbSmmService (
  IN       AMDSBCFG* pConfig
  )
{
  AMDSBCFG*   pTmp;      //lx-dummy for /W4 build
  pTmp = pConfig;

  TRACE ((DMSG_SB_TRACE, "CIMx - Entering SMM services \n"));

  TRACE ((DMSG_SB_TRACE, "CIMx - Exiting SMM services \n"));
}

/**
 * softwareSMIservice - Software SMI service
 *
 * @param[in] VOID Southbridge software SMI service ID.
 *
 */
VOID
softwareSMIservice (
  IN       VOID
  )
{
  TRACE ((DMSG_SB_TRACE, "SMI CMD Port Address: %X SMICMD Port value is %X \n", dwSmiCmdPort, dwVar));
}





