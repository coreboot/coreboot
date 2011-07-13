/**
 * @file
 *
 * AGESA options structures
 *
 * Contains options control structures for the AGESA build options
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 6104 $   @e \$Date: 2008-05-19 06:54:08 -0500 (Mon, 19 May 2008) $
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
 * 
 *
 ***************************************************************************/


#ifndef _OPTIONS_H_
#define _OPTIONS_H_

/**
 * Provide topology limits for loops and runtime, based on supported families.
 */
typedef struct {
  UINT32              PlatformNumberOfSockets;                  ///< The limit to the number of processors based on
                                                                ///< supported families and other build options.
  UINT32              PlatformNumberOfModules;                  ///< The limit to the number of modules in a processor, based
                                                                ///< on supported families.
} OPTIONS_CONFIG_TOPOLOGY;

/**
 * Dispatch Table.
 *
 * The push high dispatcher uses this table to find what entries are currently in the build image.
 */
typedef struct {
  UINT32              FunctionId;                   ///< The function id specified.
  IMAGE_ENTRY         EntryPoint;                   ///< The corresponding entry point to call.
} DISPATCH_TABLE;

/*
 * Platform common build option and configuration processing.
 *
 * Defaults selection goes into the platform install .h file.  If the default is
 * not platform specific, it goes here.
 *
 */

  // HT Configuration

#ifdef BLDCFG_STARTING_BUSNUM
  #define CFG_STARTING_BUSNUM         (BLDCFG_STARTING_BUSNUM)
#else
  #define CFG_STARTING_BUSNUM         (0)
#endif

#ifdef BLDCFG_MAXIMUM_BUSNUM
  #define CFG_MAXIMUM_BUSNUM          (BLDCFG_MAXIMUM_BUSNUM)
#else
  #define CFG_MAXIMUM_BUSNUM          (0xF8)
#endif

#ifdef BLDCFG_ALLOCATED_BUSNUM
  #define CFG_ALLOCATED_BUSNUM        (BLDCFG_ALLOCATED_BUSNUM)
#else
  #define CFG_ALLOCATED_BUSNUM        (0x20)
#endif

#ifdef BLDCFG_BUID_SWAP_LIST
  #define CFG_BUID_SWAP_LIST         (BLDCFG_BUID_SWAP_LIST)
#else
  #define CFG_BUID_SWAP_LIST         (NULL)
#endif

#ifdef BLDCFG_HTDEVICE_CAPABILITIES_OVERRIDE_LIST
  #define CFG_HTDEVICE_CAPABILITIES_OVERRIDE_LIST         (BLDCFG_HTDEVICE_CAPABILITIES_OVERRIDE_LIST)
#else
  #define CFG_HTDEVICE_CAPABILITIES_OVERRIDE_LIST         (NULL)
#endif

#ifdef BLDCFG_HTFABRIC_LIMITS_LIST
  #define CFG_HTFABRIC_LIMITS_LIST         (BLDCFG_HTFABRIC_LIMITS_LIST)
#else
  #define CFG_HTFABRIC_LIMITS_LIST         (NULL)
#endif

#ifdef BLDCFG_HTCHAIN_LIMITS_LIST
  #define CFG_HTCHAIN_LIMITS_LIST         (BLDCFG_HTCHAIN_LIMITS_LIST)
#else
  #define CFG_HTCHAIN_LIMITS_LIST         (NULL)
#endif

#ifdef BLDCFG_BUS_NUMBERS_LIST
  #define CFG_BUS_NUMBERS_LIST         (BLDCFG_BUS_NUMBERS_LIST)
#else
  #define CFG_BUS_NUMBERS_LIST         (NULL)
#endif

#ifdef BLDCFG_IGNORE_LINK_LIST
  #define CFG_IGNORE_LINK_LIST         (BLDCFG_IGNORE_LINK_LIST)
#else
  #define CFG_IGNORE_LINK_LIST         (NULL)
#endif

#ifdef BLDCFG_LINK_SKIP_REGANG_LIST
  #define CFG_LINK_SKIP_REGANG_LIST         (BLDCFG_LINK_SKIP_REGANG_LIST)
#else
  #define CFG_LINK_SKIP_REGANG_LIST         (NULL)
#endif

#ifdef BLDCFG_SET_HTCRC_SYNC_FLOOD
  #define CFG_SET_HTCRC_SYNC_FLOOD         (BLDCFG_SET_HTCRC_SYNC_FLOOD)
#else
  #define CFG_SET_HTCRC_SYNC_FLOOD         (FALSE)
#endif

#ifdef BLDCFG_USE_UNIT_ID_CLUMPING
  #define CFG_USE_UNIT_ID_CLUMPING         (BLDCFG_USE_UNIT_ID_CLUMPING)
#else
  #define CFG_USE_UNIT_ID_CLUMPING         (FALSE)
#endif

#ifdef BLDCFG_ADDITIONAL_TOPOLOGIES_LIST
  #define CFG_ADDITIONAL_TOPOLOGIES_LIST         (BLDCFG_ADDITIONAL_TOPOLOGIES_LIST)
#else
  #define CFG_ADDITIONAL_TOPOLOGIES_LIST         (NULL)
#endif

#ifdef BLDCFG_USE_HT_ASSIST
  #define CFG_USE_HT_ASSIST               (BLDCFG_USE_HT_ASSIST)
#else
  #define CFG_USE_HT_ASSIST               (TRUE)
#endif

#ifdef BLDCFG_PLATFORM_CONTROL_FLOW_MODE
  #define CFG_PLATFORM_CONTROL_FLOW_MODE  (BLDCFG_PLATFORM_CONTROL_FLOW_MODE)
#else
  #define CFG_PLATFORM_CONTROL_FLOW_MODE  (Nfcm)
#endif

#ifdef BLDCFG_PLATFORM_DEEMPHASIS_LIST
  #define CFG_PLATFORM_DEEMPHASIS_LIST  (BLDCFG_PLATFORM_DEEMPHASIS_LIST)
#else
  #define CFG_PLATFORM_DEEMPHASIS_LIST  (NULL)
#endif

#ifdef BLDCFG_VRM_ADDITIONAL_DELAY
  #define CFG_VRM_ADDITIONAL_DELAY      (BLDCFG_VRM_ADDITIONAL_DELAY)
#else
  #define CFG_VRM_ADDITIONAL_DELAY      (0)
#endif

#ifdef BLDCFG_PLATFORM_POWER_POLICY_MODE
  #define CFG_PLATFORM_POWER_POLICY_MODE  (BLDCFG_PLATFORM_POWER_POLICY_MODE)
#else
  #define CFG_PLATFORM_POWER_POLICY_MODE  (Performance)
#endif

#ifdef BLDCFG_PCI_MMIO_BASE
  #define CFG_PCI_MMIO_BASE               (BLDCFG_PCI_MMIO_BASE)
#else
  #define CFG_PCI_MMIO_BASE               (0)
#endif

#ifdef BLDCFG_PCI_MMIO_SIZE
  #define CFG_PCI_MMIO_SIZE               (BLDCFG_PCI_MMIO_SIZE)
#else
  #define CFG_PCI_MMIO_SIZE               (0)
#endif

#ifdef BLDCFG_AP_MTRR_SETTINGS_LIST
  #define CFG_AP_MTRR_SETTINGS_LIST           (BLDCFG_AP_MTRR_SETTINGS_LIST)
#else
  #define CFG_AP_MTRR_SETTINGS_LIST           (NULL)
#endif

#endif // _OPTIONS_H_
