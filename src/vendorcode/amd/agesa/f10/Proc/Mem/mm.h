/**
 * @file
 *
 * mm.h
 *
 * Common main functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 46768 $ @e \$Date: 2011-02-09 16:14:12 -0700 (Wed, 09 Feb 2011) $
 *
 **/
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
  * ***************************************************************************
  *
 */

#ifndef _MM_H_
#define _MM_H_

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

#define ALLOC_SOCKET_STRUCT_HANDLE    0
#define ALLOC_DIE_STRUCT_HANDLE       1
#define ALLOC_DCT_STRUCT_HANDLE       2
#define ALLOC_CHL_STRUCT_HANDLE       3
#define ALLOC_PLATFORM_PTR_HANDLE     4
#define ALLOC_FORM_FACTOR_HANDLE      5
#define ALLOC_TRN_DATA_HANDLE         6
#define ALLOC_DIMM_DATA_HANDLE        7
#define ALLOC_PAR_TRN_HANDLE          8

#define GENERATE_MEM_HANDLE(type, x, y, z) (\
  AMD_MEM_MISC_HANDLES_START + (((type) << 18) + ((x) << 12) + ((y) << 6) + (z)) \
)

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// Bit field names used in memory initialization
typedef enum {
  BFDevVendorIDReg,                     ///< Bit field DevVendorIDReg
  BFNodeID,                             ///< Bit field NodeID
  BFNodeCnt,                            ///< Bit field NodeCnt

  BFDramBaseReg0,                       ///< Bit field DramBaseReg0
  BFDramBaseReg1,                       ///< Bit field DramBaseReg1
  BFDramBaseReg2,                       ///< Bit field DramBaseReg2
  BFDramBaseReg3,                       ///< Bit field DramBaseReg3
  BFDramBaseReg4,                       ///< Bit field DramBaseReg4
  BFDramBaseReg5,                       ///< Bit field DramBaseReg5
  BFDramBaseReg6,                       ///< Bit field DramBaseReg6
  BFDramBaseReg7,                       ///< Bit field DramBaseReg7

  BFDramLimitReg0,                      ///< Bit field DramLimitReg0
  BFDramLimitReg1,                      ///< Bit field DramLimitReg1
  BFDramLimitReg2,                      ///< Bit field DramLimitReg2
  BFDramLimitReg3,                      ///< Bit field DramLimitReg3
  BFDramLimitReg4,                      ///< Bit field DramLimitReg4
  BFDramLimitReg5,                      ///< Bit field DramLimitReg5
  BFDramLimitReg6,                      ///< Bit field DramLimitReg6
  BFDramLimitReg7,                      ///< Bit field DramLimitReg7

  BFDramBaseHiReg0,                     ///< Bit field DramBaseHiReg0
  BFDramBaseHiReg1,                     ///< Bit field DramBaseHiReg1
  BFDramBaseHiReg2,                     ///< Bit field DramBaseHiReg2
  BFDramBaseHiReg3,                     ///< Bit field DramBaseHiReg3
  BFDramBaseHiReg4,                     ///< Bit field DramBaseHiReg4
  BFDramBaseHiReg5,                     ///< Bit field DramBaseHiReg5
  BFDramBaseHiReg6,                     ///< Bit field DramBaseHiReg6
  BFDramBaseHiReg7,                     ///< Bit field DramBaseHiReg7

  BFDramLimitHiReg0,                    ///< Bit field DramLimitHiReg0
  BFDramLimitHiReg1,                    ///< Bit field DramLimitHiReg1
  BFDramLimitHiReg2,                    ///< Bit field DramLimitHiReg2
  BFDramLimitHiReg3,                    ///< Bit field DramLimitHiReg3
  BFDramLimitHiReg4,                    ///< Bit field DramLimitHiReg4
  BFDramLimitHiReg5,                    ///< Bit field DramLimitHiReg5
  BFDramLimitHiReg6,                    ///< Bit field DramLimitHiReg6
  BFDramLimitHiReg7,                    ///< Bit field DramLimitHiReg7

  BFDramHoleAddrReg,                    ///< Bit field DramHoleAddrReg

  BFCSBaseAddr0Reg,                     ///< Bit field CSBaseAddr0Reg
  BFCSBaseAddr1Reg,                     ///< Bit field CSBaseAddr1Reg
  BFCSBaseAddr2Reg,                     ///< Bit field CSBaseAddr2Reg
  BFCSBaseAddr3Reg,                     ///< Bit field CSBaseAddr3Reg
  BFCSBaseAddr4Reg,                     ///< Bit field CSBaseAddr4Reg
  BFCSBaseAddr5Reg,                     ///< Bit field CSBaseAddr5Reg
  BFCSBaseAddr6Reg,                     ///< Bit field CSBaseAddr6Reg
  BFCSBaseAddr7Reg,                     ///< Bit field CSBaseAddr7Reg

  BFCSMask0Reg,                         ///< Bit field CSMask0Reg
  BFCSMask1Reg,                         ///< Bit field CSMask1Reg
  BFCSMask2Reg,                         ///< Bit field CSMask2Reg
  BFCSMask3Reg,                         ///< Bit field CSMask3Reg

  BFDramControlReg,                     ///< Bit field DramControlReg
  BFDramInitRegReg,                     ///< Bit field DramInitRegReg
  BFDramBankAddrReg,                    ///< Bit field DramBankAddrReg
  BFDramMRSReg,                         ///< Bit field DramMRSReg
  BFDramTimingLoReg,                    ///< Bit field DramTimingLoReg
  BFDramTimingHiReg,                    ///< Bit field DramTimingHiReg
  BFDramConfigLoReg,                    ///< Bit field DramConfigLoReg
  BFDramConfigHiReg,                    ///< Bit field DramConfigHiReg
  BFDctAddlOffsetReg,                   ///< Bit field DctAddlOffsetReg
  BFDctAddlDataReg,                     ///< Bit field DctAddlDataReg
  BFDctAccessDone,                      ///< Bit field DctAccessDone
  BFDctAccessError,                     ///< Bit field DctAccessError
  BFDctExtraOffsetReg,                  ///< Bit field DctExtraOffsetReg
  BFDctExtraDataReg,                    ///< Bit field DctExtraDataReg
  BFDctExtraAccessDone,                 ///< Bit field DctExtraAccessDone
  BFDramConfigMiscReg,                  ///< Bit field DramConfigMiscReg
  BFDramCtrlMiscReg2,                   ///< Bit field DramCtrlMiscReg2
  BFMctCfgHiReg,                        ///< Bit field MctCfgHiReg
  BFMctCfgLoReg,                        ///< Bit field MctCfgLoReg
  BFExtMctCfgLoReg,                     ///< Bit field ExtMctCfgLoReg

  BFDramHoleBase,                       ///< Bit field DramHoleBase
  BFDramHoleOffset,                     ///< Bit field DramHoleOffset
  BFDramMemHoistValid,                  ///< Bit field DramMemHoistValid
  BFDramHoleValid,                      ///< Bit field DramHoleValid
  BFDramBaseAddr,                       ///< Bit field DramBaseAddr
  BFDramIntlvSel,                       ///< Bit field DramIntlvSel
  BFDramLimitAddr,                      ///< Bit field DramLimitAddr
  BFDramIntlvEn,                        ///< Bit field DramIntlvEn

  BFMcaNbCtlReg,                        ///< Bit field McaNbCtlReg
  BFDramEccEn,                          ///< Bit field DramEccEn
  BFSyncOnUcEccEn,                      ///< Bit field SyncOnUcEccEn
  BFEccSymbolSize,                      ///< Bit field EccSymbolSize
  BFMcaNbStatusLoReg,                   ///< Bit field McaNbStatusLoReg
  BFMcaNbStatusHiReg,                   ///< Bit field McaNbStatusHiReg
  BFDramScrub,                          ///< Bit field DramScrub
  BFL2Scrub,                            ///< Bit field L2Scrub
  BFDcacheScrub,                        ///< Bit field DcacheScrub
  BFL3Scrub,                            ///< Bit field L3Scrub
  BFScrubReDirEn,                       ///< Bit field ScrubReDirEn
  BFScrubAddrLoReg,                     ///< Bit field ScrubAddrLoReg
  BFScrubAddrHiReg,                     ///< Bit field ScrubAddrHiReg
  BFC1ClkDivisor,                       ///< Bit field C1ClkDivisor
  BFDisDatMsk,                          ///< Bit field DisDatMsk
  BFNbFid,                              ///< Bit field NbFid
  BFMTC1eEn,                            ///< Bit field MTC1eEn
  BFL3Capable,                          ///< Bit field L3Capable
  BFF3X188B8,                           ///<
  BFEnhMemProtCap,                      ///< Bit field EnhMemProtCap
  BFNbPsCtrlDis,                        ///< Bit field NbPsCtrlDis

  BFNonSPDHi,                           ///< Bit field NonSPDHi
  BFRdPtrInit,                          ///< Bit field RdPtrInit
  BFF2X78B16,                           ///<
  BFDqsRcvEnTrain,                      ///< Bit field DqsRcvEnTrain
  BFEarlyArbEn,                         ///< Bit field EarlyArbEn
  BFMaxLatency,                         ///< Bit field either MaxRdLat or MaxAsyncLat

  BFMrsAddress,                         ///< Bit field MrsAddress
  BFMrsBank,                            ///< Bit field MrsBank
  BFMrsChipSel,                         ///< Bit field MrsChipSel
  BFSendPchgAll,                        ///< Bit field SendPchgAll
  BFSendAutoRefresh,                    ///< Bit field SendAutoRefresh
  BFSendMrsCmd,                         ///< Bit field SendMrsCmd
  BFDeassertMemRstX,                    ///< Bit field DeassertMemRstX
  BFAssertCke,                          ///< Bit field AssertCke
  BFSendZQCmd,                          ///< Bit field SendZQCmd
  BFSendCtrlWord,                       ///< Bit field SendCtrlWord
  BFEnDramInit,                         ///< Bit field EnDramInit
  BFMrsLevel,                           ///< Bit field MrsLevel
  BFMrsQoff,                            ///< Bit field MrsQoff

  BFBurstCtrl,                          ///< Bit field BurstCtrl
  BFDrvImpCtrl,                         ///< Bit field DrvImpCtrl
  BFDramTerm_DDR3,                      ///< Bit field DramTerm_DDR3
  BFDramTermDyn,                        ///< Bit field DramTermDyn
  BFQoff,                               ///< Bit field Qoff
  BFASR,                                ///< Bit field ASR
  BFSRT,                                ///< Bit field SRT
  BFTcwl,                               ///< Bit field Tcwl
  BFPchgPDModeSel,                      ///< Bit field PchgPDModeSel

  BFTwrDDR3,                            ///< Bit field TwrDDR3
  BFTcl,                                ///< Bit field Tcl
  BFTrcd,                               ///< Bit field Trcd
  BFTrp,                                ///< Bit field Trp
  BFTrtp,                               ///< Bit field Trtp
  BFTras,                               ///< Bit field Tras
  BFTrc,                                ///< Bit field Trc
  BFTwr,                                ///< Bit field Twr
  BFTrrd,                               ///< Bit field Trrd
  BFMemClkDis,                          ///< Bit field MemClkDis
  BFDramTiming0,                        ///< Bit field BFDramTiming0
  BFDramTiming1,                        ///< Bit field BFDramTiming1

  BFNonSPD,                             ///< Bit field NonSPD
  BFTrwtWB,                             ///< Bit field TrwtWB
  BFTrwtTO,                             ///< Bit field TrwtTO
  BFTwtr,                               ///< Bit field Twtr
  BFTwrrd,                              ///< Bit field Twrrd
  BFTwrrdHi,                            ///< Bit field TwrrdHi
  BFTwrwr,                              ///< Bit field Twrwr
  BFTwrwrHi,                            ///< Bit field TwrwrHi
  BFTrdrdSD,                            ///< Bit field TrdrdSD
  BFTwrwrSD,                            ///< Bit field TwrwrSD
  BFTwrrdSD,                            ///< Bit field TwrrdSD
  BFRdOdtTrnOnDly,                      ///< Bit field RdOdtTrnOnDly
  BFRdOdtOnDuration,                    ///< Bit field RdOdtOnDuration
  BFWrOdtTrnOnDly,                      ///< Bit field WrOdtTrnOnDly
  BFWrOdtOnDuration,                    ///< Bit field WrOdtOnDuration

  BFTrdrd,                              ///< Bit field Trdrd
  BFTrdrdHi,                            ///< Bit field TrdrdHi
  BFTref,                               ///< Bit field Tref
  BFDisAutoRefresh,                     ///< Bit field DisAutoRefresh
  BFTrfc0,                              ///< Bit field Trfc0
  BFTrfc1,                              ///< Bit field Trfc1
  BFTrfc2,                              ///< Bit field Trfc2
  BFTrfc3,                              ///< Bit field Trfc3

  BFInitDram,                           ///< Bit field InitDram
  BFExitSelfRef,                        ///< Bit field ExitSelfRef
  BFDramTerm,                           ///< Bit field DramTerm
  BFParEn,                              ///< Bit field ParEn
  BFBurstLength32,                      ///< Bit field BurstLength32
  BFWidth128,                           ///< Bit field Width128
  BFX4Dimm,                             ///< Bit field X4Dimm
  BFDimmEccEn,                          ///< Bit field DimmEccEn
  BFUnBuffDimm,                         ///< Bit field UnBuffDimm
  BFEnterSelfRef,                       ///< Bit field EnterSelfRef
  BFDynPageCloseEn,                     ///< Bit field DynPageCloseEn
  BFIdleCycInit,                        ///< Bit field IdleCycInit
  BFFreqChgInProg,                      ///< Bit field FreqChgInProg
  BFForceAutoPchg,                      ///< Bit field ForceAutoPchg
  BFEnDispAutoPrecharge,                ///< Bit field EnDispAutoPrecharge
  BFDisDllShutdownSR,                   ///< Bit field DisDllShutdownSR
  BFDisSscClkGateData,                  ///< Bit field DisSscClkGateData
  BFDisSscClkGateCmdAddr,               ///< Bit field DisSscClkGateCmdAddr

  BFMemClkFreq,                         ///< Bit field MemClkFreq
  BFMemClkFreqVal,                      ///< Bit field MemClkFreqVal
  BFDdr3Mode,                           ///< Bit field Ddr3Mode
  BFLegacyBiosMode,                     ///< Bit field LegacyBiosMode
  BFZqcsInterval,                       ///< Bit field ZqcsInterval
  BFRDqsEn,                             ///< Bit field RDqsEn
  BFDisDramInterface,                   ///< Bit field DisDramInterface
  BFPowerDownEn,                        ///< Bit field PowerDownEn
  BFPowerDownMode,                      ///< Bit field PowerDownMode
  BFF2X98B17,                           ///<
  BFDcqArbBypassEn,                     ///< Bit field DcqArbBypassEn
  BFFourRankRDimm,                      ///< Bit field FourRankRDimm
  BFSlowAccessMode,                     ///< Bit field SlowAccessMode
  BFBankSwizzleMode,                    ///< Bit field BankSwizzleMode
  BFDcqBypassMax,                       ///< Bit field DcqBypassMax
  BFFourActWindow,                      ///< Bit field FourActWindow

  BFODTSEn,                             ///< Bit field ODTSEn
  BFCmdThrottleMode,                    ///< Bit field CmdThrottleMode

  BFDdr3FourSocketCh,                   ///< Bit field Ddr3FourSocketCh
  BFSubMemclkRegDly,                    ///< Bit field SubMemclkRegDly
  BFOdtSwizzle,                         ///< Bit field OdtSwizzle
  BFProgOdtEn,                          ///< Bit field ProgOdtEn
  BFCtrlWordCS,                         ///< Bit field CtrlWordCS
  BFDataTxFifoWrDly,                    ///< Bit field DataTxFifoWrDly

  BFIntLvRgnSwapEn,                     ///< Bit field IntLvRgnSwapEn
  BFIntLvRgnBaseAddr,                   ///< Bit field IntLvRgnBaseAddr
  BFIntLvRgnLmtAddr,                    ///< Bit field IntLvRgnLmtAddr
  BFIntLvRgnSize,                       ///< Bit field IntLvRgnSize

  BFDctSelHiRngEn,                      ///< Bit field DctSelHiRngEn
  BFDctSelHi,                           ///< Bit field DctSelHi
  BFDctSelIntLvEn,                      ///< Bit field DctSelIntLvEn
  BFMemClrInit,                         ///< Bit field MemClrInit
  BFDctGangEn,                          ///< Bit field DctGangEn
  BFDctDatIntLv,                        ///< Bit field DctDatIntLv
  BFDctSelIntLvAddr,                    ///< Bit field DctSelIntLvAddr
  BFDctSelIntLvAddrHi,                  ///< Bit field DctSelIntLvAddrHi
  BFDramEnabled,                        ///< Bit field DramEnabled
  BFMemClrBusy,                         ///< Bit field MemClrBusy
  BFMemCleared,                         ///< Bit field MemCleared
  BFDctSelBaseAddr,                     ///< Bit field DctSelBaseAddr
  BFDctSelBaseOffset,                   ///< Bit field DctSelBaseOffset

  BFAdapPrefMissRatio,                  ///< Bit field AdapPrefMissRatio
  BFAdapPrefPosStep,                    ///< Bit field AdapPrefPosStep
  BFAdapPrefNegStep,                    ///< Bit field AdapPrefNegStep
  BFCohPrefPrbLmt,                      ///< Bit field CohPrefPrbLmt

  BFPrefDramTrainDone,                  ///< Bit field PrefDramTrainDone
  BFWrDramTrainMode,                    ///< Bit field WrDramTrainMode
  BFMctPrefReqLimit,                    ///< Bit field MctPrefReqLimit
  BFPrefDramTrainMode,                  ///< Bit field PrefDramTrainMode
  BFDctWrLimit,                         ///< Bit field DctWrLimit
  BFMctWrLimit,                         ///< Bit field MctWrLimit
  BFDramTrainPdbDis,                    ///< Bit field DramTrainPdbDis
  BFTrainLength,                        ///< Bit field TrainLength
  BFRdTrainGo,                          ///< Bit field RdTrainGo
  BFWrTrainGo,                          ///< Bit field WrTrainGo
  BFWrTrainAdrPtrLo,                    ///< Bit field WrTrainAdrPtrLo
  BFWrTrainAdrPtrHi,                    ///< Bit field WrTrainAdrPtrHi
  BFWrTrainBufAddr,                     ///< Bit field WrTrainBufAddr
  BFWrTrainBufDat,                      ///< Bit field WrTrainBufDat
  BFFlushWr,                            ///< Bit field FlushWr
  BFFlushWrOnStpGnt,                    ///< Bit field FlushWrOnStpGnt
  BFPrefCpuDis,                         ///< Bit field PrefCpuDis
  BFPrefIoDis,                          ///< Bit field PrefIoDis
  BFTrainCmpSts,                        ///< Bit field TrainCmpSts
  BFTrainCmpSts2,                       ///< Bit field TrainCmpSts2

  BFAddrCmdDrvStren,                    ///< Bit field AddrCmdDrvStren
  BFDataDrvStren,                       ///< Bit field DataDrvStren
  BFCkeDrvStren,                        ///< Bit field CkeDrvStren
  BFCsOdtDrvStren,                      ///< Bit field CsOdtDrvStren
  BFClkDrvStren,                        ///< Bit field ClkDrvStren
  BFDqsDrvStren,                        ///< Bit field DqsDrvStren
  BFODCControl,                         ///< Bit field ODCControl
  BFAddrTmgControl,                     ///< Bit field AddrTmgControl

  BFWrtLvTrEn,                          ///< Bit field WrtLvTrEn
  BFWrtLvTrMode,                        ///< Bit field WrtLvTrMode
  BFPhyFenceTrEn,                       ///< Bit field PhyFenceTrEn
  BFTrDimmSel,                          ///< Bit field TrDimmSel
  BFFenceTrSel,                         ///< Bit field FenceTrSel
  BFWrLvOdt,                            ///< Bit field WrLvOdt
  BFWrLvOdtEn,                          ///< Bit field WrLvOdtEn
  BFDqsRcvTrEn,                         ///< Bit field DqsRcvTrEn
  BFDisAutoComp,                        ///< Bit field DisAutoComp
  BFWrtLvErr,                           ///< Bit field WrtLvErr
  BFODTAssertionCtl,                    ///< Bit field ODTAssertionCtl
  BFNibbleTrainModeEn,                  ///< Bit field NibbleTrainModeEn
  BFRankTrainModeEn,                    ///< Bit field RankTrainModeEn
  BFPllMult,                            ///< Bit field PllMult
  BFPllDiv,                             ///< Bit field PllDiv

  BFDramPhyStatusReg,                   ///< Bit field DramPhyStatusReg

  BFD3Cmp2PCal,                         ///< Bit field D3Cmp2PCal
  BFD3Cmp2NCal,                         ///< Bit field D3Cmp2NCal
  BFD3Cmp1PCal,                         ///< Bit field D3Cmp1PCal
  BFD3Cmp1NCal,                         ///< Bit field D3Cmp1NCal
  BFD3Cmp0PCal,                         ///< Bit field D3Cmp0PCal
  BFD3Cmp0NCal,                         ///< Bit field D3Cmp0NCal

  BFPhyFence,                           ///< Bit field PhyFence
  BFODTTri,                             ///< Bit field ODTTri
  BFCKETri,                             ///< Bit field CKETri
  BFChipSelTri,                         ///< Bit field ChipSelTri
  BFPhyRODTCSLow,                       ///< Bit field PhyRODTCSLow
  BFPhyRODTCSHigh,                      ///< Bit field PhyRODTCSHigh
  BFPhyWODTCSLow,                       ///< Bit field PhyWODTCSLow
  BFPhyWODTCSHigh,                      ///< Bit field PhyWODTCSHigh
  BFUSPLLCtlAll,                        ///< Bit field USPLLCtlAll
  BFDSPLLCtlAll,                        ///< Bit field DSPLLCtlAll
  BFUSNibbleAlignEn,                    ///< Bit field USNibbleAlignEn
  BFChnLinitClkEn,                      ///< Bit field ChnLinitClkEn

  BFTSLinkSelect,                       ///< Bit field TSLinkSelect
  BFTS2BitLockEn,                       ///< Bit field TS2BitLockEn
  BFTS2En,                              ///< Bit field TS2En
  BFTS1En,                              ///< Bit field TS1En
  BFTS0LinkStarEn,                      ///< Bit field TS0LinkStarEn
  BFTS0En,                              ///< Bit field TS0En

  BFLinkTrainData,                      ///< Bit field LinkTrainData

  BFRstRxFifoPtrs,                      ///< Bit field RstRxFifoPtrs
  BFRxFifoPtrInit,                      ///< Bit field RxFifoPtrInit
  BFRstTxFifoPtrs,                      ///< Bit field RstTxFifoPtrs
  BFTxFifoPtrInit,                      ///< Bit field TxFifoPtrInit

  BFLpbkCount,                          ///< Bit field LpbkCount
  BFLpbkMap,                            ///< Bit field LpbkMap
  BFSendLpbkMaintCmd,                   ///< Bit field SendLpbkMaintCmd
  BFLpbkData,                           ///< Bit field LpbkData

  BFMbRdPtrEn,                          ///< Bit field MbRdPtrEn
  BFLnkLpBkLat,                         ///< Bit field LnkLpBkLat
  BFLpbkRndTripLatDone,                 ///< Bit field LpbkRndTripLatDone
  BFLnkLatTrainEn,                      ///< Bit field LnkLatTrainEn

  BFDsPhyReset,                         ///< Bit field DsPhyReset
  BFLinkReset,                          ///< Bit field LinkReset

  BFPllLockTime,                        ///< Bit field PllLockTime
  BFPllRegWaitTime,                     ///< Bit field PllRegWaitTime
  BFNclkFreqDone,                       ///< Bit field NclkFreqDone
  BFNbPs0NclkDiv,                       ///< Bit field NbPs0NclkDiv
  BFNbPs1NclkDiv,                       ///< Bit field NbPs1NclkDiv
  BFNbPs1Act,                           ///< Bit field NbPs1Act

  BFOnLineSpareControl,                 ///< Bit field OnLineSpareControl
  BFDdrMaxRate,                         ///< Bit field DdrMaxRate

  BFC6Base,                             ///< Bit field C6Base
  BFC6DramLock,                         ///< Bit field C6DramLock

  BFRxPtrInitReq,                       ///< Bit field RxPtrInitReq
  BFTxPtrInitReq,                       ///< Bit field TxPtrInitReq
  BFAddrCmdTriEn,                       ///< Bit field AddrCmdTriEn
  BFForceCasToSlot0,                    ///< Bit field ForceCasToSlot0
  BFDisCutThroughMode,                  ///< Bit field DisCutThroughMode
  BFDbeSkidBufDis,                      ///< Bit field DbeSkidBufDis
  BFBankSwap,                           ///< Bit field BankSwap
  BFDbeGskMemClkAlignMode,              ///< Bit field DbeGskMemClkAlignMode
  BFEnCpuSerRdBehindNpIoWr,             ///< Bit field EnCpuSerRdBehindNpIoWr
  BFDbeCmdThrottle,                     ///< Bit field DbeCmdThrottle
  BFRxDLLWakeupTime,                    ///< Bit field RxDllWakeupTime
  BFRxCPUpdPeriod,                      ///< Bit field RxCPUpdPeriod
  BFRxMaxDurDllNoLock,                  ///< Bit field RxMaxDurDllNoLock
  BFTxDLLWakeupTime,                    ///< Bit field TxDllWakeupTime
  BFTxCPUpdPeriod,                      ///< Bit field TxCPUpdPeriod
  BFTxMaxDurDllNoLock,                  ///< Bit field TxMaxDurDllNoLock
  BFEnRxPadStandby,                     ///< Bit field EnRxPadStandby

  BFMemTempHot,                         ///< Bit field MemTempHot
  BFDoubleTrefRateEn,                   ///< Bit field DoubleTrefRateEn

  BFCpu1En,                             ///< Bit field BFCpu1En
  BFCpu32En,                            ///< Bit field BFCpu32En
  BFAcpiPwrStsCtrlHi,                   ///< Bit field BFAcpiPwrStsCtrlHi
  BFDramSrHysEn,                        ///< Bit field BFDramSrHysEn
  BFDramSrHys,                          ///< Bit field BFDramSrHys
  BFMemTriStateEn,                      ///< Bit field BFMemTriStateEn
  BFDramSrEn,                           ///< Bit field BFDramSrEn

  /* Bit fields for workarounds */
  BFErr263,                             ///< Bit field Err263
  BFErr350,                             ///< Bit field Err350
  BFErr322I,                            ///< Bit field Err322I
  BFErr322II,                           ///< Bit field Err322II

  /* Bit fields for Phy */
  BFEccDLLConf,                         ///< Bit field EccDLLConf
  BFEccDLLPwrDnConf,                    ///< Bit field EccDLLPwrDnConf
  BFPhyPLLLockTime,                     ///< Bit field PhyPLLLockTime
  BFPhyDLLLockTime,                     ///< Bit field PhyDLLLockTime
  BFPhyDLLControl,                      ///< Bit field BFPhyDLLControl
  BFPhy0x0D080F0C,                      ///< Bit field BFPhy0x0D080F0C
  BFPhy0x0D080F10,                      ///< Bit field BFPhy0x0D080F10
  BFPhy0x0D080F11,                      ///< Bit field BFPhy0x0D080F11
  BFPhy0x0D088F30,                      ///< Bit field BFPhy0x0D088F30
  BFPhy0x0D08C030,                      ///< Bit field BFPhy0x0D08C030
  BFPhy0x0D082F30,                      ///< Bit field BFPhy0x0D082F30

  BFPhy0x0D040F3E,                      ///< Bit field BFPhy0x0D040F3E
  BFPhy0x0D042F3E,                      ///< Bit field BFPhy0x0D042F3E
  BFPhy0x0D048F3E,                      ///< Bit field BFPhy0x0D048F3E
  BFPhy0x0D04DF3E,                      ///< Bit field BFPhy0x0D04DF3E

  BFPhyClkConfig0,                    ///< Bit field ClkConfig0
  BFPhyClkConfig1,                    ///< Bit field ClkConfig1
  BFPhyClkConfig2,                    ///< Bit field ClkConfig2
  BFPhyClkConfig3,                    ///< Bit field ClkConfig3

  BFPhy0x0D0F0F13,                      ///< Bit field BFPhy0x0D0F0F13
  BFPhy0x0D0F0F13Bit0to7,               ///< Bit field BFPhy0x0D0F0F13Bit0to7
  BFPhy0x0D0F0830,                      ///< Bit field BFPhy0x0D0F0830
  BFPhy0x0D07812F,                      ///< Bit field BFPhy0x0D0F8108

  BFDataRxVioLvl,                       ///< Bit field DataRxVioLvl
  BFClkRxVioLvl,                        ///< Bit field ClkRxVioLvl
  BFCmdRxVioLvl,                        ///< Bit field CmdRxVioLvl
  BFAddrRxVioLvl,                       ///< Bit field AddrRxVioLvl
  BFCmpVioLvl,                          ///< Bit field CmpVioLvl
  BFAlwaysEnDllClks,                    ///< Bit field AlwaysEnDllClks
  BFPhy0x0D0FE00A,                      ///< Bit field Phy0x0D0FE00A

  BFDataFence2,                         ///< Bit field DataFence2
  BFClkFence2,                          ///< Bit field ClkFence2
  BFCmdFence2,                          ///< Bit field CmdFence2
  BFAddrFence2,                         ///< Bit field AddrFence2

  BFAddrCmdTri,                         ///< Bit field BFAddrCmdTri
  BFLowPowerDrvStrengthEn,              ///< Bit field BFLowPowerDrvStrengthEn
  BFLevel,                              ///< Bit field Level

  BFDbeGskFifoNumerator,                ///< Bit field DbeGskFifoNumerator
  BFDbeGskFifoDenominator,              ///< Bit field DbeGskFifoDenominator
  BFDataTxFifoSchedDlyNegSlot0,         ///< Bit field DataTxFifoSchedDlyNegSlot0
  BFDataTxFifoSchedDlyNegSlot1,         ///< Bit field DataTxFifoSchedDlyNegSlot1
  BFDataTxFifoSchedDlySlot0,            ///< Bit field DataTxFifoSchedDlySlot0
  BFDataTxFifoSchedDlySlot1,            ///< Bit field DataTxFifoSchedDlySlot1

  BFDisablePredriverCal,                ///< Bit field DisablePredriverCal
  BFDataByteTxPreDriverCal,             ///< Bit field DataByteTxPreDriverCal
  BFDataByteTxPreDriverCal2Pad1,        ///< Bit field DataByteTxPreDriverCal2Pad1
  BFDataByteTxPreDriverCal2Pad2,        ///< Bit field DataByteTxPreDriverCal2Pad2
  BFCmdAddr0TxPreDriverCal2Pad1,        ///< Bit field CmdAddr0TxPreDriverCal2Pad1
  BFCmdAddr0TxPreDriverCal2Pad2,        ///< Bit field CmdAddr0TxPreDriverCal2Pad2
  BFCmdAddr1TxPreDriverCal2Pad1,        ///< Bit field CmdAddr1TxPreDriverCal2Pad1
  BFCmdAddr1TxPreDriverCal2Pad2,        ///< Bit field CmdAddr1TxPreDriverCal2Pad2
  BFAddrTxPreDriverCal2Pad1,            ///< Bit field AddrTxPreDriverCal2Pad1
  BFAddrTxPreDriverCal2Pad2,            ///< Bit field AddrTxPreDriverCal2Pad2
  BFAddrTxPreDriverCal2Pad3,            ///< Bit field AddrTxPreDriverCal2Pad3
  BFAddrTxPreDriverCal2Pad4,            ///< Bit field AddrTxPreDriverCal2Pad4
  BFCmdAddr0TxPreDriverCalPad0,         ///< Bit field CmdAddr0TxPreDriverCalPad0
  BFCmdAddr1TxPreDriverCalPad0,         ///< Bit field CmdAddr1TxPreDriverCalPad0
  BFAddrTxPreDriverCalPad0,             ///< Bit field AddrTxPreDriverCalPad0
  BFClock0TxPreDriverCalPad0,           ///< Bit field Clock0TxPreDriverCalPad0
  BFClock1TxPreDriverCalPad0,           ///< Bit field Clock1TxPreDriverCalPad0

  // Reserved
  BFReserved01,                         ///< Reserved 01
  BFReserved02,                         ///< Reserved 02
  BFReserved03,                         ///< Reserved 03
  BFReserved04,                         ///< Reserved 04
  BFReserved05,                         ///< Reserved 05
  BFReserved06,                         ///< Reserved 06
  BFReserved07,                         ///< Reserved 07
  BFReserved08,                         ///< Reserved 08
  BFReserved09,                         ///< Reserved 09
  BFReserved10,                         ///< Reserved 10
  BFDctSelBaseAddrReg,                  ///< Bit field DctSelBaseAddrReg
  BFDctSelBaseOffsetReg,                ///< Bit field DctSelBaseOffsetReg

  /* End of accessible list --- entries below this line are for private use ------------*/
  BFEndOfList,                          ///< End of bit field list

  // Only for Table Drive Support define.
  BFRcvEnDly,       ///< F2x[1,0]9C_x[2B:10] Dram DQS Receiver Enable Timing Control Registers
  BFWrDatDly,       ///< F2x[1, 0]9C_x[302:301, 202:201, 102:101, 02:01] DRAM Write Data Timing [High:Low] Registers
  BFRdDqsDly,       ///< F2x[1, 0]9C_x[306:305, 206:205, 106:105, 06:05] DRAM Read DQS Timing Control [High:Low] Registers
  BFWrDqsDly,       ///< F2x[1, 0]9C_x[4A:30] DRAM DQS Write Timing Control Registers
  BFPhRecDly,       ///< F2x[1, 0]9C_x[51:50] DRAM Phase Recovery Control Register [High:Low] Registers

  /* Do not define any entries beyond this point */
  BFAbsLimit                            ///< Beyond this point is reserved for bit field manipulation

} BIT_FIELD_NAME;

