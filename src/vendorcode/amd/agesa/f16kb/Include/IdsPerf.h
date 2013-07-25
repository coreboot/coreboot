/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug Routines for performance analysis
 *
 * Contains AMD AGESA debug macros and functions for performance analysis
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 */
/*****************************************************************************
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
#ifndef _IDS_PERFORMANCE_DATA_POINT

  #define _IDS_PERFORMANCE_DATA_POINT
  #define IDS_PERF_VERSION 0x00010000ul  //version number 0.1.0.0
/// Time points performance function used
  typedef enum {
    TP_BEGINPROCAMDINITEARLY             = 0x100,  ///< BeginProcAmdInitEarly
    TP_ENDPROCAMDINITEARLY               = 0x101,  ///< EndProcAmdInitEarly
    TP_BEGINAMDHTINITIALIZE              = 0x102,  ///< BeginAmdHtInitialize
    TP_ENDAMDHTINITIALIZE                = 0x103,  ///< EndAmdHtInitialize
    TP_BEGINGNBINITATEARLIER             = 0x104,  ///< BeginGnbInitAtEarlier
    TP_ENDGNBINITATEARLIER               = 0x105,  ///< EndGnbInitAtEarlier
    TP_BEGINAMDCPUEARLY                  = 0x106,  ///< BeginAmdCpuEarly
    TP_ENDAMDCPUEARLY                    = 0x107,  ///< EndAmdCpuEarly
    TP_BEGINGNBINITATEARLY               = 0x108,  ///< BeginGnbInitAtEarly
    TP_ENDGNBINITATEARLY                 = 0x109,  ///< EndGnbInitAtEarly
    TP_BEGINPROCAMDINITENV               = 0x10A,  ///< BeginProcAmdInitEnv
    TP_ENDPROCAMDINITENV                 = 0x10B,  ///< EndProcAmdInitEnv
    TP_BEGININITENV                      = 0x10C,  ///< BeginInitEnv
    TP_ENDINITENV                        = 0x10D,  ///< EndInitEnv
    TP_BEGINGNBINITATENV                 = 0x10E,  ///< BeginGnbInitAtEnv
    TP_ENDGNBINITATENV                   = 0x10F,  ///< EndGnbInitAtEnv
    TP_BEGINPROCAMDINITLATE              = 0x110,  ///< BeginProcAmdInitLate
    TP_ENDPROCAMDINITLATE                = 0x111,  ///< EndProcAmdInitLate
    TP_BEGINCREATSYSTEMTABLE             = 0x112,  ///< BeginCreatSystemTable
    TP_ENDCREATSYSTEMTABLE               = 0x113,  ///< EndCreatSystemTable
    TP_BEGINDISPATCHCPUFEATURESLATE      = 0x114,  ///< BeginDispatchCpuFeaturesLate
    TP_ENDDISPATCHCPUFEATURESLATE        = 0x115,  ///< EndDispatchCpuFeaturesLate
    TP_BEGINAMDCPULATE                   = 0x116,  ///< BeginAmdCpuLate
    TP_ENDAMDCPULATE                     = 0x117,  ///< EndAmdCpuLate
    TP_BEGINGNBINITATLATE                = 0x118,  ///< BeginGnbInitAtLate
    TP_ENDGNBINITATLATE                  = 0x119,  ///< EndGnbInitAtLate
    TP_BEGINPROCAMDINITMID               = 0x11A,  ///< BeginProcAmdInitMid
    TP_ENDPROCAMDINITMID                 = 0x11B,  ///< EndProcAmdInitMid
    TP_BEGINDISPATCHCPUFEATURESMID       = 0x11C,  ///< BeginDispatchCpuFeaturesMid
    TP_ENDDISPATCHCPUFEATURESMID         = 0x11D,  ///< EndDispatchCpuFeaturesMid
    TP_BEGININITMID                      = 0x11E,  ///< BeginInitMid
    TP_ENDINITMID                        = 0x11F,  ///< EndInitMid
    TP_BEGINGNBINITATMID                 = 0x120,  ///< BeginGnbInitAtMid
    TP_ENDGNBINITATMID                   = 0x121,  ///< EndGnbInitAtMid
    TP_BEGINPROCAMDINITPOST              = 0x122,  ///< BeginProcAmdInitPost
    TP_ENDPROCAMDINITPOST                = 0x123,  ///< EndProcAmdInitPost
    TP_BEGINGNBINITATPOST                = 0x124,  ///< BeginGnbInitAtPost
    TP_ENDGNBINITATPOST                  = 0x125,  ///< EndGnbInitAtPost
    TP_BEGINAMDMEMAUTO                   = 0x126,  ///< BeginAmdMemAuto
    TP_ENDAMDMEMAUTO                     = 0x127,  ///< EndAmdMemAuto
    TP_BEGINAMDCPUPOST                   = 0x128,  ///< BeginAmdCpuPost
    TP_ENDAMDCPUPOST                     = 0x129,  ///< EndAmdCpuPost
    TP_BEGINGNBINITATPOSTAFTERDRAM       = 0x12A,  ///< BeginGnbInitAtPostAfterDram
    TP_ENDGNBINITATPOSTAFTERDRAM         = 0x12B,  ///< EndGnbInitAtPostAfterDram
    TP_BEGINPROCAMDINITRESET             = 0x12C,  ///< BeginProcAmdInitReset
    TP_ENDPROCAMDINITRESET               = 0x12D,  ///< EndProcAmdInitReset
    TP_BEGININITRESET                    = 0x12E,  ///< BeginInitReset
    TP_ENDINITRESET                      = 0x12F,  ///< EndInitReset
    TP_BEGINHTINITRESET                  = 0x130,  ///< BeginHtInitReset
    TP_ENDHTINITRESET                    = 0x131,  ///< EndHtInitReset
    TP_BEGINPROCAMDINITRESUME            = 0x132,  ///< BeginProcAmdInitResume
    TP_ENDPROCAMDINITRESUME              = 0x133,  ///< EndProcAmdInitResume
    TP_BEGINAMDMEMS3RESUME               = 0x134,  ///< BeginAmdMemS3Resume
    TP_ENDAMDMEMS3RESUME                 = 0x135,  ///< EndAmdMemS3Resume
    TP_BEGINDISPATCHCPUFEATURESS3RESUME  = 0x136,  ///< BeginDispatchCpuFeaturesS3Resume
    TP_ENDDISPATCHCPUFEATURESS3RESUME    = 0x137,  ///< EndDispatchCpuFeaturesS3Resume
    TP_BEGINSETCORESTSCFREQSEL           = 0x138,  ///< BeginSetCoresTscFreqSel
    TP_ENDSETCORESTSCFREQSEL             = 0x139,  ///< EndSetCoresTscFreqSel
    TP_BEGINMEMFMCTMEMCLR_INIT           = 0x13A,  ///< BeginMemFMctMemClr_Init
    TP_ENDNMEMFMCTMEMCLR_INIT            = 0x13B,  ///< EndnMemFMctMemClr_Init
    TP_BEGINMEMBEFOREMEMDATAINIT         = 0x13C,  ///< BeginMemBeforeMemDataInit
    TP_ENDMEMBEFOREMEMDATAINIT           = 0x13D,  ///< EndMemBeforeMemDataInit
    TP_BEGINPROCAMDMEMAUTO               = 0x13E,  ///< BeginProcAmdMemAuto
    TP_ENDPROCAMDMEMAUTO                 = 0x13F,  ///< EndProcAmdMemAuto
    TP_BEGINMEMMFLOWC32                  = 0x140,  ///< BeginMemMFlowC32
    TP_ENDMEMMFLOWC32                    = 0x141,  ///< EndMemMFlowC32
    TP_BEGINMEMINITIALIZEMCT             = 0x142,  ///< BeginMemInitializeMCT
    TP_ENDMEMINITIALIZEMCT               = 0x143,  ///< EndMemInitializeMCT
    TP_BEGINMEMSYSTEMMEMORYMAPPING       = 0x144,  ///< BeginMemSystemMemoryMapping
    TP_ENDMEMSYSTEMMEMORYMAPPING         = 0x145,  ///< EndMemSystemMemoryMapping
    TP_BEGINMEMDRAMTRAINING              = 0x146,  ///< BeginMemDramTraining
    TP_ENDMEMDRAMTRAINING                = 0x147,  ///< EndMemDramTraining
    TP_BEGINMEMOTHERTIMING               = 0x148,  ///< BeginMemOtherTiming
    TP_ENDMEMOTHERTIMING                 = 0x149,  ///< EndMemOtherTiming
    TP_BEGINMEMUMAMEMTYPING              = 0x14A,  ///< BeginMemUMAMemTyping
    TP_ENDMEMUMAMEMTYPING                = 0x14B,  ///< EndMemUMAMemTyping
    TP_BEGINMEMMEMCLR                    = 0x14C,  ///< BeginMemMemClr
    TP_ENDMEMMEMCLR                      = 0x14D,  ///< EndMemMemClr
    TP_BEGINMEMMFLOWTN                   = 0x14E,  ///< BeginMemMFlowTN
    TP_ENDMEMMFLOWTN                     = 0x14F,  ///< EndMemMFlowTN
    TP_BEGINAGESAHOOKBEFOREDRAMINIT      = 0x150,  ///< BeginAgesaHookBeforeDramInit
    TP_ENDAGESAHOOKBEFOREDRAMINIT        = 0x151,  ///< EndAgesaHookBeforeDramInit
    TP_BEGINPROCMEMDRAMTRAINING          = 0x152,  ///< BeginProcMemDramTraining
    TP_ENDPROCMEMDRAMTRAINING            = 0x153,  ///< EndProcMemDramTraining
    TP_BEGINGNBINITATS3SAVE              = 0x154,  ///< BeginGnbInitAtS3Save
    TP_ENDGNBINITATS3SAVE                = 0x155,  ///< EndGnbInitAtS3Save
    TP_BEGINGNBLOADSCSDATA               = 0x156,  ///< BeginGnbLoadScsData
    TP_ENDGNBLOADSCSDATA                 = 0x157,  ///< EndGnbLoadScsData
    TP_BEGINGNBPCIETRAINING              = 0x158,  ///< BeginGnbPcieTraining
    TP_ENDGNBPCIETRAINING                = 0x159,  ///< EndGnbPcieTraining
    IDS_TP_END                                     ///< End of IDS TP list
  } IDS_PERF_DATA;
#endif
