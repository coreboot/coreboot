/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD HyperTransport option API.
 *
 * Contains option pre-compile logic.  This file is used by the options
 * installer and internally by the HT code initializers.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  OPTION
 * @e \$Revision: 34897 $   @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
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

#ifndef _OPTION_HT_H_
#define _OPTION_HT_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */

/**
 * Provide HT build option results
 */
typedef struct {
  CONST BOOLEAN IsUsingRecoveryHt;                   ///< Manual BUID Swap List processing should assume that HT Recovery was used.
  CONST BOOLEAN IsSetHtCrcFlood;                     ///< Enable setting of HT CRC Flood.
                                                     ///< Build-time only customizable - @BldCfgItem{BLDCFG_SET_HTCRC_SYNC_FLOOD}
  CONST BOOLEAN IsUsingUnitIdClumping;               ///< Enable automatically HT Spec compliant Unit Id Clumping.
                                                     ///< Build-time only customizable - @BldCfgItem{BLDCFG_USE_UNIT_ID_CLUMPING}
  CONST AMD_HT_INTERFACE *HtOptionPlatformDefaults;  ///< A set of build time options for HT constructor.
  CONST VOID *HtOptionInternalInterface;             ///< Use this internal interface initializer.
  CONST VOID *HtOptionInternalFeatures;              ///< Use this internal feature set initializer.
  CONST VOID *HtOptionFamilyNorthbridgeList;         ///< Use this list of northbridge initializers.
  CONST UINT8 *CONST *HtOptionBuiltinTopologies;     ///< Use this list of built-in topologies.
} OPTION_HT_CONFIGURATION;

typedef AGESA_STATUS
F_OPTION_HT_INIT_RESET (
  IN       AMD_CONFIG_PARAMS        *StdHeader,
  IN       AMD_HT_RESET_INTERFACE   *AmdHtResetInterface
  );

typedef F_OPTION_HT_INIT_RESET *PF_OPTION_HT_INIT_RESET;

typedef AGESA_STATUS
F_OPTION_HT_RESET_CONSTRUCTOR (
  IN       AMD_CONFIG_PARAMS         *StdHeader,
  IN       AMD_HT_RESET_INTERFACE    *AmdHtResetInterface
  );

typedef F_OPTION_HT_RESET_CONSTRUCTOR *PF_OPTION_HT_RESET_CONSTRUCTOR;

/**
 * Provide HT reset initialization build option results
 */
typedef struct {
  PF_OPTION_HT_INIT_RESET HtInitReset;               ///< Method: HT reset initialization.
  PF_OPTION_HT_RESET_CONSTRUCTOR HtResetConstructor; ///< Method: HT reset initialization.
} OPTION_HT_INIT_RESET;

/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

#endif  // _OPTION_HT_H_
