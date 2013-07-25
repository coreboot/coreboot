/* $NoKeywords:$ */
/**
 * @file
 *
 * mf2DRdWrTraining.h
 *
 * Common Definitions ot support 2D Read/Write Training
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/2DRdWrTraining)
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
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
  * ***************************************************************************
  *
 */

#ifndef _MFRDWR2DTRAINING_H_
#define _MFRDWR2DTRAINING_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_2D_VREF_ENTRIES         0x20 ///< Maximum number of vref entries
#define MAX_RD_WR_DLY_ENTRIES       0x40 ///< Maximum number of RD/WR Delay Entries
#define VREF_ADDITIONAL_STEP_SIZE   0x0  ///< Vref Additional Step size
#define RDDQS_ADDITIONAL_STEP_SIZE  0x0  ///< RdDqs Additional Step size
#define MAX_2D_RD_SEED_COUNT          4  ///< Max Seed count
#define MAX_2D_RD_WR_CS_PER_CHANNEL   8  ///< Max CS per Rd Wr delay group

#define EYERIM_PARALLEL_SAMPLING    TRUE
#define EYERIM_BROADCAST_DELAYS     TRUE

/// Structure for RD DQS 2D training RDDQS delays.
typedef struct {
  UINT32  PosRdWrDly;      ///< Positive RdDQS Delay
  UINT32  NegRdWrDly;      ///< Negative RdDQS Delay
} RD_WR_2D;

/// Structure for RD DQS 2D training Vref delays
typedef struct {
  RD_WR_2D *Vref;                             ///< Pointer to Vref Entries
  UINT16 Convolution[MAX_RD_WR_DLY_ENTRIES];  ///< Total number of passes in each mask
  UINT8 PosHeight[MAX_RD_WR_DLY_ENTRIES / 2]; ///< Positive Vref height Height per Delay
  UINT8 NegHeight[MAX_RD_WR_DLY_ENTRIES / 2]; ///< Negative Vref height Height per Delay
  UINT8 HalfDiamondHeight;                    ///< Half of the Height per BL (height of pos/neg vref)
  UINT8 MaxRdWrDly;                           ///< Max RdWrDly from convolution function
} VREF_RD_WR_2D;

/// Structure for RD DQS 2D training Nibbles
typedef struct {
  VREF_RD_WR_2D  Lane[MAX_NUMBER_LANES];                      ///< Bytelane or Nibble
  UINT8 Vnom;                                                 ///< Nominal Vref value
  UINT8 MaxRdWrSweep;                                         ///< Maximum RdDqs or WrDat Sweep size
  UINT8 SmallestPosMaxVrefperCS[MAX_2D_RD_WR_CS_PER_CHANNEL]; ///< Smallest Positive Max Vref per CS
  UINT8 SmallestNegMaxVrefperCS[MAX_2D_RD_WR_CS_PER_CHANNEL]; ///< Smallest Negative Max Vref per CS
  UINT8 DiamondLeft[MAX_2D_VREF_ENTRIES];                     ///< Left edge of Diamond for shape display
  UINT8 DiamondRight[MAX_2D_VREF_ENTRIES];                    ///< Left edge of Diamond for shape display
  UINT8 RdWrDly;                                              ///< RdDQS or WrDat Dly setting
  VOID* SavedData;                                            ///< Algorithm-specific saved data
} MEM_RD_WR_2D_ENTRY;

/// Structure used for RD WR 2D Eye Rim Search Algorithm
typedef struct {
  VREF_RD_WR_2D  LaneSaved[MAX_NUMBER_LANES]; ///< Bytelane or Nibble that was saved
  INT8    xMin;                            ///< Minimum value for RdDqs delays
  INT8    xMax;                            ///< Maximum value for RdDqs delays
  INT8    yMin;                            ///< Minimum value for vref delays
  INT8    yMax;                            ///< Maximum value for vref delays
  BOOLEAN ParallelSampling;                ///< Flag to indicate parallel sampling feature
  BOOLEAN BroadcastDelays;                 ///< Flag to indicate if delays will be broadcast
  UINT32  SampleCount;                     ///< Count of samples taken
  UINT32  VrefUpdates;                     ///< Count of updates to Vref
  UINT32  RdWrDlyUpdates;                  ///< Count of updates to RdWrDly
  INT8    Dirs[2];                         ///< List of directions to search for each axis
} MEM_RD_WR_2D_RIM_ENTRY;

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */
BOOLEAN
MemFAmdRdWr2DTraining (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID* OptParam
  );

BOOLEAN
MemFCheckRdWr2DTrainingPerConfig (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemFRdWr2DProgramIntExtVrefSelect (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemFRdWr2DProgramVref (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *VrefPtr
  );

BOOLEAN
MemFRdWr2DProgramDelays (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *Delay
  );

VOID
MemFRdWr2DStoreResult (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data,
  IN       UINT32 InPhaseResult[],
  IN       UINT32 PhaseResult180[]
  );

BOOLEAN
MemFRdWr2DHeight (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  );

UINT8
MemFGetRdWr2DWidth (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  );

BOOLEAN
MemFCheckRdWr2DDiamondMaskStep (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data,
  IN       UINT8 Vref,
  IN       UINT8 Lane
  );
BOOLEAN
MemFRdWr2DApplyMask (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  );

BOOLEAN
MemFRdWr2DProcessConvolution (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  );

BOOLEAN
MemFRdWr2DProgramMaxDelays (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  );

BOOLEAN
MemFRdWr2DFindCsVrefMargin (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  );

BOOLEAN
MemFRdWr2DFinalVrefMargin (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  );

UINT8
MemFRdWr2DGetMaxLanes (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemFRdWr2DTrainingInit (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemFRdWr2DEyeRimSearch (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *OptParam
  );

VOID
MemFRdWr2DDisplaySearch (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  );

/*******************************
* Pattern Generation Functions
*******************************/
BOOLEAN
MemFRdWr2DCompareInPhase (
  IN OUT   MEM_NB_BLOCK *NBPtr,
     OUT   VOID *Result
  );

BOOLEAN
MemFRdWr2DCompare180Phase (
  IN OUT   MEM_NB_BLOCK *NBPtr,
     OUT   VOID *Result
  );

BOOLEAN
MemFRdWr2DInitVictim (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemFRdWr2DInitVictimChipSel (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemFRdWr2DStartVictim (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *SeedCountPtr
  );

BOOLEAN
MemFRdWr2DFinalizeVictim  (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemFRdWr2DProgramDataPattern (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID* PatternIndexPtr
  );

#endif  /* _MFRDWR2DTRAINING_H_ */
