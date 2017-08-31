/* $NoKeywords:$ */
/**
 * @file
 *
 * IDS Option Install File
 *
 * This file generates the defaults tables for family 10h model 5 processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 34897 $   @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
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
#ifndef _OPTION_IDS_INSTALL_H_
#define _OPTION_IDS_INSTALL_H_
#include "Ids.h"
#include "IdsHt.h"
#include "IdsLib.h"
#ifdef __IDS_EXTENDED__
  #include OPTION_IDS_EXT_INSTALL_FILE
#endif

#define IDS_LATE_RUN_AP_TASK

#define M_HTIDS_PORT_OVERRIDE_HOOK (PF_HtIdsGetPortOverride)CommonVoid
#if (IDSOPT_IDS_ENABLED == TRUE)
  #if (IDSOPT_CONTROL_ENABLED == TRUE)
    // Check for all families which include HT Features.
  #endif
#endif // OPTION_IDS_LEVEL
CONST PF_HtIdsGetPortOverride ROMDATA pf_HtIdsGetPortOverride = M_HTIDS_PORT_OVERRIDE_HOOK;

#if (IDSOPT_IDS_ENABLED == TRUE)
  #if (AGESA_ENTRY_INIT_LATE == TRUE)
    #undef IDS_LATE_RUN_AP_TASK
    #define IDS_LATE_RUN_AP_TASK
  #endif
#endif // OPTION_IDS_LEVEL

#if (IDSOPT_TRACING_ENABLED == TRUE)
  #if (AGESA_ENTRY_INIT_POST == TRUE)
    #include <mu.h>
    CONST SCRIPT_FUNCTION ROMDATA ScriptFuncList[] = {
      { (UINTN) MemUWriteCachelines, "WriteCl(PhyAddrLo,BufferAddr,ClCnt)"},
      { (UINTN) MemUReadCachelines, "ReadCl(BufferAddr,PhyAddrLo,ClCnt)"},
      { (UINTN) MemUFlushPattern, "FlushCl(PhyAddrLo,ClCnt)"}
    };
  #else
    CONST SCRIPT_FUNCTION ROMDATA ScriptFuncList[] = {
      { (UINTN) CommonReturnFalse, "DefRet()"},
      { (UINTN) CommonReturnFalse, "DefRet()"},
      { (UINTN) CommonReturnFalse, "DefRet()"}
    };
  #endif
#endif


///Ids Feat Options
#if (IDSOPT_IDS_ENABLED == TRUE)
  #if (IDSOPT_CONTROL_ENABLED == TRUE)

    #ifndef OPTION_IDS_EXTEND_FEATS
      #define OPTION_IDS_EXTEND_FEATS
    #endif

    #define OPTION_IDS_FEAT_GNB_PLATFORMCFG\
                OPTION_IDS_FEAT_GNB_PLATFORMCFGF14

/*----------------------------------------------------------------------------
 *                        Family 14 feat blocks
 *
 *----------------------------------------------------------------------------
 */
    #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF14
    #ifdef OPTION_FAMILY14H
      #if OPTION_FAMILY14H == TRUE
        extern CONST IDS_FEAT_STRUCT ROMDATA IdsFeatGnbPlatformCfgBlockF14;
        #undef OPTION_IDS_FEAT_GNB_PLATFORMCFGF14
        #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF14 &IdsFeatGnbPlatformCfgBlockF14,
      #endif
    #endif

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatUcodeBlock =
    {
      IDS_FEAT_UCODE_UPDATE,
      IDS_ALL_CORES,
      IDS_UCODE,
      IDS_FAMILY_ALL,
      IdsSubUCode
    };

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatPowerPolicyBlock =
    {
      IDS_FEAT_POWER_POLICY,
      IDS_ALL_CORES,
      IDS_PLATFORMCFG_OVERRIDE,
      IDS_FAMILY_ALL,
      IdsSubPowerPolicyOverride
    };

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatTargetPstateBlock =
    {
      IDS_FEAT_TARGET_PSTATE,
      IDS_BSP_ONLY,
      IDS_INIT_LATE_AFTER,
      IDS_FAMILY_ALL,
      IdsSubTargetPstate
    };

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatPostPstateBlock =
    {
      IDS_FEAT_POSTPSTATE,
      IDS_ALL_CORES,
      IDS_CPU_Early_Override,
      IDS_FAMILY_ALL,
      IdsSubPostPState
    };

    //Dram controller Features
    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatDctAllMemClkBlock =
    {
      IDS_FEAT_DCT_ALLMEMCLK,
      IDS_BSP_ONLY,
      IDS_ALL_MEMORY_CLOCK,
      IDS_FAMILY_ALL,
      IdsSubAllMemClkEn
    };

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatDctGangModeBlock =
    {
      IDS_FEAT_DCT_GANGMODE,
      IDS_BSP_ONLY,
      IDS_GANGING_MODE,
      IDS_FAMILY_ALL,
      IdsSubGangingMode
    };

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatDctBurstLengthBlock =
    {
      IDS_FEAT_DCT_BURSTLENGTH,
      IDS_BSP_ONLY,
      IDS_BURST_LENGTH32,
      AMD_FAMILY_10,
      IdsSubBurstLength32
    };

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatDctPowerDownCtrlBlock =
    {
      IDS_FEAT_DCT_POWERDOWN,
      IDS_BSP_ONLY,
      IDS_INIT_POST_BEFORE,
      IDS_FAMILY_ALL,
      IdsSubPowerDownCtrl
    };

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatDctDllShutDownBlock =
    {
      IDS_FEAT_DCT_DLLSHUTDOWN,
      IDS_BSP_ONLY,
      IDS_DLL_SHUT_DOWN,
      IDS_FAMILY_ALL,
      IdsSubDllShutDownSR
    };


    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatDctPowerDownModeBlock =
    {
      IDS_FEAT_DCT_POWERDOWN,
      IDS_BSP_ONLY,
      IDS_POWERDOWN_MODE,
      IDS_FAMILY_ALL,
      IdsSubPowerDownMode
    };

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatHdtOutBlock =
    {
      IDS_FEAT_HDTOUT,
      IDS_BSP_ONLY,
      IDS_INIT_EARLY_BEFORE,
      IDS_FAMILY_ALL,
      IdsSubHdtOut
    };

    CONST IDS_FEAT_STRUCT ROMDATA IdsFeatHtSettingBlock =
    {
      IDS_FEAT_HT_SETTING,
      IDS_BSP_ONLY,
      IDS_HT_CONTROL,
      IDS_FAMILY_ALL,
      IdsSubHtLinkControl
    };

    CONST IDS_FEAT_STRUCT* ROMDATA IdsCommonFeats[] =
    {
      &IdsFeatUcodeBlock,
      &IdsFeatPowerPolicyBlock,

      &IdsFeatTargetPstateBlock,

      &IdsFeatPostPstateBlock,

      OPTION_IDS_FEAT_ECCSYMBOLSIZE

      OPTION_IDS_FEAT_ECCCTRL

      &IdsFeatDctAllMemClkBlock,

      &IdsFeatDctGangModeBlock,

      &IdsFeatDctBurstLengthBlock,

      &IdsFeatDctPowerDownCtrlBlock,

      &IdsFeatDctPowerDownModeBlock,

      &IdsFeatDctPowerDownModeBlock,

      OPTION_IDS_FEAT_HT_ASSIST

      &IdsFeatHdtOutBlock,

      &IdsFeatHtSettingBlock,

      OPTION_IDS_FEAT_GNB_PLATFORMCFG

      OPTION_IDS_FEAT_CPB_CTRL

      OPTION_IDS_FEAT_HTC_CTRL

      OPTION_IDS_FEAT_MEMORY_MAPPING

      OPTION_IDS_EXTEND_FEATS

      NULL
    };
  #else
    CONST IDS_FEAT_STRUCT* ROMDATA IdsCommonFeats[] =
    {
      NULL
    };
  #endif//IDSOPT_CONTROL_ENABLED
#else
  CONST IDS_FEAT_STRUCT* ROMDATA IdsCommonFeats[] =
  {
    NULL
  };
#endif// IDSOPT_IDS_ENABLED


#endif
