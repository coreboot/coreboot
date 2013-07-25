/* $NoKeywords:$ */
/**
 * @file
 *
 * mnpmu.h
 *
 * Definitions for PMU support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
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

#ifndef _MNPMU_H_
#define _MNPMU_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/// PMU Firmware Set
/// PMU firmware can be fragmented. This structure is a set of all fragmented firmware for one run.
typedef struct {
  UINT16  SramBaseAddr;     ///< SRAM base address
  UINT16  *ImagePtr;        ///< Pointer to firmware image
  UINT16  ImageSize;        ///< Size of firmware image
} PMU_FIRMWARE_SET;

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

//
// Upstream mailbox 1 messages
//
#define USMSG_DEVINIT      0x00  ///< PMU has completed DevInit
#define USMSG_TRAINWRLVL   0x01  ///< PMU has completed TSTAGE_WrLvl
#define USMSG_TRAINRXEN    0x02  ///< PMU has completed TSTAGE_RxEn
#define USMSG_TRAINRDDQS1D 0x03  ///< PMU has completed TSTAGE_TrainRdDqs1D
#define USMSG_TRAINWRDQ1D  0x04  ///< PMU has completed TSTAGE_TrainWrDq1D
#define USMSG_TRAINRD2D    0x05  ///< PMU has completed TSTAGE_TrainRd2D
#define USMSG_TRAINWR2D    0x06  ///< PMU has completed TSTAGE_TrainWr2D
#define USMSG_PMUQEMPTY    0x07  ///< PMU has completed all of its SequenceCtl tasks and is in a powergated idle state
#define USMSG_US2MSGRDY    0x08  ///< PMU is ready to stream a message through US mailbox 2
#define USMSG_FAIL         0xFF  ///< PMU has encountered an error which requires requester to abort waiting for remaining pending upstream messages

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
/// Fields on PMU SRAM
typedef enum {
  PmuSRAMRangeStart = RegIdLimit, ///< -------------------------- Start of PMU SRAM range

  // DDR3 SRAM message block
  PmuRevision,                    ///< Revision         Table revision
  PmuCpuId,                       ///< CpuId            32-bit CPUID
  PmuCpuIdHi,                     ///< CpuId            32-bit CPUID
  PmuDramType,                    ///< DramType
  PmuModuleType,                  ///< ModuleType
  PmuRawCard0,                    ///< RawCard0
  PmuRawCard1,                    ///< RawCard1
  PmuRawCard2,                    ///< RawCard2
  PmuRawCard3,                    ///< RawCard3
  PmuChipSelect,                  ///< ChipSelect       Specifies which chipselects are present and should be trained. Bit 0 is chip select 0, Bit 1 is
                                  ///<                  chip,select 1, etc. IF Gddr5Mode then ChipSelect[7:1] are ignored.
  PmuAddrMirror,                  ///< AddrMirror       For each chipselect, specifies which chipselects have address mirroring (BA0 swapped with BA1,
                                  ///<                  A3,swapped with A4, A5 swapped with A6, A7 swapped with A8). This field is used in Ddr3Mode.
  PmuDimm0Cols,                   ///< Dimm0Cols        Number of device column address bits for Dimm0.
  PmuDimm1Cols,                   ///< Dimm1Cols        Number of device column address bits for Dimm1.
  PmuDimm2Cols,                   ///< Dimm2Cols        Number of device column address bits for Dimm2.
  PmuDimm3Cols,                   ///< Dimm3Cols        Number of device column address bits for Dimm3.
  PmuDimm0Banks,                  ///< Dimm0Banks       Number of device bank address bits for Dimm0.
  PmuDimm1Banks,                  ///< Dimm1Banks       Number of device bank address bits for Dimm1.
  PmuDimm2Banks,                  ///< Dimm2Banks       Number of device bank address bits for Dimm2.
  PmuDimm3Banks,                  ///< Dimm3Banks       Number of device bank address bits for Dimm3.
  PmuDimm0Rows,                   ///< Dimm0Rows        Number of device row address bits for Dimm0.
  PmuDimm1Rows,                   ///< Dimm1Rows        Number of device row address bits for Dimm1.
  PmuDimm2Rows,                   ///< Dimm2Rows        Number of device row address bits for Dimm2.
  PmuDimm3Rows,                   ///< Dimm3Rows        Number of device row address bits for Dimm3.
  PmuPerRankTiming,               ///< PerRankTiming    1=The channel is configured to use four timing sources for four independent chipselects. 0=The
                                  ///<                  channel,is configured to use four timing sources for four pairs of chipselects (logical DIMMs).
  PmuCurrentTemp,                 ///< CurrentTemp      11-bit temperature in degrees C, with -49C offset, in 0.125C increments, unsigned. See
                                  ///<                  D18F5xA8_x383[GblMaxTemp] in,TN BKDG
  PmuSequenceCtl,                 ///< SequenceCtl      PMU Sequence Control Word
                                  ///<                  [0] 1=DevInit
                                  ///<                  [1] 1=RxEnDly,Training
                                  ///<                  [2] 1=1D,Rd-Dqs Training
                                  ///<                  [3] 1=1D,Wr-Dq Training
                                  ///<                  [4] 1=2D,Read Training
                                  ///<                  [5] 1=2D,Write Training
                                  ///<                  [15:6] Reserved
  PmuHdtCtl,                      ///< HdtCtl
  PmuCkeSetup,                    ///< CkeSetup
  PmuCsOdtSetup,                  ///< CsOdtSetup
  PmuAddrCmdSetup,                ///< AddrCmdSetup
  PmuSlowAccessMode,              ///< SlowAccessMode   SlowAccessMode
  PmutRP,                         ///< tRP              In units of memclocks. See D18F2x200_dct[3:0]_mp[1:0][Trp] for definition.
  PmutMRD,                        ///< tMRD             In units of memclocks. See D18F2x220_dct[3:0][Tmrd] for definition.
  PmutRFC,                        ///< tRFC             In units of memclocks. The maximum value for all DIMMs. See D18F2x208_dct[3:0][Trfc].
  PmuMR0,                         ///< MR0
  PmuMR1,                         ///< MR1
  PmuMR2,                         ///< MR2
  PmuCD_R_W,                      ///< CD_R_W           Command delay, read to write, any chip select to any other chipselect. Units are memclocks, unsigned integer.
  PmuCD_R_R,                      ///< CD_R_R           Command delay, read to read, any chip select to any other chipselect. Units are memclocks, unsigned integer.
  PmuCD_W_W,                      ///< CD_W_W           Command delay, write to write, any chip select to any other chipselect. Units are memclocks, unsigned integer.
  PmuCD_W_R,                      ///< CD_W_R           Command delay, write to read, any chip select to any other chipselect. Units are memclocks, unsigned integer.
  PmuCD_R_R_SD,                   ///< CD_R_R_SD        Command delay, read to read, any chip select to another chipselect of same DIMM. Units are memclocks, unsigned integer.
  PmuCD_W_W_SD,                   ///< CD_W_W_SD        Command delay, write to write, any chip select to another chipselect of same DIMM. Units are memclocks, unsigned integer.
  PmuTrdrdban_Phy,                ///< Trdrdban_Phy     In memclocks.

  //  GDDR5 SRAM message block
  PmuCh0Dis,                      ///< Ch0Dis           1=Disable the lower 32-bits. Used for GDDR5 mode only.
  PmuCh1Dis,                      ///< Ch1Dis           1=Disable the upper 32-bits. Used for GDDR5 mode only.
  PmuInitACTerm,                  ///< InitACTerm       1=Addr/Cmd term is ZQ. Addr/Cmd term is ZQ/2. Used during DevInit1, for GDDR5 mode only.
  PmuMR3,                         ///< MR3
  PmuMR4,                         ///< MR4
  PmuMR5,                         ///< MR5
  PmuMR6,                         ///< MR6
  PmuMR7,                         ///< MR7
  PmuMR8,                         ///< MR8
  PmuMR15,                        ///< MR15
  PmuMRnum,                       ///< MRnum            SendMrs Specifies the MR number for a single MRS command. Write data is one of MR0 to MR15 in this table.
                                  ///<                  is, chip select 1, etc..IF Gddr5Mode then ChipSelect[7:1] are ignored.

  PmuSRAMRangeEnd                 ///< -------------------------- Start of PMU SRAM range
} REG_BF_NAME;

/// PMU SRAM field location
typedef struct {
  UINT32  LoBit: 6;         ///< Low bit of the bit field
  UINT32  HiBit: 6;         ///< High bit of the bit field
  UINT32  RegOffset: 10;    ///< Register that the bit field is on
  UINT32  Reserved: 9;      ///< Reserved
  UINT32  Linked: 1;        ///< 1: The bit field has high bits defined in the very next Bf identifier
  ///< 0: The bit field has no further extension
} PMU_SRAM_ADDR;

/**
  PMU_SRAM (TableName, BfIndex, RegOffset, Hi, Lo)

  @param[in]    TableName
  @param[in]    BfIndex
  @param[in]    RegOffset
  @param[in]    Hi
  @param[in]    Lo

  @return   PMU_SRAM  Encrypted PMU SRAM field location
--*/
#define PMU_SRAM(TableName, BfIndex, RegOffset, Hi, Lo) \
  TableName[BfIndex] = ( \
    ((UINT32) RegOffset << 12) | ( \
      (Hi > Lo) ? (\
        (((UINT32) Hi << 6) | (UINT32) Lo) \
      ) : ( \
        (((UINT32) Lo << 6) | (UINT32) Hi) \
      ) \
    ) \
  )

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
VOID
MemNPmuResetNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNLoadPmuFirmwareNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       PMU_FIRMWARE_SET *FirmwareSet,
  IN       UINT16 NumberOfEntries
  );

VOID
MemNStartPmuNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNPendOnPmuCompletionNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNCalcMR0 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNCalcMR1 (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  );

UINT32
MemNCalcMR2 (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  );

#endif  /* _MNPMU_H_ */

