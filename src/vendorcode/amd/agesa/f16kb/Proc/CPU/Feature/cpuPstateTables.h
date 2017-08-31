/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU Pstate Table Functions declarations.
 *
 * Contains code that declares the AGESA CPU _PSS related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_PSTATE_TABLES_H_
#define _CPU_PSTATE_TABLES_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */
//  Forward declaration needed for multi-structure mutual references
AGESA_FORWARD_DECLARATION (PSTATE_CPU_FAMILY_SERVICES);

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// P-state structure for each state
typedef struct {
  IN OUT   UINT32               PStateEnable;                ///< Pstate enable
  IN OUT   UINT32               CoreFreq;                    ///< MHz
  IN OUT   UINT32               Power;                       ///< milliWatts
  IN OUT   UINT32               IddValue;                    ///< Current value field
  IN OUT   UINT32               IddDiv;                      ///< Current divisor field
  IN OUT   UINT32               SwPstateNumber;              ///< Software P-state number
} S_PSTATE_VALUES;

/// P-state structure for each core
typedef struct {
  IN OUT   UINT8                PStateMaxValue;              ///< Max p-state number in this core
  IN OUT   UINT8                HtcPstateLimit;              ///< Htc limit
  IN OUT   UINT8                HtcCapable;                  ///< Htc capable
  IN OUT   UINT8                LocalApicId;                 ///< Local Apic Id
  IN OUT   UINT8                NumberOfBoostedStates;       ///< Number of boost P-states
  IN OUT   S_PSTATE_VALUES      PStateStruct[];              ///< P state struc
} S_PSTATE;

/// P-state structure for each node
typedef struct {
  IN       UINT8                SetPState0;                  ///< If value = 0x55 (Don't set PState0)
  IN       UINT8                TotalCoresInNode;            ///< core number per node
  IN       UINT16               PStateLevelingSizeOfBytes;   ///< Size
  IN       BOOLEAN              OnlyOneEnabledPState;        ///< Only P0
  IN       UINT8                InitStruct;                  ///< Init struc
  IN       BOOLEAN              AllCpusHaveIdenticalPStates; ///< Have Identical p state
  IN       UINT8                CreateAcpiTables;            ///< Create table flag
  IN       UINT8                SocketNumber;                ///< Physical socket number of this socket
  IN       UINT8                Reserved[2];                 ///< Reserved.
  IN OUT   S_PSTATE             PStateCoreStruct[1];         ///< P state core struc
} PSTATE_LEVELING;

/// P-state structure for whole system
typedef struct {
  IN OUT   UINT32               TotalSocketInSystem;         ///< Total node number in system
  IN OUT   UINT32               SizeOfBytes;                 ///< Structure size
  IN OUT   PSTATE_LEVELING      PStateLevelingStruc[1];      ///< P state level structure
} S_CPU_AMD_PSTATE;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if PSD need to be generated.
 *
 * @param[in]     PstateCpuFamilyServices  Pstate CPU services.
 * @param[in,out] PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]     StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               PSD need to be generated
 * @retval       FALSE              PSD does NOT need to be generated
 *
 */
typedef BOOLEAN F_PSTATE_PSD_IS_NEEDED (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN OUT   PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_PSTATE_PSD_IS_NEEDED *PF_PSTATE_PSD_IS_NEEDED;


/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if Pstate PSD is dependent.
 *
 * @param[in]     PstateCpuFamilyServices  Pstate CPU services.
 * @param[in,out] PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]     StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               PSD is dependent.
 * @retval       FALSE              PSD is independent.
 *
 */
