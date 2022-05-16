/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Family Translation functions.
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

#ifndef _CPU_FAMILY_TRANSLATION_H_
#define _CPU_FAMILY_TRANSLATION_H_

/**
 * @page cpuimplfss CPU Family Specific Services Implementation Guide
 *
 * CPU Family Specific Services provides access to supported family service functions and data,
 * in a manner that isolates calling code from knowledge about particular families or which
 * families are supported in the current build.
 *
 * @par Adding a Method to Family Specific Services
 *
 * To add a new method to Family Specific Services, follow these steps.
 * <ul>
 * <li> Create a typedef for the Method with the correct parameters and return type.
 *
 *   <ul>
 *   <li> Name the method typedef (*PF_METHOD_NAME)(), where METHOD_NAME is the same name as the method table item,
 *     but with "_"'s and UPPERCASE, rather than mixed case.
 *     @n <tt> typedef VOID (*PF_METHOD_NAME)(); </tt> @n
 *
 *   <li> [Optionally make the type F_<name> and provide a separate:
 *     @n <tt> typedef F_METHOD_NAME *PF_METHOD_NAME> </tt> @n
 *     and provide a single line "///" doxygen comment brief description on the PF_ type.]
 *   </ul>
 *
 * <li> The first parameter to @b all Family Specific Service Methods is @b required to be a reference to
 *   their Family Service struct.
 *   @n <tt> IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices </tt> @n
 *
 * <li> Provide a standard doxygen function preamble for the Method typedef.  Begin the
 *   detailed description by provide a reference to the method instances page by including
 *   the lines below:
 *   @code
 *   *
 *   * @CpuServiceInstances
 *   *
 *   @endcode
 *   @note It is important to provide documentation for the method type, because the method may not
 *   have an implementation in any families supported by the current package. @n
 *
 * <li> Add to the CPU_SPECIFIC_SERVICES struct an item for the Method:
 *   @n <tt>  PF_METHOD_NAME MethodName; ///< Method: description. </tt> @n
 * </ul>
 *
 * @par Implementing a Family Specific Instance of the method.
 *
 * To implement an instance of a method for a specific family follow these steps.
 *
 * - In appropriate files in the family specific directory, implement the method with the return type
 *   and parameters matching the method typedef.
 *
 * - Name the function FnnMethodName(), where nn is the family number.
 *
 * - Create a doxygen function preamble for the method instance.  Begin the detailed description with
 *   an Implements command to reference the method type and add this instance to the Method Instances page.
 *   @code
 *   *
 *   *  @CpuServiceMethod{::F_METHOD_NAME}.
 *   *
 *   @endcode
 *
 * - To access other family specific services as part of the method implementation, the function
 *   @b must use FamilySpecificServices->OtherMethod().  Do not directly call other family specific
 *   routines, because in the table there may be overrides or this routine may be shared by multiple families.
 *
 * - Do @b not call Family translation services from a family specific instance.  Use the parameter.
 *
 * - Add the instance to the family specific CPU_SPECIFIC_SERVICES instance.
 *
 * - If a family does not need an instance of the method use one of the CommonReturns from
 *   CommonReturns.h with the same return type.
 *
 * @par Invoking Family Specific Services.
 *
 * The following example shows how to invoke a family specific method.
 * @n @code
 *        CPU_SPECIFIC_SERVICES *FamilyServices;
 *
 *        GetCpuServicesOfCurrentCore (&FamilyServices, StdHeader);
 *        ASSERT (FamilyServices != NULL);
 *        FamilyServices->MethodName (FamilyServices, StdHeader);
 * @endcode
 *
 */


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */
#include "cpuPostInit.h"
#include "cpuEnvInit.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "Table.h"
#include "Ids.h"
#include "Topology.h"

