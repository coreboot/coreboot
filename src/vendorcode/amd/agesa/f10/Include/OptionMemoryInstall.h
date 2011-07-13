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

#ifndef _OPTION_MEMORY_INSTALL_H_
#define _OPTION_MEMORY_INSTALL_H_

/*-------------------------------------------------------------------------------
 *  This option file is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */

/*----------------------------------------------------------------------------------
 * FEATURE BLOCK FUNCTIONS
 *
 *  This section defines function names that depend upon options that are selected
 *  in the platform solution install file.
 */
BOOLEAN MemFDefRet (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return FALSE;
}

BOOLEAN MemMDefRet (
  IN   MEM_MAIN_DATA_BLOCK *MMPtr
  )
{
  return TRUE;
}

STATIC BOOLEAN MemMDefRetFalse (
  IN   MEM_MAIN_DATA_BLOCK *MMPtr
  )
{
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the northbridge block for dimm identification translator
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *     @param[in,out]   NodeID   - ID of current node to construct
 *     @return          TRUE     - This is the correct constructor for the targeted node.
 *     @return          FALSE    - This isn't the correct constructor for the targeted node.
 */
#if AGESA_ENTRY_INIT_GENERAL_SERVICES == FALSE
STATIC BOOLEAN MemNIdentifyDimmConstructorRetDef (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  return FALSE;
}
#endif

/*----------------------------------------------------------------------------------
 * TABLE FEATURE BLOCK FUNCTIONS
 *
 *  This section defines function names that depend upon options that are selected
 *  in the platform solution install file.
 */
UINT8 MemFTableDefRet (
  IN OUT   MEM_TABLE_ALIAS **MTPtr
  )
{
  return 0;
}
/*----------------------------------------------------------------------------------
 * FEATURE S3 BLOCK FUNCTIONS
 *
 *  This section defines function names that depend upon options that are selected
 *  in the platform solution install file.
 */
BOOLEAN MemFS3DefConstructorRet (
  IN OUT   VOID *S3NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  return TRUE;
}

#if (OPTION_MEMCTLR_DR == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockDr;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DR MemS3ResumeConstructNBBlockDr
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DR MemFS3DefConstructorRet
    #endif
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorDr;
    #define MEM_IDENDIMM_DR MemNIdentifyDimmConstructorDr
  #else
    #define MEM_IDENDIMM_DR MemNIdentifyDimmConstructorRetDef
  #endif
#endif

#if (OPTION_MEMCTLR_DA == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      #if (OPTION_MEMCTLR_Ni == TRUE)
        extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockNi;
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA MemS3ResumeConstructNBBlockNi
      #else
        extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockDA;
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA MemS3ResumeConstructNBBlockDA
      #endif
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA MemFS3DefConstructorRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA MemFS3DefConstructorRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorDA;
    #define MEM_IDENDIMM_DA MemNIdentifyDimmConstructorDA
  #else
    #define MEM_IDENDIMM_DA MemNIdentifyDimmConstructorRetDef
  #endif
#endif

#if (OPTION_MEMCTLR_OR == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockOr;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_OR MemS3ResumeConstructNBBlockOr
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_OR MemFS3DefConstructorRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_OR MemFS3DefConstructorRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorOr;
    #define MEM_IDENDIMM_OR MemNIdentifyDimmConstructorOr
  #else
    #define MEM_IDENDIMM_OR MemNIdentifyDimmConstructorRetDef
  #endif
#endif

#if (OPTION_MEMCTLR_HY == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockHy;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_HY MemS3ResumeConstructNBBlockHy
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_HY MemFS3DefConstructorRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_HY MemFS3DefConstructorRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorHy;
    #define MEM_IDENDIMM_HY MemNIdentifyDimmConstructorHy
  #else
    #define MEM_IDENDIMM_HY MemNIdentifyDimmConstructorRetDef
  #endif
#endif

#if (OPTION_MEMCTLR_C32 == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockC32;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_C32 MemS3ResumeConstructNBBlockC32
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_C32 MemFS3DefConstructorRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_C32 MemFS3DefConstructorRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorC32;
    #define MEM_IDENDIMM_C32 MemNIdentifyDimmConstructorC32
  #else
    #define MEM_IDENDIMM_C32 MemNIdentifyDimmConstructorRetDef
  #endif
#endif

#if (OPTION_MEMCTLR_LN == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockLN;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_LN MemS3ResumeConstructNBBlockLN
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_LN MemFS3DefConstructorRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_LN MemFS3DefConstructorRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorLN;
    #define MEM_IDENDIMM_LN MemNIdentifyDimmConstructorLN
  #else
    #define MEM_IDENDIMM_LN MemNIdentifyDimmConstructorRetDef
  #endif
#endif

#if (OPTION_MEMCTLR_ON == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockON;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_ON MemS3ResumeConstructNBBlockON
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_ON MemFS3DefConstructorRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_ON MemFS3DefConstructorRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorON;
    #define MEM_IDENDIMM_ON MemNIdentifyDimmConstructorON
  #else
    #define MEM_IDENDIMM_ON MemNIdentifyDimmConstructorRetDef
  #endif
#endif

/*----------------------------------------------------------------------------------
 * NORTHBRIDGE BLOCK CONSTRUCTOR AND INITIALIZER FUNCTION DEFAULT ASSIGNMENTS
 *
 *----------------------------------------------------------------------------------
*/
#define MEM_NB_SUPPORT_DR
#define MEM_NB_SUPPORT_DA
#define MEM_NB_SUPPORT_HY
#define MEM_NB_SUPPORT_LN
#define MEM_NB_SUPPORT_OR
#define MEM_NB_SUPPORT_C32
#define MEM_NB_SUPPORT_ON
#define MEM_NB_SUPPORT_END { MEM_NB_SUPPORT_STRUCT_VERSION, 0, 0, 0, 0, 0 }

#if (AGESA_ENTRY_INIT_POST == TRUE)
  /*----------------------------------------------------------------------------------
   * FLOW CONTROL FUNCTION
   *
   *  This section selects the function that controls the memory initialization sequence
   *  based upon the number of processor families that the BIOS will support.
   */

  #if (OPTION_MEMCTLR_DR == TRUE)
    extern MEM_MAIN_FLOW_CONTROL MemMFlowDr;
    #define MEM_MAIN_FLOW_CONTROL_PTR MemMFlowDr
  #elif (OPTION_MEMCTLR_DA == TRUE)
    extern MEM_MAIN_FLOW_CONTROL MemMFlowDA;
    #define MEM_MAIN_FLOW_CONTROL_PTR MemMFlowDA
  #elif (OPTION_MEMCTLR_HY == TRUE)
    extern MEM_MAIN_FLOW_CONTROL MemMFlowHy;
    #define MEM_MAIN_FLOW_CONTROL_PTR MemMFlowHy
  #elif (OPTION_MEMCTLR_LN == TRUE)
    extern MEM_MAIN_FLOW_CONTROL MemMFlowLN;
    #define MEM_MAIN_FLOW_CONTROL_PTR MemMFlowLN
  #elif (OPTION_MEMCTLR_ON == TRUE)
    extern MEM_MAIN_FLOW_CONTROL MemMFlowON;
    #define MEM_MAIN_FLOW_CONTROL_PTR MemMFlowON
  #elif (OPTION_MEMCTLR_C32 == TRUE)
    extern MEM_MAIN_FLOW_CONTROL MemMFlowC32;
    #define MEM_MAIN_FLOW_CONTROL_PTR MemMFlowC32
  #else
    #error "Unknown memory initialization sequence for this processor family."
  #endif
  MEM_MAIN_FLOW_CONTROL* MemMainFlowControlPtr = MEM_MAIN_FLOW_CONTROL_PTR;

  #if (OPTION_ONLINE_SPARE == TRUE)
    extern OPTION_MEM_FEATURE_MAIN MemMOnlineSpare;
    #define MEM_MAIN_FEATURE_ONLINE_SPARE  MemMOnlineSpare
    extern OPTION_MEM_FEATURE_NB MemFOnlineSpare;
    #define MEM_FEATURE_ONLINE_SPARE  MemFOnlineSpare
  #else
    #define MEM_MAIN_FEATURE_ONLINE_SPARE  MemMDefRet
    #define MEM_FEATURE_ONLINE_SPARE  MemFDefRet
  #endif

  #if (OPTION_MEM_RESTORE == TRUE)
    extern OPTION_MEM_FEATURE_MAIN MemMContextSave;
    extern OPTION_MEM_FEATURE_MAIN MemMContextRestore;
    #define MEM_MAIN_FEATURE_MEM_SAVE     MemMContextSave
    #define MEM_MAIN_FEATURE_MEM_RESTORE  MemMContextRestore
  #else
    #define MEM_MAIN_FEATURE_MEM_SAVE     MemMDefRet
    #define MEM_MAIN_FEATURE_MEM_RESTORE  MemMDefRetFalse
  #endif

  #if (OPTION_BANK_INTERLEAVE == TRUE)
    extern OPTION_MEM_FEATURE_NB MemFInterleaveBanks;
    #define MEM_FEATURE_BANK_INTERLEAVE  MemFInterleaveBanks
    extern OPTION_MEM_FEATURE_NB MemFUndoInterleaveBanks;
    #define MEM_FEATURE_UNDO_BANK_INTERLEAVE MemFUndoInterleaveBanks
  #else
    #define MEM_FEATURE_BANK_INTERLEAVE  MemFDefRet
    #define MEM_FEATURE_UNDO_BANK_INTERLEAVE MemFDefRet
  #endif

  #if (OPTION_NODE_INTERLEAVE == TRUE)
    extern OPTION_MEM_FEATURE_MAIN MemMInterleaveNodes;
    #define MEM_MAIN_FEATURE_NODE_INTERLEAVE  MemMInterleaveNodes
    extern OPTION_MEM_FEATURE_NB MemFCheckInterleaveNodes;
    extern OPTION_MEM_FEATURE_NB MemFInterleaveNodes;
    #define MEM_FEATURE_NODE_INTERLEAVE_CHECK  MemFCheckInterleaveNodes
    #define MEM_FEATURE_NODE_INTERLEAVE  MemFInterleaveNodes
  #else
    #define MEM_FEATURE_NODE_INTERLEAVE_CHECK  MemFDefRet
    #define MEM_FEATURE_NODE_INTERLEAVE  MemFDefRet
    #define MEM_MAIN_FEATURE_NODE_INTERLEAVE  MemMDefRet
  #endif

  #if (OPTION_DCT_INTERLEAVE == TRUE)
    extern OPTION_MEM_FEATURE_NB MemFInterleaveChannels;
    #define MEM_FEATURE_CHANNEL_INTERLEAVE  MemFInterleaveChannels
  #else
    #define MEM_FEATURE_CHANNEL_INTERLEAVE  MemFDefRet
  #endif

  #if (OPTION_ECC == TRUE)
    extern OPTION_MEM_FEATURE_MAIN MemMEcc;
    #define MEM_MAIN_FEATURE_ECC  MemMEcc
    extern OPTION_MEM_FEATURE_NB MemFCheckECC;
    extern OPTION_MEM_FEATURE_NB MemFInitECC;
    #define MEM_FEATURE_CK_ECC   MemFCheckECC
    #define MEM_FEATURE_ECC   MemFInitECC
  #else
    #define MEM_MAIN_FEATURE_ECC  MemMDefRet
    #define MEM_FEATURE_CK_ECC   MemFDefRet
    #define MEM_FEATURE_ECC   MemFDefRet
  #endif

  #if (OPTION_EMP == TRUE)
    extern OPTION_MEM_FEATURE_NB MemFInitEMP;
    #define MEM_FEATURE_EMP   MemFInitEMP
  #else
    #define MEM_FEATURE_EMP   MemFDefRet
  #endif

  extern OPTION_MEM_FEATURE_MAIN MemMMctMemClr;
  #define MEM_MAIN_FEATURE_MEM_CLEAR  MemMMctMemClr

  #if (OPTION_DMI == TRUE)
    #if (OPTION_DDR3 == TRUE)
      extern OPTION_MEM_FEATURE_MAIN MemFDMISupport3;
      #define MEM_MAIN_FEATURE_MEM_DMI MemFDMISupport3
    #else
      extern OPTION_MEM_FEATURE_MAIN MemFDMISupport2;
      #define MEM_MAIN_FEATURE_MEM_DMI MemFDMISupport2
    #endif
  #else
    #define MEM_MAIN_FEATURE_MEM_DMI MemMDefRet
  #endif

  #if (OPTION_DDR3 == TRUE)
    extern OPTION_MEM_FEATURE_NB MemFOnDimmThermal;
    extern OPTION_MEM_FEATURE_MAIN MemMLvDdr3;
    extern OPTION_MEM_FEATURE_NB MemFLvDdr3;
    #define MEM_FEATURE_ONDIMMTHERMAL MemFOnDimmThermal
    #define MEM_MAIN_FEATURE_LVDDR3 MemMLvDdr3
    #define MEM_FEATURE_LVDDR3 MemFLvDdr3
  #else
    #define MEM_FEATURE_ONDIMMTHERMAL MemFDefRet
    #define MEM_MAIN_FEATURE_LVDDR3 MemMDefRet
    #define MEM_FEATURE_LVDDR3 MemFDefRet
  #endif

  extern OPTION_MEM_FEATURE_NB MemFInterleaveRegion;
  #define MEM_FEATURE_REGION_INTERLEAVE    MemFInterleaveRegion

  extern OPTION_MEM_FEATURE_MAIN MemMUmaAlloc;
  #define MEM_MAIN_FEATURE_UMAALLOC   MemMUmaAlloc

  #if (OPTION_PARALLEL_TRAINING == TRUE)
    extern OPTION_MEM_FEATURE_MAIN MemMParallelTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMParallelTraining
  #else
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
  #endif

  #if (OPTION_DIMM_EXCLUDE == TRUE)
    extern OPTION_MEM_FEATURE_MAIN MemMRASExcludeDIMM;
    #define MEM_MAIN_FEATURE_DIMM_EXCLUDE  MemMRASExcludeDIMM
    extern OPTION_MEM_FEATURE_NB MemFRASExcludeDIMM;
    #define MEM_FEATURE_DIMM_EXCLUDE  MemFRASExcludeDIMM
  #else
    #define MEM_FEATURE_DIMM_EXCLUDE  MemFDefRet
    #define MEM_MAIN_FEATURE_DIMM_EXCLUDE  MemMDefRet
  #endif

  /*----------------------------------------------------------------------------------
   * TECHNOLOGY BLOCK CONSTRUCTOR FUNCTION ASSIGNMENTS
   *
   *----------------------------------------------------------------------------------
  */
  #if OPTION_DDR2 == TRUE
    extern MEM_TECH_CONSTRUCTOR MemConstructTechBlock2;
    #define MEM_TECH_CONSTRUCTOR_DDR2 MemConstructTechBlock2,
    #if (OPTION_HW_DRAM_INIT == TRUE)
      extern MEM_TECH_FEAT MemTDramInitHw;
      #define MEM_TECH_FEATURE_HW_DRAMINIT  MemTDramInitHw
    #else
      #define MEM_TECH_FEATURE_HW_DRAMINIT  MemTFeatDef
    #endif
    #if (OPTION_SW_DRAM_INIT == TRUE)
      #define MEM_TECH_FEATURE_SW_DRAMINIT  MemTFeatDef
    #else
      #define MEM_TECH_FEATURE_SW_DRAMINIT  MemTFeatDef
    #endif
  #else
    #define MEM_TECH_CONSTRUCTOR_DDR2
  #endif
  #if OPTION_DDR3 == TRUE
    extern MEM_TECH_CONSTRUCTOR MemConstructTechBlock3;
    #define MEM_TECH_CONSTRUCTOR_DDR3 MemConstructTechBlock3,
    #if (OPTION_HW_DRAM_INIT == TRUE)
      extern MEM_TECH_FEAT MemTDramInitHw;
      #define MEM_TECH_FEATURE_HW_DRAMINIT  MemTDramInitHw
    #else
      #define  MEM_TECH_FEATURE_HW_DRAMINIT MemTFeatDef
    #endif
    #if (OPTION_SW_DRAM_INIT == TRUE)
//      extern MEM_TECH_FEAT MemTDramInitSw3;
      #define MEM_TECH_FEATURE_SW_DRAMINIT  MemTDramInitSw3
    #else
      #define MEM_TECH_FEATURE_SW_DRAMINIT  MemTFeatDef
    #endif
  #else
    #define MEM_TECH_CONSTRUCTOR_DDR3
  #endif

  /*---------------------------------------------------------------------------------------------------
   * FEATURE BLOCKS
   *
   *  This section instantiates a feature block structure for each memory controller installed
   *  by the platform solution install file.
   *---------------------------------------------------------------------------------------------------
   */

  /*---------------------------------------------------------------------------------------------------
   * DEERHOUND FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_DR == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #if (OPTION_CONTINOUS_PATTERN_GENERATION == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitCPGNb;
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGNb
    #else
      #define MEM_TECH_FEATURE_CPG    MemFDefRet
    #endif

    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    extern OPTION_MEM_FEATURE_NB MemFStandardTraining;
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    MEM_FEAT_BLOCK_NB  MemFeatBlockDr = {
      MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
      MEM_FEATURE_ONLINE_SPARE,
      MEM_FEATURE_BANK_INTERLEAVE,
      MEM_FEATURE_UNDO_BANK_INTERLEAVE,
      MEM_FEATURE_NODE_INTERLEAVE_CHECK,
      MEM_FEATURE_NODE_INTERLEAVE,
      MEM_FEATURE_CHANNEL_INTERLEAVE,
      MemFDefRet,
      MEM_FEATURE_CK_ECC,
      MEM_FEATURE_ECC,
      MEM_FEATURE_TRAINING,
      MEM_FEATURE_LVDDR3,
      MemFDefRet,
      MEM_TECH_FEATURE_DRAMINIT,
      MEM_FEATURE_DIMM_EXCLUDE,
      MEM_TECH_FEATURE_CPG
    };

    #undef MEM_NB_SUPPORT_DR
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockDR;
    extern MEM_INITIALIZER MemNInitDefaultsDR;


    #define MEM_NB_SUPPORT_DR { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockDR, MemNInitDefaultsDR, &MemFeatBlockDr, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DR, MEM_IDENDIMM_DR },
  #endif // OPTION_MEMCTRL_DR

  /*---------------------------------------------------------------------------------------------------
   * DASHOUND FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_DA == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #if (OPTION_CONTINOUS_PATTERN_GENERATION == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitCPGNb;
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGNb
    #else
      #define MEM_TECH_FEATURE_CPG    MemFDefRet
    #endif

    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    extern OPTION_MEM_FEATURE_NB MemFStandardTraining;
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    #if (OPTION_MEMCTLR_Ni == TRUE)
      MEM_FEAT_BLOCK_NB  MemFeatBlockDA = {
        MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
        MemFDefRet,
        MEM_FEATURE_BANK_INTERLEAVE,
        MEM_FEATURE_UNDO_BANK_INTERLEAVE,
        MemFDefRet,
        MemFDefRet,
        MEM_FEATURE_CHANNEL_INTERLEAVE,
        MEM_FEATURE_REGION_INTERLEAVE,
        MEM_FEATURE_CK_ECC,
        MEM_FEATURE_ECC,
        MEM_FEATURE_TRAINING,
        MEM_FEATURE_LVDDR3,
        MemFDefRet,
        MEM_TECH_FEATURE_DRAMINIT,
        MEM_FEATURE_DIMM_EXCLUDE,
        MEM_TECH_FEATURE_CPG
      };

      #undef MEM_NB_SUPPORT_DA
      extern MEM_NB_CONSTRUCTOR MemConstructNBBlockNi;
      extern MEM_INITIALIZER MemNInitDefaultsNi;

      #define MEM_NB_SUPPORT_DA { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockNi, MemNInitDefaultsNi, &MemFeatBlockDA, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA, MEM_IDENDIMM_DA },
    #else
      MEM_FEAT_BLOCK_NB  MemFeatBlockDA = {
        MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
        MemFDefRet,
        MEM_FEATURE_BANK_INTERLEAVE,
        MEM_FEATURE_UNDO_BANK_INTERLEAVE,
        MemFDefRet,
        MemFDefRet,
        MEM_FEATURE_CHANNEL_INTERLEAVE,
        MEM_FEATURE_REGION_INTERLEAVE,
        MEM_FEATURE_CK_ECC,
        MEM_FEATURE_ECC,
        MEM_FEATURE_TRAINING,
        MEM_FEATURE_LVDDR3,
        MemFDefRet,
        MEM_TECH_FEATURE_DRAMINIT,
        MEM_FEATURE_DIMM_EXCLUDE,
        MEM_TECH_FEATURE_CPG
      };

      #undef MEM_NB_SUPPORT_DA
      extern MEM_NB_CONSTRUCTOR MemConstructNBBlockDA;
      extern MEM_INITIALIZER MemNInitDefaultsDA;


      #define MEM_NB_SUPPORT_DA { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockDA, MemNInitDefaultsDA, &MemFeatBlockDA, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA, MEM_IDENDIMM_DA },
    #endif
  #endif // OPTION_MEMCTRL_DA

  /*---------------------------------------------------------------------------------------------------
   * HYDRA FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_HY == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #if (OPTION_CONTINOUS_PATTERN_GENERATION == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitCPGNb;
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGNb
    #else
      #define MEM_TECH_FEATURE_CPG    MemFDefRet
    #endif

    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    extern OPTION_MEM_FEATURE_NB MemFStandardTraining;
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    MEM_FEAT_BLOCK_NB  MemFeatBlockHy = {
      MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
      MEM_FEATURE_ONLINE_SPARE,
      MEM_FEATURE_BANK_INTERLEAVE,
      MEM_FEATURE_UNDO_BANK_INTERLEAVE,
      MEM_FEATURE_NODE_INTERLEAVE_CHECK,
      MEM_FEATURE_NODE_INTERLEAVE,
      MEM_FEATURE_CHANNEL_INTERLEAVE,
      MemFDefRet,
      MEM_FEATURE_CK_ECC,
      MEM_FEATURE_ECC,
      MEM_FEATURE_TRAINING,
      MEM_FEATURE_LVDDR3,
      MEM_FEATURE_ONDIMMTHERMAL,
      MEM_TECH_FEATURE_DRAMINIT,
      MEM_FEATURE_DIMM_EXCLUDE,
      MEM_TECH_FEATURE_CPG
    };

    #undef MEM_NB_SUPPORT_HY
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockHY;
    extern MEM_INITIALIZER MemNInitDefaultsHY;
    #define MEM_NB_SUPPORT_HY { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockHY, MemNInitDefaultsHY, &MemFeatBlockHy, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_HY, MEM_IDENDIMM_HY },
  #endif // OPTION_MEMCTRL_HY
  /*---------------------------------------------------------------------------------------------------
   * LLANO FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_LN == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #if (OPTION_CONTINOUS_PATTERN_GENERATION == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitCPGClientNb;
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGClientNb
    #else
      #define MEM_TECH_FEATURE_CPG    MemFDefRet
    #endif

    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    extern OPTION_MEM_FEATURE_NB MemFStandardTraining;
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    MEM_FEAT_BLOCK_NB  MemFeatBlockLn = {
      MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
      MemFDefRet,
      MEM_FEATURE_BANK_INTERLEAVE,
      MEM_FEATURE_UNDO_BANK_INTERLEAVE,
      MemFDefRet,
      MemFDefRet,
      MEM_FEATURE_CHANNEL_INTERLEAVE,
      MEM_FEATURE_REGION_INTERLEAVE,
      MEM_FEATURE_CK_ECC,
      MemFDefRet,
      MEM_FEATURE_TRAINING,
      MEM_FEATURE_LVDDR3,
      MEM_FEATURE_ONDIMMTHERMAL,
      MEM_TECH_FEATURE_DRAMINIT,
      MEM_FEATURE_DIMM_EXCLUDE,
      MEM_TECH_FEATURE_CPG
    };
    #undef MEM_NB_SUPPORT_LN
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockLN;
    extern MEM_INITIALIZER MemNInitDefaultsLN;
    #define MEM_NB_SUPPORT_LN { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockLN, MemNInitDefaultsLN, &MemFeatBlockLn, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_LN, MEM_IDENDIMM_LN },

  #endif // OPTION_MEMCTRL_LN

  /*---------------------------------------------------------------------------------------------------
   * ONTARIO FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_ON == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #if (OPTION_CONTINOUS_PATTERN_GENERATION == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitCPGClientNb;
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGClientNb
    #else
      #define MEM_TECH_FEATURE_CPG    MemFDefRet
    #endif

    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    extern OPTION_MEM_FEATURE_NB MemFStandardTraining;
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    MEM_FEAT_BLOCK_NB  MemFeatBlockOn = {
      MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
      MemFDefRet,
      MEM_FEATURE_BANK_INTERLEAVE,
      MEM_FEATURE_UNDO_BANK_INTERLEAVE,
      MemFDefRet,
      MemFDefRet,
      MemFDefRet,
      MemFDefRet,
      MemFDefRet,
      MemFDefRet,
      MEM_FEATURE_TRAINING,
      MEM_FEATURE_LVDDR3,
      MEM_FEATURE_ONDIMMTHERMAL,
      MEM_TECH_FEATURE_DRAMINIT,
      MEM_FEATURE_DIMM_EXCLUDE,
      MEM_TECH_FEATURE_CPG
    };

    #undef MEM_NB_SUPPORT_ON
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockON;
    extern MEM_INITIALIZER MemNInitDefaultsON;
    #define MEM_NB_SUPPORT_ON { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockON, MemNInitDefaultsON, &MemFeatBlockOn, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_ON, MEM_IDENDIMM_ON },

  #endif // OPTION_MEMCTRL_ON

  /*---------------------------------------------------------------------------------------------------
   * OROCHI FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_OR == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #undef MEM_FEATURE_TRAINING
    #if (OPTION_PARALLEL_TRAINING == TRUE)
      extern OPTION_MEM_FEATURE_NB MemFParallelTrainingOr;
      #define MEM_FEATURE_TRAINING  MemFParallelTrainingOr
    #else
      extern OPTION_MEM_FEATURE_NB MemFStandardTraining;
      #define MEM_FEATURE_TRAINING  MemFStandardTraining
    #endif

    MEM_FEAT_BLOCK_NB  MemFeatBlockOr = {
      MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
      MEM_FEATURE_ONLINE_SPARE,
      MEM_FEATURE_BANK_INTERLEAVE,
      MEM_FEATURE_UNDO_BANK_INTERLEAVE,
      MEM_FEATURE_NODE_INTERLEAVE_CHECK,
      MEM_FEATURE_NODE_INTERLEAVE,
      MEM_FEATURE_CHANNEL_INTERLEAVE,
      MemFDefRet,
      MEM_FEATURE_CK_ECC,
      MEM_FEATURE_ECCX8,
      MEM_FEATURE_TRAINING,
      MEM_FEATURE_LVDDR3,
      MEM_FEATURE_ONDIMMTHERMAL,
      MEM_TECH_FEATURE_DRAMINIT,
      MEM_FEATURE_DIMM_EXCLUDE,
      MemFDefRet
    };

    #undef MEM_NB_SUPPORT_OR
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockOR;
    extern MEM_INITIALIZER MemNInitDefaultsOR;
    #define MEM_NB_SUPPORT_OR { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockOR, MemNInitDefaultsOR, &MemFeatBlockOr, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_OR, MEM_IDENDIMM_OR },

  #endif // OPTION_MEMCTRL_OR

  /*---------------------------------------------------------------------------------------------------
   * C32 FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_C32 == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #if (OPTION_CONTINOUS_PATTERN_GENERATION == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitCPGNb;
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGNb
    #else
      #define MEM_TECH_FEATURE_CPG    MemFDefRet
    #endif

    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    extern OPTION_MEM_FEATURE_NB MemFStandardTraining;
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    MEM_FEAT_BLOCK_NB  MemFeatBlockC32 = {
      MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
      MEM_FEATURE_ONLINE_SPARE,
      MEM_FEATURE_BANK_INTERLEAVE,
      MEM_FEATURE_UNDO_BANK_INTERLEAVE,
      MEM_FEATURE_NODE_INTERLEAVE_CHECK,
      MEM_FEATURE_NODE_INTERLEAVE,
      MEM_FEATURE_CHANNEL_INTERLEAVE,
      MemFDefRet,
      MEM_FEATURE_CK_ECC,
      MEM_FEATURE_ECC,
      MEM_FEATURE_TRAINING,
      MEM_FEATURE_LVDDR3,
      MEM_FEATURE_ONDIMMTHERMAL,
      MEM_TECH_FEATURE_DRAMINIT,
      MEM_FEATURE_DIMM_EXCLUDE,
      MEM_TECH_FEATURE_CPG
    };

    #undef MEM_NB_SUPPORT_C32
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockC32;
    extern MEM_INITIALIZER MemNInitDefaultsC32;
    #define MEM_NB_SUPPORT_C32 { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockC32, MemNInitDefaultsC32, &MemFeatBlockC32, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_C32, MEM_IDENDIMM_C32 },
  #endif // OPTION_MEMCTRL_C32

  /*---------------------------------------------------------------------------------------------------
   * MAIN FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  MEM_FEAT_BLOCK_MAIN MemFeatMain = {
    MEM_FEAT_BLOCK_MAIN_STRUCT_VERSION,
    MEM_MAIN_FEATURE_TRAINING,
    MEM_MAIN_FEATURE_DIMM_EXCLUDE,
    MEM_MAIN_FEATURE_ONLINE_SPARE,
    MEM_MAIN_FEATURE_NODE_INTERLEAVE,
    MEM_MAIN_FEATURE_ECC,
    MEM_MAIN_FEATURE_MEM_CLEAR,
    MEM_MAIN_FEATURE_MEM_DMI,
    MEM_MAIN_FEATURE_LVDDR3,
    MEM_MAIN_FEATURE_UMAALLOC,
    MEM_MAIN_FEATURE_MEM_SAVE,
    MEM_MAIN_FEATURE_MEM_RESTORE
  };


  /*---------------------------------------------------------------------------------------------------
   * Technology Training SPECIFIC CONFIGURATION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #define MEM_TECH_TRAINING_FEAT_NULL_TERNMIATOR 0
  #if OPTION_MEMCTLR_DR
    #if OPTION_DDR2
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR2    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming2Nb;
      #define NB_TRAIN_FLOW_DDR2    MemNDQSTiming2Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR2    MemNDefNb
    #endif
    #if OPTION_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTPreparePhyAssistedTraining
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3    MemTExitPhyAssistedTraining
      #if (OPTION_HW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_HW_WL_P1_DDR3   MemTWriteLevelizationHw3Pass1
        #define TECH_TRAIN_HW_WL_P2_DDR3   MemTWriteLevelizationHw3Pass2
      #else
        #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_SW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_SW_WL_DDR3    MemTFeatDef
      #else
        #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #endif
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainOptRcvrEnSwPass1
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR3    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming3Nb;
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR3    MemNDefNb
    #endif
  #endif

  #if OPTION_MEMCTLR_DA
    #if OPTION_DDR2
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR2    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming2Nb;
      #define NB_TRAIN_FLOW_DDR2    MemNDQSTiming2Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR2    MemNDefNb
    #endif
    #if OPTION_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTPreparePhyAssistedTraining
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3    MemTExitPhyAssistedTraining
      #if (OPTION_HW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_HW_WL_P1_DDR3   MemTWriteLevelizationHw3Pass1
        #define TECH_TRAIN_HW_WL_P2_DDR3   MemTWriteLevelizationHw3Pass2
      #else
        #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_SW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_SW_WL_DDR3    MemTFeatDef
      #else
        #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #endif
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainOptRcvrEnSwPass1
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR3    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming3Nb;
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR3    MemNDefNb
    #endif
  #endif

  #if OPTION_MEMCTLR_HY
    #if OPTION_DDR2
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR2    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming2Nb;
      #define NB_TRAIN_FLOW_DDR2    MemNDQSTiming2Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR2    MemNDefNb
    #endif
    #if OPTION_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTPreparePhyAssistedTraining
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3    MemTExitPhyAssistedTraining
      #if (OPTION_HW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_HW_WL_P1_DDR3   MemTWriteLevelizationHw3Pass1
        #define TECH_TRAIN_HW_WL_P2_DDR3   MemTWriteLevelizationHw3Pass2
      #else
        #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_SW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_SW_WL_DDR3    MemTFeatDef
      #else
        #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #endif
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainOptRcvrEnSwPass1
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR3    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #endif
//      extern OPTION_MEM_FEATURE_NB MemNDQSTiming3Nb;
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR3    MemNDefNb
    #endif
  #endif

  #if OPTION_MEMCTLR_C32
    #if OPTION_DDR2
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR2    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming2Nb;
      #define NB_TRAIN_FLOW_DDR2    MemNDQSTiming2Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR2    MemNDefNb
    #endif
    #if OPTION_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTPreparePhyAssistedTraining
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3    MemTExitPhyAssistedTraining
      #if (OPTION_HW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_HW_WL_P1_DDR3   MemTWriteLevelizationHw3Pass1
        #define TECH_TRAIN_HW_WL_P2_DDR3   MemTWriteLevelizationHw3Pass2
      #else
        #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_SW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_SW_WL_DDR3    MemTFeatDef
      #else
        #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #endif
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainOptRcvrEnSwPass1
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR3    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming3Nb;
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR3    MemNDefNb
    #endif
  #endif

  #if OPTION_MEMCTLR_LN
    #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
    #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
    #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
    #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
    #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
    #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
    #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
    #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
    #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
    #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
    #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
    #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
    #define NB_TRAIN_FLOW_DDR2    MemNDefNb
    #if OPTION_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3    MemTExitPhyAssistedTrainingClient3
      #if (OPTION_HW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_HW_WL_P1_DDR3   MemTWriteLevelizationHw3Pass1
        #define TECH_TRAIN_HW_WL_P2_DDR3   MemTWriteLevelizationHw3Pass2
      #else
        #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #endif
      #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainOptRcvrEnSwPass1
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR3    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming3Nb;
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR3    MemNDefNb
    #endif
  #endif

  #if OPTION_MEMCTLR_OR
    #if OPTION_DDR2
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR2    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming2Nb;
      #define NB_TRAIN_FLOW_DDR2    MemNDQSTiming2Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR2    MemNDefNb
    #endif
    #if OPTION_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTPreparePhyAssistedTraining
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3    MemTExitPhyAssistedTraining
      #if (OPTION_HW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_HW_WL_P1_DDR3   MemTWriteLevelizationHw3Pass1
        #define TECH_TRAIN_HW_WL_P2_DDR3   MemTWriteLevelizationHw3Pass2
      #else
        #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_SW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_SW_WL_DDR3    MemTFeatDef
      #else
        #define TECH_TRAIN_SW_WL MemTFeatDef
      #endif
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3    MemTFeatDef
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR3    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #endif
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
    #endif
  #endif

  #if OPTION_MEMCTLR_ON
    #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
    #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
    #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
    #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
    #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
    #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
    #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
    #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
    #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2 MemTFeatDef
    #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
    #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2 MemTFeatDef
    #define TECH_TRAIN_MAX_RD_LAT_DDR2 MemTFeatDef
    #define NB_TRAIN_FLOW_DDR2    MemNDefNb
    #if OPTION_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3    MemTExitPhyAssistedTrainingClient3
      #if (OPTION_HW_WRITE_LEV_TRAINING == TRUE)
        #define TECH_TRAIN_HW_WL_P1_DDR3   MemTWriteLevelizationHw3Pass1
        #define TECH_TRAIN_HW_WL_P2_DDR3   MemTWriteLevelizationHw3Pass2
      #else
        #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #endif
      #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainRcvrEnSwPass1
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3    MemTTrainOptRcvrEnSwPass1
      #else
        #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE)
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3    MemTTrainDQSEdgeDetectSw
      #else
        #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #endif
      #if (OPTION_MAX_RD_LAT_TRAINING == TRUE)
        #define TECH_TRAIN_MAX_RD_LAT_DDR3    MemTTrainMaxLatency
      #else
        #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #endif
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming3Nb;
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
    #else
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #define TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3 MemTFeatDef
      #define TECH_TRAIN_MAX_RD_LAT_DDR3 MemTFeatDef
      #define NB_TRAIN_FLOW_DDR3    MemNDefNb
    #endif
  #endif

  MEM_TECH_FEAT_BLOCK  memTechTrainingFeatDDR2 = {
    MEM_TECH_FEAT_BLOCK_STRUCT_VERSION,
    TECH_TRAIN_ENTER_HW_TRN_DDR2,
    TECH_TRAIN_SW_WL_DDR2,
    TECH_TRAIN_HW_WL_P1_DDR2,
    TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2,
    TECH_TRAIN_HW_WL_P2_DDR2,
    TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2,
    TECH_TRAIN_EXIT_HW_TRN_DDR2,
    TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR2,
    TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR2,
    TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR2,
    TECH_TRAIN_OPT_SW_RD_WR_POS_DDR2,
    TECH_TRAIN_MAX_RD_LAT_DDR2
  };

  MEM_TECH_FEAT_BLOCK  memTechTrainingFeatDDR3 = {
    MEM_TECH_FEAT_BLOCK_STRUCT_VERSION,
    TECH_TRAIN_ENTER_HW_TRN_DDR3,
    TECH_TRAIN_SW_WL_DDR3,
    TECH_TRAIN_HW_WL_P1_DDR3,
    TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3,
    TECH_TRAIN_HW_WL_P2_DDR3,
    TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3,
    TECH_TRAIN_EXIT_HW_TRN_DDR3,
    TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3,
    TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3,
    TECH_TRAIN_NON_OPT_SW_RD_WR_POS_DDR3,
    TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3,
    TECH_TRAIN_MAX_RD_LAT_DDR3
  };
  /*---------------------------------------------------------------------------------------------------
   * NB TRAINING FLOW CONTROL
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  OPTION_MEM_FEATURE_NB* memNTrainFlowControl[] = {    // Training flow control
    NB_TRAIN_FLOW_DDR2,
    NB_TRAIN_FLOW_DDR3,
  };
  /*---------------------------------------------------------------------------------------------------
   * TECHNOLOGY BLOCK
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  MEM_TECH_CONSTRUCTOR* memTechInstalled[] = {    // Types of technology installed
    MEM_TECH_CONSTRUCTOR_DDR2
    MEM_TECH_CONSTRUCTOR_DDR3
    NULL
  };
   /*---------------------------------------------------------------------------------------------------
   * PLATFORM SPECIFIC BLOCK FORM FACTOR DEFINITION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #if  OPTION_MEMCTLR_HY
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PLAT_SP_HY_FF_UDIMM2    MemPConstructPsUHy2,
      #else
        #define PLAT_SP_HY_FF_UDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_HY_FF_UDIMM3    MemPConstructPsUHy3,
      #else
        #define PLAT_SP_HY_FF_UDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_HY_FF_UDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_HY_FF_UDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PLAT_SP_HY_FF_RDIMM2    MemPConstructPsRHy2,
      #else
        #define PLAT_SP_HY_FF_RDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_HY_FF_RDIMM3    MemPConstructPsRHy3,
      #else
        #define PLAT_SP_HY_FF_RDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_HY_FF_RDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_HY_FF_RDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR2
        #define PLAT_SP_HY_FF_SDIMM2    MemPConstructPsSHy2,
      #else
        #define PLAT_SP_HY_FF_SDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_HY_FF_SDIMM3    MemPConstructPsSHy3,
      #else
        #define PLAT_SP_HY_FF_SDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_HY_FF_SDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_HY_FF_SDIMM3    MemPConstructPsUDef,
    #endif
  #else
    #define PLAT_SP_HY_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_HY_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_HY_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_HY_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_HY_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_HY_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledHy[MAX_FF_TYPES] = {
    PLAT_SP_HY_FF_UDIMM2
    PLAT_SP_HY_FF_RDIMM2
    PLAT_SP_HY_FF_SDIMM2
    PLAT_SP_HY_FF_UDIMM3
    PLAT_SP_HY_FF_RDIMM3
    PLAT_SP_HY_FF_SDIMM3
  };

  #if OPTION_MEMCTLR_DR
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PLAT_SP_DR_FF_UDIMM2    MemPConstructPsUDr2,
      #else
        #define PLAT_SP_DR_FF_UDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_DR_FF_UDIMM3    MemPConstructPsUDr3,
      #else
        #define PLAT_SP_DR_FF_UDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_DR_FF_UDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_DR_FF_UDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PLAT_SP_DR_FF_RDIMM2    MemPConstructPsRDr2,
      #else
        #define PLAT_SP_DR_FF_RDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_DR_FF_RDIMM3    MemPConstructPsRDr3,
      #else
        #define PLAT_SP_DR_FF_RDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_DR_FF_RDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_DR_FF_RDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR2
        #define PLAT_SP_DR_FF_SDIMM2    MemPConstructPsUDef,
      #else
        #define PLAT_SP_DR_FF_SDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_DR_FF_SDIMM3    MemPConstructPsSDr3,
      #else
        #define PLAT_SP_DR_FF_SDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_DR_FF_SDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_DR_FF_SDIMM3    MemPConstructPsUDef,
    #endif
  #else
    #define PLAT_SP_DR_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_DR_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_DR_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_DR_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_DR_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_DR_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledDR[MAX_FF_TYPES] = {
    PLAT_SP_DR_FF_UDIMM2
    PLAT_SP_DR_FF_RDIMM2
    PLAT_SP_DR_FF_SDIMM2
    PLAT_SP_DR_FF_UDIMM3
    PLAT_SP_DR_FF_RDIMM3
    PLAT_SP_DR_FF_SDIMM3
  };

  #if OPTION_MEMCTLR_DA
    #if OPTION_MEMCTLR_Ni
      #define PLAT_SP_DA_FF_SDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_DA_FF_RDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_DA_FF_UDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_DA_FF_SDIMM3    MemPConstructPsSNi3,
      #define PLAT_SP_DA_FF_RDIMM3    MemPConstructPsUDef,
      #define PLAT_SP_DA_FF_UDIMM3    MemPConstructPsUNi3,
    #else
      #if OPTION_UDIMMS
        #if OPTION_DDR2
          #define PLAT_SP_DA_FF_UDIMM2    MemPConstructPsUDef,
        #else
          #define PLAT_SP_DA_FF_UDIMM2    MemPConstructPsUDef,
        #endif
        #if OPTION_DDR3
          #define PLAT_SP_DA_FF_UDIMM3    MemPConstructPsUDA3,
        #else
          #define PLAT_SP_DA_FF_UDIMM3    MemPConstructPsUDef,
        #endif
      #else
        #define PLAT_SP_DA_FF_UDIMM2    MemPConstructPsUDef,
        #define PLAT_SP_DA_FF_UDIMM3    MemPConstructPsUDef,
      #endif
      #if OPTION_RDIMMS
        #if OPTION_DDR2
          #define PLAT_SP_DA_FF_RDIMM2    MemPConstructPsUDef,
        #else
          #define PLAT_SP_DA_FF_RDIMM2    MemPConstructPsUDef,
        #endif
        #if OPTION_DDR3
          #define PLAT_SP_DA_FF_RDIMM3    MemPConstructPsUDef,
        #else
          #define PLAT_SP_DA_FF_RDIMM3    MemPConstructPsUDef,
        #endif
      #else
        #define PLAT_SP_DA_FF_RDIMM2    MemPConstructPsUDef,
        #define PLAT_SP_DA_FF_RDIMM3    MemPConstructPsUDef,
      #endif
      #if OPTION_SODIMMS
        #if OPTION_DDR2
          #define PLAT_SP_DA_FF_SDIMM2    MemPConstructPsSDA2,
        #else
          #define PLAT_SP_DA_FF_SDIMM2    MemPConstructPsUDef,
        #endif
        #if OPTION_DDR3
          #define PLAT_SP_DA_FF_SDIMM3    MemPConstructPsSDA3,
        #else
          #define PLAT_SP_DA_FF_SDIMM3    MemPConstructPsUDef,
        #endif
      #else
        #define PLAT_SP_DA_FF_SDIMM2    MemPConstructPsUDef,
        #define PLAT_SP_DA_FF_SDIMM3    MemPConstructPsUDef,
      #endif
    #endif
  #else
    #define PLAT_SP_DA_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_DA_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_DA_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_DA_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_DA_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_DA_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledDA[MAX_FF_TYPES] = {
    PLAT_SP_DA_FF_UDIMM2
    PLAT_SP_DA_FF_RDIMM2
    PLAT_SP_DA_FF_SDIMM2
    PLAT_SP_DA_FF_UDIMM3
    PLAT_SP_DA_FF_RDIMM3
    PLAT_SP_DA_FF_SDIMM3
  };

  #if OPTION_MEMCTLR_Or
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PLAT_SP_OR_FF_UDIMM2    MemPConstructPsUOr2,
      #else
        #define PLAT_SP_OR_FF_UDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_OR_FF_UDIMM3    MemPConstructPsUOr3,
      #else
        #define PLAT_SP_OR_FF_UDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_OR_FF_UDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_OR_FF_UDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PLAT_SP_OR_FF_RDIMM2    MemPConstructPsROr2,
      #else
        #define PLAT_SP_OR_FF_RDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_OR_FF_RDIMM3    MemPConstructPsROr3,
      #else
        #define PLAT_SP_OR_FF_RDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_OR_FF_RDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_OR_FF_RDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR2
        #define PLAT_SP_OR_FF_SDIMM2    MemPConstructPsSOr2,
      #else
        #define PLAT_SP_OR_FF_SDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_OR_FF_SDIMM3    MemPConstructPsSOr3,
      #else
        #define PLAT_SP_OR_FF_SDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_OR_FF_UDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_OR_FF_UDIMM3    MemPConstructPsUDef,
    #endif
  #else
    #define PLAT_SP_OR_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_OR_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_OR_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_OR_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_OR_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_OR_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledOr[MAX_FF_TYPES] = {
    PLAT_SP_OR_FF_UDIMM2
    PLAT_SP_OR_FF_RDIMM2
    PLAT_SP_OR_FF_SDIMM2
    PLAT_SP_OR_FF_UDIMM3
    PLAT_SP_OR_FF_RDIMM3
    PLAT_SP_OR_FF_SDIMM3
  };

  #if OPTION_MEMCTLR_LN
    #if OPTION_UDIMMS
      #if OPTION_DDR3
        #define PLAT_SP_LN_FF_UDIMM3    MemPConstructPsULN3,
      #else
        #define PLAT_SP_LN_FF_UDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_LN_FF_UDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR3
        #define PLAT_SP_LN_FF_SDIMM3    MemPConstructPsSLN3,
      #else
        #define PLAT_SP_LN_FF_SDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_LN_FF_SDIMM3    MemPConstructPsUDef,
    #endif
  #else
    #define PLAT_SP_LN_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_LN_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledLN[] = {
    PLAT_SP_LN_FF_SDIMM3
    PLAT_SP_LN_FF_UDIMM3
    NULL
  };

  #if  OPTION_MEMCTLR_C32
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PLAT_SP_C32_FF_UDIMM2    MemPConstructPsUC32_2,
      #else
        #define PLAT_SP_C32_FF_UDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_C32_FF_UDIMM3    MemPConstructPsUC32_3,
      #else
        #define PLAT_SP_C32_FF_UDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_C32_FF_UDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_C32_FF_UDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PLAT_SP_C32_FF_RDIMM2    MemPConstructPsRC32_2,
      #else
        #define PLAT_SP_C32_FF_RDIMM2    MemPConstructPsUDef,
      #endif
      #if OPTION_DDR3
        #define PLAT_SP_C32_FF_RDIMM3    MemPConstructPsRC32_3,
      #else
        #define PLAT_SP_C32_FF_RDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_C32_FF_RDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_C32_FF_RDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_SODIMMS
      #define PLAT_SP_C32_FF_SDIMM2    MemPConstructPsUDef,
      #define PLAT_SP_C32_FF_SDIMM3    MemPConstructPsUDef,
    #endif
  #else
    #define PLAT_SP_C32_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_C32_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_C32_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_C32_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_C32_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_C32_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledC32[MAX_FF_TYPES] = {
    PLAT_SP_C32_FF_UDIMM2
    PLAT_SP_C32_FF_RDIMM2
    PLAT_SP_C32_FF_SDIMM2
    PLAT_SP_C32_FF_UDIMM3
    PLAT_SP_C32_FF_RDIMM3
    PLAT_SP_C32_FF_SDIMM3
  };

  #if OPTION_MEMCTLR_ON
    #if OPTION_UDIMMS
      #if OPTION_DDR3
        #define PLAT_SP_ON_FF_UDIMM3    MemPConstructPsUON3,
      #else
        #define PLAT_SP_ON_FF_UDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_ON_FF_UDIMM3    MemPConstructPsUDef,
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR3
        #define PLAT_SP_ON_FF_SDIMM3    MemPConstructPsSON3,
      #else
        #define PLAT_SP_ON_FF_SDIMM3    MemPConstructPsUDef,
      #endif
    #else
      #define PLAT_SP_ON_FF_SDIMM3    MemPConstructPsUDef,
    #endif
  #else
    #define PLAT_SP_ON_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_ON_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledON[] = {
    PLAT_SP_ON_FF_SDIMM3
    PLAT_SP_ON_FF_UDIMM3
    NULL
  };

  /*---------------------------------------------------------------------------------------------------
   * PLATFORM-SPECIFIC CONFIGURATION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */

  #if OPTION_MEMCTLR_DR
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PSC_DR_UDIMM_DDR2     //MemAGetPsCfgUDr2
      #else
        #define PSC_DR_UDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_DR_UDIMM_DDR3    MemAGetPsCfgUDr3,
      #else
        #define PSC_DR_UDIMM_DDR3
      #endif
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PSC_DR_RDIMM_DDR2    MemAGetPsCfgRDr2,
      #else
        #define PSC_DR_RDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_DR_RDIMM_DDR3    MemAGetPsCfgRDr3,
      #else
        #define PSC_DR_RDIMM_DDR3
      #endif
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR2
        #define PSC_DR_SODIMM_DDR2    //MemAGetPsCfgSDr2
      #else
        #define PSC_DR_SODIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_DR_SODIMM_DDR3    //MemAGetPsCfgSDr3
      #else
        #define PSC_DR_SODIMM_DDR3
      #endif
    #endif
  #endif

  #if OPTION_MEMCTLR_DA
    #if OPTION_MEMCTLR_Ni
      #define PSC_DA_UDIMM_DDR2
      #define PSC_DA_UDIMM_DDR3     MemAGetPsCfgUNi3,
      #define PSC_DA_RDIMM_DDR2
      #define PSC_DA_RDIMM_DDR3
      #define PSC_DA_SODIMM_DDR2
      #define PSC_DA_SODIMM_DDR3    MemAGetPsCfgSNi3,
    #else
      #if OPTION_UDIMMS
        #if OPTION_DDR2
          #define PSC_DA_UDIMM_DDR2     //MemAGetPsCfgUDr2
        #else
          #define PSC_DA_UDIMM_DDR2
        #endif
        #if OPTION_DDR3
          #define PSC_DA_UDIMM_DDR3    MemAGetPsCfgUDA3,
        #else
          #define PSC_DA_UDIMM_DDR3
        #endif
      #endif
      #if OPTION_RDIMMS
        #if OPTION_DDR2
          #define PSC_DA_RDIMM_DDR2
        #else
          #define PSC_DA_RDIMM_DDR2
        #endif
        #if OPTION_DDR3
          #define PSC_DA_RDIMM_DDR3
        #else
          #define PSC_DA_RDIMM_DDR3
        #endif
      #endif
      #if OPTION_SODIMMS
        #if OPTION_DDR2
          #define PSC_DA_SODIMM_DDR2    MemAGetPsCfgSDA2,
        #else
          #define PSC_DA_SODIMM_DDR2
        #endif
        #if OPTION_DDR3
          #define PSC_DA_SODIMM_DDR3    MemAGetPsCfgSDA3,
        #else
          #define PSC_DA_SODIMM_DDR3
        #endif
      #endif
    #endif
  #endif

  #if OPTION_MEMCTLR_HY
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PSC_HY_UDIMM_DDR2     //MemAGetPsCfgUDr2,
      #else
        #define PSC_HY_UDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_HY_UDIMM_DDR3    MemAGetPsCfgUHy3,
      #else
        #define PSC_HY_UDIMM_DDR3
      #endif
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PSC_HY_RDIMM_DDR2
      #else
        #define PSC_HY_RDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_HY_RDIMM_DDR3    MemAGetPsCfgRHy3,
      #else
        #define PSC_HY_RDIMM_DDR3
      #endif
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR2
        #define PSC_HY_SODIMM_DDR2    //MemAGetPsCfgSHy2,
      #else
        #define PSC_HY_SODIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_HY_SODIMM_DDR3    //MemAGetPsCfgSHy3,
      #else
        #define PSC_HY_SODIMM_DDR3
      #endif
    #endif
  #endif

  #if OPTION_MEMCTLR_C32
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PSC_C32_UDIMM_DDR2     //MemAGetPsCfgUDr2,
      #else
        #define PSC_C32_UDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_C32_UDIMM_DDR3    MemAGetPsCfgUC32_3,
      #else
        #define PSC_C32_UDIMM_DDR3
      #endif
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PSC_C32_RDIMM_DDR2
      #else
        #define PSC_C32_RDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_C32_RDIMM_DDR3    MemAGetPsCfgRC32_3,
      #else
        #define PSC_C32_RDIMM_DDR3
      #endif
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR2
        #define PSC_C32_SODIMM_DDR2    //MemAGetPsCfgSC32_2,
      #else
        #define PSC_C32_SODIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_C32_SODIMM_DDR3    //MemAGetPsCfgSC32_3,
      #else
        #define PSC_C32_SODIMM_DDR3
      #endif
    #endif
  #endif

  #if OPTION_MEMCTLR_LN
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PSC_LN_UDIMM_DDR2     //MemAGetPsCfgULN2,
      #else
        #define PSC_LN_UDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_LN_UDIMM_DDR3    MemAGetPsCfgULN3,
      #else
        #define PSC_LN_UDIMM_DDR3
      #endif
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PSC_LN_RDIMM_DDR2
      #else
        #define PSC_LN_RDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_LN_RDIMM_DDR3    //MemAGetPsCfgRLN3,
      #else
        #define PSC_LN_RDIMM_DDR3
      #endif
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR2
        #define PSC_LN_SODIMM_DDR2    //MemAGetPsCfgSLN2,
      #else
        #define PSC_LN_SODIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_LN_SODIMM_DDR3   MemAGetPsCfgSLN3,
      #else
        #define PSC_LN_SODIMM_DDR3
      #endif
    #endif
  #endif

  #if OPTION_MEMCTLR_OR
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PSC_OR_UDIMM_DDR2     //MemAGetPsCfgUOr2,
      #else
        #define PSC_OR_UDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_OR_UDIMM_DDR3    MemAGetPsCfgUOr3,
      #else
        #define PSC_OR_UDIMM_DDR3
      #endif
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PSC_OR_RDIMM_DDR2
      #else
        #define PSC_OR_RDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_OR_RDIMM_DDR3    MemAGetPsCfgROr3,
      #else
        #define PSC_OR_RDIMM_DDR3
      #endif
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR2
        #define PSC_OR_SODIMM_DDR2    //MemAGetPsCfgSOr2,
      #else
        #define PSC_OR_SODIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_OR_SODIMM_DDR3    //MemAGetPsCfgSOr3,
      #else
        #define PSC_OR_SODIMM_DDR3
      #endif
    #endif
  #endif

  #if OPTION_MEMCTLR_ON
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PSC_ON_UDIMM_DDR2     //MemAGetPsCfgUON2,
      #else
        #define PSC_ON_UDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_ON_UDIMM_DDR3    MemAGetPsCfgUON3,
      #else
        #define PSC_ON_UDIMM_DDR3
      #endif
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR2
        #define PSC_ON_RDIMM_DDR2
      #else
        #define PSC_ON_RDIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_ON_RDIMM_DDR3    //MemAGetPsCfgRON3,
      #else
        #define PSC_ON_RDIMM_DDR3
      #endif
    #endif
    #if OPTION_SODIMMS
      #if OPTION_DDR2
        #define PSC_ON_SODIMM_DDR2    //MemAGetPsCfgSON2,
      #else
        #define PSC_ON_SODIMM_DDR2
      #endif
      #if OPTION_DDR3
        #define PSC_ON_SODIMM_DDR3   MemAGetPsCfgSON3,
      #else
        #define PSC_ON_SODIMM_DDR3
      #endif
    #endif
  #endif

  /*----------------------------------------------------------------------
   * DEFAULT PSCFG DEFINITIONS
   *
   *----------------------------------------------------------------------
   */

  #ifndef PSC_DR_UDIMM_DDR2
    #define PSC_DR_UDIMM_DDR2
  #endif
  #ifndef PSC_DR_RDIMM_DDR2
    #define PSC_DR_RDIMM_DDR2
  #endif
  #ifndef PSC_DR_SODIMM_DDR2
    #define PSC_DR_SODIMM_DDR2
  #endif
  #ifndef PSC_DR_UDIMM_DDR3
    #define PSC_DR_UDIMM_DDR3
  #endif
  #ifndef PSC_DR_RDIMM_DDR3
    #define PSC_DR_RDIMM_DDR3
  #endif
  #ifndef PSC_DR_SODIMM_DDR3
    #define PSC_DR_SODIMM_DDR3
  #endif
  #ifndef PSC_DA_UDIMM_DDR2
    #define PSC_DA_UDIMM_DDR2
  #endif
  #ifndef PSC_DA_RDIMM_DDR2
    #define PSC_DA_RDIMM_DDR2
  #endif
  #ifndef PSC_DA_SODIMM_DDR2
    #define PSC_DA_SODIMM_DDR2
  #endif
  #ifndef PSC_DA_UDIMM_DDR3
    #define PSC_DA_UDIMM_DDR3
  #endif
  #ifndef PSC_DA_RDIMM_DDR3
    #define PSC_DA_RDIMM_DDR3
  #endif
  #ifndef PSC_DA_SODIMM_DDR3
    #define PSC_DA_SODIMM_DDR3
  #endif
  #ifndef PSC_HY_UDIMM_DDR2
    #define PSC_HY_UDIMM_DDR2
  #endif
  #ifndef PSC_HY_RDIMM_DDR2
    #define PSC_HY_RDIMM_DDR2
  #endif
  #ifndef PSC_HY_SODIMM_DDR2
    #define PSC_HY_SODIMM_DDR2
  #endif
  #ifndef PSC_HY_UDIMM_DDR3
    #define PSC_HY_UDIMM_DDR3
  #endif
  #ifndef PSC_HY_RDIMM_DDR3
    #define PSC_HY_RDIMM_DDR3
  #endif
  #ifndef PSC_HY_SODIMM_DDR3
    #define PSC_HY_SODIMM_DDR3
  #endif
  #ifndef PSC_LN_UDIMM_DDR2
    #define PSC_LN_UDIMM_DDR2
  #endif
  #ifndef PSC_LN_RDIMM_DDR2
    #define PSC_LN_RDIMM_DDR2
  #endif
  #ifndef PSC_LN_SODIMM_DDR2
    #define PSC_LN_SODIMM_DDR2
  #endif
  #ifndef PSC_LN_UDIMM_DDR3
    #define PSC_LN_UDIMM_DDR3
  #endif
  #ifndef PSC_LN_RDIMM_DDR3
    #define PSC_LN_RDIMM_DDR3
  #endif
  #ifndef PSC_LN_SODIMM_DDR3
    #define PSC_LN_SODIMM_DDR3
  #endif
  #ifndef PSC_OR_UDIMM_DDR2
    #define PSC_OR_UDIMM_DDR2
  #endif
  #ifndef PSC_OR_RDIMM_DDR2
    #define PSC_OR_RDIMM_DDR2
  #endif
  #ifndef PSC_OR_SODIMM_DDR2
    #define PSC_OR_SODIMM_DDR2
  #endif
  #ifndef PSC_OR_UDIMM_DDR3
    #define PSC_OR_UDIMM_DDR3
  #endif
  #ifndef PSC_OR_RDIMM_DDR3
    #define PSC_OR_RDIMM_DDR3
  #endif
  #ifndef PSC_OR_SODIMM_DDR3
    #define PSC_OR_SODIMM_DDR3
  #endif
  #ifndef PSC_C32_UDIMM_DDR3
    #define PSC_C32_UDIMM_DDR3
  #endif
  #ifndef PSC_C32_RDIMM_DDR3
    #define PSC_C32_RDIMM_DDR3
  #endif
  #ifndef PSC_ON_UDIMM_DDR2
    #define PSC_ON_UDIMM_DDR2
  #endif
  #ifndef PSC_ON_RDIMM_DDR2
    #define PSC_ON_RDIMM_DDR2
  #endif
  #ifndef PSC_ON_SODIMM_DDR2
    #define PSC_ON_SODIMM_DDR2
  #endif
  #ifndef PSC_ON_UDIMM_DDR3
    #define PSC_ON_UDIMM_DDR3
  #endif
  #ifndef PSC_ON_RDIMM_DDR3
    #define PSC_ON_RDIMM_DDR3
  #endif
  #ifndef PSC_ON_SODIMM_DDR3
    #define PSC_ON_SODIMM_DDR3
  #endif

  MEM_PLATFORM_CFG* memPlatformTypeInstalled[] = {
    PSC_DR_UDIMM_DDR2
    PSC_DR_RDIMM_DDR2
    PSC_DR_SODIMM_DDR2
    PSC_DR_UDIMM_DDR3
    PSC_DR_RDIMM_DDR3
    PSC_DR_SODIMM_DDR3
    PSC_DA_SODIMM_DDR2
    PSC_DA_UDIMM_DDR3
    PSC_DA_SODIMM_DDR3
    PSC_HY_UDIMM_DDR3
    PSC_HY_RDIMM_DDR3
    PSC_HY_SODIMM_DDR3
    PSC_LN_UDIMM_DDR3
    PSC_LN_RDIMM_DDR3
    PSC_LN_SODIMM_DDR3
    PSC_OR_UDIMM_DDR3
    PSC_OR_RDIMM_DDR3
    PSC_OR_SODIMM_DDR3
    PSC_C32_UDIMM_DDR3
    PSC_C32_RDIMM_DDR3
    PSC_ON_UDIMM_DDR3
    PSC_ON_RDIMM_DDR3
    PSC_ON_SODIMM_DDR3
    NULL
  };
  CONST UINTN SIZE_OF_PLATFORM = (sizeof (memPlatformTypeInstalled) / sizeof (MEM_PLATFORM_CFG*));

#else
  /*---------------------------------------------------------------------------------------------------
   * NB TRAINING FLOW CONTROL
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  OPTION_MEM_FEATURE_NB* memNTrainFlowControl[] = {    // Training flow control
    NULL
  };
  /*---------------------------------------------------------------------------------------------------
   * DEFAULT TECHNOLOGY BLOCK
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  MEM_TECH_CONSTRUCTOR* memTechInstalled[] = {    // Types of technology installed
    NULL
  };

  /*---------------------------------------------------------------------------------------------------
   * DEFAULT TECHNOLOGY MAP
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  UINT8 MemoryTechnologyMap[MAX_SOCKETS_SUPPORTED] = {0, 0, 0, 0, 0, 0, 0, 0};

  /*---------------------------------------------------------------------------------------------------
   * DEFAULT MAIN FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  MEM_FEAT_BLOCK_MAIN MemFeatMain = {
    NULL
  };

  /*---------------------------------------------------------------------------------------------------
   * DEFAULT NORTHBRIDGE SUPPORT LIST
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_DR == TRUE)
    #undef MEM_NB_SUPPORT_DR
    #define MEM_NB_SUPPORT_DR { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DR, MEM_IDENDIMM_DR },
  #endif
  #if (OPTION_MEMCTLR_DA == TRUE)
    #undef MEM_NB_SUPPORT_DA
    #define MEM_NB_SUPPORT_DA { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA, MEM_IDENDIMM_DA },
  #endif
  #if (OPTION_MEMCTLR_HY == TRUE)
    #undef MEM_NB_SUPPORT_HY
    #define MEM_NB_SUPPORT_HY { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_HY, MEM_IDENDIMM_HY },
  #endif
  #if (OPTION_MEMCTLR_C32 == TRUE)
    #undef MEM_NB_SUPPORT_C32
    #define MEM_NB_SUPPORT_C32 { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_C32, MEM_IDENDIMM_C32 },
  #endif
  #if (OPTION_MEMCTLR_LN == TRUE)
    #undef MEM_NB_SUPPORT_LN
    #define MEM_NB_SUPPORT_LN { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_LN, MEM_IDENDIMM_LN },
  #endif
  #if (OPTION_MEMCTLR_ON == TRUE)
    #undef MEM_NB_SUPPORT_ON
    #define MEM_NB_SUPPORT_ON { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_ON, MEM_IDENDIMM_ON },
  #endif
  /*---------------------------------------------------------------------------------------------------
   * DEFAULT Technology Training
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #if OPTION_DDR2
    MEM_TECH_FEAT_BLOCK  memTechTrainingFeatDDR2 = {
      NULL
    };
  #endif
  #if OPTION_DDR3
    MEM_TECH_FEAT_BLOCK  memTechTrainingFeatDDR3 = {
      NULL
    };
  #endif
    /*---------------------------------------------------------------------------------------------------
     * DEFAULT Platform Specific list
     *
     *
     *---------------------------------------------------------------------------------------------------
     */
  #if (OPTION_MEMCTLR_DR == TRUE)
    MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledDr[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_DA == TRUE)
    MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledDA[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_LN == TRUE)
    MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledLN[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_HY == TRUE)
    MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledHy[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_Or == TRUE)
    MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledOr[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_C32 == TRUE)
    MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledC32[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_ON == TRUE)
    MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledON[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  /*----------------------------------------------------------------------
   * DEFAULT PSCFG DEFINITIONS
   *
   *----------------------------------------------------------------------
   */
  MEM_PLATFORM_CFG* memPlatformTypeInstalled[] = {
    NULL
  };
#endif

/*---------------------------------------------------------------------------------------------------
 * NORTHBRIDGE SUPPORT LIST
 *
 *
 *---------------------------------------------------------------------------------------------------
 */
MEM_NB_SUPPORT memNBInstalled[] = {
  MEM_NB_SUPPORT_DR
  MEM_NB_SUPPORT_DA
  MEM_NB_SUPPORT_HY
  MEM_NB_SUPPORT_LN
  MEM_NB_SUPPORT_OR
  MEM_NB_SUPPORT_C32
  MEM_NB_SUPPORT_ON
  MEM_NB_SUPPORT_END
};

#endif  // _OPTION_MEMORY_INSTALL_H_
