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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 *****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