/// Bit field aliases
#define BFMainPllOpFreqId       BFNbFid
#define BFRdDramTrainMode       BFPrefDramTrainMode
#define BFThrottleEn            BFCmdThrottleMode
#define BFIntlvRegionEn         BFIntLvRgnSwapEn
#define BFIntlvRegionBase       BFIntLvRgnBaseAddr
#define BFIntlvRegionLimit      BFIntLvRgnLmtAddr
#define BFRdOdtPatReg           BFPhyRODTCSLow
#define BFWrOdtPatReg           BFPhyWODTCSLow

/// Bit field names per DRAM CS base address register
typedef enum {
  BFCSEnable = 0,               ///< Chip select enable
  BFSpare = 1,                  ///< Spare rank
  BFTestFail = 2,               ///< Memory test failed
  BFOnDimmMirror = 3            ///< on-DIMM mirroring enable
} CS_BASE_BIT_FIELD;

/// Flag for exclude dimm
typedef enum {
  NORMAL,                       ///< Normal mode, exclude the dimm if there is new dimm failure
  TRAINING,                     ///< Training mode, exclude dimms that fail during training after training is done
  END_TRAINING                  ///< End training mode, exclude all dimms that failed during training
} DIMM_EXCLUDE_FLAG;

#define BSP_DIE   0
#define MAX_NODES_SUPPORTED     8   ///< Maximum number of nodes in the system.
#define MAX_CS_PER_CHANNEL      8   ///< Max CS per channel
#define MAX_CS_PER_DELAY        2   ///< Max Chip Select Controlled by a set of delays.

