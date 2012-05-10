/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 */
/*****************************************************************************
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
 ******************************************************************************
 */

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "IdsLib.h"
#include "amdlib.h"
#include "AMD.h"
#include "heapManager.h"
#include "cpuRegisters.h"
#include "Filecode.h"
#define FILECODE PROC_IDS_DEBUG_IDSDEBUG_FILECODE

extern BUILD_OPT_CFG UserOptions;
typedef struct _IDS_CONSOLE IDS_CONSOLE;

/*--------------------------------------------------------------------------------------*/
/**
 *  IDS back-end code for AGESA_TESTPOINT
 *
 *  @param[in] TestPoint  Progress indicator value, see @ref AGESA_TP
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID
IdsAgesaTestPoint (
  IN       AGESA_TP      TestPoint,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoWrite (AccessWidth8, IDS_DEBUG_PORT, &TestPoint, StdHeader);
  IDS_PERF_TIMESTAMP (StdHeader, TestPoint);
  if (TestPoint == EndAgesaTps) {
    IDS_PERF_ANALYSE (StdHeader);
  }
}

#define DEBUG_PRINT_INIT        0x10BF0000
#define DEBUG_PRINT_EXIT        0xE0BF0000
#define DEBUG_PRINT_COMMAND     0xC0BF0000
#define DEBUG_PRINT_BREAKPOINT  0xB0BF0000
#define DEBUG_PRINT_EVENT       0x1EBF0000

#define IDS_HDTOUT_BPFLAG_FORMAT_STR   0
#define IDS_HDTOUT_BPFLAG_STATUS_STR   1

#define HDTOUT_BP_ACTION_HALT          1
#define HDTOUT_BP_ACTION_PRINTON       2
#define HDTOUT_BP_ACTION_PRINTONE      3
#define HDTOUT_BP_ACTION_PRINTOFF      4

typedef struct _BREAKPOINT_UNIT {
  UINT8 AndFlag : 1;        ///< Next string is ANDed to current string
  UINT8 BpFlag : 1;         ///< Format string or Status string
  UINT8 Action : 4;         ///< Halt, start HDTOUT, or stop HDT,...
  UINT8 BpStrOffset;        ///< Offset from BreakpointList to the breakpoint string
} BREAKPOINT_UNIT;

typedef enum {
  IDS_STATE_OFF = 0xF0,
  IDS_STATE_ON
} IDS_STATE_TYPE;

typedef enum {
  NON_CONSOLE = 0xD0,
  HDT_CONSOLE,
  IDS_CONSOLE_END
} IDS_CONSOLE_TYPE;

// IDS HdtOut Event Level
typedef enum {
  EVENT_OFF = 0x30,              ///< Default,no event triggered.
  EVENT_WARNING,                 ///< Event warning.
  EVENT_ERROR,                   ///< Event error.
  EVENT_FAIL_BUFFER_ALLOCATION,  ///< Reserved, fail buffer allocation
  EVENT_END                   ///< Event end sentinel.
} CONSOLE_EVENT_TYPE;

#define _INT_SIZE_OF(n) ((sizeof (n) + sizeof (UINTN) - 1) &~(sizeof (UINTN) - 1))

//
// Also support coding convention rules for var arg macros
//
typedef CHAR8 *VA_LIST;
#define VA_START(ap, v) (ap = (VA_LIST) & (v) + _INT_SIZE_OF (v))
#define VA_ARG(ap, t)   (*(t *) ((ap += _INT_SIZE_OF (t)) - _INT_SIZE_OF (t)))
#define VA_END(ap)      (ap = (VA_LIST) 0)

#define LEFT_JUSTIFY    0x01
#define PREFIX_SIGN     0x02
#define PREFIX_BLANK    0x04
#define COMMA_TYPE      0x08
#define LONG_TYPE       0x10
#define PREFIX_ZERO     0x20

/**
 *  Create console context
 *
 *  Do hardware settings related with specific console context
 *
 *  @param[in,out] IdsConsole    The Pointer of IDS console
 *
 **/
typedef VOID F_CREATE_CONSOLE_CONTEXT (
  IN OUT   IDS_CONSOLE *IdsConsole
  );
/// Reference to a method.
typedef F_CREATE_CONSOLE_CONTEXT *PF_CREATE_CONSOLE_CONTEXT;

/**
 *  Initialize console context
 *
 *  Initilize preference settings related with specific console context
 *
 *  @param[in,out] IdsConsole    The Pointer of IDS console
 *
 **/
typedef VOID F_INIT_CONSOLE_CONTEXT (
  IN OUT   IDS_CONSOLE *IdsConsole
  );
/// Reference to a method.
typedef F_INIT_CONSOLE_CONTEXT *PF_INIT_CONSOLE_CONTEXT;

/**
 *  Update console context
 *
 *  Update preference settings related with specific console context
 *
 *  @param[in,out] IdsConsole    The Pointer of IDS console
 *
 **/
typedef VOID F_UPDATE_CONSOLE_CONTEXT (
  IN OUT   IDS_CONSOLE *IdsConsole
  );
/// Reference to a method.
typedef F_UPDATE_CONSOLE_CONTEXT *PF_UPDATE_CONSOLE_CONTEXT;

/**
 *  Save console context
 *
 *  Save console context snapshot
 *
 *  @param[in,out] IdsConsole    The Pointer of IDS console
 *
 **/
typedef VOID F_SAVE_CONSOLE_CONTEXT (
  IN OUT   IDS_CONSOLE *IdsConsole
  );
