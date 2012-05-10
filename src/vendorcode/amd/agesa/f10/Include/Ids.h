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
 * @e \$Revision:  $   @e \$Date: 2008-04-07 15:08:45 -0500 (Mon, 07 Apr 2008) $
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
#define IDS_SUCCESS         ((IDS_STATUS) 0x00000000) ///< IDS Function is Successful.
#define IDS_UNSUPPORTED     ((IDS_STATUS) 0xFFFFFFFF) ///< IDS Function is not existed.

#define IDS_STRINGIZE(a) #a ///< for define stringize macro
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
                                          ///<   ACPI SRAT Table
                                          ///<   ACPI SLIT Table
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
  IDS_AFTER_WARM_RESET ,              ///< 12 Override PCI or MSR Registers Before Warm Reset
  IDS_BEFORE_MEM_INIT,                ///< 13 Override PCI or MSR Registers Before Memory Init
  IDS_BEFORE_PCI_INIT,                ///< 14 Override PCI or MSR Registers Before PCI Init
  IDS_BEFORE_OS,                      ///< 15 Override PCI or MSR Registers Before booting to OS
  IDS_UCODE,                          ///< 16 Enable or Disable microcode patching

  IDS_PLATFORM_RSVD1 = 0x38,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD2 = 0x39,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD3 = 0x3a,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD4 = 0x3b,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD5 = 0x3c,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD6 = 0x3d,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD7 = 0x3e,          ///< from 0x38 to 0x3f will reserved for platform used
  IDS_PLATFORM_RSVD8 = 0x3f,          ///< from 0x38 to 0x3f will reserved for platform used

  // All the above timing point is used by BVM, their value should never be changed
  IDS_HT_CONTROL,                     ///< 40 Override the setting of HT Link Control
  IDS_HT_TRISTATE,                    ///< 41 Enable or Disable HT Tri-state during an LDTSTP#
  IDS_INIT_DRAM_TABLE,                ///< 42 Generate override table for Dram Timing
                                          ///< Dram Controller, Drive Strength and DQS Timing
  IDS_GET_DRAM_TABLE,                 ///< 43 Generate override table for Dram Timing
  IDS_GANGING_MODE,                   ///< 44 override Memory Mode Unganged
  IDS_POWERDOWN_MODE,                 ///< 45 override Power Down Mode
  IDS_BURST_LENGTH32,                 ///< 46 override Burst Length32
  IDS_ALL_MEMORY_CLOCK,               ///< 47 override All Memory Clks Enable
  IDS_ECC,                            ///< 48 override ECC parameter
  IDS_ECCSYMBOLSIZE,                  ///< 49 override ECC symbol size
  IDS_CPU_Early_Override,             ///< 4a override CPU early parameter
  IDS_CACHE_FLUSH_HLT,                ///< 4b override Cache Flush Hlt
  IDS_CHANNEL_INTERLEAVE,             ///< 4c override Channel Interleave
  IDS_MEM_ERROR_RECOVERY,             ///< 4d override memory error recovery
  IDS_MEM_RETRAIN_TIMES,              ///< 4e override memory retrain times
  IDS_MEM_SIZE_OVERLAY,               ///< 4f Override the syslimit
  IDS_HT_ASSIST,                      ///< 50 Override Probe Filter
  IDS_CHECK_NEGATIVE_WL,              ///< 51 Check for negative write leveling result
  IDS_DLL_SHUT_DOWN,                  ///< 52 Check for Dll Shut Down
  IDS_POR_MEM_FREQ,                   ///< 53 Entry to enable/disable MemClk frequency enforcement
  IDS_PHY_DLL_STANDBY_CNTRL,          ///< 54 Enable/Disable Phy DLL standby feature
  IDS_PLATFORMCFG_OVERRIDE,           ///< 55 Hook for Override PlatformConfig structure
  IDS_LOADCARD_ERROR_RECOVERY,        ///< 56 Special error handling for load card support
  IDS_MEM_IGNORE_ERROR,                ///< 57 Ignore error and do not do fatal exit in memory
  IDS_GNB_PPFUSE_OVERRIDE,         ///< 58 override GNB default PowerPlay fuse table
  IDS_GNB_INTEGRATED_TABLE_CONFIG, ///<59 override GNB integrated table config
  IDS_GNB_SMU_SERVICE_CONFIG,          ///< 5a Config GNB SMU service
  IDS_GNB_PCIE_PLATFORM_CONFIG,      ///< 5b Config PCIE platform
  IDS_GNB_ORBDYNAMIC_WAKE,                   ///< 5c config GNB dynamic wake
  IDS_GNB_PLATFORMCFG_OVERRIDE   ///< 5d override ids gnb platform config
} AGESA_IDS_OPTION;

