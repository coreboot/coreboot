/* $NoKeywords:$ */
/**
 * @file
 *
 * mtot3.c
 *
 * Technology Non-SPD Timings for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 **/
/*****************************************************************************
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
* ***************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mtot3.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_TECH_DDR3_MTOT3_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function adjusts the Twrwr value for DDR3.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTAdjustTwrwr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  DCT_STRUCT *DCTPtr;

  DCTPtr = TechPtr->NBPtr->DCTPtr;

  // For DDR3, value 0000b-0001b and >= 1011b of Twrwr is reserved.
  if (DCTPtr->Timings.Twrwr < 2) {
    DCTPtr->Timings.Twrwr = 2;
  } else if (DCTPtr->Timings.Twrwr > 10) {
    DCTPtr->Timings.Twrwr = 10;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function adjusts the Twrrd value for DDR3.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTAdjustTwrrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  DCT_STRUCT *DCTPtr;

  DCTPtr = TechPtr->NBPtr->DCTPtr;

  // For DDR3, value 0000b, 0001b, and > 1010b of Twrrd is reserved.
  if (DCTPtr->Timings.Twrrd < 2) {
    DCTPtr->Timings.Twrrd = 2;
  } else if (DCTPtr->Timings.Twrrd > 10) {
    DCTPtr->Timings.Twrrd = 10;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets the LD value for DDR3.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  Value of LD
 */

INT8
MemTGetLD3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  INT8 LD;
  MEM_NB_BLOCK *NBPtr;
  NBPtr = TechPtr->NBPtr;
  //
  // For DDR3, BIOS calculates the latency difference (Ld) as equal to read CAS latency minus write CAS
  // latency, in MEMCLKs (see F2x[1, 0]88[Tcl] and F2x[1, 0]84[Tcwl]) which can be a negative or positive
  // value.
  //
  LD = ((INT8) NBPtr->GetBitField (NBPtr, BFTcl) + 4) - ((INT8) NBPtr->GetBitField (NBPtr, BFTcwl) + 5);

  return LD;
}
