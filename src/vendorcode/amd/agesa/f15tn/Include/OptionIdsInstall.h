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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
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
    // Check for all families which include HT Features.
    #if ((OPTION_FAMILY10H == TRUE) || (OPTION_FAMILY15H_OR == TRUE) || ) && (AGESA_ENTRY_INIT_POST == TRUE)
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
      { (UINT32) /*(UINT64)*/ MemUWriteCachelines, "WriteCl(PhyAddrLo,BufferAddr,ClCnt)"},
      { (UINT32) /*(UINT64)*/ MemUReadCachelines,  "ReadCl(BufferAddr,PhyAddrLo,ClCnt)"},
      { (UINT32) /*(UINT64)*/ MemUFlushPattern,    "FlushCl(PhyAddrLo,ClCnt)"}
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
    { (UINT32) /*(UINT64)*/ CommonReturnFalse, "DefRet()"},
    { (UINT32) /*(UINT64)*/ CommonReturnFalse,  "DefRet()"},
    { (UINT32) /*(UINT64)*/ CommonReturnFalse,    "DefRet()"}
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

      #ifdef OPTION_FAMILY10H
        #if OPTION_FAMILY10H == TRUE
          #define OPTION_IDS_NV_TO_CMOS_F10
        #endif
      #endif

      #ifdef OPTION_FAMILY12H
        #if OPTION_FAMILY12H == TRUE
          #define OPTION_IDS_NV_TO_CMOS_F12
        #endif
      #endif

      #ifdef OPTION_FAMILY14H
        #if OPTION_FAMILY14H == TRUE
          #define OPTION_IDS_NV_TO_CMOS_F14
        #endif
      #endif

      #ifdef OPTION_FAMILY15H_OR
        #if OPTION_FAMILY15H_OR == TRUE
          #define OPTION_IDS_NV_TO_CMOS_F15_OR
        #endif
      #endif

      #ifdef OPTION_FAMILY15H_TN
        #if OPTION_FAMILY15H_TN == TRUE
          #define OPTION_IDS_NV_TO_CMOS_F15_TN\
            {IDS_NV_TO_CMOS_LEN_BYTE, AGESA_IDS_NV_UCODE},
        #endif
      #endif

      #ifndef OPTION_IDS_NV_TO_CMOS_F10
        #define OPTION_IDS_NV_TO_CMOS_F10
      #endif

      #ifndef OPTION_IDS_NV_TO_CMOS_F12
        #define OPTION_IDS_NV_TO_CMOS_F12
      #endif

      #ifndef OPTION_IDS_NV_TO_CMOS_F14
        #define OPTION_IDS_NV_TO_CMOS_F14
      #endif

      #ifndef OPTION_IDS_NV_TO_CMOS_F15_OR
        #define OPTION_IDS_NV_TO_CMOS_F15_OR
      #endif

      #ifndef OPTION_IDS_NV_TO_CMOS_F15_TN
        #define OPTION_IDS_NV_TO_CMOS_F15_TN
      #endif

      #ifndef OPTION_IDS_NV_TO_CMOS_EXTEND
        #define OPTION_IDS_NV_TO_CMOS_EXTEND
      #endif

      IDS_NV_TO_CMOS gIdsNVToCmos[] = {
        OPTION_IDS_NV_TO_CMOS_COMMON
        OPTION_IDS_NV_TO_CMOS_F10
        OPTION_IDS_NV_TO_CMOS_F12
        OPTION_IDS_NV_TO_CMOS_F14
        OPTION_IDS_NV_TO_CMOS_F15_OR
        OPTION_IDS_NV_TO_CMOS_F15_TN
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

    #define OPTION_IDS_FEAT_ECCCTRL\
                OPTION_IDS_FEAT_ECCCTRL_F10 \
                OPTION_IDS_FEAT_ECCCTRL_F12 \
                OPTION_IDS_FEAT_ECCCTRL_F15_OR

    #define OPTION_IDS_FEAT_GNB_PLATFORMCFG\
                OPTION_IDS_FEAT_GNB_PLATFORMCFGF12 \
                OPTION_IDS_FEAT_GNB_PLATFORMCFGF14 \
                OPTION_IDS_FEAT_GNB_PLATFORMCFGF15TN

    #define OPTION_IDS_FEAT_CPB_CTRL\
                OPTION_IDS_FEAT_CPB_CTRL_F12

    #define OPTION_IDS_FEAT_HTC_CTRL\
                OPTION_IDS_FEAT_HTC_CTRL_F15_OR \
                OPTION_IDS_FEAT_HTC_CTRL_F15_TN

    #define OPTION_IDS_FEAT_MEMORY_MAPPING\
                OPTION_IDS_FEAT_MEMORY_MAPPING_F12 \
                OPTION_IDS_FEAT_MEMORY_MAPPING_F15_OR \
                OPTION_IDS_FEAT_MEMORY_MAPPING_F15_TN

    #define OPTION_IDS_FEAT_HT_ASSIST\
                OPTION_IDS_FEAT_HT_ASSIST_F10HY \
                OPTION_IDS_FEAT_HT_ASSIST_F15_OR

    #define OPTION_IDS_FEAT_ECCSYMBOLSIZE\
                OPTION_IDS_FEAT_ECCSYMBOLSIZE_F10 \
                OPTION_IDS_FEAT_ECCSYMBOLSIZE_F15_OR

/*----------------------------------------------------------------------------
 *                        Family 10 feat blocks
 *
 *----------------------------------------------------------------------------
 */
    #define OPTION_IDS_FEAT_ECCSYMBOLSIZE_F10
    #define OPTION_IDS_FEAT_ECCCTRL_F10
    #ifdef OPTION_FAMILY10H
      #if OPTION_FAMILY10H == TRUE
//Ecc symbol size
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatEccSymbolSizeBlockF10;
        #undef OPTION_IDS_FEAT_ECCSYMBOLSIZE_F10
        #define OPTION_IDS_FEAT_ECCSYMBOLSIZE_F10 &IdsFeatEccSymbolSizeBlockF10,

//ECC scrub control
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatEccCtrlBlockF10;
        #undef OPTION_IDS_FEAT_ECCCTRL_F10
        #define OPTION_IDS_FEAT_ECCCTRL_F10 &IdsFeatEccCtrlBlockF10,
      #endif
    #endif

    //Misc Features
    #define OPTION_IDS_FEAT_HT_ASSIST_F10HY
    #ifdef OPTION_FAMILY10H_HY
      #if OPTION_FAMILY10H_HY == TRUE
        #undef OPTION_IDS_FEAT_HT_ASSIST_F10HY
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHtAssistBlockPlatformCfgF10Hy;

        #define OPTION_IDS_FEAT_HT_ASSIST_F10HY \
                    &IdsFeatHtAssistBlockPlatformCfgF10Hy,
      #endif
    #endif
/*----------------------------------------------------------------------------
 *                        Family 12 feat blocks
 *
 *----------------------------------------------------------------------------
 */
    #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF12
    #define OPTION_IDS_FEAT_ECCCTRL_F12
    #define OPTION_IDS_FEAT_CPB_CTRL_F12
    #define OPTION_IDS_FEAT_MEMORY_MAPPING_F12
    #ifdef OPTION_FAMILY12H
      #if OPTION_FAMILY12H == TRUE
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatGnbPlatformCfgBlockF12;
        #undef OPTION_IDS_FEAT_GNB_PLATFORMCFGF12
        #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF12 &IdsFeatGnbPlatformCfgBlockF12,

        //ECC scrub control
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatEccCtrlBlockF12;
        #undef OPTION_IDS_FEAT_ECCCTRL_F12
        #define OPTION_IDS_FEAT_ECCCTRL_F12 &IdsFeatEccCtrlBlockF12,

        #undef OPTION_IDS_FEAT_CPB_CTRL_F12
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatCpbCtrlBlockF12;
        #define OPTION_IDS_FEAT_CPB_CTRL_F12 &IdsFeatCpbCtrlBlockF12,

        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryChIntlvPostBeforeBlockF12;
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryMappingChIntlvBlockF12;
        #undef OPTION_IDS_FEAT_MEMORY_MAPPING_F12
        #define OPTION_IDS_FEAT_MEMORY_MAPPING_F12 \
                    &IdsFeatMemoryChIntlvPostBeforeBlockF12, \
                    &IdsFeatMemoryMappingChIntlvBlockF12,

      #endif
    #endif

/*----------------------------------------------------------------------------
 *                        Family 14 ON feat blocks
 *
 *----------------------------------------------------------------------------
 */
    #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF14
    #ifdef OPTION_FAMILY14H_ON
      #if OPTION_FAMILY14H_ON == TRUE
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatGnbPlatformCfgBlockF14;
        #undef OPTION_IDS_FEAT_GNB_PLATFORMCFGF14
        #define OPTION_IDS_FEAT_GNB_PLATFORMCFGF14 &IdsFeatGnbPlatformCfgBlockF14,
      #endif
    #endif


/*----------------------------------------------------------------------------
 *                        Family 15 OR feat blocks
 *
 *----------------------------------------------------------------------------
 */
    #define OPTION_IDS_FEAT_HTC_CTRL_F15_OR
    #define OPTION_IDS_FEAT_MEMORY_MAPPING_F15_OR
    #define OPTION_IDS_FEAT_HT_ASSIST_F15_OR
    #define OPTION_IDS_FEAT_ECCCTRL_F15_OR
    #define OPTION_IDS_FEAT_ECCSYMBOLSIZE_F15_OR
    #ifdef OPTION_FAMILY15H_OR
      #if OPTION_FAMILY15H_OR == TRUE
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHtcControlBlockF15Or;
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHtcControlLateBlockF15Or;
        #undef OPTION_IDS_FEAT_HTC_CTRL_F15_OR
        #define OPTION_IDS_FEAT_HTC_CTRL_F15_OR\
                    &IdsFeatHtcControlBlockF15Or,\
                    &IdsFeatHtcControlLateBlockF15Or,

        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryMappingPostBeforeBlockF15Or;
        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryMappingChIntlvBlockF15Or;
        #undef OPTION_IDS_FEAT_MEMORY_MAPPING_F15_OR
        #define OPTION_IDS_FEAT_MEMORY_MAPPING_F15_OR\
        &IdsFeatMemoryMappingPostBeforeBlockF15Or,\
        &IdsFeatMemoryMappingChIntlvBlockF15Or,

        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHtAssistBlockPlatformCfgF15Or;
        #undef OPTION_IDS_FEAT_HT_ASSIST_F15_OR
        #define OPTION_IDS_FEAT_HT_ASSIST_F15_OR\
        &IdsFeatHtAssistBlockPlatformCfgF15Or,

        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatEccCtrlBlockF15Or;
        #undef OPTION_IDS_FEAT_ECCCTRL_F15_OR
        #define OPTION_IDS_FEAT_ECCCTRL_F15_OR &IdsFeatEccCtrlBlockF15Or,

        extern CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatEccSymbolSizeBlockF15Or;
        #undef OPTION_IDS_FEAT_ECCSYMBOLSIZE_F15_OR
        #define OPTION_IDS_FEAT_ECCSYMBOLSIZE_F15_OR &IdsFeatEccSymbolSizeBlockF15Or,

      #endif
    #endif
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

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatDctBurstLengthBlock =
    {
      IDS_FEAT_DCT_BURSTLENGTH,
      IDS_BSP_ONLY,
      IDS_BURST_LENGTH32,
      AMD_FAMILY_10,
      IdsSubBurstLength32
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatDctPowerDownCtrlBlock =
    {
      IDS_FEAT_DCT_POWERDOWN,
      IDS_BSP_ONLY,
      IDS_INIT_POST_BEFORE,
      IDS_FAMILY_ALL,
      IdsSubPowerDownCtrl
    };

    CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatDctDllShutDownBlock =
    {
      IDS_FEAT_DCT_DLLSHUTDOWN,
      IDS_BSP_ONLY,
      IDS_DLL_SHUT_DOWN,
      IDS_FAMILY_ALL,
      IdsSubDllShutDownSR
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

    CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsContorlFeats[] =
    {
      &IdsFeatUcodeBlock,
      &IdsFeatPowerPolicyBlock,

      &IdsFeatTargetPstateBlock,

      &IdsFeatPostPstateBlock,

      OPTION_IDS_FEAT_NV_TO_CMOS

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
    CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsContorlFeats[] =
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


  CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsRegAccessTbl[] =
  {
    OPTION_IDS_FAM_REGACC_F15TN
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

    CONST IDS_DEBUG_PRINT* ROMDATA IdsDebugPrint[] =
    {
      IDS_TRACING_CONSOLE_SERIALPORT
      IDS_TRACING_CONSOLE_HDTOUT
      IDS_TRACING_CONSOLE_REDIRECT_IO
      NULL
    };
  #else
    CONST IDS_DEBUG_PRINT* ROMDATA IdsDebugPrint[] =
    {
      NULL
    };
  #endif

#else
  CONST IDS_FAMILY_FEAT_STRUCT* ROMDATA IdsContorlFeats[] =
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
