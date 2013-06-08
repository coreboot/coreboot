/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB API definition.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
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
* ***************************************************************************
*
*/

#ifndef _GNBINTERFACE_H_
#define _GNBINTERFACE_H_

AGESA_STATUS
GnbInitAtReset (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  );

AGESA_STATUS
GnbInitAtEarly (
  IN OUT  AMD_EARLY_PARAMS        *EarlyParamsPtr
  );

VOID
GnbInitDataStructAtPostDef (
  IN OUT  GNB_POST_CONFIGURATION  *GnbPostConfigPtr,
  IN      AMD_POST_PARAMS         *PostParamsPtr
  );

AGESA_STATUS
GnbInitAtPost (
  IN OUT  AMD_POST_PARAMS         *PostParamsPtr
  );

VOID
GnbInitDataStructAtEnvDef (
  IN OUT  GNB_ENV_CONFIGURATION   *GnbEnvConfigPtr,
  IN      AMD_ENV_PARAMS          *EnvParamsPtr
  );

AGESA_STATUS
GnbInitAtEnv (
  IN       AMD_ENV_PARAMS          *EnvParamsPtr
  );

AGESA_STATUS
GnbInitAtMid (
  IN OUT   AMD_MID_PARAMS          *MidParamsPtr
  );

AGESA_STATUS
GnbInitAtLate (
  IN OUT   AMD_LATE_PARAMS         *LateParamsPtr
  );

AGESA_STATUS
GnbInitAtPostAfterDram (
  IN OUT   AMD_POST_PARAMS         *PostParamsPtr
  );

AGESA_STATUS
AmdGnbRecovery (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbInitAtEarlier (
  IN OUT   AMD_EARLY_PARAMS        *EarlyParamsPtr
  );

AGESA_STATUS
GnbInitAtS3Save (
  IN OUT   AMD_S3SAVE_PARAMS       *AmdS3SaveParams
  );

#endif
