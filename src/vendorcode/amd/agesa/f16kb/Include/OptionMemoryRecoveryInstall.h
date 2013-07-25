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

#ifndef _OPTION_MEMORY_RECOVERY_INSTALL_H_
#define _OPTION_MEMORY_RECOVERY_INSTALL_H_

#if (AGESA_ENTRY_INIT_RECOVERY == TRUE)

  #if (OPTION_MEMCTLR_TN == TRUE)
    extern MEM_REC_NB_CONSTRUCTOR MemRecConstructNBBlockTN;
    #define MEM_REC_NB_SUPPORT_TN MemRecConstructNBBlockTN,
  #else
    #define MEM_REC_NB_SUPPORT_TN
  #endif

  MEM_REC_NB_CONSTRUCTOR* MemRecNBInstalled[] = {
    MEM_REC_NB_SUPPORT_TN
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

  MEM_PLATFORM_CFG* memRecPlatformTypeInstalled[] = {
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

  #if OPTION_MEMCTLR_TN
    #if OPTION_UDIMMS
      extern PSC_TBL_ENTRY RecTNDramTermTblEntU;
      #define PSC_REC_TBL_TN_UDIMM3_DRAM_TERM  &RecTNDramTermTblEntU,
      extern PSC_TBL_ENTRY RecTNSAOTblEntU3;
      #define PSC_REC_TBL_TN_UDIMM3_SAO  &RecTNSAOTblEntU3,
    #endif
    #if OPTION_SODIMMS
      extern PSC_TBL_ENTRY RecTNSAOTblEntSO3;
      #define PSC_REC_TBL_TN_SODIMM3_SAO  &RecTNSAOTblEntSO3,
      extern PSC_TBL_ENTRY RecTNDramTermTblEntSO;
      #define PSC_REC_TBL_TN_SODIMM3_DRAM_TERM  &RecTNDramTermTblEntSO,
    #endif
    extern PSC_TBL_ENTRY RecTNMR0WrTblEntry;
    extern PSC_TBL_ENTRY RecTNMR0CLTblEntry;
    extern PSC_TBL_ENTRY RecTNDdr3CKETriEnt;
    extern PSC_TBL_ENTRY RecTNOdtPatTblEnt;

    #ifndef PSC_REC_TBL_TN_UDIMM3_DRAM_TERM
      #define PSC_REC_TBL_TN_UDIMM3_DRAM_TERM
    #endif
    #ifndef PSC_REC_TBL_TN_SODIMM3_DRAM_TERM
      #define PSC_REC_TBL_TN_SODIMM3_DRAM_TERM
    #endif
    #ifndef PSC_REC_TBL_TN_SODIMM3_SAO
      #define PSC_REC_TBL_TN_SODIMM3_SAO
    #endif
    #ifndef PSC_REC_TBL_TN_UDIMM3_SAO
      #define PSC_REC_TBL_TN_UDIMM3_SAO
    #endif

    PSC_TBL_ENTRY* memRecPSCTblDramTermArrayTN[] = {
      PSC_REC_TBL_TN_UDIMM3_DRAM_TERM
      PSC_REC_TBL_TN_SODIMM3_DRAM_TERM
      PSC_REC_TBL_END
    };

    PSC_TBL_ENTRY* memRecPSCTblODTPatArrayTN[] = {
      &RecTNOdtPatTblEnt,
      PSC_REC_TBL_END
    };

    PSC_TBL_ENTRY* memRecPSCTblSAOArrayTN[] = {
      PSC_REC_TBL_TN_SODIMM3_SAO
      PSC_REC_TBL_TN_UDIMM3_SAO
      PSC_REC_TBL_END
    };

    PSC_TBL_ENTRY* memRecPSCTblMR0WRArrayTN[] = {
      &RecTNMR0WrTblEntry,
      PSC_REC_TBL_END
    };

    PSC_TBL_ENTRY* memRecPSCTblMR0CLArrayTN[] = {
      &RecTNMR0CLTblEntry,
      PSC_REC_TBL_END
    };

    MEM_PSC_TABLE_BLOCK memRecPSCTblBlockTN = {
      NULL,
      (PSC_TBL_ENTRY **)&memRecPSCTblDramTermArrayTN,
      (PSC_TBL_ENTRY **)&memRecPSCTblODTPatArrayTN,
      (PSC_TBL_ENTRY **)&memRecPSCTblSAOArrayTN,
      (PSC_TBL_ENTRY **)&memRecPSCTblMR0WRArrayTN,
      (PSC_TBL_ENTRY **)&memRecPSCTblMR0CLArrayTN,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
    };
    extern MEM_PSC_FLOW MemPRecGetRttNomWr;
    #define PSC_REC_FLOW_TN_DRAM_TERM   MemPRecGetRttNomWr
    extern MEM_PSC_FLOW MemPRecGetODTPattern;
    #define PSC_REC_FLOW_TN_ODT_PATTERN   MemPRecGetODTPattern
    extern MEM_PSC_FLOW MemPRecGetSAO;
    #define PSC_REC_FLOW_TN_SAO   MemPRecGetSAO
    extern MEM_PSC_FLOW MemPRecGetMR0WrCL;
    #define PSC_REC_FLOW_TN_MR0_WRCL   MemPRecGetMR0WrCL

    MEM_PSC_FLOW_BLOCK memRecPlatSpecFlowTN = {
      &memRecPSCTblBlockTN,
      MEM_REC_PSC_FLOW_DEFTRUE,
      PSC_REC_FLOW_TN_DRAM_TERM,
      PSC_REC_FLOW_TN_ODT_PATTERN,
      PSC_REC_FLOW_TN_SAO,
      PSC_REC_FLOW_TN_MR0_WRCL,
      MEM_REC_PSC_FLOW_DEFTRUE,
      MEM_REC_PSC_FLOW_DEFTRUE,
      MEM_REC_PSC_FLOW_DEFTRUE,
      MEM_REC_PSC_FLOW_DEFTRUE,
      MEM_REC_PSC_FLOW_DEFTRUE,
      MEM_REC_PSC_FLOW_DEFTRUE
    };
    #define MEM_PSC_REC_FLOW_BLOCK_TN &memRecPlatSpecFlowTN,
  #else
    #define MEM_PSC_REC_FLOW_BLOCK_TN
  #endif

  MEM_PSC_FLOW_BLOCK* memRecPlatSpecFlowArray[] = {
    MEM_PSC_REC_FLOW_BLOCK_TN
    MEM_PSC_REC_FLOW_BLOCK_END
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
