/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD IDS Routines
 *
 * Contains AMD AGESA Integrated Debug Macros
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision$   @e \$Date$
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2016, Advanced Micro Devices, Inc.
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

#include <check_for_wrapper.h>

  /* Macros to aid debugging */
  /* These definitions expand to zero (0) bytes of code when disabled */

#ifndef _IDS_H_
#define _IDS_H_

#undef FALSE
#undef TRUE
#define FALSE 0
#define TRUE  1
// Proto type for optionsids.h
typedef UINT32 IDS_STATUS;  ///< Status of IDS function.
#define IDS_SUCCESS         ((IDS_STATUS) 0x00000000ul) ///< IDS Function is Successful.
#define IDS_UNSUPPORTED     ((IDS_STATUS) 0xFFFFFFFFul) ///< IDS Function is not existed.

#define IDS_STRINGIZE(a) #a ///< for define stringize macro
#ifndef IDS_DEADLOOP
  #define IDS_DEADLOOP()    { volatile UINTN __i; __i = 1; while (__i); }
#endif
/**
 *  IDS Option Hook Points
 *
 *  These are the values to indicate hook point in AGESA for IDS Options.
 *
 */
typedef enum {                        //vv- for debug reference only
  IDS_INIT_EARLY_BEFORE,              ///< 00 Option Hook Point before AGESA function AMD_INIT_EARLY.
                                          ///<   IDS Object is initialized.
                                          ///<   Override CPU Core Leveling Mode.
                                          ///<   Set P-State in Post
  IDS_INIT_EARLY_AFTER,               ///< 01 Option Hook Point after AGESA function AMD_INIT_EARLY.
  IDS_INIT_LATE_BEFORE,               ///< 02 Option Hook Point before AGESA function AMD_INIT_LATE.
                                          ///< It will be used to control the following tables.
                                          ///<   ACPI P-State Table (_PSS, XPSS, _PCT, _PSD, _PPC)
                                          ///<   ACPI WHEA Table
                                          ///<   DMI Table
  IDS_INIT_LATE_AFTER,                ///< 03 Option Hook Point after AGESA function AMD_INIT_LATE.
  IDS_INIT_MID_BEFORE,                ///< 04 Option Hook Point before AGESA function AMD_INIT_MID.
  IDS_INIT_MID_AFTER,                 ///< 05 Option Hook Point after AGESA function AMD_INIT_MID.
  IDS_INIT_POST_BEFORE,               ///< 06 Option Hook Point before AGESA function AMD_INIT_POST.
                                          ///<   Control Interleaving and DRAM memory hole
                                          ///<   Override the setting of ECC Control
                                          ///<   Override the setting of Online Spare Rank
  IDS_INIT_POST_AFTER,                ///< 07 Option Hook Point after AGESA function AMD_INIT_POST.
  IDS_INIT_RESET_BEFORE,              ///< 08 Option Hook Point before AGESA function AMD_INIT_RESET.
  IDS_INIT_RESET_AFTER,               ///< 09 Option Hook Point after AGESA function AMD_INIT_RESET.
  IDS_INIT_POST_MID,                  ///< 0a Option Hook Point after AGESA function AMD_INIT_POST.
  IDS_BEFORE_S3_SAVE,                 ///< 0b override any settings before S3 save.
  IDS_BEFORE_S3_RESTORE,              ///< 0c  override any settings before S3 restore
  IDS_AFTER_S3_SAVE,                  ///< 0d Override any settings after S3 save
  IDS_AFTER_S3_RESTORE,               ///< 0e Override any settings after S3 restore
  IDS_BEFORE_DQS_TRAINING,            ///< 0f override any settings before DQS training
  IDS_BEFORE_DRAM_INIT,               ///< 10 override any settings before Dram initialization
  IDS_BEFORE_MEM_FREQ_CHG,            ///< 11 override settings before MemClk frequency change
  IDS_BEFORE_WARM_RESET ,             ///< 12 Override PCI or MSR Registers Before Warm Reset
  IDS_BEFORE_PCI_INIT,                ///< 13 Override PCI or MSR Registers Before PCI Init
  IDS_BEFORE_AP_EARLY_HALT,           ///< 14 Option Hook Point before AP early halt
  IDS_BEFORE_S3_RESUME,               ///< 15 Option Hook Point before s3 resume
  IDS_AFTER_S3_RESUME,                ///< 16 Option Hook Point after s3 resume
  IDS_BEFORE_PM_INIT,                 ///< 17 Option Hook Point Before Pm Init

  IDS_INIT_RTB_BEFORE,                ///< 18 Option Hook Point before AGESA function AMD_INIT_RTB.
  IDS_INIT_RTB_AFTER,                 ///< 19 Option Hook Point after AGESA function AMD_INIT_RTB.

  IDS_MT_BASE = 0x20,                 ///< 0x20 ~ 0x38 24 time points reserved for MTTime

  IDS_PLATFORM_RSVD1 = 0x38,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD2 = 0x39,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD3 = 0x3a,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD4 = 0x3b,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD5 = 0x3c,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD6 = 0x3d,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD7 = 0x3e,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD8 = 0x3f,          ///< from 0x38 to 0x3f will reserved for platform used

  // All the above timing point is used by BVM, their value should never be changed
  IDS_INIT_DRAM_TABLE,                ///< 40 Generate override table for Dram Timing
                                          ///< Dram Controller, Drive Strength and DQS Timing
  IDS_GET_DRAM_TABLE,                 ///< 41 Generate override table for Dram Timing
  IDS_GANGING_MODE,                   ///< 42 override Memory Mode Unganged
  IDS_POWERDOWN_MODE,                 ///< 43 override Power Down Mode
  IDS_BURST_LENGTH32,                 ///< 44 override Burst Length32
  IDS_ALL_MEMORY_CLOCK,               ///< 45 override All Memory Clks Enable
  IDS_ECC,                            ///< 46 override ECC parameter
  IDS_ECCSYMBOLSIZE,                  ///< 47 override ECC symbol size
  IDS_CPU_Early_Override,             ///< 48 override CPU early parameter
  IDS_CACHE_FLUSH_HLT,                ///< 49 override Cache Flush Hlt
  IDS_CHANNEL_INTERLEAVE,             ///< 4a override Channel Interleave
  IDS_MEM_ERROR_RECOVERY,             ///< 4b override memory error recovery
  IDS_MEM_RETRAIN_TIMES,              ///< 4c override memory retrain times
  IDS_MEM_SIZE_OVERLAY,               ///< 4d Override the syslimit
  IDS_CHECK_NEGATIVE_WL,              ///< 4e Check for negative write leveling result
  IDS_DLL_SHUT_DOWN,                  ///< 4f Check for Dll Shut Down
  IDS_POR_MEM_FREQ,                   ///< 50 Entry to enable/disable MemClk frequency enforcement
  IDS_PHY_DLL_STANDBY_CTRL,           ///< 51 Enable/Disable Phy DLL standby feature
  IDS_PLATFORMCFG_OVERRIDE,           ///< 52 Hook for Override PlatformConfig structure
  IDS_LOADCARD_ERROR_RECOVERY,        ///< 53 Special error handling for load card support
  IDS_MEM_IGNORE_ERROR,               ///< 54 Ignore error and do not do fatal exit in memory
  IDS_GNB_SMU_SERVICE_CONFIG,         ///< 55 Config GNB SMU service
  IDS_GNB_ORBDYNAMIC_WAKE,            ///< 56 config GNB dynamic wake
  IDS_GNB_PLATFORMCFG_OVERRIDE,       ///< 57 override ids gnb platform config
  IDS_GNB_LCLK_DPM_EN,                ///< 58 override GNB LCLK DPM configuration
  IDS_GNB_LCLK_DEEP_SLEEP,            ///< 59 override GNB LCLK DPM deep sleep
  IDS_GNB_CLOCK_GATING,               ///< 5a Override GNB Clock gating config
  IDS_NB_PSTATE_DIDVID,               ///< 5b Override NB P-state settings
  IDS_CPB_CTRL,                       ///< 5c Config the Core peformance boost feature
  IDS_HTC_CTRL,                       ///< 5d Hook for Hardware Thermal Control
  IDS_CC6_WORKAROUND,                 ///< 5e Hook for skip CC6 work around
  IDS_MEM_MR0,                        ///< 5f Hook for override Memory Mr0 register
  IDS_TRAP_TABLE,                     ///< 60 Hook for add IDS register table to the loop
  IDS_NBBUFFERALLOCATIONATEARLY,      ///< 61 Hook for override North bridge bufer allocation
  IDS_BEFORE_S3_SPECIAL,              ///< 62 Hook to bypass S3 special functions
  IDS_SET_PCI_REGISTER_ENTRY,         ///< 63 Hook to SetRegisterForPciEntry
  IDS_ERRATUM463_WORKAROUND,          ///< 64 Hook to Erratum 463 workaround
  IDS_BEFORE_MEMCLR,                  ///< 65 Hook before set Memclr bit
  IDS_OVERRIDE_IO_CSTATE,             ///< 66 Hook for override io C-state setting
  IDS_NBPSDIS_OVERRIDE,               ///< 67 Hook for override NB pstate disable setting
  IDS_NBPS_REG_OVERRIDE,              ///< 68 Hook for override Memory NBps reg
  IDS_MEM_PHY_POWER_SAVING,           ///< 69 Hook to bypass DRAM Phy power savings
  IDS_CST_CREATE,                     ///< 6a Hook for create _CST
  IDS_CST_SIZE,                       ///< 6b Hook for get _CST size
  IDS_ENFORCE_VDDIO,                  ///< 6c Hook to override VDDIO
  IDS_STRETCH_FREQUENCY_LIMIT,        ///< 6d Hook for enforcing memory stretch frequency limit
  IDS_INIT_MEM_REG_TABLE,             ///< 6e Hook for init memory register table
  IDS_SKIP_FUSED_MAX_RATE,            ///< 6f Hook to skip fused max rate cap
  IDS_FCH_INIT_AT_RESET,              ///< 70 Hook for FCH reset parameter
  IDS_FCH_INIT_AT_ENV,                ///< 71 Hook for FCH ENV parameter
  IDS_ENFORCE_PLAT_TABLES,            ///< 72 Hook to enforce platform specific tables
  IDS_NBPS_MIN_FREQ,                  ///< 73 Hook for override MIN nb ps freq
  IDS_GNB_FORCE_CABLESAFE,            ///< 74 Hook for override Force Cable Safe
  IDS_SKIP_PM_TRANSITION_STEP,        ///< 75 Hook for provide IDS ability to skip this PM step
  IDS_GNB_PROPERTY,                   ///< 76 Hook for GNB Property
  IDS_GNB_PCIE_POWER_GATING,          ///< 77 Hook for GNB PCIe Power Gating
  IDS_MEM_DYN_DRAM_TERM,              ///< 78 Hook for Override Dynamic Dram Term
  IDS_MEM_DRAM_TERM,                  ///< 79 Hook for Override Dram Term
  IDS_GNB_ALTVDDNB,                   ///< 7a Hook for Override AltVddNB
  IDS_UCODE,                          ///< 7b Enable or Disable microcode patching
  IDS_FAM_REG_GMMX,                   ///< 7c GMMX register access
  IDS_MEMORY_POWER_POLICY,            ///< 7d Memory power policy
  IDS_GET_STRETCH_FREQUENCY_LIMIT,    ///< 7e Hook for enforcing memory stretch frequency limit
  IDS_CPU_FEAT,                       ///< 7f Hook for runtime force cpu feature disable
  IDS_AFTER_DCT_PHY_ACCESS,           ///< 80 Hook for DctAccessDone check
  IDS_FORCE_PHY_TO_M0,                ///< 81 Hook to bypass M0 enforcement
  IDS_GNB_PMM_SWTJOFFSET,             ///< 82 Hook to GNBSWTJOFFSET
  IDS_LOCK_DRAM_CFG,                  ///< 83 Hook to BFLockDramCfg
  IDS_BEFORE_GEN2_INIT,               ///< 84 Hook to Before Gen2 Init
  IDS_BEFORE_GPP_TRAINING,            ///< 85 Hook to Before Gpp training
  IDS_BEFORE_RECONFIGURATION,         ///< 86 Hook to Before Reconfiguration
  IDS_BEFORE_GEN3_INIT,               ///< 87 Hook to Before Gen3 Init
  IDS_BEFORE_POWER_GATING,            ///< 88 Hook to Before Power Gating
  IDS_AFTER_EARLY_INIT_ONCORE,        ///< 89 Hook to after EarlyInit On Core
  IDS_GNB_PCIE_PORT_REMAP,            ///< 8a Hook to change mapping of PCIe devices
  IDS_ECC_CONTROL,                    ///< 8b Enable/Disable ECC feature
  IDS_GNB_PCIE_PHY_ISOLATION,         ///< 8c Enable/Disable PCIE PHY ISOLATION
  IDS_AFTER_RESTORING_PCI_REG,        ///< 8d Hook after restoring PCI register during S3 resume
  IDS_MSR_ACCESS_OVERRIDE,            ///< 8e Hook to disable MSR access
  IDS_REPORT_SMU_FW_VERSION,          ///< 8f Hook to report SMU firmware version
  IDS_GNB_LOAD_SAMU_PATCH,            ///< 90 Hook to Load SAMU patch
  IDS_DLLSTAGGERDLY_OVERRIDE,         ///< 91 Hook to skip Dll Stagger Delay
  IDS_CSAMPLE_TIMER,                  ///< 92 Hook to override CSampleTimer
  IDS_PIPE_THROTTLE,                  ///< 93 Hook to override PipeThrottle
  IDS_GNB_PMM_NATIVEGEN1PLL,          ///< 94 Enable/Disable Native Gen1 PLL
  IDS_BEFORE_PLAT_TABLES,             ///< 95 Hook before processing platform specific tables
  IDS_GNB_PCIE_MASTERPLL_SELECTION,   ///< 96 Hook to override PCIe Master PLL selection
  IDS_GNB_PCIE_PHYLANE_CONFIG,        ///< 97 Hook to override PCIe PhyLane configuration
  IDS_DSM_LP_SELECTION,               ///< 98 Set Dsm low power mode
  IDS_GNB_EQUAL_PRESET,               ///< 99 Set equalization preset
  IDS_GNB_GEN1_LOOPBACK,              ///< 9a Override Gen 1 loopback mode
  IDS_GNB_SMU_SERVICE_MASK,           ///< 9b Override SMU service enablement mask
  IDS_BEFORE_HT_MEM_MAP,              ///< 9c Hook before mapping memory address space among DCTs
  IDS_GNB_GEN3_SETTINGS_OVERRIDE,     ///< 9d Hook to override Gen3 Adapt PI Offset Bypass Enable
  IDS_GNB_TARGET_TDP,                 ///< 9e Hook to override Target TDP
  IDS_GNB_SMU_PORT80_PARAMS,          ///< 9f Hook to override gnb smu port80 values
  IDS_CPU_OVERRIDE_REG_AFTER_AP_LAUNCH, ///< a0 Hook to override registers after AP lauch
  IDS_AFTER_FEAT_MID_INIT,            ///< a1 Hook to override feature setting after cpuMidInit
  IDS_MEM_MCLK_ABOVE_NCLK,            ///< a2 Hook to bypass system bandwidth for memory clock
  IDS_ALL_MEMORY_CKE,                 ///< a3 override All Memory CKE
  IDS_ALL_MEMORY_CS,                  ///< a4 override All Memory CS
  IDS_NPST,                           ///< a5 cTDP NbPstate Selection Table
  IDS_MEM_MRL_RETRAIN_TIMES,          ///< a6 override memory MRL retrain times
  IDS_AFTER_DQS_TRAINING,             ///< a7 override any settings after DQS training
  IDS_OVERRIDE_DIMM_MASK,             ///< a8 override DimmMask for S3 data blob creation
  IDS_BYPASS_S3_REGISTERS,            ///< a9 bypass restoring certain registers
  IDS_MEM_RTTNOM,                     ///< aa Hook for Override RttNom
  IDS_MEM_RTTWR,                      ///< ab Hook for Override RttWr
  IDS_MEM_RTTPARK,                    ///< ac Hook for Override RttPark
  IDS_MEM_ADDR_CMD_TMG,               ///< ad Address command timing
  IDS_MEM_MR6_VREF_DQ,                ///< ae MR6 VRefDQ
  IDS_MEM_PMU_RETRAIN_TIMES,          ///< af override memory PMU retrain times
  IDS_OPTION_END                      ///< B0 End of IDS option
} AGESA_IDS_OPTION;

