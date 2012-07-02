/* $NoKeywords:$ */
/**
 * @file
 *
 * mu.h
 *
 * Utility support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
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
  * ***************************************************************************
  *
 */

#ifndef _MU_H_
#define _MU_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */
#ifndef PSO_ENTRY
  #define PSO_ENTRY UINT8
#endif

//
// Maximum value macro
//
#ifndef MAX
  #define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))
#endif
//
// Minimum Value macro
//
#ifndef MIN
  #define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif
//
// Absolute Value Macro
//
#ifndef ABS
  #define ABS(X) (((X) < 0) ? (-(X)) : (X))
#endif
//
// Taking ceiling of (a / b)
//
#define CEIL_DIV(a, b)  (((a) + (b) - 1) / (b))
//
// Check if value x is a power of 2 or not
//
#define IS_POWER_OF_2(x)  (((x) & ((x) - 1)) == 0)


/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// Test patterns for DQS training
typedef enum {
  TestPattern0,             ///< Test pattern used in first pass of receiver enable training
  TestPattern1,             ///< Test pattern used in first pass of receiver enable training
  TestPattern2,             ///< Test pattern used in second pass of receiver enable training
  TestPatternJD1B,          ///< 72-bit test pattern used in position training (ganged mode)
  TestPatternJD1A,          ///< 72-bit test pattern used in position training
  TestPatternJD256B,        ///< 256-bit test pattern used in position training (ganged mode)
  TestPatternJD256A,        ///< 256-bit test pattern used in position training
  TestPatternML,            ///< Test pattern used in first pass of max latency training
  TestPattern3,             ///< Test pattern used in first pass of receiver enable training
  TestPattern4              ///< Test pattern used in first pass of receiver enable training
} TRAIN_PATTERN;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

VOID
MemUWriteCachelines (
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemUReadCachelines (
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemUDummyCLRead (
  IN       UINT32 Address
  );

VOID
MemUFlushPattern (
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );


VOID
MemUFillTrainPattern (
  IN       TRAIN_PATTERN Pattern,
  IN       UINT8 Buffer[],
  IN       UINT16 Size
  );

UINT32
MemUSetUpperFSbase (
  IN       UINT32 Address,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemUSetTargetWTIO (
  IN       UINT32 Address,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemUResetTargetWTIO (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemUProcIOClFlush (
  IN       UINT32 Address,
  IN       UINT16 ClCount,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemUWait10ns (
  IN       UINT32 Count,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemUGetWrLvNblErr (
  IN OUT   UINT16 *ErrBitmap,
  IN       UINT32 TestAddr,
  IN       UINT16 ClCount
  );

VOID
AlignPointerTo16Byte (
  IN OUT   UINT8 **BufferPtrPtr
  );

VOID *
FindPSOverrideEntry (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       PSO_ENTRY EntryType,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID,
  IN       UINT8 DimmID,
  IN       CPU_LOGICAL_ID *LogicalIdPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT8
GetMaxDimmsPerChannel (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN UINT8 SocketID,
  IN UINT8 ChannelID
  );

UINT8
GetMaxSolderedDownDimmsPerChannel (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID
  );

UINT8
GetMaxChannelsPerSocket (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN UINT8 SocketID,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT8
GetMaxCSPerChannel (
  IN      PSO_TABLE *PlatformMemoryConfiguration,
  IN      UINT8 SocketID,
  IN      UINT8 ChannelID
  );

UINT8
GetSpdSocketIndex (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN UINT8 SocketID,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT8
GetSpdChannelIndex (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN UINT8 SocketID,
  IN UINT8 ChannelID,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
GetVarMtrrHiMsk (
  IN       CPU_LOGICAL_ID *LogicalIdPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
MemUMFenceInstr (VOID
  );

UINT32
MemUnsToMemClk (
  IN       MEMORY_BUS_SPEED Speed,
  IN       UINT32 NumberOfns
  );
#endif  /* _MU_H_ */


