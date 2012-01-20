/* $NoKeywords:$ */
/**
 * @file
 *
 * mt.h
 *
 * Common Technology
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 54372 $ @e \$Date: 2011-06-07 22:22:22 -0600 (Tue, 07 Jun 2011) $
 *
 **/
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
  * ***************************************************************************
  *
 */

#ifndef _MT_H_
#define _MT_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

#define FIRST_PASS      1
#define SECOND_PASS     2
#define BIGPAGE_X8_RJ16 0x80
#define BIGPAGE_X8      0x800000
#define DQS_FAIL        1
#define DQS_PASS        0
#define DQS_WRITE_DIR    1
#define DQS_READ_DIR     0
#define MIN_DQS_WNDW    3
#define ST_UNSTEADY     0
#define ST_STEADY       1
#define ST_GROSS_SWEEP  2
#define ST_FINE_SWEEP   3
#define ST_FINISH       4
#define NIBBLE_0        0
#define NIBBLE_1        1

#define MAX_BYTELANES_PER_CHANNEL   (8 + 1)  ///< Max Bytelanes per channel

#define MAX_FILTER_DLY_DDR2 0x20
#define MAX_FILTER_DLY_DDR3 0x28

#define NEW_RECEIVER_START_VALUE  0x4
#define NEW_RECEIVER_STEP_1       4
#define NEW_RECEIVER_STEP_2       7

#define NEW_RECEIVER_FINAL_OFFSETVALUE 5

#define MAX_POS_RX_EN_SEED_GROSS_RANGE 0x20     ///< Max Range RxEn Seed Gross
#define MAX_POS_RX_EN_SEED_GROSS_DIR 0x2       ///< Max RxEn Seed Gross Direction

#define DBG_PRINT_STAGE           18                    // "Stage"
#define DBG_PRINT_0_TO_64         23                    // "0...64"
#define DBG_SPACES_4              21                    // 4 spaces
#define DBG_POS_NEW_LINE          11                    // New Line for POS training
#define DBG_WR_DLY                24                    // "Write Delay: "
#define DBG_B_L_R_W_M             22                    // " Bytelane  Left   Right  Width  Middle"
#define DBG_RX_EN_NEW_LINE        25                    // New Line for Rx En
#define DBG_RX_EN_STAGE1          6                     // "Receiver Enable Training Stage 1:"
#define DBG_RX_EN_STAGE2          7                     // "Receiver Enable Training Stage 2:"
#define DBG_RX_EN_STAGE3          8                     // "Receiver Enable Training Stage 3:"
#define DBG_DLY_PER_BL            9                     // "Dly per BL   -"
#define DBG_A_B_DLY               10                    // "ALL BLs have Dly:"
#define DBG_RCVR_PRT_VALUE        0x0010F               // PORT for RX EN training to print a value
#define DBG_RX_POS_PRT_VALUE      0x0011F               // PORT for POS training to print a value

#define DONE_FILTER 0           ///<  optimized receiver enable training glitch search complete
#define START_FILTER 1          ///<  optimized receiver enable training start glitch filter search
#define FILTER_FIRST_STAGE_COUNT 4 ///<  optimized receiver enable training glitch filter first stage count
#define FILTER_SECOND_STAGE_COUNT 7  ///<  optimized receiver enable training glitch second stage count
#define FILTER_OFFSET_VALUE 0x1C ///<  optimized receiver enable training glitch filter offset value int preamble
#define FILTER_WINDOW_SIZE 0x28  ///< optimized receiver enable training glitch filter search window size
#define FILTER_MAX_REC_EN_DLY_VALUE 0x1FF ///< optimized receiver enable glitch filter max receiver value
#define FILTER_NEW_RECEIVER_START_VALUE  0x0    ///< optimized receiver enable glitch filter  Start value
#define MAX_NUMBER_NIBBLES 18      ///< Maximum number of nibbles
#define MAX_NUMBER_LANES 18      ///< Maximum number of lanes (nibbles or bytes)
#define MAX____VREF_ENTRIES  0x20        ///< Maximum number of vref entries
#define MAX_RD_DQS_ENTRIES  0x40         ///< Maximum number of RDDQS Entries
#define VREF_ADDITIONAL_STEP_SIZE  0x0         ///< Vref Additional Step size
#define RDDQS_ADDITIONAL_STEP_SIZE  0x0         ///< RdDqs Additional Step size
#define MAX____DQS_SEED_COUNT 2                      ///< Max Seed count
#define MAX____RDDQS_CS_PER_CHANNEL      8   ///< Max CS per RdDQS delay group


