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
 * @e \$Revision: 281181 $   @e \$Date: 2013-12-18 02:18:55 -0600 (Wed, 18 Dec 2013) $
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

#include <check_for_wrapper.h>

#ifndef _IDS_LIB_H_
#define _IDS_LIB_H_

/// Data Structure of Parameters for TestPoint_TSC.
typedef struct {
  UINT32 LineInFile;                    ///< Line of current time counter
  UINT64 Description;                  ///<Description ID
  UINT64 StartTsc;                  ///< The StartTimer of TestPoint_TSC
} TestPoint_TSC;

#define RESERVED_TP_NUMER 0x20
#define MAX_PERFORMANCE_UNIT_NUM (IDS_TP_END - TP_BEGINPROCAMDINITEARLY + 1 + RESERVED_TP_NUMER)
/// Data Structure of Parameters for TP_Perf_STRUCT.
typedef struct {
  UINT32 Signature;                ///< "PERF"
  UINT32 Version;       ///< version
  UINT32 Index;                    ///< The Index of TP_Perf_STRUCT
  UINT32 TscInMhz;            ///< Tsc counter in 1 mhz
  TestPoint_TSC TP[MAX_PERFORMANCE_UNIT_NUM];       ///< The TP of TP_Perf_STRUCT
} TP_Perf_STRUCT;

#endif //_IDS_LIB_H_
