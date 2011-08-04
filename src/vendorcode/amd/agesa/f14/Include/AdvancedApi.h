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
 * @e \$Revision: 44325 $   @e \$Date: 2010-12-22 03:29:53 -0700 (Wed, 22 Dec 2010) $
 */
/*****************************************************************************
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
memDefRet (
  VOID
  );

BOOLEAN
memDefTrue (
  VOID
  );

BOOLEAN
memDefFalse (
  VOID
  );

VOID
MemRecDefRet (VOID);

BOOLEAN
MemRecDefTrue (VOID);

#endif // _ADVANCED_API_H_
