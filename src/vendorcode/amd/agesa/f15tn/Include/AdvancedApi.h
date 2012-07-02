/* $NoKeywords:$ */
/**
 * @file
 *
 * Advanced API Interface for HT, Memory and CPU
 *
 * Contains additional declarations need to use HT, Memory and CPU Advanced interface, such as
 * would be required by the basic interface implementations.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
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
 *
 ***************************************************************************/


#ifndef _ADVANCED_API_H_
#define _ADVANCED_API_H_

/*----------------------------------------------------------------------------
 *                           HT FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

/**
 * A constructor for the HyperTransport input structure.
 *
 * Sets inputs to valid, basic level, defaults.
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
  );

/**
 * The top level external interface for Hypertransport Initialization.
 *
 * Create our initial internal state, initialize the coherent fabric,
 * initialize the non-coherent chains, and perform any required fabric tuning or
 * optimization.
 *
 * @param[in]   StdHeader              Opaque handle to standard config header
 * @param[in]   PlatformConfiguration  The platform configuration options.
 * @param[in]   AmdHtInterface         HT Interface structure.
 *
 * @retval      AGESA_SUCCESS     Only information events logged.
 * @retval      AGESA_ALERT       Sync Flood or CRC error logged.
 * @retval      AGESA_WARNING     Example: expected capability not found
 * @retval      AGESA_ERROR       logged events indicating some devices may not be available
 * @retval      AGESA_FATAL       Mixed Family or MP capability mismatch
 *
 */
AGESA_STATUS
AmdHtInitialize (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfiguration,
  IN       AMD_HT_INTERFACE       *AmdHtInterface
  );

/*----------------------------------------------------------------------------
 *                        HT Recovery   FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

/**
 * A constructor for the HyperTransport input structure.
 *
 */
AGESA_STATUS
AmdHtResetConstructor (
  IN       AMD_CONFIG_PARAMS         *StdHeader,
  IN       AMD_HT_RESET_INTERFACE    *AmdHtResetInterface
  );

/**
 * Initialize HT at Reset for both Normal and Recovery.
 *
 */
AGESA_STATUS
AmdHtInitReset (
  IN       AMD_CONFIG_PARAMS        *StdHeader,
  IN       AMD_HT_RESET_INTERFACE   *AmdHtResetInterface
  );

/**
 * Initialize the Node and Socket maps for an AP Core.
 *
 */
AGESA_STATUS
AmdHtInitRecovery (
  IN       AMD_CONFIG_PARAMS           *StdHeader
  );

///----------------------------------------------------------------------------
///                          MEMORY FUNCTIONS PROTOTYPE
///
///----------------------------------------------------------------------------

AGESA_STATUS
AmdMemRecovery (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

AGESA_STATUS
AmdMemAuto (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
AmdMemInitDataStructDef (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT  PLATFORM_CONFIGURATION   *PlatFormConfig
  );

VOID
memDefRet ( VOID );

BOOLEAN
memDefTrue ( VOID );

BOOLEAN
memDefFalse ( VOID );

VOID
MemRecDefRet ( VOID );

BOOLEAN
MemRecDefTrue ( VOID );

#endif // _ADVANCED_API_H_