/// Reference to a method.
typedef F_SAVE_CONSOLE_CONTEXT *PF_SAVE_CONSOLE_CONTEXT;

/**
 *  Destroy console context
 *
 *  Destroy console context snapshot
 *
 *  @param[in,out] IdsConsole    The Pointer of IDS console
 *
 **/
typedef VOID F_DESTROY_CONSOLE_CONTEXT (
  IN OUT   IDS_CONSOLE *IdsConsole
  );
/// Reference to a method.
typedef F_DESTROY_CONSOLE_CONTEXT *PF_DESTROY_CONSOLE_CONTEXT;

/**
 *  Print function
 *
 *  Print function related with specific console
 *
 *  @param[in] PrintType             Print Type
 *  @param[in] ConsoleBufferAddress  The address of console buffer
 *  @param[in] ConsoleBufferSize     The size of console buffer
 *  @param[in,out] IdsConsole        The Pointer of IDS console
 *
 **/
typedef VOID F_PRINT (
  IN       UINT32 PrintType,
  IN       UINT32 ConsoleBufferAddress,
  IN       UINT32 ConsoleBufferSize,
  IN OUT   IDS_CONSOLE *IdsConsole
  );
/// Reference to a method.
typedef F_PRINT *PF_PRINT;

/// IDS Console Operations
typedef struct _IDS_CONSOLE_OPERATIONS {
  PF_CREATE_CONSOLE_CONTEXT CreateConsoleContext;
  PF_INIT_CONSOLE_CONTEXT InitConsoleContext;
  PF_UPDATE_CONSOLE_CONTEXT UpdateConsoleContext;
  PF_DESTROY_CONSOLE_CONTEXT DestroyConsoleContext;
  PF_PRINT Print;
} IDS_CONSOLE_OPERATIONS;

/// IDS Console Header
typedef struct _IDS_CONSOLE_HEADER {
  UINT32 Signature;               ///< Signature information.
  UINT32 Version;                 ///< Version.
  UINT8  ConsoleType;             ///< Console type
  UINT8  Event;                   ///< Event type.
  UINT8  PrintState;              ///< On or Off
  UINT8  OutBufferMode;           ///< Off:stack mode, On: heap mode
  UINT16 OutBufferSize;           ///< Buffer size
  UINT16 OutBufferIndex;          ///< Buffer index
  UINT32 NumBreakpointUnit;       ///< default 0 no bp unit others number of bp unit
} IDS_CONSOLE_HEADER;

/// IDS Console
struct _IDS_CONSOLE
{
  IDS_CONSOLE_HEADER Header;      /**< IDS console header - 5 dwords */
  UINT32 FuncListAddr;            /**< 32 bit address to the list of functions that script can execute */
  UINT8  Reserved[56 - 24];       /**< ----------------- New fields must be added here. */
  CHAR8  BreakpointList[300];     /**< Breakpoint list */
  CHAR8  StatusStr[156];          /**< Shows current node, DCT, CS,... */
  CHAR8  OutBuffer[2];            /**< Console Out content. Its size will be determined by BufferSize. */
};

/**
 *  Create hdt console context
 *
 *  Do hardware settings for hdt console context
 *
 *  @param[in,out] IdsConsole    The Pointer of IDS console
 *
 **/
STATIC VOID
CreateHdtConsoleContext (
  IN OUT   IDS_CONSOLE *IdsConsole
  )
{
  UINT64 SMsr;
  UINT32 CR4reg;

  LibAmdMsrRead (0xC001100A, (UINT64*)&SMsr, NULL);
  SMsr |= 1;
  LibAmdMsrWrite (0xC001100A, (UINT64*)&SMsr, NULL);

  LibAmdWriteCpuReg (DR2_REG, 0x99CC);
  LibAmdWriteCpuReg (DR7_REG, 0x02000420);

  LibAmdReadCpuReg (CR4_REG, &CR4reg);
  LibAmdWriteCpuReg (CR4_REG, CR4reg | ((UINT32)1 << 3));
}

/**
 *  Initialize hdt console context
 *
 *  Initilize preference settings for hdt console context
 *
 *  @param[in,out] IdsConsole    The Pointer of IDS console
 *
 **/
STATIC VOID
InitHdtConsoleContext (
  IN OUT   IDS_CONSOLE *IdsConsole
  )
{
  IDS_FUNCLIST_EXTERN ();

  IdsConsole->FuncListAddr = (UINT32) IDS_FUNCLIST_ADDR;
  IdsConsole->StatusStr[0] = 0;

}

/**
 *  Update hdt console context
 *
 *  Update preference settings for hdt console context
 *
 *  @param[in,out] IdsConsole    The Pointer of IDS console
 *
 **/
STATIC VOID
UpdateHdtConsoleContext (
  IN OUT   IDS_CONSOLE *IdsConsole
  )
{
  if (IdsConsole->Header.OutBufferMode == IDS_STATE_OFF) {
    IdsConsole->Header.OutBufferSize = 0;
  }

}

/**
 *  Destroy hdt console context
 *
 *  Save hdt console context snapshot
 *
 *  @param[in,out] IdsConsole    The Pointer of IDS console
 *
 **/
STATIC VOID
DestroyHdtConsoleContext (
  IN OUT   IDS_CONSOLE *IdsConsole
  )
{
  UINT64 SMsr;

  LibAmdMsrRead (0xC001100A, (UINT64 *)&SMsr, NULL);
  SMsr &= ~BIT0;
  LibAmdMsrWrite (0xC001100A, (UINT64 *)&SMsr, NULL);

  LibAmdWriteCpuReg (DR2_REG, 0);
  LibAmdWriteCpuReg (DR3_REG, 0);
  LibAmdWriteCpuReg (DR7_REG, 0);
}

