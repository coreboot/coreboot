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
 * @e \$Revision: 20379 $   @e \$Date: 2009-10-12 23:41:06 -0700 (Mon, 12 Oct 2009) $
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
 * @param[in] StdHeader       Standard configuration header
 * @param[in] PlatformConfig  Platform configuration
 * @param[in] GnbConfig       Gnb configuration
 * @retval    AGESA_STATUS    Always succeeds
 */

AGESA_STATUS
GnbInitAtEarly (
  IN      AMD_CONFIG_PARAMS       *StdHeader,
  IN      PLATFORM_CONFIGURATION  *PlatformConfig,
  IN      GNB_CONFIGURATION       *GnbConfig
  )
{

  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Env
 *
 *
 *
 * @param[in] StdHeader     Standard configuration header
 * @retval    AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GnbInitAtEnv (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  )
{

  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GNB at Reset
 *
 *
 *
 * @param[in]     StdHeader     Standard configuration header
 * @param[in,out] MemConfig     Memory configuration
 * @retval        AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GnbInitAtPost (
  IN       AMD_CONFIG_PARAMS       *StdHeader,
  IN OUT   MEM_PARAMETER_STRUCT    *MemConfig
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
 * @param[in] StdHeader     Standard configuration header
 * @retval    AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GnbInitAtMid (
  IN      AMD_CONFIG_PARAMS       *StdHeader
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
 * @param[in] StdHeader     Standard configuration header
 * @retval    AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GnbInitAtLate (
  IN      AMD_CONFIG_PARAMS       *StdHeader
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