#include "OptionsIds.h"
#include "Filecode.h"

/* Initialize IDS controls */
#ifndef IDSOPT_IDS_ENABLED
  #define IDSOPT_IDS_ENABLED        FALSE
#endif

#ifndef IDSOPT_CONTROL_ENABLED
  #define IDSOPT_CONTROL_ENABLED    FALSE
#endif

#ifndef IDSOPT_TRACING_ENABLED
  #define IDSOPT_TRACING_ENABLED    FALSE
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

#ifndef IDSOPT_ERROR_TRAP_ENABLED
  #define IDSOPT_ERROR_TRAP_ENABLED   FALSE
#endif

#ifndef IDSOPT_CAR_CORRUPTION_CHECK_ENABLED
  #define IDSOPT_CAR_CORRUPTION_CHECK_ENABLED   FALSE
#endif

#if IDSOPT_IDS_ENABLED == FALSE
  #undef  IDSOPT_CONTROL_ENABLED
  #undef  IDSOPT_TRACING_ENABLED
  #undef  IDSOPT_PERF_ANALYSIS
  #undef  IDSOPT_HEAP_CHECKING
  #undef  IDSOPT_ASSERT_ENABLED
  #undef  IDSOPT_ERROR_TRAP_ENABLED
  #undef  IDSOPT_CAR_CORRUPTION_CHECK_ENABLED

  #define IDSOPT_CONTROL_ENABLED    FALSE
  #define IDSOPT_TRACING_ENABLED    FALSE
  #define IDSOPT_PERF_ANALYSIS      FALSE
  #define IDSOPT_HEAP_CHECKING      FALSE
  #define IDSOPT_ASSERT_ENABLED     FALSE
  #define IDSOPT_ERROR_TRAP_ENABLED FALSE
  #define IDSOPT_CAR_CORRUPTION_CHECK_ENABLED FALSE
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
  #define STOP_CODE (((UINT32)FILECODE)*0x10000 + \
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
    #define STOP_CODE (((UINT32)FILECODE)*0x10000 + \
                        ((__LINE__) % 10) + (((__LINE__ / 10) % 10)*0x10) + \
                        (((__LINE__ / 100) % 10)*0x100) + (((__LINE__ / 1000) % 10)*0x1000))

    #define ASSERT(conditional) ((conditional) ? 0 : IdsErrorStop (STOP_CODE));
  #else
    #define ASSERT(conditional)
  #endif
#endif
#if IDSOPT_CAR_CORRUPTION_CHECK_ENABLED == TRUE
  #undef  IDSOPT_ERROR_TRAP_ENABLED
  #define IDSOPT_ERROR_TRAP_ENABLED TRUE
  #define IDS_CAR_CORRUPTION_CHECK(StdHeader)  //IdsCarCorruptionCheck(StdHeader)
#else
  #define IDS_CAR_CORRUPTION_CHECK(StdHeader)
#endif

/**
 * @def IDS_ERROR_TRAP
 * Trap AGESA Error events with stop code display.
 *
 * Works similarly to use of "ASSERT (FALSE);"
 *
 */
