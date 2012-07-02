/* $NoKeywords:$ */
/**
 * @file
 *
 * The initializer for Family 15h Mode 10h-1Fh northbridge support.
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
#include "amdlib.h"
#include "OptionsHt.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htNb.h"
#include "CommonReturns.h"
#include "htNbUtilitiesFam15Mod1x.h"
#include "cpuFamRegisters.h"
#include "Filecode.h"

#define FILECODE PROC_HT_FAM15MOD1X_HTNBFAM15MOD1X_FILECODE

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
CONST NORTHBRIDGE ROMDATA HtFam15Mod1xNb =
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
  Fam15Mod1xGetNumCoresOnNode,
  Fam15Mod1xSetTotalCores,
  Fam15Mod1xGetNodeCount,
  (PF_LIMIT_NODES)CommonVoid,
  (PF_READ_TRUE_LINK_FAIL_STATUS)CommonReturnFalse,
  (PF_GET_NEXT_LINK)CommonReturnZero32,
  (PF_GET_PACKAGE_LINK)CommonReturnZero8,
  (PF_MAKE_LINK_BASE)CommonReturnZero32,
  (PF_GET_MODULE_INFO)CommonVoid,
  (PF_POST_MAILBOX)CommonVoid,
  (PF_RETRIEVE_MAILBOX)CommonReturnZero32,
  (PF_GET_SOCKET)CommonReturnZero8,
  (PF_GET_ENABLED_COMPUTE_UNITS)Fam15Mod1xGetEnabledComputeUnits,
  (PF_GET_DUALCORE_COMPUTE_UNITS)Fam15Mod1xGetDualcoreComputeUnits,
  0,
  0,
  0,
  TRUE,
  TRUE,
  AMD_FAMILY_TN ,
  NULL,
  0,
  NULL,
  (PF_MAKE_KEY)CommonReturnZero64,
  NULL
};
