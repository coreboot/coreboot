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
 * @e \$Revision: 64574 $   @e \$Date: 2012-01-25 01:01:51 -0600 (Wed, 25 Jan 2012) $
 */
/*****************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

#if (OPTION_MEMCTLR_DR == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockDr;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DR MemS3ResumeConstructNBBlockDr
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DR MemFS3DefConstructorRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DR MemFS3DefConstructorRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorDr;
    #define MEM_IDENDIMM_DR MemNIdentifyDimmConstructorDr
  #else
    #define MEM_IDENDIMM_DR MemNIdentifyDimmConstructorRetDef
  #endif
#endif

#if (OPTION_MEMCTLR_DA == TRUE  || OPTION_MEMCTLR_Ni == TRUE || OPTION_MEMCTLR_RB == TRUE || OPTION_MEMCTLR_PH == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      #if (OPTION_MEMCTLR_Ni == TRUE)
        extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockNi;
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_Ni MemS3ResumeConstructNBBlockNi
      #else
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_Ni MemFS3DefConstructorRet
      #endif
      #if (OPTION_MEMCTLR_DA == TRUE)
        extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockDA;
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA MemS3ResumeConstructNBBlockDA
      #else
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA MemFS3DefConstructorRet
      #endif
      #if (OPTION_MEMCTLR_PH == TRUE)
        extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockPh;
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_PH MemS3ResumeConstructNBBlockPh
      #else
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_PH MemFS3DefConstructorRet
      #endif
      #if (OPTION_MEMCTLR_RB == TRUE)
        extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockRb;
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_RB MemS3ResumeConstructNBBlockRb
      #else
        #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_RB MemFS3DefConstructorRet
      #endif
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA MemFS3DefConstructorRet
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_Ni MemFS3DefConstructorRet
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_RB MemFS3DefConstructorRet
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_PH MemFS3DefConstructorRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorDA;
    #define MEM_IDENDIMM_DA MemNIdentifyDimmConstructorDA
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorRb;
    #define MEM_IDENDIMM_RB MemNIdentifyDimmConstructorRb
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorPh;
    #define MEM_IDENDIMM_PH MemNIdentifyDimmConstructorPh
  #else
    #define MEM_IDENDIMM_DA MemNIdentifyDimmConstructorRetDef
    #define MEM_IDENDIMM_RB MemNIdentifyDimmConstructorRetDef
    #define MEM_IDENDIMM_PH MemNIdentifyDimmConstructorRetDef
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

#if (OPTION_MEMCTLR_TN == TRUE)
  #if ((AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE))
    #if (OPTION_S3_MEM_SUPPORT == TRUE)
      extern MEM_RESUME_CONSTRUCTOR MemS3ResumeConstructNBBlockTN;
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN MemS3ResumeConstructNBBlockTN
    #else
      #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN MemFS3DefConstructorRet
    #endif
  #else
    #define MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN MemFS3DefConstructorRet
  #endif
  #if (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
    extern MEM_IDENDIMM_CONSTRUCTOR MemNIdentifyDimmConstructorTN;
    #define MEM_IDENDIMM_TN MemNIdentifyDimmConstructorTN
  #else
    #define MEM_IDENDIMM_TN MemNIdentifyDimmConstructorRetDef
  #endif
#endif




/*----------------------------------------------------------------------------------
 * NORTHBRIDGE BLOCK CONSTRUCTOR AND INITIALIZER FUNCTION DEFAULT ASSIGNMENTS
 *
 *----------------------------------------------------------------------------------
*/
#define MEM_NB_SUPPORT_DR
#define MEM_NB_SUPPORT_RB
#define MEM_NB_SUPPORT_DA
#define MEM_NB_SUPPORT_Ni
#define MEM_NB_SUPPORT_PH
#define MEM_NB_SUPPORT_HY
#define MEM_NB_SUPPORT_LN
#define MEM_NB_SUPPORT_OR
#define MEM_NB_SUPPORT_C32
#define MEM_NB_SUPPORT_ON
#define MEM_NB_SUPPORT_TN
#define MEM_NB_SUPPORT_END { MEM_NB_SUPPORT_STRUCT_VERSION, 0, 0, 0, 0, 0 }