#if IDSOPT_ERROR_TRAP_ENABLED == TRUE
  #ifdef STOP_CODE
    #undef STOP_CODE
  #endif
  #define STOP_CODE (((UINT32)FILECODE)*0x10000 + \
                      ((__LINE__) % 10) + (((__LINE__ / 10) % 10)*0x10) + \
                      (((__LINE__ / 100) % 10)*0x100) + (((__LINE__ / 1000) % 10)*0x1000))

  #define IDS_ERROR_TRAP  IdsErrorStop (STOP_CODE)
#else
  #define IDS_ERROR_TRAP
#endif

///give the extended Macro default value
#ifndef __IDS_EXTENDED__
  #define IDS_EXTENDED_HOOK(idsoption, dataptr, idsnvptr, stdheader) IDS_SUCCESS
  #define IDS_INITIAL_F10_PM_STEP
  #define IDS_EXTENDED_GET_DATA_EARLY(data, StdHeader)
  #define IDS_EXTENDED_GET_DATA_LATE(data, StdHeader)
  #define IDS_EXTENDED_HEAP_SIZE 0
  #define IDS_EXT_INCLUDE_F10(file)
  #define IDS_EXT_INCLUDE(file)
#endif

#ifndef IDS_NUM_NV_ITEM
  #define IDS_NUM_NV_ITEM (IDS_NUM_EXT_NV_ITEM)
#endif

#if IDSOPT_CONTROL_ENABLED == TRUE
  #define IDS_OPTION_HOOK(IdsOption, DataPtr, StdHeader) \
          AmdIdsCtrlDispatcher ((IdsOption), (DataPtr), (StdHeader))
#else
  #define IDS_OPTION_HOOK(IdsOption, DataPtr, StdHeader)
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
#if IDSOPT_IDS_ENABLED == TRUE
  #if IDSOPT_HEAP_CHECKING == TRUE
    #define SIZE_OF_SENTINEL 4
    #define SENTINEL_BEFORE_VALUE 0xCCAA5533
    #define SENTINEL_AFTER_VALUE 0x3355AACC
    #define SET_SENTINEL_BEFORE(NodePtr) (*(UINT32 *) ((UINT8 *) NodePtr + sizeof (BUFFER_NODE)) = SENTINEL_BEFORE_VALUE);
    #define SET_SENTINEL_AFTER(NodePtr) (*(UINT32 *) ((UINT8 *) NodePtr + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL + NodePtr->BufferSize) = SENTINEL_AFTER_VALUE);
    #define Heap_Check(stdheader) AmdHeapIntactCheck(stdheader)
  #else
    #define SIZE_OF_SENTINEL 0
    #define SENTINEL_BEFORE_VALUE 0xCCAA5533
    #define SENTINEL_AFTER_VALUE 0x3355AACC
    #define SET_SENTINEL_BEFORE(NodePtr)
    #define SET_SENTINEL_AFTER(NodePtr)
    #define Heap_Check(stdheader)
  #endif
#else
  #define SIZE_OF_SENTINEL 0
  #define SENTINEL_BEFORE_VALUE 0xCCAA5533
  #define SENTINEL_AFTER_VALUE 0x3355AACC
  #define SET_SENTINEL_BEFORE(NodePtr)
  #define SET_SENTINEL_AFTER(NodePtr)
  #define Heap_Check(stdheader)
#endif

/**
 *  Macro to add HDT OUT in memory code.
 *
 *  The default minimal action is to do nothing and there is no any code to increase.
 *  For debug environments, the debug information can be displayed in HDT or other
 *  devices.
 *
 *  @param[in]       Command    HDT_OUT command to control the communication between
 *                              HDT and memory initialization code.
 *  @param[in]       Data       Data to be sent to HDT.
 *  @param[in]       Condition  Allow the Data to be output if TRUE.
 *  @param[in]       NvId       NV ID to control this HDT OUT.
 *  @param[in,out]   StdHeader  The Pointer of AGESA Header.
 *
 **/