typedef BOOLEAN F_PSTATE_PSD_IS_DEPENDENT (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN OUT   PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_PSTATE_PSD_IS_DEPENDENT *PF_PSTATE_PSD_IS_DEPENDENT;

/**
 *  Family specific call to get CPU pstate transition latency for current socket.
 *
 *  @param[in]     PstateCpuFamilyServices        Pstate CPU services.
 *  @param[in]     PStateLevelingBufferStructPtr  Pstate row data buffer pointer.
 *  @param[in]     PciAddress                     Pci address struct.
 *  @param[out]    TransitionLatency              Pstate Transition latency result.
 *  @param[in]     StdHeader                      Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_PSTATE_TRANSITION_LATENCY (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       PSTATE_LEVELING        *PStateLevelingBufferStructPtr,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *TransitionLatency,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_PSTATE_TRANSITION_LATENCY *PF_CPU_PSTATE_TRANSITION_LATENCY;

/**
 *  Family specific call to get the desired P-state's frequency in megahertz.
 *
 *  @param[in]     PstateCpuFamilyServices Pstate CPU services.
 *  @param[in]     StateNumber             P-state number.
 *  @param[out]    PowerInMw               P-state frequency in megahertz.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_GET_PSTATE_FREQ (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *FreqInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_PSTATE_FREQ *PF_CPU_GET_PSTATE_FREQ;

/**
 *  Family specific call to set the system wide P-state settings on the current core.
 *
 *  @param[in]     PstateCpuFamilyServices Pstate CPU services.
 *  @param[in]     CpuAmdPState            The current core's P-state data.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_SET_PSTATE_LEVELING_REG (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       S_CPU_AMD_PSTATE   *CpuAmdPState,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_SET_PSTATE_LEVELING_REG *PF_CPU_SET_PSTATE_LEVELING_REG;

/**
 *  Family specific call to get the desired P-state's rated power in milliwatts.
 *
 *  @param[in]     PstateCpuFamilyServices Pstate CPU services.
 *  @param[in]     StateNumber             P-state number.
 *  @param[out]    PowerInMw               P-state power in milliwatts.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_GET_PSTATE_POWER (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *PowerInMw,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_PSTATE_POWER *PF_CPU_GET_PSTATE_POWER;

/**
 *  Family specific call to get CPU Pstate Max State.
 *
 *  @param[in]     PstateCpuFamilyServices        Pstate CPU services.
 *  @param[out]    MaxPStateNumber                The max hw pstate value on the current socket.
 *  @param[out]    NumberOfBoostStates            The number of boosted P-states on the current socket.
 *  @param[in]     StdHeader                      Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_GET_PSTATE_MAX_STATE (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
     OUT   UINT32              *MaxPStateNumber,
     OUT   UINT8               *NumberOfBoostStates,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_PSTATE_MAX_STATE *PF_CPU_GET_PSTATE_MAX_STATE;

/**
 *  Family specific call to get CPU pstate register information.
 *
 *  @param[in]     PstateCpuFamilyServices        Pstate CPU services.
 *  @param[in]     PState                         Input hardware Pstate number for query.
 *  @param[out]    PStateEnabled                  Boolean flag return pstate enable.
 *  @param[in,out] IddVal                         Pstate current value.
 *  @param[in,out] IddDiv                         Pstate current divisor.
 *  @param[out]    SwPstateNumber                 Software P-state number.
 *  @param[in]     StdHeader                      Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_GET_PSTATE_REGISTER_INFO (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT32              PState,
     OUT   BOOLEAN             *PStateEnabled,
  IN OUT   UINT32              *IddVal,
  IN OUT   UINT32              *IddDiv,
     OUT   UINT32              *SwPstateNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_PSTATE_REGISTER_INFO *PF_CPU_GET_PSTATE_REGISTER_INFO;

/**
 * Provide the interface to the Pstate dependent Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
struct _PSTATE_CPU_FAMILY_SERVICES {
  UINT16          Revision;                                         ///< Interface version
  // Public Methods.
  PF_PSTATE_PSD_IS_NEEDED IsPstatePsdNeeded;                        ///< Method: Family specific call to check if PSD need to be generated.
  PF_PSTATE_PSD_IS_DEPENDENT IsPstatePsdDependent;                  ///< Method: Family specific call to check if PSD is dependent.
  PF_CPU_PSTATE_TRANSITION_LATENCY GetPstateLatency;                ///< Method: Family specific call to get pstate transition latency.
  PF_CPU_GET_PSTATE_FREQ GetPstateFrequency;                        ///< Method: Family specific call to get the desired P-state's frequency in megahertz.
  PF_CPU_SET_PSTATE_LEVELING_REG SetPStateLevelReg;                 ///< Method: Family specific call to set the system wide P-state settings on the current core.
  PF_CPU_GET_PSTATE_POWER GetPstatePower;                           ///< Method: Family specific call to get the desired P-state's rated power in milliwatts.
  PF_CPU_GET_PSTATE_MAX_STATE GetPstateMaxState;                    ///< Method: Family specific call to get pstate max state number.
  PF_CPU_GET_PSTATE_REGISTER_INFO GetPstateRegisterInfo;            ///< Method: Family specific call to get pstate register information.
};


/*----------------------------------------------------------------------------------------
 *                          F U N C T I O N S     P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PStateGatherData (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   S_CPU_AMD_PSTATE       *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

AGESA_STATUS
PStateLeveling (
  IN OUT   S_CPU_AMD_PSTATE  *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
CpuGetPStateLevelStructure (
     OUT   PSTATE_LEVELING     **PStateBufferPtr,
  IN       S_CPU_AMD_PSTATE    *CpuAmdPState,
  IN       UINT32              LogicalSocketNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

#endif  // _CPU_PSTATE_TABLES_H_
