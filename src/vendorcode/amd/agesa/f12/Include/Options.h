/* $NoKeywords:$ */
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
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
