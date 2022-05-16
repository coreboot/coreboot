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
 * @e \$Revision: 37402 $   @e \$Date: 2010-09-03 05:36:02 +0800 (Fri, 03 Sep 2010) $
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

BOOLEAN MemMDefRetFalse (
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
BOOLEAN MemNIdentifyDimmConstructorRetDef (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  return FALSE;
}
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
#define MEM_NB_SUPPORT_ON
#define MEM_NB_SUPPORT_END { MEM_NB_SUPPORT_STRUCT_VERSION, 0, 0, 0, 0, 0 }

#if (AGESA_ENTRY_INIT_POST == TRUE)
  /*----------------------------------------------------------------------------------
   * FLOW CONTROL FUNCTION
   *
   *  This section selects the function that controls the memory initialization sequence
   *  based upon the number of processor families that the BIOS will support.
   */

  extern MEM_FLOW_CFG MemMFlowDef;

  #if (OPTION_MEMCTLR_ON == TRUE)
    extern MEM_FLOW_CFG MemMFlowON;
    #define MEM_MAIN_FLOW_CONTROL_PTR_ON MemMFlowON,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_ON MemMFlowDef,
  #endif

  MEM_FLOW_CFG* CONST memFlowControlInstalled[] = {
    MEM_MAIN_FLOW_CONTROL_PTR_ON
    NULL
  };

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
    #define MEM_FEATURE_ECCX8  MemMDefRet
  #else
    #define MEM_MAIN_FEATURE_ECC  MemMDefRet
    #define MEM_FEATURE_CK_ECC   MemFDefRet
    #define MEM_FEATURE_ECC   MemFDefRet
    #define MEM_FEATURE_ECCX8  MemMDefRet
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

  extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
  #if (OPTION_PARALLEL_TRAINING == TRUE)
    extern OPTION_MEM_FEATURE_MAIN MemMParallelTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMParallelTraining
  #else
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
   * ONTARIO FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_ON == TRUE)
    #if OPTION_DDR3
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #if (OPTION_CONTINOUS_PATTERN_GENERATION == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitCPGClientNb;
      #undef MEM_TECH_FEATURE_CPG
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGClientNb
    #else
      #undef MEM_TECH_FEATURE_CPG
      #define MEM_TECH_FEATURE_CPG    MemFDefRet
    #endif

    #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitDqsTrainRcvrEnHwNb;
      #undef MEM_TECH_FEATURE_HWRXEN
      #define MEM_TECH_FEATURE_HWRXEN    MemNInitDqsTrainRcvrEnHwNb
    #else
      extern OPTION_MEM_FEATURE_NB MemNDisableDqsTrainRcvrEnHwNb;
      #undef MEM_TECH_FEATURE_HWRXEN
      #define MEM_TECH_FEATURE_HWRXEN    MemNDisableDqsTrainRcvrEnHwNb
    #endif

    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    extern OPTION_MEM_FEATURE_NB MemFStandardTraining;
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    #if (OPTION_EARLY_SAMPLES == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitEarlySampleSupportON;
      #define MEM_EARLY_SAMPLE_SUPPORT    MemNInitEarlySampleSupportON
    #else
      #define MEM_EARLY_SAMPLE_SUPPORT    MemFDefRet
    #endif

    CONST MEM_FEAT_BLOCK_NB  MemFeatBlockOn = {
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
      MEM_EARLY_SAMPLE_SUPPORT,
      MEM_TECH_FEATURE_CPG,
      MEM_TECH_FEATURE_HWRXEN
    };

    #undef MEM_NB_SUPPORT_ON
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockON;
    extern MEM_INITIALIZER MemNInitDefaultsON;
    #define MEM_NB_SUPPORT_ON { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockON, MemNInitDefaultsON, &MemFeatBlockOn, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_ON, MEM_IDENDIMM_ON },

  #endif // OPTION_MEMCTRL_ON

  /*---------------------------------------------------------------------------------------------------
   * MAIN FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  CONST MEM_FEAT_BLOCK_MAIN MemFeatMain = {
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

  #if OPTION_MEMCTLR_ON
    extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceON;
    #if OPTION_DDR3
      #undef TECH_TRAIN_ENTER_HW_TRN_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTFeatDef
      #undef TECH_TRAIN_EXIT_HW_TRN_DDR3
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
      #undef TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3
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
      CONST MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3ON = {
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
//      extern OPTION_MEM_FEATURE_NB MemNDQSTiming3Nb;
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
//      extern OPTION_MEM_FEATURE_NB memNSequenceDDR3Nb;
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_ON {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR3Nb, memNEnableTrainSequenceON, &memTechTrainingFeatSequenceDDR3ON },
    #else
      #undef TECH_TRAIN_ENTER_HW_TRN_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3 MemTFeatDef
      #undef TECH_TRAIN_EXIT_HW_TRN_DDR3
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
      #define NB_TRAIN_FLOW_DDR3    (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_ON  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
  #else
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_ON  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
  #endif

  #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_END { MEM_NB_SUPPORT_STRUCT_VERSION, 0, 0, 0 }

  CONST MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR3[] = {
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_ON
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_END
  };
  /*---------------------------------------------------------------------------------------------------
   * NB TRAINING FLOW CONTROL
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #define NB_TRAIN_FLOW_DDR2    (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue

  OPTION_MEM_FEATURE_NB* CONST memNTrainFlowControl[] = {    // Training flow control
    NB_TRAIN_FLOW_DDR2,
    NB_TRAIN_FLOW_DDR3,
  };
  /*---------------------------------------------------------------------------------------------------
   * TECHNOLOGY BLOCK
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  MEM_TECH_CONSTRUCTOR* CONST memTechInstalled[] = {    // Types of technology installed
    MEM_TECH_CONSTRUCTOR_DDR3
    NULL
  };
   /*---------------------------------------------------------------------------------------------------
   * PLATFORM SPECIFIC BLOCK FORM FACTOR DEFINITION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */

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
  MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledON[MAX_FF_TYPES] = {
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

  #if OPTION_MEMCTLR_ON
    #if OPTION_UDIMMS
      #if OPTION_DDR3
        #define PSC_ON_UDIMM_DDR3    MemAGetPsCfgUON3,
      #else
        #define PSC_ON_UDIMM_DDR3
      #endif
    #endif
    #if OPTION_RDIMMS
      #if OPTION_DDR3
        #define PSC_ON_RDIMM_DDR3    //MemAGetPsCfgRON3,
      #else
        #define PSC_ON_RDIMM_DDR3
      #endif
    #endif
    #if OPTION_SODIMMS
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

  #ifndef PSC_ON_UDIMM_DDR3
    #define PSC_ON_UDIMM_DDR3
  #endif
  #ifndef PSC_ON_RDIMM_DDR3
    #define PSC_ON_RDIMM_DDR3
  #endif
  #ifndef PSC_ON_SODIMM_DDR3
    #define PSC_ON_SODIMM_DDR3
  #endif

  MEM_PLATFORM_CFG* CONST memPlatformTypeInstalled[] = {
    PSC_ON_UDIMM_DDR3
    PSC_ON_RDIMM_DDR3
    PSC_ON_SODIMM_DDR3
    NULL
  };
  CONST UINTN SIZE_OF_PLATFORM = (sizeof (memPlatformTypeInstalled) / sizeof (MEM_PLATFORM_CFG*));
//  #if SIZE_OF_PLATFORM > MAX_PLATFORM_TYPES
//    #error   Size of memPlatformTypeInstalled array larger than MAX_PLATFORM_TYPES
//  #endif

  /*---------------------------------------------------------------------------------------------------
   * EXTRACTABLE PLATFORM SPECIFIC CONFIGURATION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #define MEM_PSC_FLOW_BLOCK_END NULL
  #define PSC_TBL_END NULL
  #define MEM_PSC_FLOW_DEFTRUE (BOOLEAN (*) (MEM_NB_BLOCK*, MEM_PSC_TABLE_BLOCK *)) memDefTrue


  MEM_PSC_FLOW_BLOCK* CONST memPlatSpecFlowArray[] = {
    MEM_PSC_FLOW_BLOCK_END
  };

  /*---------------------------------------------------------------------------------------------------
  *
  *  LRDIMM CONTROL
  *
  *---------------------------------------------------------------------------------------------------
  */
  #if (OPTION_LRDIMMS == TRUE)
    #define MEM_TECH_FEATURE_LRDIMM_INIT    MemTFeatDef
  #else //#if (OPTION_LRDIMMS == FALSE)
    #define MEM_TECH_FEATURE_LRDIMM_INIT    MemTFeatDef
  #endif
  CONST MEM_TECH_LRDIMM memLrdimmSupported = {
    MEM_TECH_LRDIMM_STRUCT_VERSION,
    MEM_TECH_FEATURE_LRDIMM_INIT
  };
#else
  /*---------------------------------------------------------------------------------------------------
   * MAIN FLOW CONTROL
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  MEM_FLOW_CFG* CONST memFlowControlInstalled[] = {
    NULL
  };
  /*---------------------------------------------------------------------------------------------------
   * NB TRAINING FLOW CONTROL
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  OPTION_MEM_FEATURE_NB* CONST memNTrainFlowControl[] = {    // Training flow control
    NULL,
    NULL,
  };
  /*---------------------------------------------------------------------------------------------------
   * DEFAULT TECHNOLOGY BLOCK
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  MEM_TECH_CONSTRUCTOR* CONST memTechInstalled[] = {    // Types of technology installed
    NULL
  };

  /*---------------------------------------------------------------------------------------------------
   * DEFAULT TECHNOLOGY MAP
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  CONST UINT8 MemoryTechnologyMap[MAX_SOCKETS_SUPPORTED] = {0, 0, 0, 0, 0, 0, 0, 0};

  /*---------------------------------------------------------------------------------------------------
   * DEFAULT MAIN FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  CONST MEM_FEAT_BLOCK_MAIN MemFeatMain = {
    0
  };

  /*---------------------------------------------------------------------------------------------------
   * DEFAULT NORTHBRIDGE SUPPORT LIST
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
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
  #if OPTION_DDR3
    CONST MEM_TECH_FEAT_BLOCK  memTechTrainingFeatDDR3 = {
      0
    };
    CONST MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR3[] = {
      { 0 }
    };
  #endif

    /*---------------------------------------------------------------------------------------------------
     * DEFAULT Platform Specific list
     *
     *
     *---------------------------------------------------------------------------------------------------
     */
  #if (OPTION_MEMCTLR_ON == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledON[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  /*----------------------------------------------------------------------
   * DEFAULT PSCFG DEFINITIONS
   *
   *----------------------------------------------------------------------
   */
  MEM_PLATFORM_CFG* CONST memPlatformTypeInstalled[] = {
    NULL
  };

  /*----------------------------------------------------------------------
   * EXTRACTABLE PLATFORM SPECIFIC CONFIGURATION
   *
   *----------------------------------------------------------------------
   */
  MEM_PSC_FLOW_BLOCK* CONST memPlatSpecFlowArray[] = {
    NULL
  };

  CONST MEM_TECH_LRDIMM memLrdimmSupported = {
    MEM_TECH_LRDIMM_STRUCT_VERSION,
    NULL
  };
#endif

/*---------------------------------------------------------------------------------------------------
 * NORTHBRIDGE SUPPORT LIST
 *
 *
 *---------------------------------------------------------------------------------------------------
 */
CONST MEM_NB_SUPPORT memNBInstalled[] = {
  MEM_NB_SUPPORT_ON
  MEM_NB_SUPPORT_END
};

#endif  // _OPTION_MEMORY_INSTALL_H_
