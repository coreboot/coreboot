/* $NoKeywords:$ */
/**
 * @file
 *
 * The initializer for Family 12h northbridge support.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44846 $   @e \$Date: 2011-01-07 13:21:05 +0800 (Fri, 07 Jan 2011) $
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
#include "htNb.h"
#include "CommonReturns.h"
#include "htNbUtilitiesFam12.h"
#include "cpuFamRegisters.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_FAM12_HTNBFAM12_FILECODE

extern OPTION_HT_CONFIGURATION OptionHtConfiguration;

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

/***************************************************************************
 ***               FAMILY/NORTHBRIDGE SPECIFIC FUNCTIONS                 ***
 ***************************************************************************/


/**
 * Initial construction data for no HT Northbridge.
 */
CONST NORTHBRIDGE ROMDATA HtFam12Nb =
{
  1,
  (PF_WRITE_ROUTING_TABLE)CommonVoid,
  (PF_WRITE_NODEID)CommonVoid,
  (PF_READ_DEFAULT_LINK)CommonReturnZero8,
  (PF_ENABLE_ROUTING_TABLES)CommonVoid,
  (PF_DISABLE_ROUTING_TABLES)CommonVoid,
  (PF_VERIFY_LINK_IS_COHERENT)CommonReturnFalse,
  (PF_READ_TOKEN)CommonReturnZero8,
  (PF_WRITE_TOKEN)CommonVoid,
  (PF_WRITE_FULL_ROUTING_TABLE)CommonVoid,
  (PF_IS_ILLEGAL_TYPE_MIX)CommonReturnFalse,
  (PF_IS_EXCEEDED_CAPABLE)CommonReturnFalse,
  (PF_STOP_LINK)CommonVoid,
  (PF_HANDLE_SPECIAL_LINK_CASE)CommonReturnFalse,
  (PF_HANDLE_SPECIAL_NODE_CASE)CommonReturnFalse,
  (PF_READ_SB_LINK)CommonReturnZero8,
  (PF_VERIFY_LINK_IS_NON_COHERENT)CommonReturnFalse,
  (PF_SET_CONFIG_ADDR_MAP)CommonVoid,
  (PF_NORTH_BRIDGE_FREQ_MASK)CommonReturnZero32,
  (PF_GATHER_LINK_FEATURES)CommonVoid,
  (PF_SET_LINK_REGANG)CommonVoid,
  (PF_SET_LINK_FREQUENCY)CommonVoid,
  (PF_SET_LINK_UNITID_CLUMPING)CommonVoid,
  (PF_WRITE_TRAFFIC_DISTRIBUTION)CommonVoid,
  (PF_WRITE_LINK_PAIR_DISTRIBUTION)CommonVoid,
  (PF_WRITE_VICTIM_DISTRIBUTION)CommonVoid,
  (PF_BUFFER_OPTIMIZATIONS)CommonVoid,
  Fam12GetNumCoresOnNode,
  (PF_SET_TOTAL_NODES_AND_CORES)CommonVoid,
  Fam12GetNodeCount,
  (PF_LIMIT_NODES)CommonVoid,
  (PF_READ_TRUE_LINK_FAIL_STATUS)CommonReturnFalse,
  (PF_GET_NEXT_LINK)CommonReturnZero32,
  (PF_GET_PACKAGE_LINK)CommonReturnZero8,
  (PF_MAKE_LINK_BASE)CommonReturnZero32,
  (PF_GET_MODULE_INFO)CommonVoid,
  (PF_POST_MAILBOX)CommonVoid,
  Fam12RetrieveMailbox,
  (PF_GET_SOCKET)CommonReturnZero8,
  (PF_GET_ENABLED_COMPUTE_UNITS)CommonReturnZero8,
  (PF_GET_DUALCORE_COMPUTE_UNITS)CommonReturnZero8,
  0,
  0,
  0,
  TRUE,
  TRUE,
  AMD_FAMILY_12,
  NULL,
  0,
  NULL,
  (PF_MAKE_KEY)CommonReturnZero64,
  NULL
};