///
/// MEM_SHARED_DATA
/// This structure defines the shared data area that is used by the memory
/// code to share data between different northbridge objects. Each substructure
/// in the data area defines how this data area is used by a different purpose.
///
/// There should only be one instance of this struct created for all of the memory
/// code to use.
///
typedef struct _MEM_SHARED_DATA {

  // System memory map data
  UINT32  CurrentNodeSysBase;       ///< Base[47:16] (system address) DRAM base address for current node.
  /// Memory map data for each node
  BOOLEAN AllECC; ///< ECC support on the system
  DIMM_EXCLUDE_FLAG DimmExcludeFlag;  ///< Control the exclude dimm behavior
  UINT8 VoltageMap; ///< The commonly supported voltage map in the system

  /// Data structure for node map
  struct {
    BOOLEAN IsValid;                ///< TRUE if this node contains memory.
    UINT32  SysBase;                ///< Base[47:16] (system address) DRAM base address of this node.
    UINT32  SysLimit;               ///< Base[47:16] (system address) DRAM limit address of this node.
  } NodeMap[MAX_NODES_SUPPORTED];
  BOOLEAN UndoHoistingAbove1TB;     ///< Undo hoisting above 1TB

  /// Data structure for node interleave feature
  struct {
    BOOLEAN IsValid;                ///< TRUE if the data in this structure is valid.
    UINT8   NodeCnt;                ///< Number of nodes in the system.
    UINT32  NodeMemSize;            ///< Total memory of this node.
    UINT32  Dct0MemSize;            ///< Total memory of this DCT 0.
    UINT8   NodeIntlvSel;           ///< Index to each node.
  } NodeIntlv;
} MEM_SHARED_DATA;

