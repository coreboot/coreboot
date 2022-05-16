/* $NoKeywords:$ */
/**
 * @file
 *
 * mn.h
 *
 * Common Northbridge
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 48803 $ @e \$Date: 2011-03-10 20:18:28 -0700 (Thu, 10 Mar 2011) $
 *
 **/
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

#ifndef _MN_H_
#define _MN_H_

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
#define _4GB_RJ16    ((UINT32)4 << (30 - 16))
#define DCT_ACCESS_WRITE (UINT32) 0x40000000
#define MTRR_VALID  11
#define THERMAL_OPT 31

#define NB_ACCESS       0
#define DCT_PHY_ACCESS  1
#define DCT_EXTRA       2

#define DCT_PHY_DIRECT  0xF1

#define VT_MSK_VALUE 0
#define VT_ARRAY     1
/*---------------------------------------------
 *   TSEFO - Type Start End Function Offset
 *
 *   31:30   Type of access        (2-bits)
 *   29:29   Special               (1-bit)
 *   28:28   Phy Direct            (1-bit)
 *   27:27   Whole Register Access (1-bit)
 *   26:26   Linked                (1-bit)
 *   25:21   Start bit             (5-bits)
 *   20:16   End bit               (5-bits)
 *   15:00   Function_Offset/Index (16-bits)
 *---------------------------------------------
 */
typedef UINT32 TSEFO;

/**
  MAKE_TSEFO(TableName, a, b, c, d, BitFieldIndex):

  @param[in]    TableName
  @param[in]    BitFieldIndex
  @param[in]    a  Type of access.
  @param[in]    b  Index of register (can be in Function_Offset format).
  @param[in]    c  Highest bit of the bit field.
  @param[in]    d  Lowest bit of the bit field.

  @return   TSEFO  Access params encrypted in TSEFO format.
--*/
#define MAKE_TSEFO(TableName, a, b, c, d, BitFieldIndex) \
TableName[BitFieldIndex] = ( \
  (a == DCT_PHY_DIRECT) ? ( \
    (((UINT32) DCT_PHY_ACCESS) << 30) | (((UINT32) 1) << 28) | (((UINT32) b) & 0xFFFF) | (\
      ((c == 15) && (d == 0)) ? ( \
        (((UINT32) 1) << 27) | (((UINT32) b) & 0xF0000) \
      ) : ( \
        (c >= d) ? ( \
          (((UINT32) c) << 21) | (((UINT32) d) << 16) \
        ) : ( \
          (((UINT32) d) << 21) | (((UINT32) c) << 16) \
        ) \
      ) \
    ) \
  ) : ( \
    (((UINT32) a) << 30) | (((UINT32) b) & 0xFFFFFFF) | ( \
      (((UINT32) b) >> 16) ? ( \
        (((UINT32) 1) << 29) \
      ) : ( \
        (c >= d) ? ( \
          (((UINT32) c) << 21) | (((UINT32) d) << 16) \
        ) : ( \
          (((UINT32) d) << 21) | (((UINT32) c) << 16) \
        ) \
      ) \
    ) \
  ) \
)

/**
  LINK_TSEFO(TableName, LowerBitFieldIndex, HigherBitFieldIndex):
  This is one way link: any write to LowerBitFieldIndex would write to HigherBitFieldIndex,
                        but NOT the other way around.
  Requirement: LowerBitFieldIndex must be declared *right* before HigherBitFieldIndex.

  @param[in]    TableName
  @param[in]    LowerBitFieldIndex
  @param[in]    HigherBitFieldIndex

  @return   TSEFO  Access params encrypted in TSEFO format.
--*/
#define LINK_TSEFO(TableName, LowerBitFieldIndex, HigherBitFieldIndex) { \
  ASSERT (LowerBitFieldIndex == (HigherBitFieldIndex - 1)) ; \
  TableName[LowerBitFieldIndex] = TableName[LowerBitFieldIndex] | (((UINT32) 1) << 26); \
}

#define TSEFO_TYPE(x)  ((UINT8) (((UINT32) (x) >> 30) & 0x03))
#define TSEFO_START(x) ((UINT8) (((UINT32) (x) >> 21) & 0x1F))
#define TSEFO_END(x)   ((UINT8) (((UINT32) (x) >> 16) & 0x1F))
#define TSEFO_OFFSET(x) ((UINT32) (x) & 0xFFFF)
#define TSEFO_LINKED(x) ((UINT8) (((UINT32) (x) >> 26) & 0x01))
#define TSEFO_DIRECT_EN(x) ((UINT8) (((UINT32) (x) >> 28) & 0x01))
#define TSEFO_WHOLE_REG_ACCESS(x) ((UINT8) (((UINT32) (x) >> 27) & 0x01))
#define _FN(x, y) (((UINT32) (x) << 12) + (UINT32) (y))
#define _NOT_USED_ 0

/* */
#define B0_DLY   0
#define B1_DLY   1
#define B2_DLY   2
#define B3_DLY   3
#define B4_DLY   4
#define B5_DLY   5
#define B6_DLY   6
#define B7_DLY   7
#define ECC_DLY  8

#define DDR2_TRAIN_FLOW 0
#define DDR3_TRAIN_FLOW 1

//
// Minimum Data Eye width in consecutive 32nds of a UI of
// valid data
//
#define MIN_RD_DATAEYE_WIDTH_NB 4
#define MIN_WR_DATAEYE_WIDTH_NB 4

