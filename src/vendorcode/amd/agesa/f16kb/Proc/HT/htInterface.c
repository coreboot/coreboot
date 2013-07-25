/* $NoKeywords:$ */
/**
 * @file
 *
 * External Interface implementation.
 *
 * Contains routines for implementing the interface to the client BIOS.
 * This file includes the interface access constructor.
 * This file implements build options using conditional compilation.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 *****************************************************************************
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
 *
 */

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "OptionsHt.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "CommonReturns.h"
#include "htInterfaceGeneral.h"
#include "htInterfaceCoherent.h"
#include "htInterfaceNonCoherent.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_HTINTERFACE_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

extern CONST OPTION_HT_CONFIGURATION OptionHtConfiguration;

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/**
 * The default initializer for the HT internal interface, full features.
 */
CONST HT_INTERFACE ROMDATA HtInterfaceDefault =
{
  GetCpu2CpuPcbLimits,
  GetSkipRegang,
  NewHopCountTable,
  GetOverrideBusNumbers,
  GetManualBuidSwapList,
  GetDeviceCapOverride,
  GetIoPcbLimits,
  GetSocketFromMap,
  GetIgnoreLink,
  PostMapToAp,
  NewNodeAndSocketTables,
  CleanMapsAfterError,
  SetNodeToSocketMap,
  GetMinNbCoreFreq
};

/**
 * The initializer for the HT internal interface, coherent only features.
 */
CONST HT_INTERFACE ROMDATA HtInterfaceCoherentOnly =
{
  GetCpu2CpuPcbLimits,
  GetSkipRegang,
  NewHopCountTable,
  GetOverrideBusNumbers,
  (PF_GET_MANUAL_BUID_SWAP_LIST)CommonReturnFalse,
  (PF_GET_DEVICE_CAP_OVERRIDE)CommonVoid,
  (PF_GET_IO_PCB_LIMITS)CommonVoid,
  GetSocketFromMap,
  GetIgnoreLink,
  PostMapToAp,
  NewNodeAndSocketTables,
  CleanMapsAfterError,
  SetNodeToSocketMap,
  GetMinNbCoreFreq
};

/**
 * The non-coherent only build option initializer for the HT internal interface.
 */
CONST HT_INTERFACE ROMDATA HtInterfaceNonCoherentOnly =
{
  (PF_GET_CPU_2_CPU_PCB_LIMITS)CommonVoid,
  (PF_GET_SKIP_REGANG)CommonReturnFalse,
  (PF_NEW_HOP_COUNT_TABLE)CommonVoid,
  GetOverrideBusNumbers,
  GetManualBuidSwapList,
  GetDeviceCapOverride,
  GetIoPcbLimits,
  GetSocketFromMap,
  GetIgnoreLink,
  PostMapToAp,
  NewNodeAndSocketTables,
  (PF_CLEAN_MAPS_AFTER_ERROR)CommonVoid,
  SetNodeToSocketMap,
  GetMinNbCoreFreq
};

/**
 * Topology Maps only feature build option initializer for the HT internal interface.
 */
CONST HT_INTERFACE ROMDATA HtInterfaceMapsOnly =
{
  (PF_GET_CPU_2_CPU_PCB_LIMITS)CommonVoid,
  (PF_GET_SKIP_REGANG)CommonReturnFalse,
  (PF_NEW_HOP_COUNT_TABLE)CommonVoid,
  (PF_GET_OVERRIDE_BUS_NUMBERS)CommonReturnFalse,
  (PF_GET_MANUAL_BUID_SWAP_LIST)CommonReturnFalse,
  (PF_GET_DEVICE_CAP_OVERRIDE)CommonVoid,
  (PF_GET_IO_PCB_LIMITS)CommonVoid,
  (PF_GET_SOCKET_FROM_MAP)CommonReturnZero8,
  (PF_GET_IGNORE_LINK)CommonReturnFalse,
  PostMapToAp,
  NewNodeAndSocketTables,
  (PF_CLEAN_MAPS_AFTER_ERROR)CommonVoid,
  SetNodeToSocketMap,
  (PF_GET_MIN_NB_CORE_FREQ)CommonReturnZero8
};

/**
 * No features build option initializer for the HT internal interface.
 */
CONST HT_INTERFACE ROMDATA HtInterfaceNone =
{
  (PF_GET_CPU_2_CPU_PCB_LIMITS)CommonVoid,
  (PF_GET_SKIP_REGANG)CommonReturnFalse,
  (PF_NEW_HOP_COUNT_TABLE)CommonVoid,
  (PF_GET_OVERRIDE_BUS_NUMBERS)CommonReturnFalse,
  (PF_GET_MANUAL_BUID_SWAP_LIST)CommonReturnFalse,
  (PF_GET_DEVICE_CAP_OVERRIDE)CommonVoid,
  (PF_GET_IO_PCB_LIMITS)CommonVoid,
  (PF_GET_SOCKET_FROM_MAP)CommonReturnZero8,
  (PF_GET_IGNORE_LINK)CommonReturnFalse,
  (PF_POST_MAP_TO_AP)CommonVoid,
  (PF_NEW_NODE_AND_SOCKET_TABLES)CommonVoid,
  (PF_CLEAN_MAPS_AFTER_ERROR)CommonVoid,
  (PF_SET_NODE_TO_SOCKET_MAP)CommonVoid,
  (PF_GET_MIN_NB_CORE_FREQ)CommonReturnZero8
};

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

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * A constructor for the internal Ht Interface.
 *
 * The install has a reference to the initializer appropriate to the user selected build
 * options.  Use the selected initializer to construct the internal interface.
 *
 * @param[in,out]   HtInterface    Contains pointer to HT Interface structure to initialize.
 * @param[in]       StdHeader         Opaque handle to standard config header
 *
*/
VOID
NewHtInterface (
     OUT   HT_INTERFACE         *HtInterface,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  LibAmdMemCopy (
    (VOID *) HtInterface,
    (VOID *) OptionHtConfiguration.HtOptionInternalInterface,
    (sizeof (HT_INTERFACE)),
    StdHeader
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * A "constructor" for the HyperTransport external interface.
 *
 * Sets inputs to valid, basic level, defaults.
 *
 * Copy the initial default values from the build options tables to the interface struct.
 *
 * @param[in]   StdHeader       Opaque handle to standard config header
 * @param[in]   AmdHtInterface  HT Interface structure to initialize.
 *
 * @retval AGESA_SUCCESS      Constructors are not allowed to fail
*/
AGESA_STATUS
AmdHtInterfaceConstructor (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       AMD_HT_INTERFACE     *AmdHtInterface
  )
{
  LibAmdMemCopy (
    (VOID *) AmdHtInterface,
    (VOID *) OptionHtConfiguration.HtOptionPlatformDefaults,
    (UINT32) (sizeof (AMD_HT_INTERFACE)),
    StdHeader
    );
  return AGESA_SUCCESS;
}