#include "OptionsIds.h"
#include "Filecode.h"
#include "IdsPerf.h"

/* Initialize IDS controls */
#ifndef IDSOPT_IDS_ENABLED
  #define IDSOPT_IDS_ENABLED        FALSE
#endif

#ifndef IDSOPT_CONTROL_ENABLED
  #define IDSOPT_CONTROL_ENABLED    FALSE
#endif

#ifndef IDSOPT_CONTROL_NV_TO_CMOS
  #define IDSOPT_CONTROL_NV_TO_CMOS FALSE
#endif

#ifndef IDSOPT_TRACING_ENABLED
  #define IDSOPT_TRACING_ENABLED    FALSE
#endif

#ifndef IDSOPT_TRACE_USER_OPTIONS
  #define IDSOPT_TRACE_USER_OPTIONS TRUE
#endif

#ifndef IDSOPT_PERF_ANALYSIS
  #define IDSOPT_PERF_ANALYSIS      FALSE
#endif

#ifndef IDSOPT_HEAP_CHECKING
  #define IDSOPT_HEAP_CHECKING           FALSE
#endif

#ifndef IDSOPT_ASSERT_ENABLED
  #define IDSOPT_ASSERT_ENABLED         FALSE
#endif

#ifndef IDSOPT_CAR_CORRUPTION_CHECK_ENABLED
  #define IDSOPT_CAR_CORRUPTION_CHECK_ENABLED   FALSE
#endif

#ifndef IDSOPT_DEBUG_CODE_ENABLED
  #define IDSOPT_DEBUG_CODE_ENABLED   FALSE
#endif

#ifndef IDSOPT_IDT_EXCEPTION_TRAP
  #define IDSOPT_IDT_EXCEPTION_TRAP FALSE
#endif

#ifndef IDSOPT_C_OPTIMIZATION_DISABLED
  #define IDSOPT_C_OPTIMIZATION_DISABLED   FALSE
#endif

#ifndef IDSOPT_TRACING_CONSOLE_HDTOUT
  #define IDSOPT_TRACING_CONSOLE_HDTOUT   TRUE
#endif

#ifndef IDSOPT_TRACING_CONSOLE_SERIALPORT
  #define IDSOPT_TRACING_CONSOLE_SERIALPORT   FALSE
#endif

#ifndef IDSOPT_TRACING_CONSOLE_REDIRECT_IO
  #define IDSOPT_TRACING_CONSOLE_REDIRECT_IO  FALSE
#endif

#ifndef IDSOPT_TRACING_CONSOLE_RAM
  #define IDSOPT_TRACING_CONSOLE_RAM  FALSE
#endif

#if IDSOPT_IDS_ENABLED == FALSE
  #undef  IDSOPT_CONTROL_ENABLED
  #undef  IDSOPT_TRACING_ENABLED
  #undef  IDSOPT_PERF_ANALYSIS
  #undef  IDSOPT_HEAP_CHECKING
  #undef  IDSOPT_ASSERT_ENABLED
  #undef  IDSOPT_CAR_CORRUPTION_CHECK_ENABLED
  #undef  IDSOPT_DEBUG_CODE_ENABLED
  #undef  IDSOPT_TRACE_USER_OPTIONS
  #undef  IDSOPT_TRACING_CONSOLE_HDTOUT
  #undef  IDSOPT_TRACING_CONSOLE_SERIALPORT
  #undef  IDSOPT_TRACING_CONSOLE_REDIRECT_IO
  #undef  IDSOPT_TRACING_CONSOLE_RAM

  #define IDSOPT_CONTROL_ENABLED    FALSE
  #define IDSOPT_TRACING_ENABLED    FALSE
  #define IDSOPT_PERF_ANALYSIS      FALSE
  #define IDSOPT_HEAP_CHECKING      FALSE
  #define IDSOPT_ASSERT_ENABLED     FALSE
  #define IDSOPT_CAR_CORRUPTION_CHECK_ENABLED FALSE
  #define IDSOPT_DEBUG_CODE_ENABLED FALSE
  #define IDSOPT_TRACE_USER_OPTIONS FALSE
  #define IDSOPT_TRACING_CONSOLE_HDTOUT FALSE
  #define IDSOPT_TRACING_CONSOLE_SERIALPORT FALSE
  #define IDSOPT_TRACING_CONSOLE_REDIRECT_IO FALSE
  #define IDSOPT_TRACING_CONSOLE_RAM FALSE
