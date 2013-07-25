/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains all functions related to HDTOUT
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
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

#ifndef _IDS_HDTOUT_H_
#define _IDS_HDTOUT_H_

#define HDTOUT_VERSION          0x0200

/// HDTOUT command
#define HDTOUT_COMMAND        0x99cc
#define HDTOUT_INIT           (0x10BF0000ul | HDTOUT_COMMAND)
#define HDTOUT_ASSERT         (0xA0BF0000ul | HDTOUT_COMMAND)
#define HDTOUT_EXIT           (0xE0BF0000ul | HDTOUT_COMMAND)
#define HDTOUT_PRINT          (0xC0BF0000ul | HDTOUT_COMMAND)
#define HDTOUT_TIME_ANALYSE   (0xD0BF0000ul | HDTOUT_COMMAND)
#define HDTOUT_BREAKPOINT     (0xB0BF0000ul | HDTOUT_COMMAND)
#define HDTOUT_ERROR          (0x1EBF0000ul | HDTOUT_COMMAND)


#define HDTOUT_ERROR_HEAP_ALLOCATION       0x1
#define HDTOUT_ERROR_HEAP_AllOCATE_FAIL       0x2

#define HDTOUT_PRINTCTRL_OFF      0
#define HDTOUT_PRINTCTRL_ON       1
#define HDTOUT_ALL_CORES          0
#define HDTOUT_BSP_ONLY           1
#define HDTOUT_BUFFER_MODE_OFF    0
#define HDTOUT_BUFFER_MODE_ON     1

#define HDTOUT_HEADER_SIGNATURE     0xDB1099CCul
#define HDTOUT_DEFAULT_BUFFER_SIZE  0x1000
/// HDTOUT Header.
typedef struct _HDTOUT_HEADER {
  UINT32 Signature;               ///< 0xDB1099CC
  UINT16 Version;                 ///< HDTOUT version.
  UINT16 BufferSize;              ///< Size in bytes.
  UINT16 DataIndex;               ///< Data Index.
  UINT8  PrintCtrl;               ///< 0 off no print  1 on print
  UINT8  NumBreakpointUnit;       ///< default 0 no bp unit others number of bp unit
  UINT32 FuncListAddr;            ///< 32 bit address to the list of functions that script can execute
  UINT8  ConsoleType;             ///< Console type - deprecated
  UINT8  Event;                   ///< Event type. - deprecated
  UINT8  OutBufferMode;           ///< Off:stack mode, On: heap mode - deprecated
  UINT32 EnableMask;              ///< Bitmap to select which part should be streamed out
  UINT64 ConsoleFilter;           ///< Filter use to select which part should be streamed out
  UINT8  BspOnlyFlag;             ///< 1 Only Enable Bsp output, 0 enable On All cores
  UINT8  Reserved[56 - 32];       ///< Reserved for header expansion

  CHAR8  BreakpointList[300];     ///< Breakpoint list
  CHAR8  StatusStr[156];          ///< Shows current node, DCT, CS,...
  CHAR8  Data[2];                 ///< HDTOUT content. Its size will be determined by BufferSize.
} HDTOUT_HEADER;

#define IDS_HDTOUT_BP_AND_OFF             0
#define IDS_HDTOUT_BP_AND_ON              1

#define IDS_HDTOUT_BPFLAG_FORMAT_STR   0
#define IDS_HDTOUT_BPFLAG_STATUS_STR   1

#define HDTOUT_BP_ACTION_HALT          1
#define HDTOUT_BP_ACTION_PRINTON       2
#define HDTOUT_BP_ACTION_PRINTONE      3
#define HDTOUT_BP_ACTION_PRINTOFF      4

///breakpoint unit of HDTOUT
typedef struct _BREAKPOINT_UNIT {
  UINT8 AndFlag : 1;        ///< Next string is ANDed to current string
  UINT8 BpFlag : 1;         ///< Format string or Status string
  UINT8 Action : 4;         ///< Halt, start HDTOUT, or stop HDT,...
  UINT8 BpStrOffset;        ///< Offset from BreakpointList to the breakpoint string
} BREAKPOINT_UNIT;


BOOLEAN
AmdIdsHdtOutSupport (
  VOID
  );

#endif //_IDS_HDTOUT_H_