/**
 *  Hdt console print function
 *
 *  Print function related with hdt console
 *
 *  @param[in] PrintType             Print Type
 *  @param[in] ConsoleBufferAddress  The address of console buffer
 *  @param[in] ConsoleBufferSize     The size of console buffer
 *  @param[in,out] IdsConsole        The Pointer of IDS console
 *
 **/
STATIC VOID
HdtConsolePrint (
  IN       UINT32 PrintType,
  IN       UINT32 ConsoleBufferAddress,
  IN       UINT32 ConsoleBufferSize,
  IN OUT   IDS_CONSOLE *IdsConsole
  )
{
  IdsOutPort (PrintType | 0x99CC, ConsoleBufferAddress, ConsoleBufferSize);
}

/// Initial construction data for HDT console header.
CONST IDS_CONSOLE_HEADER ROMDATA HdtConsoleHeader =
{
 0xDB1099CC,
 0x0100,
 IDS_STATE_ON,
 EVENT_OFF,
 IDS_STATE_ON,
 IDS_STATE_ON,
 0x1000,
 0x0,
 0,
};

#define OPTION_HDT_CONSOLE_HEADER &HdtConsoleHeader

/// Initial construction data for HDT console operations.
CONST IDS_CONSOLE_OPERATIONS ROMDATA HdtConsoleOperations =
{
 CreateHdtConsoleContext,
 InitHdtConsoleContext,
 UpdateHdtConsoleContext,
 DestroyHdtConsoleContext,
 HdtConsolePrint
};

#define OPTION_HDT_CONSOLE_OPERATIONS &HdtConsoleOperations

/**
 *  Parses flag and width information from theFormat string and returns the next index
 *  into the Format string that needs to be parsed. See file headed for details of Flag and Width.
 *
 *  @param[in]  Format  Current location in the AvSPrint format string.
 *  @param[out] Flags  Returns flags
 *  @param[out] Width  Returns width of element
 *  @param[out] Marker Vararg list that may be partially consumed and returned.
 *
 *  @retval Pointer indexed into the Format string for all the information parsed by this routine.
 *
 **/
STATIC CHAR8 *
GetFlagsAndWidth (
  IN       CHAR8 *Format,
     OUT   UINTN *Flags,
     OUT   UINTN *Width,
  IN OUT   VA_LIST *Marker
  )
{
  UINTN   Count;
  BOOLEAN Done;

  *Flags = 0;
  *Width = 0;
  for (Done = FALSE; !Done; ) {
    Format++;

    switch (*Format) {

    case '-':
      *Flags |= LEFT_JUSTIFY;
      break;
    case '+':
      *Flags |= PREFIX_SIGN;
      break;
    case ' ':
      *Flags |= PREFIX_BLANK;
      break;
    case ',':
      *Flags |= COMMA_TYPE;
      break;
    case 'L':
    case 'l':
      *Flags |= LONG_TYPE;
      break;

    case '*':
      *Width = VA_ARG (*Marker, UINTN);
      break;

    case '0':
      *Flags |= PREFIX_ZERO;
      break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      Count = 0;
      do {
        Count = (Count * 10) + *Format - '0';
        Format++;
      } while ((*Format >= '0')  &&  (*Format <= '9'));
      Format--;
      *Width = Count;
      break;

    default:
      Done = TRUE;
    }
  }
  return Format;
}