#endif

//Disable when master token Tracing is set to FALSE
#if (IDSOPT_TRACING_ENABLED == FALSE) || (defined (IDSOPT_CUSTOMIZE_TRACING_SERVICE))
  #undef  IDSOPT_TRACING_CONSOLE_HDTOUT
  #define IDSOPT_TRACING_CONSOLE_HDTOUT FALSE

  #undef  IDSOPT_TRACING_CONSOLE_SERIALPORT
  #define IDSOPT_TRACING_CONSOLE_SERIALPORT FALSE

  #undef IDSOPT_TRACING_CONSOLE_REDIRECT_IO
  #define IDSOPT_TRACING_CONSOLE_REDIRECT_IO FALSE

  #undef IDSOPT_TRACING_CONSOLE_RAM
  #define IDSOPT_TRACING_CONSOLE_RAM FALSE
#endif

//Disable Tracing if all support HW layer set to FALSE
#if ((IDSOPT_TRACING_CONSOLE_HDTOUT == FALSE) && (IDSOPT_TRACING_CONSOLE_SERIALPORT == FALSE) && (IDSOPT_TRACING_CONSOLE_REDIRECT_IO == FALSE) && (IDSOPT_TRACING_CONSOLE_RAM == FALSE))
  #ifndef IDSOPT_CUSTOMIZE_TRACING_SERVICE
    #undef  IDSOPT_TRACING_ENABLED
    #define IDSOPT_TRACING_ENABLED FALSE
  #endif
#endif
/**
 *  Make a Progress Report to the User.
 *
 *  This Macro is always enabled. The default action is to write the TestPoint value
 *  to an I/O port. The I/O port is 8 bits in size and the default address is 0x80.
 *  IBVs can change AGESA's default port by defining IDS_DEBUG_PORT to desired port
 *  in OptionsIds.h in their build tip.
 *
 *  @param[in]      TestPoint       The value for display indicating progress
 *  @param[in,out]  StdHeader       Pointer of AMD_CONFIG_PARAMS
 *
 **/

#define AGESA_TESTPOINT(TestPoint, StdHeader) IdsAgesaTestPoint ((TestPoint), (StdHeader))

#ifndef IDS_DEBUG_PORT
  #define IDS_DEBUG_PORT  0x80
#endif

#if IDS_DEBUG_PORT > 0xFFFFul
  #error "Invalid debug port defined.  IDS_DEBUG_PORT address must not be larger than 16 bits."
#endif

#ifndef IDS_DEBUG_PORT_SIZE_IN_BYTES
  #define IDS_DEBUG_PORT_SIZE_IN_BYTES  4
#endif

#if IDS_DEBUG_PORT_SIZE_IN_BYTES == 1
  #define IDS_DEBUG_PORT_ACCESSWIDTH  AccessWidth8
  #ifndef IDS_DEBUG_TP_PREFIX
    #define IDS_DEBUG_TP_PREFIX  0x00ul
  #endif
  #if IDS_DEBUG_TP_PREFIX != 0
    #error "Invalid debug port prefix defined.  IDS_DEBUG_TP_PREFIX << 8 must not exceed IDS_DEBUG_PORT_SIZE_IN_BYTES."
  #endif
#else
  #if IDS_DEBUG_PORT_SIZE_IN_BYTES == 2
    #define IDS_DEBUG_PORT_ACCESSWIDTH  AccessWidth16
    #ifndef IDS_DEBUG_TP_PREFIX
      #define IDS_DEBUG_TP_PREFIX  0xA0ul
    #endif
    #if IDS_DEBUG_TP_PREFIX > 0xFFul
      #error "Invalid debug port prefix defined.  IDS_DEBUG_TP_PREFIX << 8 must not exceed IDS_DEBUG_PORT_SIZE_IN_BYTES."
    #endif
  #else
    #if IDS_DEBUG_PORT_SIZE_IN_BYTES == 4
      #define IDS_DEBUG_PORT_ACCESSWIDTH  AccessWidth32
      #ifndef IDS_DEBUG_TP_PREFIX
        #define IDS_DEBUG_TP_PREFIX  0xA9E5A0ul
      #endif
      #if IDS_DEBUG_TP_PREFIX > 0xFFFFFFul
        #error "Invalid debug port prefix defined.  IDS_DEBUG_TP_PREFIX << 8 must not exceed IDS_DEBUG_PORT_SIZE_IN_BYTES."
      #endif
    #else
      #error "Invalid debug port size defined.  Acceptable values of IDS_DEBUG_PORT_SIZE_IN_BYTES are 1, 2, or 4."
    #endif
  #endif
#endif

#if (IDS_DEBUG_PORT & (IDS_DEBUG_PORT_SIZE_IN_BYTES - 1)) != 0
  #error "IDS_DEBUG_PORT must be aligned on IDS_DEBUG_PORT_SIZE_IN_BYTES boundary."
#endif

/**
 * @def STOP_HERE
 *  (macro) - Causes program to halt. This is @b only for use during active debugging .
 *
 *  Causes the program to halt and display the file number of the source of the
 *  halt (displayed in decimal).
 *
 **/
#if IDSOPT_IDS_ENABLED == TRUE
  #ifdef STOP_CODE
    #undef STOP_CODE
  #endif
  #define STOP_CODE (((UINT32)FILECODE)*0x10000ul + \
                      ((__LINE__) % 10) + (((__LINE__ / 10) % 10)*0x10) + \
                      (((__LINE__ / 100) % 10)*0x100) + (((__LINE__ / 1000) % 10)*0x1000))
  #define STOP_HERE IdsErrorStop (STOP_CODE);
#else
  #define STOP_HERE  STOP_HERE_Needs_To_Be_Removed //"WARNING: Debug code needs to be removed for production builds."
#endif

/**
 * @def ASSERT
 *  Test an assertion that the given statement is True.
 *
 *  The statement is evaluated to a boolean value. If the statement is True,
 *  then no action is taken (no error). If the statement is False, a error stop
 *  is generated to halt the program. Used for testing for fatal errors that
 *  must be resolved before production. This is used to do parameter checks,
 *  bounds checking, range checks and 'sanity' checks.
 *
 * @param[in]   conditional    Assert that evaluating this conditional results in TRUE.
 *
 **/
#ifndef ASSERT
  #if IDSOPT_ASSERT_ENABLED == TRUE
    #ifdef STOP_CODE
      #undef STOP_CODE
    #endif
    #define STOP_CODE (((UINT32)FILECODE)*0x10000ul + \
                        ((__LINE__) % 10) + (((__LINE__ / 10) % 10)*0x10) + \
                        (((__LINE__ / 100) % 10)*0x100) + (((__LINE__ / 1000) % 10)*0x1000))

    #define ASSERT(conditional) ((conditional) ? 0 : IdsAssert (STOP_CODE));
  #else
    #define ASSERT(conditional)
  #endif
#endif

#if IDSOPT_CAR_CORRUPTION_CHECK_ENABLED == TRUE
  #define IDS_CAR_CORRUPTION_CHECK(StdHeader)  IdsCarCorruptionCheck(StdHeader)
#else
  #define IDS_CAR_CORRUPTION_CHECK(StdHeader)
#endif
/**
 * @def DEBUG_CODE
 * Make the code active when IDSOPT_DEBUG_CODE_ENABLED enable
 *
 */
#ifndef DEBUG_CODE
  #if IDSOPT_DEBUG_CODE_ENABLED == TRUE
    #define  DEBUG_CODE(Code) Code
  #else
    #define  DEBUG_CODE(Code)
  #endif
#endif

///give the extended Macro default value
#ifndef __IDS_EXTENDED__
  #define IDS_EXTENDED_HOOK(idsoption, dataptr, idsnvptr, stdheader) IDS_SUCCESS
  #define IDS_TRACE_DEFAULT (0)
  #define IDS_INITIAL_F15_CZ_PM_STEP
  #define IDS_INITIAL_F15_ST_PM_STEP
  #define IDS_F15_CZ_PM_CUSTOM_STEP
  #define IDS_F15_ST_PM_CUSTOM_STEP
  #define IDS_EXTENDED_GET_DATA_EARLY(data, StdHeader)
  #define IDS_EXTENDED_GET_DATA_LATE(data, StdHeader)
  #define IDS_EXTENDED_HEAP_SIZE 0
  #define IDS_EXT_INCLUDE_F15(file)
  #define IDS_EXT_INCLUDE(file)
  #define IDS_PAD_4K
  #define IDS_EXTENDED_CODE(code)
  #define SMU_FIRMWARE_PADS_1K
#endif

#ifndef IDS_NUM_NV_ITEM
  #define IDS_NUM_NV_ITEM (IDS_NUM_EXT_NV_ITEM)
#endif

#define IDS_CMOS_INDEX_PORT 0x70
#define IDS_CMOS_DATA_PORT 0x71
#define IDS_CMOS_REGION_START 0x20
#define IDS_CMOS_REGION_END 0x7F
#define IDS_AP_GET_NV_FROM_CMOS(x) FALSE

#if IDSOPT_CONTROL_ENABLED == TRUE
  #define IDS_OPTION_HOOK(IdsOption, DataPtr, StdHeader) \
          AmdIdsCtrlDispatcher ((IdsOption), (DataPtr), (StdHeader))

  #define IDS_OPTION_CALLOUT(CallOutId, DataPtr, StdHeader) \
          IdsOptionCallout ((CallOutId), (DataPtr), (StdHeader))
  #if IDSOPT_CONTROL_NV_TO_CMOS == TRUE
    #undef IDS_AP_GET_NV_FROM_CMOS
    #define IDS_AP_GET_NV_FROM_CMOS(x) AmdIdsApGetNvFromCmos(x)
    #ifdef IDS_OPT_CMOS_INDEX_PORT
      #undef IDS_CMOS_INDEX_PORT
      #define IDS_CMOS_INDEX_PORT IDS_OPT_CMOS_INDEX_PORT
    #endif

    #ifdef IDS_OPT_CMOS_DATA_PORT
      #undef IDS_CMOS_DATA_PORT
      #define IDS_CMOS_DATA_PORT IDS_OPT_CMOS_DATA_PORT
    #endif

    #ifdef IDS_OPT_CMOS_REGION_START
      #undef IDS_CMOS_REGION_START
      #define IDS_CMOS_REGION_START IDS_OPT_CMOS_REGION_START
    #endif

    #ifdef IDS_OPT_CMOS_REGION_END
      #undef IDS_CMOS_REGION_END
      #define IDS_CMOS_REGION_END IDS_OPT_CMOS_REGION_END
    #endif
  #endif