#if (AGESA_ENTRY_INIT_POST == TRUE)
  /*----------------------------------------------------------------------------------
   * FLOW CONTROL FUNCTION
   *
   *  This section selects the function that controls the memory initialization sequence
   *  based upon the number of processor families that the BIOS will support.
   */
  extern MEM_FLOW_CFG MemMFlowDef;

  #if (OPTION_MEMCTLR_DR == TRUE)
    extern MEM_FLOW_CFG MemMFlowDr;
    #define MEM_MAIN_FLOW_CONTROL_PTR_Dr MemMFlowDr,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_Dr MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_DA == TRUE)
    extern MEM_FLOW_CFG MemMFlowDA;
    #define MEM_MAIN_FLOW_CONTROL_PTR_DA MemMFlowDA,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_DA MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_HY == TRUE)
    extern MEM_FLOW_CFG MemMFlowHy;
    #define MEM_MAIN_FLOW_CONTROL_PTR_Hy MemMFlowHy,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_Hy MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_OR == TRUE)
    extern MEM_FLOW_CFG MemMFlowOr;
    #define MEM_MAIN_FLOW_CONTROL_PTR_OR MemMFlowOr,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_OR MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_LN == TRUE)
    extern MEM_FLOW_CFG MemMFlowLN;
    #define MEM_MAIN_FLOW_CONTROL_PTR_LN MemMFlowLN,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_LN MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_C32 == TRUE)
    extern MEM_FLOW_CFG MemMFlowC32;
    #define MEM_MAIN_FLOW_CONTROL_PTR_C32 MemMFlowC32,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_C32 MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_ON == TRUE)
    extern MEM_FLOW_CFG MemMFlowON;
    #define MEM_MAIN_FLOW_CONTROL_PTR_ON MemMFlowON,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_ON MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_Ni == TRUE)
    extern MEM_FLOW_CFG MemMFlowDA;
    #define MEM_MAIN_FLOW_CONTROL_PTR_Ni MemMFlowDA,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_Ni MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_RB == TRUE)
    extern MEM_FLOW_CFG MemMFlowRb;
    #define MEM_MAIN_FLOW_CONTROL_PTR_RB MemMFlowRb,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_RB MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_PH == TRUE)
    extern MEM_FLOW_CFG MemMFlowPh;
    #define MEM_MAIN_FLOW_CONTROL_PTR_PH MemMFlowPh,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_PH MemMFlowDef,
  #endif
  #if (OPTION_MEMCTLR_TN == TRUE)
    extern MEM_FLOW_CFG MemMFlowTN;
    #define MEM_MAIN_FLOW_CONTROL_PTR_TN MemMFlowTN,
  #else
    #define MEM_MAIN_FLOW_CONTROL_PTR_TN MemMFlowDef,
  #endif

  MEM_FLOW_CFG* CONST memFlowControlInstalled[] = {
    MEM_MAIN_FLOW_CONTROL_PTR_Dr
    MEM_MAIN_FLOW_CONTROL_PTR_DA
    MEM_MAIN_FLOW_CONTROL_PTR_RB
    MEM_MAIN_FLOW_CONTROL_PTR_PH
    MEM_MAIN_FLOW_CONTROL_PTR_Hy
    MEM_MAIN_FLOW_CONTROL_PTR_OR
    MEM_MAIN_FLOW_CONTROL_PTR_LN
    MEM_MAIN_FLOW_CONTROL_PTR_C32
    MEM_MAIN_FLOW_CONTROL_PTR_ON
    MemMFlowDef,
    MEM_MAIN_FLOW_CONTROL_PTR_Ni
    MEM_MAIN_FLOW_CONTROL_PTR_TN
    MemMFlowDef,
    MemMFlowDef,
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

  #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
    extern OPTION_MEM_FEATURE_NB MemNInitDqsTrainRcvrEnHwNb;
  #endif
  extern OPTION_MEM_FEATURE_NB MemFStandardTraining;

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

    #undef MEM_TECH_FEATURE_CPG
    #define MEM_TECH_FEATURE_CPG    MemFDefRet

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
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
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
      MemFDefRet,
      MEM_TECH_FEATURE_CPG,
      MEM_TECH_FEATURE_HWRXEN
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
  #if (OPTION_MEMCTLR_DA == TRUE || OPTION_MEMCTLR_Ni == TRUE || OPTION_MEMCTLR_RB == TRUE || OPTION_MEMCTLR_PH == TRUE)
    #if OPTION_DDR2
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_HW_DRAMINIT
    #endif
    #if OPTION_DDR3
      #undef MEM_TECH_FEATURE_DRAMINIT
      #define MEM_TECH_FEATURE_DRAMINIT MEM_TECH_FEATURE_SW_DRAMINIT
    #endif

    #undef MEM_TECH_FEATURE_CPG
    #define MEM_TECH_FEATURE_CPG    MemFDefRet

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
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    #if (OPTION_MEMCTLR_Ni == TRUE)
      MEM_FEAT_BLOCK_NB  MemFeatBlockNi = {
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
        MemFDefRet,
        MEM_TECH_FEATURE_CPG,
        MEM_TECH_FEATURE_HWRXEN
      };

      #undef MEM_NB_SUPPORT_Ni
      extern MEM_NB_CONSTRUCTOR MemConstructNBBlockNi;
      extern MEM_INITIALIZER MemNInitDefaultsNi;

      #define MEM_NB_SUPPORT_Ni { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockNi, MemNInitDefaultsNi, &MemFeatBlockNi, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_Ni, MEM_IDENDIMM_DA },
    #endif

    #if (OPTION_MEMCTLR_PH == TRUE)
      MEM_FEAT_BLOCK_NB  MemFeatBlockPh = {
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
        MemFDefRet,
        MEM_TECH_FEATURE_CPG,
        MEM_TECH_FEATURE_HWRXEN
      };

      #undef MEM_NB_SUPPORT_PH
      extern MEM_NB_CONSTRUCTOR MemConstructNBBlockPh;
      extern MEM_INITIALIZER MemNInitDefaultsPh;

      #define MEM_NB_SUPPORT_PH { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockPh, MemNInitDefaultsPh, &MemFeatBlockPh, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_PH, MEM_IDENDIMM_PH },
    #endif

    #if (OPTION_MEMCTLR_RB == TRUE)
      MEM_FEAT_BLOCK_NB  MemFeatBlockRb = {
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
        MemFDefRet,
        MEM_TECH_FEATURE_CPG,
        MEM_TECH_FEATURE_HWRXEN
      };

      #undef MEM_NB_SUPPORT_RB
      extern MEM_NB_CONSTRUCTOR MemConstructNBBlockRb;
      extern MEM_INITIALIZER MemNInitDefaultsRb;

      #define MEM_NB_SUPPORT_RB { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockRb, MemNInitDefaultsRb, &MemFeatBlockRb, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_RB, MEM_IDENDIMM_RB },
    #endif

    #if (OPTION_MEMCTLR_DA == TRUE)
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
        MemFDefRet,
        MEM_TECH_FEATURE_CPG,
        MEM_TECH_FEATURE_HWRXEN
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

    #undef MEM_TECH_FEATURE_CPG
    #define MEM_TECH_FEATURE_CPG    MemFDefRet

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
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
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
      MemFDefRet,
      MEM_TECH_FEATURE_CPG,
      MEM_TECH_FEATURE_HWRXEN
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

    #if (OPTION_EARLY_SAMPLES == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitEarlySampleSupportLN;
      #define MEM_EARLY_SAMPLE_SUPPORT    MemNInitEarlySampleSupportLN
    #else
      #define MEM_EARLY_SAMPLE_SUPPORT    MemFDefRet
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
      #undef MEM_TECH_FEATURE_HWRXEN
      #define MEM_TECH_FEATURE_HWRXEN    MemNInitDqsTrainRcvrEnHwNb
    #else
      extern OPTION_MEM_FEATURE_NB MemNDisableDqsTrainRcvrEnHwNb;
      #undef MEM_TECH_FEATURE_HWRXEN
      #define MEM_TECH_FEATURE_HWRXEN    MemNDisableDqsTrainRcvrEnHwNb
    #endif

    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
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
      MEM_EARLY_SAMPLE_SUPPORT,
      MEM_TECH_FEATURE_CPG,
      MEM_TECH_FEATURE_HWRXEN
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
      #undef MEM_TECH_FEATURE_CPG
      #define MEM_TECH_FEATURE_CPG    MemNInitCPGClientNb
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
    extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
    #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    #define MEM_FEATURE_TRAINING  MemFStandardTraining

    #if (OPTION_EARLY_SAMPLES == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitEarlySampleSupportON;
      #define MEM_EARLY_SAMPLE_SUPPORT    MemNInitEarlySampleSupportON
    #else
      #define MEM_EARLY_SAMPLE_SUPPORT    MemFDefRet
    #endif

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
   * OROCHI FEATURE BLOCK
   *---------------------------------------------------------------------------------------------------
   */
  #if (OPTION_MEMCTLR_OR == TRUE)
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

    #if (OPTION_G34_SOCKET_SUPPORT || OPTION_C32_SOCKET_SUPPORT)
      #undef MEM_FEATURE_REGION_INTERLEAVE
      #define MEM_FEATURE_REGION_INTERLEAVE MemFDefRet
    #endif

    #if (OPTION_EARLY_SAMPLES == TRUE)
      extern OPTION_MEM_FEATURE_NB MemNInitEarlySampleSupportOr;
      #define MEM_EARLY_SAMPLE_SUPPORT    MemNInitEarlySampleSupportOr
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

    MEM_FEAT_BLOCK_NB  MemFeatBlockOr = {
      MEM_FEAT_BLOCK_NB_STRUCT_VERSION,
      MEM_FEATURE_ONLINE_SPARE,
      MEM_FEATURE_BANK_INTERLEAVE,
      MEM_FEATURE_UNDO_BANK_INTERLEAVE,
      MEM_FEATURE_NODE_INTERLEAVE_CHECK,
      MEM_FEATURE_NODE_INTERLEAVE,
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
      MEM_TECH_FEATURE_HWRXEN
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

    #undef MEM_TECH_FEATURE_CPG
    #define MEM_TECH_FEATURE_CPG    MemFDefRet

    #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
      #undef MEM_TECH_FEATURE_HWRXEN
      #define MEM_TECH_FEATURE_HWRXEN    MemNInitDqsTrainRcvrEnHwNb
    #else
      extern OPTION_MEM_FEATURE_NB MemNDisableDqsTrainRcvrEnHwNb;
      #define MEM_TECH_FEATURE_HWRXEN    MemNDisableDqsTrainRcvrEnHwNb
    #endif

    #undef MEM_MAIN_FEATURE_TRAINING
    #undef MEM_FEATURE_TRAINING
    #if (OPTION_MEMCTLR_OR == TRUE)
      #if (OPTION_RDDQS_2D_TRAINING == TRUE)
        extern OPTION_MEM_FEATURE_MAIN MemMStandardTrainingUsingAdjacentDies;
        #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTrainingUsingAdjacentDies
      #else
        extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
        #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
      #endif
    #else
      extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
      #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
    #endif
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
      MemFDefRet,
      MEM_TECH_FEATURE_CPG,
      MEM_TECH_FEATURE_HWRXEN
    };

    #undef MEM_NB_SUPPORT_C32
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockC32;
    extern MEM_INITIALIZER MemNInitDefaultsC32;
    #define MEM_NB_SUPPORT_C32 { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockC32, MemNInitDefaultsC32, &MemFeatBlockC32, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_C32, MEM_IDENDIMM_C32 },
  #endif // OPTION_MEMCTRL_C32

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

      #define MEM_EARLY_SAMPLE_SUPPORT    MemFDefRet

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
      //extern OPTION_MEM_FEATURE_MAIN MemMStandardTraining;
      #define MEM_MAIN_FEATURE_TRAINING  MemMStandardTraining
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
      MEM_TECH_FEATURE_HWRXEN
    };

    #undef MEM_NB_SUPPORT_TN
    extern MEM_NB_CONSTRUCTOR MemConstructNBBlockTN;
    extern MEM_INITIALIZER MemNInitDefaultsTN;
    #define MEM_NB_SUPPORT_TN { MEM_NB_SUPPORT_STRUCT_VERSION, MemConstructNBBlockTN, MemNInitDefaultsTN, &MemFeatBlockTN, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN, MEM_IDENDIMM_TN },
  #endif // OPTION_MEMCTRL_TN



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
    MemMDefRet,
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
    extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceDr;
    #if OPTION_DDR2
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2    MemTDqsTrainRcvrEnHwPass2
        #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2  MemTTrainDQSEdgeDetect
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
        #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2    MemTTrainDQSEdgeDetect
        #else
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2  MemTFeatDef
        #endif
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
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR2Dr = {
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
        TECH_TRAIN_MAX_RD_LAT_DDR2,
        TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2,
        NULL
      };
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming2Nb;
      #define NB_TRAIN_FLOW_DDR2    MemNDQSTiming2Nb
      extern OPTION_MEM_FEATURE_NB memNSequenceDDR2ServerNb;
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DR { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, memNSequenceDDR2ServerNb, memNEnableTrainSequenceDr, &memTechTrainingFeatSequenceDDR2Dr },
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
      #define NB_TRAIN_FLOW_DDR2    (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DR  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
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
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTDqsTrainRcvrEnHwPass2
        #if (OPTION_HW_DQS_REC_EN_SEED_TRAINING == TRUE)
          #undef TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTRdPosWithRxEnDlySeeds3
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
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
      extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceDr;
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3Dr = {
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
        MemTFeatDef
      };
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_DR { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, memNSequenceDDR3Nb, memNEnableTrainSequenceDr, &memTechTrainingFeatSequenceDDR3Dr },
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
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_DR  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
  #else
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DR  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_DR  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
  #endif

  #if (OPTION_MEMCTLR_DA || OPTION_MEMCTLR_Ni || OPTION_MEMCTLR_PH || OPTION_MEMCTLR_RB)
    #if OPTION_DDR2
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2    MemTDqsTrainRcvrEnHwPass2
        #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2    MemTTrainDQSEdgeDetect
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
        #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2    MemTTrainDQSEdgeDetect
        #else
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2    MemTFeatDef
        #endif
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
      MEM_TECH_FEAT_BLOCK  omi1867  = {
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
        TECH_TRAIN_MAX_RD_LAT_DDR2,
        TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2,
        NULL
      };
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR2PH = {
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
        TECH_TRAIN_MAX_RD_LAT_DDR2,
        TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2,
        NULL
      };
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR2Rb = {
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
        TECH_TRAIN_MAX_RD_LAT_DDR2,
        TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2,
        NULL
      };
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR2Ni = {
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
        TECH_TRAIN_MAX_RD_LAT_DDR2,
        TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2,
        NULL
      };
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming2Nb;
      #define NB_TRAIN_FLOW_DDR2    MemNDQSTiming2Nb
      extern OPTION_MEM_FEATURE_NB memNSequenceDDR2ServerNb;
      #if (OPTION_MEMCTLR_DA)
        extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceDA
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DA { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, memNSequenceDDR2ServerNb, memNEnableTrainSequenceDA, &omi1867  },
      #else
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DA  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #endif
      #if (OPTION_MEMCTLR_PH)
        extern OPTION_MEM_FEATURE_NB memNEnableTrainSequencePh
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_PH { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, memNSequenceDDR2ServerNb, memNEnableTrainSequencePh, &memTechTrainingFeatSequenceDDR2PH },
      #else
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_PH  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #endif
      #if (OPTION_MEMCTLR_RB)
        extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceRb
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_RB { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, memNSequenceDDR2ServerNb, memNEnableTrainSequenceRb, &memTechTrainingFeatSequenceDDR2Rb },
      #else
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_RB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #endif

      #if (OPTION_MEMCTLR_Ni)
        extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceNi
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_Ni { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR2ServerNb, memNEnableTrainSequenceNi, &memTechTrainingFeatSequenceDDR2Ni },
      #else
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DA  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #endif
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
      #define NB_TRAIN_FLOW_DDR2    (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DA  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_Ni  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_PH  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_RB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
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
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTFeatDef
        #ifdef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
          #undef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
        #endif
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTFeatDef
        #if (OPTION_HW_DQS_REC_EN_SEED_TRAINING == TRUE)
          #undef TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
        #else
          #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE || OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
            #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
          #else
            #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3 MemTFeatDef
          #endif
        #endif
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3 MemTFeatDef
        #if (OPTION_OPT_SW_RD_WR_POS_TRAINING == TRUE  || OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
          #undef TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
        #else
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3 MemTFeatDef
        #endif
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
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3DA = {
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
        NULL
      };
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3Ph = {
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
        NULL
      };
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3Rb = {
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
        NULL
      };
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3Ni = {
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

      };
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
      #if (OPTION_MEMCTLR_DA)
        extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceDA;
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_DA { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, memNSequenceDDR3Nb, memNEnableTrainSequenceDA, &memTechTrainingFeatSequenceDDR3DA },
      #else
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_DA  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #endif
      #if (OPTION_MEMCTLR_PH)
        extern OPTION_MEM_FEATURE_NB memNEnableTrainSequencePh;
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_PH { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, memNSequenceDDR3Nb, memNEnableTrainSequencePh, &memTechTrainingFeatSequenceDDR3Ph },
      #else
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_PH  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #endif
      #if (OPTION_MEMCTLR_RB)
        extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceRb;
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_RB { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, memNSequenceDDR3Nb, memNEnableTrainSequenceRb, &memTechTrainingFeatSequenceDDR3Rb },
      #else
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_RB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #endif
      #if (OPTION_MEMCTLR_Ni)
        extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceNi;
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_Ni {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR3Nb, memNEnableTrainSequenceNi, &memTechTrainingFeatSequenceDDR3Ni },
      #else
        #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_Ni  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #endif
    #else
      #undef TECH_TRAIN_ENTER_HW_TRN_DDR3
      #define TECH_TRAIN_ENTER_HW_TRN_DDR3 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR3 MemTFeatDef
      #undef TECH_TRAIN_EXIT_HW_TRN_DDR3
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
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_Ni  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_DA  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_PH  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_RB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
  #else
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_Ni  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_Ni  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DA  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_DA  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_PH  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_PH  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_RB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_RB  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
  #endif

  #if OPTION_MEMCTLR_HY
    extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceHy;
    #if OPTION_DDR2
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2    MemTDqsTrainRcvrEnHwPass2
        #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2    MemTTrainDQSEdgeDetect
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
        #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2    MemTTrainDQSEdgeDetect
        #else
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2    MemTFeatDef
        #endif
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
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR2Hy = {
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
        TECH_TRAIN_MAX_RD_LAT_DDR2,
        TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2,
        NULL
      };
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming2Nb;
      #define NB_TRAIN_FLOW_DDR2    MemNDQSTiming2Nb
      extern OPTION_MEM_FEATURE_NB memNSequenceDDR2ServerNb;
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_HY {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR2ServerNb, memNEnableTrainSequenceHy, &memTechTrainingFeatSequenceDDR2Hy },
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
      #define NB_TRAIN_FLOW_DDR2    (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_HY  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
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
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTFeatDef
        #ifdef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
          #undef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
        #endif
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTFeatDef
        #if (OPTION_HW_DQS_REC_EN_SEED_TRAINING == TRUE)
          #undef TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
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
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3Hy = {
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
        NULL
      };
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_HY {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR3Nb, memNEnableTrainSequenceHy, &memTechTrainingFeatSequenceDDR3Hy },
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
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_HY  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
  #else
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_HY  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_HY  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
  #endif

  #if OPTION_MEMCTLR_C32
    extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceC32;
    #if OPTION_DDR2
      #define TECH_TRAIN_ENTER_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_EXIT_HW_TRN_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P1_DDR2 MemTFeatDef
      #define TECH_TRAIN_HW_WL_P2_DDR2 MemTFeatDef
      #define TECH_TRAIN_SW_WL_DDR2 MemTFeatDef
      #if (OPTION_HW_DQS_REC_EN_TRAINING == TRUE)
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2    MemTDqsTrainRcvrEnHwPass1
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2    MemTDqsTrainRcvrEnHwPass2
        #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2    MemTTrainDQSEdgeDetect
      #else
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR2 MemTFeatDef
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR2 MemTFeatDef
        #if (OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == TRUE)
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
        #else
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3 MemTFeatDef
        #endif
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
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR2C32 = {
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
        TECH_TRAIN_MAX_RD_LAT_DDR2,
        TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR2,
        NULL
      };
      extern OPTION_MEM_FEATURE_NB MemNDQSTiming2Nb;
      #define NB_TRAIN_FLOW_DDR2    MemNDQSTiming2Nb
      extern OPTION_MEM_FEATURE_NB memNSequenceDDR2ServerNb;
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_C32 {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR2ServerNb, memNEnableTrainSequenceC32, &memTechTrainingFeatSequenceDDR2C32 },
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
      #define NB_TRAIN_FLOW_DDR2    (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_C32  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
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
        #define TECH_TRAIN_HW_DQS_REC_EN_P1_DDR3    MemTFeatDef
        #ifdef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
          #undef TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3
        #endif
        #define TECH_TRAIN_HW_DQS_REC_EN_P2_DDR3    MemTFeatDef
        #if (OPTION_HW_DQS_REC_EN_SEED_TRAINING == TRUE)
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTTrainDQSEdgeDetect
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
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3C32 = {
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
        NULL
      };
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_C32 {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR3Nb, memNEnableTrainSequenceC32, &memTechTrainingFeatSequenceDDR3C32 },
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
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_C32  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
  #else
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_C32  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_C32 { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
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
    #define NB_TRAIN_FLOW_DDR2    (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_LN  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceLN;
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
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTRdPosWithRxEnDlySeeds3
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
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3LN = {
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
        NULL
      };
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_LN {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, memNSequenceDDR3Nb, memNEnableTrainSequenceLN, &memTechTrainingFeatSequenceDDR3LN },
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
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_LN  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
  #else
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_LN  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_LN  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
  #endif


  #if OPTION_MEMCTLR_OR
    extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceOr;
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
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_OR  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
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
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTRdPosWithRxEnDlySeeds3
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
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3OR = {
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
        NULL
      };
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_OR {MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION,memNSequenceDDR3Nb, memNEnableTrainSequenceOr, &memTechTrainingFeatSequenceDDR3OR },
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
      #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_OR  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #endif
  #else
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_OR  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_OR  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
  #endif


  #if OPTION_MEMCTLR_ON
    extern OPTION_MEM_FEATURE_NB memNEnableTrainSequenceON;
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
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_ON  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
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
        #if (OPTION_HW_DQS_REC_EN_SEED_TRAINING == TRUE)
          #undef TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3
          #define TECH_TRAIN_HW_DQS_REC_EN_SEED_DDR3    MemTRdPosWithRxEnDlySeeds3
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
      MEM_TECH_FEAT_BLOCK  memTechTrainingFeatSequenceDDR3ON = {
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
        NULL
      };
      #define NB_TRAIN_FLOW_DDR3    MemNDQSTiming3Nb
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
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_ON  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
    #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_ON  { MEM_TECH_TRAIN_SEQUENCE_STRUCT_VERSION, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue, (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefFalse, 0 },
  #endif

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



  #define MEM_TECH_ENABLE_TRAINING_SEQUENCE_END { MEM_NB_SUPPORT_STRUCT_VERSION, 0, 0, 0 }
  MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR2[] = {
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DR
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_DA
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_HY
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_LN
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_C32
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_ON
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_Ni
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_OR
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_PH
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_RB
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR2_TN
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_END
  };

  CONST MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR3[] = {
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_DR
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_DA
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_HY
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_LN
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_C32
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_ON
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_Ni
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_OR
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_PH
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_RB
    MEM_TECH_ENABLE_TRAINING_SEQUENCE_DDR3_TN
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
  #if  OPTION_MEMCTLR_HY
    #if OPTION_UDIMMS
      #if OPTION_DDR2
        #define PLAT_SP_HY_FF_UDIMM2    MemPConstructPsUDef,
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
        #define PLAT_SP_HY_FF_RDIMM2    MemPConstructPsUDef,
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
        #define PLAT_SP_HY_FF_SDIMM2    MemPConstructPsUDef,
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
        extern MEM_PLAT_SPEC_CFG MemPConstructPsUDr2;
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
        extern MEM_PLAT_SPEC_CFG MemPConstructPsRDr2;
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

  #if (OPTION_MEMCTLR_DA == TRUE)
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

  #if (OPTION_MEMCTLR_Ni == TRUE)
    #define PLAT_SP_NI_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_NI_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_NI_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_NI_FF_SDIMM3    MemPConstructPsSNi3,
    #define PLAT_SP_NI_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_NI_FF_UDIMM3    MemPConstructPsUNi3,
  #else
    #define PLAT_SP_NI_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_NI_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_NI_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_NI_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_NI_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_NI_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledNi[MAX_FF_TYPES] = {
    PLAT_SP_NI_FF_UDIMM2
    PLAT_SP_NI_FF_RDIMM2
    PLAT_SP_NI_FF_SDIMM2
    PLAT_SP_NI_FF_UDIMM3
    PLAT_SP_NI_FF_RDIMM3
    PLAT_SP_NI_FF_SDIMM3
  };

  #if (OPTION_MEMCTLR_PH == TRUE)
    #define PLAT_SP_PH_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_PH_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_PH_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_PH_FF_SDIMM3    MemPConstructPsSPh3,
    #define PLAT_SP_PH_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_PH_FF_UDIMM3    MemPConstructPsUPh3,
  #else
    #define PLAT_SP_PH_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_PH_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_PH_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_PH_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_PH_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_PH_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledPh[MAX_FF_TYPES] = {
    PLAT_SP_PH_FF_UDIMM2
    PLAT_SP_PH_FF_RDIMM2
    PLAT_SP_PH_FF_SDIMM2
    PLAT_SP_PH_FF_UDIMM3
    PLAT_SP_PH_FF_RDIMM3
    PLAT_SP_PH_FF_SDIMM3
  };

  #if (OPTION_MEMCTLR_RB == TRUE)
    #define PLAT_SP_RB_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_RB_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_RB_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_RB_FF_SDIMM3    MemPConstructPsSRb3,
    #define PLAT_SP_RB_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_RB_FF_UDIMM3    MemPConstructPsURb3,
  #else
    #define PLAT_SP_RB_FF_SDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_RB_FF_RDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_RB_FF_UDIMM2    MemPConstructPsUDef,
    #define PLAT_SP_RB_FF_SDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_RB_FF_RDIMM3    MemPConstructPsUDef,
    #define PLAT_SP_RB_FF_UDIMM3    MemPConstructPsUDef,
  #endif
  MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledRb[MAX_FF_TYPES] = {
    PLAT_SP_RB_FF_UDIMM2
    PLAT_SP_RB_FF_RDIMM2
    PLAT_SP_RB_FF_SDIMM2
    PLAT_SP_RB_FF_UDIMM3
    PLAT_SP_RB_FF_RDIMM3
    PLAT_SP_RB_FF_SDIMM3
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
        #define PLAT_SP_C32_FF_UDIMM2    MemPConstructPsUDef,
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
        #define PLAT_SP_C32_FF_RDIMM2    MemPConstructPsUDef,
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

  #if (OPTION_MEMCTLR_DA == TRUE || OPTION_MEMCTLR_Ni == TRUE || OPTION_MEMCTLR_RB == TRUE || OPTION_MEMCTLR_PH == TRUE)
    #if OPTION_MEMCTLR_Ni
      #define PSC_NI_UDIMM_DDR2
      #define PSC_NI_UDIMM_DDR3     MemAGetPsCfgUNi3,
      #define PSC_NI_RDIMM_DDR2
      #define PSC_NI_RDIMM_DDR3
      #define PSC_NI_SODIMM_DDR2
      #define PSC_NI_SODIMM_DDR3    MemAGetPsCfgSNi3,
    #endif
    #if OPTION_MEMCTLR_PH
      #define PSC_PH_UDIMM_DDR2
      #define PSC_PH_UDIMM_DDR3     MemAGetPsCfgUPh3,
      #define PSC_PH_RDIMM_DDR2
      #define PSC_PH_RDIMM_DDR3
      #define PSC_PH_SODIMM_DDR2
      #define PSC_PH_SODIMM_DDR3    MemAGetPsCfgSPh3,
    #endif
    #if OPTION_MEMCTLR_RB
      #define PSC_RB_UDIMM_DDR2
      #define PSC_RB_UDIMM_DDR3     MemAGetPsCfgURb3,
      #define PSC_RB_RDIMM_DDR2
      #define PSC_RB_RDIMM_DDR3
      #define PSC_RB_SODIMM_DDR2
      #define PSC_RB_SODIMM_DDR3    MemAGetPsCfgSRb3,
    #endif
    #if OPTION_MEMCTLR_DA
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
        #define PSC_OR_UDIMM_DDR3     //MemAGetPsCfgUOr3,
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
        #define PSC_OR_RDIMM_DDR3     //MemAGetPsCfgROr3,
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
  #ifndef PSC_RB_UDIMM_DDR2
    #define PSC_RB_UDIMM_DDR2
  #endif
  #ifndef PSC_RB_RDIMM_DDR2
    #define PSC_RB_RDIMM_DDR2
  #endif
  #ifndef PSC_RB_SODIMM_DDR2
    #define PSC_RB_SODIMM_DDR2
  #endif
  #ifndef PSC_RB_UDIMM_DDR3
    #define PSC_RB_UDIMM_DDR3
  #endif
  #ifndef PSC_RB_RDIMM_DDR3
    #define PSC_RB_RDIMM_DDR3
  #endif
  #ifndef PSC_RB_SODIMM_DDR3
    #define PSC_RB_SODIMM_DDR3
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
  #ifndef PSC_NI_UDIMM_DDR2
    #define PSC_NI_UDIMM_DDR2
  #endif
  #ifndef PSC_NI_RDIMM_DDR2
    #define PSC_NI_RDIMM_DDR2
  #endif
  #ifndef PSC_NI_SODIMM_DDR2
    #define PSC_NI_SODIMM_DDR2
  #endif
  #ifndef PSC_NI_UDIMM_DDR3
    #define PSC_NI_UDIMM_DDR3
  #endif
  #ifndef PSC_NI_RDIMM_DDR3
    #define PSC_NI_RDIMM_DDR3
  #endif
  #ifndef PSC_NI_SODIMM_DDR3
    #define PSC_NI_SODIMM_DDR3
  #endif
  #ifndef PSC_PH_UDIMM_DDR2
    #define PSC_PH_UDIMM_DDR2
  #endif
  #ifndef PSC_PH_RDIMM_DDR2
    #define PSC_PH_RDIMM_DDR2
  #endif
  #ifndef PSC_PH_SODIMM_DDR2
    #define PSC_PH_SODIMM_DDR2
  #endif
  #ifndef PSC_PH_UDIMM_DDR3
    #define PSC_PH_UDIMM_DDR3
  #endif
  #ifndef PSC_PH_RDIMM_DDR3
    #define PSC_PH_RDIMM_DDR3
  #endif
  #ifndef PSC_PH_SODIMM_DDR3
    #define PSC_PH_SODIMM_DDR3
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
    PSC_RB_UDIMM_DDR3
    PSC_RB_SODIMM_DDR3
    PSC_DA_SODIMM_DDR2
    PSC_DA_UDIMM_DDR3
    PSC_DA_SODIMM_DDR3
    PSC_NI_UDIMM_DDR3
    PSC_NI_SODIMM_DDR3
    PSC_PH_UDIMM_DDR3
    PSC_PH_SODIMM_DDR3
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
  //remove warning#if SIZE_OF_PLATFORM > MAX_PLATFORM_TYPES
  //  #error   Size of memPlatformTypeInstalled array larger than MAX_PLATFORM_TYPES
  //#endif

  /*---------------------------------------------------------------------------------------------------
   * EXTRACTABLE PLATFORM SPECIFIC CONFIGURATION
   *
   *
   *---------------------------------------------------------------------------------------------------
   */
  #define MEM_PSC_FLOW_BLOCK_END NULL
  #define PSC_TBL_END NULL
  #define MEM_PSC_FLOW_DEFTRUE (BOOLEAN (*) (MEM_NB_BLOCK*, CONST MEM_PSC_TABLE_BLOCK *)) memDefTrue

  #if OPTION_MEMCTLR_OR
    #if OPTION_UDIMMS
      #if OPTION_AM3_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY MaxFreqTblEntUAM3;
        #define PSC_TBL_OR_UDIMM3_MAX_FREQ_AM3  &MaxFreqTblEntUAM3,
        extern PSC_TBL_ENTRY DramTermTblEntUAM3;
        #define PSC_TBL_OR_UDIMM3_DRAM_TERM_AM3  &DramTermTblEntUAM3,
        extern PSC_TBL_ENTRY OdtPat1DTblEntUAM3;
        #define PSC_TBL_OR_UDIMM3_ODT_PAT_1D_AM3  &OdtPat1DTblEntUAM3,
        extern PSC_TBL_ENTRY OdtPat2DTblEntUAM3;
        #define PSC_TBL_OR_UDIMM3_ODT_PAT_2D_AM3  &OdtPat2DTblEntUAM3,
        extern PSC_TBL_ENTRY OdtPat3DTblEntUAM3;
        #define PSC_TBL_OR_UDIMM3_ODT_PAT_3D_AM3  &OdtPat3DTblEntUAM3,
        extern PSC_TBL_ENTRY SAOTblEntUAM3;
        #define PSC_TBL_OR_UDIMM3_SAO_AM3  &SAOTblEntUAM3,
        extern PSC_TBL_ENTRY ClkDisMapEntUAM3;
        #define PSC_TBL_OR_UDIMM3_CLK_DIS_AM3 &ClkDisMapEntUAM3,
        extern PSC_TBL_ENTRY S2DTblEntUAM3;
        #define PSC_TBL_OR_UDIMM3_S2D_AM3  &S2DTblEntUAM3,
        extern PSC_TBL_ENTRY WLPass1SeedEntUAM3;
        #define PSC_TBL_OR_UDIMM3_WL_SEED_AM3  &WLPass1SeedEntUAM3,
        extern PSC_TBL_ENTRY HWRxEnPass1SeedEntUAM3;
        #define PSC_TBL_OR_UDIMM3_HWRXEN_SEED_AM3  &HWRxEnPass1SeedEntUAM3,
      #endif
      #if OPTION_C32_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY MaxFreqTblEntUC32;
        #define PSC_TBL_OR_UDIMM3_MAX_FREQ_C32  &MaxFreqTblEntUC32,
        extern PSC_TBL_ENTRY DramTermTblEntUC32;
        #define PSC_TBL_OR_UDIMM3_DRAM_TERM_C32  &DramTermTblEntUC32,
        extern PSC_TBL_ENTRY OdtPat1DTblEntUC32;
        #define PSC_TBL_OR_UDIMM3_ODT_PAT_1D_C32  &OdtPat1DTblEntUC32,
        extern PSC_TBL_ENTRY OdtPat2DTblEntUC32;
        #define PSC_TBL_OR_UDIMM3_ODT_PAT_2D_C32  &OdtPat2DTblEntUC32,
        extern PSC_TBL_ENTRY OdtPat3DTblEntUC32;
        #define PSC_TBL_OR_UDIMM3_ODT_PAT_3D_C32  &OdtPat3DTblEntUC32,
        extern PSC_TBL_ENTRY SAOTblEntUC32;
        #define PSC_TBL_OR_UDIMM3_SAO_C32  &SAOTblEntUC32,
        extern PSC_TBL_ENTRY ClkDisMapEntUC32;
        #define PSC_TBL_OR_UDIMM3_CLK_DIS_C32 &ClkDisMapEntUC32,
        extern PSC_TBL_ENTRY ClkDisMap3DEntUC32;
        #define PSC_TBL_OR_UDIMM3_CLK_DIS_3D_C32 &ClkDisMap3DEntUC32,
        extern PSC_TBL_ENTRY S2DTblEntUC32;
        #define PSC_TBL_OR_UDIMM3_S2D_C32  &S2DTblEntUC32,
        extern PSC_TBL_ENTRY WLPass1SeedEntUC32;
        #define PSC_TBL_OR_UDIMM3_WL_SEED_C32  &WLPass1SeedEntUC32,
        extern PSC_TBL_ENTRY HWRxEnPass1SeedEntUC32;
        #define PSC_TBL_OR_UDIMM3_HWRXEN_SEED_C32  &HWRxEnPass1SeedEntUC32,
      #endif
      #if OPTION_G34_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY MaxFreqTblEntUG34;
        #define PSC_TBL_OR_UDIMM3_MAX_FREQ_G34  &MaxFreqTblEntUG34,
        extern PSC_TBL_ENTRY DramTermTblEntUG34;
        #define PSC_TBL_OR_UDIMM3_DRAM_TERM_G34  &DramTermTblEntUG34,
        extern PSC_TBL_ENTRY OdtPat1DTblEntUG34;
        #define PSC_TBL_OR_UDIMM3_ODT_PAT_1D_G34  &OdtPat1DTblEntUG34,
        extern PSC_TBL_ENTRY OdtPat2DTblEntUG34;
        #define PSC_TBL_OR_UDIMM3_ODT_PAT_2D_G34  &OdtPat2DTblEntUG34,
        extern PSC_TBL_ENTRY OdtPat3DTblEntUG34;
        #define PSC_TBL_OR_UDIMM3_ODT_PAT_3D_G34  &OdtPat3DTblEntUG34,
        extern PSC_TBL_ENTRY SAOTblEntUG34;
        #define PSC_TBL_OR_UDIMM3_SAO_G34  &SAOTblEntUG34,
        extern PSC_TBL_ENTRY ClkDisMapEntUG34;
        #define PSC_TBL_OR_UDIMM3_CLK_DIS_G34 &ClkDisMapEntUG34,
        extern PSC_TBL_ENTRY S2DTblEntUG34;
        #define PSC_TBL_OR_UDIMM3_S2D_G34  &S2DTblEntUG34,
        extern PSC_TBL_ENTRY WLPass1SeedEntUG34;
        #define PSC_TBL_OR_UDIMM3_WL_SEED_G34  &WLPass1SeedEntUG34,
        extern PSC_TBL_ENTRY HWRxEnPass1SeedEntUG34;
        #define PSC_TBL_OR_UDIMM3_HWRXEN_SEED_G34  &HWRxEnPass1SeedEntUG34,
      #endif
    #endif
    #if OPTION_RDIMMS
      #if OPTION_C32_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY MaxFreqTblEntRC32;
        #define PSC_TBL_OR_RDIMM3_MAX_FREQ_C32  &MaxFreqTblEntRC32,
        extern PSC_TBL_ENTRY DramTermTblEntRC32;
        #define PSC_TBL_OR_RDIMM3_DRAM_TERM_C32  &DramTermTblEntRC32,
        extern PSC_TBL_ENTRY OdtPat1DTblEntRC32;
        #define PSC_TBL_OR_RDIMM3_ODT_PAT_1D_C32  &OdtPat1DTblEntRC32,
        extern PSC_TBL_ENTRY OdtPat2DTblEntRC32;
        #define PSC_TBL_OR_RDIMM3_ODT_PAT_2D_C32  &OdtPat2DTblEntRC32,
        extern PSC_TBL_ENTRY OdtPat3DTblEntRC32;
        #define PSC_TBL_OR_RDIMM3_ODT_PAT_3D_C32  &OdtPat3DTblEntRC32,
        extern PSC_TBL_ENTRY SAOTblEntRC32;
        #define PSC_TBL_OR_RDIMM3_SAO_C32  &SAOTblEntRC32,
        extern PSC_TBL_ENTRY RC2IBTTblEntRC32;
        #define PSC_TBL_OR_RDIMM3_RC2IBT_C32  &RC2IBTTblEntRC32,
        extern PSC_TBL_ENTRY RC10OpSpdTblEntRC32;
        #define PSC_TBL_OR_RDIMM3_RC10OPSPD_C32  &RC10OpSpdTblEntRC32,
        extern PSC_TBL_ENTRY ClkDisMapEntRC32;
        #define PSC_TBL_OR_RDIMM3_CLK_DIS_C32 &ClkDisMapEntRC32,
        extern PSC_TBL_ENTRY S2DTblEntRC32;
        #define PSC_TBL_OR_RDIMM3_S2D_C32  &S2DTblEntRC32,
        extern PSC_TBL_ENTRY WLPass1SeedEntRC32;
        #define PSC_TBL_OR_RDIMM3_WL_SEED_C32  &WLPass1SeedEntRC32,
        extern PSC_TBL_ENTRY HWRxEnPass1SeedEntRC32;
        #define PSC_TBL_OR_RDIMM3_HWRXEN_SEED_C32  &HWRxEnPass1SeedEntRC32,
      #endif
      #if OPTION_G34_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY MaxFreqTblEntRG34;
        #define PSC_TBL_OR_RDIMM3_MAX_FREQ_G34  &MaxFreqTblEntRG34,
        extern PSC_TBL_ENTRY DramTermTblEntRG34;
        #define PSC_TBL_OR_RDIMM3_DRAM_TERM_G34  &DramTermTblEntRG34,
        extern PSC_TBL_ENTRY OdtPat1DTblEntRG34;
        #define PSC_TBL_OR_RDIMM3_ODT_PAT_1D_G34  &OdtPat1DTblEntRG34,
        extern PSC_TBL_ENTRY OdtPat2DTblEntRG34;
        #define PSC_TBL_OR_RDIMM3_ODT_PAT_2D_G34  &OdtPat2DTblEntRG34,
        extern PSC_TBL_ENTRY OdtPat3DTblEntRG34;
        #define PSC_TBL_OR_RDIMM3_ODT_PAT_3D_G34  &OdtPat3DTblEntRG34,
        extern PSC_TBL_ENTRY SAOTblEntRG34;
        #define PSC_TBL_OR_RDIMM3_SAO_G34  &SAOTblEntRG34,
        extern PSC_TBL_ENTRY RC2IBTTblEntRG34;
        #define PSC_TBL_OR_RDIMM3_RC2IBT_G34  &RC2IBTTblEntRG34,
        extern PSC_TBL_ENTRY RC10OpSpdTblEntRG34;
        #define PSC_TBL_OR_RDIMM3_RC10OPSPD_G34  &RC10OpSpdTblEntRG34,
        extern PSC_TBL_ENTRY ClkDisMapEntRG34;
        #define PSC_TBL_OR_RDIMM3_CLK_DIS_G34 &ClkDisMapEntRG34,
        extern PSC_TBL_ENTRY S2DTblEntRG34;
        #define PSC_TBL_OR_RDIMM3_S2D_G34  &S2DTblEntRG34,
        extern PSC_TBL_ENTRY WLPass1SeedEntRG34;
        #define PSC_TBL_OR_RDIMM3_WL_SEED_G34  &WLPass1SeedEntRG34,
        extern PSC_TBL_ENTRY HWRxEnPass1SeedEntRG34;
        #define PSC_TBL_OR_RDIMM3_HWRXEN_SEED_G34  &HWRxEnPass1SeedEntRG34,
      #endif
    #endif
    //#if OPTION_SODIMMS
    //#endif
    #if OPTION_LRDIMMS
    //  #if OPTION_C32_SOCKET_SUPPORT
    //    extern PSC_TBL_ENTRY MaxFreqTblEntLRC32;
    //    #define PSC_TBL_OR_LRDIMM3_MAX_FREQ_C32  &MaxFreqTblEntLRC32,
    //    extern PSC_TBL_ENTRY DramTermTblEntLRC32;
    //    #define PSC_TBL_OR_LRDIMM3_DRAM_TERM_C32  &DramTermTblEntLRC32,
    //    extern PSC_TBL_ENTRY OdtPat1DTblEntRC32;
    //    #define PSC_TBL_OR_LRDIMM3_ODT_PAT_1D_C32  &OdtPat1DTblEntLRC32,
    //    extern PSC_TBL_ENTRY OdtPat2DTblEntRC32;
    //    #define PSC_TBL_OR_LRDIMM3_ODT_PAT_2D_C32  &OdtPat2DTblEntLRC32,
    //    extern PSC_TBL_ENTRY OdtPat3DTblEntRC32;
    //    #define PSC_TBL_OR_LRDIMM3_ODT_PAT_3D_C32  &OdtPat3DTblEntLRC32,
    //    extern PSC_TBL_ENTRY SAOTblEntRC32;
    //    #define PSC_TBL_OR_LRDIMM3_SAO_C32  &SAOTblEntLRC32,
    //    extern PSC_TBL_ENTRY IBTTblEntLRC32;
    //    #define PSC_TBL_OR_LRDIMM3_IBT_C32  &IBTTblEntLRC32,
    //    extern PSC_TBL_ENTRY ClkDisMapEntLRC32;
    //    #define PSC_TBL_OR_LRDIMM3_CLK_DIS_C32 &ClkDisMapEntLRC32,
    //    extern PSC_TBL_ENTRY S2DTblEntLRC32;
    //    #define PSC_TBL_OR_LRDIMM3_S2D_C32  &S2DTblEntLRC32,
    //  #endif
      #if OPTION_G34_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY MaxFreqTblEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_MAX_FREQ_G34  &MaxFreqTblEntLRG34,
        extern PSC_TBL_ENTRY DramTermTblEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_DRAM_TERM_G34  &DramTermTblEntLRG34,
        extern PSC_TBL_ENTRY OdtPat1DTblEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_ODT_PAT_1D_G34  &OdtPat1DTblEntLRG34,
        extern PSC_TBL_ENTRY OdtPat2DTblEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_ODT_PAT_2D_G34  &OdtPat2DTblEntLRG34,
        extern PSC_TBL_ENTRY OdtPat3DTblEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_ODT_PAT_3D_G34  &OdtPat3DTblEntLRG34,
        extern PSC_TBL_ENTRY SAOTblEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_SAO_G34  &SAOTblEntLRG34,
        extern PSC_TBL_ENTRY IBTTblEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_IBT_G34  &IBTTblEntLRG34,
        extern PSC_TBL_ENTRY ClkDisMapEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_CLK_DIS_G34 &ClkDisMapEntLRG34,
        extern PSC_TBL_ENTRY S2DTblEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_S2D_G34  &S2DTblEntLRG34,
        extern PSC_TBL_ENTRY WLPass1SeedEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_WL_SEED_G34  &WLPass1SeedEntLRG34,
        extern PSC_TBL_ENTRY HWRxEnPass1SeedEntLRG34;
        #define PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_G34 &HWRxEnPass1SeedEntLRG34,
      #endif
    #endif
    extern PSC_TBL_ENTRY MR0WrTblEntry;
    #define PSC_TBL_OR_MR0_WR  &MR0WrTblEntry,
    extern PSC_TBL_ENTRY MR0CLTblEntry;
    #define PSC_TBL_OR_MR0_CL  &MR0CLTblEntry,
    extern PSC_TBL_ENTRY OrDdr3CKETriEnt;
    #define PSC_TBL_OR_CKE_TRI &OrDdr3CKETriEnt,
    extern PSC_TBL_ENTRY OrDdr3ODTTri3DEnt;
    #define PSC_TBL_OR_ODT_TRI_3D &OrDdr3ODTTri3DEnt,
    extern PSC_TBL_ENTRY OrDdr3ODTTriEnt;
    #define PSC_TBL_OR_ODT_TRI &OrDdr3ODTTriEnt,
    extern PSC_TBL_ENTRY OrUDdr3CSTriEnt;
    #define PSC_TBL_OR_UDIMM3_CS_TRI &OrUDdr3CSTriEnt,
    extern PSC_TBL_ENTRY OrDdr3CSTriEnt;
    #define PSC_TBL_OR_CS_TRI &OrDdr3CSTriEnt,
    extern PSC_TBL_ENTRY OrLRDdr3ODTTri3DEnt;
    #define PSC_TBL_OR_LRDIMM3_ODT_TRI_3D &OrLRDdr3ODTTri3DEnt,
    extern PSC_TBL_ENTRY OrLRDdr3ODTTriEnt;
    #define PSC_TBL_OR_LRDIMM3_ODT_TRI &OrLRDdr3ODTTriEnt,

    #ifndef PSC_TBL_OR_UDIMM3_MAX_FREQ_AM3
      #define PSC_TBL_OR_UDIMM3_MAX_FREQ_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_MAX_FREQ_C32
      #define PSC_TBL_OR_UDIMM3_MAX_FREQ_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_MAX_FREQ_G34
      #define PSC_TBL_OR_UDIMM3_MAX_FREQ_G34
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_DRAM_TERM_AM3
      #define PSC_TBL_OR_UDIMM3_DRAM_TERM_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_DRAM_TERM_C32
      #define PSC_TBL_OR_UDIMM3_DRAM_TERM_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_DRAM_TERM_G34
      #define PSC_TBL_OR_UDIMM3_DRAM_TERM_G34
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_ODT_PAT_1D_AM3
      #define PSC_TBL_OR_UDIMM3_ODT_PAT_1D_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_ODT_PAT_1D_C32
      #define PSC_TBL_OR_UDIMM3_ODT_PAT_1D_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_ODT_PAT_1D_G34
      #define PSC_TBL_OR_UDIMM3_ODT_PAT_1D_G34
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_ODT_PAT_2D_AM3
      #define PSC_TBL_OR_UDIMM3_ODT_PAT_2D_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_ODT_PAT_2D_C32
      #define PSC_TBL_OR_UDIMM3_ODT_PAT_2D_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_ODT_PAT_2D_G34
      #define PSC_TBL_OR_UDIMM3_ODT_PAT_2D_G34
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_ODT_PAT_3D_AM3
      #define PSC_TBL_OR_UDIMM3_ODT_PAT_3D_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_ODT_PAT_3D_C32
      #define PSC_TBL_OR_UDIMM3_ODT_PAT_3D_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_ODT_PAT_3D_G34
      #define PSC_TBL_OR_UDIMM3_ODT_PAT_3D_G34
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_SAO_AM3
      #define PSC_TBL_OR_UDIMM3_SAO_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_SAO_C32
      #define PSC_TBL_OR_UDIMM3_SAO_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_SAO_G34
      #define PSC_TBL_OR_UDIMM3_SAO_G34
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_S2D_AM3
      #define PSC_TBL_OR_UDIMM3_S2D_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_S2D_C32
      #define PSC_TBL_OR_UDIMM3_S2D_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_S2D_G34
      #define PSC_TBL_OR_UDIMM3_S2D_G34
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_WL_SEED_AM3
      #define PSC_TBL_OR_UDIMM3_WL_SEED_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_WL_SEED_C32
      #define PSC_TBL_OR_UDIMM3_WL_SEED_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_WL_SEED_G34
      #define PSC_TBL_OR_UDIMM3_WL_SEED_G34
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_HWRXEN_SEED_AM3
      #define PSC_TBL_OR_UDIMM3_HWRXEN_SEED_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_HWRXEN_SEED_C32
      #define PSC_TBL_OR_UDIMM3_HWRXEN_SEED_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_HWRXEN_SEED_G34
      #define PSC_TBL_OR_UDIMM3_HWRXEN_SEED_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_MAX_FREQ_AM3
      #define PSC_TBL_OR_RDIMM3_MAX_FREQ_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_MAX_FREQ_C32
      #define PSC_TBL_OR_RDIMM3_MAX_FREQ_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_MAX_FREQ_G34
      #define PSC_TBL_OR_RDIMM3_MAX_FREQ_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_DRAM_TERM_AM3
      #define PSC_TBL_OR_RDIMM3_DRAM_TERM_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_DRAM_TERM_C32
      #define PSC_TBL_OR_RDIMM3_DRAM_TERM_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_DRAM_TERM_G34
      #define PSC_TBL_OR_RDIMM3_DRAM_TERM_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_ODT_PAT_1D_AM3
      #define PSC_TBL_OR_RDIMM3_ODT_PAT_1D_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_ODT_PAT_1D_C32
      #define PSC_TBL_OR_RDIMM3_ODT_PAT_1D_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_ODT_PAT_1D_G34
      #define PSC_TBL_OR_RDIMM3_ODT_PAT_1D_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_ODT_PAT_2D_AM3
      #define PSC_TBL_OR_RDIMM3_ODT_PAT_2D_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_ODT_PAT_2D_C32
      #define PSC_TBL_OR_RDIMM3_ODT_PAT_2D_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_ODT_PAT_2D_G34
      #define PSC_TBL_OR_RDIMM3_ODT_PAT_2D_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_ODT_PAT_3D_AM3
      #define PSC_TBL_OR_RDIMM3_ODT_PAT_3D_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_ODT_PAT_3D_C32
      #define PSC_TBL_OR_RDIMM3_ODT_PAT_3D_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_ODT_PAT_3D_G34
      #define PSC_TBL_OR_RDIMM3_ODT_PAT_3D_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_SAO_AM3
      #define PSC_TBL_OR_RDIMM3_SAO_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_SAO_C32
      #define PSC_TBL_OR_RDIMM3_SAO_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_SAO_G34
      #define PSC_TBL_OR_RDIMM3_SAO_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_S2D_AM3
      #define PSC_TBL_OR_RDIMM3_S2D_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_S2D_C32
      #define PSC_TBL_OR_RDIMM3_S2D_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_S2D_G34
      #define PSC_TBL_OR_RDIMM3_S2D_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_RC2IBT_AM3
      #define PSC_TBL_OR_RDIMM3_RC2IBT_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_RC2IBT_C32
      #define PSC_TBL_OR_RDIMM3_RC2IBT_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_RC2IBT_G34
      #define PSC_TBL_OR_RDIMM3_RC2IBT_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_RC10OPSPD_AM3
      #define PSC_TBL_OR_RDIMM3_RC10OPSPD_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_RC10OPSPD_C32
      #define PSC_TBL_OR_RDIMM3_RC10OPSPD_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_RC10OPSPD_G34
      #define PSC_TBL_OR_RDIMM3_RC10OPSPD_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_WL_SEED_AM3
      #define PSC_TBL_OR_RDIMM3_WL_SEED_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_WL_SEED_C32
      #define PSC_TBL_OR_RDIMM3_WL_SEED_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_WL_SEED_G34
      #define PSC_TBL_OR_RDIMM3_WL_SEED_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_HWRXEN_SEED_AM3
      #define PSC_TBL_OR_RDIMM3_HWRXEN_SEED_AM3
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_HWRXEN_SEED_C32
      #define PSC_TBL_OR_RDIMM3_HWRXEN_SEED_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_HWRXEN_SEED_G34
      #define PSC_TBL_OR_RDIMM3_HWRXEN_SEED_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_MAX_FREQ_C32
      #define PSC_TBL_OR_LRDIMM3_MAX_FREQ_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_MAX_FREQ_G34
      #define PSC_TBL_OR_LRDIMM3_MAX_FREQ_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_DRAM_TERM_C32
      #define PSC_TBL_OR_LRDIMM3_DRAM_TERM_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_DRAM_TERM_G34
      #define PSC_TBL_OR_LRDIMM3_DRAM_TERM_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_ODT_PAT_1D_C32
      #define PSC_TBL_OR_LRDIMM3_ODT_PAT_1D_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_ODT_PAT_1D_G34
      #define PSC_TBL_OR_LRDIMM3_ODT_PAT_1D_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_ODT_PAT_2D_C32
      #define PSC_TBL_OR_LRDIMM3_ODT_PAT_2D_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_ODT_PAT_2D_G34
      #define PSC_TBL_OR_LRDIMM3_ODT_PAT_2D_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_ODT_PAT_3D_C32
      #define PSC_TBL_OR_LRDIMM3_ODT_PAT_3D_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_ODT_PAT_3D_G34
      #define PSC_TBL_OR_LRDIMM3_ODT_PAT_3D_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_SAO_C32
      #define PSC_TBL_OR_LRDIMM3_SAO_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_SAO_G34
      #define PSC_TBL_OR_LRDIMM3_SAO_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_S2D_C32
      #define PSC_TBL_OR_LRDIMM3_S2D_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_S2D_G34
      #define PSC_TBL_OR_LRDIMM3_S2D_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_IBT_C32
      #define PSC_TBL_OR_LRDIMM3_IBT_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_IBT_G34
      #define PSC_TBL_OR_LRDIMM3_IBT_G34
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_CLK_DIS_AM3
      #define PSC_TBL_OR_UDIMM3_CLK_DIS_AM3
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_CLK_DIS_C32
      #define PSC_TBL_OR_UDIMM3_CLK_DIS_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_CLK_DIS_3D_C32
      #define PSC_TBL_OR_UDIMM3_CLK_DIS_3D_C32
    #endif
    #ifndef PSC_TBL_OR_UDIMM3_CLK_DIS_G34
      #define PSC_TBL_OR_UDIMM3_CLK_DIS_G34
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_CLK_DIS_C32
      #define PSC_TBL_OR_RDIMM3_CLK_DIS_C32
    #endif
    #ifndef PSC_TBL_OR_RDIMM3_CLK_DIS_G34
      #define PSC_TBL_OR_RDIMM3_CLK_DIS_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_CLK_DIS_C32
      #define PSC_TBL_OR_LRDIMM3_CLK_DIS_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_CLK_DIS_G34
      #define PSC_TBL_OR_LRDIMM3_CLK_DIS_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_WL_SEED_AM3
      #define PSC_TBL_OR_LRDIMM3_WL_SEED_AM3
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_WL_SEED_C32
      #define PSC_TBL_OR_LRDIMM3_WL_SEED_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_WL_SEED_G34
      #define PSC_TBL_OR_LRDIMM3_WL_SEED_G34
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_AM3
      #define PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_AM3
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_C32
      #define PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_C32
    #endif
    #ifndef PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_G34
      #define PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_G34
    #endif


    PSC_TBL_ENTRY* memPSCTblMaxFreqArrayOR[] = {
      PSC_TBL_OR_UDIMM3_MAX_FREQ_AM3
      PSC_TBL_OR_UDIMM3_MAX_FREQ_C32
      PSC_TBL_OR_UDIMM3_MAX_FREQ_G34
      PSC_TBL_OR_RDIMM3_MAX_FREQ_AM3
      PSC_TBL_OR_RDIMM3_MAX_FREQ_C32
      PSC_TBL_OR_RDIMM3_MAX_FREQ_G34
      PSC_TBL_OR_LRDIMM3_MAX_FREQ_C32
      PSC_TBL_OR_LRDIMM3_MAX_FREQ_G34
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblDramTermArrayOR[] = {
      PSC_TBL_OR_UDIMM3_DRAM_TERM_AM3
      PSC_TBL_OR_UDIMM3_DRAM_TERM_C32
      PSC_TBL_OR_UDIMM3_DRAM_TERM_G34
      PSC_TBL_OR_RDIMM3_DRAM_TERM_AM3
      PSC_TBL_OR_RDIMM3_DRAM_TERM_C32
      PSC_TBL_OR_RDIMM3_DRAM_TERM_G34
      PSC_TBL_OR_LRDIMM3_DRAM_TERM_C32
      PSC_TBL_OR_LRDIMM3_DRAM_TERM_G34
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblODTPatArrayOR[] = {
      PSC_TBL_OR_UDIMM3_ODT_PAT_1D_AM3
      PSC_TBL_OR_UDIMM3_ODT_PAT_2D_AM3
      PSC_TBL_OR_UDIMM3_ODT_PAT_3D_AM3
      PSC_TBL_OR_RDIMM3_ODT_PAT_1D_AM3
      PSC_TBL_OR_RDIMM3_ODT_PAT_2D_AM3
      PSC_TBL_OR_RDIMM3_ODT_PAT_3D_AM3
      PSC_TBL_OR_UDIMM3_ODT_PAT_1D_C32
      PSC_TBL_OR_UDIMM3_ODT_PAT_2D_C32
      PSC_TBL_OR_UDIMM3_ODT_PAT_3D_C32
      PSC_TBL_OR_RDIMM3_ODT_PAT_1D_C32
      PSC_TBL_OR_RDIMM3_ODT_PAT_2D_C32
      PSC_TBL_OR_RDIMM3_ODT_PAT_3D_C32
      PSC_TBL_OR_LRDIMM3_ODT_PAT_1D_C32
      PSC_TBL_OR_LRDIMM3_ODT_PAT_2D_C32
      PSC_TBL_OR_LRDIMM3_ODT_PAT_3D_C32
      PSC_TBL_OR_UDIMM3_ODT_PAT_1D_G34
      PSC_TBL_OR_UDIMM3_ODT_PAT_2D_G34
      PSC_TBL_OR_UDIMM3_ODT_PAT_3D_G34
      PSC_TBL_OR_RDIMM3_ODT_PAT_1D_G34
      PSC_TBL_OR_RDIMM3_ODT_PAT_2D_G34
      PSC_TBL_OR_RDIMM3_ODT_PAT_3D_G34
      PSC_TBL_OR_LRDIMM3_ODT_PAT_1D_G34
      PSC_TBL_OR_LRDIMM3_ODT_PAT_2D_G34
      PSC_TBL_OR_LRDIMM3_ODT_PAT_3D_G34
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblSAOArrayOR[] = {
      PSC_TBL_OR_UDIMM3_SAO_AM3
      PSC_TBL_OR_UDIMM3_SAO_C32
      PSC_TBL_OR_UDIMM3_SAO_G34
      PSC_TBL_OR_RDIMM3_SAO_AM3
      PSC_TBL_OR_RDIMM3_SAO_C32
      PSC_TBL_OR_RDIMM3_SAO_G34
      PSC_TBL_OR_LRDIMM3_SAO_C32
      PSC_TBL_OR_LRDIMM3_SAO_G34
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblS2DArrayOR[] = {
      PSC_TBL_OR_UDIMM3_S2D_AM3
      PSC_TBL_OR_UDIMM3_S2D_C32
      PSC_TBL_OR_UDIMM3_S2D_G34
      PSC_TBL_OR_RDIMM3_S2D_AM3
      PSC_TBL_OR_RDIMM3_S2D_C32
      PSC_TBL_OR_RDIMM3_S2D_G34
      PSC_TBL_OR_LRDIMM3_S2D_C32
      PSC_TBL_OR_LRDIMM3_S2D_G34
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblMR0WRArrayOR[] = {
      PSC_TBL_OR_MR0_WR
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblMR0CLArrayOR[] = {
      PSC_TBL_OR_MR0_CL
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblRC2IBTArrayOR[] = {
      PSC_TBL_OR_RDIMM3_RC2IBT_AM3
      PSC_TBL_OR_RDIMM3_RC2IBT_C32
      PSC_TBL_OR_RDIMM3_RC2IBT_G34
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblRC10OPSPDArrayOR[] = {
      PSC_TBL_OR_RDIMM3_RC10OPSPD_AM3
      PSC_TBL_OR_RDIMM3_RC10OPSPD_C32
      PSC_TBL_OR_RDIMM3_RC10OPSPD_G34
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblLRIBTArrayOR[] = {
      PSC_TBL_OR_LRDIMM3_IBT_C32
      PSC_TBL_OR_LRDIMM3_IBT_G34
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblGenArrayOR[] = {
      PSC_TBL_OR_UDIMM3_CLK_DIS_AM3
      PSC_TBL_OR_UDIMM3_CLK_DIS_C32
      PSC_TBL_OR_UDIMM3_CLK_DIS_3D_C32
      PSC_TBL_OR_UDIMM3_CLK_DIS_G34
      PSC_TBL_OR_RDIMM3_CLK_DIS_C32
      PSC_TBL_OR_RDIMM3_CLK_DIS_G34
      PSC_TBL_OR_LRDIMM3_CLK_DIS_C32
      PSC_TBL_OR_LRDIMM3_CLK_DIS_G34
      PSC_TBL_OR_CKE_TRI
      PSC_TBL_OR_ODT_TRI_3D
      PSC_TBL_OR_ODT_TRI
      PSC_TBL_OR_LRDIMM3_ODT_TRI_3D
      PSC_TBL_OR_LRDIMM3_ODT_TRI
      PSC_TBL_OR_UDIMM3_CS_TRI
      PSC_TBL_OR_CS_TRI
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblWLSeedArrayOR[] = {
      PSC_TBL_OR_UDIMM3_WL_SEED_AM3
      PSC_TBL_OR_UDIMM3_WL_SEED_C32
      PSC_TBL_OR_UDIMM3_WL_SEED_G34
      PSC_TBL_OR_RDIMM3_WL_SEED_AM3
      PSC_TBL_OR_RDIMM3_WL_SEED_C32
      PSC_TBL_OR_RDIMM3_WL_SEED_G34
      PSC_TBL_OR_LRDIMM3_WL_SEED_AM3
      PSC_TBL_OR_LRDIMM3_WL_SEED_C32
      PSC_TBL_OR_LRDIMM3_WL_SEED_G34
      PSC_TBL_END
    };

    PSC_TBL_ENTRY* memPSCTblHWRxEnSeedArrayOR[] = {
      PSC_TBL_OR_UDIMM3_HWRXEN_SEED_AM3
      PSC_TBL_OR_UDIMM3_HWRXEN_SEED_C32
      PSC_TBL_OR_UDIMM3_HWRXEN_SEED_G34
      PSC_TBL_OR_RDIMM3_HWRXEN_SEED_AM3
      PSC_TBL_OR_RDIMM3_HWRXEN_SEED_C32
      PSC_TBL_OR_RDIMM3_HWRXEN_SEED_G34
      PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_AM3
      PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_C32
      PSC_TBL_OR_LRDIMM3_HWRXEN_SEED_G34
      PSC_TBL_END
    };

    MEM_PSC_TABLE_BLOCK memPSCTblBlockOr = {
      (PSC_TBL_ENTRY **)&memPSCTblMaxFreqArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblDramTermArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblODTPatArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblSAOArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblMR0WRArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblMR0CLArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblRC2IBTArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblRC10OPSPDArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblLRIBTArrayOR,
      NULL,
      NULL,
      (PSC_TBL_ENTRY **)&memPSCTblGenArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblS2DArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblWLSeedArrayOR,
      (PSC_TBL_ENTRY **)&memPSCTblHWRxEnSeedArrayOR
    };

    extern MEM_PSC_FLOW MemPGetMaxFreqSupported;
    #define PSC_FLOW_OR_MAX_FREQ   MemPGetMaxFreqSupported
    extern MEM_PSC_FLOW MemPGetRttNomWr;
    #define PSC_FLOW_OR_DRAM_TERM   MemPGetRttNomWr
    extern MEM_PSC_FLOW MemPGetODTPattern;
    #define PSC_FLOW_OR_ODT_PATTERN   MemPGetODTPattern
    extern MEM_PSC_FLOW MemPGetSAO;
    #define PSC_FLOW_OR_SAO   MemPGetSAO
    extern MEM_PSC_FLOW MemPGetMR0WrCL;
    #define PSC_FLOW_OR_MR0_WRCL   MemPGetMR0WrCL
    extern MEM_PSC_FLOW MemPGetTrainingSeeds;
    #define PSC_FLOW_OR_SEED MemPGetTrainingSeeds
    #if OPTION_RDIMMS
      extern MEM_PSC_FLOW MemPGetRC2IBT;
      #define PSC_FLOW_OR_RC2_IBT    MemPGetRC2IBT
      extern MEM_PSC_FLOW MemPGetRC10OpSpd;
      #define PSC_FLOW_OR_RC10_OPSPD   MemPGetRC10OpSpd
    #endif
    #if OPTION_LRDIMMS
      extern MEM_PSC_FLOW MemPGetLRIBT;
      #define PSC_FLOW_OR_LR_IBT   MemPGetLRIBT
      extern MEM_PSC_FLOW MemPGetLRNPR;
      #define PSC_FLOW_OR_LR_NPR   MemPGetLRNPR
      extern MEM_PSC_FLOW MemPGetLRNLR;
      #define PSC_FLOW_OR_LR_NLR  MemPGetLRNLR
    #endif
    #ifndef PSC_FLOW_OR_MAX_FREQ
      #define PSC_FLOW_OR_MAX_FREQ   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_DRAM_TERM
      #define PSC_FLOW_OR_DRAM_TERM   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_ODT_PATTERN
      #define PSC_FLOW_OR_ODT_PATTERN   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_SAO
      #define PSC_FLOW_OR_SAO   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_MR0_WRCL
      #define PSC_FLOW_OR_MR0_WRCL   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_RC2_IBT
      #define PSC_FLOW_OR_RC2_IBT   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_RC10_OPSPD
      #define PSC_FLOW_OR_RC10_OPSPD   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_LR_IBT
      #define PSC_FLOW_OR_LR_IBT   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_LR_NPR
      #define PSC_FLOW_OR_LR_NPR   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_LR_NLR
      #define PSC_FLOW_OR_LR_NLR   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_S2D
      #define PSC_FLOW_OR_S2D   MEM_PSC_FLOW_DEFTRUE
    #endif
    #ifndef PSC_FLOW_OR_SEED
      #define PSC_FLOW_OR_SEED   MEM_PSC_FLOW_DEFTRUE
    #endif

    MEM_PSC_FLOW_BLOCK memPlatSpecFlowOR = {
      &memPSCTblBlockOr,
      PSC_FLOW_OR_MAX_FREQ,
      PSC_FLOW_OR_DRAM_TERM,
      PSC_FLOW_OR_ODT_PATTERN,
      PSC_FLOW_OR_SAO,
      PSC_FLOW_OR_MR0_WRCL,
      PSC_FLOW_OR_RC2_IBT,
      PSC_FLOW_OR_RC10_OPSPD,
      PSC_FLOW_OR_LR_IBT,
      PSC_FLOW_OR_LR_NPR,
      PSC_FLOW_OR_LR_NLR,
      PSC_FLOW_OR_S2D,
      PSC_FLOW_OR_SEED
    };
    #define MEM_PSC_FLOW_BLOCK_OR &memPlatSpecFlowOR,
  #else
    #define MEM_PSC_FLOW_BLOCK_OR
  #endif

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
    #if OPTION_FM2_SOCKET_SUPPORT
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
      extern PSC_TBL_ENTRY TNDramTermTblEntU;
      #define PSC_TBL_TN_UDIMM3_DRAM_TERM  &TNDramTermTblEntU,
      extern PSC_TBL_ENTRY TNSAOTblEntU3;
      #define PSC_TBL_TN_UDIMM3_SAO  &TNSAOTblEntU3,
      #undef PSC_TBL_TN_UDIMM3_S2D_FM2
      extern PSC_TBL_ENTRY ex891_1 ;
      #define PSC_TBL_TN_UDIMM3_S2D_FM2  &ex891_1 ,
    #endif
    #if OPTION_SODIMMS
      #if OPTION_FS1_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY TNSAOTblEntSO3;
        #define PSC_TBL_TN_SODIMM3_SAO  &TNSAOTblEntSO3,
        extern PSC_TBL_ENTRY TNDramTermTblEntSO;
        #define PSC_TBL_TN_SODIMM3_DRAM_TERM  &TNDramTermTblEntSO,
        extern PSC_TBL_ENTRY TNMaxFreqTblEntSO;
        #define PSC_TBL_TN_SODIMM3_MAX_FREQ  &TNMaxFreqTblEntSO,
        #undef PSC_TBL_TN_SODIMM3_S2D_FS1
        #define PSC_TBL_TN_SODIMM3_S2D_FS1
      #endif
      #if OPTION_FM2_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY TNSAOTblEntSO3;
        #define PSC_TBL_TN_SODIMM3_SAO  &TNSAOTblEntSO3,
        extern PSC_TBL_ENTRY TNDramTermTblEntSO;
        #define PSC_TBL_TN_SODIMM3_DRAM_TERM  &TNDramTermTblEntSO,
        extern PSC_TBL_ENTRY TNMaxFreqTblEntSO;
        #define PSC_TBL_TN_SODIMM3_MAX_FREQ  &TNMaxFreqTblEntSO,
        #undef PSC_TBL_TN_SODIMM3_S2D_FM2
        extern PSC_TBL_ENTRY ex891_1 ;
        #define PSC_TBL_TN_SODIMM3_S2D_FM2 &ex891_1 ,
      #endif
      #if OPTION_FP2_SOCKET_SUPPORT
        extern PSC_TBL_ENTRY TNSAOTblEntSODWNSO3;
        #define PSC_TBL_TN_SODWN_SODIMM3_SAO  &TNSAOTblEntSODWNSO3,
        extern PSC_TBL_ENTRY TNDramTermTblEntSODWNSO;
        #define PSC_TBL_TN_SODWN_SODIMM3_DRAM_TERM  &TNDramTermTblEntSODWNSO,
        extern PSC_TBL_ENTRY TNMaxFreqTblEntSODWNSO;
        #define PSC_TBL_TN_SODWN_SODIMM3_MAX_FREQ  &TNMaxFreqTblEntSODWNSO,
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
    #ifndef PSC_TBL_TN_SODWN_SODIMM3_MAX_FREQ
      #define PSC_TBL_TN_SODWN_SODIMM3_MAX_FREQ
    #endif
    #ifndef PSC_TBL_TN_UDIMM3_MAX_FREQ
      #define PSC_TBL_TN_UDIMM3_MAX_FREQ
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

    PSC_TBL_ENTRY* CONST memPSCTblMaxFreqArrayTN[] = {
      PSC_TBL_TN_SODIMM3_MAX_FREQ
      PSC_TBL_TN_SODWN_SODIMM3_MAX_FREQ
      PSC_TBL_TN_UDIMM3_MAX_FREQ
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
      MEM_PSC_FLOW_DEFTRUE,
      MEM_PSC_FLOW_DEFTRUE
    };
    #define MEM_PSC_FLOW_BLOCK_TN &memPlatSpecFlowTN,
  #else
    #define MEM_PSC_FLOW_BLOCK_TN
  #endif


  CONST MEM_PSC_FLOW_BLOCK* CONST memPlatSpecFlowArray[] = {
    MEM_PSC_FLOW_BLOCK_OR
    MEM_PSC_FLOW_BLOCK_TN
    MEM_PSC_FLOW_BLOCK_END
  };

  /*---------------------------------------------------------------------------------------------------
  *
  *  LRDIMM CONTROL
  *
  *---------------------------------------------------------------------------------------------------
  */
  #if (OPTION_LRDIMMS == TRUE)
    #if (OPTION_MEMCTLR_OR == TRUE)
      #define MEM_TECH_FEATURE_LRDIMM_INIT  &MemTLrdimmConstructor3
    #else //#if (OPTION_MEMCTLR_OR == FALSE)
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
  MEM_FLOW_CFG* CONST  memFlowControlInstalled[] = {
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
  #if (OPTION_MEMCTLR_DR == TRUE)
    #undef MEM_NB_SUPPORT_DR
    #define MEM_NB_SUPPORT_DR { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DR, MEM_IDENDIMM_DR },
  #endif
  #if (OPTION_MEMCTLR_RB == TRUE)
    #undef MEM_NB_SUPPORT_RB
    #define MEM_NB_SUPPORT_RB { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_RB, MEM_IDENDIMM_RB },
  #endif
  #if (OPTION_MEMCTLR_DA == TRUE)
    #undef MEM_NB_SUPPORT_DA
    #define MEM_NB_SUPPORT_DA { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_DA, MEM_IDENDIMM_DA },
  #endif
  #if (OPTION_MEMCTLR_PH == TRUE)
    #undef MEM_NB_SUPPORT_PH
    #define MEM_NB_SUPPORT_PH { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_PH, MEM_IDENDIMM_PH },
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
  #if (OPTION_MEMCTLR_OR == TRUE)
    #undef MEM_NB_SUPPORT_OR
    #define MEM_NB_SUPPORT_OR { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_OR, MEM_IDENDIMM_OR },
  #endif
  #if (OPTION_MEMCTLR_TN == TRUE)
    #undef MEM_NB_SUPPORT_TN
    #define MEM_NB_SUPPORT_TN { MEM_NB_SUPPORT_STRUCT_VERSION, NULL, NULL, NULL, MEM_FEATURE_S3_RESUME_CONSTRUCTOR_TN, MEM_IDENDIMM_TN },
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
  #if (OPTION_MEMCTLR_DR == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledDr[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_RB == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledRb[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_DA == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledDA[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_Ni == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledNi[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_PH == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledPh[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_LN == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledLN[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_HY == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledHy[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_OR == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledOr[MAX_FF_TYPES] = {
      NULL
    };
  #endif
  #if (OPTION_MEMCTLR_C32 == TRUE)
    MEM_PLAT_SPEC_CFG* CONST memPlatSpecFFInstalledC32[MAX_FF_TYPES] = {
      NULL
    };
  #endif
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
  MEM_NB_SUPPORT_RB
  MEM_NB_SUPPORT_DA
  MEM_NB_SUPPORT_Ni
  MEM_NB_SUPPORT_PH
  MEM_NB_SUPPORT_HY
  MEM_NB_SUPPORT_LN
  MEM_NB_SUPPORT_OR
  MEM_NB_SUPPORT_C32
  MEM_NB_SUPPORT_ON
  MEM_NB_SUPPORT_TN
  MEM_NB_SUPPORT_END
};

#endif  // _OPTION_MEMORY_INSTALL_H_