/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
/// List for Technology specific functions that are supported
typedef enum {
  WlTrainingPrepareLrdimm,          ///< Technology specific tasks to prepare LRDIMMs for Training
  LrdimmControlRegInit,             ///< Technology specific tasks to send control words to initialize an LRDIMM
  LrdimmFreqChgCtrlWrd,             ///< Technology specific tasks to send control words to reprogram LRDIMM's register
  LrdimmSendAllMRCmds,              ///< Technology specific tasks to send all MR commands
  LrdimmRankMultiplication,         ///< Determine Rank Multiplication to be used
  LrdimmBuf2DramTrain,              ///< Perform buffer to DRAM training for LRDIMMs
  LrdimmSyncTrainedDlys,            ///< Copy trained delay of the first rank of a QR LRDIMM to the third rank
  LrdimmPresence,                   ///< Perform LRDIMM specific tasks at the time of Dimm Presence Detection

  NumberOfTechHooks                    ///< Size of list
} TECHNOLOGY_SPECIFIC_FUNC_INDEX;

/// Structure for RD DQS training RDDQS delays.
typedef struct {
  UINT32  PosRdDqsDly; ///< Positive RdDQS Delay
  UINT32  NegRdDqsDly; ///< Negative RdDQS Delay
} RD_DQS___;
/// Structure for RD DQS training Vref delays
typedef struct {
  RD_DQS___  *Vref;///< pointer to Vref Entries
  UINT8 Convolution[MAX_RD_DQS_ENTRIES]; ///< Total number of passes in each mask
  UINT8 PosHeight[MAX_RD_DQS_ENTRIES/2]; ///< Positive Vref height Height per Delay
  UINT8 NegHeight[MAX_RD_DQS_ENTRIES/2]; ///< Negative Vref height Height per Delay
  UINT8 HalfDiamondHeight;   ///< Half of the Height per BL (height of pos/neg vref)
  UINT8 MaxRdDqs; ///< Max RdDqs from convolution function
} VREF___;
/// Structure for RD DQS training Nibbles
typedef struct {
  VREF___  Lane[MAX_NUMBER_LANES]; ///< Bytelane or Nibble
  UINT8 Vnom;  // Nominal Vref value
  UINT8 MaxRdDqsSweep; // Maximum RdDqs Sweep size
  UINT8 SmallestPosMaxVrefperCS[MAX____RDDQS_CS_PER_CHANNEL];  // Smallest Positive Max Vref per CS
  UINT8 SmallestNegMaxVrefperCS[MAX____RDDQS_CS_PER_CHANNEL];  // Smallest Negative Max Vref per CS
  UINT8 DiamondLeft[MAX____VREF_ENTRIES];          // Left edge of Diamond for shape display
  UINT8 DiamondRight[MAX____VREF_ENTRIES];         // Left edge of Diamond for shape display
} MEM____ENTRY;

