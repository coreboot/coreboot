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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
****************************************************************************
*/
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
VOID          RwPci (IN UINT32 Address, IN UINT8 OpFlag, IN UINT32 Mask, IN UINT32  Data, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ProgramPciByteTable (IN REG8_MASK* pPciByteTable, IN UINT16 dwTableSize, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ProgramFchAcpiMmioTbl (IN ACPI_REG_WRITE  *pAcpiTbl, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ProgramFchSciMapTbl (IN SCI_MAP_CONTROL  *pSciMapTbl, IN FCH_RESET_DATA_BLOCK *FchResetDataBlock);
VOID          ProgramFchGpioTbl (IN GPIO_CONTROL  *pGpioTbl, IN FCH_RESET_DATA_BLOCK *FchResetDataBlock);
VOID          ProgramFchSataPhyTbl (IN SATA_PHY_CONTROL  *pSataPhyTbl, IN FCH_RESET_DATA_BLOCK *FchResetDataBlock);
VOID          GetChipSysMode (IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN       IsImcEnabled (IN AMD_CONFIG_PARAMS *StdHeader);
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
///  Pei Phase
///
VOID  FchInitResetPcie    (IN VOID* FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvPcie        (IN VOID* FchDataPtr);
VOID  FchInitMidPcie        (IN VOID* FchDataPtr);
VOID  FchInitLatePcie       (IN VOID* FchDataPtr);
VOID  ProgramPcieNativeMode (IN VOID* FchDataPtr);

///
/// Fch Gpp Routines
///
///  Pei Phase
///
VOID  FchInitResetGpp (IN VOID* FchDataPtr);
VOID  ProgramFchGppInitReset (IN FCH_GPP *FchGpp, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchResetPcie (IN RESET_BLOCK  ResetBlock, IN RESET_OP ResetOp, IN AMD_CONFIG_PARAMS *StdHeader);

///
///  Dxe Phase
///
VOID  FchInitEnvGpp (IN VOID* FchDataPtr);
VOID  FchInitMidGpp (IN VOID* FchDataPtr);
VOID  FchInitLateGpp (IN VOID* FchDataPtr);

///
///  Common Gpp Routines
///
VOID  ProgramGppTogglePcieReset (IN BOOLEAN DoToggling, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchGppForceGen1 (IN FCH_GPP *FchGpp, IN CONST UINT8 ActivePorts, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchGppForceGen2 (IN FCH_GPP *FchGpp, IN CONST UINT8 ActivePorts, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchGppDynamicPowerSaving (IN FCH_GPP *FchGpp, IN AMD_CONFIG_PARAMS *StdHeader);
UINT8 GppPortPollingLtssm (IN FCH_GPP *FchGpp, IN UINT8 ActivePorts, IN BOOLEAN IsGen2, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  GppGen2Workaround (IN FCH_GPP *FchGpp, IN AMD_CONFIG_PARAMS *StdHeader);
UINT8 FchFindPciCap (IN UINT32 PciAddress, IN UINT8 TargetCapId, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchGppPortInit (IN FCH_GPP *FchGpp, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchGppPortInitPhaseII (IN FCH_GPP *FchGpp, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchGppPortInitS3Phase (IN FCH_GPP *FchGpp, IN AMD_CONFIG_PARAMS *StdHeader);
UINT32 GppGetFchTempBus (IN AMD_CONFIG_PARAMS *StdHeader);

///
///
///  Pei Phase
///
VOID  Fchdef174     (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  Fchdef178       (IN VOID  *FchDataPtr);
VOID  Fchdef179       (IN VOID  *FchDataPtr);
VOID  Fchdef180      (IN VOID  *FchDataPtr);


///
/// Fch GEC Routines
///
///  Pei Phase
///
VOID  FchInitResetGec (IN VOID*   FchDataPtr);
///
///  Dxe Phase
///
VOID FchInitEnvGec (IN VOID*   FchDataPtr);
VOID FchInitMidGec (IN VOID*   FchDataPtr);
VOID FchInitLateGec (IN VOID*   FchDataPtr);
///
///  Other Public Routines
///
VOID FchInitGecController (IN VOID*   FchDataPtr);
VOID FchSwInitGecBootRom (IN VOID*   FchDataPtr);

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
VOID C3PopupSetting         (IN VOID  *FchDataPtr);
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
VOID  HwmProcessParameter (IN VOID* FchDataPtr);
VOID  HwmSetRegister (IN VOID* FchDataPtr);
VOID  HwmGetCalibrationFactor (IN VOID* FchDataPtr);
VOID  HwmFchtsiAutoPolling (IN VOID* FchDataPtr);
VOID  HwmGetRawData (IN VOID* FchDataPtr);
VOID  HwmCaculate (IN VOID* FchDataPtr);
VOID  HwmFchtsiAutoPollingOff (IN VOID* FchDataPtr);
VOID  FchECfancontrolservice (IN VOID* FchDataPtr);


///
/// Fch Ide Routines
///
VOID  FchInitEnvIde (IN VOID* FchDataPtr);
VOID  FchInitMidIde (IN VOID* FchDataPtr);
VOID  FchInitLateIde (IN VOID* FchDataPtr);


///
/// Fch Imc Routines
///
///  Pei Phase
///
VOID    FchInitResetImc    (IN VOID  *FchDataPtr);
VOID    FchInitResetEc     (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID    FchInitEnvImc      (IN VOID  *FchDataPtr);
VOID    FchInitMidImc      (IN VOID  *FchDataPtr);
VOID    FchInitLateImc     (IN VOID  *FchDataPtr);
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

VOID    ImcSleep                   (IN VOID  *FchDataPtr);
VOID    ImcEnableSurebootTimer     (IN VOID  *FchDataPtr);
VOID    ImcDisarmSurebootTimer     (IN VOID  *FchDataPtr);
VOID    ImcDisableSurebootTimer    (IN VOID  *FchDataPtr);
VOID    ImcWakeup                  (IN VOID  *FchDataPtr);
VOID    ImcIdle                    (IN VOID  *FchDataPtr);
BOOLEAN ValidateImcFirmware        (IN VOID  *FchDataPtr);
VOID    SoftwareToggleImcStrapping (IN VOID  *FchDataPtr);


///
/// Fch Ir Routines
///
///  Dxe Phase
///
VOID  FchInitEnvIr (IN VOID* FchDataPtr);
VOID  FchInitMidIr (IN VOID* FchDataPtr);
VOID  FchInitLateIr (IN VOID* FchDataPtr);

///
/// Fch Pcib Routines
///
///  Pei Phase
///
VOID  FchInitResetPcib (IN VOID* FchDataPtr);
VOID  FchInitResetPcibPort80Enable (IN VOID* FchDataPtr);

///
///  Dxe Phase
///
VOID  FchInitEnvPcib (IN VOID* FchDataPtr);
VOID  FchInitMidPcib (IN VOID* FchDataPtr);
VOID  FchInitLatePcib (IN VOID* FchDataPtr);


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
VOID  FchSataGpioInitial (IN  VOID  *FchDataPtr);
VOID  SataBar5RegSet (IN  VOID  *FchDataPtr);
VOID  SataSetPortGenMode (IN  VOID  *FchDataPtr);
VOID  FchSataSetPortGenMode (IN  VOID  *FchDataPtr);
VOID  FchProgramSataPhy (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchSataDriveFpga (IN  VOID  *FchDataPtr);
VOID  FchInitEnvSataRaidProgram  (IN VOID  *FchDataPtr);

///
/// FCH USB Controller Public Function
///
///  Pei Phase
///
VOID  FchInitResetUsb            (IN VOID  *FchDataPtr);
VOID  FchInitResetOhci           (IN VOID  *FchDataPtr);
VOID  FchInitResetEhci           (IN VOID  *FchDataPtr);
VOID  FchInitResetXhci           (IN VOID  *FchDataPtr);
VOID  FchInitResetXhciProgram    (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvUsb              (IN VOID  *FchDataPtr);
VOID  FchInitMidUsb              (IN VOID  *FchDataPtr);
VOID  FchInitLateUsb             (IN VOID  *FchDataPtr);
VOID  FchInitEnvUsbOhci          (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbOhci          (IN VOID  *FchDataPtr);
VOID  FchInitLateUsbOhci         (IN VOID  *FchDataPtr);
VOID  FchInitEnvUsbEhci          (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbEhci          (IN VOID  *FchDataPtr);
VOID  FchInitLateUsbEhci         (IN VOID  *FchDataPtr);
VOID  FchInitEnvUsbXhci          (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbXhci          (IN VOID  *FchDataPtr);
VOID  FchInitLateUsbXhci         (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbOhci1         (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbOhci2         (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbOhci3         (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbOhci4         (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbEhci1         (IN FCH_DATA_BLOCK  *FchDataPtr);
VOID  FchInitMidUsbEhci2         (IN FCH_DATA_BLOCK  *FchDataPtr);
VOID  FchInitMidUsbEhci3         (IN FCH_DATA_BLOCK  *FchDataPtr);
///
///  Other Public Routines
///
VOID  FchSetUsbEnableReg         (IN FCH_DATA_BLOCK *FchDataPtr);
VOID  FchOhciInitAfterPciInit    (IN UINT32 Value, IN FCH_DATA_BLOCK* FchDataPtr);
VOID  FchEhciInitAfterPciInit    (IN UINT32 Value, IN FCH_DATA_BLOCK* FchDataPtr);
VOID  FchXhciInitBeforePciInit   (IN FCH_DATA_BLOCK* FchDataPtr);
VOID  FchXhciInitIndirectReg     (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchInitLateUsbXhciProgram  (IN VOID  *FchDataPtr);
VOID  FchXhciPowerSavingProgram  (IN FCH_DATA_BLOCK* FchDataPtr);



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

/*--------------------------- Documentation Pages ---------------------------*/
VOID  FchStall (IN UINT32 uSec, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  CimFchStall (IN UINT32 uSec, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchPciReset (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  OutPort80 (IN UINT32 pcode, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  OutPort1080 (IN UINT32 pcode, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  GetEfuseStatus (IN VOID* Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  TurnOffCG2 (OUT VOID);
VOID  BackUpCG2 (OUT VOID);
VOID  FchCopyMem (IN VOID* pDest, IN VOID* pSource, IN UINTN Length);
VOID* GetRomSigPtr (IN UINTN* RomSigPtr, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  RwXhciIndReg (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  RwXhci0IndReg (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  RwXhci1IndReg (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  AcLossControl (IN UINT8 AcLossControlValue);
VOID  FchVgaInit (OUT VOID);
VOID  RecordFchConfigPtr (IN UINT32 FchConfigPtr);
VOID  ValidateFchVariant (IN VOID  *FchDataPtr);
VOID  RecordSmiStatus (IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN  IsGCPU (IN VOID  *FchDataPtr);
BOOLEAN  IsExternalClockMode (IN VOID  *FchDataPtr);
BOOLEAN  IsLpcRom (OUT VOID);
VOID  SbSleepTrapControl (IN BOOLEAN SleepTrap);

#endif

