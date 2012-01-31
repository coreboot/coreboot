/**
 * @file
 *
 * NB definitions
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
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
 *
 ***************************************************************************/

#ifndef _NBINIT_H_
#define _NBINIT_H_


VOID
NbLibSetSSID (
  IN      AMD_NB_CONFIG  *pConfig
  );

VOID
NbLibSetupClumping  (
  IN      AMD_NB_CONFIG  *pConfig
  );

AGESA_STATUS
NbLibSetTopOfMemory (
  IN      AMD_NB_CONFIG  *pConfig
  );

AGESA_STATUS
NbLibEarlyPostInitValidateInput (
  IN      AMD_NB_CONFIG  *pConfig
  );

AGESA_STATUS
NbLibPostInitValidateInput (
  IN      AMD_NB_CONFIG  *pConfig
  );

AGESA_STATUS
NbLibPrepareToOS (
  IN      AMD_NB_CONFIG  *pConfig
  );

VOID
NbMultiNbIocInit (
  IN      AMD_NB_CONFIG  *pConfig
  );

AGESA_STATUS
NbLibInitializer (
  IN OUT  AMD_NB_CONFIG *pConfig
  );

AGESA_STATUS
AmdNbInitializer (
  IN OUT  AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

#ifndef HT_INTERRUPT_ENCODING_OFFSET
  #define HT_INTERRUPT_ENCODING_OFFSET 0x2
#endif

VOID
NbLibSetNmiRouting (
  IN      AMD_NB_CONFIG  *pConfig
  );

AGESA_STATUS
NbLibGetCore0ApicId (
  IN      AMD_NB_CONFIG  *pConfig
  );

#endif