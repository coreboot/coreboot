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
 * @e \$Revision: 49896 $   @e \$Date: 2011-03-30 16:18:18 +0800 (Wed, 30 Mar 2011) $
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
    MEM_REC_NB_SUPPORT_RB
    MEM_REC_NB_SUPPORT_DA
    MEM_REC_NB_SUPPORT_PH
    MEM_REC_NB_SUPPORT_HY
    MEM_REC_NB_SUPPORT_C32
    MEM_REC_NB_SUPPORT_LN
    MEM_REC_NB_SUPPORT_OR
    MEM_REC_NB_SUPPORT_ON
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
  #define MEM_REC_PSC_FLOW_DEFTRUE (BOOLEAN (*) (MEM_NB_BLOCK*, MEM_PSC_TABLE_BLOCK *)) memDefTrue

  #if OPTION_MEMCTLR_OR
    #if OPTION_UDIMMS
      #if OPTION_AM3_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY RecDramTermTblEntUAM3;
        #define PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_AM3  &RecDramTermTblEntUAM3,
        extern PSC_TBL_ENTRY RecOdtPat1DTblEntUAM3;
        #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_AM3  &RecOdtPat1DTblEntUAM3,
        extern PSC_TBL_ENTRY RecOdtPat2DTblEntUAM3;
        #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_AM3  &RecOdtPat2DTblEntUAM3,
        extern PSC_TBL_ENTRY RecOdtPat3DTblEntUAM3;
        #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_AM3  &RecOdtPat3DTblEntUAM3,
        extern PSC_TBL_ENTRY RecSAOTblEntUAM3;
        #define PSC_REC_TBL_OR_UDIMM3_SAO_AM3  &RecSAOTblEntUAM3,
      #endif
      #if OPTION_C32_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY RecDramTermTblEntUC32;
        #define PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_C32  &RecDramTermTblEntUC32,
        extern PSC_TBL_ENTRY RecOdtPat1DTblEntUC32;
        #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_C32  &RecOdtPat1DTblEntUC32,
        extern PSC_TBL_ENTRY RecOdtPat2DTblEntUC32;
        #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_C32  &RecOdtPat2DTblEntUC32,
        extern PSC_TBL_ENTRY RecOdtPat3DTblEntUC32;
        #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_C32  &RecOdtPat3DTblEntUC32,
        extern PSC_TBL_ENTRY RecSAOTblEntUC32;
        #define PSC_REC_TBL_OR_UDIMM3_SAO_C32  &RecSAOTblEntUC32,
      #endif
      #if OPTION_G34_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY RecDramTermTblEntUG34;
        #define PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_G34  &RecDramTermTblEntUG34,
        extern PSC_TBL_ENTRY RecOdtPat1DTblEntUG34;
        #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_G34  &RecOdtPat1DTblEntUG34,
        extern PSC_TBL_ENTRY RecOdtPat2DTblEntUG34;
        #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_G34  &RecOdtPat2DTblEntUG34,
        extern PSC_TBL_ENTRY RecOdtPat3DTblEntUG34;
        #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_G34  &RecOdtPat3DTblEntUG34,
        extern PSC_TBL_ENTRY RecSAOTblEntUG34;
        #define PSC_REC_TBL_OR_UDIMM3_SAO_G34  &RecSAOTblEntUG34,
      #endif
    #endif
    #if OPTION_RDIMMS
      #if OPTION_C32_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY RecDramTermTblEntRC32;
        #define PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_C32  &RecDramTermTblEntRC32,
        extern PSC_TBL_ENTRY RecOdtPat1DTblEntRC32;
        #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_C32  &RecOdtPat1DTblEntRC32,
        extern PSC_TBL_ENTRY RecOdtPat2DTblEntRC32;
        #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_C32  &RecOdtPat2DTblEntRC32,
        extern PSC_TBL_ENTRY RecOdtPat3DTblEntRC32;
        #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_C32  &RecOdtPat3DTblEntRC32,
        extern PSC_TBL_ENTRY RecSAOTblEntRC32;
        #define PSC_REC_TBL_OR_RDIMM3_SAO_C32  &RecSAOTblEntRC32,
        extern PSC_TBL_ENTRY RecRC2IBTTblEntRC32;
        #define PSC_REC_TBL_OR_RDIMM3_RC2IBT_C32  &RecRC2IBTTblEntRC32,
    #endif
      #if OPTION_G34_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY RecDramTermTblEntRG34;
        #define PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_G34  &RecDramTermTblEntRG34,
        extern PSC_TBL_ENTRY RecOdtPat1DTblEntRG34;
        #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_G34  &RecOdtPat1DTblEntRG34,
        extern PSC_TBL_ENTRY RecOdtPat2DTblEntRG34;
        #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_G34  &RecOdtPat2DTblEntRG34,
        extern PSC_TBL_ENTRY RecOdtPat3DTblEntRG34;
        #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_G34  &RecOdtPat3DTblEntRG34,
        extern PSC_TBL_ENTRY RecSAOTblEntRG34;
        #define PSC_REC_TBL_OR_RDIMM3_SAO_G34  &RecSAOTblEntRG34,
        extern PSC_TBL_ENTRY RecRC2IBTTblEntRG34;
        #define PSC_REC_TBL_OR_RDIMM3_RC2IBT_G34  &RecRC2IBTTblEntRG34,
      #endif
    #endif
    //#if OPTION_SODIMMS
    //#endif
    //#if OPTION_LRDIMMS
    //#endif
    extern PSC_TBL_ENTRY RecMR0WrTblEntry;
    #define PSC_REC_TBL_OR_MR0_WR  &RecMR0WrTblEntry,
    extern PSC_TBL_ENTRY RecMR0CLTblEntry;
    #define PSC_REC_TBL_OR_MR0_CL  &RecMR0CLTblEntry,

    #ifndef PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_AM3
      #define PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_C32
      #define PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_C32
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_G34
      #define PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_G34
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_AM3
      #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_C32
      #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_C32
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_G34
      #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_G34
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_AM3
      #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_C32
      #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_C32
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_G34
      #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_G34
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_AM3
      #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_C32
      #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_C32
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_G34
      #define PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_G34
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_SAO_AM3
      #define PSC_REC_TBL_OR_UDIMM3_SAO_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_SAO_C32
      #define PSC_REC_TBL_OR_UDIMM3_SAO_C32
    #endif
    #ifndef PSC_REC_TBL_OR_UDIMM3_SAO_G34
      #define PSC_REC_TBL_OR_UDIMM3_SAO_G34
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_AM3
      #define PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_C32
      #define PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_C32
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_G34
      #define PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_G34
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_AM3
      #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_C32
      #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_C32
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_G34
      #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_G34
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_AM3
      #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_C32
      #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_C32
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_G34
      #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_G34
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_AM3
      #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_C32
      #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_C32
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_G34
      #define PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_G34
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_SAO_AM3
      #define PSC_REC_TBL_OR_RDIMM3_SAO_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_SAO_C32
      #define PSC_REC_TBL_OR_RDIMM3_SAO_C32
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_SAO_G34
      #define PSC_REC_TBL_OR_RDIMM3_SAO_G34
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_RC2IBT_AM3
      #define PSC_REC_TBL_OR_RDIMM3_RC2IBT_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_RC2IBT_C32
      #define PSC_REC_TBL_OR_RDIMM3_RC2IBT_C32
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_RC2IBT_G34
      #define PSC_REC_TBL_OR_RDIMM3_RC2IBT_G34
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_RC10OPSPD_AM3
      #define PSC_REC_TBL_OR_RDIMM3_RC10OPSPD_AM3
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_RC10OPSPD_C32
      #define PSC_REC_TBL_OR_RDIMM3_RC10OPSPD_C32
    #endif
    #ifndef PSC_REC_TBL_OR_RDIMM3_RC10OPSPD_G34
      #define PSC_REC_TBL_OR_RDIMM3_RC10OPSPD_G34
    #endif

    PSC_TBL_ENTRY* memRecPSCTblDramTermArrayOR[] = {
      PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_AM3
      PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_C32
      PSC_REC_TBL_OR_UDIMM3_DRAM_TERM_G34
      PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_AM3
      PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_C32
      PSC_REC_TBL_OR_RDIMM3_DRAM_TERM_G34
      PSC_REC_TBL_END
    };

    PSC_TBL_ENTRY* memRecPSCTblODTPatArrayOR[] = {
      PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_AM3
      PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_AM3
      PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_AM3
      PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_AM3
      PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_AM3
      PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_AM3
      PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_C32
      PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_C32
      PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_C32
      PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_C32
      PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_C32
      PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_C32
      PSC_REC_TBL_OR_UDIMM3_ODT_PAT_1D_G34
      PSC_REC_TBL_OR_UDIMM3_ODT_PAT_2D_G34
      PSC_REC_TBL_OR_UDIMM3_ODT_PAT_3D_G34
      PSC_REC_TBL_OR_RDIMM3_ODT_PAT_1D_G34
      PSC_REC_TBL_OR_RDIMM3_ODT_PAT_2D_G34
      PSC_REC_TBL_OR_RDIMM3_ODT_PAT_3D_G34
      PSC_REC_TBL_END
    };

    PSC_TBL_ENTRY* memRecPSCTblSAOArrayOR[] = {
      PSC_REC_TBL_OR_UDIMM3_SAO_AM3
      PSC_REC_TBL_OR_UDIMM3_SAO_C32
      PSC_REC_TBL_OR_UDIMM3_SAO_G34
      PSC_REC_TBL_OR_RDIMM3_SAO_AM3
      PSC_REC_TBL_OR_RDIMM3_SAO_C32
      PSC_REC_TBL_OR_RDIMM3_SAO_G34
      PSC_REC_TBL_END
    };

    PSC_TBL_ENTRY* memRecPSCTblMR0WRArrayOR[] = {
      PSC_REC_TBL_OR_MR0_WR
      PSC_REC_TBL_END
    };

    PSC_TBL_ENTRY* memRecPSCTblMR0CLArrayOR[] = {
      PSC_REC_TBL_OR_MR0_CL
      PSC_REC_TBL_END
    };

    PSC_TBL_ENTRY* memRecPSCTblRC2IBTArrayOR[] = {
      PSC_REC_TBL_OR_RDIMM3_RC2IBT_AM3
      PSC_REC_TBL_OR_RDIMM3_RC2IBT_C32
      PSC_REC_TBL_OR_RDIMM3_RC2IBT_G34
      PSC_REC_TBL_END
    };

    MEM_PSC_TABLE_BLOCK memRecPSCTblBlockOr = {
      NULL,
      (PSC_TBL_ENTRY **)&memRecPSCTblDramTermArrayOR,
      (PSC_TBL_ENTRY **)&memRecPSCTblODTPatArrayOR,
      (PSC_TBL_ENTRY **)&memRecPSCTblSAOArrayOR,
      (PSC_TBL_ENTRY **)&memRecPSCTblMR0WRArrayOR,
      (PSC_TBL_ENTRY **)&memRecPSCTblMR0CLArrayOR,
      (PSC_TBL_ENTRY **)&memRecPSCTblRC2IBTArrayOR,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
    };

    extern MEM_PSC_FLOW MemPRecGetRttNomWr;
    #define PSC_REC_FLOW_OR_DRAM_TERM   MemPRecGetRttNomWr
    extern MEM_PSC_FLOW MemPRecGetODTPattern;
    #define PSC_REC_FLOW_OR_ODT_PATTERN   MemPRecGetODTPattern
    extern MEM_PSC_FLOW MemPRecGetSAO;
    #define PSC_REC_FLOW_OR_SAO   MemPRecGetSAO
    extern MEM_PSC_FLOW MemPRecGetMR0WrCL;
    #define PSC_REC_FLOW_OR_MR0_WRCL   MemPRecGetMR0WrCL
    #if OPTION_RDIMMS
      extern MEM_PSC_FLOW MemPRecGetRC2IBT;
      #define PSC_REC_FLOW_OR_RC2_IBT    MemPRecGetRC2IBT
    #endif
    //#if OPTION_LRDIMMS
    extern MEM_PSC_FLOW MemPRecGetLRIBT;
    #define PSC_REC_FLOW_OR_LR_IBT   MemPRecGetLRIBT
    extern MEM_PSC_FLOW MemPRecGetLRNPR;
    #define PSC_REC_FLOW_OR_LR_NPR   MemPRecGetLRNPR
    extern MEM_PSC_FLOW MemPRecGetLRNLR;
    #define PSC_REC_FLOW_OR_LR_NLR  MemPRecGetLRNLR
    //#endif
    #ifndef PSC_REC_FLOW_OR_DRAM_TERM
      #define PSC_REC_FLOW_OR_DRAM_TERM   MEM_REC_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_REC_FLOW_OR_ODT_PATTERN
      #define PSC_REC_FLOW_OR_ODT_PATTERN   MEM_REC_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_REC_FLOW_OR_SAO
      #define PSC_REC_FLOW_OR_SAO   MEM_REC_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_REC_FLOW_OR_MR0_WRCL
      #define PSC_REC_FLOW_OR_MR0_WRCL   MEM_REC_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_REC_FLOW_OR_RC2_IBT
      #define PSC_REC_FLOW_OR_RC2_IBT   MEM_REC_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_REC_FLOW_OR_LR_IBT
      #define PSC_REC_FLOW_OR_LR_IBT   MEM_REC_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_REC_FLOW_OR_LR_NPR
      #define PSC_REC_FLOW_OR_LR_NPR   MEM_REC_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_REC_FLOW_OR_LR_NLR
      #define PSC_REC_FLOW_OR_LR_NLR   MEM_REC_PSC_FLOW_DEFTRUE
    #endif
    MEM_PSC_FLOW_BLOCK memRecPlatSpecFlowOR = {
      &memRecPSCTblBlockOr,
      NULL,
      PSC_REC_FLOW_OR_DRAM_TERM,
      PSC_REC_FLOW_OR_ODT_PATTERN,
      PSC_REC_FLOW_OR_SAO,
      PSC_REC_FLOW_OR_MR0_WRCL,
      PSC_REC_FLOW_OR_RC2_IBT,
      NULL,
      PSC_REC_FLOW_OR_LR_IBT,
      PSC_REC_FLOW_OR_LR_NPR,
      PSC_REC_FLOW_OR_LR_NLR
    };
    #define MEM_PSC_REC_FLOW_BLOCK_OR &memRecPlatSpecFlowOR,
  #else
    #define MEM_PSC_REC_FLOW_BLOCK_OR
  #endif

  MEM_PSC_FLOW_BLOCK* memRecPlatSpecFlowArray[] = {
    MEM_PSC_REC_FLOW_BLOCK_OR
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
  MEM_NB_SUPPORT* MemRecNBInstalled[] = {
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
