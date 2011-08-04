/* $NoKeywords:$ */
/**
 * @file
 *
 *
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  GNB
 * @e \$Revision: 44325 $   @e \$Date: 2010-12-22 03:29:53 -0700 (Wed, 22 Dec 2010) $
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

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Reset Stub
 *
 *
 *
 * @param[in] StdHeader     Standard configuration header
 * @retval    AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GnbInitAtReset (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  )
{

  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Early Stub
 *
 *
 *
 * @param[in,out] EarlyParamsPtr    Pointer to early configuration params.
 * @retval        AGESA_STATUS      Always succeeds
 */

AGESA_STATUS
GnbInitAtEarly (
  IN OUT   AMD_EARLY_PARAMS        *EarlyParamsPtr
  )
{
  return  AGESA_SUCCESS;
}

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
  IN OUT  GNB_ENV_CONFIGURATION   *GnbEnvConfigPtr,
  IN      AMD_ENV_PARAMS          *EnvParamsPtr
  )
{

}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Env
 *
 *
 *
 * @param[in] EnvParamsPtr        Pointer to env configuration params.
* @retval     AGESA_STATUS        Always succeeds
 */

AGESA_STATUS
GnbInitAtEnv (
  IN       AMD_ENV_PARAMS          *EnvParamsPtr
  )
{

  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Post
 *
 *
 *
 * @param[in,out] PostParamsPtr    Pointer to Post configuration params.
 * @retval        AGESA_STATUS     Always succeeds
 */

AGESA_STATUS
GnbInitAtPost (
  IN OUT  AMD_POST_PARAMS         *PostParamsPtr
  )
{

  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Mid post
 *
 *
 *
 * @param[in,out] MidParamsPtr    Pointer to mid configuration params.
 * @retval        AGESA_STATUS    Always succeeds
 */

AGESA_STATUS
GnbInitAtMid (
  IN OUT   AMD_MID_PARAMS          *MidParamsPtr
  )
{

  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Late post
 *
 *
 *
 * @param[in,out] LateParamsPtr    Pointer to late configuration params.
 * @retval        AGESA_STATUS     Always succeeds
 */

AGESA_STATUS
GnbInitAtLate (
  IN OUT   AMD_LATE_PARAMS         *LateParamsPtr
  )
{

  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * AmdGnbRecovery
 *
 *
 *
 * @param[in] StdHeader     Standard configuration header
 * @retval    AGESA_STATUS  Always succeeds
 */
AGESA_STATUS
AmdGnbRecovery (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  return  AGESA_SUCCESS;
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
  return  AGESA_SUCCESS;
}