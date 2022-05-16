/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of multiple CPU features.
 *
 * Aggregates enabled CPU features into a list for the dispatcher to process.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 63692 $   @e \$Date: 2012-01-03 22:13:28 -0600 (Tue, 03 Jan 2012) $
 */
/*****************************************************************************
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
 *
 ***************************************************************************/

#ifndef _OPTION_CPU_FEATURES_INSTALL_H_
#define _OPTION_CPU_FEATURES_INSTALL_H_

#include "OptionHwC1eInstall.h"
#include "OptionMsgBasedC1eInstall.h"
#include "OptionSwC1eInstall.h"
#include "OptionL3FeaturesInstall.h"
#include "OptionCpuCoreLevelingInstall.h"
#include "OptionIoCstateInstall.h"
#include "OptionC6Install.h"
#include "OptionCpbInstall.h"
#include "OptionCpuCacheFlushOnHaltInstall.h"
#include "OptionPstateHpcModeInstall.h"
#include "OptionApmInstall.h"
#include "OptionPsiInstall.h"
#include "OptionHtcInstall.h"
#include "OptionPreserveMailboxInstall.h"

CONST CPU_FEATURE_DESCRIPTOR* ROMDATA CONST SupportedCpuFeatureList[] =
{
  OPTION_HW_C1E_FEAT
  OPTION_MSG_BASED_C1E_FEAT
  OPTION_SW_C1E_FEAT
  OPTION_L3_FEAT
  OPTION_CPU_CORE_LEVELING_FEAT
  OPTION_IO_CSTATE_FEAT
  OPTION_C6_STATE_FEAT
  OPTION_CPB_FEAT
  OPTION_CPU_CACHE_FLUSH_ON_HALT_FEAT
  OPTION_CPU_PSTATE_HPC_MODE_FEAT             // this function should be run before low power pstate for prochot
  OPTION_CPU_APM_FEAT
  OPTION_CPU_PSI_FEAT
  OPTION_CPU_HTC_FEAT
  OPTION_PRESERVE_MAILBOX_FEAT
  NULL
};


#endif  // _OPTION_CPU_FEATURES_INSTALL_H_
