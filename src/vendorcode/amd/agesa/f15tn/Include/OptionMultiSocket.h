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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
 * @return     The most severe error code from AP_TASK
 *
 */
typedef AGESA_STATUS OPTION_MULTISOCKET_PM_CORE0_TASK (
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

/**
 * This function returns the current running core's PCI Config Space address.
 *
 * @param[out]   PciAddress   The Processor's PCI Config Space address (Function 0, Register 0)
 * @param[in]    StdHeader    Header for library and services.
 */
typedef BOOLEAN OPTION_MULTISOCKET_GET_PCI_ADDRESS (
     OUT   PCI_ADDR               *PciAddress,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 *  This function writes to all nodes on the executing core's socket.
 *
 *  @param[in]     PciAddress    The Function and Register to update
 *  @param[in]     Mask          The bitwise AND mask to apply to the current register value
 *  @param[in]     Data          The bitwise OR mask to apply to the current register value
 *  @param[in]     StdHeader     Header for library and services.
 *
 */
typedef VOID OPTION_MULTISOCKET_MODIFY_CURR_SOCKET_PCI (
  IN       PCI_ADDR               *PciAddress,
  IN       UINT32                 Mask,
  IN       UINT32                 Data,
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
  OPTION_MULTISOCKET_GET_PCI_ADDRESS *GetCurrPciAddr;              ///< Method: Get PCI Config Space Address for the current running core.
  OPTION_MULTISOCKET_MODIFY_CURR_SOCKET_PCI *ModifyCurrSocketPci;  ///< Method: Writes to all nodes on the executing core's socket.
} OPTION_MULTISOCKET_CONFIGURATION;

/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */


#endif  // _OPTION_MULTISOCKET_H_