//
// RELIABLE READ/WRITE MODE DEFINITIONS
//
#define PRECHARGE_ALL_BANKS 0xFF         ///< Use to specify PrechargeAll Command to Precharge Cmd Function
#define CMD_TGT_A           0x00         ///< Issue Commands to Command Target A
#define CMD_TGT_AB          0x01         ///< Issue Commands to Command Targets A and B
#define CMD_TYPE_READ       0x00         ///< Read Command
#define CMD_TYPE_WRITE      0x01         ///< Write Command
#define CMD_TYPE_WR_RD      0x02         ///< Alternating Write and Read Commands
#define CPG_BANK_ADDRESS_A  0x0          ///< Dimm Bank address used in Reliable RD/RW mode training
#define CPG_BANK_ADDRESS_B  0x1          ///< Dimm Bank address used in Reliable RD/RW mode training
#define CPG_ROW_ADDRESS_A   0x0          ///< Dimm Row address used in Reliable RD/RW mode training
#define CPG_ROW_ADDRESS_B   0x0          ///< Dimm Row address used in Reliable RD/RW mode training
#define CPG_COL_ADDRESS_A   0x0          ///< Dimm Column address used in Reliable RD/RW mode training
#define CPG_COL_ADDRESS_B   0x0          ///< Dimm Column address used in Reliable RD/RW mode training
#define CPG_COMPARE_MASK_LOW 0x00000000  ///< Dram DQMask[31:0] used to mask comparison on reads.  1=ignore
#define CPG_COMPARE_MASK_HI  0x00000000  ///< Dram DQMask[63:32] used to mask comparison on reads.  1=ignore
#define CPG_COMPARE_MASK_ECC 0x00        ///< Dram EccMask used to mask comparison on reads.  1=ignore
#define PRBS_SEED_32        0x062221     ///< Data PRBS Seed
#define PRBS_SEED_64        0x066665     ///< Data PRBS Seed
#define PRBS_SEED_128       0x026666     ///< Data PRBS Seed
#define PRBS_SEED_256       0x044443     ///< Data PRBS Seed


/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// Structure for Reliable Read/Write Mode Data
/// These are values that may need to be referenced by the low level functions
/// during training and are initialized at the begining of a particular type of training.
typedef struct _RRW_SETTINGS {
  UINT8 CmdTgt;                          ///< Value to program into CmdTgt
  UINT8 TgtBankAddressA;                 ///< Target A Bank address
  UINT32 TgtRowAddressA;                 ///< Target A Row address
  UINT32 TgtColAddressA;                 ///< Target A Column address
  UINT8 TgtBankAddressB;                 ///< Target B Bank address
  UINT32 TgtRowAddressB;                 ///< Target B Row address
  UINT32 TgtColAddressB;                 ///< Target B Column address
  UINT32 CompareMaskLow;                 ///< Compare Mask Bits 31:0
  UINT32 CompareMaskHigh;                ///< Compare Mask Bits 63:32
  UINT8 CompareMaskEcc;                  ///< Compare Mask Ecc
  UINT32 DataPrbsSeed;                   ///< PRBS Seed value
} RRW_SETTINGS;

/// DQS training related delays
typedef enum {
  AccessRcvEnDly,                       ///< Receiver enable delay
  AccessWrDatDly,                       ///< Write data delay
  AccessRdDqsDly,                       ///< Read DQS delay
  AccessWrDqsDly,                       ///< Write DQS delay
  AccessPhRecDly                        ///< Phase recovery delay
} TRN_DLY_TYPE;

/// Training patterns for position training
typedef enum {
  POS_PATTERN_72B,                      ///< 72 bit pattern
  POS_PATTERN_256B,                     ///< 256 bit pattern
} POS_TRN_PATTERN_TYPE;

/// ODT mode
typedef enum {
  MISSION_MODE,                         ///< ODT during mission mode
  WRITE_LEVELING_MODE                   ///< ODT during write leveling
} ODT_MODE;

/*
 *   DRBN - Dimm-Rank-Byte-Nibble
 *   31:12   Reserved
 *   11:09   Dimm       (3-bits)
 *   08      Rank       (1-bit)
 *   07:05   Reserved
 *   04:01   Byte       (4-bits)
 *   00      Nibble     (1-bit)
 */
typedef UINT32 DRBN;
#define MAKE_DRBN(dimm, rank, byte, nibble) ((((UINT32) (dimm)) << 9) | (((UINT32) (rank)) << 8) | \
(((UINT32) (byte)) << 1) | ((UINT32) (nibble)) )
#define DIMM_BYTE_ACCESS(dimm, byte) ((((UINT32) (dimm)) << 9) | (((UINT32) (byte)) << 1))
#define CS_NBBL_ACCESS(cs, nibble) ((((UINT32) (cs)) << 8) | ((UINT32) (nibble)))
#define DRBN_DIMM(x) ((UINT8) (((UINT32) (x) >> 9) & 0x07))
#define DRBN_RANK(x) ((UINT8) (((UINT32) (x) >> 8) & 0x01))
#define DRBN_BYTE(x) ((UINT8) (((UINT32) (x) >> 1) & 0x0F))
#define DRBN_NBBL(x) ((UINT8) (((UINT32) (x)) & 0x01))

/* Dimm Type mask */
#define DT_X4       0x01
#define DT_X8       0x02
#define DT_X16      0x04
#define DT_SR       0x10
#define DT_DR       0x20
#define DT_QR       0x40

#define DT_ANY_X4   0x71
#define DT_ANY_X8   0x72
#define DT_ANY_X16  0x74
#define DT_ANY_SR   0x17
#define DT_ANY_DR   0x27
#define DT_ANY_QR   0x47
#define DT_ANY_SR_DR    (DT_ANY_SR | DT_ANY_DR)
#define DT_ANY          (DT_ANY_SR | DT_ANY_DR | DT_ANY_QR)

/// Delay Scaling Info Struct - Describes number of delay increments per UI of a delay type
///
typedef struct _TRN_DLY_PARMS {
  UINT8  Min;    ///< Minimum Value
  UINT8  Max;    ///< Maximum Value
  UINT8  Mask;   ///< Mask to be applied (i.e. 0xFF if adjustable by one, 0xFE if adjustable by 2, etc.)
} TRN_DLY_PARMS;

/// Structure for certain data saving needed for DCT.
typedef struct {
  UINT8  RcvEnDlyCounts[8]; ///< DQS Receiver Enable Delay counts
  UINT32 PhRecReg[3];     ///< 3 Phase recovery control registers
} MEM_DCT_CACHE;