/// Structure for Technology block.
typedef struct _MEM_TECH_BLOCK {
  MEM_NB_BLOCK  *NBPtr; ///< point to northbridge block.
  MEM_PARAMETER_STRUCT *RefPtr; ///< point to parameter list.

  /* Temporary storage */
  UINT32 HwcrLo; ///< value of HWCR.
  UINT32 CR4reg; ///< CR4 register value.
  UINT8  DramEcc; ///< value of Dram ECC bit.
  UINT8  *TestBufPtr; ///< point to buffer to store read-back data.
  UINT8  *PatternBufPtr; ///< point to pattern buffer.
  UINT16 PatternLength; ///< the length of pattern buffer in cache lines.
  UINT8  Direction; ///< direction during training.
  UINT8  ChipSel; ///< chip select number.
  INT8   RestartChipSel;    ///< is used to save the chipsel at which first RdDqsDly retrain is issued
  UINT16 MaxDlyForMaxRdLat; ///< Largest possible value for Receiver enable delay.
  UINT16 PrevSpeed; ///< Previous MemClk frequency
  TRAINING_TYPE  TrainingType;  ///< Type of training currently being done
  UINT8 TargetDIMM; ///< Target DIMM to being trained
  INT16 WLCriticalDelay;  ///< Minimum WL Dly of all byte lanes and all DIMMs
  UINT8 Bytelane; ///< Bytelane being trained
  UINT8 TrnNibble; ///< Nibble being trained


  UINT8  Pass; ///< current pass of training.
  UINT16 DqsRdWrPosSaved;    ///< for position training byte lane saved flag
  UINT16 DqsRcvEnSaved;      ///< for TrainRcvrEn UINT8 lane saved flag
  UINT16 DqsRcvEnSavedS1; ///< for TrainRcvrEn UINT8 lane saved flag
  UINT16 DqsRcvEnFirstPassVal; ///< for TrainRcvrEn UINT8 lane saved flag
  BOOLEAN GetFirstPassVal; ///< If the first passing value has been found.
  BOOLEAN RevertPassVal; ///< Flag to restart training during training process when glitch is found.
  UINT8 MaxFilterDly; ///< Maximum filter delay value for RcvrTraining.
  UINT16 RcvrEnDlyOpt[MAX_BYTELANES_PER_CHANNEL];     ///< Receiver Enable delay for optimized filter
  UINT16 MaxRcvrEnDlyBlOpt[MAX_BYTELANES_PER_CHANNEL]; ///< Max Receiver Enable delay for optimized filter
  UINT16 RcvrEnDlyLimitOpt[MAX_BYTELANES_PER_CHANNEL]; ///< Receiver Enable delay Limit for optimized filter
  UINT16 FilterStatusOpt[MAX_BYTELANES_PER_CHANNEL];  ///< Filter status to indicate if a ByteLane is "DONE", "SKIP" or "CONTINUE"
  UINT16 FilterCountOpt;  ///< Filter count to indicate the total number of ByteLanes completed
  BOOLEAN DqsRcvEnSavedOpt[MAX_BYTELANES_PER_CHANNEL];      ///< for optimized TrainRcvrEn lane saved flag
  UINT16 DqsRcvEnFirstPassValOpt[MAX_BYTELANES_PER_CHANNEL]; ///< for TrainRcvrEn UINT8 lane saved flag for optimized
  BOOLEAN GetFirstPassValOpt[MAX_BYTELANES_PER_CHANNEL]; ///< If the first passing value has been found for optimized.
  BOOLEAN RevertPassValOpt[MAX_BYTELANES_PER_CHANNEL]; ///< Flag to restart training during training process when glitch is found for optimized.
  UINT8 MaxFilterDlyBlOpt[MAX_BYTELANES_PER_CHANNEL]; ///< Maximum filter delay value for RcvrTraining for optimized.
  BOOLEAN IncBy1ForNextCountOpt[MAX_BYTELANES_PER_CHANNEL]; ///< Used to determine when to increment by 1 in second stage of opt. rec. en. training
  UINT8 FilterSidePassCountOpt[MAX_BYTELANES_PER_CHANNEL];  ///< Indicates that previous side passed
  UINT16 DiffSeedGrossSeedPreGross[MAX_BYTELANES_PER_CHANNEL];  ///< Gross difference between GrossSeed and SeedPreGross for HwRxEn Training.
  UINT16 PrevPassRcvEnDly[MAX_BYTELANES_PER_CHANNEL];  ///< Receiver Enable Delay value from the previous pass
  BOOLEAN SmallDqsPosWindow; ///< Status flag to record small DQS position window event
  UINT8 WlNibbleDly[MAX_BYTELANES_PER_CHANNEL];  ///< Nibble based trainig results for Nibble 0 of Write Levelization
  UINT16 WlNibble0Seed[MAX_BYTELANES_PER_CHANNEL];  ///< Nibble based trainig seed value for Nibble 0 Write Levelization
  UINT16 RxEnNibbleDly[MAX_BYTELANES_PER_CHANNEL];  ///< Nibble based trainig results for Nibble 0 of Rx En training
  BOOLEAN ByteLaneError[MAX_BYTELANES_PER_CHANNEL]; ///< Indicates that an error has occured on a bytelane
  UINT16 RxOrig[MAX_BYTELANES_PER_CHANNEL];  ///< Original RxEn Delays for seedless training

  /* PUBLIC functions */
  VOID (*SendAllMRCmds) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 ChipSel); ///< Send MRS command.
  VOID (*FreqChgCtrlWrd) (struct _MEM_TECH_BLOCK *TechPtr); ///< Frequency change control word.
  BOOLEAN (*SetDramMode) (struct _MEM_TECH_BLOCK *TechPtr); ///< Set dram mode (DDR2 or DDR3).
  BOOLEAN (*DimmPresence) (struct _MEM_TECH_BLOCK *TechPtr); ///< determines if DIMMs present.
  BOOLEAN (*SpdCalcWidth) (struct _MEM_TECH_BLOCK *TechPtr); ///< check the symmetry of DIMM pairs.
  BOOLEAN (*SpdGetTargetSpeed) (struct _MEM_TECH_BLOCK *TechPtr); ///< get supported frequency.
  BOOLEAN (*AutoCycTiming) (struct _MEM_TECH_BLOCK *TechPtr); ///< configure timing based on spd data.
  BOOLEAN (*SpdSetBanks) (struct _MEM_TECH_BLOCK *TechPtr); ///< set bank address.
  BOOLEAN (*SetDqsEccTmgs) (struct _MEM_TECH_BLOCK *TechPtr); ///< DQS training.
  VOID (*GetCSIntLvAddr) (UINT8 BankEnc, UINT8 *LowBit, UINT8 *HiBit); ///< Get Chip select interleave address.
  VOID (*AdjustTwrwr) (struct _MEM_TECH_BLOCK *TechPtr); ///< Adjust Twrwr for certain dimm technology.
  VOID (*AdjustTwrrd) (struct _MEM_TECH_BLOCK *TechPtr); ///< Adjust Twrrd for certain dimm technology.
  INT8 (*GetLD) (struct _MEM_TECH_BLOCK *TechPtr); ///< Get LD value for certain dimm technology.
  VOID (*DramInit) (struct _MEM_TECH_BLOCK *TechPtr); ///< dram initialization.

  /* PRIVATE functions */
  VOID (*InitDQSPos4RcvrEn) (struct _MEM_TECH_BLOCK *TechPtr); ///< Initialize training register before training.
  VOID (*SetRcvrEnDly) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Receiver, UINT16 RcvEnDly); ///< Set receiver enable delay register value.
  VOID (*LoadRcvrEnDly) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Receiver); ///< Load receiver enable delay register value.
  BOOLEAN (*SaveRcvrEnDly) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Receiver, UINT16 RcvEnDly, UINT16 cmpResultRank0, UINT16 cmpResultRank1); ///< Save receiver enable delay register value.
  BOOLEAN (*SaveRcvrEnDlyFilter) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Receiver, UINT16 RcvEnDly, UINT16 cmpResultRank0, UINT16 cmpResultRank1); ///< saves passing DqsRcvEnDly values to the stack.
  VOID (*ResetDCTWrPtr) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Receiver); ///< resets the DCT input buffer write pointer.
  UINT16 (*Compare1ClPattern) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Buffer[], UINT8 Pattern[]); ///< Compare training pattern of 1 cache line.
  VOID (*SkipChipSelPass1) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 *ChipSel); ///< skips odd chip select if training at 800MT or above.
  VOID (*SkipChipSelPass2) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 *ChipSel); ///< skips odd chip select if training at 800MT or above.
  UINT16 (*CompareTestPatternFilter) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Buffer[], UINT8 Pattern[], UINT16 ByteCount); ///< compare training pattern with filter.
  UINT8 (*MaxByteLanes) ( VOID ); ///< return maximum number of bytelanes.
  VOID (*SetDQSDelayCSR) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 ByteLane, UINT8 Dly); ///< Set CSR.
  VOID (*DQSWindowSave) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 ByteLane, UINT8 DlyMin, UINT8 DlyMax); ///< programs the trained DQS delay for the specified byte lane and stores its DQS window for reference.
  BOOLEAN (*FindMaxDlyForMaxRdLat) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 *ChipSel); ///< Find maximum receiver enable delay value.
  UINT8 (*DlyTableWidth) ( VOID ); ///< return the width of the delay tables (eg. RcvEnDlys, WrDqsDlys,...) in number of bytes.
  UINT16 (*Compare1ClPatternOpt) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Buffer[], UINT8 Pattern[], UINT8 Side, UINT8 Receiver, BOOLEAN Side1En); ///< Compare training pattern of 1 cache line.
  VOID (*LoadRcvrEnDlyOpt) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Receiver); ///< Load receiver enable delay register value.
  VOID (*SetRcvrEnDlyOpt) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Receiver, UINT16 RcvEnDly); ///< Set receiver enable delay register value.
  BOOLEAN (*CheckRcvrEnDlyLimitOpt) (struct _MEM_TECH_BLOCK *TechPtr); ///< Find limit for all bytelanes
  UINT16 (*GetMaxValueOpt) (struct _MEM_TECH_BLOCK *TechPtr); ///<  Returns the max value of all bytelanes
  VOID (*InitializeVariablesOpt) (struct _MEM_TECH_BLOCK *TechPtr); ///< Initialized variables for optimized training
  BOOLEAN (*SetSweepErrorOpt)(struct _MEM_TECH_BLOCK *TechPtr, UINT8 Receiver, UINT8 DCT, BOOLEAN ErrorCheck); ///< records any errors generated from optimized sweep
  VOID (*LoadInitialRcvrEnDlyOpt) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 Receiver); ///< Load the starting value for receiver DQS training.
  BOOLEAN (*GetDimmSpdBuffer) (struct _MEM_TECH_BLOCK *TechPtr, UINT8 **SpdBuffer, UINT8 Dimm); ///< Gets pointer to spd buffer for a dimm on the current channel, if present
  UINT8 (*GetMinMaxGrossDly) (struct _MEM_TECH_BLOCK *TechPtr, TRN_DLY_TYPE TrnDlyType, BOOLEAN IfMax); ///< Gets the minimum or maximum gross dly value

  /* Technology Specific Hooks */
  BOOLEAN (*(TechnologySpecificHook[NumberOfTechHooks])) (struct _MEM_TECH_BLOCK *TechPtr, VOID *OptParam); ///< Technology specific functions
} MEM_TECH_BLOCK;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