#else
  #define IDS_OPTION_HOOK(IdsOption, DataPtr, StdHeader)

  #define IDS_OPTION_CALLOUT(CallOutId, DataPtr, StdHeader) AGESA_SUCCESS
#endif

/**
 *  Macro to add a *skip* hook for IDS options
 *
 *  The default minimal action is to do nothing and there is no any code to increase.
 *  For debug environments, IDS dispatcher function will be called to perform
 *  the detailed action and to skip AGESA code if necessary.
 *
 * @param[in]       IdsOption       IDS Option ID for this hook point
 * @param[in, out]  DataPtr         Data Pointer to override
 * @param[in, out]  StdHeader       Pointer of AMD_CONFIG_PARAMS
 *
 *
 **/

#if IDSOPT_CONTROL_ENABLED == TRUE
  #define IDS_SKIP_HOOK(IdsOption, DataPtr, StdHeader) \
          if (AmdIdsCtrlDispatcher (IdsOption, DataPtr, StdHeader) == IDS_SUCCESS)
#else
  #define IDS_SKIP_HOOK(IdsOption, DataPtr, StdHeader)
#endif

/**
 *  Macro to add a heap manager routine
 *
 *  when memory is allocated the heap manager actually allocates two extra dwords of data,
 *  one dword buffer before the actual memory, and one dword afterwards.
 *  a complete heap walk and check to be performed at any time.
 *  it would ASSERT if the heap is corrupt
 *
 * @param[in]  StdHeader       Pointer of AMD_CONFIG_PARAMS
 *
 *
 **/

// Heap debug feature
#define SENTINEL_BEFORE_VALUE 0x64616548ul    // "Head"
#define SENTINEL_AFTER_VALUE  0x6C696154ul    // "Tail"
#if IDSOPT_IDS_ENABLED == TRUE
  #if IDSOPT_HEAP_CHECKING == TRUE
    #define SIZE_OF_SENTINEL  4
    #define NUM_OF_SENTINEL   2             // Before ("Head") and After ("Tail")
    #define SET_SENTINEL_BEFORE(NodePtr, AlignTo16Byte) (*(UINT32 *) ((UINT8 *) NodePtr + sizeof (BUFFER_NODE) + AlignTo16Byte) = SENTINEL_BEFORE_VALUE);
    #define SET_SENTINEL_AFTER(NodePtr) (*(UINT32 *) ((UINT8 *) NodePtr + sizeof (BUFFER_NODE) + NodePtr->BufferSize - SIZE_OF_SENTINEL) = SENTINEL_AFTER_VALUE);
    #define Heap_Check(stdheader) AmdHeapIntactCheck(stdheader)
  #else
    #define SIZE_OF_SENTINEL  0
    #define NUM_OF_SENTINEL   0
    #define SET_SENTINEL_BEFORE(NodePtr, AlignTo16Byte)
    #define SET_SENTINEL_AFTER(NodePtr)
    #define Heap_Check(stdheader)
  #endif
#else
  #define SIZE_OF_SENTINEL    0
  #define NUM_OF_SENTINEL     0
  #define SET_SENTINEL_BEFORE(NodePtr, AlignTo16Byte)
  #define SET_SENTINEL_AFTER(NodePtr)
  #define Heap_Check(stdheader)
#endif

/**
 *  Macro to add IDT for debugging exception.
 *
 *  A debug feature. Adding a 'jmp $' into every exception handler.
 *  So debugger could use HDT to skip 'jmp $' and execute the iret,
 *  then they could find which instruction cause the exception.
 *
 * @param[in]       FunctionId      IDS Function ID for this hook point
 * @param[in, out]  DataPtr         Data Pointer to override
 * @param[in, out]  StdHeader       Pointer of AMD_CONFIG_PARAMS
 *
 *
 **/
#if IDSOPT_IDS_ENABLED == TRUE
  #if IDSOPT_IDT_EXCEPTION_TRAP == TRUE
    #define IDS_EXCEPTION_TRAP(FunctionId, DataPtr, StdHeader) IdsExceptionTrap (FunctionId, DataPtr, StdHeader)
  #else
    #define IDS_EXCEPTION_TRAP(FunctionId, DataPtr, StdHeader)
  #endif
#else
  #define IDS_EXCEPTION_TRAP(FunctionId, DataPtr, StdHeader)
#endif


 //Note a is from 0 to 63
#define DEBUG_PRINT_SHIFT(a)   ((UINT64)1 << a)
//If you change the Bitmap definition below, please change the Hash in ParseFilter of hdtout2008.pl accordingly
//Memory Masks
#define MEM_SETREG                                  DEBUG_PRINT_SHIFT (0)
#define MEM_GETREG                                  DEBUG_PRINT_SHIFT (1)
#define MEM_FLOW                                    DEBUG_PRINT_SHIFT (2)
#define MEM_STATUS                                  DEBUG_PRINT_SHIFT (3)
#define MEM_UNDEF_BF                                DEBUG_PRINT_SHIFT (4)
#define MEM_PMU                                     DEBUG_PRINT_SHIFT (5)
#define MEMORY_TRACE_RSV3                           DEBUG_PRINT_SHIFT (6)
#define MEMORY_TRACE_RSV4                           DEBUG_PRINT_SHIFT (7)
#define MEMORY_TRACE_RSV5                           DEBUG_PRINT_SHIFT (8)
#define MEMORY_TRACE_RSV6                           DEBUG_PRINT_SHIFT (9)

//CPU Masks
#define CPU_TRACE                                   DEBUG_PRINT_SHIFT (10)
#define CPU_TRACE_RSV1                              DEBUG_PRINT_SHIFT (11)
#define CPU_TRACE_RSV2                              DEBUG_PRINT_SHIFT (12)
#define CPU_TRACE_RSV3                              DEBUG_PRINT_SHIFT (13)
#define CPU_TRACE_RSV4                              DEBUG_PRINT_SHIFT (14)
#define CPU_TRACE_RSV5                              DEBUG_PRINT_SHIFT (15)
#define CPU_TRACE_RSV6                              DEBUG_PRINT_SHIFT (16)
#define CPU_TRACE_RSV7                              DEBUG_PRINT_SHIFT (17)
#define CPU_TRACE_RSV8                              DEBUG_PRINT_SHIFT (18)
#define CPU_TRACE_RSV9                              DEBUG_PRINT_SHIFT (19)

//GNB Masks
#define GNB_TRACE                                   DEBUG_PRINT_SHIFT (20)
#define PCIE_MISC                                   DEBUG_PRINT_SHIFT (21)
#define PCIE_PORTREG_TRACE                          DEBUG_PRINT_SHIFT (22)
#define PCIE_HOSTREG_TRACE                          DEBUG_PRINT_SHIFT (23)
#define GNB_TRACE_RSV2                              DEBUG_PRINT_SHIFT (24)
#define NB_MISC                                     DEBUG_PRINT_SHIFT (25)
#define GNB_TRACE_RSV3                              DEBUG_PRINT_SHIFT (26)
#define GFX_MISC                                    DEBUG_PRINT_SHIFT (27)
#define NB_SMUREG_TRACE                             DEBUG_PRINT_SHIFT (28)
#define GNB_TRACE_RSV1                              DEBUG_PRINT_SHIFT (29)

//Topology Masks
#define TOPO_TRACE                                  DEBUG_PRINT_SHIFT (30)
#define TOPO_TRACE_RSV1                             DEBUG_PRINT_SHIFT (31)
#define TOPO_TRACE_RSV2                             DEBUG_PRINT_SHIFT (32)
#define TOPO_TRACE_RSV3                             DEBUG_PRINT_SHIFT (33)
#define TOPO_TRACE_RSV4                             DEBUG_PRINT_SHIFT (34)
#define TOPO_TRACE_RSV5                             DEBUG_PRINT_SHIFT (35)
#define TOPO_TRACE_RSV6                             DEBUG_PRINT_SHIFT (36)
#define TOPO_TRACE_RSV7                             DEBUG_PRINT_SHIFT (37)
#define TOPO_TRACE_RSV8                             DEBUG_PRINT_SHIFT (38)
#define TOPO_TRACE_RSV9                             DEBUG_PRINT_SHIFT (39)

//FCH Masks
#define FCH_TRACE                                    DEBUG_PRINT_SHIFT (40)
#define FCH_TRACE_RSV1                               DEBUG_PRINT_SHIFT (41)
#define FCH_TRACE_RSV2                               DEBUG_PRINT_SHIFT (42)
#define FCH_TRACE_RSV3                               DEBUG_PRINT_SHIFT (43)
#define FCH_TRACE_RSV4                               DEBUG_PRINT_SHIFT (44)
#define FCH_TRACE_RSV5                               DEBUG_PRINT_SHIFT (45)
#define FCH_TRACE_RSV6                               DEBUG_PRINT_SHIFT (46)
#define FCH_TRACE_RSV7                               DEBUG_PRINT_SHIFT (47)
#define FCH_TRACE_RSV8                               DEBUG_PRINT_SHIFT (48)
#define FCH_TRACE_RSV9                               DEBUG_PRINT_SHIFT (49)

//Other Masks
#define MAIN_FLOW                                    DEBUG_PRINT_SHIFT (50)
#define EVENT_LOG                                    DEBUG_PRINT_SHIFT (51)
#define PERFORMANCE_ANALYSE                          DEBUG_PRINT_SHIFT (52)

//Ids Masks
#define IDS_TRACE                                    DEBUG_PRINT_SHIFT (53)
#define BVM_TRACE                                    DEBUG_PRINT_SHIFT (54)
#define IDS_TRACE_RSV2                               DEBUG_PRINT_SHIFT (55)
#define IDS_TRACE_RSV3                               DEBUG_PRINT_SHIFT (56)

//S3
#define S3_TRACE                                     DEBUG_PRINT_SHIFT (57)

//Library function to read/write PCI/MSR registers
#define LIB_PCI_RD                                   DEBUG_PRINT_SHIFT (58)
#define LIB_PCI_WR                                   DEBUG_PRINT_SHIFT (59)

//AGESA test points
#define TEST_POINT                                   DEBUG_PRINT_SHIFT (60)

