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
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 */
/*****************************************************************************
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
 *
 ***************************************************************************/
#ifndef _OPTION_IDS_INSTALL_H_
#define _OPTION_IDS_INSTALL_H_
#include "Ids.h"
#include "IdsHt.h"
#include "IdsLib.h"
#include "IdsDebugPrint.h"
#ifdef __IDS_EXTENDED__
  #include OPTION_IDS_EXT_INSTALL_FILE
#endif

#define IDS_LATE_RUN_AP_TASK

#define M_HTIDS_PORT_OVERRIDE_HOOK (PF_HtIdsGetPortOverride)CommonVoid
#if (IDSOPT_IDS_ENABLED == TRUE)
  #if (IDSOPT_CONTROL_ENABLED == TRUE)
    // Check for all families which include HT Features.To add new family support replace FALSE
    #if (FALSE) && (AGESA_ENTRY_INIT_POST == TRUE)
      #undef M_HTIDS_PORT_OVERRIDE_HOOK
      #define M_HTIDS_PORT_OVERRIDE_HOOK HtIdsGetPortOverride
    #endif
  #endif
#endif // OPTION_IDS_LEVEL
CONST PF_HtIdsGetPortOverride ROMDATA pf_HtIdsGetPortOverride = M_HTIDS_PORT_OVERRIDE_HOOK;

#if (IDSOPT_IDS_ENABLED == TRUE)
  #if (AGESA_ENTRY_INIT_LATE == TRUE)
    #undef IDS_LATE_RUN_AP_TASK
    #define IDS_LATE_RUN_AP_TASK  {IDS_LATE_RUN_AP_TASK_ID, (IMAGE_ENTRY)AmdIdsRunApTaskLate},
  #endif
#endif // OPTION_IDS_LEVEL

#if (IDSOPT_TRACING_ENABLED == TRUE)
  #if (AGESA_ENTRY_INIT_POST == TRUE)
    #include <mu.h>
    CONST SCRIPT_FUNCTION ROMDATA ScriptFuncList[] = {
      { (UINT32) MemUWriteCachelines, "WriteCl(PhyAddrLo,BufferAddr,ClCnt)"},
      { (UINT32) MemUReadCachelines,  "ReadCl(BufferAddr,PhyAddrLo,ClCnt)"},
      { (UINT32) MemUFlushPattern,    "FlushCl(PhyAddrLo,ClCnt)"}
    };
  #elif (AGESA_ENTRY_INIT_RECOVERY == TRUE)
    #include <mru.h>
    CONST SCRIPT_FUNCTION ROMDATA ScriptFuncList[] = {
      { (UINT32) (UINT64) MemRecUWrite1CL, "Write1Cl(PhyAddrLo,BufferAddr)"},
      { (UINT32) (UINT64) MemRecURead1CL,  "Read1Cl(BufferAddr,PhyAddrLo)"},
      { (UINT32) (UINT64) MemRecUFlushPattern,    "Flush1Cl(PhyAddrLo)"}
    };
  #else
    CONST SCRIPT_FUNCTION ROMDATA ScriptFuncList[] = {
      { (UINT32) (UINT64) CommonReturnFalse, "DefRet()"},
      { (UINT32) (UINT64) CommonReturnFalse,  "DefRet()"},
      { (UINT32) (UINT64) CommonReturnFalse,    "DefRet()"}
    };
  #endif
#else
  CONST SCRIPT_FUNCTION ROMDATA ScriptFuncList[] = {
    { (UINT32) CommonReturnFalse, "DefRet()"},
    { (UINT32) CommonReturnFalse,  "DefRet()"},
    { (UINT32) CommonReturnFalse,    "DefRet()"}
  };
#endif


