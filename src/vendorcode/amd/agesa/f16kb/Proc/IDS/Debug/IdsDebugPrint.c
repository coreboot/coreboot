/**
 * @file
 *
 * AMD Integrated Debug Print Routines
 *
 * Contains all functions related to IDS Debug Print
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

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "IdsLib.h"
#include "amdlib.h"
#include "IdsDebugPrint.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_IDS_DEBUG_IDSDEBUGPRINT_FILECODE

//
// Also support coding convention rules for var arg macros
//
#define _INT_SIZE_OF(n) ((sizeof (n) + sizeof (UINTN) - 1) &~(sizeof (UINTN) - 1))
typedef CHAR8 *VA_LIST;
#undef VA_START
#undef VA_ARG
#undef VA_END
#define VA_START(ap, v) (ap = (VA_LIST) & (v) + _INT_SIZE_OF (v))
#define VA_ARG(ap, t)   (*(t *) ((ap += _INT_SIZE_OF (t)) - _INT_SIZE_OF (t)))
#define VA_END(ap)      (ap = (VA_LIST) 0)

#define LEFT_JUSTIFY    0x01
#define PREFIX_SIGN     0x02
#define PREFIX_BLANK    0x04
#define COMMA_TYPE      0x08
#define LONG_TYPE       0x10
#define PREFIX_ZERO     0x20

#define MAX_LOCAL_BUFFER_SIZE   512
#define BUFFER_OVERFLOW 0xFFFF

/**
 *  Check If any print service is enabled.
 *
 *  @param[in] DebugPrintList    The Pointer to print service list
 *
 *  @retval       TRUE    At least on print service is enabled
 *  @retval       FALSE   All print service is disabled
 *
 **/
STATIC BOOLEAN
AmdIdsDebugPrintCheckSupportAll (
  IN      IDS_DEBUG_PRINT **DebugPrintList
  )
{
  BOOLEAN IsSupported;
  UINTN i;
  IsSupported = FALSE;
  for (i = 0; DebugPrintList[i] != NULL; i++) {
    if (DebugPrintList[i]->support ()) {
      IsSupported = TRUE;
    }
  }
  return IsSupported;
}

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
extern CONST IDS_DEBUG_PRINT* ROMDATA IdsDebugPrint[];

/**
 *
 *  @param[in,out] Value  - Hex value to convert to a string in Buffer.
 *
 *
 */
VOID
GetDebugPrintList (
  IN OUT   CONST IDS_DEBUG_PRINT   ***pIdsDebugPrintListPtr
  )
{
  *pIdsDebugPrintListPtr = &IdsDebugPrint[0];
}

/**
 *
 *  @param[in,out] Buffer Location to place ascii hex string of Value.
 *  @param[in] Value  - Hex value to convert to a string in Buffer.
 *  @param[in] Flags  - Flags to use in printing Hex string, see file header for details.
 *  @param[in] Width  - Width of hex value.
 *  @param[in,out] BufferSize  - Size of input buffer
 *
 *  @retval Number of characters printed.
 **/

