/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Multi-socket option API.
 *
 * Contains structures and values used to control the multi-socket option code.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  OPTION
 * @e \$Revision: 48937 $   @e \$Date: 2011-03-15 03:37:15 +0800 (Tue, 15 Mar 2011) $
 *
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
 ******************************************************************************
 */

#ifndef _OPTION_MULTISOCKET_H_
#define _OPTION_MULTISOCKET_H_

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
 * This function loops through all possible socket locations, gathering the number
 * of power management steps each populated socket requires, and returns the
 * highest number.
 *
 * @param[out] NumSystemSteps    Maximum number of system steps required
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
typedef VOID OPTION_MULTISOCKET_PM_STEPS (
     OUT   UINT8 *NumSystemSteps,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * This function loops through all possible socket locations, starting core 0 of
 * each populated socket to perform the passed in AP_TASK.  After starting all
 * other core 0s, the BSC will perform the AP_TASK as well.  This must be run by
 * the system BSC only.
 *
 * @param[in]  TaskPtr           Function descriptor
 * @param[in]  StdHeader         Config handle for library and services
 * @param[in]  ConfigParams      AMD entry point's CPU parameter structure
 *
 */
typedef VOID OPTION_MULTISOCKET_PM_CORE0_TASK (
  IN       VOID *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       VOID *ConfigParams
  );

/**
 * This function loops through all possible socket locations, comparing the
 * maximum NB frequencies to determine the slowest.  This function also
 * determines if all coherent NB frequencies are equivalent.
 *
 * @param[in]  NbPstate                    NB P-state number to check (0 = fastest)
 * @param[in]  PlatformConfig              Platform profile/build option config structure.
 * @param[out] SystemNbCofNumerator        NB frequency numerator for the system in MHz
 * @param[out] SystemNbCofDenominator      NB frequency denominator for the system
 * @param[out] SystemNbCofsMatch           Whether or not all NB frequencies are equivalent
 * @param[out] NbPstateIsEnabledOnAllCPUs  Whether or not NbPstate is valid on all CPUs
 * @param[in]  StdHeader                   Config handle for library and services
 *
 * @retval     TRUE                        At least one processor has NbPstate enabled.
 * @retval     FALSE                       NbPstate is disabled on all CPUs
 */
typedef BOOLEAN OPTION_MULTISOCKET_PM_NB_COF (
  IN       UINT32 NbPstate,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   UINT32 *SystemNbCofNumerator,
     OUT   UINT32 *SystemNbCofDenominator,
     OUT   BOOLEAN *SystemNbCofsMatch,
     OUT   BOOLEAN *NbPstateIsEnabledOnAllCPUs,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * This function loops through all possible socket locations, checking whether
 * any populated sockets require NB COF VID programming.
 *
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
typedef BOOLEAN OPTION_MULTISOCKET_PM_NB_COF_UPDATE (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * This function loops through all possible socket locations, collecting any
 * power management initialization errors that may have occurred.  These errors
 * are transferred from the core 0s of the socket in which the errors occurred
 * to the BSC's heap.  The BSC's heap is then searched for the most severe error
 * that occurred, and returns it.  This function must be called by the BSC only.
 *
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
typedef AGESA_STATUS OPTION_MULTISOCKET_PM_GET_EVENTS (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * This function loops through all possible socket locations and Nb Pstates,
 * comparing the NB frequencies to determine the slowest NB P0 and NB Pmin in
 * the system.
 *
 * @param[in]  PlatformConfig      Platform profile/build option config structure.
 * @param[out] MinSysNbFreq        NB frequency numerator for the system in MHz
 * @param[out] MinP0NbFreq         NB frequency numerator for P0 in MHz
 * @param[in]  StdHeader           Config handle for library and services
 */
typedef VOID OPTION_MULTISOCKET_PM_NB_MIN_COF (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   UINT32                 *MinSysNbFreq,
     OUT   UINT32                 *MinP0NbFreq,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

#define MULTISOCKET_STRUCT_VERSION  0x01

/**
 * Provide build configuration of cpu multi-socket or single socket support.
 *
 */
typedef struct  {
  UINT16              OptMultiSocketVersion;                       ///< Table version
  OPTION_MULTISOCKET_PM_STEPS *GetNumberOfSystemPmSteps;           ///< Method: Get number of power mgt tasks
  OPTION_MULTISOCKET_PM_CORE0_TASK *BscRunCodeOnAllSystemCore0s;   ///< Method: Perform tasks on Core 0 of each processor
  OPTION_MULTISOCKET_PM_NB_COF *GetSystemNbPstateSettings;         ///< Method: Find the Northbridge frequency for the specified Nb Pstate in the system.
  OPTION_MULTISOCKET_PM_NB_COF_UPDATE *GetSystemNbCofVidUpdate;    ///< Method: Determine if any Northbridges in the system need to update their COF/VID.
  OPTION_MULTISOCKET_PM_GET_EVENTS *BscRetrievePmEarlyInitErrors;  ///< Method: Gathers error information from all Core 0s.
  OPTION_MULTISOCKET_PM_NB_MIN_COF *GetMinNbCof;                   ///< Method: Get the minimum system and minimum P0 Northbridge frequency.
} OPTION_MULTISOCKET_CONFIGURATION;

/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */


#endif  // _OPTION_MULTISOCKET_H_