///
/// MEM_MAIN_DATA_BLOCK
///
typedef struct _MEM_MAIN_DATA_BLOCK {
  struct _MEM_DATA_STRUCT *MemPtr;          ///< Pointer to customer shared data
  struct _MEM_NB_BLOCK    *NBPtr;           ///< Pointer to array of NB Blocks
  struct _MEM_TECH_BLOCK  *TechPtr;         ///< Pointer to array of Tech Blocks
  struct _MEM_SHARED_DATA *mmSharedPtr;     ///< Pointer to shared data area.
  UINT8           DieCount;                 ///< Total number of Dies installed
} MEM_MAIN_DATA_BLOCK;


/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */


/*
node: Indicates the Node
- Value ranges from 0-7, 0xF: for all nodes
- Size - 4 Bits

dct: Indicate the DRAM Controller
- Value is 0, 1 (0xF: apply setting to all DCTs)
- Size - 4 Bits

dimm: This values specifies which DIMM register will be applied
- The value varies from 0 to 3, 0xF: all DIMMs
- Size - 4 Bits

attr - Indicates if the value needs to be added, subtracted, overridden or Auto (not changed)
- 0: Do not change the current value in the register
- 1: Use the value provided in the table to override the current value in the register (the one that AGESA initially determined)
- 2: Add the value provided in the table as an offset to the current value in the register (the one that AGESA initially determined)
- 3: Subtract the value provided in the table as an offset to the current value in the register (the one that AGESA initially determined)
- Size - 2 Bits

time - Indicate the timing for the register which is written.
- 0:  Write register value before Dram init
- 1:  Write register value before memory training
- 2:  Write register value after memory training
- Size - 1 Bits

bytelane: bytelane number
- This determines specifies which bytelane register will be applied
- Bit0 =1 - set value into Bytelane0
- Bit1 =1 - set value into Bytelane1
- Bit2 =1 - set value into Bytelane2
...
...
- 0xFFFF: all bytelane
- Size - 16 Bits.

bfIndex: Indicate the bitfield index
- Size - 16 Bits

value - Value to be used
- This can be an offset (sub or Add) or an override value.
- Size - DWORD
*/