VOID
MemTDimmByteTrainInit (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTTrainMaxLatency (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTSetDQSEccTmgs (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTSetDQSEccTmgsRDdr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTTrainRcvrEnSwPass1 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTTrainDQSEdgeDetectSw (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTTrainDQSEdgeDetect (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTDramInitSw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );
VOID
MemTDramInitHw (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );
BOOLEAN
MemTFeatDef (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );
BOOLEAN
MemTSaveRcvrEnDlyByteFilter (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly,
  IN       UINT16 CmpResultRank0,
  IN       UINT16 CmpResultRank1
  );

BOOLEAN
MemTSaveRcvrEnDlyByteFilterOpt (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly,
  IN       UINT16 CmpResultRank0,
  IN       UINT16 CmpResultRank1
  );

BOOLEAN
MemTNewRevTrainingSupport (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTTrainOptRcvrEnSwPass1 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTWriteLevelizationHw3Pass1 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTWriteLevelizationHw3Pass2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTPreparePhyAssistedTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTExitPhyAssistedTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTDqsTrainRcvrEnHwPass1 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTDqsTrainRcvrEnHwPass2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemRecTSetWrDatRdDqs (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 WrDatDly
  );

VOID
MemRecTTrainDQSPosSw (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemRecTTrainRcvrEnSw (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemRecTTrainRcvrEnHw (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemRecTTrainRcvrEnHwSeedless (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemRecTBeginTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemRecTEndTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTSetSweepErrorOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT8 Dct,
  IN       BOOLEAN ErrorCheck
  );

VOID
MemTInitializeVariablesOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

UINT16
MemTGetMaxValueOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTCheckRcvrEnDlyLimitOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemTMarkTrainFail (
  IN OUT   MEM_TECH_BLOCK *TechPtr
);

VOID
MemTBeginTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemTEndTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemTSetDQSDelayAllCSR (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dly
  );

BOOLEAN
MemTExitPhyAssistedTrainingClient3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTFindMaxRcvrEnDlyRdDqsDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
     OUT   UINT8 *ChipSel
  );

BOOLEAN
MemTFindMaxRcvrEnDlyRdDqsDlyByteUnb (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
     OUT   UINT8 *ChipSel
  );

VOID
MemTSendCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 CmdNum,
  IN       UINT8 Value
  );

VOID
MemTCommonTechInit (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTLrdimmConstructor3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTRdPosWithRxEnDlySeeds3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTTrackRxEnSeedlessRdWrNoWindBLError (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemTTrackRxEnSeedlessRdWrSmallWindBLError (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );
#endif  /* _MT_H_ */