/// Structure for table driven support.
typedef struct _MEM_TBL_ALIAS {
  UINT8  time;            ///< Modification time.
  UINT8  node:4;          ///< Node on which to make modification.
  UINT8  dct:4;           ///< DCT on which to make modification.
  UINT8  dimm:4;          ///< Dimm on which to make modification.
  UINT8  attr:3;          ///< Attribute of modification.
  UINT8  vtype:1;         ///< Flag indicating value type.
  UINT32 bfindex;         ///< Bit field index that need to be modified.
  union {                 ///< Union is defined to easy select between single and multiple bytelane cases.
    struct {              ///< Sub-struct used for one bytelane value.
      UINT16 bytelane:16; ///< Bytelane on which to make modification.
      UINT32 value;       ///< Modified value.
      UINT8 reserved[3];  ///< Reserved for this purpose
    } s;                  ///< single value to one or multiple bytelanes
    UINT8 bytelanevalue[9]; ///< Array to specify individual bytelane values
  } data;
} MEM_TABLE_ALIAS;

/// Structure for Platform Specific Block.
typedef struct _MEM_PS_BLOCK {
  UINT8 DramTerm; ///< Dram Term
  UINT8 QR_DramTerm; ///< Dram Term for QR
  UINT8 DynamicDramTerm; ///< Dynamic Dram Term
  UINT8 NumOfReg[MAX_DIMMS_PER_CHANNEL]; ///< Number of registers on each RDIMM (From SPD)
  UINT8 MR0WR;  ///< MR0WR
  UINT8 MR0CL31;  ///< MR0[CL][3:1]
  UINT8 MR0CL0;  ///< MR0CL[0]
  UINT8 RttNom[8];  ///< RttNom value for maximum 8 chipsels per channel
  UINT8 RttWr[8];  ///< RttWr value for maximum 8 chipsels per channel
  UINT8 F0RC8;  ///< F0RC8
  UINT8 F1RC0;  ///< F1RC0
  UINT8 F1RC1;  ///< F1RC1
  UINT8 F1RC2;  ///< F1RC2
  UINT8 RC10OpSpd;  ///< RC10[OperatingSpeed]
  UINT8 LrdimmRowAddrBits[MAX_DIMMS_PER_CHANNEL]; ///< Effective Row address bits used by LRDIMMS
  /* PUBLIC functions */
  BOOLEAN (*MemPDoPs) (struct _MEM_NB_BLOCK *NBPtr); ///< Function that gets Form factor info.
  VOID (*MemPGetPORFreqLimit) (struct _MEM_NB_BLOCK *NBPtr); ///< Function that gets the speed limit of a dimm population.
} MEM_PS_BLOCK;

/// Structure parameters needed in frequency change of client NB.
typedef struct _MEM_FREQ_CHANGE_PARAM {
  UINT16 PllLockTimeDefault; ///< Default PllLockTime
  UINT8  RdPtrInit667orHigher; ///< RdPtrInit for frequency 667MHz and higher
  UINT8  RdPtrInitLower667; ///< RdPtrInit for frequency lower than 667MHz
  UINT8  NclkPeriodMul2x; ///< Multiplier for NclkPeriod in parial sum calculation x 2
  UINT8  MemClkPeriodMul2x; ///< Multiplier for MemClkPeriod in parial sum calculation x 2
  UINT8  SyncTimeMul4x;         ///< Multiplier for SyncTime
  UINT16 TDataProp800orHigher;  ///< TDataProp for frequency 800MHz or higher
  UINT16 TDataPropLower800;     ///< TDataProp for frequency lower than 800MHz
} MEM_FREQ_CHANGE_PARAM;

/// List for NB items that are supported
typedef enum {
  SetSpareEn,                      ///< Sets spare enable
  CheckSpareEn,                    ///< Spare enabled
  SetDllShutDown,                  ///< Sets DllShutDown
  CheckEccDLLPwrDnConfig,          ///< Checks to determine if EccDLLPwrDnConf needs to be adjusted
  DimmBasedOnSpeed,                ///< Checks to determine if Dimm number needs to be adjusted based on speed
  CheckMaxDramRate,                ///< Checks to determine the maximum rate
  Check1GAlign,                    ///< Checks to determine if 1 GB alignment is supported
  DramModeBeforeDimmPres,          ///< Check to determine if DRAM mode needs to be set before dimm presence
  DramModeAfterDimmPres,           ///< Check to determine if DRAM mode needs to be set after dimm presence
  CheckClearOnDimmMirror,          ///< Check to determine if we need to clear on DIMM mirror
  CheckDisDllShutdownSR,           ///< Check to determine if DisDllShutdown needs to be set
  CheckMemClkCSPresent,            ///< Check to determine if chipselect needs to be set based on disabled memclocks
  CheckChangeAvgValue,             ///< Check to determine if we need to change average value
  CheckMaxRdDqsDlyPtr,             ///< Check to determine change Max Rd Dqs Delay
  CheckPhyFenceTraining,           ///< Check to determine if we need to Phy Fence training
  CheckGetMCTSysAddr,              ///< Check to determine if we need to GetMCTSysAddr
  CheckSendAllMRCmds,              ///< Check to determine if we need to SendAllMRCmds
  CheckFindPSOverideWithSocket,    ///< Check to determine if we need to Find PSOveride With Socket
  CheckFindPSDct,                  ///< Check to determine if we need to Find PSOveride With DCT
  CheckODTControls,                ///< Check to determine if we need to set ODT controls
  CheckDummyCLRead,                ///< Check to determine if an extra dummy read is required
  CheckDllStdBy,                   ///< Check to determine if setting DLL stand by is required
  CheckSlewWithMarginImprv,        ///< Check to determine if setting of Slew With MarginImprv is required
  CheckSlewWithoutMarginImprv,     ///< Check to determine if setting of Slew Without MarginImprv is required
  CheckDllSpeedUp,                 ///< Check to determine if setting of Dll SpeedUp is required
  CheckDllRegDis,                  ///< Check to determine if setting of DLL Regulator Disable is required
  FenceTrnBeforeDramInit,          ///< Check to determine if fence training has been done before Dram init
  WLSeedAdjust,                    ///< Check to determine if WL seed needs to be adjusted
  UnifiedNbFence,                  ///< Check to determine if Phy fence is of Unified NB
  AdjustTwr,                       ///< Check to determine if Twr needs to be adjusted
  ChannelPDMode,                   ///< Check to determine if channel power down mode is the only that is supported
  ForceEnMemHoleRemapping,         ///< Check to determine if we need to force enabling memory hole remapping
  AdjustTrdrdSD,                   ///< Check to determine if we need to adjust TrdrdSD
  ReverseMaxRdLatTrain,            ///< Check to determine if reverse (pass to fail) algorithm is supported for MaxRdLat training
  SkipErrTrain,                    ///< Check to determine if skip error training is supported
  DramSrHys,                       ///< Check to determine if DRAM SR hysteresis is supported
  PchgPDMode,                      ///< Check to determine if Precharge powerdown mode is supported
  EccByteTraining,                 ///< Check to determine if DRAM ECC Byte training
  CheckDrvImpCtrl,                 ///< Check to determine if we need to set DrvImpCtrl
  CheckDramTerm,                   ///< Check to determine if we need to set DramTerm
  CheckDramTermDyn,                ///< Check to determine if we need to set DramTermDyn
  CheckQoff,                       ///< Check to determine if we need to set Qoff
  CheckSetSameDctODTsEn,           ///< Check to defermine if we need to set "ODTsEn" the same on each DCT
  WLNegativeDelay,                 ///< Check to determine if the NB can tolerate a negtive WL delay value
  SchedDlySlot1Extra,              ///< Check to determine if DataTxSchedDly Slot1 equation in slowMode to subtract an extra MEMCLK
  TwoStageDramInit,                ///< Check to determine if we need to seperate Draminit into 2 stages. The first one processes info on all nodes. The second one does Dram Init.
  AdjustTrc,                       ///< Check to determine if we need to adjust Trc

  EnumSize                         ///< Size of list
} NB_SUPPORTED;

