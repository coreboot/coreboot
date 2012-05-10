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
ExecuteFinalHltInstruction (
  IN       AP_MTRR_SETTINGS  *ApMtrrSettingsList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

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


//----------------------------------------------------------------------------

STATIC
VOID
PrimaryCoreFunctions (AP_MTRR_SETTINGS  *ApMtrrSettingsList)
   {
   UINT64 data;
   UINT32 msrno;
   // Configure the MTRRs on the AP so
   // when it runs remote code it will execute
   // out of RAM instead of ROM.
   // Disable MTRRs and turn on modification enable bit

   data = __readmsr (0xC0010010);         // MTRR_SYS_CFG
   data &= ~(1 << 18);                    // MtrrFixDramEn
   data &= ~(1 << 20);                    // MtrrVarDramEn
   data |= (1 << 19);                     // MtrrFixDramModEn
   data |= (1 << 17);                     // SysUcLockEn
   __writemsr (0xC0010010, data);

   // Set 7FFFh-00000h and 9FFFFh-80000h as WB DRAM
   __writemsr (0x250, 0x1E1E1E1E1E1E1E1Eull); // AMD_MTRR_FIX64k_00000
   __writemsr (0x258, 0x1E1E1E1E1E1E1E1Eull); // AMD_MTRR_FIX16k_80000

   // Set BFFFFh-A0000h, DFFFFh-C0000h as Uncacheable Memory-mapped IO
   __writemsr (0x259, 0);                 // AMD_AP_MTRR_FIX16k_A0000
   __writemsr (0x268, 0);                 // AMD_MTRR_FIX4k_C0000
   __writemsr (0x269, 0);                 // AMD_MTRR_FIX4k_C8000
   __writemsr (0x26A, 0);                 // AMD_MTRR_FIX4k_D0000
   __writemsr (0x26B, 0);                 // AMD_MTRR_FIX4k_D8000

   // Set FFFFFh-E0000h as Uncacheable Memory
   for (msrno = 0x26C; msrno <= 0x26F; msrno++)
      __writemsr (msrno, 0x1818181818181818ull);

   // If IBV provided settings for Fixed-Sized MTRRs,
   // overwrite the default settings.
   if ((uintptr_t) ApMtrrSettingsList != 0 && (uintptr_t) ApMtrrSettingsList != 0xFFFFFFFF)
      {
      int index;
      for (index = 0; ApMtrrSettingsList [index].MsrAddr != CPU_LIST_TERMINAL; index++)
         __writemsr (ApMtrrSettingsList [index].MsrAddr, ApMtrrSettingsList [index].MsrData);
      }

   // restore variable MTTR6 and MTTR7 to default states
   for (msrno = 0x20F; msrno <= 0x20C; msrno--)  // decrement so that the pair is disable before the base is cleared
      __writemsr (msrno, 0);

   // Enable fixed-range and variable-range MTRRs
   // Set Fixed-Range Enable (FE) and MTRR Enable (E) bits
   __writemsr (0x2FF, __readmsr (0x2FF) | 0xC00);

   // Enable Top-of-Memory setting
   // Enable use of RdMem/WrMem bits attributes
   data = __readmsr (0xC0010010);         // MTRR_SYS_CFG
   data |= (1 << 18);                     // MtrrFixDramEn
   data |= (1 << 20);                     // MtrrVarDramEn
   data &= ~(1 << 19);                    // MtrrFixDramModEn
   __writemsr (0xC0010010, data);
   }

//----------------------------------------------------------------------------

VOID
ExecuteFinalHltInstruction (
  IN       AP_MTRR_SETTINGS  *ApMtrrSettingsList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
   int abcdRegs [4];
   UINT32 cr0val;
   UINT64 data;

   cr0val = __readcr0 ();
    //Make sure cache is disabled for all APs
    __writecr0 (cr0val | 0x60000000);

    //Configure the MTRRs on the AP so when it runs remote code it will execute
    //out of RAM instead of ROM.
    PrimaryCoreFunctions (ApMtrrSettingsList);

   // Make sure not to touch any Shared MSR from this point on

   // Restore settings that were temporarily overridden for the cache as ram phase
   data = __readmsr (0xC0011022);      // MSR_DC_CFG
   data &= ~(1 << 4);                  // DC_DIS_SPEC_TLB_RLD
   data &= ~(1 << 8);                  // DIS_CLR_WBTOL2_SMC_HIT
   data &= ~(1 << 13);                 // DIS_HW_PF
   __writemsr (0xC0011022, data);

   data = __readmsr (0xC0011021);      // MSR_IC_CFG - C001_1021
   data &= ~(1 << 9);                  // IC_DIS_SPEC_TLB_RLD
   __writemsr (0xC0011021, data);

   // AMD_DISABLE_STACK_FAMILY_HOOK
   __cpuid (abcdRegs, 1);
   if ((abcdRegs [0] >> 20) == 1) //-----family 10h (Hydra) only-----
      {
      data = __readmsr (0xC0011022);
      data &= ~(1 << 4);
      data &= ~(1 << 8);
      data &= ~(1 << 13);
      __writemsr (0xC0011022, data);

      data = __readmsr (0xC0011021);
      data &= ~(1 << 14);
      data &= ~(1 << 9);
      __writemsr (0xC0011021, data);

      data = __readmsr (0xC001102A);
      data &= ~(1 << 15);
      data &= ~(1ull << 35);
      __writemsr (0xC001102A, data);
      }
   else if ((abcdRegs [0] >> 20) == 6) //-----family 15h (Orochi) only-----
      {
      data = __readmsr (0xC0011020);
      data &= ~(1 << 28);
      __writemsr (0xC0011020, data);

      data = __readmsr (0xC0011021);
      data &= ~(1 << 9);
      __writemsr (0xC0011021, data);

      data = __readmsr (0xC0011022);
      data &= ~(1 << 4);
      data &= ~(1l << 13);
      __writemsr (0xC0011022, data);
      }

   for (;;)
     {
     _disable ();
     __halt ();
     }
  }

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