#define NV_TO_CMOS(Len, NV_ID) {Len, NV_ID},
#define OPTION_IDS_NV_TO_CMOS_END NV_TO_CMOS (IDS_NV_TO_CMOS_LEN_END, IDS_NV_TO_CMOS_ID_END)
#if (IDSOPT_IDS_ENABLED == TRUE)
  #if ((IDSOPT_CONTROL_ENABLED == TRUE) && \
       ((AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_ENV == TRUE) || \
        (AGESA_ENTRY_INIT_MID == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || \
        (AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE)))
    #if (IDSOPT_CONTROL_NV_TO_CMOS == TRUE)
      #define OPTION_IDS_NV_TO_CMOS_COMMON

      //Family 15h TN
      #ifdef OPTION_FAMILY15H_TN
        #if OPTION_FAMILY15H_TN == TRUE
          #define OPTION_IDS_NV_TO_CMOS_F15_TN\
            {IDS_NV_TO_CMOS_LEN_BYTE, AGESA_IDS_NV_UCODE},
        #endif
      #endif

      #ifndef OPTION_IDS_NV_TO_CMOS_F15_TN
        #define OPTION_IDS_NV_TO_CMOS_F15_TN
      #endif


      //Family 16h KB
      #ifdef OPTION_FAMILY16H_KB
        #if OPTION_FAMILY16H_KB == TRUE
          #define OPTION_IDS_NV_TO_CMOS_F16_KB\
            {IDS_NV_TO_CMOS_LEN_BYTE, AGESA_IDS_NV_UCODE},
        #endif
      #endif

      #ifndef OPTION_IDS_NV_TO_CMOS_F16_KB
        #define OPTION_IDS_NV_TO_CMOS_F16_KB
      #endif

      #ifndef OPTION_IDS_NV_TO_CMOS_EXTEND
        #define OPTION_IDS_NV_TO_CMOS_EXTEND
      #endif

      IDS_NV_TO_CMOS gIdsNVToCmos[] = {
        OPTION_IDS_NV_TO_CMOS_COMMON
        OPTION_IDS_NV_TO_CMOS_F15_TN
        OPTION_IDS_NV_TO_CMOS_F16_KB
        OPTION_IDS_NV_TO_CMOS_EXTEND
        OPTION_IDS_NV_TO_CMOS_END
      };
    #else
      IDS_NV_TO_CMOS gIdsNVToCmos[] = {
        OPTION_IDS_NV_TO_CMOS_END
      };
    #endif
  #else
    IDS_NV_TO_CMOS gIdsNVToCmos[] = {
      OPTION_IDS_NV_TO_CMOS_END
    };
  #endif
#else
  IDS_NV_TO_CMOS gIdsNVToCmos[] = {
    OPTION_IDS_NV_TO_CMOS_END
  };
#endif

///Ids Feat Options
#if ((IDSOPT_IDS_ENABLED == TRUE) && \
     ((AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_ENV == TRUE) || \
      (AGESA_ENTRY_INIT_MID == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || \
      (AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE)))
  #if (IDSOPT_CONTROL_ENABLED == TRUE)
    #ifndef OPTION_IDS_EXTEND_FEATS
      #define OPTION_IDS_EXTEND_FEATS
    #endif

    #define OPTION_IDS_FEAT_ECCCTRL

    #define OPTION_IDS_FEAT_GNB_PLATFORMCFG\
                OPTION_IDS_FEAT_GNB_PLATFORMCFGF15TN \
                OPTION_IDS_FEAT_GNB_PLATFORMCFGF16KB


    #define OPTION_IDS_FEAT_CPB_CTRL

    #define OPTION_IDS_FEAT_HTC_CTRL\
                OPTION_IDS_FEAT_HTC_CTRL_F15_TN \
                OPTION_IDS_FEAT_HTC_CTRL_F16_KB


    #define OPTION_IDS_FEAT_MEMORY_MAPPING\
                OPTION_IDS_FEAT_MEMORY_MAPPING_F15_TN \
                OPTION_IDS_FEAT_MEMORY_MAPPING_F16_KB


    #define OPTION_IDS_FEAT_HT_ASSIST

/*----------------------------------------------------------------------------
 *                        Family 15 TN feat blocks
 *
 *----------------------------------------------------------------------------
 */
    #define OPTION_IDS_FEAT_HTC_CTRL_F15_TN
    #define OPTION_IDS_FEAT_MEMORY_MAPPING_F15_TN
    #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF15TN
    #ifdef OPTION_FAMILY15H_TN
      #if OPTION_FAMILY15H_TN == TRUE
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHtcControlBlockF15Tn;
        #undef OPTION_IDS_FEAT_HTC_CTRL_F15_TN
        #define OPTION_IDS_FEAT_HTC_CTRL_F15_TN\
                    &IdsFeatHtcControlBlockF15Tn,

        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryMappingPostBeforeBlockF15Tn;
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryMappingChIntlvBlockF15Tn;
        #undef OPTION_IDS_FEAT_MEMORY_MAPPING_F15_TN
        #define OPTION_IDS_FEAT_MEMORY_MAPPING_F15_TN\
        &IdsFeatMemoryMappingPostBeforeBlockF15Tn,\
        &IdsFeatMemoryMappingChIntlvBlockF15Tn,

        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatGnbPlatformCfgBlockF15Tn;
        #undef OPTION_IDS_FEAT_GNB_PLATFORMCFGF15TN
        #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF15TN &IdsFeatGnbPlatformCfgBlockF15Tn,
      #endif
    #endif


