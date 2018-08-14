/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH routine definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision$   @e \$Date$
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2017, Advanced Micro Devices, Inc.
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

#include <check_for_wrapper.h>

#ifndef _FCH_DEF_H_
#define _FCH_DEF_H_


UINT32        ReadAlink (IN UINT32 Index, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WriteAlink (IN UINT32 Index, IN UINT32 Data, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          RwAlink (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ReadMem (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *ValuePtr);
VOID          WriteMem (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *ValuePtr);
VOID          RwMem (IN UINT32 Address, IN UINT8 OpFlag, IN UINT32 Mask, IN UINT32 Data);
VOID          ReadPci (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WritePci (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ProgramFchSciMapTbl (IN SCI_MAP_CONTROL  *pSciMapTbl, IN FCH_RESET_DATA_BLOCK *FchResetDataBlock);
VOID          ProgramFchGpioTbl (IN GPIO_CONTROL  *pGpioTbl);
VOID          ProgramFchSataPhyTbl (IN SATA_PHY_CONTROL  *pSataPhyTbl, IN FCH_RESET_DATA_BLOCK *FchResetDataBlock);
VOID          GetChipSysMode (IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ReadPmio (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WritePmio (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          RwPmio (IN UINT8 Address, IN UINT8 OpFlag, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ReadPmio2 (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WritePmio2 (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          RwPmio2 (IN UINT8 Address, IN UINT8  OpFlag, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ReadBiosram (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WriteBiosram (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          GetFchAcpiMmioBase (OUT UINT32 *AcpiMmioBase, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          GetFchAcpiPmBase (OUT  UINT16 *AcpiPmBase, IN AMD_CONFIG_PARAMS *StdHeader);
UINT8         ReadFchSleepType (IN AMD_CONFIG_PARAMS *StdHeader);
UINT8         ReadFchChipsetRevision (IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN       FchCheckBR_ST (IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN       FchCheckBR (IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN       FchCheckST (IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN       FchCheckCZ (IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN       FchCheckPackageAM4 (IN AMD_CONFIG_PARAMS *StdHeader);
UINT64        FchGetScratchFuse (IN AMD_CONFIG_PARAMS *StdHeader);
VOID          FchInitResetRequest (IN AMD_CONFIG_PARAMS *StdHeader);

///
/// Fch Ab Routines
///
///  Pei Phase
///
VOID  FchInitResetAb (IN VOID* FchDataPtr);
VOID  FchProgramAbPowerOnReset (IN VOID* FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvAb (IN VOID* FchDataPtr);
VOID  FchInitEnvAbSpecial (IN VOID* FchDataPtr);
VOID  FchInitMidAb (IN VOID* FchDataPtr);
VOID  FchInitLateAb (IN VOID* FchDataPtr);
///
///  Other Public Routines
///
VOID  FchInitEnvAbLinkInit (IN VOID* FchDataPtr);
BOOLEAN IsUmiOneLaneGen1Mode (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchAbLateProgram (IN VOID* FchDataPtr);

///
/// Fch Pcie Routines
///
///
///  Dxe Phase
///
VOID  ProgramPcieNativeMode (IN VOID* FchDataPtr);

///
/// Fch Gpp Routines
///

///
///  Common Gpp Routines
///
VOID  FchGppDynamicPowerSaving (IN FCH_GPP *FchGpp, IN AMD_CONFIG_PARAMS *StdHeader);

///
/// Fch Azalia Routines
///
///  Pei Phase
///
VOID  FchInitResetAzalia    (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvAzalia      (IN VOID  *FchDataPtr);
VOID  FchInitMidAzalia      (IN VOID  *FchDataPtr);
VOID  FchInitLateAzalia     (IN VOID  *FchDataPtr);


///
/// Fch HwAcpi Routines
///
///  Pei Phase
///
VOID  FchInitResetHwAcpiP    (IN VOID  *FchDataPtr);
VOID  FchInitResetHwAcpi     (IN VOID  *FchDataPtr);
VOID  ProgramFchHwAcpiResetP (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvHwAcpiP                      (IN VOID  *FchDataPtr);
VOID  FchInitEnvHwAcpi                       (IN VOID  *FchDataPtr);
VOID  ProgramEnvPFchAcpiMmio                 (IN VOID *FchDataPtr);
VOID  ProgramFchEnvHwAcpiPciReg              (IN VOID *FchDataPtr);
VOID  ProgramSpecificFchInitEnvAcpiMmio      (IN VOID *FchDataPtr);
VOID  ProgramFchEnvSpreadSpectrum            (IN VOID *FchDataPtr);
VOID  FchInitMidHwAcpi      (IN VOID  *FchDataPtr);
VOID  FchInitLateHwAcpi     (IN VOID  *FchDataPtr);

///
///  Other Public Routines
///
VOID HpetInit               (IN VOID  *FchDataPtr);
VOID MtC1eEnable            (IN VOID  *FchDataPtr);
VOID GcpuRelatedSetting     (IN VOID  *FchDataPtr);
VOID StressResetModeLate    (IN VOID  *FchDataPtr);

///
/// Fch Hwm Routines
///
///  Pei Phase
///
VOID  FchInitResetHwm (IN VOID* FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvHwm (IN VOID* FchDataPtr);
VOID  FchInitMidHwm (IN VOID* FchDataPtr);
VOID  FchInitLateHwm (IN VOID* FchDataPtr);
///
///  Other Public Routines
///
VOID  HwmInitRegister (IN VOID* FchDataPtr);
VOID  FchECfancontrolservice (IN VOID* FchDataPtr);


///
/// Fch EC Routines
///
///  Pei Phase
///
VOID    FchInitResetEc     (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID    FchInitEnvEc       (IN VOID  *FchDataPtr);
VOID    FchInitMidEc       (IN VOID  *FchDataPtr);
VOID    FchInitLateEc      (IN VOID  *FchDataPtr);
///
///  Other Public Routines
///
VOID    EnterEcConfig (IN AMD_CONFIG_PARAMS *StdHeader);
VOID    ExitEcConfig (IN AMD_CONFIG_PARAMS *StdHeader);
VOID    ReadEc8    (IN UINT8 Address, IN UINT8* Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID    WriteEc8   (IN UINT8 Address, IN UINT8* Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID    RwEc8      (IN UINT8 Address, IN UINT8 AndMask, IN UINT8 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID    WriteECmsg (IN UINT8 Address, IN UINT8 OpFlag, IN VOID* Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID    ReadECmsg  (IN UINT8 Address, IN UINT8 OpFlag, OUT VOID* Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID    WaitForEcLDN9MailboxCmdAck (IN AMD_CONFIG_PARAMS *StdHeader);

///
/// Fch Ir Routines
///
///  Dxe Phase
///
VOID  FchInitEnvIr (IN VOID* FchDataPtr);
VOID  FchInitMidIr (IN VOID* FchDataPtr);
VOID  FchInitLateIr (IN VOID* FchDataPtr);

///
/// Fch SATA Routines
///
///  Pei Phase
///
VOID  FchInitResetSata          (IN VOID  *FchDataPtr);
VOID  FchInitResetSataProgram   (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitMidSata                   (IN VOID  *FchDataPtr);
VOID  FchInitEnvSata                   (IN VOID  *FchDataPtr);
VOID  FchInitEnvProgramSataPciRegs     (IN VOID  *FchDataPtr);
VOID  FchInitMidProgramSataRegs        (IN VOID  *FchDataPtr);
VOID  FchInitLateProgramSataRegs       (IN VOID  *FchDataPtr);

VOID  FchInitLateSata           (IN VOID  *FchDataPtr);
VOID  FchInitEnvSataIde         (IN VOID  *FchDataPtr);
VOID  FchInitMidSataIde         (IN VOID  *FchDataPtr);
VOID  FchInitLateSataIde        (IN VOID  *FchDataPtr);
VOID  FchInitEnvSataAhci        (IN VOID  *FchDataPtr);
VOID  FchInitMidSataAhci        (IN VOID  *FchDataPtr);
VOID  FchInitLateSataAhci       (IN VOID  *FchDataPtr);
VOID  FchInitEnvSataRaid        (IN VOID  *FchDataPtr);
VOID  FchInitMidSataRaid        (IN VOID  *FchDataPtr);
VOID  FchInitLateSataRaid       (IN VOID  *FchDataPtr);
VOID  FchInitEnvSataIde2Ahci    (IN VOID  *FchDataPtr);
VOID  FchInitMidSataIde2Ahci    (IN VOID  *FchDataPtr);
VOID  FchInitLateSataIde2Ahci   (IN VOID  *FchDataPtr);

VOID SataAhciSetDeviceNumMsi     (IN VOID  *FchDataPtr);
VOID SataRaidSetDeviceNumMsi     (IN VOID  *FchDataPtr);
VOID SataIde2AhciSetDeviceNumMsi (IN VOID  *FchDataPtr);
VOID  SataSetIrqIntResource (IN VOID  *FchDataPtr, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  SataBar5setting (IN VOID *FchDataPtr, IN UINT32 *Bar5Ptr);
VOID  SataEnableWriteAccess (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  SataDisableWriteAccess (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  SataSetDeviceNumMsi (IN VOID  *FchDataPtr);
VOID  FchSataSetDeviceNumMsi (IN VOID  *FchDataPtr);
VOID  ShutdownUnconnectedSataPortClock (IN VOID *FchDataPtr, IN UINT32 Bar5);
VOID  FchShutdownUnconnectedSataPortClock (IN VOID *FchDataPtr, IN UINT32 Bar5);
VOID  SataDriveDetection (IN VOID *FchDataPtr, IN UINT32 *Bar5Ptr);
VOID  FchSataDriveDetection (IN VOID *FchDataPtr, IN UINT32 *Bar5Ptr);
VOID  SataBar5RegSet (IN  VOID  *FchDataPtr);
VOID  SataSetPortGenMode (IN  VOID  *FchDataPtr);
VOID  FchSataSetPortGenMode (IN  VOID  *FchDataPtr);
VOID  FchProgramSataPhy (IN VOID  *FchDataPtr);
VOID  FchInitEnvSataRaidProgram  (IN VOID  *FchDataPtr);

///
/// FCH USB Controller Public Function
///
///  Pei Phase
///
VOID  FchInitResetUsb            (IN VOID  *FchDataPtr);
VOID  FchInitResetEhci           (IN VOID  *FchDataPtr);
VOID  FchInitResetXhci           (IN VOID  *FchDataPtr);
VOID  FchInitResetXhciProgram    (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvUsb              (IN VOID  *FchDataPtr);
VOID  FchInitMidUsb              (IN VOID  *FchDataPtr);
VOID  FchInitLateUsb             (IN VOID  *FchDataPtr);
VOID  FchInitEnvUsbEhci          (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbEhci          (IN VOID  *FchDataPtr);
VOID  FchInitLateUsbEhci         (IN VOID  *FchDataPtr);
VOID  FchEhciDebugPortService    (IN VOID  *FchDataPtr);
VOID  FchInitEnvUsbXhci          (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbXhci          (IN VOID  *FchDataPtr);
VOID  FchInitLateUsbXhci         (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbEhci1         (IN FCH_DATA_BLOCK  *FchDataPtr);
///
///  Other Public Routines
///
VOID  FchSetUsbEnableReg         (IN FCH_DATA_BLOCK *FchDataPtr);
VOID  FchEhciInitAfterPciInit    (IN UINT32 Value, IN FCH_DATA_BLOCK* FchDataPtr);
VOID  FchXhciInitBeforePciInit   (IN FCH_DATA_BLOCK* FchDataPtr);
VOID  FchXhciInitIndirectReg     (IN FCH_DATA_BLOCK* FchDataPtr);
VOID  FchInitLateUsbXhciProgram  (IN VOID  *FchDataPtr);
VOID  FchXhciPowerSavingProgram  (IN FCH_DATA_BLOCK* FchDataPtr);
VOID  FchXhciUsbPhyCalibrated    (IN FCH_DATA_BLOCK* FchDataPtr);
UINT8 FchUsbCommonPhyCalibration (IN FCH_DATA_BLOCK* FchDataPtr);

///
/// Fch Sd Routines
///
VOID  FchInitEnvSd  (IN VOID  *FchDataPtr);
VOID  FchInitMidSd  (IN VOID  *FchDataPtr);
VOID  FchInitLateSd (IN VOID  *FchDataPtr);

///
///  Other Public Routines
///

VOID FchInitEnvSdProgram (IN VOID  *FchDataPtr);

///
/// Fch Spi Routines
///
///  Pei Phase
///
VOID  FchInitResetSpi        (IN VOID  *FchDataPtr);
VOID  FchInitResetLpc        (IN VOID  *FchDataPtr);
VOID  FchInitResetLpcProgram (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvSpi          (IN VOID  *FchDataPtr);
VOID  FchInitMidSpi          (IN VOID  *FchDataPtr);
VOID  FchInitLateSpi         (IN VOID  *FchDataPtr);
VOID  FchInitEnvLpc          (IN VOID  *FchDataPtr);
VOID  FchInitMidLpc          (IN VOID  *FchDataPtr);
VOID  FchInitLateLpc         (IN VOID  *FchDataPtr);
VOID  FchInitEnvLpcProgram   (IN VOID  *FchDataPtr);
///
///  Other Public Routines
///
VOID  FchSpiUnlock       (IN VOID  *FchDataPtr);
VOID  FchSpiLock         (IN VOID  *FchDataPtr);
VOID  FchUsb3D3ColdCallback     (IN VOID  *FchDataPtr);
VOID  FchUsb3D0Callback         (IN VOID  *FchDataPtr);

/*--------------------------- Documentation Pages ---------------------------*/
VOID  FchStall (IN UINT32 uSec, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  CimFchStall (IN UINT32 uSec, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchPciReset (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  OutPort80 (IN UINT32 pcode, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  OutPort1080 (IN UINT32 pcode, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  TurnOffCG2 (OUT VOID);
VOID  BackUpCG2 (OUT VOID);
VOID  FchCopyMem (IN VOID* pDest, IN VOID* pSource, IN UINTN Length);
VOID* GetRomSigPtr (IN UINTN* RomSigPtr, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  ReadXhci0Phy (IN UINT32 Port, IN UINT32 Address, IN UINT32 *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  ReadXhci1Phy (IN UINT32 Port, IN UINT32 Address, IN UINT32 *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  AcLossControl (IN UINT8 AcLossControlValue);
VOID  FchVgaInit (OUT VOID);
VOID  RecordFchConfigPtr (IN UINT32 FchConfigPtr);
VOID  ValidateFchVariant (IN VOID  *FchDataPtr);
VOID  RecordSmiStatus (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  ClearAllSmiStatus (IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN  IsExternalClockMode (IN VOID  *FchDataPtr);
VOID  SbSleepTrapControl (IN BOOLEAN SleepTrap);

AGESA_STATUS
FchSpiTransfer (
  IN       UINT8    PrefixCode,
  IN       UINT8    Opcode,
  IN  OUT  UINT8    *DataPtr,
  IN       UINT8    *AddressPtr,
  IN       UINT8    Length,
  IN       BOOLEAN  WriteFlag,
  IN       BOOLEAN  AddressFlag,
  IN       BOOLEAN  DataFlag,
  IN       BOOLEAN  FinishedFlag
  );

BOOLEAN
FchConfigureSpiDeviceDummyCycle (
  IN       UINT32     DeviceID,
  IN       UINT8      SpiMode
  );

UINT32
FchReadSpiId (
  IN       BOOLEAN    Flag
  );

BOOLEAN
FchPlatformSpiQe (
  IN       VOID     *FchDataPtr
  );

FCH_DATA_BLOCK*
FchInitLoadDataBlock (
  IN       FCH_INTERFACE        *FchInterface,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

FCH_DATA_BLOCK*
FchInitEnvCreatePrivateData (
  IN       AMD_ENV_PARAMS        *EnvParams
  );

FCH_RESET_DATA_BLOCK*
FchInitResetLoadPrivateDefault (
  IN       AMD_RESET_PARAMS      *ResetParams
  );

VOID
RetrieveDataBlockFromInitReset (
  IN       FCH_DATA_BLOCK       *FchParams
  );

#endif