//Reserved for platform log
#define PLAT_RSV1                                    DEBUG_PRINT_SHIFT (61)
#define PLAT_RSV2                                    DEBUG_PRINT_SHIFT (62)
#define PLAT_RSV3                                    DEBUG_PRINT_SHIFT (63)

#define GNB_TRACE_DEFAULT\
      (\
      GNB_TRACE | PCIE_MISC | NB_MISC | GFX_MISC \
      )

#define GNB_TRACE_REG\
      (\
      PCIE_PORTREG_TRACE | PCIE_HOSTREG_TRACE | \
      NB_SMUREG_TRACE | GNB_TRACE_RSV1 \
      )

#define GNB_TRACE_ALL\
      (\
      GNB_TRACE_DEFAULT | GNB_TRACE_REG \
      )

#define CPU_TRACE_ALL\
      (\
      CPU_TRACE | CPU_TRACE_RSV1 | CPU_TRACE_RSV2 | CPU_TRACE_RSV3 | \
      CPU_TRACE_RSV4 | CPU_TRACE_RSV5 | CPU_TRACE_RSV6 | CPU_TRACE_RSV7 | \
      CPU_TRACE_RSV8 | CPU_TRACE_RSV9\
      )

#define MEMORY_TRACE_ALL\
      (\
      MEM_FLOW | MEM_GETREG | MEM_SETREG | MEM_STATUS | \
      MEM_UNDEF_BF | MEM_PMU | MEMORY_TRACE_RSV3 | MEMORY_TRACE_RSV4 | \
      MEMORY_TRACE_RSV5 | MEMORY_TRACE_RSV6\
      )

#define MEMORY_TRACE_DEFAULT\
      (\
      MEM_FLOW | MEM_STATUS | MEM_PMU\
      )


#define TOPO_TRACE_ALL\
      (\
      TOPO_TRACE | TOPO_TRACE_RSV1 | TOPO_TRACE_RSV2 | TOPO_TRACE_RSV3 | \
      TOPO_TRACE_RSV4 | TOPO_TRACE_RSV5 | TOPO_TRACE_RSV6 | TOPO_TRACE_RSV7 | \
      TOPO_TRACE_RSV8 | TOPO_TRACE_RSV9\
      )

#define FCH_TRACE_ALL\
      (\
      FCH_TRACE | FCH_TRACE_RSV1 | FCH_TRACE_RSV2 | FCH_TRACE_RSV3 | \
      FCH_TRACE_RSV4 | FCH_TRACE_RSV5 | FCH_TRACE_RSV6 | FCH_TRACE_RSV7 | \
      FCH_TRACE_RSV8 | FCH_TRACE_RSV9\
      )

#define IDS_TRACE_ALL\
      (\
      IDS_TRACE | BVM_TRACE | IDS_TRACE_RSV2 | IDS_TRACE_RSV3\
      )

#define OTHER_TRACE_ALL\
      (\
      MAIN_FLOW | EVENT_LOG | PERFORMANCE_ANALYSE\
      )


#define TRACE_MASK_ALL (0xFFFFFFFFFFFFFFFFull)
#ifndef IDS_DEBUG_PRINT_MASK
  #define IDS_DEBUG_PRINT_MASK (GNB_TRACE_DEFAULT  | CPU_TRACE_ALL | MEMORY_TRACE_DEFAULT | TOPO_TRACE_ALL | FCH_TRACE_ALL | MAIN_FLOW | IDS_TRACE_DEFAULT | TEST_POINT)
#endif

/// if no specific define INIT & EXIT will be NULL
#define IDS_HDT_CONSOLE_INIT(x)
#define IDS_HDT_CONSOLE_EXIT(x)

/// AGESA tracing service
#if IDSOPT_TRACING_ENABLED == TRUE
  #ifdef VA_ARGS_SUPPORTED
    #if IDSOPT_C_OPTIMIZATION_DISABLED == TRUE
      #define IDS_HDT_CONSOLE(f, s, ...)        AmdIdsDebugPrint (f, s, __VA_ARGS__)
    #else
      #pragma warning(disable: 4127)
      #define IDS_HDT_CONSOLE(f, s, ...)        if      (f == MEM_FLOW) AmdIdsDebugPrintMem (s, __VA_ARGS__); \
                                                 else if (f == CPU_TRACE) AmdIdsDebugPrintCpu (s, __VA_ARGS__); \
                                                  else if (f == TOPO_TRACE) AmdIdsDebugPrintTopology (s, __VA_ARGS__); \
                                                  else if (f == GNB_TRACE) AmdIdsDebugPrintGnb (s, __VA_ARGS__); \
                                                 else AmdIdsDebugPrint (f, s, __VA_ARGS__)
    #endif
  #else
    #define IDS_HDT_CONSOLE    AmdIdsDebugPrint
  #endif
  #define CONSOLE            AmdIdsDebugPrintAll
  #define IDS_HDT_CONSOLE_DEBUG_CODE(Code)  Code
  #define IDS_TIMEOUT_CTL(t)                IdsMemTimeOut (t)
#else
  #define IDS_HDT_CONSOLE           1 ? (VOID) 0 : AmdIdsDebugPrint
  #define IDS_HDT_CONSOLE_DEBUG_CODE(Code)
  #define CONSOLE   CONSOLE_Needs_To_Be_Removed_For_Production_Build //"WARNING: CONSOLE needs to be removed for production builds."
  #define IDS_TIMEOUT_CTL(t)
#endif

/// Macros for serial port tracing
#ifdef IDSOPT_SERIAL_PORT
  #define IDS_SERIAL_PORT IDSOPT_SERIAL_PORT
#endif

#ifndef IDS_SERIAL_PORT
  #define IDS_SERIAL_PORT 0x3F8
#endif

// Macros for redirect IO tracing
#ifdef IDSOPT_DEBUG_PRINT_IO_PORT
  #define IDS_DEBUG_PRINT_IO_PORT IDSOPT_DEBUG_PRINT_IO_PORT
#endif

#ifndef IDS_DEBUG_PRINT_IO_PORT
  #define IDS_DEBUG_PRINT_IO_PORT 0x80
#endif

#define IDS_DPRAM_BASE 0
#define IDS_DPRAM_SIZE 0
///Default policy, shift the old data when buffer full
#define IDS_DPRAM_STOP_LOGGING_WHEN_BUFFER_FULL FALSE
#if IDSOPT_TRACING_CONSOLE_RAM  == TRUE
  #ifdef IDSOPT_DPRAM_BASE
    #undef IDS_DPRAM_BASE
    #define IDS_DPRAM_BASE  IDSOPT_DPRAM_BASE
  #else
    #error "Debug Print Ram Base not specified, please define IDSOPT_DPRAM_BASE in optionsids.h"
  #endif

  #ifdef IDSOPT_DPRAM_SIZE
    #undef IDS_DPRAM_SIZE
    #define IDS_DPRAM_SIZE  IDSOPT_DPRAM_SIZE
  #else
    #error "Debug Print Ram size not specified, please define IDSOPT_DPRAM_SIZE in optionsids.h"
  #endif

  #ifdef IDSOPT_DPRAM_STOP_LOGGING_WHEN_BUFFER_FULL
    #define IDS_DPRAM_STOP_LOGGING_WHEN_BUFFER_FULL IDSOPT_DPRAM_STOP_LOGGING_WHEN_BUFFER_FULL
  #endif
#endif
/**
 *  Macros to add HDT OUT
 *
 *  The default minimal action is to do nothing and there is no any code to increase.
 *  For debug environments, the debug information can be displayed in HDT or other
 *  devices.
 *
 **/
#if IDSOPT_TRACING_CONSOLE_HDTOUT == TRUE
  #undef IDS_HDT_CONSOLE_INIT
  #undef IDS_HDT_CONSOLE_EXIT
  #define IDS_HDT_CONSOLE_INIT(x)           AmdIdsHdtOutInit (x)
  #define IDS_HDT_CONSOLE_EXIT(x)           AmdIdsHdtOutExit (x)
  #define IDS_HDT_CONSOLE_S3_EXIT(x)        AmdIdsHdtOutS3Exit (x)
  #define IDS_HDT_CONSOLE_S3_AP_EXIT(x)     AmdIdsHdtOutS3ApExit (x)

  #define IDS_HDT_CONSOLE_FLUSH_BUFFER(x)   AmdIdsHdtOutBufferFlush (x)
  #define IDS_HDT_CONSOLE_ASSERT(x)   AmdIdsDebugPrintAssert (x)
  #define IDS_FUNCLIST_ADDR                 ScriptFuncList
  #define IDS_FUNCLIST_EXTERN()             extern SCRIPT_FUNCTION ScriptFuncList[]
#else
  #define IDS_HDT_CONSOLE_S3_EXIT(x)
  #define IDS_HDT_CONSOLE_S3_AP_EXIT(x)
  #define IDS_HDT_CONSOLE_FLUSH_BUFFER(x)
  #define IDS_HDT_CONSOLE_ASSERT(x)
  #define IDS_FUNCLIST_ADDR                 NULL
  #define IDS_FUNCLIST_EXTERN()
#endif

#if IDSOPT_TRACING_ENABLED == TRUE
  #ifdef IDSOPT_CUSTOMIZE_TRACING_SERVICE
    #undef  IDS_HDT_CONSOLE
    #define IDS_HDT_CONSOLE        IDSOPT_CUSTOMIZE_TRACING_SERVICE
  #endif
  #ifdef IDSOPT_CUSTOMIZE_TRACING_SERVICE_INIT
    #undef IDS_HDT_CONSOLE_INIT
    #define IDS_HDT_CONSOLE_INIT(x)           IDSOPT_CUSTOMIZE_TRACING_SERVICE_INIT (x)
  #endif
  #ifdef IDSOPT_CUSTOMIZE_TRACING_SERVICE_EXIT
    #undef IDS_HDT_CONSOLE_EXIT
    #define IDS_HDT_CONSOLE_EXIT(x)           IDSOPT_CUSTOMIZE_TRACING_SERVICE_EXIT (x)
  #endif
#endif

#define IDS_TRACE_SHOW_BLD_OPT_CFG  IDSOPT_TRACE_USER_OPTIONS

