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
 * @e \$Revision: 85859 $   @e \$Date: 2013-01-14 02:57:14 -0600 (Mon, 14 Jan 2013) $
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

#ifndef _OPTION_MEMORY_INSTALL_H_
#define _OPTION_MEMORY_INSTALL_H_

/* Memory Includes */
#include "OptionMemory.h"

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
  return FALSE;
}

#if (OPTION_MEMCTLR_TN == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE) || ((AGESA_ENTRY_INIT_POST) && (OPTION_S3_MEM_SUPPORT == TRUE)))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockTN;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN MemS3ResumeConstructNBBlockTN
      #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
        extern OPTION_MEM_FEATURE_MAIN MemMS3Save;
        #define MEM_MAIN_FEATURE_MEM_S3_SAVE MemMS3Save
      #else
        #define MEM_MAIN_FEATURE_MEM_S3_SAVE MemMDefRet
      #endif
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN MemFS3DefConstructorRet
      #define MEM_MAIN_FEATURE_MEM_S3_SAVE MemMDefRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN MemFS3DefConstructorRet
    #define MEM_MAIN_FEATURE_MEM_S3_SAVE MemMDefRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorTN;
    #define MEM_IDENDIMM_TN MemNIdentifyDimmConstructorTN
  #else
    #define MEM_IDENDIMM_TN MemNIdentifyDimmConstructorRetDef
  #endif
#endif


#if (OPTION_MEMCTLR_KB == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE) || ((AGESA_ENTRY_INIT_POST) && (OPTION_S3_MEM_SUPPORT == TRUE)))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockKB;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_KB MemS3ResumeConstructNBBlockKB
      #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
        extern OPTION_MEM_FEATURE_MAIN MemMS3Save;
        #define MEM_MAIN_FEATURE_MEM_S3_SAVE MemMS3Save
      #else
        #define MEM_MAIN_FEATURE_MEM_S3_SAVE MemMDefRet
      #endif
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_KB MemFS3DefConstructorRet
      #define MEM_MAIN_FEATURE_MEM_S3_SAVE MemMDefRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_KB MemFS3DefConstructorRet
    #define MEM_MAIN_FEATURE_MEM_S3_SAVE MemMDefRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorKB;
    #define MEM_IDENDIMM_KB MemNIdentifyDimmConstructorKB
  #else
    #define MEM_IDENDIMM_KB MemNIdentifyDimmConstructorRetDef
  #endif
#endif



/*----------------------------------------------------------------------------------
 * NORTHBRIDGE BLOCK CONSTRUCTOR AND INITIALIZER FUNCTION DEFAULT ASSIGNMENTS
 *
 *----------------------------------------------------------------------------------
*/
#define MEM_NB_SUPPORT_TN
#define MEM_NB_SUPPORT_KB
#define MEM_NB_SUPPORT_END { MEM_NB_SUPPORT_STRUCT_VERSION, 0, 0, 0, 0, 0 }

