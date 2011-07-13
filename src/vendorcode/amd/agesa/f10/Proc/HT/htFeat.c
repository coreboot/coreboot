/**
 * @file
 *
 * HyperTransport features constructor.
 *
 * Initialize the set of available features.
 * This file implements build options using conditional compilation.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
#include "OptionsHt.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "CommonReturns.h"
#include "Filecode.h"
#define FILECODE PROC_HT_HTFEAT_FILECODE
extern CONST OPTION_HT_CONFIGURATION OptionHtConfiguration;

/**
 * A no features Initializer.
 */
CONST HT_FEATURES ROMDATA HtFeaturesNone =
{
  (PF_COHERENT_DISCOVERY)CommonVoid,
  (PF_LOOKUP_COMPUTE_AND_LOAD_ROUTING_TABLES)CommonVoid,
  (PF_MAKE_HOP_COUNT_TABLE)CommonVoid,
  (PF_PROCESS_LINK)CommonVoid,
  (PF_GATHER_LINK_DATA)CommonVoid,
  (PF_SELECT_OPTIMAL_WIDTH_AND_FREQUENCY)CommonVoid,
  (PF_REGANG_LINKS)CommonVoid,
  (PF_SUBLINK_RATIO_FIXUP)CommonVoid,
  (PF_IS_COHERENT_RETRY_FIXUP)CommonReturnFalse,
  (PF_SET_LINK_DATA)CommonVoid,
  (PF_TRAFFIC_DISTRIBUTION)CommonVoid,
  (PF_SET_HT_CONTROL_REGISTER_BITS)CommonVoid,
  (PF_CONVERT_WIDTH_TO_BITS)CommonReturnZero8
};

/*----------------------------------------------------------------------------------------*/
/**
 * Provide the current Feature set implementation.
 *
 * Initialize using the installed initializer.
 *
 * @param[in]   HtFeatures      A feature object to initialize
 * @param[in]   StdHeader       Opaque handle to standard config header
*/
VOID
NewHtFeatures (
     OUT   HT_FEATURES         *HtFeatures,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  LibAmdMemCopy (
    (VOID *) HtFeatures,
    (VOID *) OptionHtConfiguration.HtOptionInternalFeatures ,
    (UINT32) (sizeof (HT_FEATURES)),
    StdHeader
    );
}