#if IDSOPT_PERF_ANALYSIS == TRUE
  #include "IdsPerf.h"
  #ifdef STOP_CODE
    #undef STOP_CODE
  #endif
  #define STOP_CODE (((UINT32)FILECODE)*0x10000ul + \
                        ((__LINE__) % 10) + (((__LINE__ / 10) % 10)*0x10) + \
                        (((__LINE__ / 100) % 10)*0x100) + (((__LINE__ / 1000) % 10)*0x1000))

  #define IDS_PERF_TIMESTAMP(ID, StdHeader)  IdsPerfTimestamp (STOP_CODE, ID, (StdHeader))
  #define IDS_PERF_ANALYSE(StdHeader)   IdsPerfAnalyseTimestamp (StdHeader)
#else
  #define IDS_PERF_TIMESTAMP(ID, StdHeader)
  #define IDS_PERF_ANALYSE(StdHeader)
#endif

/// Function entry for HDT script to call
typedef struct _SCRIPT_FUNCTION {
  UINT32  FuncAddr;       ///< Function address in ROM
  CHAR8   FuncName[40];   ///< Function name
} SCRIPT_FUNCTION;

/// Data Structure for Mem ECC parameter override
typedef struct {
  IN BOOLEAN CfgEccRedirection;                   ///< ECC Redirection
  IN UINT16  CfgScrubDramRate;                    ///< Scrub Dram Rate
  IN UINT16  CfgScrubL2Rate;                      ///< Scrub L2Rate
  IN UINT16  CfgScrubL3Rate;                      ///< Scrub L3Rate
  IN UINT16  CfgScrubIcRate;                      ///< Scrub Ic Rate
  IN UINT16  CfgScrubDcRate;                      ///< Scrub Dc Rate
  IN BOOLEAN CfgEccSyncFlood;                     ///< ECC Sync Flood
} ECC_OVERRIDE_STRUCT;




/**
 *  AGESA Test Points
 *
 *  These are the values displayed to the user to indicate progress through boot.
 *  These can be used in a debug environment to stop the debugger at a specific
 *  test point:
 *  For SimNow!, this command
 *      bi 81 w vb 49
 *  will stop the debugger on one of the TracePoints (49 is the TP value in this example).
 *
 */