CHAR8 STATIC HexStr[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
/**
 *
 *  @param[in] Buffer Location to place ascii hex string of Value.
 *  @param[in] Value  - Hex value to convert to a string in Buffer.
 *  @param[in] Flags  - Flags to use in printing Hex string, see file header for details.
 *  @param[in] Width  - Width of hex value.
 *
 *  @retval Number of characters printed.
 **/

STATIC UINTN
ValueTomHexStr (
  IN OUT   CHAR8   *Buffer,
  IN       UINT64      Value,
  IN       UINTN       Flags,
  IN       UINTN       Width
  )
{
  CHAR8   TempBuffer[30];
  CHAR8   *TempStr;
  CHAR8   Prefix;
  CHAR8   *BufferPtr;
  UINTN   Count;
  UINTN   Index;

  TempStr = TempBuffer;
  BufferPtr = Buffer;
  //
  // Count starts at one since we will null terminate. Each iteration of the
  // loop picks off one nibble. Oh yea TempStr ends up backwards
  //
  Count = 0;
  do {
    *(TempStr++) = HexStr[Value & 0x0f];
    Value >>= 4;
    Count++;
  } while (Value != 0);

  if (Flags & PREFIX_ZERO) {
    Prefix = '0';
  } else if (!(Flags & LEFT_JUSTIFY)) {
    Prefix = ' ';
  } else {
    Prefix = 0x00;
  }
  for (Index = Count; Index < Width; Index++) {
    *(TempStr++) = Prefix;
  }

  //
  // Reverse temp string into Buffer.
  //
  while (TempStr != TempBuffer) {
    *(BufferPtr++) = *(--TempStr);
  }

  *BufferPtr = 0;
  return Index;
}

/**
 *  Prints a Value as a decimal number in Buffer
 *
 *  @param[in] Buffer  Location to place ascii decimal number string of Value.
 *  @param[in] Value  Decimal value to convert to a string in Buffer.
 *  @param[in] Flags  Flags to use in printing decimal string, see file header for details.
 *
 *  @retval Number of characters printed.
 *
**/

STATIC UINTN
ValueToString (
  IN OUT   CHAR8   *Buffer,
  IN       INT32       Value,
  IN       UINTN       Flags
  )
{
  CHAR8   TempBuffer[30];
  CHAR8   *TempStr;
  CHAR8   *BufferPtr;
  UINTN   Count;
  UINTN   Remainder;

  TempStr = TempBuffer;
  BufferPtr = Buffer;
  Count = 0;

  if (Value < 0) {
    *(BufferPtr++) = '-';
    Value = - Value;
    Count++;
  }

  do {
    Remainder = Value % 10;
    Value /= 10;
    *(TempStr++) = (CHAR8)(Remainder + '0');
    Count++;
    if ((Flags & COMMA_TYPE) == COMMA_TYPE) {
      if (Count % 3 == 0) {
        *(TempStr++) = ',';
      }
    }
  } while (Value != 0);

  //
  // Reverse temp string into Buffer.
  //
  while (TempStr != TempBuffer) {
    *(BufferPtr++) = *(--TempStr);
  }

  *BufferPtr = 0;
  return Count;
}

/**
 *  Check if String contain the substring
 *
 *  @param[in] String  Pointer of string.
 *  @param[in] Substr  Pointer of sub string.
 *
 *  @retval TRUE S2 is substring of S1
 *  @retval FALSE S2 isn't substring of S1
 *
**/
STATIC BOOLEAN
AmdIdsSubStr (
  IN      CHAR8 *String,
  IN      CHAR8 *Substr
  )
{
  UINT16 i;
  UINT16 j;

  for (i = 0; String[i] != 0 ; i++) {
    for (j = 0; (Substr[j] != 0) && (Substr[j] == String[i + j]); j++) {
    }
    if (Substr[j] == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 *  IDS Backend Function for Memory timeout control
 *
 *  This function is used to override Memory timeout control.
 *
 *  @param[in,out]   DataPtr      The Pointer of UINT8.
 *
 **/
VOID
IdsMemTimeOut (
  IN OUT   VOID *DataPtr
  )
{
  UINT32 DR2reg;

  LibAmdReadCpuReg (DR2_REG, &DR2reg);
  if (DR2reg == 0x99CC) {
    // Turn timeout off if HDTout is on
    *((UINT8 *)DataPtr) = (UINT8)0;
  }
}

/**
 *
 *  IDS Debug Function to check the sentinels are intact
 *
 * This function complete heap walk and check to be performed at any time.
 *
 * @param[in]      StdHeader         Config handle for library and services.
 *
 * @retval         TRUE     No error
 *
 **/
BOOLEAN
AmdHeapIntactCheck (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8       *HeapBufferPtr;
  BUFFER_NODE       *StartOfBufferPtr;
  BUFFER_NODE       *EndOfBufferPtr;
  HEAP_MANAGER *HeapManagerPtr;
  BUFFER_NODE *HeadNodePtr;
  BUFFER_NODE *CurrentNodePtr;
  UINT32  AmdHeapRamAddress;
  UINT32 SentinelBefore;
  UINT32 SentinelAfter;

  ASSERT (StdHeader != NULL);

  AmdHeapRamAddress = (UINT32)UserOptions.CfgHeapDramAddress;

  if (StdHeader->HeapStatus == HEAP_LOCAL_CACHE) {
    HeapBufferPtr = (UINT8 *) HeapGetCurrentBase (StdHeader);
  } else if (StdHeader->HeapStatus == HEAP_TEMP_MEM) {
    HeapBufferPtr = (UINT8 *) AmdHeapRamAddress;
  } else {
    return TRUE;
  }

  HeapManagerPtr = (HEAP_MANAGER *) HeapBufferPtr;
  HeadNodePtr = (BUFFER_NODE *) (HeapBufferPtr + sizeof (HEAP_MANAGER));
  CurrentNodePtr = HeadNodePtr;

  if (HeapManagerPtr->AvailableSize != AMD_HEAP_SIZE_PER_CORE - sizeof (HEAP_MANAGER)) {
    while (CurrentNodePtr != NULL) {
      StartOfBufferPtr = (BUFFER_NODE *) ((UINT8 *) CurrentNodePtr + sizeof (BUFFER_NODE) + CurrentNodePtr->BufferSize + 2 * SIZE_OF_SENTINEL);
      EndOfBufferPtr = (BUFFER_NODE *) ((UINT8 *) HeadNodePtr + AMD_HEAP_SIZE_PER_CORE);
      if (CurrentNodePtr->NextNodePtr != NULL) {
        ASSERT ((CurrentNodePtr->NextNodePtr >= StartOfBufferPtr) && (CurrentNodePtr->NextNodePtr < EndOfBufferPtr));
        SentinelBefore = *(UINT32 *) ((UINT8 *) CurrentNodePtr + sizeof (BUFFER_NODE));
        SentinelAfter = *(UINT32 *) ((UINT8 *) CurrentNodePtr + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL + CurrentNodePtr->BufferSize);

        ASSERT ((SentinelBefore == SENTINEL_BEFORE_VALUE) && (SentinelAfter == SENTINEL_AFTER_VALUE));
      } else {
        ASSERT ((UINT8 *) StartOfBufferPtr == HeapBufferPtr + AMD_HEAP_SIZE_PER_CORE - HeapManagerPtr->AvailableSize);
        SentinelBefore = *(UINT32 *) ((UINT8 *) CurrentNodePtr + sizeof (BUFFER_NODE));
        SentinelAfter = *(UINT32 *) ((UINT8 *) CurrentNodePtr + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL + CurrentNodePtr->BufferSize);
        ASSERT ((SentinelBefore == SENTINEL_BEFORE_VALUE) && (SentinelAfter == SENTINEL_AFTER_VALUE));
      }
      CurrentNodePtr = CurrentNodePtr->NextNodePtr;
    }
  }
  return TRUE;
}

/**
 *  Determine whether IDS console is enabled.
 *
 *  @param[in,out] IdsConsole    The Pointer of Ids console data
 *
 *  @retval       TRUE    Ids console is enabled.
 *  @retval       FALSE   Ids console is disabled.
 *
 **/
BOOLEAN
STATIC
IsIdsConsoleEnabled (
  IN OUT   UINTN *IdsConsole
  )
{
  BOOLEAN Result;
  UINT32  DR2reg;

  Result = FALSE;

  LibAmdReadCpuReg (DR2_REG, &DR2reg);
  if (DR2reg == 0x99CC) {
    Result = TRUE;
  }

  return Result;
}

/**
 *  Get IDS console.
 *
 *  @param[in,out] IdsConsolePtr  The Pointer of Ids console data
 *
 **/
STATIC VOID
GetIdsConsole (
  IN OUT   UINTN *IdsConsolePtr
  )
{
  UINT32 DR3Reg;

  LibAmdReadCpuReg (DR3_REG, &DR3Reg);
  *IdsConsolePtr = (UINTN) DR3Reg;
}

/**
 *  Get IDS console operations.
 *
 *  @param[in,out] IdsConsoleOperations  The Pointer of Ids console operations
 *
 **/
STATIC VOID
GetIdsConsoleHeader (
  IN OUT   UINTN *IdsConsoleHeader
  )
{
  IDS_CONSOLE_TYPE IdsConsoleType;

  IdsConsoleType = (IDS_CONSOLE_TYPE) OPTION_IDS_CONSOLE;

  if (IdsConsoleType == HDT_CONSOLE) {
    *IdsConsoleHeader = (UINTN) OPTION_HDT_CONSOLE_HEADER;
  } else {
    ASSERT (FALSE);
  }
}

/**
 *  Get IDS console operations.
 *
 *  @param[in,out] IdsConsoleOperations  The Pointer of Ids console operations
 *
 **/
STATIC VOID
GetIdsConsoleOperations (
  IN OUT   UINTN *IdsConsoleOperations
  )
{
  IDS_CONSOLE_TYPE IdsConsoleType;

  IdsConsoleType = (IDS_CONSOLE_TYPE) OPTION_IDS_CONSOLE;

  if (IdsConsoleType == HDT_CONSOLE) {
    *IdsConsoleOperations = (UINTN) OPTION_HDT_CONSOLE_OPERATIONS;
  } else {
    ASSERT (FALSE);
  }
}

/**
 *  Create IDS console.
 *
 *  @param[in,out] IdsConsole    The Pointer of Ids console data
 *  @param[in,out] StdHeader     The Pointer of AGESA Header
 **/
STATIC VOID
NewIdsConsole (
  IN OUT   IDS_CONSOLE *IdsConsole,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDS_CONSOLE_HEADER *IdsConsoleHeader;
  UINTN IdsConsoleHeaderPtr;

  GetIdsConsoleHeader (&IdsConsoleHeaderPtr);
  IdsConsoleHeader = &(IdsConsole->Header);

  LibAmdMemCopy ((VOID *)IdsConsoleHeader, (VOID *) (IdsConsoleHeaderPtr), (UINT32) sizeof (IDS_CONSOLE_HEADER), StdHeader);
}

/**
 *  Destroy IDS console.
 *
 *  @param[in,out] IdsConsole    The Pointer of Ids console data
 *  @param[in,out] StdHeader     The Pointer of AGESA Header
 **/
STATIC VOID
DestroyIdsConsole (
  IN OUT   IDS_CONSOLE *IdsConsole,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  HeapDeallocateBuffer (IDS_HDT_OUT_BUFFER_HANDLE, StdHeader);
}

/**
 *  Save IDS console Snapshot.
 *
 *  @param[in,out] IdsConsole    The Pointer of Ids console data
 *  @param[in,out] StdHeader     The Pointer of AGESA Header
 **/
STATIC VOID
SaveIdsConsole (
  IN OUT   IDS_CONSOLE *IdsConsole,
  IN OUT   IDS_CONSOLE_OPERATIONS *IdsConsoleOps,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  do {
    AllocHeapParams.RequestedBufferSize = IdsConsole->Header.OutBufferSize + sizeof (IDS_CONSOLE) - 2;
    AllocHeapParams.BufferHandle = IDS_HDT_OUT_BUFFER_HANDLE;
    AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
    if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
      break;
    } else {
      IdsConsole->Header.OutBufferSize -= 256;
      IdsConsole->Header.Event = EVENT_FAIL_BUFFER_ALLOCATION;
      IdsConsoleOps->Print (DEBUG_PRINT_EVENT, (UINT32)IdsConsole, 0, IdsConsole);
    }
  } while ((IdsConsole->Header.OutBufferSize & 0x8000) == 0);

  if ((IdsConsole->Header.OutBufferSize & 0x8000) == 0) {
    LibAmdWriteCpuReg (DR3_REG, (UINT32)AllocHeapParams.BufferPtr);
    LibAmdMemCopy (AllocHeapParams.BufferPtr, (VOID *) IdsConsole, (UINT32) sizeof (IDS_CONSOLE) - 2, StdHeader);
  }
}

/**
 *
 *  Initial function for IDS console.
 *
 *  Create IDS console context, let Ids console function to be ready.
 *
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
VOID
AmdIdsConsoleInit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDS_CONSOLE IdsConsole;
  IDS_CONSOLE_OPERATIONS *IdsConsoleOps;
  UINTN IdsConsolePtr;
  UINTN IdsConsoleOpsPtr;

  if (IsIdsConsoleEnabled (&IdsConsolePtr)) {
    GetIdsConsoleOperations (&IdsConsoleOpsPtr);
    IdsConsoleOps = (IDS_CONSOLE_OPERATIONS *) IdsConsoleOpsPtr;
    NewIdsConsole (&IdsConsole, StdHeader);

    IdsConsoleOps->CreateConsoleContext (&IdsConsole);
    IdsConsoleOps->InitConsoleContext (&IdsConsole);
    IdsConsoleOps->Print (DEBUG_PRINT_INIT, (UINT32)&IdsConsole, 0, &IdsConsole);
    IdsConsoleOps->UpdateConsoleContext (&IdsConsole);

    SaveIdsConsole (&IdsConsole, IdsConsoleOps, StdHeader);
  }
}

/**
 *  Prints string to debug host like printf in C
 *
 *  @param[in] Format of string
 *  @param[in] ... Variable parameter
 *
**/
VOID
AmdIdsConsolePrint (
  IN      CHAR8 *Format,
  IN      ...
  )
{
  IDS_CONSOLE *IdsConsole;
  UINTN IdsConsolePtr;
  IDS_CONSOLE_OPERATIONS *IdsConsoleOps;
  UINTN  IdsConsoleOpsPtr;
  VA_LIST Marker;
  CHAR8 LocalBuffer[256];
  CHAR8 *ConsoleBuffer;
  CHAR8 Null_Str[] = " < null string > ";
  CHAR8 *AsciiStr;
  UINTN Index;
  UINTN Flags;
  UINTN Width;
  UINTN ConsoleBufferSize;
  UINT64 Value;
  UINT16 *Array;
  UINT32 ArrayLength;
  UINT32 ArrayIndex;
  BOOLEAN SaveStatus;
  UINT32 LastIndex;
  UINT32 i;
  UINT32 j;
  UINT32 NumBpUnit;
  BREAKPOINT_UNIT *Pbpunit;
  CHAR8 *Pbpstr;
  CHAR8 *PCmpStr;
  CHAR8 *EventLevelStr;
  BOOLEAN LastBpmatched;
  BOOLEAN Bpmatched;
  IDS_STATE_TYPE PrintCtrl;

  //Init the default Value
  IdsConsole = NULL;
  IdsConsolePtr = 0;
  ConsoleBuffer = LocalBuffer;
  Index = 0;
  LastIndex = 0;
  ConsoleBufferSize = 256;
  NumBpUnit = 0;
  PrintCtrl = IDS_STATE_ON;

  if (IsIdsConsoleEnabled (&IdsConsolePtr)) {
    GetIdsConsole (&IdsConsolePtr);
    IdsConsole = (IDS_CONSOLE *) IdsConsolePtr;
    GetIdsConsoleOperations (&IdsConsoleOpsPtr);
    IdsConsoleOps = (IDS_CONSOLE_OPERATIONS *) IdsConsoleOpsPtr;

    if (IdsConsole->Header.OutBufferMode == IDS_STATE_ON) {
      ConsoleBuffer = IdsConsole->OutBuffer;
      Index = IdsConsole->Header.OutBufferIndex;
      ConsoleBufferSize = (UINTN) (IdsConsole->Header.OutBufferSize);
      NumBpUnit = IdsConsole->Header.NumBreakpointUnit;
      PrintCtrl = IdsConsole->Header.PrintState;
    }

    if ((PrintCtrl != 0) || (NumBpUnit > 0)) {
      VA_START(Marker,Format); //init marker to 1st dynamic parameters.

      LastIndex = (UINT32) Index;

      if (*Format != '@') {
        if (*Format == '!') {
          SaveStatus = TRUE;
          Format++;
        } else {
          SaveStatus = FALSE;
        }

        for (; *Format != '\0'; Format++) {
          if (*Format != '%') {
            ConsoleBuffer[Index++] = *Format;
          } else {
            Format = GetFlagsAndWidth (Format, &Flags, &Width, &Marker);
            switch (*Format) {
            // Using %[EventType] style to catch predefined event.
            case '[':
              EventLevelStr = Format;
              EventLevelStr++;
              if ((*EventLevelStr > EVENT_OFF) && (*EventLevelStr < EVENT_END)) {
                EventLevelStr++;
                if (*EventLevelStr == ']') {
                  EventLevelStr--;
                  IdsConsole->Header.Event = (UINT8) (CONSOLE_EVENT_TYPE) (*EventLevelStr);
                }
              }
              break;
            case 'X':
              Flags |= PREFIX_ZERO;
              Width = sizeof (UINT64) * 2;
              //
              // break skipped on purpose
              //
            case 'x':
              if ((Flags & LONG_TYPE) == LONG_TYPE) {
                Value = VA_ARG (Marker, UINT64);
              } else {
                Value = VA_ARG (Marker, UINTN);
              }
              Index += ValueTomHexStr (&ConsoleBuffer[Index], Value, Flags, Width);
              break;

            case 'd':
              Value = (UINTN)VA_ARG (Marker, UINT32);
              Index += ValueToString (&ConsoleBuffer[Index], (UINT32)Value, Flags);
              break;

            case 's':
            case 'S':
              AsciiStr = (CHAR8 *)VA_ARG (Marker, CHAR8 *);
              if (AsciiStr == NULL) {
                AsciiStr = Null_Str; //" < null string > ";
              }
              while (*AsciiStr != '\0') {
                ConsoleBuffer[Index++] = *AsciiStr++;
              }
              break;

            case 'c':
              ConsoleBuffer[Index++] = (CHAR8)VA_ARG (Marker, UINTN);
              break;

            case 'a':
              Array = (UINT16 *) VA_ARG (Marker, VOID *);
              ArrayLength = (UINT32) VA_ARG (Marker, UINT32);
              for (ArrayIndex = 0; ArrayIndex < ArrayLength; ArrayIndex++) {
                // Only support hex format of an array of UINT16 for now.
                Width = 2;
                Flags = PREFIX_ZERO;
                ConsoleBuffer[Index++] = ' ';
                Index += ValueTomHexStr (&ConsoleBuffer[Index], Array[ArrayIndex] & 0xFF, Flags, Width);

                // If buffer is full
                if (Index > (ConsoleBufferSize - 8)) {
                  if (IdsConsole != NULL) {
                    if (LastIndex != 0) {
                      // Stream all out except current string
                      if (PrintCtrl != 0) {
                        IdsConsoleOps->Print (DEBUG_PRINT_COMMAND, (UINT32)ConsoleBuffer, LastIndex, IdsConsole);
                      }
                      Index = Index - LastIndex;
                      // Move current string too top
                      for (i = 0; i < Index; i++) {
                        ConsoleBuffer[i] = ConsoleBuffer[LastIndex + i];
                      }
                      LastIndex = 0;
                    } else {
                      // Buffer size is too small
                      ASSERT (FALSE);
                    }
                  } else {
                    break;
                  }
                }
              }
              break;

            case 'v':
              ConsoleBuffer[Index++] = '%';
              ConsoleBuffer[Index++] = 'v';
              Format++;
              ConsoleBuffer[Index++] = *Format;
              if (*Format == 'h') {
                Format++;
                ConsoleBuffer[Index++] = *Format;
              }
              break;

            case '%':
              ConsoleBuffer[Index++] = *Format;
              break;

            default:
              //
              // if the type is unknown print it to the screen
              //
              ConsoleBuffer[Index++] = '%';
              ConsoleBuffer[Index++] = *Format;
            }
          }

          // If buffer is full
          if (Index > (ConsoleBufferSize - 32)) {
            if (IdsConsole != NULL) {
              if (LastIndex != 0) {
                // Stream all out except current string
                if (PrintCtrl != IDS_STATE_OFF) {
                  IdsConsoleOps->Print (DEBUG_PRINT_COMMAND, (UINT32)ConsoleBuffer, LastIndex, IdsConsole);
                }
                Index = Index - LastIndex;
                // Move current string too top
                for (i = 0; i < Index; i++) {
                  ConsoleBuffer[i] = ConsoleBuffer[LastIndex + i];
                }
                LastIndex = 0;
              } else {
                // Buffer size is too small
                ASSERT (FALSE);
              }
            } else {
              break;
            }
          }
        }

        if (IdsConsole->Header.OutBufferMode == IDS_STATE_OFF) {
          IdsConsoleOps->Print (DEBUG_PRINT_COMMAND, (UINT32)ConsoleBuffer, (UINT32) Index, IdsConsole);
          Index = 0;
        }

        if ((IdsConsole->Header.Event > EVENT_OFF) && (IdsConsole->Header.Event < EVENT_END) ) {
          // if HdtOut Buffer is on, flush hdtout buffer contents out.
          if (IdsConsole->Header.OutBufferMode == IDS_STATE_ON) {
            IdsConsoleOps->Print (DEBUG_PRINT_COMMAND, (UINT32)ConsoleBuffer, (UINT32) Index, IdsConsole);
            Index = 0;
          }

          IdsConsoleOps->Print (DEBUG_PRINT_EVENT, (UINT32)IdsConsole, 0, IdsConsole);
          IdsConsole->Header.Event = EVENT_OFF;
        }

        //
        //  Check breakpoint
        //
        if (NumBpUnit) {
          Pbpunit = (BREAKPOINT_UNIT *) IdsConsole->BreakpointList;
          LastBpmatched = TRUE;
          Bpmatched = TRUE;
          for (i = 0; i < NumBpUnit; i++) {
            Pbpstr = IdsConsole->BreakpointList + Pbpunit[i].BpStrOffset;
            if (Pbpunit[i].BpFlag == IDS_HDTOUT_BPFLAG_FORMAT_STR) {
              PCmpStr = &ConsoleBuffer[LastIndex];
              ConsoleBuffer[Index] = 0;
            } else {
              PCmpStr = IdsConsole->StatusStr;
            }

            if (LastBpmatched) {
              Bpmatched = AmdIdsSubStr (PCmpStr, Pbpstr);
              if (Bpmatched) {
                if (Pbpunit[i].AndFlag == 0) {
                  // This is the last of matching string of an AND block, apply action here
                  switch (Pbpunit[i].Action) {
                  case HDTOUT_BP_ACTION_HALT:
                    IdsConsoleOps->Print (DEBUG_PRINT_BREAKPOINT, (UINT32)(ConsoleBuffer + LastIndex), ( i << 16) | (UINT32) Index, IdsConsole);
                    break;
                  case HDTOUT_BP_ACTION_PRINTON:
                    PrintCtrl = IDS_STATE_ON;
                    IdsConsole->Header.PrintState = IDS_STATE_ON;
                    break;
                  case HDTOUT_BP_ACTION_PRINTOFF:
                    if (IdsConsole->Header.PrintState != IDS_STATE_OFF) {
                      IdsConsole->Header.PrintState = IDS_STATE_OFF;
                      IdsConsoleOps->Print (DEBUG_PRINT_COMMAND, (UINT32)ConsoleBuffer, (UINT32)Index, IdsConsole);
                    }
                    break;
                  default:
                    ASSERT (FALSE);
                  }
                  break;
                }
              }
            }
            if (Pbpunit[i].AndFlag == 1) {
              LastBpmatched = Bpmatched;
            } else {
              LastBpmatched = TRUE;
            }
          }
        }

        //
        //  Store status fields
        //
        if (SaveStatus) {
          // Look for the start of the first word
          for (; LastIndex < Index; LastIndex++) {
            if ((ConsoleBuffer[LastIndex] > 32) && (ConsoleBuffer[LastIndex] < 127)) {
              break;
            }
          }
          if (LastIndex < Index) {
            // Match the first word in StatusStr
            SaveStatus = FALSE;
            j = LastIndex;
            for (i = 0; !SaveStatus && (IdsConsole->StatusStr[i] != 0); i++) {
              ArrayLength = 1;
              for (j = LastIndex; ConsoleBuffer[j] == IdsConsole->StatusStr[i]; j++) {
                i++;
                ArrayLength++;
                if ((j == (Index - 1)) || (ConsoleBuffer[j] == ' ')) {
                  // Find the length of this entry
                  ArrayIndex = i;
                  for (; IdsConsole->StatusStr[i] != '\n'; i++) {
                    ArrayLength++;
                  }

                  // Remove old entry if it does not fit
                  if (ArrayLength != ((UINT32) Index - LastIndex)) {
                    for (i++; IdsConsole->StatusStr[i] != 0; i++) {
                      IdsConsole->StatusStr[i - ArrayLength] = IdsConsole->StatusStr[i];
                    }
                    j = LastIndex;
                    i = i - ArrayLength - 1;
                    // Mark the end of string
                    IdsConsole->StatusStr[i + ((UINT32) Index - LastIndex) + 1] = 0;
                  } else {
                    i = ArrayIndex - 2;
                  }

                  // Word match, exit for saving
                  SaveStatus = TRUE;
                  break;
                }
              }
            }

            // Copy string to StatusStr
            for (; j < Index; j++, i++) {
              IdsConsole->StatusStr[i] = ConsoleBuffer[j];
            }
            if (!SaveStatus) {
              // Mark the end of string if not done so
              IdsConsole->StatusStr[i] = 0;
            }
          }
        }

      }
    }

    if (IdsConsole != NULL) {
      if (IdsConsole->Header.PrintState == IDS_STATE_OFF) {
        Index = 0;    // Clear buffer if the data will not be printed out.
      }
      IdsConsole->Header.OutBufferIndex = (UINT16) Index;
    } else {
      IdsConsoleOps->Print (DEBUG_PRINT_COMMAND, (UINT32)ConsoleBuffer, (UINT32)Index, IdsConsole);
    }
  }
}

/**
 *
 *  Exit function for IDS console Function.
 *
 *  Restore debug register and Deallocate heap.
 *
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
VOID
AmdIdsConsoleExit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDS_CONSOLE *IdsConsole;
  UINTN IdsConsolePtr;
  IDS_CONSOLE_OPERATIONS *IdsConsoleOps;
  UINTN  IdsConsoleOpsPtr;

  if (IsIdsConsoleEnabled (&IdsConsolePtr)) {
    GetIdsConsole (&IdsConsolePtr);
    IdsConsole = (IDS_CONSOLE *) IdsConsolePtr;
    GetIdsConsoleOperations (&IdsConsoleOpsPtr);
    IdsConsoleOps = (IDS_CONSOLE_OPERATIONS *) IdsConsoleOpsPtr;

    if (IdsConsole != NULL) {
      if (IdsConsole->Header.PrintState != IDS_STATE_OFF) {
        IdsConsoleOps->Print (DEBUG_PRINT_COMMAND, (UINT32)(IdsConsole->OutBuffer), IdsConsole->Header.OutBufferIndex, IdsConsole);
      }

      IdsConsoleOps->Print (DEBUG_PRINT_EXIT, (UINT32)IdsConsole, 0, IdsConsole);

      IdsConsoleOps->DestroyConsoleContext (IdsConsole);
      DestroyIdsConsole (IdsConsole, StdHeader);

    }
  }
}
/**
 * Check for CAR Corruption, the performance monitor number three programed to log the CAR Corruption.
 * Check to see if control register is enabled and then check the preformance counter and stop the system by executing
 * IDS_ERROR_TRAP if counter has any value other than zero.
 *
 *  @param[in,out] StdHeader    The Pointer of Standard Header.
 *
 *
 **/
VOID
IdsCarCorruptionCheck (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{

  UINT64 Perf_Msr;

  LibAmdMsrRead (MSR_PERF_CONTROL3, (UINT64*)&Perf_Msr, StdHeader);
  if ((Perf_Msr & PERF_RESERVE_BIT_MASK) == PERF_CAR_CORRUPTION_EVENT) {
    LibAmdMsrRead (MSR_PERF_COUNTER3, (UINT64*)&Perf_Msr, StdHeader);
    if ((Perf_Msr != 0)) {
      IDS_ERROR_TRAP;
    }
  }
}
