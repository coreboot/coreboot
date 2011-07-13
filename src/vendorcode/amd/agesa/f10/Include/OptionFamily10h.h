/**
 * @file
 *
 * Structures and definitions for family 10h support
 *
 * This file defines the structures and definitions needed for installation
 * of family 10h processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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


#ifndef _OPTION_FAMILY_10H_H_
#define _OPTION_FAMILY_10H_H_

#ifdef OPTION_FAMILY10H
  #if OPTION_FAMILY10H == TRUE

    extern F_CPU_GET_PSTATE_POWER F10GetPstatePower;
    extern F_CPU_GET_PSTATE_FREQ F10GetPstateFrequency;
    extern F_CPU_DISABLE_PSTATE F10DisablePstate;
    extern F_CPU_TRANSITION_PSTATE F10TransitionPstate;
    extern F_CPU_GET_TSC_RATE F10GetTscRate;
    extern F_CPU_PSTATE_TRANSITION_LATENCY F10GetPstateTransLatency;
    extern F_CPU_GET_PSTATE_REGISTER_INFO F10GetPstateRegisterInfo;
    extern F_CPU_GET_PSTATE_MAX_STATE F10GetPstateMaxState;
    extern F_CPU_SET_PSTATE_LEVELING_REG F10PstateLevelingCoreMsrModify;
    extern F_CPU_GET_NB_FREQ F10GetNbFrequency;
    extern F_CPU_AP_INITIAL_LAUNCH F10LaunchApCore;
    extern F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE F10GetApMailboxFromHardware;
    extern F_CPU_SET_AP_CORE_NUMBER F10SetApCoreNumber;
    extern F_CPU_GET_AP_CORE_NUMBER F10GetApCoreNumber;
    extern F_CPU_TRANSFER_AP_CORE_NUMBER F10TransferApCoreNumber;
    extern F_CORE_ID_POSITION_IN_INITIAL_APIC_ID F10CpuAmdCoreIdPositionInInitialApicId;
    extern F_CPU_SAVE_FEATURES F10SaveFeatures;
    extern F_CPU_WRITE_FEATURES F10WriteFeatures;
    extern F_CPU_SET_WARM_RESET_FLAG F10SetAgesaWarmResetFlag;
    extern F_CPU_GET_WARM_RESET_FLAG F10GetAgesaWarmResetFlag;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10BrandIdString1;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10BrandIdString2;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10CacheInfo;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10SysPmTable;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10WheaInitData;
    //extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetEmptyArray;
    extern F_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO F10GetPlatformTypeSpecificInfo;
    extern F_DOES_LINK_HAVE_HTFPY_FEATS F10DoesLinkHaveHtPhyFeats;
    extern F_SET_HT_PHY_REGISTER F10SetHtPhyRegister;
    extern F_GET_HT_LINK_FEATURES F10GetHtLinkFeatures;
    extern F_DO_TABLE_ENTRY F10SetRegisterForHtLinkTokenEntry;
    extern CONST REGISTER_TABLE ROMDATA F10PciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10MsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10HtPhyRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10MultiLinkPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10SingleLinkPciRegisterTable;
    extern F_GET_EARLY_INIT_TABLE GetCommonEarlyInitOnCoreTable;
  #endif  // OPTION_FAMILY10H == TRUE
#endif  //  defined (OPTION_FAMILY10H)

#endif  // _OPTION_FAMILY_10H_H_
