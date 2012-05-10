/**
 * @file
 *
 * Install of build option: Memory
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 6049 $   @e \$Date: 2008-05-14 01:58:02 -0500 (Wed, 14 May 2008) $
 */
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
 *
 ***************************************************************************/

#ifndef _OPTION_MEMORY_RECOVERY_INSTALL_H_
#define _OPTION_MEMORY_RECOVERY_INSTALL_H_

#if (AGESA_ENTRY_INIT_RECOVERY == TRUE)

  #define MEM_REC_NB_SUPPORT_OR

  #if (OPTION_MEMCTLR_DR == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockDR;
    #define MEM_REC_NB_SUPPORT_DR MemRecConstructNBBlockDR,
  #else
    #define MEM_REC_NB_SUPPORT_DR
  #endif
  #if (OPTION_MEMCTLR_DA == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockDA;
    #define MEM_REC_NB_SUPPORT_DA MemRecConstructNBBlockDA,
  #else
    #define MEM_REC_NB_SUPPORT_DA
  #endif
  #if (OPTION_MEMCTLR_HY == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockHY;
    #define MEM_REC_NB_SUPPORT_HY MemRecConstructNBBlockHY,
  #else
    #define MEM_REC_NB_SUPPORT_HY
  #endif
  #if (OPTION_MEMCTLR_C32 == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockC32;
    #define MEM_REC_NB_SUPPORT_C32 MemRecConstructNBBlockC32,
  #else
    #define MEM_REC_NB_SUPPORT_C32
  #endif
  #if (OPTION_MEMCTLR_LN == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockLN;
    #define MEM_REC_NB_SUPPORT_LN MemRecConstructNBBlockLN,
  #else
    #define MEM_REC_NB_SUPPORT_LN
  #endif
  #if (OPTION_MEMCTLR_ON == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockON;
    #define MEM_REC_NB_SUPPORT_ON MemRecConstructNBBlockON,
  #else
    #define MEM_REC_NB_SUPPORT_ON
  #endif

  MEM_REC_NB_CONSTRUCTOR* MemRecNBInstalled[] = {
    MEM_REC_NB_SUPPORT_DR
    MEM_REC_NB_SUPPORT_DA
    MEM_REC_NB_SUPPORT_HY
    MEM_REC_NB_SUPPORT_C32
    MEM_REC_NB_SUPPORT_LN
    MEM_REC_NB_SUPPORT_OR
    MEM_REC_NB_SUPPORT_ON
    {0, NULL, NULL, NULL, NULL, NULL,},
  };

  #define MEM_REC_TECH_CONSTRUCTOR_DDR2
  #if (OPTION_DDR3 == TRUE)
    extern MEM_REC_TECH_CONSTRUCTOR MemRecConstructTechBlock3;
    #define MEM_REC_TECH_CONSTRUCTOR_DDR3 MemRecConstructTechBlock3,
  #else
    #define MEM_REC_TECH_CONSTRUCTOR_DDR3
  #endif

  MEM_REC_TECH_CONSTRUCTOR* MemRecTechInstalled[] = {
    MEM_REC_TECH_CONSTRUCTOR_DDR3
    MEM_REC_TECH_CONSTRUCTOR_DDR2
    NULL
  };
#else
  /*---------------------------------------------------------------------------------------------------
   * DEFAULT TECHNOLOGY BLOCK
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  MEM_TECH_CONSTRUCTOR* MemRecTechInstalled[] = {    // Types of technology installed
    NULL
  };
  /*---------------------------------------------------------------------------------------------------
   * DEFAULT NORTHBRIDGE SUPPORT LIST
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  MEM_NB_SUPPORT MemRecNBInstalled[] = {
    {0, NULL, NULL, NULL, NULL, NULL,},
  };
#endif
#endif  // _OPTION_MEMORY_RECOVERY_INSTALL_H_