/// List for family specific functions that are supported
typedef enum {
  BeforePhyFenceTraining,          ///< Family specific tasks before Phy Fence Training
  BeforeMemClkFreqVal,             ///< hook before setting MemClkFreqVal bit
  AfterMemClkFreqVal,              ///< Override PllMult and PllDiv
  OverridePllMult,                 ///< Override PllMult
  OverridePllDiv,                  ///< Override PllDiv
  BeforeMemClr,                    ///< Before MemClr
  SendMrsCmdsPerCs,                ///< Send MRS commands per CS
  SetupHwTrainingEngine,           ///< Setup Hardware training engine for specific training type
  OverrideRcvEnSeed,               ///< Override seed for hardware based RcvEn training
  AddlMaxRdLatTrain,               ///< Perform additional MaxRdLat training if needed
  ForceAutoComp,                   ///< Force Auto Comp
  DetectMemPllError,               ///< Detect MemPll Divide by 3 bug
  ReEnablePhyComp,                 ///< Re-Enable Phy Compensation after RcvEn Training
  ExtractWLODT,                    ///< Extract WL ODT value thr given ODT pattern
  DCTSelectSwitch,                 ///< Select DCT when we switch DCT
  ScrubberErratum,                 ///< Erratum for setting scrubber rate
  MR0_PPD,                         ///< Override MR0[PPD]
  GetDdrMaxRate,                   ///< Interpret DdrMaxRate with Familiy-specific encoding
  ExitPhyAssistedTraining,         ///< Perform family specific tasks when exiting phy assisted training
  AfterSaveRestore,                ///< Action after save/restore execution
  OverrideDataTxFifoWrDly,         ///< Override DataTxFifoWrDly based on training result of WrDatDly
  OverrideRcvEnSeedPassN,          ///< Override seed for hardware based RcvEn training where N greater than 0
  AfterMemClkFreqChg,              ///< Reprogram DIMMs' buffers after MEMCLK frequency change
  AdjustTxpdll,                    ///< Adjust Txpdll value to encoded register value
  CalcWrDqDqsEarly,                ///< Calculate WrDqDqsEarly
  TrainWlPerNibble,                ///< Train Write Leveling per nibble
  TrainWlPerNibbleAdjustWLDly,     ///< Train WL per nibble and adjust the WL delay
  TrainWlPerNibbleSeed,             ///< Save the seed for WL nibble based training
  TrainRxEnPerNibble,              ///< Train Rx Enable Training per nibble
  TrainRxEnAdjustDlyPerNibble,     ///< Train Rx Enable Training nibble and adjust the RxEn delay
  TrainRxEnGetAvgDlyPerNibble,     ///< Display Rx Enable Training average nibble value for each BL
  InitPerNibbleTrn,                ///< Initiates Per Nibble Training.
  BeforeSetCsTri,                  ///< Modify CS tri-state bit map.
  ForceRdDqsPhaseB,                ///< Force RdDqsDly to phase B
  SetDqsODT,                       ///< Set DQS ODT
  DisLowPwrDrvStr,                 ///< Hook to skip setting LowPowerDriveStrengthEn
  AdjustRdDqsDlyOffset,            ///< Adjust the bit offset of the RdDqsDly Bit Bitfield before writing and after reading
  ResetRxFifoPtr,                  ///< Reset RxFifo pointer during Read DQS training
  EnableParityAfterMemRst,         ///< Enable DRAM Address Parity after memory reset.
  FinalizeVDDIO,                   ///< Finalize VDDIO
  BfAfExcludeDimm,                 ///< Workaround before and after excluding dimms

  NumberOfHooks                    ///< Size of list
} FAMILY_SPECIFIC_FUNC_INDEX;

///< Entry for SPD Timing
typedef struct {
  BIT_FIELD_NAME BitField;  ///< Bit field name of the timing
  UINT8 Min; ///< Minimum value for timing
  UINT8 Max; ///< Maximum value for timing
  UINT8 Bias; ///< Bias from actual value
  UINT8 Ratio_x2; ///< Actual value will be multiplied by (Ratio_x2/2)
} CTENTRY;

