/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB POST init interface
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
#include  "Ids.h"
#include  "Gnb.h"
#include  "OptionGnb.h"
#include  "GnbLibFeatures.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_GNBINITATPOST_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern  OPTION_GNB_CONFIGURATION  GnbPostFeatureTable[];
extern  OPTION_GNB_CONFIGURATION  GnbPostAfterDramFeatureTable[];
extern  BUILD_OPT_CFG             UserOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
GnbInitDataStructAtPostDef (
  IN OUT   GNB_POST_CONFIGURATION  *GnbPostConfigPtr,
  IN       AMD_POST_PARAMS         *PostParamsPtr
  );

AGESA_STATUS
GnbInitAtPost (
  IN OUT   AMD_POST_PARAMS         *PostParamsPtr
  );

AGESA_STATUS
GnbInitAtPostAfterDram (
  IN OUT   AMD_POST_PARAMS         *PostParamsPtr
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Default constructor of GNB configuration at Env
 *
 *
 *
 * @param[in] GnbPostConfigPtr     Pointer to GNB POST configuration params.
 * @param[in] PostParamsPtr        Pointer to POST configuration params.
 */
VOID
GnbInitDataStructAtPostDef (
  IN OUT   GNB_POST_CONFIGURATION  *GnbPostConfigPtr,
  IN       AMD_POST_PARAMS         *PostParamsPtr
  )
{
  GnbPostConfigPtr->IgpuEnableDisablePolicy = UserOptions.CfgIgpuEnableDisablePolicy;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Post
 *
 *
 *
 * @param[in]     PostParamsPtr Pointer to post configuration parameters
 * @retval                      Initialization status.
 */

AGESA_STATUS
GnbInitAtPost (
  IN OUT   AMD_POST_PARAMS         *PostParamsPtr
  )
{
  AGESA_STATUS          Status;
  Status = GnbLibDispatchFeatures (&GnbPostFeatureTable[0], &PostParamsPtr->StdHeader);
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Post after DRAM init
 *
 *
 *
 * @param[in]     PostParamsPtr Pointer to post configuration parameters
 * @retval                      Initialization status.
 */

AGESA_STATUS
GnbInitAtPostAfterDram (
  IN OUT   AMD_POST_PARAMS         *PostParamsPtr
  )
{
  AGESA_STATUS          Status;
  Status = GnbLibDispatchFeatures (&GnbPostAfterDramFeatureTable[0], &PostParamsPtr->StdHeader);
  return  Status;
}
