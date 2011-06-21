/* $NoKeywords:$ */
/**
 * @file
 *
 * HyperTransport feature sets initializers.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 35136 $   @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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
#include "htFeatDynamicDiscovery.h"
#include "htFeatRouting.h"
#include "htFeatNoncoherent.h"
#include "htFeatOptimization.h"
#include "htFeatGanging.h"
#include "htFeatSublinks.h"
#include "htFeatTrafficDistribution.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_FEATURES_HTFEATSETS_FILECODE
extern CONST OPTION_HT_CONFIGURATION OptionHtConfiguration;

/**
 * Initializer for the default feature set implementation,
 * full features.
 */
CONST HT_FEATURES ROMDATA HtFeaturesDefault =
{
  CoherentDiscovery,
  LookupComputeAndLoadRoutingTables,
  MakeHopCountTable,
  ProcessLink,
  GatherLinkData,
  SelectOptimalWidthAndFrequency,
  RegangLinks,
  SubLinkRatioFixup,
  IsCoherentRetryFixup,
  SetLinkData,
  TrafficDistribution,
  SetHtControlRegisterBits,
  ConvertWidthToBits
};

/**
 * Initializer for the non-coherent only build option.
 */
CONST HT_FEATURES ROMDATA HtFeaturesNonCoherentOnly =
{
  (PF_COHERENT_DISCOVERY)CommonVoid,
  (PF_LOOKUP_COMPUTE_AND_LOAD_ROUTING_TABLES)CommonVoid,
  (PF_MAKE_HOP_COUNT_TABLE)CommonVoid,
  ProcessLink,
  GatherLinkData,
  SelectOptimalWidthAndFrequency,
  (PF_REGANG_LINKS)CommonVoid,
  (PF_SUBLINK_RATIO_FIXUP)CommonVoid,
  (PF_IS_COHERENT_RETRY_FIXUP)CommonReturnFalse,
  SetLinkData,
  (PF_TRAFFIC_DISTRIBUTION)CommonVoid,
  SetHtControlRegisterBits,
  ConvertWidthToBits
};