/// Structure for northbridge block.
typedef struct _MEM_NB_BLOCK {
  MEM_DATA_STRUCT *MemPtr; ///< Point to MEM_DATA_STRUCT.
  MEM_PARAMETER_STRUCT *RefPtr; ///< Point to MEM_PARAMETER_STRUCT.
  DIE_STRUCT *MCTPtr;             ///< point to current Node's MCT struct
  DCT_STRUCT *DCTPtr;             ///< point to current Node's DCT struct
  DCT_STRUCT *AllDCTPtr;          ///< point to all Node's DCT structs
  CH_DEF_STRUCT *ChannelPtr;       ///< point to current channel data
  SPD_DEF_STRUCT *SPDPtr; ///< Point to SPD data for current DCT.
  struct _MEM_TECH_BLOCK *TechPtr; ///< point to technology block.
  CONST struct _MEM_FEAT_BLOCK_NB *FeatPtr; ///< point to NB Specific feature block.
  struct _MEM_SHARED_DATA *SharedPtr;     ///< Pointer to Memory scratchpad area
  SPD_DEF_STRUCT *AllNodeSPDPtr; ///< Point to SPD data for the system.
  DIE_STRUCT *AllNodeMCTPtr;      ///< point to all Node's MCT structs
  UINT8 DimmToBeUsed; ///< Dimm to be used in recovery mode.
  MEM_PS_BLOCK *PsPtr; ///< point to platform specific block
  MEM_PS_BLOCK *PSBlock; ///< point to the first platform specific block on this node.
  MEM_FREQ_CHANGE_PARAM *FreqChangeParam; ///< pointer to parameter of frequency change.

  PCI_ADDR PciAddr;                ///< PCI address for this node
  TSEFO NBRegTable[BFEndOfList];         ///< contains all bit field definitions

  UINT8 Node; ///< current node.
  UINT8 Dct; ///< current DCT.
  UINT8 Channel; ///< current channel.
  UINT8 DctCount; ///< number of DCTs on the current NB.
  UINT8 ChannelCount; ///< number of channels per DCT of the current NB.
  UINT8 NodeCount; ///< number of Nodes supported
  BOOLEAN Ganged; ///< mode for current MCT controller.
  POS_TRN_PATTERN_TYPE PosTrnPattern; ///< specifies the pattern that should be used for position training.
  BOOLEAN MemCleared; ///< memory clear flag.
  UINT32  CPGInit; ///< continuous pattern generation flag.
  UINT16  StartupSpeed; ///< startup speed for DDR3.
  UINT16  RcvrEnDlyLimit; ///< maximum value that RcvrEnDly field can take.
  UINT32 McaNbCtlReg; ///< reserve MCA reports.
  UINT32 VarMtrrHiMsk;   ///< variable MTRR mask for upper 32 bits.
  UINT32 NBClkFreq; ///< Current NB Clock frequency
  UINT8  DefDctSelIntLvAddr; ///< Default DctSelIntLvAddr
  UINT8  TrainingSequenceIndex; ///< Index into the Training Sequence
  RRW_SETTINGS RrwSettings;  ///<Settings for Reliable Read/Write mode

  UINT16 MaxFreqVDDIO[VOLT1_25 + 1]; ///< Max Frequency each voltage supports.

  MEM_DCT_CACHE DctCache[MAX_CHANNELS_PER_SOCKET]; ///< Allocate space for MCT_DCT_CACHE.
  MEM_DCT_CACHE *DctCachePtr;    ///< pointer to current Node's Node struct

  /* Temporary storage */
  BOOLEAN   ClToNbFlag;        ///< is used to restore ClLinesToNbDis bit after memory
  UINT8     NbFreqChgState;    ///< is used as a state index in NB frequency change state machine
  UINT32    NbPsCtlReg;        ///< is used to save/restore NB Pstate control register
  CONST UINT32 *RecModeDefRegArray;   ///< points to an array of default register values that are set for recovery mode

  ///< Determines if code should be executed on a give NB
  BOOLEAN IsSupported[EnumSize];
  BOOLEAN (*FamilySpecificHook[NumberOfHooks]) (struct _MEM_NB_BLOCK *NBPtr, VOID *OptParam);   ///< This array of pointers point to
                                                                                                ///< family specific functions.

  /* PUBLIC functions */
  VOID (*SwitchDCT) (struct _MEM_NB_BLOCK *NBPtr, UINT8 DCT); ///< Switch to current DCT.
  VOID (*SwitchChannel) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Channel); ///< Switch to current channel.
  VOID (*SetMaxLatency) (struct _MEM_NB_BLOCK *NBPtr, UINT16 MaxRcvEnDly); ///< Set Max Rd Latency.
  VOID (*getMaxLatParams) (struct _MEM_NB_BLOCK *NBPtr, UINT16 MaxDlyForMaxRdLat, UINT16 *MinDly, UINT16 *MaxDly, UINT16 *DlyBias); ///< retrieves the Max latency parameters.
  BOOLEAN (*GetSysAddr) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Receiver, UINT32 *Addr); ///< Get system address for training dimm.
  BOOLEAN (*RankEnabled) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Receiver); ///< Check if a rank has been enabled.
  BOOLEAN (*InitializeMCT) (struct _MEM_NB_BLOCK *NBPtr); ///< MCT initialization.
  BOOLEAN (*FinalizeMCT) (struct _MEM_NB_BLOCK *NBPtr); ///< sets final values in BUCFG and BUCFG2.
  BOOLEAN (*InitMCT) (struct _MEM_NB_BLOCK *NBPtr); ///< main entry call for memory initialization.
  VOID (*SendMrsCmd) (struct _MEM_NB_BLOCK *NBPtr); ///< send MRS command.
  VOID (*sendZQCmd) (struct _MEM_NB_BLOCK *NBPtr); ///< send ZQ command.
  VOID (*TrainingFlow) (struct _MEM_NB_BLOCK *NBPtr); ///< Set the training flow control
  VOID (*WritePattern) (struct _MEM_NB_BLOCK *NBPtr, UINT32 Address, UINT8 Pattern[], UINT16 ClCount); ///< Write training pattern.
  VOID (*ReadPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT32 Address, UINT16 ClCount); ///< Read training pattern.
  VOID (*GenHwRcvEnReads) (struct _MEM_NB_BLOCK *NBPtr, UINT32 Address);  ///< generates a continuous burst of reads during HW RcvEn training.
  UINT16 (*CompareTestPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT8 Pattern[], UINT16 ByteCount); ///< Compare training pattern.
  UINT16 (*InsDlyCompareTestPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT8 Pattern[], UINT16 ByteCount); ///< Compare training pattern using 1 beat offset to check for insertion delay
  BOOLEAN (*StitchMemory) (struct _MEM_NB_BLOCK *NBPtr); ///< combines all the memory into a contiguous map.
  VOID (*ProgramCycTimings) (struct _MEM_NB_BLOCK *NBPtr); ///< programs the memory controller with SPD timings.
  BOOLEAN (*AutoConfig) (struct _MEM_NB_BLOCK *NBPtr); ///< programs the memory controller with configuration parameters
  BOOLEAN (*PlatformSpec) (struct _MEM_NB_BLOCK *NBPtr); ///< programs platform specific parameters.
  VOID (*DisableDCT) (struct _MEM_NB_BLOCK *NBPtr); ///< disable a DCT if no dimm presents.
  VOID (*StartupDCT) (struct _MEM_NB_BLOCK *NBPtr); ///< start a DCT.
  VOID (*SyncTargetSpeed) (struct _MEM_NB_BLOCK *NBPtr); ///< Check and sync the target speed of all channels of this node.
  VOID (*ChangeFrequency) (struct _MEM_NB_BLOCK *NBPtr); ///< Frequency change sequence.
  BOOLEAN (*RampUpFrequency) (struct _MEM_NB_BLOCK *NBPtr); ///< Change frequency to the next supported level.
  BOOLEAN (*ChangeNbFrequency) (struct _MEM_NB_BLOCK *NBPtr); ///< Change NB frequency.
  VOID (*PhyFenceTraining) (struct _MEM_NB_BLOCK *NBPtr); ///< Phy fence training.
  BOOLEAN (*SyncDctsReady) (struct _MEM_NB_BLOCK *NBPtr); ///< Synchronize DCTs.
  BOOLEAN (*HtMemMapInit) (struct _MEM_NB_BLOCK *NBPtr); ///< Memory map initialization.
  VOID (*SyncAddrMapToAllNodes) (struct _MEM_NB_BLOCK *NBPtr); ///< copies the Node 0 map to all the other nodes.
  BOOLEAN (*CpuMemTyping) (struct _MEM_NB_BLOCK *NBPtr); ///< MTRR and TOM setting.
  VOID (*BeforeDqsTraining) (struct _MEM_NB_BLOCK *NBPtr); ///< processes needed before DQS training.
  VOID (*AfterDqsTraining) (struct _MEM_NB_BLOCK *NBPtr); ///< processes needed after DQS training.
  BOOLEAN (*OtherTiming) (struct _MEM_NB_BLOCK *NBPtr); ///< setting non-spd timing.
  VOID (*UMAMemTyping) (struct _MEM_NB_BLOCK *NBPtr); ///< MTRR and TOM setting needed for UMA platform.
  VOID (*Feature) (struct _MEM_NB_BLOCK *NBPtr); ///< Feature support.
  UINT8 (*GetSocketRelativeChannel) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Dct, UINT8 Channel); ///< Get channel number relative to a socket.
  VOID (*SetDramOdtRec) (struct _MEM_NB_BLOCK *NBPtr, ODT_MODE OdtMode, UINT8 ChipSelect, UINT8 TargetCS); ///< Set Dram ODT.
  UINT32 (*GetSysAddrRec) (VOID); ///< Get system address for training.
  VOID (*SwitchNodeRec) (struct _MEM_NB_BLOCK *NBPtr, UINT8 NodeID); ///< Switch to current node.
  VOID (*TechBlockSwitch) (struct _MEM_NB_BLOCK *NBPtr); ///< Selects appropriate Tech functions for the NB.
  VOID (*SetEccSymbolSize) (struct _MEM_NB_BLOCK *NBPtr); ///< Set Ecc Symbol Size.
  VOID (*GetTrainDlyParms) (struct _MEM_NB_BLOCK *NBPtr, TRN_DLY_TYPE TrnDly, TRN_DLY_PARMS *Parms); ///< Retrieve Specific Delay range info for current NB under current conditions.
  AGESA_STATUS (*TrainingPatternInit) (struct _MEM_NB_BLOCK *NBPtr); ///< Initialize the training Pattern
  AGESA_STATUS (*TrainingPatternFinalize) (struct _MEM_NB_BLOCK *NBPtr); ///< Finalize the training Pattern
  BOOLEAN (*GetApproximateWriteDatDelay) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Index, UINT8 *Value); ///< Retrieve the next WrDat Delay Approximation
  UINT8 (*CSPerChannel) (struct _MEM_NB_BLOCK *NBPtr); ///< Return number of CS per channel.
  UINT8 (*CSPerDelay) (struct _MEM_NB_BLOCK *NBPtr); ///< Return number of CS controlled per set of delay registers.
  VOID (*FlushPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT32 Address, UINT16 ClCount);  ///<Flush the training pattern
  UINT8 (*MinDataEyeWidth) (struct _MEM_NB_BLOCK *NBPtr);  ///<Get Min Data Eye Width in 32nds of a UI
  VOID (*MemNCapSpeedBatteryLife) (struct _MEM_NB_BLOCK *NBPtr); ///< Caps speed based on battery life check.
  UINT32 (*GetUmaSize) (struct _MEM_NB_BLOCK *NBPtr); ///< Get Uma Size
  UINT8 (*GetMemClkFreqId) (struct _MEM_NB_BLOCK *NBPtr, UINT16 Speed); ///< Translate MemClk frequency in MHz to MemClkFreq value
  VOID (*EnableSwapIntlvRgn) (struct _MEM_NB_BLOCK *NBPtr, UINT32 Base, UINT32 Limit); ///< Enable swapped interleaving region
  BOOLEAN (*ChangeNbFrequencyWrap) (struct _MEM_NB_BLOCK *NBPr, UINT32 NBPstate); ///< Wrapper for NB Pstate change function
  VOID (*WaitXMemClks) (struct _MEM_NB_BLOCK *NBPr, UINT32 MemClkCount); ///< Waits a specified number of MemClks
  VOID (*ProgramNbPsDependentRegs) (struct _MEM_NB_BLOCK *NBPtr); ///< Programs NB Pstate dependent registers
  VOID (*AllocateC6Storage) (struct _MEM_NB_BLOCK *NBPtr); ///< Allocates DRAM region for Core C6

  /* PUBLIC Get/Set register field functions */
  UINT32 (*GetBitField) (struct _MEM_NB_BLOCK *NBPtr, BIT_FIELD_NAME FieldName); ///< Pci register bit field read.
  VOID (*SetBitField) (struct _MEM_NB_BLOCK *NBPtr, BIT_FIELD_NAME FieldName, UINT32 Value); ///< Pci register bit field write.
  BOOLEAN (*BrdcstCheck) (struct _MEM_NB_BLOCK *NBPtr, BIT_FIELD_NAME FieldName, UINT32 Value); ///< Pci register bit field broadcast read.
  VOID (*BrdcstSet) (struct _MEM_NB_BLOCK *NBPtr, BIT_FIELD_NAME FieldName, UINT32 Value); ///< Pci register bit field broadcast write.
  VOID (*PollBitField) (struct _MEM_NB_BLOCK *NBPtr, BIT_FIELD_NAME FieldName, UINT32 Field, UINT32 MicroSecond, BOOLEAN IfBroadCast); ///< Poll a Pci register bitfield.
  UINT32 (*GetTrainDly) (struct _MEM_NB_BLOCK *NBPtr, TRN_DLY_TYPE TrnDly, DRBN DrbnVar); ///< Training register bit field read.
  VOID (*SetTrainDly) (struct _MEM_NB_BLOCK *NBPtr, TRN_DLY_TYPE TrnDly, DRBN DrbnVar, UINT16 Value); ///< Training register bit field write.
  AGESA_STATUS (*InitRecovery) (struct _MEM_NB_BLOCK *NBPtr); ///< Recover mode memory init
  VOID (*MemRecNInitializeMctNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Initialize MCT changes
  VOID (*MemRecNFinalizeMctNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Finalize MCT changes
  VOID (*MemNInitPhyComp) (struct _MEM_NB_BLOCK *NBPtr); ///< Init Phy compensation
  VOID (*MemNBeforeDramInitNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Before Dram init
  BOOLEAN (*MemNIsIdSupportedNb) (struct _MEM_NB_BLOCK *NBPtr, CPU_LOGICAL_ID *LogicalIdPtr); ///< Determines if a given CPU id is supported
  BOOLEAN (*InitNBRegTableNb) (struct _MEM_NB_BLOCK *NBPtr, TSEFO NBRegTable[]); ///< Initializes register table
  BOOLEAN (*MemNPlatformSpecificFormFactorInitNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Platform specific functions
  VOID (*MemNSetOtherTimingNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Set non-spd timings
  VOID (*MemNBeforePlatformSpecNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Apply settings prior to platform specific settings
  UINT32 (*MemNCmnGetSetFieldNb) (struct _MEM_NB_BLOCK *NBPtr, UINT8 IsSet, BIT_FIELD_NAME FieldName, UINT32 Field); ///< Sets a register value
  UINT32 (*MemNcmnGetSetTrainDly) (struct _MEM_NB_BLOCK *NBPtr, UINT8 IsSet, TRN_DLY_TYPE TrnDly, DRBN DrbnVar, UINT16 Field); ///< Sets a training delay field
  VOID (*MemPPhyFenceTrainingNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Phy Fence training
  VOID (*MemPNodeMemBoundaryNb) (struct _MEM_NB_BLOCK *NBPtr, UINT32 *NodeSysLimit); ///< Phy Fence training
  UINT32 (*MemRecNCmnGetSetFieldNb) (struct _MEM_NB_BLOCK *NBPtr, UINT8 IsSet, BIT_FIELD_NAME FieldName, UINT32 Field); ///< This functions sets bit fields in recover mode
  UINT32 (*MemRecNcmnGetSetTrainDlyNb) (struct _MEM_NB_BLOCK *NBPtr, UINT8 IsSet, TRN_DLY_TYPE TrnDly, DRBN DrbnVar, UINT16 Field); ///< This functions sets bit fields in recover mode
  VOID (*MemRecNSwitchDctNb) (struct _MEM_NB_BLOCK *NBPtr, UINT8 NodeID); ///< S3 Exit self refresh register
  VOID (*MemNPFenceAdjustNb) (struct _MEM_NB_BLOCK *NBPtr, UINT16 *Value16); ///< Adjust Avg PRE value of Phy fence training
  VOID (*MemNPrepareRcvrEnDlySeed) (struct _MEM_NB_BLOCK *NBPtr); ///< Seed valude for HW RxEn training
  UINT8 (*MemNGetDramTerm) (struct _MEM_NB_BLOCK *NBPtr, UINT8 ChipSel); ///< Dram Term value
  UINT8 (*MemNGetDynDramTerm) (struct _MEM_NB_BLOCK *NBPtr, UINT8 ChipSel); ///< Dynamic Dram Term value
  UINT32 (*MemNGetMR0CL) (struct _MEM_NB_BLOCK *NBPtr); ///< MR0[CL] value
  UINT32 (*MemNGetMR0WR) (struct _MEM_NB_BLOCK *NBPtr); ///< MR0[WR] value
  UINT32 (*MemNGetMR2CWL) (struct _MEM_NB_BLOCK *NBPtr); ///< MR2[CWL] value

} MEM_NB_BLOCK;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
VOID
MemNInitNBDataNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSwitchDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct
  );

VOID
MemNSwitchChannelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Channel
  );

UINT32
MemNGetBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName
  );

VOID
MemNSetBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

BOOLEAN
MemNBrdcstCheckNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

VOID
MemNBrdcstSetNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );


UINT32
MemNGetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar
  );

VOID
MemNSetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

BOOLEAN
MemNRankEnabledNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Receiver
  );

UINT8 MemNGetSocketRelativeChannelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct,
  IN       UINT8 Channel
  );

VOID
MemNPhyFenceTrainingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNGetMCTSysAddrNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Receiver,
  OUT      UINT32 *AddrPtr
  );

BOOLEAN
MemNPlatformSpecNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNStitchMemoryNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNDisableDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNDisableDCTClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNStartupDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNStartupDCTUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNChangeFrequencyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNRampUpFrequencyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramCycTimingsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNGetMaxLatParamsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  );

UINT16
MemNTotalSyncComponentsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSetMaxLatencyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  );

VOID
MemNSendZQCmdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSwapBitsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSwapBitsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNTrainPhyFenceNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitPhyCompNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNGetTrainDlyParmsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN OUT   TRN_DLY_PARMS *Parms
  );


VOID
MemNGetTrainDlyParmsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN OUT   TRN_DLY_PARMS *Parms
  );

VOID
MemNBeforeDQSTrainingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNcmnGetSetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

UINT32
MemNcmnGetSetTrainDlyClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

VOID
MemNSyncTargetSpeedNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNSyncDctsReadyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNHtMemMapInitNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNGetTrdrdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNGetTwrwrNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNGetTwrrdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNGetTrwtTONb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNGetTrwtWBNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPowerDownCtlNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNCPUMemTypingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNUMAMemTypingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSyncAddrMapToAllNodesNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNInitMCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNTechBlockSwitchNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemRecNGetBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName
  );