// Sample code
// NBACCESS (MTBeforeDInit, MTNodes,  MTDct0, BFCSBaseAddr5Reg, MTOverride,   0x400001),
// NBACCESS (MTBeforeTrn,   MTNodes,  MTDct1, BFCSBaseAddr7Reg, MTOverride,   0xFFFFFFFF),
// DQSACCESS (MTAfterTrn,    MTNodes,  MTDcts, MTDIMM0,  MTBL1+MTBL2,    BFRcvEnDly, MTSubtract,   2),
// DQSACCESS (MTAfterTrn,    MTNodes,  MTDct1, MTDIMM1,  MTBLNOECC,      BFRcvEnDly, MTAdd,        1),

#define ENDMEMTDS  0, 0, 0, 0, 0, 0, 0xFFFFFFFF, 0

#define NBACCESS(time, node, dct, bitfield, attr, value) \
{ (time), \
  ((node) & 0x0F) | ((dct) << 4), \
  (((attr) & 0x07) << 4) | (VT_MSK_VALUE << 7) , \
  (UINT8)((bitfield) & 0x000000FF), \
  (UINT8)(((bitfield) >> 8)  & 0x000000FF), \
  (UINT8)(((bitfield) >> 16) & 0x000000FF), \
  (UINT8)(((bitfield) >> 24) & 0x000000FF), \
  0, 0, \
  (UINT8)((value) & 0x000000FF), \
  (UINT8)(((value) >> 8)  & 0x000000FF), \
  (UINT8)(((value) >> 16) & 0x000000FF), \
  (UINT8)(((value) >> 24) & 0x000000FF), \
  0, 0, 0 \
}

