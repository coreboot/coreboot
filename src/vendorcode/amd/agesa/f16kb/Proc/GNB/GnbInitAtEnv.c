/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB env init interface
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "OptionGnb.h"
#include  "GnbLibFeatures.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_GNBINITATENV_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern  OPTION_GNB_CONFIGURATION  GnbEnvFeatureTable[];
extern  BUILD_OPT_CFG UserOptions;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
GnbInitDataStructAtEnvDef (
  IN OUT   GNB_ENV_CONFIGURATION   *GnbEnvConfigPtr,
  IN       AMD_ENV_PARAMS          *EnvParamsPtr
  );

AGESA_STATUS
GnbInitAtEnv (
  IN       AMD_ENV_PARAMS          *EnvParamsPtr
  );
/*----------------------------------------------------------------------------------------*/
/**
 * Default constructor of GNB configuration at Env
 *
 *
 *
 * @param[in] GnbEnvConfigPtr     Pointer to gnb env configuration params.
 * @param[in] EnvParamsPtr        Pointer to env configuration params.
 */
VOID
GnbInitDataStructAtEnvDef (
  IN OUT   GNB_ENV_CONFIGURATION   *GnbEnvConfigPtr,
  IN       AMD_ENV_PARAMS          *EnvParamsPtr
  )
{
  GnbEnvConfigPtr->Gnb3dStereoPinIndex = UserOptions.CfgGnb3dStereoPinIndex;
  GnbEnvConfigPtr->IommuSupport = UserOptions.CfgIommuSupport;
  GnbEnvConfigPtr->LvdsSpreadSpectrum = UserOptions.CfgLvdsSpreadSpectrum;
  GnbEnvConfigPtr->LvdsSpreadSpectrumRate = UserOptions.CfgLvdsSpreadSpectrumRate;
  GnbEnvConfigPtr->LvdsPowerOnSeqDigonToDe = UserOptions.CfgLvdsPowerOnSeqDigonToDe;
  GnbEnvConfigPtr->LvdsPowerOnSeqDeToVaryBl = UserOptions.CfgLvdsPowerOnSeqDeToVaryBl;
  GnbEnvConfigPtr->LvdsPowerOnSeqDeToDigon = UserOptions.CfgLvdsPowerOnSeqDeToDigon;
  GnbEnvConfigPtr->LvdsPowerOnSeqVaryBlToDe = UserOptions.CfgLvdsPowerOnSeqVaryBlToDe;
  GnbEnvConfigPtr->LvdsPowerOnSeqOnToOffDelay = UserOptions.CfgLvdsPowerOnSeqOnToOffDelay;
  GnbEnvConfigPtr->LvdsPowerOnSeqVaryBlToBlon = UserOptions.CfgLvdsPowerOnSeqVaryBlToBlon;
  GnbEnvConfigPtr->LvdsPowerOnSeqBlonToVaryBl = UserOptions.CfgLvdsPowerOnSeqBlonToVaryBl;
  GnbEnvConfigPtr->LvdsMaxPixelClockFreq = UserOptions.CfgLvdsMaxPixelClockFreq;
  GnbEnvConfigPtr->LcdBitDepthControlValue = UserOptions.CfgLcdBitDepthControlValue;
  GnbEnvConfigPtr->Lvds24bbpPanelMode = UserOptions.CfgLvds24bbpPanelMode;
  GnbEnvConfigPtr->LvdsMiscControl.Value = 0;
  GnbEnvConfigPtr->LvdsMiscControl.Value = UserOptions.CfgLvdsMiscControl.Value;
  GnbEnvConfigPtr->PcieRefClkSpreadSpectrum = UserOptions.CfgPcieRefClkSpreadSpectrum;
  GnbEnvConfigPtr->GnbRemoteDisplaySupport = UserOptions.CfgGnbRemoteDisplaySupport;
  GnbEnvConfigPtr->LvdsMiscVoltAdjustment = UserOptions.CfgLvdsMiscVoltAdjustment;
  GnbEnvConfigPtr->DisplayMiscControl.Value = UserOptions.CfgDisplayMiscControl.Value;
  GnbEnvConfigPtr->DpFixedVoltSwingType = UserOptions.CfgDpFixedVoltSwingType;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Env
 *
 *
 *
 * @param[in] EnvParamsPtr        Pointer to env configuration params.
 * @retval                        Initialization status.
 */

AGESA_STATUS
GnbInitAtEnv (
  IN       AMD_ENV_PARAMS          *EnvParamsPtr
  )
{
  AGESA_STATUS  Status;
  Status = GnbLibDispatchFeatures (&GnbEnvFeatureTable[0], &EnvParamsPtr->StdHeader);
  return  Status;
}