//  Forward declaration needed for multi-structure mutual references.
AGESA_FORWARD_DECLARATION (CPU_SPECIFIC_SERVICES);
/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/**
 *  Disable the desired P-state.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StateNumber             Hardware P-state number.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_DISABLE_PSTATE (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_DISABLE_PSTATE *PF_CPU_DISABLE_PSTATE;

/**
 *  Transition the current core to the desired P-state.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StateNumber             Software P-state number.
 *  @param[in]     WaitForChange           Wait/don't wait for P-state change to complete.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_TRANSITION_PSTATE (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
  IN       BOOLEAN WaitForChange,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_TRANSITION_PSTATE *PF_CPU_TRANSITION_PSTATE;

/**
 *  Get the desired P-state's maximum current required in milliamps.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StateNumber             The desired hardware P-state number.
 *  @param[out]    ProcIddMax              The P-state's maximum current.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @retval        TRUE                    The P-state is enabled, and ProcIddMax is valid.
 *  @retval        FALSE                   The P-state is disabled.
 *
 */
typedef BOOLEAN F_CPU_GET_IDD_MAX (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *ProcIddMax,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_IDD_MAX *PF_CPU_GET_IDD_MAX;


/**
 *  Returns the rate at which the current core's timestamp counter increments in megahertz.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[out]    FreqInMHz               The rate at which the TSC increments in megahertz.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_GET_TSC_RATE (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   UINT32 *FreqInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_TSC_RATE *PF_CPU_GET_TSC_RATE;

/**
 *  Returns the processor north bridge's clock rate in megahertz.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[out]    FreqInMHz               The desired node's frequency in megahertz.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @retval        AGESA_SUCCESS           FreqInMHz is valid.
 */
typedef AGESA_STATUS F_CPU_GET_NB_FREQ (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   UINT32 *FreqInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_NB_FREQ *PF_CPU_GET_NB_FREQ;

/**
 *  Returns the node's minimum and maximum northbridge frequency.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     PlatformConfig          Platform profile/build option config structure.
 *  @param[in]     PciAddress              The segment, bus, and device numbers of the CPU in question.
 *  @param[out]    MinFreqInMHz            The minimum north bridge frequency.
 *  @param[out]    MaxFreqInMHz            The maximum north bridge frequency.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @retval        AGESA_STATUS            Northbridge frequency is valid
 */
typedef AGESA_STATUS F_CPU_GET_MIN_MAX_NB_FREQ (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR *PciAddress,
     OUT   UINT32 *MinFreqInMHz,
     OUT   UINT32 *MaxFreqInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_MIN_MAX_NB_FREQ *PF_CPU_GET_MIN_MAX_NB_FREQ;

/**
 *  Returns the processor north bridge's P-state settings.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     PlatformConfig          Platform profile/build option config structure.
 *  @param[in]     PciAddress              The segment, bus, and device numbers of the CPU in question.
 *  @param[in]     NbPstate                The NB P-state number to check.
 *  @param[out]    FreqNumeratorInMHz      The desired node's frequency numerator in megahertz.
 *  @param[out]    FreqDivisor             The desired node's frequency divisor.
 *  @param[out]    VoltageInuV             The desired node's voltage in microvolts.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @retval        TRUE                    NbPstate is valid
 *  @retval        FALSE                   NbPstate is disabled or invalid
 */
typedef BOOLEAN F_CPU_GET_NB_PSTATE_INFO (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR *PciAddress,
  IN       UINT32 NbPstate,
     OUT   UINT32 *FreqNumeratorInMHz,
     OUT   UINT32 *FreqDivisor,
     OUT   UINT32 *VoltageInuV,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_NB_PSTATE_INFO *PF_CPU_GET_NB_PSTATE_INFO;

/**
 *  Returns whether or not the NB frequency initialization sequence is required
 *  to be performed by the BIOS.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     PciAddress              The northbridge to query by pci base address.
 *  @param[out]    NbVidUpdateAll          Do all NbVids need to be updated as well.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef BOOLEAN F_CPU_IS_NBCOF_INIT_NEEDED (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PCI_ADDR *PciAddress,
     OUT   BOOLEAN *NbVidUpdateAll,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_IS_NBCOF_INIT_NEEDED *PF_CPU_IS_NBCOF_INIT_NEEDED;

/**
 *  Get the desired NB P-state's maximum current required in milliamps.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StateNumber             The desired hardware P-state number.
 *  @param[out]    NbIddMax                The NB P-state's maximum current.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @retval        TRUE                    The NB P-state is enabled, and NbIddMax is valid.
 *  @retval        FALSE                   The NB P-state is disabled.
 *
 */
typedef BOOLEAN F_CPU_GET_NB_IDD_MAX (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *NbIddMax,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_NB_IDD_MAX *PF_CPU_GET_NB_IDD_MAX;

/**
 *  Launches the desired core from the reset vector.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     SocketNumber            The desired core's socket number.
 *  @param[in]     ModuleNumber            The desired core's die number.
 *  @param[in]     CoreNumber              The desired core's die relative core number.
 *  @param[in]     PrimaryCoreNumber       SocketNumber / ModuleNumber's primary core number.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @retval        TRUE                    The core was launched successfully.
 *  @retval        FALSE                   The core was previously launched, or has a problem.
 */
typedef BOOLEAN F_CPU_AP_INITIAL_LAUNCH (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT32 SocketNumber,
  IN       UINT32 ModuleNumber,
  IN       UINT32 CoreNumber,
  IN       UINT32 PrimaryCoreNumber,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_AP_INITIAL_LAUNCH *PF_CPU_AP_INITIAL_LAUNCH;

/**
 *  Returns the appropriate number of physical processor cores
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @return        One-based number of physical cores on current processor
 */
typedef UINT8 F_CPU_NUMBER_OF_PHYSICAL_CORES (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_NUMBER_OF_PHYSICAL_CORES *PF_CPU_NUMBER_OF_PHYSICAL_CORES;

/**
 *  Returns a family specific table of information pointer and size.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[out]    FamilySpecificArray     Pointer to the appropriate list for the core.
 *  @param[out]    NumberOfElements        Number of valid entries FamilySpecificArray.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID F_CPU_GET_FAMILY_SPECIFIC_ARRAY (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   CONST VOID **FamilySpecificArray,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_FAMILY_SPECIFIC_ARRAY *PF_CPU_GET_FAMILY_SPECIFIC_ARRAY;

/**
 *  Returns a model specific list of logical IDs.
 *
 *  @param[out]    LogicalIdXlat           Installed logical ID table.
 *  @param[out]    NumberOfElements        Number of entries in the Logical ID translate table.
 *  @param[out]    LogicalFamily           Base logical family bit mask.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID F_CPU_GET_SUBFAMILY_ID_ARRAY (
     OUT   CONST CPU_LOGICAL_ID_XLAT **LogicalIdXlat,
     OUT   UINT8 *NumberOfElements,
     OUT   UINT64 *LogicalFamily,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a method.
typedef F_CPU_GET_SUBFAMILY_ID_ARRAY *PF_CPU_GET_SUBFAMILY_ID_ARRAY;

/**
 *  Use the Mailbox Register to get the Ap Mailbox info for the current core.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[out]    ApMailboxInfo           The AP Mailbox info
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID (F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   AP_MAILBOXES           *ApMailboxInfo,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a method
typedef F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE *PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE;

/**
 *  Set the AP core number in the AP's Mailbox.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     Socket                  The AP's socket
 *  @param[in]     Module                  The AP's module
 *  @param[in]     ApCoreNumber            The AP's unique core number
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID (F_CPU_SET_AP_CORE_NUMBER) (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT32                 Socket,
  IN       UINT32                 Module,
  IN       UINT32                 ApCoreNumber,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a method
typedef F_CPU_SET_AP_CORE_NUMBER *PF_CPU_SET_AP_CORE_NUMBER;

/**
 *  Get the AP core number from hardware.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @return        The AP's unique core number
 */
typedef UINT32 (F_CPU_GET_AP_CORE_NUMBER) (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a method
typedef F_CPU_GET_AP_CORE_NUMBER *PF_CPU_GET_AP_CORE_NUMBER;

/**
 *  Move the AP's core number from the mailbox to hardware.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @return        The AP's unique core number
 */
typedef VOID (F_CPU_TRANSFER_AP_CORE_NUMBER) (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a method
typedef F_CPU_TRANSFER_AP_CORE_NUMBER *PF_CPU_TRANSFER_AP_CORE_NUMBER;

/**
 * Core ID position in the initial APIC ID, reflected as a number zero or one.
 */
typedef enum {
  CoreIdPositionZero, ///< Zero, the Core Id bits are the Most Significant bits.
  CoreIdPositionOne,  ///< One, the Core Id bits are the Least Significant bits.
  CoreIdPositionMax   ///< Limit check.
} CORE_ID_POSITION;

/**
 * Return a number zero or one, based on the Core ID position in the initial APIC Id.
 *
 * @CpuServiceInstances
 *
 * @param[in]     FamilySpecificServices  The current Family Specific Services.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 * @retval        CoreIdPositionZero      Core Id is not low
 * @retval        CoreIdPositionOne       Core Id is low
 */
typedef CORE_ID_POSITION F_CORE_ID_POSITION_IN_INITIAL_APIC_ID (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a method
typedef F_CORE_ID_POSITION_IN_INITIAL_APIC_ID *PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID;

/**
 *  Get least common features set of all CPUs and save them to CPU_FEATURES_LIST
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in,out] cpuFeatureListPtr       The CPU Features List
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID (F_CPU_SAVE_FEATURES) (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   CPU_FEATURES_LIST      *cpuFeatureListPtr,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a method
typedef F_CPU_SAVE_FEATURES *PF_CPU_SAVE_FEATURES;

/**
 *  Get least common features from CPU_FEATURES_LIST and write them to CPU
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in,out] cpuFeatureListPtr       The CPU Features List
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID (F_CPU_WRITE_FEATURES) (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   CPU_FEATURES_LIST      *cpuFeatureListPtr,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a method
typedef F_CPU_WRITE_FEATURES *PF_CPU_WRITE_FEATURES;

/**
 *  Set Warm Reset Flag
 *
 *  @CpuServiceInstances
 *
 * @param[in]      FamilySpecificServices  The current Family Specific Services.
 * @param[in]      StdHeader     Header for library and services.
 * @param[in]      Request       Value to set the flags to.
 *
 */
typedef VOID (F_CPU_SET_WARM_RESET_FLAG) (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       WARM_RESET_REQUEST *Request
  );

/// Reference to a method
typedef F_CPU_SET_WARM_RESET_FLAG *PF_CPU_SET_WARM_RESET_FLAG;

/**
 *  Get Warm Reset Flag
 *
 *  @CpuServiceInstances
 *
 * @param[in]      FamilySpecificServices  The current Family Specific Services.
 * @param[in]      StdHeader     Header for library and services.
 * @param[out]     BiosRstDet    Indicate warm reset status.
 *
 */
typedef VOID (F_CPU_GET_WARM_RESET_FLAG) (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS *StdHeader,
     OUT   WARM_RESET_REQUEST *Request
  );

/// Reference to a method
typedef F_CPU_GET_WARM_RESET_FLAG *PF_CPU_GET_WARM_RESET_FLAG;


/**
 *  Get CPU Specific Platform Type Info.
 *
 *  @CpuServiceInstances
 *
 *  @param[in]     FamilySpecificServices         The current Family Specific Services.
 *  @param[in,out] FeaturesUnion                  The Features supported by this platform.
 *  @param[in]     StdHeader                      Handle of Header for calling lib functions and services.
 *
 */
typedef AGESA_STATUS F_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   PLATFORM_FEATS         *FeaturesUnion,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO *PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO;

/**
 * Is the Northbridge PState feature enabled?
 *
 * @CpuServiceInstances
 *
 * @param[in]      FamilySpecificServices         The current Family Specific Services.
 * @param[in]      PlatformConfig                 Platform profile/build option config structure.
 * @param[in]      StdHeader                      Handle of Header for calling lib functions and services.
 *
 * @retval         TRUE                           The NB PState feature is enabled.
 * @retval         FALSE                          The NB PState feature is not enabled.
 */
typedef BOOLEAN F_IS_NB_PSTATE_ENABLED (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a method
typedef F_IS_NB_PSTATE_ENABLED *PF_IS_NB_PSTATE_ENABLED;

/**
 * Gets the next link with features matching the HT phy register table entry type features.
 *
 *  @CpuServiceInstances
 *
 * @param[in]     FamilySpecificServices   The current Family Specific Services.
 * @param[in,out] HtHostCapability Initially the PCI bus, device, function=0, offset=0;
 *                                         Each call returns the HT Host Capability function and offset;
 *                                         Caller may use it to access registers, but must @b not modify it;
 *                                         Each new call passes the previous value as input.
 * @param[in,out] Link             Initially zero, each call returns the link number; caller passes it back unmodified each call.
 * @param[in]     HtPhyLinkType    Link type field from a register table entry to compare against
 * @param[out]    MatchedSublink1  TRUE: It is actually just sublink 1 that matches, FALSE: any other condition.
 * @param[out]    Frequency0       The frequency of sublink0 (200 MHz if not connected).
 * @param[out]    Frequency1       The frequency of sublink1 (200 MHz if not connected).
 * @param[in]     StdHeader        Standard Head Pointer
 *
 * @retval        TRUE             Link matches
 * @retval        FALSE            No more links
 *
 */
typedef BOOLEAN F_NEXT_LINK_HAS_HTFPY_FEATS (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   PCI_ADDR           *HtHostCapability,
  IN OUT   UINT32             *Link,
  IN       HT_PHY_LINK_FEATS  *HtPhyLinkType,
     OUT   BOOLEAN            *MatchedSublink1,
     OUT   HT_FREQUENCIES     *Frequency0,
     OUT   HT_FREQUENCIES     *Frequency1,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );
/// Reference to a Method.
typedef F_NEXT_LINK_HAS_HTFPY_FEATS *PF_NEXT_LINK_HAS_HTFPY_FEATS;

/**
 * Applies an HT Phy read-modify-write based on an HT Phy register table entry.
 *
 *  @CpuServiceInstances
 *
 * @param[in]  FamilySpecificServices    The current Family Specific Services.
 * @param[in]  HtPhyEntry                HT Phy register table entry to apply
 * @param[in]  CapabilitySet             The link's HT Host base address.
 * @param[in]  Link                      Zero based, node, link number (not package link), always a sublink0 link.
 * @param[in]  StdHeader                 Config handle for library and services
 *
 */
typedef VOID F_SET_HT_PHY_REGISTER (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       HT_PHY_TYPE_ENTRY_DATA  *HtPhyEntry,
  IN       PCI_ADDR                CapabilitySet,
  IN       UINT32                  Link,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
/// Reference to a Method.
typedef F_SET_HT_PHY_REGISTER *PF_SET_HT_PHY_REGISTER;

/**
 * Performs an early initialization function on the executing core.
 *
 * @param[in]  FamilyServices            The current Family Specific Services.
 * @param[in]  EarlyParams               CPU module early paramters.
 * @param[in]  StdHeader                 Config handle for library and services
 *
 */
typedef VOID F_PERFORM_EARLY_INIT_ON_CORE (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
/// Reference to a Method.
typedef F_PERFORM_EARLY_INIT_ON_CORE *PF_PERFORM_EARLY_INIT_ON_CORE;

/**
 * A struct that contains function pointer and function flag
 *
 * the flag indicates if the function need to be run.
 */
typedef struct _S_PERFORM_EARLY_INIT_ON_CORE {
  PF_PERFORM_EARLY_INIT_ON_CORE PerformEarlyInitOnCore; ///< Function Pointer, which points to the function need to be run at early stage
  UINT32 PerformEarlyInitFlag; ///< Function Flag, which indicates if the function need to be run.
} S_PERFORM_EARLY_INIT_ON_CORE;

/**
 * Returns the initialization steps that the executing core should
 * perform at AmdInitEarly.
 *
 *  @CpuServiceInstances
 *
 * @param[in]  FamilyServices            The current Family Specific Services.
 * @param[out] Table                     Table of appropriate init steps for the executing core.
 * @param[in]  EarlyParams               CPU module early paramters.
 * @param[in]  StdHeader                 Config handle for library and services
 *
 */
typedef VOID F_GET_EARLY_INIT_TABLE (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  );
/// Reference to a Method.
typedef F_GET_EARLY_INIT_TABLE *PF_GET_EARLY_INIT_TABLE;

/**
 * Provide the features of the next HT link.
 *
 *  @CpuServiceInstances
 *
 * This method is different than the HT Phy Features method, because for the phy registers
 * sublink 1 matches and should be programmed if the link is ganged but for PCI config
 * registers sublink 1 is reserved if the link is ganged.
 *
 * @param[in]     FamilySpecificServices    The current Family Specific Services.
 * @param[in,out] Link                      The link number, for accessing non-capability set registers.
 *                                          Zero on initial call, and passed back unmodified on each subsequent call.
 * @param[in,out] LinkBase                  IN: initially the node's PCI config base address, passed back on each call.
 *                                          OUT: the base HT Host capability PCI address for the link.
 * @param[out]    HtHostFeats               The link's features.
 * @param[in]     StdHeader                 Standard Head Pointer
 *
 * @retval        TRUE                      Valid link and features found.
 * @retval        FALSE                     No more links.
 */
typedef BOOLEAN F_GET_NEXT_HT_LINK_FEATURES (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   UINTN                  *Link,
  IN OUT   PCI_ADDR               *LinkBase,
     OUT   HT_HOST_FEATS          *HtHostFeats,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
/// Reference to a Method.
typedef F_GET_NEXT_HT_LINK_FEATURES *PF_GET_NEXT_HT_LINK_FEATURES;

/// Cache Enable / Disable policy before giving control back to OS.
typedef enum {
  InitCacheDisabled,            ///<Disable cache CR0.CD bit
  InitCacheEnabled              ///<Enable cache CR0.CD bit
} FAMILY_CACHE_INIT_POLICY;


/*---------------------------------------------------------------------------------------*/
/**
 * Provide the interface to all cpu Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 * See CPU Family Specific Services Implementation Guide for adding new services.
 */
struct _CPU_SPECIFIC_SERVICES {                                         // See the Forwaqrd Declaration above
  UINT16          Revision;                                             ///< Interface version
  // Public Methods.
  PF_CPU_DISABLE_PSTATE DisablePstate;                                  ///< Method: Disable the desired P-state.
  PF_CPU_TRANSITION_PSTATE TransitionPstate;                            ///< Method: Transition the current core to the desired P-state.
  PF_CPU_GET_IDD_MAX GetProcIddMax;                                     ///< Method: Gets P-state maximum current required
  PF_CPU_GET_TSC_RATE GetTscRate;                                       ///< Method: Returns the rate at which the current core's timestamp counter increments in megahertz.
  PF_CPU_GET_NB_FREQ GetCurrentNbFrequency;                             ///< Method: Returns the processor north bridge's clock rate in megahertz.
  PF_CPU_GET_MIN_MAX_NB_FREQ GetMinMaxNbFrequency;                      ///< Method: Returns the node's minimum and maximum northbridge frequency.
  PF_CPU_GET_NB_PSTATE_INFO GetNbPstateInfo;                            ///< Method: Returns information about the processor north bridge's P-states.
  PF_CPU_IS_NBCOF_INIT_NEEDED IsNbCofInitNeeded;                        ///< Method: Returns whether or not the NB frequency initialization sequence is required to be performed by the BIOS.
  PF_CPU_GET_NB_IDD_MAX GetNbIddMax;                                    ///< Method: Gets NB P-state maximum current required
  PF_CPU_AP_INITIAL_LAUNCH LaunchApCore;                                ///< Method: Launches the desired core from the reset vector.
  PF_CPU_NUMBER_OF_PHYSICAL_CORES GetNumberOfPhysicalCores;             ///< Method: Get the number of physical cores of current processor.
  PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE GetApMailboxFromHardware;     ///< Method: Get the AP's topology info from the hardware mailbox.
  PF_CPU_SET_AP_CORE_NUMBER SetApCoreNumber;                            ///< Method: Set the AP's core number to the hardware mailbox.
  PF_CPU_GET_AP_CORE_NUMBER GetApCoreNumber;                            ///< Method: Get the AP's core number from hardware.
  PF_CPU_TRANSFER_AP_CORE_NUMBER TransferApCoreNumber;                  ///< Method: Move the AP's core number from the mailbox to hardware.
  PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID CoreIdPositionInInitialApicId; ///< Method: Which bits in initial APIC Id are the Core Id.
  PF_CPU_SAVE_FEATURES SaveFeatures;                                    ///< Method: Get least common features set of all CPUs and save them to CPU_FEATURES_LIST
  PF_CPU_WRITE_FEATURES WriteFeatures;                                  ///< Method: Get least common features from CPU_FEATURES_LIST and write them to CPU
  PF_CPU_SET_WARM_RESET_FLAG SetWarmResetFlag;                          ///< Method: Set Warm Reset Flag
  PF_CPU_GET_WARM_RESET_FLAG GetWarmResetFlag;                          ///< Method: Get Warm Reset Flag
  PF_CPU_GET_FAMILY_SPECIFIC_ARRAY GetBrandString1;                     ///< Method: Get a Brand String table
  PF_CPU_GET_FAMILY_SPECIFIC_ARRAY GetBrandString2;                     ///< Method: Get a Brand String table
  PF_CPU_GET_FAMILY_SPECIFIC_ARRAY GetMicroCodePatchesStruct;           ///< Method: Get microcode patches
  PF_CPU_GET_FAMILY_SPECIFIC_ARRAY GetMicrocodeEquivalenceTable;        ///< Method: Get CPU equivalence for loading microcode patches.
  PF_CPU_GET_FAMILY_SPECIFIC_ARRAY GetCacheInfo;                        ///< Method: Get setup for cache use and initialization.
  PF_CPU_GET_FAMILY_SPECIFIC_ARRAY GetSysPmTableStruct;                 ///< Method: Get Power Management settings.
  PF_CPU_GET_FAMILY_SPECIFIC_ARRAY GetWheaInitData;                     ///< Method: Get Whea Initial Data.
  PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO GetPlatformTypeSpecificInfo;   ///< Method: Get Specific platform Type features.
  PF_IS_NB_PSTATE_ENABLED IsNbPstateEnabled;                            ///< Method: Get whether Northbridge PStates feature is enabled.
  PF_NEXT_LINK_HAS_HTFPY_FEATS NextLinkHasHtPhyFeats;                   ///< Method: Iterate over HT Links matching features, for HT PHY entries.
  PF_SET_HT_PHY_REGISTER SetHtPhyRegister;                              ///< Method: Set an Ht Phy register based on table entry.
  PF_GET_NEXT_HT_LINK_FEATURES GetNextHtLinkFeatures;                   ///< Method: Iterate over HT links, returning link features.
  CONST REGISTER_TABLE **RegisterTableList;                                   ///< Public Data: The available register tables.
  TABLE_ENTRY_TYPE_DESCRIPTOR *TableEntryTypeDescriptors;               ///< Public Data: implemented register table entry types.
  PACKAGE_HTLINK_MAP PackageLinkMap;                                    ///< Public Data: translate northbridge HT links to package level links, or NULL.
  CORE_PAIR_MAP *CorePairMap;                                           ///< Public Data: translate compute unit core pairing, or NULL.
  FAMILY_CACHE_INIT_POLICY InitCacheDisabled;                           ///< public Data: Family related information.
  PF_GET_EARLY_INIT_TABLE GetEarlyInitOnCoreTable;                      ///< Method: Get the initialization steps needed at AmdInitEarly.
};

/**
 * A Family Id and an interface to it's implementations of Family Specific Services.
 *
 * Note that this is a logical family id, which may specify family, model (or even stepping).
 */
typedef struct {
  UINT64          Family;                         ///< The Family to which this interface belongs.
  CONST VOID      *TablePtr;                      ///< The interface to its Family Specific Services.
} CPU_SPECIFIC_SERVICES_XLAT;

/**
 * A collection of Family specific interfaces to Family Specific services.
 */
typedef struct {
  UINT8  Elements;                                       ///< The number of tables to search.
  CONST    CPU_SPECIFIC_SERVICES_XLAT *FamilyTable;      ///< The family interfaces.
} CPU_FAMILY_SUPPORT_TABLE;

/**
 * Implement the translation of a logical CPU id to an id that can be used to get Family specific services.
 */
typedef struct {
  UINT32 Family;                                              ///< Provide translation for this family
  CPU_LOGICAL_ID UnknownRevision;                             ///< In this family, unrecognized models (or steppings) are treated as though they were this model and stepping.
  CONST    PF_CPU_GET_SUBFAMILY_ID_ARRAY *SubFamilyIdTable;   ///< Method: Get family specific model (and stepping) resolution.
  UINT8  Elements;                                            ///< The number of family specific model tables pointed to by SubFamilyIdTable
} CPU_LOGICAL_ID_FAMILY_XLAT;

/**
 * A collection of all available family id translations.
 */
typedef struct {
  UINT8  Elements;                                            ///< The number of family translation items to search.
  CONST    CPU_LOGICAL_ID_FAMILY_XLAT *FamilyIdTable;         ///< The family translation items.
} CPU_FAMILY_ID_XLAT_TABLE;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

/**
 * Get a logical identifier for the specified processor, based on CPUID, but independent of CPUID formatting.
 */
VOID
GetLogicalIdOfSocket (
  IN       UINT32 Socket,
     OUT   CPU_LOGICAL_ID *LogicalId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * Get a logical identifier for the executing core, based on CPUID, but independent of CPUID formatting.
 */
VOID
GetLogicalIdOfCurrentCore (
     OUT   CPU_LOGICAL_ID *LogicalId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * Get a logical identifier for the specified CPUID value.
 */
VOID
GetLogicalIdFromCpuid (
  IN       UINT32 RawCpuid,
     OUT   CPU_LOGICAL_ID *LogicalId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 *  Retrieves a pointer to the desired processor's family specific services structure.
 */
VOID
GetCpuServicesOfSocket (
  IN       UINT32 Socket,
     OUT   CONST CPU_SPECIFIC_SERVICES **FunctionTable,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 *  Retrieves a pointer to the desired processor's family specific services structure.
 */
VOID
GetFeatureServicesOfSocket (
  IN       CPU_FAMILY_SUPPORT_TABLE *FamilyTable,
  IN       UINT32            Socket,
     OUT   CONST VOID        **CpuServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 *  Retrieves a pointer to the executing core's family specific services structure.
 */
VOID
GetCpuServicesOfCurrentCore (
     OUT   CONST CPU_SPECIFIC_SERVICES **FunctionTable,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 *  Retrieves a pointer to the executing core's family specific services structure.
 */
VOID
GetFeatureServicesOfCurrentCore (
  IN       CPU_FAMILY_SUPPORT_TABLE *FamilyTable,
     OUT   CONST VOID        **CpuServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 *  Retrieves a pointer to the family specific services structure for a processor
 *  with the given logical ID.
 */
VOID
GetCpuServicesFromLogicalId (
  IN       CPU_LOGICAL_ID *LogicalId,
     OUT   CONST CPU_SPECIFIC_SERVICES **FunctionTable,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 *  Retrieves a pointer to the family specific services structure for a processor
 *  with the given logical ID.
 */
VOID
GetFeatureServicesFromLogicalId (
  IN       CPU_FAMILY_SUPPORT_TABLE *FamilyTable,
  IN       CPU_LOGICAL_ID    *LogicalId,
     OUT   CONST VOID        **CpuServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * Used by logical families which don't need a certain register setting table or other data array.
 */
VOID
GetEmptyArray (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   CONST VOID **Empty,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif  // _CPU_FAMILY_TRANSLATION_H_
