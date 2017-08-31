/* $NoKeywords:$ */
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
 * @e \$Revision: 54577 $   @e \$Date: 2011-06-09 04:28:28 -0600 (Thu, 09 Jun 2011) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

  #if (OPTION_MEMCTLR_DR == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockDR;
    #define MEM_REC_NB_SUPPORT_DR MemRecConstructNBBlockDR,
  #else
    #define MEM_REC_NB_SUPPORT_DR
  #endif
  #if (OPTION_MEMCTLR_RB == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockRb;
    #define MEM_REC_NB_SUPPORT_RB MemRecConstructNBBlockRb,
  #else
    #define MEM_REC_NB_SUPPORT_RB
  #endif
  #if (OPTION_MEMCTLR_DA == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockDA;
    #define MEM_REC_NB_SUPPORT_DA MemRecConstructNBBlockDA,
  #else
    #define MEM_REC_NB_SUPPORT_DA
  #endif
  #if (OPTION_MEMCTLR_NI == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockNi;
    #define MEM_REC_NB_SUPPORT_NI MemRecConstructNBBlockNi,
  #else
    #define MEM_REC_NB_SUPPORT_NI
  #endif
  #if (OPTION_MEMCTLR_PH == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockPh;
    #define MEM_REC_NB_SUPPORT_PH MemRecConstructNBBlockPh,
  #else
    #define MEM_REC_NB_SUPPORT_PH
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
  #if (OPTION_MEMCTLR_OR == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockOr;
    #define MEM_REC_NB_SUPPORT_OR MemRecConstructNBBlockOr,
  #else
    #define MEM_REC_NB_SUPPORT_OR
  #endif

  MEM_REC_NB_CONSTRUCTOR* MemRecNBInstalled[] = {
    MEM_REC_NB_SUPPORT_DR
    MEM_REC_NB_SUPPORT_RB
    MEM_REC_NB_SUPPORT_DA
    MEM_REC_NB_SUPPORT_PH
    MEM_REC_NB_SUPPORT_HY
    MEM_REC_NB_SUPPORT_C32
    MEM_REC_NB_SUPPORT_OR
    MEM_REC_NB_SUPPORT_NI
    NULL
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

  #if OPTION_MEMCTLR_DR
    #define PSC_REC_DR_UDIMM_DDR2
    #define PSC_REC_DR_UDIMM_DDR3    MemRecNGetPsCfgUDIMM3Nb,
    #define PSC_REC_DR_RDIMM_DDR2
    #define PSC_REC_DR_RDIMM_DDR3    MemRecNGetPsCfgRDIMM3Nb,
    #define PSC_REC_DR_SODIMM_DDR2
    #define PSC_REC_DR_SODIMM_DDR3    MemRecNGetPsCfgSODIMM3Nb,
  #endif
  #if ((OPTION_MEMCTLR_DA == TRUE) || (OPTION_MEMCTLR_Ni == TRUE) || (OPTION_MEMCTLR_PH == TRUE) || (OPTION_MEMCTLR_RB == TRUE))
    #define PSC_REC_DA_UDIMM_DDR3    MemRecNGetPsCfgUDIMM3Nb,
    #define PSC_REC_DA_SODIMM_DDR2
    #define PSC_REC_DA_SODIMM_DDR3    MemRecNGetPsCfgSODIMM3Nb,
  #endif
  #if OPTION_MEMCTLR_HY
    #define PSC_REC_HY_UDIMM_DDR3    MemRecNGetPsCfgUDIMM3Nb,
    #define PSC_REC_HY_RDIMM_DDR3    MemRecNGetPsCfgRDIMM3Nb,
  #endif
  #if OPTION_MEMCTLR_C32
    #define PSC_REC_C32_UDIMM_DDR3    MemRecNGetPsCfgUDIMM3Nb,
    #define PSC_REC_C32_RDIMM_DDR3    MemRecNGetPsCfgRDIMM3Nb,
  #endif
  #if OPTION_MEMCTLR_OR
    #define PSC_REC_OR_UDIMM_DDR3    //MemRecNGetPsCfgUDIMM3OR,
    #define PSC_REC_OR_RDIMM_DDR3    //MemRecNGetPsCfgRDIMM3OR,
  #endif

  #ifndef PSC_REC_DR_UDIMM_DDR2
    #define PSC_REC_DR_UDIMM_DDR2
  #endif
  #ifndef PSC_REC_DR_UDIMM_DDR3
    #define PSC_REC_DR_UDIMM_DDR3
  #endif
  #ifndef PSC_REC_DR_RDIMM_DDR2
    #define PSC_REC_DR_RDIMM_DDR2
  #endif
  #ifndef PSC_REC_DR_RDIMM_DDR3
    #define PSC_REC_DR_RDIMM_DDR3
  #endif
  #ifndef PSC_REC_DR_SODIMM_DDR2
    #define PSC_REC_DR_SODIMM_DDR2
  #endif
  #ifndef PSC_REC_DR_SODIMM_DDR3
    #define PSC_REC_DR_SODIMM_DDR3
  #endif
  #ifndef PSC_REC_DA_UDIMM_DDR3
    #define PSC_REC_DA_UDIMM_DDR3
  #endif
  #ifndef PSC_REC_DA_SODIMM_DDR2
    #define PSC_REC_DA_SODIMM_DDR2
  #endif
  #ifndef PSC_REC_DA_SODIMM_DDR3
    #define PSC_REC_DA_SODIMM_DDR3
  #endif
  #ifndef PSC_REC_HY_UDIMM_DDR3
    #define PSC_REC_HY_UDIMM_DDR3
  #endif
  #ifndef PSC_REC_HY_RDIMM_DDR3
    #define PSC_REC_HY_RDIMM_DDR3
  #endif
  #ifndef PSC_REC_C32_UDIMM_DDR3
    #define PSC_REC_C32_UDIMM_DDR3
  #endif
  #ifndef PSC_REC_C32_RDIMM_DDR3
    #define PSC_REC_C32_RDIMM_DDR3
  #endif
  #ifndef PSC_REC_OR_UDIMM_DDR3
    #define PSC_REC_OR_UDIMM_DDR3
  #endif
  #ifndef PSC_REC_OR_RDIMM_DDR3
    #define PSC_REC_OR_RDIMM_DDR3
  #endif

  MEM_PLATFORM_CFG* memRecPlatformTypeInstalled[] = {
    PSC_REC_DR_UDIMM_DDR2
    PSC_REC_DR_RDIMM_DDR2
    PSC_REC_DR_SODIMM_DDR2
    PSC_REC_DR_UDIMM_DDR3
    PSC_REC_DR_RDIMM_DDR3
    PSC_REC_DR_SODIMM_DDR3
    PSC_REC_DA_SODIMM_DDR2
    PSC_REC_DA_UDIMM_DDR3
    PSC_REC_DA_SODIMM_DDR3
    PSC_REC_HY_UDIMM_DDR3
    PSC_REC_HY_RDIMM_DDR3
    PSC_REC_C32_UDIMM_DDR3
    PSC_REC_C32_RDIMM_DDR3
    PSC_REC_OR_UDIMM_DDR3
    PSC_REC_OR_RDIMM_DDR3
    NULL
  };

  /*---------------------------------------------------------------------------------------------------
   * EXTRACTABLE PLATFORM SPECIFIC CONFIGURATION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #define MEM_PSC_REC_FLOW_BLOCK_END NULL
  #define PSC_REC_TBL_END NULL
  #define MEM_REC_PSC_FLOW_DEFTRUE (BOOLEAN (*) (MEM_NB_BLOCK*, MEM_PSC_TABLE_BLOCK *)) MemRecDefTrue


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
  MEM_REC_NB_CONSTRUCTOR* MemRecNBInstalled[] = {
    NULL
  };
  /*----------------------------------------------------------------------
   * DEFAULT PSCFG DEFINITIONS
   *
   *----------------------------------------------------------------------
   */
  MEM_PLATFORM_CFG* memRecPlatformTypeInstalled[] = {
    NULL
  };
  /*----------------------------------------------------------------------
   * EXTRACTABLE PLATFORM SPECIFIC CONFIGURATION
   *
   *----------------------------------------------------------------------
   */
  MEM_PSC_FLOW_BLOCK* memRecPlatSpecFlowArray[] = {
    NULL
  };
#endif
#endif  // _OPTION_MEMORY_RECOVERY_INSTALL_H_
