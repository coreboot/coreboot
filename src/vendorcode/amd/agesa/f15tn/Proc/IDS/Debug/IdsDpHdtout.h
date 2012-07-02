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