#define OPTION_NON_CONSOLE      0xD0
#define OPTION_HDT_CONSOLE      0xD1
#define OPTION_IDS_CONSOLE      OPTION_NON_CONSOLE
#if IDSOPT_IDS_ENABLED == TRUE
  #if IDSOPT_TRACING_ENABLED == TRUE
    #undef  OPTION_IDS_CONSOLE
    #define OPTION_IDS_CONSOLE       OPTION_HDT_CONSOLE
    #define IDS_FUNCLIST_ADDR        ScriptFuncList
    #define IDS_FUNCLIST_EXTERN()    extern SCRIPT_FUNCTION ScriptFuncList[]
    #define IDS_TIMEOUT_CTL(t)       IdsMemTimeOut (t)

    #define IDS_HDT_CONSOLE_INIT(x)
    #define IDS_HDT_CONSOLE_EXIT(x)
    #ifdef __GNUC__
      #if CONFIG_REDIRECT_IDS_HDT_CONSOLE_TO_SERIAL
        #define IDS_HDT_CONSOLE(s, args...) do_printk(BIOS_DEBUG, s, ##args)
        ///#define IDS_HDT_CONSOLE(f, s, args...) printk(((MEM_FLOW) - (f) + (BIOS_DEBUG)), s, ##args)
      #else
        #define IDS_HDT_CONSOLE(s, args...)
      #endif
    #else
     #define IDS_HDT_CONSOLE(s, args...)
    #endif
  #else
    #define IDS_HDT_CONSOLE_INIT(x)
    #define IDS_HDT_CONSOLE_EXIT(x)
    #define IDS_HDT_CONSOLE          1 ? (VOID) 0 : AmdIdsConsolePrint
    #define IDS_FUNCLIST_ADDR        NULL
    #define IDS_FUNCLIST_EXTERN()
    #define IDS_TIMEOUT_CTL(t)
  #endif
#else
  #define IDS_HDT_CONSOLE_INIT(x)
  #define IDS_HDT_CONSOLE_EXIT(x)
  #define IDS_HDT_CONSOLE          1 ? (VOID) 0 : AmdIdsConsolePrint
  #define IDS_FUNCLIST_ADDR        NULL
  #define IDS_FUNCLIST_EXTERN()
  #define IDS_TIMEOUT_CTL(t)
#endif

#if IDSOPT_PERF_ANALYSIS == TRUE
  #define IDS_PERF_TIMESTAMP(StdHeader, TestPoint)  //IdsPerfTimestamp ((StdHeader), (TestPoint))
  #define IDS_PERF_ANALYSE(StdHeader)   //IdsPerfAnalyseTimestamp (StdHeader)
  #define IDS_PERF_TIME_MEASURE(StdHeader)   //AGESA_TESTPOINT (TpPerfUnit, StdHeader); AGESA_TESTPOINT (TpPerfUnit, StdHeader);
#else
  #define IDS_PERF_TIMESTAMP(StdHeader, TestPoint)
  #define IDS_PERF_ANALYSE(StdHeader)
  #define IDS_PERF_TIME_MEASURE(StdHeader)
#endif

///For IDS feat use
#define IDS_FAMILY_ALL  0xFFFFFFFFFFFFFFFFull
#define IDS_BSP_ONLY    TRUE
#define IDS_ALL_CORES   FALSE

#define IDS_LATE_RUN_AP_TASK_ID     PROC_IDS_CONTROL_IDSLIB_FILECODE