/*----------------------------------------------------------------------------
 *                        Family 16 KB feat blocks
 *
 *----------------------------------------------------------------------------
 */
    #define OPTION_IDS_FEAT_HTC_CTRL_F16_KB
    #define OPTION_IDS_FEAT_MEMORY_MAPPING_F16_KB
    #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF16KB
    #ifdef OPTION_FAMILY16H_KB
      #if OPTION_FAMILY16H_KB == TRUE
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHtcControlBlockF16Kb;
        #undef OPTION_IDS_FEAT_HTC_CTRL_F16_KB
        #define OPTION_IDS_FEAT_HTC_CTRL_F16_KB\
                    &IdsFeatHtcControlBlockF16Kb,

        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryMappingPostBeforeBlockF16Kb;
        #undef OPTION_IDS_FEAT_MEMORY_MAPPING_F16_KB
        #define OPTION_IDS_FEAT_MEMORY_MAPPING_F16_KB\
        &IdsFeatMemoryMappingPostBeforeBlockF16Kb,

        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatGnbPlatformCfgBlockF16Kb;
        #undef OPTION_IDS_FEAT_GNB_PLATFORMCFGF16KB
        #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF16KB &IdsFeatGnbPlatformCfgBlockF16Kb,
      #endif
    #endif

    #define OPTION_IDS_FEAT_NV_TO_CMOS
    #if IDSOPT_CONTROL_NV_TO_CMOS == TRUE
      #undef OPTION_IDS_FEAT_NV_TO_CMOS
      extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatNvToCmosSaveBlock;
      extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatNvToCmosRestoreBlock;
      #define OPTION_IDS_FEAT_NV_TO_CMOS\
        &IdsFeatNvToCmosSaveBlock, \
        &IdsFeatNvToCmosRestoreBlock,

    #endif

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatUcodeBlock =
    {
      IDS_FEAT_UCODE_UPDATE,
      IDS_ALL_CORES,
      IDS_UCODE,
      IDS_FAMILY_ALL,
      IdsSubUCode
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatPowerPolicyBlock =
    {
      IDS_FEAT_POWER_POLICY,
      IDS_ALL_CORES,
      IDS_PLATFORMCFG_OVERRIDE,
      IDS_FAMILY_ALL,
      IdsSubPowerPolicyOverride
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatTargetPstateBlock =
    {
      IDS_FEAT_TARGET_PSTATE,
      IDS_BSP_ONLY,
      IDS_INIT_LATE_AFTER,
      IDS_FAMILY_ALL,
      IdsSubTargetPstate
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatPostPstateBlock =
    {
      IDS_FEAT_POSTPSTATE,
      IDS_ALL_CORES,
      IDS_CPU_Early_Override,
      IDS_FAMILY_ALL,
      IdsSubPostPState
    };

    //Dram controller Features
    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatDctAllMemClkBlock =
    {
      IDS_FEAT_DCT_ALLMEMCLK,
      IDS_BSP_ONLY,
      IDS_ALL_MEMORY_CLOCK,
      IDS_FAMILY_ALL,
      IdsSubAllMemClkEn
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatDctGangModeBlock =
    {
      IDS_FEAT_DCT_GANGMODE,
      IDS_BSP_ONLY,
      IDS_GANGING_MODE,
      IDS_FAMILY_ALL,
      IdsSubGangingMode
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatDctPowerDownCtrlBlock =
    {
      IDS_FEAT_DCT_POWERDOWN,
      IDS_BSP_ONLY,
      IDS_INIT_POST_BEFORE,
      IDS_FAMILY_ALL,
      IdsSubPowerDownCtrl
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatDctPowerDownModeBlock =
    {
      IDS_FEAT_DCT_POWERDOWN,
      IDS_BSP_ONLY,
      IDS_POWERDOWN_MODE,
      IDS_FAMILY_ALL,
      IdsSubPowerDownMode
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHdtOutBlock =
    {
      IDS_FEAT_HDTOUT,
      IDS_BSP_ONLY,
      IDS_INIT_EARLY_BEFORE,
      IDS_FAMILY_ALL,
      IdsSubHdtOut
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHtSettingBlock =
    {
      IDS_FEAT_HT_SETTING,
      IDS_BSP_ONLY,
      IDS_HT_CONTROL,
      IDS_FAMILY_ALL,
      IdsSubHtLinkControl
    };

    CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsControlFeats[] =
    {
      &IdsFeatUcodeBlock,
      &IdsFeatPowerPolicyBlock,

      &IdsFeatTargetPstateBlock,

      &IdsFeatPostPstateBlock,

      OPTION_IDS_FEAT_NV_TO_CMOS

      OPTION_IDS_FEAT_ECCCTRL

      &IdsFeatDctAllMemClkBlock,

      &IdsFeatDctGangModeBlock,

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
    CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsControlFeats[] =
    {
      NULL
    };
  #endif//IDSOPT_CONTROL_ENABLED

  #define OPTION_IDS_FAM_REGACC_F15TN
  #ifdef OPTION_FAMILY15H_TN
    #if OPTION_FAMILY15H_TN == TRUE
      extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatRegGmmxF15Tn;
      #undef OPTION_IDS_FAM_REGACC_F15TN
      #define OPTION_IDS_FAM_REGACC_F15TN \
                &IdsFeatRegGmmxF15Tn,
    #endif
  #endif


  #define OPTION_IDS_FAM_REGACC_F16KB
  #ifdef OPTION_FAMILY16H_KB
    #if OPTION_FAMILY16H_KB == TRUE
      extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatRegGmmxF16Kb;
      #undef OPTION_IDS_FAM_REGACC_F16KB
      #define OPTION_IDS_FAM_REGACC_F16KB \
                &IdsFeatRegGmmxF16Kb,
    #endif
  #endif

  CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsRegAccessTbl[] =
  {
    OPTION_IDS_FAM_REGACC_F15TN
    OPTION_IDS_FAM_REGACC_F16KB
    NULL
  };

/*----------------------------------------------------------------------------
 *                        IDS TRACING SERVICES
 *
 *----------------------------------------------------------------------------
 */
  #if IDSOPT_TRACING_ENABLED == TRUE
    #define IDS_TRACING_CONSOLE_HDTOUT
    #define IDS_TRACING_CONSOLE_SERIALPORT
    #define IDS_TRACING_CONSOLE_REDIRECT_IO
    #define IDS_TRACING_CONSOLE_RAM

    #ifdef IDSOPT_TRACING_CONSOLE_HDTOUT
      #if IDSOPT_TRACING_CONSOLE_HDTOUT == TRUE
        #undef IDS_TRACING_CONSOLE_HDTOUT
        extern CONST IDS_DEBUG_PRINT ROMDATA IdsDebugPrintHdtoutInstance;
        #define IDS_TRACING_CONSOLE_HDTOUT &IdsDebugPrintHdtoutInstance,
      #endif
    #endif

    #ifdef IDSOPT_TRACING_CONSOLE_SERIALPORT
      #if IDSOPT_TRACING_CONSOLE_SERIALPORT == TRUE
        #undef IDS_TRACING_CONSOLE_SERIALPORT
        extern CONST IDS_DEBUG_PRINT ROMDATA IdsDebugPrintSerialInstance;
        #define IDS_TRACING_CONSOLE_SERIALPORT &IdsDebugPrintSerialInstance,
      #endif
    #endif

    #ifdef IDSOPT_TRACING_CONSOLE_REDIRECT_IO
      #if IDSOPT_TRACING_CONSOLE_REDIRECT_IO == TRUE
        #undef IDS_TRACING_CONSOLE_REDIRECT_IO
        extern CONST IDS_DEBUG_PRINT ROMDATA IdsDebugPrintRedirectIoInstance;
        #define IDS_TRACING_CONSOLE_REDIRECT_IO &IdsDebugPrintRedirectIoInstance,
      #endif
    #endif

    #ifdef IDSOPT_TRACING_CONSOLE_RAM
      #if IDSOPT_TRACING_CONSOLE_RAM == TRUE
        #undef IDS_TRACING_CONSOLE_RAM
        extern CONST IDS_DEBUG_PRINT ROMDATA IdsDebugPrintRamInstance;
        #define IDS_TRACING_CONSOLE_RAM &IdsDebugPrintRamInstance,
      #endif
    #endif


    CONST IDS_DEBUG_PRINT* ROMDATA IdsDebugPrint[] =
    {
      IDS_TRACING_CONSOLE_SERIALPORT
      IDS_TRACING_CONSOLE_HDTOUT
      IDS_TRACING_CONSOLE_REDIRECT_IO
      IDS_TRACING_CONSOLE_RAM
      NULL
    };
  #else
    CONST IDS_DEBUG_PRINT* ROMDATA IdsDebugPrint[] =
    {
      NULL
    };
  #endif

#else
  CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsControlFeats[] =
  {
    NULL
  };

  CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsRegAccessTbl[] =
  {
    NULL
  };

  CONST IDS_DEBUG_PRINT* ROMDATA IdsDebugPrint[] =
  {
    NULL
  };
#endif// IDSOPT_IDS_ENABLED

#endif
