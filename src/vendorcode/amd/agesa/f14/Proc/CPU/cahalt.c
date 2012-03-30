/* $NoKeywords:$ */
/**
 * @file
 *
 * HyperTransport features and sequence implementation.
 *
 * Implements the external AmdHtInitialize entry point.
 * Contains routines for directing the sequence of available features.
 * Mostly, but not exclusively, AGESA_TESTPOINT invocations should be
 * contained in this file, and not in the feature code.
 *
 * From a build option perspective, it may be that a few lines could be removed
 * from compilation in this file for certain options.  It is considered that
 * the code savings from this are too small to be of concern and this file
 * should not have any explicit build option implementation.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 35978 $   @e \$Date: 2010-08-07 02:18:50 +0800 (Sat, 07 Aug 2010) $
 *
 */
/*
 *****************************************************************************
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
 * ***************************************************************************
 *
 */

#include "AGESA.h"
#include "cpuRegisters.h"
#include "Filecode.h"

 /*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

// typedef unsigned int   uintptr_t;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
SetIdtr (
  IN     IDT_BASE_LIMIT *IdtInfo,
  IN OUT AMD_CONFIG_PARAMS *StdHeaderPtr
  );

VOID
GetCsSelector (
  IN     UINT16 *Selector,
  IN OUT AMD_CONFIG_PARAMS *StdHeaderPtr
  );

VOID
NmiHandler (
  IN OUT AMD_CONFIG_PARAMS *StdHeaderPtr
  );

VOID
ExecuteHltInstruction (
  IN OUT AMD_CONFIG_PARAMS *StdHeaderPtr
  );

VOID
ExecuteWbinvdInstruction (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

 /*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/* see cahaltasm.S
VOID
ExecuteFinalHltInstruction (
  IN       UINT32 HaltFlags,
  IN       AP_MTRR_SETTINGS  *ApMtrrSettingsList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
}
*/

//----------------------------------------------------------------------------

/// Structure needed to load the IDTR using the lidt instruction

VOID
SetIdtr (
  IN     IDT_BASE_LIMIT *IdtInfo,
  IN OUT AMD_CONFIG_PARAMS *StdHeaderPtr
  )
{
   __lidt (IdtInfo);
}

//----------------------------------------------------------------------------

VOID
GetCsSelector (
  IN     UINT16 *Selector,
  IN OUT AMD_CONFIG_PARAMS *StdHeaderPtr
  )
{
  static const UINT8 opcode [] = {0x8C, 0xC8, 0xC3}; // mov eax, cs; ret
  *Selector = ((UINT16 (*)(void)) (size_t) opcode) ();
}

//----------------------------------------------------------------------------

VOID
NmiHandler (
  IN OUT AMD_CONFIG_PARAMS *StdHeaderPtr
  )
{
  static const UINT8 opcode [] = {0xCF};  // iret
  ((void (*)(void)) (size_t) opcode) ();
}

//----------------------------------------------------------------------------

VOID
ExecuteHltInstruction (
  IN OUT AMD_CONFIG_PARAMS *StdHeaderPtr
  )
{
  _disable ();
  __halt ();
}

//---------------------------------------------------------------------------

VOID
ExecuteWbinvdInstruction (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  __wbinvd ();
}

//----------------------------------------------------------------------------
