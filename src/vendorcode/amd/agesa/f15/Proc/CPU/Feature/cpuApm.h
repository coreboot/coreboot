/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU Application Power Management (APM) Functions declarations.
 *
 * Contains code that declares the AGESA CPU APM related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 54493 $   @e \$Date: 2011-06-08 15:21:06 -0600 (Wed, 08 Jun 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_APM_H_
#define _CPU_APM_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */
//  Forward declaration needed for multi-structure mutual references
AGESA_FORWARD_DECLARATION (APM_FAMILY_SERVICES);

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if Application Power Management (APM) is supported.
 *
 * @param[in]    ApmServices        APM services.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    Socket             Zero-based socket number.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               APM is supported.
 * @retval       FALSE              APM is not supported.
 *
 */
typedef BOOLEAN F_APM_IS_SUPPORTED (
  IN       APM_FAMILY_SERVICES    *CpbServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       UINT32                 Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a Method.
typedef F_APM_IS_SUPPORTED *PF_APM_IS_SUPPORTED;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to enable APM.
 *
 * @param[in]    ApmServices        APM services.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       Family specific error value.
 *
 */
typedef AGESA_STATUS F_APM_INIT (
  IN       APM_FAMILY_SERVICES    *ApmServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a Method.
typedef F_APM_INIT *PF_APM_INIT;

/**
 * Provide the interface to the APM Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
struct _APM_FAMILY_SERVICES {
  UINT16          Revision;                                             ///< Interface version
  // Public Methods.
  PF_APM_IS_SUPPORTED IsApmSupported;                                   ///< Method: Family specific call to check if APM is supported.
  PF_APM_INIT         EnableApmOnSocket;                                ///< Method: Family specific call to enable APM.
};


/*----------------------------------------------------------------------------------------
 *                          F U N C T I O N S     P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

#endif  // _CPU_APM_H_