STATIC UINTN
ValueToHexStr (
  IN OUT   CHAR8   *Buffer,
  IN       UINT64      Value,
  IN       UINTN       Flags,
  IN       UINTN       Width,
  IN OUT   UINTN       *BufferSize
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
    (*BufferSize)--;
    if (*BufferSize == 0) {
      return BUFFER_OVERFLOW;
    }
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
 *  @param[in,out] BufferSize  Size of input buffer
 *
 *  @retval Number of characters printed.
 *
**/

STATIC UINTN
ValueToString (
  IN OUT   CHAR8   *Buffer,
  IN       INT32       Value,
  IN       UINTN       Flags,
  IN OUT   UINTN       *BufferSize
  )
{
  CHAR8   TempBuffer[30];
  CHAR8   *TempStr;
  CHAR8   *BufferPtr;
  UINTN   Count;
  UINTN   Remainder;

  ASSERT (*BufferSize);
  TempStr = TempBuffer;
  BufferPtr = Buffer;
  Count = 0;

  if (Value < 0) {
    (*BufferSize)--;
    if (*BufferSize == 0) {
      return BUFFER_OVERFLOW;
    }
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
    (*BufferSize)--;
    if (*BufferSize == 0) {
      return BUFFER_OVERFLOW;
    }
    *(BufferPtr++) = *(--TempStr);
  }

  *BufferPtr = 0;
  return Count;
}

/**
 *  Worker function for print string to buffer
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] *Format - format string
 *  @param[in] Marker  - Variable parameter
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *  @param[out] OutputStringLen  - output string length, include '\0' at the end
 *
 *  @retval    IDS_DEBUG_PRINT_SUCCESS      succeed
 *  @retval    IDS_DEBUG_PRINT_BUFFER_OVERFLOW      input buffer overflow
**/
STATIC IDS_DEBUG_PRINT_STATUS
AmdIdsDebugPrintWorker (
  IN       CHAR8 *Format,
  IN       VA_LIST Marker,
  IN       CHAR8 *Buffer,
  IN       UINTN BufferSize,
     OUT   UINTN *OutputStringLen
  )
{
  UINTN Index;
  UINTN Length;
  UINTN Flags;
  UINTN Width;
  UINT64 Value;
  CHAR8 *AsciiStr;

  //Init the default Value
  Index = 0;
  //
  // Process format string
  //
  for (; (*Format != '\0') && (BufferSize > 0); Format++) {
    if (*Format != '%') {
      Buffer[Index++] = *Format;
      BufferSize--;
    } else {
      Format = GetFlagsAndWidth (Format, &Flags, &Width, &Marker);
      switch (*Format) {
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
        Length = ValueToHexStr (&Buffer[Index], Value, Flags, Width, &BufferSize);
        if (Length != BUFFER_OVERFLOW) {
          Index += Length;
        } else {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        break;

      case 'd':
        Value = (UINTN)VA_ARG (Marker, UINT32);
        Length = ValueToString (&Buffer[Index], (UINT32)Value, Flags, &BufferSize);
        if (Length != BUFFER_OVERFLOW) {
          Index += Length;
        } else {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }

        break;

      case 's':
      case 'S':
        AsciiStr = (CHAR8 *)VA_ARG (Marker, CHAR8 *);
        while (*AsciiStr != '\0') {
          BufferSize--;
          if (BufferSize == 0) {
            return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
          }
          Buffer[Index++] = *AsciiStr++;
        }
        break;

      case 'c':
        BufferSize--;
        if (BufferSize == 0) {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        Buffer[Index++] = (CHAR8)VA_ARG (Marker, UINTN);
        break;

      case 'v':
        ASSERT (FALSE);   // %v is no longer supported
        break;

      case '%':
        BufferSize--;
        if (BufferSize == 0) {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        Buffer[Index++] = *Format;
        break;

      default:
        //
        // if the type is unknown print it to the screen
        //
        BufferSize--;
        if (BufferSize == 0) {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        Buffer[Index++] = '%';

        BufferSize--;
        if (BufferSize == 0) {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        Buffer[Index++] = *Format;
        break;
      }
    }
  }
  if (BufferSize == 0) {
    return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
  }
  //Mark the end of word
  Buffer[Index] = 0;
  *OutputStringLen = Index;
  return IDS_DEBUG_PRINT_SUCCESS;
}


/**
 *  Insert Overflow warning to the tail of output
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *
**/
STATIC VOID
InsertOverflowWarningMessage (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize
  )
{
  CHAR8 *Destination;
  CHAR8 WarningString[] = "\n#BUFFER  OVERFLOW#\n";
  AMD_CONFIG_PARAMS StdHeader;

  Destination = Buffer + BufferSize - sizeof (WarningString);
  LibAmdMemCopy (Destination, WarningString, sizeof (WarningString), &StdHeader);
}

/**
 *  Process debug string
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] *Format - format string
 *  @param[in] Marker  - Variable parameter
 *
**/
STATIC VOID
AmdIdsDebugPrintProcess (
  IN      UINT64 Flag,
  IN      CHAR8 *Format,
  IN      VA_LIST Marker
  )
{
  UINT64 Filter;
  CHAR8 LocalBuffer[MAX_LOCAL_BUFFER_SIZE];
  UINTN OutPutStringLen;
  IDS_DEBUG_PRINT **DebugPrintList;
  IDS_DEBUG_PRINT_PRIVATE_DATA debugPrintPrivate;
  UINT8 i;


  GetDebugPrintList ((CONST IDS_DEBUG_PRINT ***)&DebugPrintList);
  if (AmdIdsDebugPrintCheckSupportAll (DebugPrintList)) {
    if (AmdIdsDebugPrintWorker (Format, Marker, &LocalBuffer[0], sizeof (LocalBuffer), &OutPutStringLen) == IDS_DEBUG_PRINT_BUFFER_OVERFLOW) {
      InsertOverflowWarningMessage (&LocalBuffer[0], sizeof (LocalBuffer));
      OutPutStringLen = sizeof (LocalBuffer);
    }

    //init input
    debugPrintPrivate.saveContext = FALSE;

    for (i = 0; DebugPrintList[i] != NULL; i++) {
      if (DebugPrintList[i]->support ()) {
        Filter = IDS_DEBUG_PRINT_MASK;
        //Get Customize filter (Option)
        DebugPrintList[i]->customfilter (&Filter);
        if (Flag & Filter) {
          //Init Private Date (Option)
          DebugPrintList[i]->InitPrivateData (Flag, &debugPrintPrivate);
          //Print Physical Layer
          DebugPrintList[i]->print (&LocalBuffer[0], OutPutStringLen, &debugPrintPrivate);
        }
      }
    }
  }
}

/**
 *  Prints string to debug host like printf in C
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] *Format - format string
 *  @param[in] ... Variable parameter
 *
**/
VOID
AmdIdsDebugPrint (
  IN      UINT64 Flag,
  IN      CONST CHAR8 *Format,
  IN      ...
  )
{
  VA_LIST Marker;
  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  AmdIdsDebugPrintProcess (Flag, (CHAR8 *)Format, Marker);
  VA_END (Marker);
}

/**
 *  Prints memory debug strings
 *
 *  @param[in] *Format - format string
 *  @param[in] ... Variable parameter
 *
**/
VOID
AmdIdsDebugPrintMem (
  IN      CHAR8 *Format,
  IN      ...
  )
{
  VA_LIST Marker;
  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  AmdIdsDebugPrintProcess (MEM_FLOW, Format, Marker);
  VA_END (Marker);
}

/**
 *  Prints CPU debug strings
 *
 *  @param[in] *Format - format string
 *  @param[in] ... Variable parameter
 *
**/
VOID
AmdIdsDebugPrintCpu (
  IN      CHAR8 *Format,
  IN      ...
  )
{
  VA_LIST Marker;
  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  AmdIdsDebugPrintProcess (CPU_TRACE, Format, Marker);
  VA_END (Marker);
}


/**
 *  Prints HT debug strings
 *
 *  @param[in] *Format - format string
 *  @param[in] ... Variable parameter
 *
**/
VOID
AmdIdsDebugPrintHt (
  IN      CHAR8 *Format,
  IN      ...
  )
{
  VA_LIST Marker;
  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  AmdIdsDebugPrintProcess (HT_TRACE, Format, Marker);
  VA_END (Marker);
}


/**
 *  Prints GNB debug strings
 *
 *  @param[in] *Format - format string
 *  @param[in] ... Variable parameter
 *
**/
VOID
AmdIdsDebugPrintGnb (
  IN      CHAR8 *Format,
  IN      ...
  )
{
  VA_LIST Marker;
  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  AmdIdsDebugPrintProcess (GNB_TRACE, Format, Marker);
  VA_END (Marker);
}

/**
 *  Prints debug strings in any condition
 *
 *  @param[in] *Format - format string
 *  @param[in] ... Variable parameter
 *
**/
VOID
AmdIdsDebugPrintAll (
  IN      CHAR8 *Format,
  IN      ...
  )
{
  VA_LIST Marker;
  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  AmdIdsDebugPrintProcess (TRACE_MASK_ALL, Format, Marker);
  VA_END (Marker);
}