typedef enum {
  StartProcessorTestPoints,           ///< 00 Entry used for range testing for @b Processor related TPs

  // Memory test points
  TpProcMemBeforeMemDataInit,         ///< 01 .. Memory structure initialization (Public interface)
  TpProcMemBeforeSpdProcessing,       ///< 02 .. SPD Data processing  (Public interface)
  TpProcMemAmdMemAuto,                ///< 03 .. Memory configuration  (Public interface)
  TpProcMemDramInit,                  ///< 04 .. DRAM initialization
  TpProcMemSPDChecking,               ///< 05 ..
  TpProcMemModeChecking,              ///< 06 ..
  TpProcMemSpeedTclConfig,            ///< 07 .. Speed and TCL configuration
  TpProcMemSpdTiming,                 ///< 08 ..
  TpProcMemDramMapping,               ///< 09 ..
  TpProcMemPlatformSpecificConfig,    ///< 0A ..
  TPProcMemPhyCompensation,           ///< 0B ..
  TpProcMemStartDcts,                 ///< 0C ..
  TpProcMemBeforeDramInit,            ///< 0D .. (Public interface)
  TpProcMemPhyFenceTraining,          ///< 0E ..
  TpProcMemSynchronizeDcts,           ///< 0F ..
  TpProcMemSystemMemoryMapping,       ///< 10 ..
  TpProcMemMtrrConfiguration,         ///< 11 ..
  TpProcMemDramTraining,              ///< 12 ..
  TpProcMemBeforeAnyTraining,         ///< 13 .. (Public interface)
  TpProcMemWriteLevelizationTraining, ///< 14 ..
  TpProcMemWlFirstPass,               ///< 15 .. Below 800Mhz first pass start
  TpProcMemWlSecondPass,              ///< 16 .. Above 800Mhz second pass start
  TpProcMemWlTrainTargetDimm,         ///< 17 .. Target DIMM configured
  TpProcMemWlPrepDimms,               ///< 18 ..  Prepare DIMMS for WL
  TpProcMemWlConfigDimms,             ///< 19 ..  Configure DIMMS for WL
  TpProcMemReceiverEnableTraining,    ///< 1A ..
  TpProcMemRcvrStartSweep,            ///< 1B .. Start sweep loop
  TpProcMemRcvrSetDelay,              ///< 1C .. Set receiver Delay
  TpProcMemRcvrWritePattern,          ///< 1D .. Write test pattern
  TpProcMemRcvrReadPattern,           ///< 1E .. Read test pattern
  TpProcMemRcvrTestPattern,           ///< 1F .. Compare test pattern
  TpProcMemRcvrCalcLatency,           ///< 20 .. Calculate MaxRdLatency per channel
  TpProcMemReceiveDqsTraining,        ///< 21 ..
  TpProcMemRcvDqsSetDelay,            ///< 22 .. Set Write Data delay
  TpProcMemRcvDqsWritePattern,        ///< 23 .. Write test pattern
  TpProcMemRcvDqsStartSweep,          ///< 24 .. Start read sweep
  TpProcMemRcvDqsSetRcvDelay,         ///< 25 .. Set Receive DQS delay
  TpProcMemRcvDqsReadPattern,         ///< 26 .. Read Test pattern
  TpProcMemRcvDqsTstPattern,          ///< 27 .. Compare Test pattern
  TpProcMemRcvDqsResults,             ///< 28 .. Update results
  TpProcMemRcvDqsFindWindow,          ///< 29 .. Start Find passing window
  TpProcMemTransmitDqsTraining,       ///< 2A ..
  TpProcMemTxDqStartSweep,            ///< 2B .. Start write sweep
  TpProcMemTxDqSetDelay,              ///< 2C .. Set Transmit DQ delay
  TpProcMemTxDqWritePattern,          ///< 2D .. Write test pattern
  TpProcMemTxDqReadPattern,           ///< 2E .. Read Test pattern
  TpProcMemTxDqTestPattern,           ///< 2F .. Compare Test pattern
  TpProcMemTxDqResults,               ///< 30 .. Update results
  TpProcMemTxDqFindWindow,            ///< 31 .. Start Find passing window
  TpProcMemMaxRdLatencyTraining,      ///< 32 ..
  TpProcMemMaxRdLatStartSweep,        ///< 33 .. Start sweep
  TpProcMemMaxRdLatSetDelay,          ///< 34 .. Set delay
  TpProcMemMaxRdLatWritePattern,      ///< 35 .. Write test pattern
  TpProcMemMaxRdLatReadPattern,       ///< 36 .. Read Test pattern
  TpProcMemMaxRdLatTestPattern,       ///< 37 .. Compare Test pattern
  TpProcMemOnlineSpareInit,           ///< 38 .. Online Spare init
  TpProcMemBankInterleaveInit,        ///< 39 .. Bank Interleave Init
  TpProcMemNodeInterleaveInit,        ///< 3A .. Node Interleave Init
  TpProcMemChannelInterleaveInit,     ///< 3B .. Channel Interleave Init
  TpProcMemEccInitialization,         ///< 3C .. ECC initialization
  TpProcMemPlatformSpecificInit,      ///< 3D .. Platform Specific Init
  TpProcMemBeforeAgesaReadSpd,        ///< 3E .. Before callout for "AgesaReadSpd"
  TpProcMemAfterAgesaReadSpd,         ///< 3F .. After callout for "AgesaReadSpd"
  TpProcMemBeforeAgesaHookBeforeDramInit,    ///< 40 .. Before optional callout "AgesaHookBeforeDramInit"
  TpProcMemAfterAgesaHookBeforeDramInit,     ///< 41 .. After optional callout "AgesaHookBeforeDramInit"
  TpProcMemBeforeAgesaHookBeforeDQSTraining, ///< 42 .. Before optional callout "AgesaHookBeforeDQSTraining"
  TpProcMemAfterAgesaHookBeforeDQSTraining,  ///< 43 .. After optional callout "AgesaHookBeforeDQSTraining"
  TpProcMemBeforeAgesaHookBeforeExitSelfRef, ///< 44 .. Before optional callout "AgesaHookBeforeDramInit"
  TpProcMemAfterAgesaHookBeforeExitSelfRef,  ///< 45 .. After optional callout "AgesaHookBeforeDramInit"
  TpProcMemAfterMemDataInit,          ///< 46 .. After MemDataInit
  TpProcMemInitializeMCT,             ///< 47 .. Before InitializeMCT
  TpProcMemLvDdr3,                    ///< 48 .. Before LV DDR3
  TpProcMemInitMCT,                   ///< 49 .. Before InitMCT
  TpProcMemOtherTiming,               ///< 4A.. Before OtherTiming
  TpProcMemUMAMemTyping,              ///< 4B .. Before UMAMemTyping
  TpProcMemSetDqsEccTmgs,             ///< 4C .. Before SetDqsEccTmgs
  TpProcMemMemClr,                    ///< 4D .. Before MemClr
  TpProcMemOnDimmThermal,             ///< 4E .. Before On DIMM Thermal
  TpProcMemDmi,                       ///< 4F .. Before DMI
  TpProcMemEnd,                       ///< 50 .. End of memory code

  // CPU test points
  TpProcCpuEntryDmi,                  ///< 51 .. Entry point CreateDmiRecords
  TpProcCpuEntryPstate,               ///< 52 .. Entry point GenerateSsdt
  TpProcCpuEntryPstateLeveling,       ///< 53 .. Entry point PStateLeveling
  TpProcCpuEntryPstateGather,         ///< 54 .. Entry point PStateGatherData
  TpProcCpuEntryWhea,                 ///< 55 .. Entry point CreateAcpiWhea
  TpProcS3Init,                       ///< 56    Entry point S3Init
  TpProcCpuProcessRegisterTables = 0X58,     ///< 58 .. Register table processing
  TpProcCpuSetBrandID,                ///< 59 .. Set brand ID
  TpProcCpuLocalApicInit,             ///< 5A .. Initialize local APIC
  TpProcCpuLoadUcode,                 ///< 5B .. Load microcode patch
  TpProcCpuBeforePMFeatureInit,       ///< 5C .. BeforePM feature dispatch point
  TpProcCpuPowerMgmtInit,             ///< 5D .. Power Management table processing
  TpProcCpuEarlyFeatureInit,          ///< 5E .. Early feature dispatch point
  TpProcCpuCoreLeveling,              ///< 5F .. Core Leveling
  TpProcCpuApMtrrSync,                ///< 60 .. AP MTRR sync up
  TpProcCpuPostFeatureInit,           ///< 61 .. POST feature dispatch point
  TpProcCpuBeforeRelinquishAPsFeatureInit = 0x63, ///< 63 .. Before Relinquishing control of APs feature dispatch point
  TpProcCpuBeforeAllocateWheaBuffer,  ///< 64 .. Before the WHEA init code calls out to allocate a buffer
  TpProcCpuAfterAllocateWheaBuffer,   ///< 65 .. After the WHEA init code calls out to allocate a buffer
  TpProcCpuBeforeAllocateSsdtBuffer = 0x6A,  ///< 6A .. Before the P-state init code calls out to allocate a buffer
  TpProcCpuAfterAllocateSsdtBuffer,   ///< 6B .. After the P-state init code calls out to allocate a buffer
  TpProcCpuEntryCrat,                 ///< 6C .. Entry point CreateAcpiCrat
  TpProcCpuEntryCdit,                 ///< 6D .. Entry point CreateAcpiCdit
  TpProcCpuEntryScs,                  ///< 6E .. Entry point InitializeScsFeature
  TpProcCpuRunBtc,                    ///< 6F .. Start of Btc run
  TpProcCpuEndBtc,                    ///< 70 .. End of Btc run

  // Topology test points
  TpProcTopologyEntry = 0x71,         ///< 71 .. Topology Init begin
  TpProcTopologyDone = 0x7C,          ///< 7C .. Topology Init complete

  // Extended memory test point
  TpProcMemSendMRS2 = 0x80,           ///< 80 .. Sending MRS2
  TpProcMemSendMRS3,                  ///< 81 .. Sedding MRS3
  TpProcMemSendMRS1,                  ///< 82 .. Sending MRS1
  TpProcMemSendMRS0,                  ///< 83 .. Sending MRS0
  TpProcMemContinPatternGenRead,      ///< 84 .. Continuous Pattern Read
  TpProcMemContinPatternGenWrite,     ///< 85 .. Continuous Pattern Write
  TpProcMem2dRdDqsTraining,           ///< 86 .. Mem: 2d RdDqs Training begin
  TpProcMemBefore2dTrainExtVrefChange,///< 87 .. Mem: Before optional callout to platform BIOS to change External Vref during 2d Training
  TpProcMemAfter2dTrainExtVrefChange, ///< 88 .. Mem: After optional callout to platform BIOS to change External Vref during 2d Training
  TpProcMemConfigureDCTForGeneral,    ///< 89 .. Configure DCT For General use begin
  TpProcMemProcConfigureDCTForTraining,    ///< 8A .. Configure DCT For training begin
  TpProcMemConfigureDCTNonExplicitSeq,///< 8B .. Configure DCT For Non-Explicit
  TpProcMemSynchronizeChannels,       ///< 8C .. Configure to Sync channels
  TpProcMemC6StorageAllocation,       ///< 8D .. Allocate C6 Storage
  TpProcMemLvDdr4,                    ///< 8E .. Before LV DDR4

  // Gnb Earlier init
  TpGnbEarlierPcieConfigurationInit = 0x90,           ///< 90 .. GNB earlier PCIE configuration init
  TpGnbEarlierInterface = 0x91,                       ///< 91 .. GNB earlier interface
  // Gnb Early init
  TpGnbEarlyInterface = 0x92,                         ///< 92 .. GNB early interface
  TpGnbPcieConfigurationMap,                          ///< 93 .. GNB early PCIE configuration map
  TpGnbPcieEarlyInterface,                            ///< 94 .. GNB early PCIE interface
  //Gnb post init
  TpGnbPciePostEarlyInterface = 0x95,                 ///< 95 .. GNB post early interface
  TpGnbGfxConfigPostInterface,                        ///< 96 .. GNB post GFX config interface
  TpGnbGfxPostInterface,                              ///< 97 .. GNB post GFX interface
  // Gnb post after DRAM init
  TpGnbPostInterface = 0x98,                          ///< 98 .. GNB post after DRAM interface
  TpGnbPciePostInterface,                             ///< 99 .. GNB post after DRAM PCIE interface
  // Gnb Env init
  TpGnbEnvInterface,                                  ///< 9A .. GNB Env Nb interface
  TpGnbPcieEnvInterface,                              ///< 9B .. GNB Env PCIE interface
  TpGnbGfxConfigEnvInterface,                         ///< 9C .. GNB Env GFX Config interface
  TpGnbGfxEnvInterface,                               ///< 9D .. GNB Env GFX interface
  // Gnb Mid init
  TpGnbGfxConfigMidInterface = 0x9E,                  ///< 9E .. GNB Mid GFX config interface
  TpGnbGfxMidInterface,                               ///< 9F .. GNB Mid GFX interface
  TpPcieMidInterface,                                 ///< A0 .. GNB Mid PCIE interface
  TpGnbMidInterface,                                  ///< A1 .. GNB Mid interface
  TpGnbSmuMidInterface,                               ///< A2 .. GNB Mid SMU feature
  TpPciePowerGateFeature,                             ///< A3 .. GNB Mid pcie power gate feature
  TpGnbPcieMaxPayloadInterface,                       ///< A4 .. GNB Mid pcie max payload interface
  TpGnbPcieClkPmInterface,                            ///< A5 .. GNB Mid pcie clk pm port interface
  TpGnbPcieAspmInterface,                             ///< A6 .. GNB Mid pcie ASPM interface
  TpGnbNbIoapicInterface,                             ///< A7 .. GNB Mid IOAPIC interface
  // Gnb Late init
  TpGnbPcieAlibFeature = 0xA8,                        ///< A8 .. GNB Late pcie ALIB feature
  TpGnbIommuIvrsTable,                                ///< A9 .. GNB Late pcie IOMMU
  TpGnbBtcRun,                                        ///< AA .. GNB Late run BTC
  // Gnb S3 Save
  TpGnbGfxInitSview = 0xAE,                           ///< AD .. GNB S3SAVE GFX sview init
  TpGnbAlibDispatchFeature = 0xAE,                    ///< AE .. GNB ALIB dispatch feature
  EndGnbTestPoints = 0xAF,                            ///< AF End of TP range for GNB

  StartFchTestPoints = 0xB0,          ///< B0 Entry used for range testing for @b FCH related TPs
  TpFchInitResetDispatching,          ///< B1 .. FCH InitReset dispatch point
  TpFchGppBeforePortTraining,         ///< B2 .. Before FCH GPP port training
  TpFchGppGen1PortPolling,            ///< B3 .. FCH GPP port polling with GEN1 speed
  TpFchGppGen2PortPolling,            ///< B4 .. FCH GPP port polling with GEN2 speed
  TpFchGppAfterPortTraining,          ///< B5 .. After FCH GPP port training
  TpFchInitEnvDispatching,            ///< B6 .. FCH InitEnv dispatch point
  TpFchInitMidDispatching,            ///< B7 .. FCH InitMid dispatch point
  TpFchInitLateDispatching,           ///< B8 .. FCH InitLate dispatch point
  TpFchGppHotPlugging,                ///< B9 .. FCH GPP hot plug event
  TpFchGppHotUnplugging,              ///< BA .. AFCH GPP hot unplug event
  TpFchInitS3EarlyDispatching,        ///< BB .. FCH InitS3Early dispatch point
  TpFchInitS3LateDispatching,         ///< BC .. FCH InitS3Late dispatch point
  EndFchTestPoints,                   ///< BF End of TP range for FCH

  // Interface test points
  TpIfAmdInitResetEntry = 0xC0,       ///< C0 .. Entry to AmdInitReset
  TpIfAmdInitResetExit,               ///< C1 .. Exiting from AmdInitReset
  TpIfAmdInitRecoveryEntry,           ///< C2 .. Entry to AmdInitRecovery
  TpIfAmdInitRecoveryExit,            ///< C3 .. Exiting from AmdInitRecovery
  TpIfAmdInitEarlyEntry,              ///< C4 .. Entry to AmdInitEarly
  TpIfAmdInitEarlyExit,               ///< C5 .. Exiting from AmdInitEarly
  TpIfAmdInitPostEntry,               ///< C6 .. Entry to AmdInitPost
  TpIfAmdInitPostExit,                ///< C7 .. Exiting from AmdInitPost
  TpIfAmdInitEnvEntry,                ///< C8 .. Entry to AmdInitEnv
  TpIfAmdInitEnvExit,                 ///< C9 .. Exiting from AmdInitEnv
  TpIfAmdInitMidEntry,                ///< CA .. Entry to AmdInitMid
  TpIfAmdInitMidExit,                 ///< CB .. Exiting from AmdInitMid
  TpIfAmdInitLateEntry,               ///< CC .. Entry to AmdInitLate
  TpIfAmdInitLateExit,                ///< CD .. Exiting from AmdInitLate
  TpIfAmdInitRtbEntry,                ///< CE .. Entry to AmdInitRtb
  TpIfAmdInitRtbExit,                 ///< CF .. Exiting from AmdInitRtb
  TpIfAmdInitResumeEntry,             ///< D0 .. Entry to AmdInitResume
  TpIfAmdInitResumeExit,              ///< D1 .. Exiting from AmdInitResume
  TpIfAmdS3LateRestoreEntry,          ///< D2 .. Entry to AmdS3LateRestore
  TpIfAmdS3LateRestoreExit,           ///< D3 .. Exiting from AmdS3LateRestore
  TpIfAmdLateRunApTaskEntry,          ///< D4 .. Entry to AmdS3LateRestore
  TpIfAmdLateRunApTaskExit,           ///< D5 .. Exiting from AmdS3LateRestore
  TpIfAmdReadEventLogEntry,           ///< D6 .. Entry to AmdReadEventLog
  TpIfAmdReadEventLogExit,            ///< D7 .. Exiting from AmdReadEventLog
  TpIfAmdGetApicIdEntry,              ///< D8 .. Entry to AmdGetApicId
  TpIfAmdGetApicIdExit,               ///< D9 .. Exiting from AmdGetApicId
  TpIfAmdGetPciAddressEntry,          ///< DA .. Entry to AmdGetPciAddress
  TpIfAmdGetPciAddressExit,           ///< DB .. Exiting from AmdGetPciAddress
  TpIfAmdIdentifyCoreEntry,           ///< DC .. Entry to AmdIdentifyCore
  TpIfAmdIdentifyCoreExit,            ///< DD .. Exiting from AmdIdentifyCore
  TpIfBeforeRunApFromIds,             ///< DE .. After IDS calls out to run code on an AP
  TpIfAfterRunApFromIds,              ///< DF .. After IDS calls out to run code on an AP
  TpIfBeforeGetIdsData,               ///< E0 .. Before IDS calls out to get IDS data
  TpIfAfterGetIdsData,                ///< E1 .. After IDS calls out to get IDS data
  TpIfBeforeAllocateHeapBuffer,       ///< E2 .. Before the heap manager calls out to allocate a buffer
  TpIfAfterAllocateHeapBuffer,        ///< E3 .. After the heap manager calls out to allocate a buffer
  TpIfBeforeDeallocateHeapBuffer,     ///< E4 .. Before the heap manager calls out to deallocate a buffer
  TpIfAfterDeallocateHeapBuffer,      ///< E5 .. After the heap manager calls out to deallocate a buffer
  TpIfBeforeLocateHeapBuffer,         ///< E6 .. Before the heap manager calls out to locate a buffer
  TpIfAfterLocateHeapBuffer,          ///< E7 .. After the heap manager calls out to locate a buffer
  TpIfBeforeRunApFromAllAps,          ///< E8 .. Before the BSP calls out to run code on an AP
  TpIfAfterRunApFromAllAps,           ///< E9 .. After the BSP calls out to run code on an AP
  TpIfBeforeRunApFromAllCore0s,       ///< EA .. Before the BSP calls out to run code on an AP
  TpIfAfterRunApFromAllCore0s,        ///< EB .. After the BSP calls out to run code on an AP
  TpIfBeforeAllocateS3SaveBuffer,     ///< EC .. Before the S3 save code calls out to allocate a buffer
  TpIfAfterAllocateS3SaveBuffer,      ///< ED .. After the S3 save code calls out to allocate a buffer
  TpIfBeforeAllocateMemoryS3SaveBuffer,  ///< EE .. Before the memory S3 save code calls out to allocate a buffer
  TpIfAfterAllocateMemoryS3SaveBuffer,   ///< EF .. After the memory S3 save code calls out to allocate a buffer
  TpIfBeforeLocateS3PciBuffer,        ///< F0 .. Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3PciBuffer,         ///< F1 .. After the memory code calls out to locate a buffer
  TpIfBeforeLocateS3CPciBuffer,       ///< F2 .. Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3CPciBuffer,        ///< F3 .. After the memory code calls out to locate a buffer
  TpIfBeforeLocateS3MsrBuffer,        ///< F4 .. Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3MsrBuffer,         ///< F5 .. After the memory code calls out to locate a buffer
  TpIfBeforeLocateS3CMsrBuffer,       ///< F6 .. Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3CMsrBuffer,        ///< F7 .. After the memory code calls out to locate a buffer
  TpReadyToBoot,                      ///< F8 .. Ready to boot event
  // PMU test points
  TpProcMemPmuFailed,                 ///< F9 .. Failed PMU training.
  TpIfAmdS3FinalRestoreEntry,         ///< FA .. Entry to AmdS3FinalRestore
  TpIfAmdS3FinalRestoreExit,          ///< FB .. Exiting from AmdS3FinalRestore

  EndAgesaTps = 0xFF,                 ///< Last defined AGESA TP
} AGESA_TP;

