/**
 * @file
 *
 * AMD Memory option API.
 *
 * Contains structures and values used to control the Memory option code.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  OPTION
 * @e \$Revision: 5937 $   @e \$Date: 2008-04-25 03:59:36 -0700 (Fri, 25 Apr 2008) $
 *
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
 ******************************************************************************
 */

#ifndef _OPTION_MEMORY_H_
#define _OPTION_MEMORY_H_

/* Memory Includes */
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "ma.h"
#include "mp.h"
/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define MAX_FF_TYPES      6   ///< Maximum number of DDR Form factors (UDIMMs, RDIMMMs, SODIMMS) supported

/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */

/*
* STANDARD MEMORY FEATURE FUNCTION POINTER
*/

typedef BOOLEAN OPTION_MEM_FEATURE_NB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

typedef BOOLEAN MEM_TECH_FEAT (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

typedef UINT8 MEM_TABLE_FEAT (
  IN OUT   MEM_TABLE_ALIAS **MTPtr
  );

#define MEM_FEAT_BLOCK_NB_STRUCT_VERSION  0x01

/**
 *  MEMORY FEATURE BLOCK - This structure serves as a vector table for standard
 *  memory feature implementation functions.  It contains vectors for all of the
 *  features that are supported by the various Northbridge devices supported by
 *  AGESA.
 */
typedef struct _MEM_FEAT_BLOCK_NB {
  UINT16              OptMemFeatVersion; ///< Version of memory feature block.
  OPTION_MEM_FEATURE_NB  *OnlineSpare; ///< Online spare support.
  OPTION_MEM_FEATURE_NB  *InterleaveBanks; ///< Bank (Chip select) interleaving support.
  OPTION_MEM_FEATURE_NB  *UndoInterleaveBanks; ///< Undo Bank (Chip Select) interleaving.
  OPTION_MEM_FEATURE_NB  *CheckInterleaveNodes; ///< Check for Node interleaving support.
  OPTION_MEM_FEATURE_NB  *InterleaveNodes; ///< Node interleaving support.
  OPTION_MEM_FEATURE_NB  *InterleaveChannels; ///< Channel interleaving support.
  OPTION_MEM_FEATURE_NB  *InterleaveRegion;  ///< Interleave Region support.
  OPTION_MEM_FEATURE_NB  *CheckEcc; ///< Check for ECC support.
  OPTION_MEM_FEATURE_NB  *InitEcc; ///< ECC support.
  OPTION_MEM_FEATURE_NB  *Training; ///< Choose the type of training (Parallel, standard or hardcoded).
  OPTION_MEM_FEATURE_NB  *LvDdr3; ///< Low voltage DDR3 dimm support
  OPTION_MEM_FEATURE_NB  *OnDimmThermal; ///< On-Dimm thermal management
  MEM_TECH_FEAT  *DramInit; ///< Choose the type of Dram init (hardware based or software based).
  OPTION_MEM_FEATURE_NB  *ExcludeDIMM; ///< Exclude a dimm.
  OPTION_MEM_FEATURE_NB  *InitCPG; ///< Continuous pattern generation.
} MEM_FEAT_BLOCK_NB;

typedef AGESA_STATUS MEM_MAIN_FLOW_CONTROL (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

typedef BOOLEAN OPTION_MEM_FEATURE_MAIN (
  IN MEM_MAIN_DATA_BLOCK *MMPtr
  );

typedef BOOLEAN MEM_NB_CONSTRUCTOR (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *mmSharedPtr,     ///< Pointer to Memory scratchpad
  IN       UINT8 NodeID
  );

typedef BOOLEAN MEM_TECH_CONSTRUCTOR (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   MEM_NB_BLOCK   *NBPtr
  );

typedef VOID MEM_INITIALIZER (
  IN OUT   MEM_DATA_STRUCT *MemPtr
 );

typedef AGESA_STATUS MEM_PLATFORM_CFG (
  IN struct _MEM_DATA_STRUCT *MemData,
  IN UINT8 SocketID,
  IN CH_DEF_STRUCT *CurrentChannel
  );

typedef BOOLEAN MEM_IDENDIMM_CONSTRUCTOR (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  );

typedef VOID MEM_TECH_TRAINING_FEAT (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  );

typedef BOOLEAN MEM_RESUME_CONSTRUCTOR (
  IN OUT   VOID *S3NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  );

typedef AGESA_STATUS MEM_PLAT_SPEC_CFG (
  IN struct _MEM_DATA_STRUCT *MemData,
  IN OUT   CH_DEF_STRUCT *CurrentChannel,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

#define MEM_FEAT_BLOCK_MAIN_STRUCT_VERSION  0x01

/**
 * MAIN FEATURE BLOCK - This structure serves as vector table for memory features
 * that shared between all northbridge devices.
 */
typedef struct _MEM_FEAT_BLOCK_MAIN {
  UINT16              OptMemFeatVersion; ///< Version of main feature block.
  OPTION_MEM_FEATURE_MAIN *Training; ///< Training features.
  OPTION_MEM_FEATURE_MAIN *ExcludeDIMM; ///< Exclude a dimm.
  OPTION_MEM_FEATURE_MAIN *OnlineSpare; ///< On-line spare.
  OPTION_MEM_FEATURE_MAIN *InterleaveNodes; ///< Node interleave.
  OPTION_MEM_FEATURE_MAIN *InitEcc; ///< Initialize ECC on all nodes if they all support it.
  OPTION_MEM_FEATURE_MAIN *MemClr; ///< Memory Clear.
  OPTION_MEM_FEATURE_MAIN *MemDmi; ///< Memory DMI Support.
  OPTION_MEM_FEATURE_MAIN *LvDDR3; ///< Low voltage DDR3 support.
  OPTION_MEM_FEATURE_MAIN *UmaAllocation; ///< Uma Allocation.
  OPTION_MEM_FEATURE_MAIN *MemSave; ///< Memory Context Save
  OPTION_MEM_FEATURE_MAIN *MemRestore; ///< Memory Context Restore
} MEM_FEAT_BLOCK_MAIN;

#define MEM_NB_SUPPORT_STRUCT_VERSION  0x01
#define MEM_TECH_FEAT_BLOCK_STRUCT_VERSION  0x01
/**
 *  MEMORY TECHNOLOGY FEATURE BLOCK - This structure serves as a vector table for standard
 *  memory feature implementation functions.  It contains vectors for all of the
 *  features that are supported by the various Technology features supported by
 *  AGESA.
 */
typedef struct _MEM_TECH_FEAT_BLOCK {
  UINT16  OptMemTechFeatVersion; ///< Version of memory Tech feature block.
  MEM_TECH_FEAT *EnterHardwareTraining; ///<Enter HW WL Training
  MEM_TECH_FEAT *SwWLTraining; ///<SW Write Levelization training
  MEM_TECH_FEAT *HwBasedWLTrainingPart1; ///<HW based write levelization Training Part 1
  MEM_TECH_FEAT *HwBasedDQSReceiverEnableTrainingPart1; ///<HW based DQS receiver Enabled Training Part 1
  MEM_TECH_FEAT *HwBasedWLTrainingPart2; ///<HW based write levelization Training Part 2
  MEM_TECH_FEAT *HwBasedDQSReceiverEnableTrainingPart2; ///<HW based DQS receiver Enabled Training Part 2
  MEM_TECH_FEAT *TrainExitHwTrn; ///<Exit HW WL Training
  MEM_TECH_FEAT *NonOptimizedSWDQSRecEnTrainingPart1; ///< Non-Optimized Software based receiver Enable Training part 1
  MEM_TECH_FEAT *OptimizedSwDqsRecEnTrainingPart1; ///< Optimized Software based receiver Enable Training part 1
  MEM_TECH_FEAT *NonOptimizedSRdWrPosTraining; ///< Non-Optimized Rd Wr Position training
  MEM_TECH_FEAT *OptimizedSRdWrPosTraining; ///< Optimized Rd Wr Position training
  MEM_TECH_FEAT *MaxRdLatencyTraining; ///< MaxReadLatency Training
} MEM_TECH_FEAT_BLOCK;


/**
 * MEMORY NORTHBRIDGE SUPPORT STRUCT - This structure groups the Northbridge dependent
 * options together in a list to provide a single access point for all code to use
 * and to ensure that everything corresponding to the same NB type is grouped together.
 *
 * The Technology Block pointers are not included in this structure because DRAM technology
 * needs to be decoupled from the northbridge type.
 *
 */
typedef struct _MEM_NB_SUPPORT {
  UINT16              MemNBSupportVersion; ///< Version of northbridge support.
  MEM_NB_CONSTRUCTOR  *MemConstructNBBlock; ///< NorthBridge block constructor.
  MEM_INITIALIZER     *MemNInitDefaults; ///< Default value initialization for MEM_DATA_STRUCT.
  MEM_FEAT_BLOCK_NB      *MemFeatBlock; ///< Memory feature block.
  MEM_RESUME_CONSTRUCTOR *MemS3ResumeConstructNBBlock; ///< S3 memory initialization northbridge block constructor.
  MEM_IDENDIMM_CONSTRUCTOR *MemIdentifyDimmConstruct; ///< Constructor for address to dimm identification.
} MEM_NB_SUPPORT;

/*
 *  MEMORY Non-Training FEATURES - This structure serves as a vector table for standard
 *  memory non-training feature implementation functions.  It contains vectors for all of the
 *  features that are supported by the various Technology devices supported by
 *  AGESA.
 */

/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */
/* Feature Default Return */
BOOLEAN MemFDefRet (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN MemMDefRet (
  IN MEM_MAIN_DATA_BLOCK *MMPtr
  );
/* Table Feature Default Return */
UINT8 MemFTableDefRet (
  IN OUT   MEM_TABLE_ALIAS **MTPtr
  );
/* S3 Feature Default Return */
BOOLEAN MemFS3DefConstructorRet (
  IN OUT   VOID *S3NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  );

BOOLEAN
MemProcessConditionalOverrides (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 PsoAction,
  IN       UINT8 Dimm
   );

#endif  // _OPTION_MEMORY_H_
