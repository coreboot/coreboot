/**
 * @file
 *
 * HT definitions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/
#ifndef _NBHTINIT_H_
#define _NBHTINIT_H_

///HT link inactive lane
typedef enum {
  InactiveLaneStateCadCtrlDrivelToLogic0 = 0,   ///< No clock
  InactiveLaneStateSameAsPhyOff,                ///< Same as PHY OFF
  InactiveLaneStateSameAsOperational,           ///< Same as operational
  InactiveLaneStateSameAsDisconnected,          ///< Same as disconnected
} HT_INACTIVE_LANE_STATE;

AGESA_STATUS
HtLibEarlyInit (
  IN  OUT   AMD_NB_CONFIG *pConfig
  );


AGESA_STATUS
HtLibInitValidateInput (
  IN  OUT   AMD_NB_CONFIG *pConfig
  );

VOID
LibHtSetNbTransmitterDeemphasis (
  IN        UINT8        NbDeemphasisLevel,
  IN        AMD_NB_CONFIG *pConfig
  );

VOID
LibHtEnableLxState (
  IN        UINT8    LSx,
  IN        AMD_NB_CONFIG *pConfig
  );

AGESA_STATUS
AmdHtInitializer (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
HtLibInitializer (
  IN  OUT   AMD_NB_CONFIG *pConfig
  );

UINT32
LibHtGetSmuWaData (
  IN        AMD_NB_CONFIG *pConfig
  );

#define LS1    0
#define LS0    1
#define LS2    2
#define LS3    3

/// HT link state
typedef enum {
  HtLinkStateLS0 = 0,             ///< LS0
  HtLinkStateLS1,                 ///< LS1
  HtLinkStateLS2,                 ///< LS2
  HtLinkStateLS3,                 ///< LS3
  HtLinkStateSameAsCpu,           ///< Same as set on CPU
  HtLinkStateSkipInit = 0x80      ///< Skip initialization
} HT_LS_STATE;

/// HT Link Tri-state
typedef enum {
  HtLinkTriStateCadCtl = 1,     ///< control/data
  HtLinkTriStateCadCtlClk,      ///< CAD clk
  HtLinkTriStateSameAsCpu,      ///< Same as set on CPU
  HtLinkTriStateSkip = 0x80     ///< Skip initialization
} HT_LINK_TRISTATE;

/// HT Link Tri-state
typedef enum {
  HtExtAddressingDisable = 0,     ///< Disable ext addressing
  HtExtAddressingEnable,          ///< Enable Ext addressing
  HtExtAddressingSameAsCpu,       ///< Set Ext addressing as on CPU
  HtExtAddressingSkip = 0x80      ///< Skip initialization
} HT_EXT_ADDRESSING;

#define HT_PATH_LINK_ID(htPath) (htPath.LinkID & 0xf)
#define HT_PATH_SUBLINK_ID(htPath) (((htPath.LinkID & 0xf0) == 0) ? 0 : (((htPath.LinkID & 0xf0) >> 4) - 1))

#endif