///Ids Feat description
typedef enum {
  IDS_FEAT_UCODE_UPDATE = 0x0000,   ///< Feat for Ucode Update
  IDS_FEAT_TARGET_PSTATE,           ///< Feat for Target Pstate
  IDS_FEAT_POSTPSTATE,              ///< Feat for Post Pstate
  IDS_FEAT_ECC_CTRL,                ///< Feat for Ecc Control
  IDS_FEAT_ECC_SYMBOL_SIZE,         ///< Feat for Ecc symbol size
  IDS_FEAT_DCT_ALLMEMCLK,           ///< Feat for all memory clock
  IDS_FEAT_DCT_ALLCKE,              ///< Feat for all CKE
  IDS_FEAT_DCT_ALLCS,               ///< Feat for all CS
  IDS_FEAT_DCT_GANGMODE,            ///< Feat for Dct gang mode
  IDS_FEAT_DCT_BURSTLENGTH,         ///< Feat for dct burst length
  IDS_FEAT_DCT_POWERDOWN,           ///< Feat for dct power down
  IDS_FEAT_DCT_DLLSHUTDOWN,         ///< Feat for dct dll shut down
  IDS_FEAT_HDTOUT,                  ///< Feat for hdt out
  IDS_FEAT_GNB_PLATFORMCFG,         ///< Feat for override GNB platform config
  IDS_FEAT_CPB_CTRL,                ///< Feat for Config the Core peformance boost feature
  IDS_FEAT_HTC_CTRL,                ///< Feat for Hardware Thermal Control
  IDS_FEAT_MEMORY_MAPPING,          ///< Feat for Memory Mapping
  IDS_FEAT_NV_TO_CMOS,              ///< Feat for Save BSP Nv to CMOS
  IDS_FEAT_COMMON,                  ///< Common Feat
  IDS_FEAT_END = 0xFF               ///< End of Common feat
} IDS_FEAT;

///Ids IDT table function ID
typedef enum {
  IDS_IDT_REPLACE_IDTR_FOR_BSC = 0x0000,   ///< Function ID for saving IDTR for BSC
  IDS_IDT_RESTORE_IDTR_FOR_BSC,            ///< Function ID for restoring IDTR for BSC
  IDS_IDT_UPDATE_EXCEPTION_VECTOR_FOR_AP,  ///< Function ID for updating exception vector
} IDS_IDT_FUNC_ID;

typedef IDS_STATUS IDS_COMMON_FUNC (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  );

typedef IDS_COMMON_FUNC *PIDS_COMMON_FUNC;

/// Data Structure of IDS Feature block
typedef  struct _IDS_FAMILY_FEAT_STRUCT {
  IDS_FEAT  IdsFeat;      ///< Ids Feat ID
  BOOLEAN IsBsp;          ///< swith for Bsp check
  AGESA_IDS_OPTION IdsOption;   ///< IDS option
  UINT64 CpuFamily;       ///<
  PIDS_COMMON_FUNC  pf_idsoption;   ///<pointer to function
} IDS_FAMILY_FEAT_STRUCT;


/// Data Structure of IDS option
typedef  struct _IDS_OPTION_STRUCT {
  AGESA_IDS_OPTION idsoption;       ///< IDS option
  PIDS_COMMON_FUNC  pf_idsoption;   ///<pointer to function
} IDS_OPTION_STRUCT;

/// Data Structure of IDS option table
typedef struct _IDS_OPTION_STRUCT_TBL {
  UINT8 version;      ///<Version of IDS option table
  UINT16 size;        ///<Size of IDS option table
  CONST IDS_OPTION_STRUCT  *pIdsOptionStruct; ///<pointer to array of  structure
} IDS_OPTION_STRUCT_TBL;

#define IDS_NV_TO_CMOS_LEN_BYTE   1
#define IDS_NV_TO_CMOS_LEN_WORD   2
#define IDS_NV_TO_CMOS_LEN_END   0xFF
#define IDS_NV_TO_CMOS_ID_END   0xFFFF

/// Data struct of set/get NV to/from CMOS
typedef struct _IDS_NV_TO_CMOS {
  UINT8 Length;       ///< Length of NV
  UINT16 IDS_NV_ID; ///< IDS id
} IDS_NV_TO_CMOS;

IDS_STATUS
AmdIdsCtrlDispatcher (
  IN       AGESA_IDS_OPTION IdsOption,
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
IdsOptionCallout (
  IN       UINTN                CallOutId,
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
AmdIdsHdtOutInit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
AmdIdsHdtOutExit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
AmdIdsHdtOutS3Exit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
AmdIdsHdtOutS3ApExit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
AmdIdsDebugPrint (
  IN       UINT64      Flag,
  IN       CONST CHAR8 *Format,
  IN       ...
  );

VOID
AmdIdsDebugPrintTopology (
  IN      CONST CHAR8 *Format,
  IN      ...
  );

VOID
AmdIdsDebugPrintCpu (
  IN      CONST CHAR8 *Format,
  IN      ...
  );

VOID
AmdIdsDebugPrintMem (
  IN      CONST CHAR8 *Format,
  IN      ...
  );

VOID
AmdIdsDebugPrintGnb (
  IN      CONST CHAR8 *Format,
  IN      ...
  );

VOID
AmdIdsDebugPrintAll (
  IN      CONST CHAR8 *Format,
  IN      ...
  );

VOID
AmdIdsHdtOutBufferFlush (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
IdsMemTimeOut (
  IN OUT   VOID *DataPtr
  );

VOID
IdsAgesaTestPoint (
  IN      AGESA_TP TestPoint,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

/**
 *  IDS Backend Function for ASSERT
 *
 * Halt execution with stop code display.  Stop Code is displayed on port 80, with rotation so that
 * it is visible on 8, 16, or 32 bit display.  The stop code is alternated with 0xDEAD on the display,
 * to help distinguish the stop code from a post code loop.
 * Additional features may be available if using simulation.
 *
 * @param[in]     FileCode    File code(define in FILECODE.h) mix with assert Line num.
 *
 * @retval         TRUE     No error
**/
BOOLEAN
IdsAssert (
  IN      UINT32 FileCode
  );

/**
 * The engine code for ASSERT MACRO
 *
 * Halt execution with stop code display.  Stop Code is displayed on port 80, with rotation so that
 * it is visible on 8, 16, or 32 bit display.  The stop code is alternated with 0xDEAD on the display,
 * to help distinguish the stop code from a post code loop.
 * Additional features may be available if using simulation.
 *
 * @param[in]     FileCode    File code(define in FILECODE.h) mix with assert Line num.
 *
 */
BOOLEAN
IdsErrorStop (
  IN      UINT32 FileCode
  );

VOID
IdsDelay (
  VOID
);

BOOLEAN
AmdHeapIntactCheck (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
IdsCarCorruptionCheck (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

IDS_STATUS
IdsExceptionTrap (
  IN       IDS_IDT_FUNC_ID IdsIdtFuncId,
  IN       VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
IdsPerfTimestamp (
  IN       UINT32 LineInFile,
  IN       UINT32 Description,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
IdsPerfAnalyseTimestamp (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );


#if IDSOPT_IDS_ENABLED == FALSE
  #undef IEM_SKIP_CODE
  #undef IEM_INSERT_CODE
#endif
#ifndef IEM_SKIP_CODE
  #define IEM_SKIP_CODE(L)
#endif
#ifndef IEM_INSERT_CODE
  #define IEM_INSERT_CODE(L, Fn, Parm)
#endif

#endif // _IDS_H_