#define IDS_CALLOUT_INIT      1       ///< The function data of IDS callout function of initialization.

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
  TpProcMemWlDimmConfig,              ///< 17 .. Target DIMM configured
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
  TpProcMemBeforeAgesaHookBeforeDramInit,     ///< 40 .. Before optional callout "AgesaHookBeforeDramInit"
  TpProcMemAfterAgesaHookBeforeDramInit,      ///< 41 .. After optional callout "AgesaHookBeforeDramInit"
  TpProcMemBeforeAgesaHookBeforeDQSTraining, ///< 42 .. Before optional callout "AgesaHookBeforeDQSTraining"
  TpProcMemAfterAgesaHookBeforeDQSTraining,  ///< 43 .. After optional callout "AgesaHookBeforeDQSTraining"
  TpProcMemBeforeAgesaHookBeforeExitSelfRef,     ///< 44 .. Before optional callout "AgesaHookBeforeDramInit"
  TpProcMemAfterAgesaHookBeforeExitSelfRef,      ///< 45 .. After optional callout "AgesaHookBeforeDramInit"
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
  TpProcCpuEntryPstate,               ///< 52 .. Entry point CreateAcpiTables
  TpProcCpuEntryPstateLeveling,       ///< 53 .. Entry point PStateLeveling
  TpProcCpuEntryPstateGather,         ///< 54 .. Entry point PStateGatherData
  TpProcCpuEntryWhea,                 ///< 55 .. Entry point CreateAcpiWhea
  TpProcCpuEntrySrat,                 ///< 56 .. Entry point CreateAcpiSrat
  TpProcCpuEntrySlit,                 ///< 57 .. Entry point CreateAcpiSlit
  TpProcCpuProcessRegisterTables,     ///< 58 .. Register table processing
  TpProcCpuSetBrandID,                ///< 59 .. Set brand ID
  TpProcCpuLocalApicInit,             ///< 5A .. Initialize local APIC
  TpProcCpuLoadUcode,                 ///< 5B .. Load microcode patch
  TpProcCpuPowerMgmtInit,             ///< 5C .. Power Management table processing
  TpProcCpuEarlyFeatureInit,          ///< 5D .. Early feature dispatch point
  TpProcCpuCoreLeveling,              ///< 5E .. Core Leveling
  TpProcCpuApMtrrSync,                ///< 5F .. AP MTRR sync up
  TpProcCpuPostFeatureInit,           ///< 60 .. POST feature dispatch point
  TpProcCpuFeatureLeveling,           ///< 61 .. CPU Feature Leveling
  TpProcCpuBeforeAllocateWheaBuffer,  ///< 62 .. Before the WHEA init code calls out to allocate a buffer
  TpProcCpuAfterAllocateWheaBuffer,   ///< 63 .. After the WHEA init code calls out to allocate a buffer
  TpProcCpuBeforeAllocateSratBuffer,  ///< 64 .. Before the SRAT init code calls out to allocate a buffer
  TpProcCpuAfterAllocateSratBuffer,   ///< 65 .. After the SRAT init code calls out to allocate a buffer
  TpProcCpuBeforeLocateSsdtBuffer,    ///< 66 .. Before the P-state init code calls out to locate a buffer
  TpProcCpuAfterLocateSsdtBuffer,     ///< 67 .. After the P-state init code calls out to locate a buffer
  TpProcCpuBeforeAllocateSsdtBuffer,  ///< 68 .. Before the P-state init code calls out to allocate a buffer
  TpProcCpuAfterAllocateSsdtBuffer,   ///< 69 .. After the P-state init code calls out to allocate a buffer

  // HT test points
  TpProcHtEntry = 0x71,               ///< 71 .. Coherent Discovery begin (Public interface)
  TpProcHtTopology,                   ///< 72 .. Topology match, routing, begin
  TpProcHtManualNc,                   ///< 73 .. Manual Non-coherent Init begin
  TpProcHtAutoNc,                     ///< 74 .. Automatic Non-coherent init begin
  TpProcHtOptGather,                  ///< 75 .. Optimization: Gather begin
  TpProcHtOptRegang,                  ///< 76 .. Optimization: Regang begin
  TpProcHtOptLinks,                   ///< 77 .. Optimization: Link Begin
  TpProcHtOptSubLinks,                ///< 78 .. Optimization: Sublinks begin
  TpProcHtOptFinish,                  ///< 79 .. Optimization: Set begin
  TpProcHtTrafficDist,                ///< 7A .. Traffic Distribution begin
  TpProcHtTuning,                     ///< 7B .. Misc Tuning Begin
  TpProcHtDone,                       ///< 7C .. HT Init complete
  TpProcHtApMapEntry,                 ///< 7D .. AP HT: Init Maps begin
  TpProcHtApMapDone,                  ///< 7E .. AP HT: Complete

  StartNbTestPoints = 0x90,           ///< 90 Entry used for range testing for @b NorthBridge related TPs
  TpNbxxx,                            ///< 91 .
  EndNbTestPoints,                    ///< 92 End of TP range for NB

  StartSbTestPoints = 0xB0,           ///< B0 Entry used for range testing for @b SouthBridge related TPs
  TpSbxxx,                            ///< B1 .
  EndSbTestPoints,                    ///< B2 End of TP range for SB

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
  TpIfAmdS3SaveEntry,                 ///< CE .. Entry to AmdS3Save
  TpIfAmdS3SaveExit,                  ///< CF .. Exiting from AmdS3Save
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
  TpPerfUnit,                              ///< F8 .. The Unit of performance measure.
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
  IDS_FEAT_DCT_GANGMODE,            ///< Feat for Dct gang mode
  IDS_FEAT_DCT_BURSTLENGTH,         ///< Feat for dct burst length
  IDS_FEAT_DCT_POWERDOWN,           ///< Feat for dct power down
  IDS_FEAT_DCT_DLLSHUTDOWN,         ///< Feat for dct dll shut down
  IDS_FEAT_PROBE_FILTER,            ///< Feat for probe filter
  IDS_FEAT_HDTOUT,                  ///< Feat for hdt out
  IDS_FEAT_HT_SETTING,              ///< Feat for Ht setting
  IDS_FEAT_GNB_PLATFORMCFG,                 ///< Feat for override GNB platform config
  IDS_FEAT_END                      ///< End of Common feat
} IDS_FEAT;

