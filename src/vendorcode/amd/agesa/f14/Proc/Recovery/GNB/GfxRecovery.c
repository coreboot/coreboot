/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD GFX Recovery API, and related functions.
 *
 * Contains code that implements the GFX Recovery functionality
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Recovery/GNB
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "GfxRecovery.h"
#include "OptionGfxRecovery.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_GNB_GFXRECOVERY_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_GFX_RECOVERY_CONFIGURATION OptionGfxRecoveryConfiguration;  // global user config record
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Performs Gfx related initialization at the recovery entry point
 *
 * This function processes the MSR and PCI register tables.
 *
 * @param[in]    StdHeader    global state, input data
 *
 * @retval     AGESA_SUCCESS         Always succeeds.
 *
 */
AGESA_STATUS
AmdGfxRecovery (
  IN    AMD_CONFIG_PARAMS     *StdHeader
  )
{
   return ((*(OptionGfxRecoveryConfiguration.GfxRecoveryFeature)) (StdHeader));
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function will called if OPTION_GfxRecovery is true
 *
 *    @param[in, out] StdHeader         Standard Head Pointer
 *
 *    @retval         AGESA_STATUS
 */
AGESA_STATUS
GetGfxRecoveryMain (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader
  )
{
  //to be done
  return AGESA_SUCCESS;
}