#if (AGESA_ENTRY_INIT_POST == TRUE)
  /*----------------------------------------------------------------------------------
   * FLOW CONTROL FUNCTION
   *
   *  This section selects the function that controls the memory initialization sequence
   *  based upon the number of processor families that the BIOS will support.
   */
  extern MEM_FLOW_CFG MemMFlowDef;

  #if (OPTION_MEMCTLR_TN == TRUE)
    extern MEM_FLOW_CFG MemMFlowTN;
    #define MEM_MAIN_FLOW_CONTROL_PTR_TN MemMFlowTN,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_TN MemMFlowDef,
  #endif


  #if (OPTION_MEMCTLR_KB == TRUE)
    extern MEM_FLOW_CFG MemMFlowKB;
    #define MEM_MAIN_FLOW_CONTROL_PTR_KB MemMFlowKB,
  #else
    extern MEM_FLOW_CFG MemMFlowDef;
    #define MEM_MAIN_FLOW_CONTROL_PTR_KB MemMFlowDef,
  #endif


  MEM_FLOW_CFG* CONST memFlowControlInstalled[] = {
    MEM_MAIN_FLOW_CONTROL_PTR_TN
    MEM_MAIN_FLOW_CONTROL_PTR_KB
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

  #if (OPTION_AGGRESSOR == TRUE)
    extern OPTION_MEM_FEATURE_MAIN MemMAggressor;
    #define MEM_MAIN_FEATURE_AGGRESSOR  MemMAggressor
    extern OPTION_MEM_FEATURE_NB MemFAggressorInit;
    #define MEM_FEATURE_AGGRESSOR  MemFAggressorInit
  #else //#if (OPTION_AGGRESSOR == FALSE)
    #define MEM_MAIN_FEATURE_AGGRESSOR  MemMDefRet
    #define MEM_FEATURE_AGGRESSOR  MemFDefRet
  #endif

  #if (OPTION_DMI == TRUE)
    #if (OPTION_DDR3 == TRUE)
      extern OPTION_MEM_FEATURE_MAIN MemFDMISupport3;
      #define MEM_MAIN_FEATURE_MEM_DMI MemFDMISupport3
    #else
      #define MEM_MAIN_FEATURE_MEM_DMI MemMDefRet
    #endif
  #else
    #define MEM_MAIN_FEATURE_MEM_DMI MemMDefRet
  #endif

  #if (OPTION_CRAT == TRUE)
    extern OPTION_MEM_FEATURE_MAIN MemFCratSupport;
    #define MEM_MAIN_FEATURE_MEM_CRAT MemFCratSupport
  #else
    #define MEM_MAIN_FEATURE_MEM_CRAT MemMDefRet
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
    #ifndef EXTERN_MEMMSTANDARDTRAINING
    #define EXTERN_MEMMSTANDARDTRAINING
        extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #endif
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

  #if (OPTION_AMP == TRUE)
    extern OPTION_MEM_FEATURE_NB MemFAMP;
    #define MEM_FEATURE_AMP  MemFAMP
  #else
    #define MEM_FEATURE_AMP  MemFDefRet
  #endif

  #if (OPTION_DATA_EYE == TRUE)
    extern OPTION_MEM_FEATURE_NB MemF2DDataEyeInit;
    #define MEM_FEATURE_DATA_EYE  MemF2DDataEyeInit
  #else //#if (OPTION_DATA_EYE == FALSE)
    #define MEM_FEATURE_DATA_EYE  MemFDefRet
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
   * TRINITY FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_TN == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_MAIN_FEATURE_LVDDR3
      extern OPTION_MEM_FEATURE_MAIN MemMLvDdr3PerformanceEnhPre;
      #define MEM_MAIN_FEATURE_LVDDR3 MemMLvDdr3PerformanceEnhPre
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #if (OPTION_EARLY_SAMPLES == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitEarlySampleSupportTN;
      #define MEM_EARLY_SAMPLE_SUPPORT    MemNInitEarlySampleSupportTN
    #else
      #define MEM_EARLY_SAMPLE_SUPPORT    MemFDefRet
    #endif

    #if (OPTION_CONTINOUS_PATTERN_GENERATION == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitCPGUnb;
      #undef MEM_TECH_FEATURE_CPG
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGUnb
    #else
      #undef MEM_TECH_FEATURE_CPG
      #define MEM_TECH_FEATURE_CPG    MemFDefRet
    #endif

    #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
      #undef MEM_TECH_FEATURE_HWRXEN
      #define MEM_TECH_FEATURE_HWRXEN    MemNInitDqsTrainRcvrEnHwNb
    #else
      extern OPTION_MEM_FEATURE_NB MemNDisableDqsTrainRcvrEnHwNb;
      #undef MEM_TECH_FEATURE_HWRXEN
      #define MEM_TECH_FEATURE_HWRXEN    MemNDisableDqsTrainRcvrEnHwNb
    #endif


    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    #if (OPTION_RDDQS_2D_TRAINING == TRUE)
      extern OPTION_MEM_FEATURE_MAIN MemMStandardTrainingUsingAdjacentDies;
      #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTrainingUsingAdjacentDies
    #else
      extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
      #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    #endif
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    CONST MEM_FEAT_BLOCK_NB  MemFeatBlockTN = {
      MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
      MEM_FEATURE_ONLINE_SPARE,
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
      MEM_FEATURE_ONDIMMTHERMAL,
      MEM_TECH_FEATURE_DRAMINIT,
      MEM_FEATURE_DIMM_EXCLUDE,
      MEM_EARLY_SAMPLE_SUPPORT,
      MEM_TECH_FEATURE_CPG,
      MEM_TECH_FEATURE_HWRXEN,
      MEM_FEATURE_AMP,
      MemFDefRet,
      MemFDefRet,
      MemFDefRet
    };

    #undef MEM_NB_SUPPORT_TN
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockTN;
    extern MEM_INITIALIZER MemNInitDefaultsTN;
    #define MEM_NB_SUPPORT_TN { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockTN, MemNInitDefaultsTN, &MemFeatBlockTN, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN, MEM_IDENDIMM_TN },
  #endif // OPTION_MEMCTRL_TN


  /*---------------------------------------------------------------------------------------------------
   * KABINI FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_KB == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_MAIN_FEATURE_LVDDR3
      extern OPTION_MEM_FEATURE_MAIN MemMLvDdr3PerformanceEnhPre;
      #define MEM_MAIN_FEATURE_LVDDR3 MemMLvDdr3PerformanceEnhPre
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #if (OPTION_EARLY_SAMPLES == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitEarlySampleSupportKB;
      #define MEM_EARLY_SAMPLE_SUPPORT    MemNInitEarlySampleSupportKB
    #else
      #define MEM_EARLY_SAMPLE_SUPPORT    MemFDefRet
    #endif

    #if (OPTION_CONTINOUS_PATTERN_GENERATION == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitCPGUnb;
      #undef MEM_TECH_FEATURE_CPG
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGUnb
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
    #if (OPTION_RDDQS_2D_TRAINING == TRUE)
      extern OPTION_MEM_FEATURE_MAIN MemMStandardTrainingUsingAdjacentDies;
      #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTrainingUsingAdjacentDies
      extern OPTION_MEM_FEATURE_NB MemFRdWr2DTrainingInitKB;
      #define MEM_FEATURE_RDWR_2D_TRAINING MemFRdWr2DTrainingInitKB
    #else
      #ifndef EXTERN_MEMMSTANDARDTRAINING
      #define EXTERN_MEMMSTANDARDTRAINING
        extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
      #endif
      #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
      #define MEM_FEATURE_RDWR_2D_TRAINING MemFDefRet
    #endif
    extern OPTION_MEM_FEATURE_NB MemFStandardTraining;
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    CONST MEM_FEAT_BLOCK_NB  MemFeatBlockKB = {
      MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
      MEM_FEATURE_ONLINE_SPARE,
      MEM_FEATURE_BANK_INTERLEAVE,
      MEM_FEATURE_UNDO_BANK_INTERLEAVE,
      MemFDefRet,
      MemFDefRet,
      MemFDefRet,
      MemFDefRet,
      MEM_FEATURE_CK_ECC,
      MEM_FEATURE_ECC,
      MEM_FEATURE_TRAINING,
      MEM_FEATURE_LVDDR3,
      MEM_FEATURE_ONDIMMTHERMAL,
      MEM_TECH_FEATURE_DRAMINIT,
      MEM_FEATURE_DIMM_EXCLUDE,
      MEM_EARLY_SAMPLE_SUPPORT,
      MEM_TECH_FEATURE_CPG,
      MEM_TECH_FEATURE_HWRXEN,
      MemFDefRet,
      MemFDefRet,
      MEM_FEATURE_RDWR_2D_TRAINING,
      MemFDefRet
    };

    #undef MEM_NB_SUPPORT_KB
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockKB;
    extern MEM_INITIALIZER MemNInitDefaultsKB;
    #define MEM_NB_SUPPORT_KB { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockKB, MemNInitDefaultsKB, &MemFeatBlockKB, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_KB, MEM_IDENDIMM_KB },
  #endif // OPTION_MEMCTRL_KB


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
    MEM_MAIN_FEATURE_MEM_CRAT,
    MEM_MAIN_FEATURE_LVDDR3,
    MEM_MAIN_FEATURE_UMAALLOC,
    MEM_MAIN_FEATURE_MEM_SAVE,
    MEM_MAIN_FEATURE_MEM_RESTORE,
    MEM_MAIN_FEATURE_MEM_S3_SAVE,
    MEM_MAIN_FEATURE_AGGRESSOR
  };


  /*---------------------------------------------------------------------------------------------------
   * Technology Training SPECIFIC CONFIGURATION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #define MEM_TECH_TRAINING_FEAT_NULL_TERNMIATOR 0

  #if OPTION_MEMCTLR_TN
    extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceTN;
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
    #define NB_TRAIN_FLOW_DDR2    (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_TN  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #if OPTION_DDR3
      #undef TECH_TRAIN_ENTER_HW_TRN_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTPreparePhyAssistedTraining
      #undef TECH_TRAIN_EXIT_HW_TRN_DDR3
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
        #ifdef TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3
          #undef TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3
        #endif
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #ifdef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
          #undef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
        #endif
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
        #if (OPTION_HW_DQS_REC_EN_SEED_TRAINING == TRUE)
          #undef TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3
          extern MEM_TECH_FEAT MemNRdPosTrnTN;
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemNRdPosTrnTN
        #else
          #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE  || OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
            #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
          #else
            #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3 MemTFeatDef
          #endif
        #endif
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
        #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE  || OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
        #else
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3 MemTFeatDef
        #endif
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3    MemTFeatDef
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #undef TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3
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
      #undef TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3
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
      #if (OPTION_RDDQS_2D_TRAINING == TRUE)
        #undef TECH_TRAIN_DQS_2D_DDR3
        #define TECH_TRAIN_DQS_2D_DDR3    MemTAmdRdDqs2DTraining
      #else
        #undef TECH_TRAIN_DQS_2D_DDR3
        #define TECH_TRAIN_DQS_2D_DDR3 MemTFeatDef
      #endif
      CONST MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3TN = {
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
        TECH_TRAIN_MAX_RD_LAT_DDR3,
        TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3,
        TECH_TRAIN_DQS_2D_DDR3
      };
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_TN {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR3Nb, memNEnableTrainSequenceTN, &memTechTrainingFeatSequenceDDR3TN },
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
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_TN  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
  #else
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_TN  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_TN  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
  #endif


  #if OPTION_MEMCTLR_KB
    extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceKB;
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
    #define NB_TRAIN_FLOW_DDR2    (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_KB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #if OPTION_DDR3
      #undef TECH_TRAIN_ENTER_HW_TRN_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3   MemTPreparePhyAssistedTraining
      #undef TECH_TRAIN_EXIT_HW_TRN_DDR3
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
        #ifdef TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3
          #undef TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3
        #endif
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #ifdef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
          #undef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
        #endif
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
        #if (OPTION_HW_DQS_REC_EN_SEED_TRAINING == TRUE)
          #undef TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3
          extern MEM_TECH_FEAT MemNRdPosTrnKB;
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemNRdPosTrnKB
        #else
          #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE  || OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
            #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
          #else
            #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3 MemTFeatDef
          #endif
        #endif
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
        #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE  || OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
        #else
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3 MemTFeatDef
        #endif
      #endif
      #if (OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3    MemTFeatDef
      #else
        #define TECH_TRAIN_NON_OPT_SW_DQS_REC_EN_P1_DDR3 MemTFeatDef
      #endif
      #undef TECH_TRAIN_OPT_SW_DQS_REC_EN_P1_DDR3
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
      #undef TECH_TRAIN_OPT_SW_RD_WR_POS_DDR3
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
      #if (OPTION_RDDQS_2D_TRAINING == TRUE)
        extern  MEM_TECH_FEAT MemFAmdRdDqs2DTraining;
        #undef TECH_TRAIN_DQS_2D_DDR3
        #define TECH_TRAIN_DQS_2D_DDR3    MemFAmdRdDqs2DTraining
      #else
        #undef TECH_TRAIN_DQS_2D_DDR3
        #define TECH_TRAIN_DQS_2D_DDR3 MemTFeatDef
      #endif
      CONST MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3KB = {
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
        TECH_TRAIN_MAX_RD_LAT_DDR3,
        TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3,
        TECH_TRAIN_DQS_2D_DDR3
      };
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_KB {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR3Nb, memNEnableTrainSequenceKB, &memTechTrainingFeatSequenceDDR3KB },
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
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_KB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
  #else
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_KB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_KB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
  #endif



  #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_END { MEM_NB_SUPPORT_STRUCT_VERSION, 0, 0, 0 }
  CONST MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR2[] = {
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_TN
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_KB
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_END
  };

  CONST MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR3[] = {
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_TN
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_KB
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_END
  };
  /*---------------------------------------------------------------------------------------------------
   * NB TRAINING FLOW CONTROL
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
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

  /*---------------------------------------------------------------------------------------------------
   * PLATFORM-SPECIFIC CONFIGURATION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */

  /*----------------------------------------------------------------------
   * DEFAULT PSCFG DEFINITIONS
   *
   *----------------------------------------------------------------------
   */

  MEM_PLATFORM_CFG* CONST memPlatformTypeInstalled[] = {
    NULL
  };
  CONST UINTN SIZE_OF_PLATFORM = (sizeof (memPlatformTypeInstalled) / sizeof (MEM_PLATFORM_CFG*));

  /*---------------------------------------------------------------------------------------------------
   * EXTRACTABLE PLATFORM SPECIFIC CONFIGURATION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #define MEM_PSC_FLOW_BLOCK_END NULL
  #define PSC_TBL_END NULL
  #define MEM_PSC_FLOW_DEFTRUE (BOOLEAN (*) (MEM_NB_BLOCK*, CONST MEM_PSC_TABLE_BLOCK *)) memDefTrue

  #define PSC_TBL_TN_UDIMM3_S2D_FM2
  #define PSC_TBL_TN_SODIMM3_S2D_FS1
  #define PSC_TBL_TN_SODIMM3_S2D_FP2
  #define PSC_TBL_TN_SODIMM3_S2D_FM2
  #if OPTION_MEMCTLR_TN
    #if OPTION_FS1_SOCKET_SUPPORT
      extern PSC_TBL_ENTRY TNClkDisMapEntSOFS1;
      #define PSC_TBL_TN_CLK_DIS_FS1 &TNClkDisMapEntSOFS1,
      extern PSC_TBL_ENTRY TNSODdr3ODTTriEntFS1;
      #define PSC_TBL_TN_ODT_TRI_FS1 &TNSODdr3ODTTriEntFS1,
      extern PSC_TBL_ENTRY TNSODdr3CSTriEntFS1;
      #define PSC_TBL_TN_CS_TRI_FS1 &TNSODdr3CSTriEntFS1,
    #endif
    #if (OPTION_FM2_SOCKET_SUPPORT || OPTION_FM2r2_SOCKET_SUPPORT)
      extern PSC_TBL_ENTRY TNClkDisMapEntUFM2;
      #define PSC_TBL_TN_CLK_DIS_FM2 &TNClkDisMapEntUFM2,
      extern PSC_TBL_ENTRY TNUDdr3ODTTriEntFM2;
      #define PSC_TBL_TN_ODT_TRI_FM2 &TNUDdr3ODTTriEntFM2,
      extern PSC_TBL_ENTRY TNUDdr3CSTriEntFM2;
      #define PSC_TBL_TN_CS_TRI_FM2 &TNUDdr3CSTriEntFM2,
    #endif
    #if OPTION_FP2_SOCKET_SUPPORT
      extern PSC_TBL_ENTRY TNClkDisMapEntSOFP2;
      #define PSC_TBL_TN_CLK_DIS_FP2 &TNClkDisMapEntSOFP2,
      extern PSC_TBL_ENTRY TNSODdr3ODTTriEntFP2;
      #define PSC_TBL_TN_ODT_TRI_FP2 &TNSODdr3ODTTriEntFP2,
      extern PSC_TBL_ENTRY TNSODdr3CSTriEntFP2;
      #define PSC_TBL_TN_CS_TRI_FP2 &TNSODdr3CSTriEntFP2,
    #endif
    #if OPTION_UDIMMS
      extern PSC_TBL_ENTRY TNMaxFreqTblEntU;
      #define PSC_TBL_TN_UDIMM3_MAX_FREQ  &TNMaxFreqTblEntU,
      extern PSC_TBL_ENTRY RLMaxFreqTblEntU;
      #define PSC_TBL_RL_UDIMM3_MAX_FREQ  &RLMaxFreqTblEntU,
      extern PSC_TBL_ENTRY TNDramTermTblEntU;
      #define PSC_TBL_TN_UDIMM3_DRAM_TERM  &TNDramTermTblEntU,
      extern PSC_TBL_ENTRY TNSAOTblEntU3;
      #define PSC_TBL_TN_UDIMM3_SAO  &TNSAOTblEntU3,
      #if (OPTION_FM2_SOCKET_SUPPORT || OPTION_FM2r2_SOCKET_SUPPORT)
        extern PSC_TBL_ENTRY TNMaxFreqTblEntUFM2;
        #define PSC_TBL_TN_UDIMM3_MAX_FREQ_FM2  &TNMaxFreqTblEntUFM2,
        extern PSC_TBL_ENTRY RLMaxFreqTblEntUFM2;
        #define PSC_TBL_RL_UDIMM3_MAX_FREQ_FM2  &RLMaxFreqTblEntUFM2,
        #undef PSC_TBL_TN_UDIMM3_S2D_FM2
        extern PSC_TBL_ENTRY S2DTblEntUFM2;
        #define PSC_TBL_TN_UDIMM3_S2D_FM2  &S2DTblEntUFM2,
      #endif
    #endif
    #if OPTION_SODIMMS
      extern PSC_TBL_ENTRY TNSAOTblEntSO3;
      #define PSC_TBL_TN_SODIMM3_SAO  &TNSAOTblEntSO3,
      extern PSC_TBL_ENTRY TNDramTermTblEntSO;
      #define PSC_TBL_TN_SODIMM3_DRAM_TERM  &TNDramTermTblEntSO,
      #if OPTION_FS1_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY TNMaxFreqTblEntSOFS1;
        #define PSC_TBL_TN_SODIMM3_MAX_FREQ_FS1  &TNMaxFreqTblEntSOFS1,
        #undef PSC_TBL_TN_SODIMM3_S2D_FS1
        #define PSC_TBL_TN_SODIMM3_S2D_FS1
      #endif
      #if (OPTION_FM2_SOCKET_SUPPORT || OPTION_FM2r2_SOCKET_SUPPORT)
        extern PSC_TBL_ENTRY TNMaxFreqTblEntSO;
        #define PSC_TBL_TN_SODIMM3_MAX_FREQ  &TNMaxFreqTblEntSO,
        #undef PSC_TBL_TN_SODIMM3_S2D_FM2
        extern PSC_TBL_ENTRY S2DTblEntUFM2;
        #define PSC_TBL_TN_SODIMM3_S2D_FM2 &S2DTblEntUFM2,
      #endif
      #if OPTION_FP2_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY TNSAOTblEntSODWNSO3;
        #define PSC_TBL_TN_SODWN_SODIMM3_SAO  &TNSAOTblEntSODWNSO3,
        extern PSC_TBL_ENTRY TNDramTermTblEntSODWNSO;
        #define PSC_TBL_TN_SODWN_SODIMM3_DRAM_TERM  &TNDramTermTblEntSODWNSO,
        extern PSC_TBL_ENTRY TNMaxFreqTblEntSODWNSO;
        #define PSC_TBL_TN_SODWN_SODIMM3_MAX_FREQ  &TNMaxFreqTblEntSODWNSO,
        extern PSC_TBL_ENTRY TNMaxFreqTblEntSOFP2;
        #define PSC_TBL_TN_SODIMM3_MAX_FREQ_FP2  &TNMaxFreqTblEntSOFP2,
        extern PSC_TBL_ENTRY RLMaxFreqTblEntSOFP2;
        #define PSC_TBL_RL_SODIMM3_MAX_FREQ_FP2  &RLMaxFreqTblEntSOFP2,
        #undef PSC_TBL_TN_SODIMM3_S2D_FP2
        #define PSC_TBL_TN_SODIMM3_S2D_FP2
      #endif
    #endif
    extern PSC_TBL_ENTRY TNMR0WrTblEntry;
    extern PSC_TBL_ENTRY TNMR0CLTblEntry;
    extern PSC_TBL_ENTRY TNDdr3CKETriEnt;
    extern PSC_TBL_ENTRY TNOdtPatTblEnt;


    #ifndef PSC_TBL_TN_SODIMM3_MAX_FREQ
      #define PSC_TBL_TN_SODIMM3_MAX_FREQ
    #endif
    #ifndef PSC_TBL_TN_SODIMM3_MAX_FREQ_FS1
      #define PSC_TBL_TN_SODIMM3_MAX_FREQ_FS1
    #endif
    #ifndef PSC_TBL_TN_SODIMM3_MAX_FREQ_FP2
      #define PSC_TBL_TN_SODIMM3_MAX_FREQ_FP2
    #endif
    #ifndef PSC_TBL_RL_SODIMM3_MAX_FREQ_FP2
      #define PSC_TBL_RL_SODIMM3_MAX_FREQ_FP2
    #endif
    #ifndef PSC_TBL_TN_SODWN_SODIMM3_MAX_FREQ
      #define PSC_TBL_TN_SODWN_SODIMM3_MAX_FREQ
    #endif
    #ifndef PSC_TBL_TN_UDIMM3_MAX_FREQ
      #define PSC_TBL_TN_UDIMM3_MAX_FREQ
    #endif
    #ifndef PSC_TBL_RL_UDIMM3_MAX_FREQ
      #define PSC_TBL_RL_UDIMM3_MAX_FREQ
    #endif
    #ifndef PSC_TBL_TN_UDIMM3_MAX_FREQ_FM2
      #define PSC_TBL_TN_UDIMM3_MAX_FREQ_FM2
    #endif
    #ifndef PSC_TBL_RL_UDIMM3_MAX_FREQ_FM2
      #define PSC_TBL_RL_UDIMM3_MAX_FREQ_FM2
    #endif
    #ifndef PSC_TBL_TN_UDIMM3_DRAM_TERM
      #define PSC_TBL_TN_UDIMM3_DRAM_TERM
    #endif
    #ifndef PSC_TBL_TN_SODIMM3_DRAM_TERM
      #define PSC_TBL_TN_SODIMM3_DRAM_TERM
    #endif
    #ifndef PSC_TBL_TN_SODWN_SODIMM3_DRAM_TERM
      #define PSC_TBL_TN_SODWN_SODIMM3_DRAM_TERM
    #endif
    #ifndef PSC_TBL_TN_SODIMM3_SAO
      #define PSC_TBL_TN_SODIMM3_SAO
    #endif
    #ifndef PSC_TBL_TN_SODWN_SODIMM3_SAO
      #define PSC_TBL_TN_SODWN_SODIMM3_SAO
    #endif
    #ifndef PSC_TBL_TN_UDIMM3_SAO
      #define PSC_TBL_TN_UDIMM3_SAO
    #endif
    #ifndef PSC_TBL_TN_CLK_DIS_FM2
      #define PSC_TBL_TN_CLK_DIS_FM2
    #endif
    #ifndef PSC_TBL_TN_ODT_TRI_FM2
      #define PSC_TBL_TN_ODT_TRI_FM2
    #endif
    #ifndef PSC_TBL_TN_CS_TRI_FM2
      #define PSC_TBL_TN_CS_TRI_FM2
    #endif
    #ifndef PSC_TBL_TN_CLK_DIS_FS1
      #define PSC_TBL_TN_CLK_DIS_FS1
    #endif
    #ifndef PSC_TBL_TN_ODT_TRI_FS1
      #define PSC_TBL_TN_ODT_TRI_FS1
    #endif
    #ifndef PSC_TBL_TN_CS_TRI_FS1
      #define PSC_TBL_TN_CS_TRI_FS1
    #endif
    #ifndef PSC_TBL_TN_CLK_DIS_FP2
      #define PSC_TBL_TN_CLK_DIS_FP2
    #endif
    #ifndef PSC_TBL_TN_ODT_TRI_FP2
      #define PSC_TBL_TN_ODT_TRI_FP2
    #endif
    #ifndef PSC_TBL_TN_CS_TRI_FP2
      #define PSC_TBL_TN_CS_TRI_FP2
    #endif

    PSC_TBL_ENTRY* CONST  memPSCTblMaxFreqArrayTN[] = {
      PSC_TBL_TN_SODIMM3_MAX_FREQ_FS1
      PSC_TBL_TN_SODIMM3_MAX_FREQ_FP2
      PSC_TBL_RL_SODIMM3_MAX_FREQ_FP2
      PSC_TBL_TN_SODIMM3_MAX_FREQ
      PSC_TBL_TN_SODWN_SODIMM3_MAX_FREQ
      PSC_TBL_TN_UDIMM3_MAX_FREQ_FM2
      PSC_TBL_RL_UDIMM3_MAX_FREQ_FM2
      PSC_TBL_TN_UDIMM3_MAX_FREQ
      PSC_TBL_RL_UDIMM3_MAX_FREQ
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblDramTermArrayTN[] = {
      PSC_TBL_TN_UDIMM3_DRAM_TERM
      PSC_TBL_TN_SODIMM3_DRAM_TERM
      PSC_TBL_TN_SODWN_SODIMM3_DRAM_TERM
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblODTPatArrayTN[] = {
      &TNOdtPatTblEnt,
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblSAOArrayTN[] = {
      PSC_TBL_TN_SODIMM3_SAO
      PSC_TBL_TN_SODWN_SODIMM3_SAO
      PSC_TBL_TN_UDIMM3_SAO
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblMR0WRArrayTN[] = {
      &TNMR0WrTblEntry,
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblMR0CLArrayTN[] = {
      &TNMR0CLTblEntry,
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblGenArrayTN[] = {
      &TNDdr3CKETriEnt,
      PSC_TBL_TN_CLK_DIS_FM2
      PSC_TBL_TN_ODT_TRI_FM2
      PSC_TBL_TN_CS_TRI_FM2
      PSC_TBL_TN_CLK_DIS_FS1
      PSC_TBL_TN_ODT_TRI_FS1
      PSC_TBL_TN_CS_TRI_FS1
      PSC_TBL_TN_CLK_DIS_FP2
      PSC_TBL_TN_ODT_TRI_FP2
      PSC_TBL_TN_CS_TRI_FP2
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblS2DArrayTN[] = {
      PSC_TBL_TN_UDIMM3_S2D_FM2
      PSC_TBL_TN_SODIMM3_S2D_FS1
      PSC_TBL_TN_SODIMM3_S2D_FP2
      PSC_TBL_TN_SODIMM3_S2D_FM2
      PSC_TBL_END
    };

    CONST MEM_PSC_TABLE_BLOCK memPSCTblBlockTN = {
      (PSC_TBL_ENTRY **)&memPSCTblMaxFreqArrayTN,
      (PSC_TBL_ENTRY **)&memPSCTblDramTermArrayTN,
      (PSC_TBL_ENTRY **)&memPSCTblODTPatArrayTN,
      (PSC_TBL_ENTRY **)&memPSCTblSAOArrayTN,
      (PSC_TBL_ENTRY **)&memPSCTblMR0WRArrayTN,
      (PSC_TBL_ENTRY **)&memPSCTblMR0CLArrayTN,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      (PSC_TBL_ENTRY **)&memPSCTblGenArrayTN,
      (PSC_TBL_ENTRY **)&memPSCTblS2DArrayTN,
      NULL,
      NULL
    };

    extern MEM_PSC_FLOW MemPGetMaxFreqSupported;
    extern MEM_PSC_FLOW MemPGetRttNomWr;
    extern MEM_PSC_FLOW MemPGetODTPattern;
    extern MEM_PSC_FLOW MemPGetSAO;
    extern MEM_PSC_FLOW MemPGetMR0WrCL;
    extern MEM_PSC_FLOW MemPGetS2D;

    CONST MEM_PSC_FLOW_BLOCK memPlatSpecFlowTN = {
      &memPSCTblBlockTN,
      MemPGetMaxFreqSupported,
      MemPGetRttNomWr,
      MemPGetODTPattern,
      MemPGetSAO,
      MemPGetMR0WrCL,
      MEM_PSC_FLOW_DEFTRUE,
      MEM_PSC_FLOW_DEFTRUE,
      MEM_PSC_FLOW_DEFTRUE,
      MEM_PSC_FLOW_DEFTRUE,
      MEM_PSC_FLOW_DEFTRUE,
      MemPGetS2D,
      MEM_PSC_FLOW_DEFTRUE
    };
    #define MEM_PSC_FLOW_BLOCK_TN &memPlatSpecFlowTN,
  #else
    #define MEM_PSC_FLOW_BLOCK_TN
  #endif


  #define PSC_TBL_KB_UDIMM3_S2D_FT3
  #define PSC_TBL_KB_SODIMM3_S2D_FT3
  #if OPTION_MEMCTLR_KB
    #if OPTION_FT3_SOCKET_SUPPORT
      extern PSC_TBL_ENTRY KBClkDisMapEntSOFT3;
      #define PSC_TBL_KB_CLK_DIS_FT3 &KBClkDisMapEntSOFT3,
      extern PSC_TBL_ENTRY KBSODdr3ODTTriEntFT3;
      #define PSC_TBL_KB_ODT_TRI_FT3 &KBSODdr3ODTTriEntFT3,
      extern PSC_TBL_ENTRY KBSODdr3CSTriEntFT3;
      #define PSC_TBL_KB_CS_TRI_FT3 &KBSODdr3CSTriEntFT3,
      #undef PSC_TBL_KB_UDIMM3_S2D_FT3
      #ifndef extern_S2DTblEntUFT3
      #define extern_S2DTblEntUFT3
        extern PSC_TBL_ENTRY S2DTblEntUFT3;
      #endif
      #define PSC_TBL_KB_UDIMM3_S2D_FT3  &S2DTblEntUFT3,
    #endif
    #if OPTION_UDIMMS
      #if (OPTION_MICROSERVER == TRUE)
        extern PSC_TBL_ENTRY KBMaxFreqTblEntMicroSrvU6L;
        #define PSC_TBL_KB_UDIMM3_MAX_FREQ_6L  &KBMaxFreqTblEntMicroSrvU6L,
      #else
        extern PSC_TBL_ENTRY KBMaxFreqTblEntU6L;
        #define PSC_TBL_KB_UDIMM3_MAX_FREQ_6L  &KBMaxFreqTblEntU6L,
      #endif
      extern PSC_TBL_ENTRY KBMaxFreqTblEntU4L;
      #define PSC_TBL_KB_UDIMM3_MAX_FREQ_4L  &KBMaxFreqTblEntU4L,
      #if OPTION_FT3_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY KBDramTermTblEntUFT3;
        #define PSC_TBL_KB_UDIMM3_DRAM_TERM_FT3  &KBDramTermTblEntUFT3,
        extern PSC_TBL_ENTRY KBSAOTblEntU3FT3;
        #define PSC_TBL_KB_UDIMM3_SAO_FT3  &KBSAOTblEntU3FT3,
      #endif
      #undef PSC_TBL_KB_UDIMM3_S2D_FT3
      #ifndef extern_S2DTblEntUFT3
      #define extern_S2DTblEntUFT3
        extern PSC_TBL_ENTRY S2DTblEntUFT3;
      #endif
      #define PSC_TBL_KB_UDIMM3_S2D_FT3  &S2DTblEntUFT3,
    #endif
    #if OPTION_SODIMMS
      #if OPTION_FT3_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY KBSAOTblEntSO3;
        #define PSC_TBL_KB_SODIMM3_SAO  &KBSAOTblEntSO3,
        extern PSC_TBL_ENTRY KBSAOTblEntSoDwnPlusSODIMM3;
        #define PSC_TBL_KB_SODWN_PLUS_SODIMM3_SAO  &KBSAOTblEntSoDwnPlusSODIMM3,
        extern PSC_TBL_ENTRY KBSAOTblEntSoDwn3;
        #define PSC_TBL_KB_SODWN_SAO  &KBSAOTblEntSoDwn3,
        extern PSC_TBL_ENTRY KBDramTermTblEntSO3;
        #define PSC_TBL_KB_SODIMM3_DRAM_TERM  &KBDramTermTblEntSO3,
        extern PSC_TBL_ENTRY KBDramTermTblEntSoDwnPlusSODIMM3;
        #define PSC_TBL_KB_SODWN_PLUS_SODIMM3_DRAM_TERM  &KBDramTermTblEntSoDwnPlusSODIMM3,
        extern PSC_TBL_ENTRY KBDramTermTblEntSoDwn3;
        #define PSC_TBL_KB_SODWN_DRAM_TERM  &KBDramTermTblEntSoDwn3,
        extern PSC_TBL_ENTRY KBMaxFreqTblEntSO6L;
        #define PSC_TBL_KB_SODIMM3_MAX_FREQ_6L  &KBMaxFreqTblEntSO6L,
        extern PSC_TBL_ENTRY KBMaxFreqTblEntSO4L;
        #define PSC_TBL_KB_SODIMM3_MAX_FREQ_4L  &KBMaxFreqTblEntSO4L,
        extern PSC_TBL_ENTRY KBMaxFreqTblEntSoDwnPlusSODIMM6L;
        #define PSC_TBL_KB_SODWN_PLUS_SODIMM3_MAX_FREQ_6L  &KBMaxFreqTblEntSoDwnPlusSODIMM6L,
        extern PSC_TBL_ENTRY KBMaxFreqTblEntSoDwnPlusSODIMM4L;
        #define PSC_TBL_KB_SODWN_PLUS_SODIMM3_MAX_FREQ_4L  &KBMaxFreqTblEntSoDwnPlusSODIMM6L,
        extern PSC_TBL_ENTRY KBMaxFreqTblEntSoDwn;
        #define PSC_TBL_KB_SODWN_MAX_FREQ  &KBMaxFreqTblEntSoDwn,
        #undef PSC_TBL_KB_SODIMM3_S2D_FT3
        #define PSC_TBL_KB_SODIMM3_S2D_FT3
      #endif
    #endif
    extern PSC_TBL_ENTRY KBMR0WrTblEntry;
    extern PSC_TBL_ENTRY KBMR0CLTblEntry;
    extern PSC_TBL_ENTRY KBDdr3CKETriEnt;
    extern PSC_TBL_ENTRY KB1DOdtPatTblEnt;
    extern PSC_TBL_ENTRY KB2DOdtPatTblEnt;

    #ifndef PSC_TBL_KB_SODIMM3_MAX_FREQ_6L
      #define PSC_TBL_KB_SODIMM3_MAX_FREQ_6L
    #endif
    #ifndef PSC_TBL_KB_SODIMM3_MAX_FREQ_4L
      #define PSC_TBL_KB_SODIMM3_MAX_FREQ_4L
    #endif
    #ifndef PSC_TBL_KB_SODWN_PLUS_SODIMM3_MAX_FREQ_6L
      #define PSC_TBL_KB_SODWN_PLUS_SODIMM3_MAX_FREQ_6L
    #endif
    #ifndef PSC_TBL_KB_SODWN_PLUS_SODIMM3_MAX_FREQ_4L
      #define PSC_TBL_KB_SODWN_PLUS_SODIMM3_MAX_FREQ_4L
    #endif
    #ifndef PSC_TBL_KB_SODWN_MAX_FREQ
      #define PSC_TBL_KB_SODWN__MAX_FREQ
    #endif
    #ifndef PSC_TBL_KB_UDIMM3_MAX_FREQ_6L
      #define PSC_TBL_KB_UDIMM3_MAX_FREQ_6L
    #endif
    #ifndef PSC_TBL_KB_UDIMM3_MAX_FREQ_4L
      #define PSC_TBL_KB_UDIMM3_MAX_FREQ_4L
    #endif
    #ifndef PSC_TBL_KB_UDIMM3_DRAM_TERM_FT3
      #define PSC_TBL_KB_UDIMM3_DRAM_TERM_FT3
    #endif
    #ifndef PSC_TBL_KB_SODIMM3_DRAM_TERM
      #define PSC_TBL_KB_SODIMM3_DRAM_TERM
    #endif
    #ifndef PSC_TBL_KB_SODWN_PLUS_SODIMM3_DRAM_TERM
      #define PSC_TBL_KB_SODWN_PLUS_SODIMM3_DRAM_TERM
    #endif
    #ifndef PSC_TBL_KB_SODWN_DRAM_TERM
      #define PSC_TBL_KB_SODWN_DRAM_TERM
    #endif
    #ifndef PSC_TBL_KB_SODIMM3_SAO
      #define PSC_TBL_KB_SODIMM3_SAO
    #endif
    #ifndef PSC_TBL_KB_SODWN_PLUS_SODIMM3_SAO
      #define PSC_TBL_KB_SODWN_PLUS_SODIMM3_SAO
    #endif
    #ifndef PSC_TBL_KB_SODWN_SAO
      #define PSC_TBL_KB_SODWN_SAO
    #endif
    #ifndef PSC_TBL_KB_UDIMM3_SAO_FT3
      #define PSC_TBL_KB_UDIMM3_SAO_FT3
    #endif
    #ifndef PSC_TBL_KB_CLK_DIS_FT3
      #define PSC_TBL_KB_CLK_DIS_FT3
    #endif
    #ifndef PSC_TBL_KB_ODT_TRI_FT3
      #define PSC_TBL_KB_ODT_TRI_FT3
    #endif
    #ifndef PSC_TBL_KB_CS_TRI_FT3
      #define PSC_TBL_KB_CS_TRI_FT3
    #endif

    PSC_TBL_ENTRY* CONST memPSCTblMaxFreqArrayKB[] = {
      // 4 layer tables block
      PSC_TBL_KB_SODIMM3_MAX_FREQ_4L
      PSC_TBL_KB_SODWN_PLUS_SODIMM3_MAX_FREQ_4L
      PSC_TBL_KB_SODWN_MAX_FREQ
      PSC_TBL_KB_UDIMM3_MAX_FREQ_4L
      PSC_TBL_END,
      // 6 layer tables block
      PSC_TBL_KB_SODIMM3_MAX_FREQ_6L
      PSC_TBL_KB_SODWN_PLUS_SODIMM3_MAX_FREQ_6L
      PSC_TBL_KB_SODWN_MAX_FREQ
      PSC_TBL_KB_UDIMM3_MAX_FREQ_6L
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblDramTermArrayKB[] = {
      PSC_TBL_KB_UDIMM3_DRAM_TERM_FT3
      PSC_TBL_KB_SODIMM3_DRAM_TERM
      PSC_TBL_KB_SODWN_PLUS_SODIMM3_DRAM_TERM
      PSC_TBL_KB_SODWN_DRAM_TERM
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblODTPatArrayKB[] = {
      &KB1DOdtPatTblEnt,
      &KB2DOdtPatTblEnt,
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblSAOArrayKB[] = {
      PSC_TBL_KB_SODIMM3_SAO
      PSC_TBL_KB_SODWN_PLUS_SODIMM3_SAO
      PSC_TBL_KB_SODWN_SAO
      PSC_TBL_KB_UDIMM3_SAO_FT3
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblMR0WRArrayKB[] = {
      &KBMR0WrTblEntry,
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST  memPSCTblMR0CLArrayKB[] = {
      &KBMR0CLTblEntry,
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblGenArrayKB[] = {
      &KBDdr3CKETriEnt,
      PSC_TBL_KB_CLK_DIS_FT3
      PSC_TBL_KB_ODT_TRI_FT3
      PSC_TBL_KB_CS_TRI_FT3
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* CONST memPSCTblS2DArrayKB[] = {
      PSC_TBL_KB_UDIMM3_S2D_FT3
      PSC_TBL_KB_SODIMM3_S2D_FT3
      PSC_TBL_END
    };

    CONST MEM_PSC_TABLE_BLOCK memPSCTblBlockKB = {
      (PSC_TBL_ENTRY **)&memPSCTblMaxFreqArrayKB,
      (PSC_TBL_ENTRY **)&memPSCTblDramTermArrayKB,
      (PSC_TBL_ENTRY **)&memPSCTblODTPatArrayKB,
      (PSC_TBL_ENTRY **)&memPSCTblSAOArrayKB,
      (PSC_TBL_ENTRY **)&memPSCTblMR0WRArrayKB,
      (PSC_TBL_ENTRY **)&memPSCTblMR0CLArrayKB,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      (PSC_TBL_ENTRY **)&memPSCTblGenArrayKB,
      (PSC_TBL_ENTRY **)&memPSCTblS2DArrayKB,
      NULL,
      NULL
    };

    extern MEM_PSC_FLOW MemPGetMaxFreqSupported;
    extern MEM_PSC_FLOW MemPGetRttNomWr;
    extern MEM_PSC_FLOW MemPGetODTPattern;
    extern MEM_PSC_FLOW MemPGetSAO;
    extern MEM_PSC_FLOW MemPGetMR0WrCL;
    extern MEM_PSC_FLOW MemPGetS2D;

    CONST MEM_PSC_FLOW_BLOCK memPlatSpecFlowKB = {
      &memPSCTblBlockKB,
      MemPGetMaxFreqSupported,
      MemPGetRttNomWr,
      MemPGetODTPattern,
      MemPGetSAO,
      MemPGetMR0WrCL,
      MEM_PSC_FLOW_DEFTRUE,
      MEM_PSC_FLOW_DEFTRUE,
      MEM_PSC_FLOW_DEFTRUE,
      MEM_PSC_FLOW_DEFTRUE,
      MEM_PSC_FLOW_DEFTRUE,
      MemPGetS2D,
      MEM_PSC_FLOW_DEFTRUE
    };
    #define MEM_PSC_FLOW_BLOCK_KB &memPlatSpecFlowKB,
  #else
    #define MEM_PSC_FLOW_BLOCK_KB
  #endif


  CONST MEM_PSC_FLOW_BLOCK* CONST memPlatSpecFlowArray[] = {
    MEM_PSC_FLOW_BLOCK_TN
    MEM_PSC_FLOW_BLOCK_KB
    MEM_PSC_FLOW_BLOCK_END
  };

  /*---------------------------------------------------------------------------------------------------
  *
  *  LRDIMM CONTROL
  *
  *---------------------------------------------------------------------------------------------------
  */
  #if (OPTION_LRDIMMS == TRUE)
    #if (OPTION_MEMCTLR_BK == TRUE)
      #define MEM_TECH_FEATURE_LRDIMM_INIT  &MemTLrdimmConstructor3
    #else
      #define MEM_TECH_FEATURE_LRDIMM_INIT    MemTFeatDef
    #endif
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
  #if (OPTION_MEMCTLR_TN == TRUE)
    #undef MEM_NB_SUPPORT_TN
    #define MEM_NB_SUPPORT_TN { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN, MEM_IDENDIMM_TN },
  #endif
  #if (OPTION_MEMCTLR_KB == TRUE)
    #undef MEM_NB_SUPPORT_KB
    #define MEM_NB_SUPPORT_KB { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_KB, MEM_IDENDIMM_KB },
  #endif
  /*---------------------------------------------------------------------------------------------------
   * DEFAULT Technology Training
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #if OPTION_DDR2
    CONST MEM_TECH_FEAT_BLOCK  memTechTrainingFeatDDR2 = {
      0
    };
    CONST MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR2[] = {
      { 0 }
    };
  #endif
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
  MEM_NB_SUPPORT_TN
  MEM_NB_SUPPORT_KB
  MEM_NB_SUPPORT_END
};

CONST UINT8 SizeOfNBInstalledTable = sizeof (memNBInstalled) / sizeof (memNBInstalled[0]);


#endif  // _OPTION_MEMORY_INSTALL_H_
