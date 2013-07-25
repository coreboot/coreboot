/* $NoKeywords:$ */
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

#ifndef _IDS_DEBUGPRINT_H_
#define _IDS_DEBUGPRINT_H_

///Debug String End flag
#define DEBUG_STRING_END_FLAG (0x0)
/// return status for debug print
typedef enum {
  IDS_DEBUG_PRINT_SUCCESS = 0,      ///< success
  IDS_DEBUG_PRINT_BUFFER_OVERFLOW,      ///< Bufer overflow
} IDS_DEBUG_PRINT_STATUS;

/// Private datas for debug print
typedef struct _IDS_DEBUG_PRINT_PRIVATE_DATA {
  BOOLEAN saveContext;      /// save context
} IDS_DEBUG_PRINT_PRIVATE_DATA;

typedef BOOLEAN (*PF_IDS_DEBUG_PRINT_SUPPORT) (VOID);
typedef BOOLEAN (*PF_IDS_DEBUG_PRINT_FILTER) (UINT64 *Filter);
typedef VOID (*PF_IDS_DEBUG_PRINT_PRINT) (CHAR8 *Buffer, UINTN BufferSize, IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate);
typedef VOID (*PF_IDS_DEBUG_INIT_PRIVATE_DATA) (UINT64 flag, IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate);

/// Debug print Hw layer service class
typedef struct _IDS_DEBUG_PRINT {
  PF_IDS_DEBUG_PRINT_SUPPORT  support;    ///Check if support
  PF_IDS_DEBUG_PRINT_FILTER  customfilter;  ///Get if any customize filters
  PF_IDS_DEBUG_INIT_PRIVATE_DATA  InitPrivateData; ///Init private data
  PF_IDS_DEBUG_PRINT_PRINT  print;  ///Print data to Hw layer
} IDS_DEBUG_PRINT;


VOID
GetDebugPrintList (
  IN OUT   CONST IDS_DEBUG_PRINT   ***pIdsDebugPrintListPtr
  );


#endif //_IDS_DEBUGPRINT_H_