#define DQSACCESS(time, node, dct, dimm, bitfield, attr, b0, b1, b2, b3, b4, b5, b6, b7, b8) \
{ (time), \
  ((node) & 0x0F) | ((dct) << 4), \
  (((dimm) & 0x0F)  | ((attr) & 0x07) << 4) | (VT_ARRAY << 7) , \
  (UINT8)((bitfield) & 0x000000FF), \
  (UINT8)(((bitfield) >> 8)  & 0x000000FF), \
  (UINT8)(((bitfield) >> 16) & 0x000000FF), \
  (UINT8)(((bitfield) >> 24) & 0x000000FF), \
  (b0), (b1), (b2), (b3), (b4), (b5), (b6), (b7), (b8) \
}

/// Type of modification supported by table driven support.
typedef enum {
  MTAuto,                       ///<  Do not change the current value in the register
  MTOverride,                   ///<  Use the value provided in the table to override the current value in the register
  MTSubtract,                   ///<  Subtract the value provided in the table as an offset to the current value in the register
  MTAdd                         ///<  Add the value provided in the table as an offset to the current value in the reg
} MTAttr;

/// Time for table driven support to make modification.
typedef enum {
  MTAfterAutoCycTiming,         ///<  After Auto Cycle Timing
  MTAfterPlatformSpec,          ///<  After Platform Specific Configuration
  MTBeforeDInit,                ///<  Before Dram init
  MTBeforeTrn,                  ///<  Before memory training
  MTAfterTrn,                   ///<  After memory training
  MTAfterSwWLTrn,               ///<  After SW Based WL Training
  MTAfterHwWLTrnP1,             ///<  After HW Based WL Training Part 1
  MTAfterHwRxEnTrnP1,           ///<  After HW Based Receiver Enable Training Part 1
  MTAfterHwWLTrnP2,             ///<  After HW Based WL Training Part 2
  MTAfterHwRxEnTrnP2,           ///<  After HW Based Receiver Enable Training Part 2
  MTAfterSwRxEnTrn,             ///<  After SW Based Receiver Enable Training
  MTAfterDqsRwPosTrn,           ///<  After DQS Read/Write Position Training
  MTAfterMaxRdLatTrn,           ///<  After Max Read Latency Training
  MTAfterNbPstateChange,        ///<  After programming NB Pstate dependent registers
  MTEnd = 0xFF ///< End of enum define.
} MTTime;