VOID
MemRecNSetBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

UINT32
MemRecNGetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar
  );

VOID
MemRecNSetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

BOOLEAN
MemRecNAutoConfigNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemRecNPlatformSpecNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemRecNStartupDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemRecNSetMaxLatencyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  );

VOID
MemRecNSetDramOdtNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       ODT_MODE OdtMode,
  IN       UINT8 ChipSelect,
  IN       UINT8 TargetCS
  );

VOID
MemRecNSendMrsCmdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );


VOID
MemRecNSendZQCmdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemRecNContReadPatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

AGESA_STATUS
MemRecNMemInitNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemRecNCPUMemRecTypingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemRecNGetMCTSysAddrNb (
  VOID
  );

UINT32
MemRecGetVarMtrrHiMsk (
  IN       CPU_LOGICAL_ID *LogicalIdPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

INT8
MemNGetOptimalCGDDNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly1,
  IN       TRN_DLY_TYPE TrnDly2
  );

VOID
MemNPollBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field,
  IN       UINT32 MicroSecond,
  IN       BOOLEAN IfBroadCast
  );

VOID
MemNSetEccSymbolSizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNDQSTiming3Nb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNDQSTiming2Nb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNTrainingFlowNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNRecTrainingFlowNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNRecTrainingFlowClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemRecNTotalSyncComponentsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemRecNStartupDCTClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );


