/**
 * @file
 *
 * mtot2.c
 *
 * Technology Non-SPD Timings for DDR2
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR2)
 * @e \$Revision: 44323 $ @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
* 
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
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mtot2.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_TECH_DDR2_MTOT2_FILECODE
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
 *   This function adjusts the Twrwr value for DDR2.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTAdjustTwrwr2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  DCT_STRUCT *DCTPtr;

  DCTPtr = TechPtr->NBPtr->DCTPtr;

  // For DDR2, 1 clock has encoded value of 0.
  // Need to transfer clk value to encoded value.
  if (DCTPtr->Timings.Twrwr >= 1) {
    DCTPtr->Timings.Twrwr -= 1;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function adjusts the Twrrd value for DDR2.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTAdjustTwrrd2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  DCT_STRUCT *DCTPtr;

  DCTPtr = TechPtr->NBPtr->DCTPtr;

  // For DDR2, 1 clock has encoded value of 0.
  // Need to transfer clk value to encoded value.
  if (DCTPtr->Timings.Twrrd >= 1) {
    DCTPtr->Timings.Twrrd -= 1;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets the LD value for DDR2
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  value of LD
 */

INT8
MemTGetLD2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  INT8 LD;

  // For DDR2, LD is always one clock (For DDR2, Tcwl is always Tcl minus 1).
  LD = 1;

  return LD;
}