typedef IDS_STATUS IDS_COMMON_FUNC (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  );

typedef IDS_COMMON_FUNC *PIDS_COMMON_FUNC;

/// Data Structure of IDS Feature block
typedef  struct _IDS_FEAT_STRUCT {
  IDS_FEAT  IdsFeat;      ///< Ids Feat ID
  BOOLEAN IsBsp;          ///< swith for Bsp check
  AGESA_IDS_OPTION IdsOption;   ///< IDS option
  UINT64 CpuFamily;      ///<
  PIDS_COMMON_FUNC  pf_idsoption;     ///<pointer to function
} IDS_FEAT_STRUCT;


/// Data Structure of IDS option
typedef  struct _IDS_OPTION_STRUCT {
  AGESA_IDS_OPTION idsoption;   ///< IDS option
  PIDS_COMMON_FUNC  pf_idsoption;     ///<pointer to function
} IDS_OPTION_STRUCT;

/// Data Structure of IDS option table
typedef struct _IDS_OPTION_STRUCT_TBL {
  UINT8 version;      ///<Version of IDS option table
  UINT16 size;        ///<Size of IDS option table
  CONST IDS_OPTION_STRUCT  *pIdsOptionStruct; ///<pointer to array of  structure
} IDS_OPTION_STRUCT_TBL;

IDS_STATUS
AmdIdsCtrlDispatcher (
  IN       AGESA_IDS_OPTION IdsOption,
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
AmdIdsConsoleInit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
AmdIdsConsolePrint (
  IN      CHAR8 *Format,
  IN      ...
  );

VOID
AmdIdsConsoleExit (
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
  IN	VOID
);

BOOLEAN
AmdHeapIntactCheck (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
IdsCarCorruptionCheck (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

#endif // _IDS_H_