/// Node on which modification should be made by table driven support.
typedef enum {
  MTNode0, ///< Node 0.
  MTNode1, ///< Node 1.
  MTNode2, ///< Node 2.
  MTNode3, ///< Node 3.
  MTNode4, ///< Node 4.
  MTNode5, ///< Node 5.
  MTNode6, ///< Node 6.
  MTNode7, ///< Node 7.
  MTNodes = 0xF                   ///<  all nodes
} MTNode;

/// DCT on which modification should be made by table driven support.
typedef enum {
  MTDct0, ///< DCT 0.
  MTDct1, ///< DCT 1.
  MTDcts = 0xF,                   ///<  all dcts
} MTDct;

/// Dimm on which modification should be made by table driven support.
typedef enum {
  MTDIMM0, ///< Dimm 0.
  MTDIMM1, ///< Dimm 1.
  MTDIMM2, ///< Dimm 2.
  MTDIMM3, ///< Dimm 3.
  MTDIMMs = 0xF,                  ///<  all Dimms
} MTDIMM;

/// Bytelane on which modification should be made by table driven support.
typedef enum {
  MTBL0 = 0x1,                    ///<  set the value into Bytelane0
  MTBL1 = 0x2,                    ///<  set the value into Bytelane1
  MTBL2 = 0x4,                    ///<  set the value into Bytelane2
  MTBL3 = 0x8,                    ///<  set the value into Bytelane3
  MTBL4 = 0x10, ///< set the value into Bytelane4
  MTBL5 = 0x20, ///< set the value into Bytelane5
  MTBL6 = 0x40, ///< set the value into Bytelane6
  MTBL7 = 0x80, ///< set the value into Bytelane7
  MTBL8 = 0x100,                  ///<  set the value into ECC
  MTBLNOECC = 0xFF,               ///<  all Bytelanes except ECC
  MTBLs = 0xFFFF,                 ///<  all Bytelanes
} MTBL;
/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
AGESA_STATUS
MemAmdFinalize (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

AGESA_STATUS
MemSocketScan (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );

VOID
SetMemError (
  IN       AGESA_STATUS  Errorval,
  IN OUT   DIE_STRUCT *MCTPtr
  );

VOID
AmdMemInitDataStructDefRecovery (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemRecDefRet (VOID);

VOID
SetMemRecError (
  IN       AGESA_STATUS  Errorval,
  IN OUT   DIE_STRUCT *MCTPtr
  );

#endif  /* _MM_H_ */