VOID
MemRecNPhyVoltageLevelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemRecNPhyFenceTrainingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

AGESA_STATUS
MemNTrainingPatternInitNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNGetApproximateWriteDatDelayNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Index,
  IN OUT   UINT8 *Value
  );

AGESA_STATUS
MemNTrainingPatternFinalizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNFlushPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

UINT8
MemNCSPerChannelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNCSPerDelayNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNMinDataEyeWidthNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT16
MemNCompareTestPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

UINT16
MemNInsDlyCompareTestPatternNb (
  IN       MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );


UINT32
MemNGetUmaSizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNSetMTRRUmaRegionUCNb  (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 *BasePtr,
  IN OUT   UINT32 *LimitPtr
  );

UINT8
MemNGetMemClkFreqIdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 Speed
  );

UINT8
MemNGetMemClkFreqIdClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 Speed
  );

UINT8
MemNGetMemClkFreqIdUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 Speed
  );

UINT16
MemNGetMemClkFreqUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 FreqId
  );

BOOLEAN
MemNGetPlatformCfgNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNPlatformSpecUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramPlatformSpecNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramCycTimingsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

INT16
MemNCalcCDDNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDlyType1,
  IN       TRN_DLY_TYPE TrnDlyType2,
  IN       BOOLEAN SameDimm,
  IN       BOOLEAN DiffDimm
  );

VOID
MemNChangeFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramNbPstateDependentRegistersClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNAllocateC6StorageClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNAllocateC6StorageUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPhyVoltageLevelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPhyFenceTrainingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPFenceAdjustUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   UINT16 *Value16
  );

VOID
MemNInitPhyCompClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNTotalSyncComponentsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNEnableSwapIntlvRgnNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN          UINT32 Base,
  IN          UINT32 Limit
  );

VOID
MemNPhyPowerSavingClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSetASRSRTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPrepareRcvrEnDlySeedNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNChangeNbFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNChangeNbFrequencyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWaitXMemClksNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr,
  IN       UINT32 MemClkCount
  );

BOOLEAN
memNSequenceDDR3Nb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT16
GetTrainDlyFromHeapNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDlyType,
  IN       DRBN Drbn
  );

BOOLEAN
MemNTrainingFlowUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNSetupHwTrainingEngineUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID* OptParam
  );

BOOLEAN
MemNBeforePhyFenceTrainingClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNReEnablePhyCompNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

UINT8
MemNGetDramTermNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  );

UINT8
MemNGetDynDramTermNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  );

UINT32
MemNGetMR0CLNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNGetMR0WRNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNGetMR2CWLNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNDctCfgSelectUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *Dct
  );

BOOLEAN
MemNGetMaxDdrRateUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *DdrMaxRate
  );

BOOLEAN
MemRecNReEnablePhyCompNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

UINT32
MemRecNcmnGetSetTrainDlyClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

VOID
MemNSetTxpNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAdjustTxpdllClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNCalcWrDqDqsEarlyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNGetTrainDlyParmsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN OUT   TRN_DLY_PARMS *Parms
  );

BOOLEAN
MemNBfAfExcludeDimmClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *IsBefore
  );

#endif  /* _MN_H_ */
