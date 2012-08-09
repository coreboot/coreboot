/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics Controller family specific service procedure
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
* ***************************************************************************
*
*/


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "FchPlatform.h"
#include  "Filecode.h"
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 * Default FCH interface settings at InitEnv phase.
 *----------------------------------------------------------------------------------------
 */
CONST FCH_INTERFACE ROMDATA FchInterfaceDefault = {
  SdAmda,              // SdConfig
  AzEnable,            // AzaliaControl
  IrRxTx0Tx1,          // IrConfig
  TRUE,                // UmiGen2
  SataAhci,            // SataClass
  TRUE,                // SataEnable
  TRUE,                // IdeEnable
  TRUE,                // SataIdeMode
  TRUE,                // Ohci1Enable
  TRUE,                // Ohci2Enable
  TRUE,                // Ohci3Enable
  TRUE,                // Ohci4Enable
  TRUE,                // XhciSwitch
  TRUE,                // GppEnable
  AlwaysOff            // FchPowerFail
};


/*----------------------------------------------------------------
 *  InitEnv Phase Data Block Default (Failsafe)
 *----------------------------------------------------------------
 */
FCH_DATA_BLOCK   InitEnvCfgDefault = {
  NULL,                      // StdHeader

  {                          // FCH_ACPI
    0xB00,                   // Smbus0BaseAddress
    0xB20,                   // Smbus1BaseAddress
    0xE00,                   // SioPmeBaseAddress
    0xFEC000F0,              // WatchDogTimerBase
    0x800,                   // AcpiPm1EvtBlkAddr
    0x804,                   // AcpiPm1CntBlkAddr
    0x808,                   // AcpiPmTmrBlkAddr
    0x810,                   // CpuControlBlkAddr
    0x820,                   // AcpiGpe0BlkAddr
    0x00B0,                  // SmiCmdPortAddr
    0xFE00,                  // AcpiPmaCntBlkAddr
    TRUE,                    // AnyHt200MhzLink
    TRUE,                    // SpreadSpectrum
    AlwaysOff,             // PwrFailShadow
    0,                       // StressResetMode
    FALSE,                   // MtC1eEnable
    NULL                     // OemProgrammingTablePtr
  },

  {                          // FCH_AB
    FALSE,                   // AbMsiEnable
    0,                       // ALinkClkGateOff
    0,                       // BLinkClkGateOff
    0,                       // AbClockGating
    0,                       // GppClockGating
    0,                       // UmiL1TimerOverride
    0,                       // UmiLinkWidth
    0,                       // UmiDynamicSpeedChange
    0,                       // PcieRefClockOverClocking
    0,                       // UmiGppTxDriverStrength
    TRUE,                    // NbSbGen2
    0,                       // FchPcieOrderRule
    0,                       // SlowSpeedAbLinkClock
    0,                       // ResetCpuOnSyncFlood
    FALSE,                   // AbDmaMemoryWrtie3264B
    FALSE,                   // AbMemoryPowerSaving
    FALSE,                   // SbgDmaMemoryWrtie3264ByteCount
    FALSE                    // SbgMemoryPowerSaving
  },

  {{{0}}},                       // FCH_GPP

  {                          // FCH_USB
    TRUE,                    // Ohci1Enable
    TRUE,                    // Ohci2Enable
    TRUE,                    // Ohci3Enable
    TRUE,                    // Ohci4Enable
    TRUE,                    // Ehci1Enable
    TRUE,                    // Ehci2Enable
    TRUE,                    // Ehci3Enable
    TRUE,                    // Xhci0Enable
    TRUE,                    // Xhci1Enable
    FALSE,                   // UsbMsiEnable
    0,                       // OhciSsid
    0,                       // Ohci4Ssid
    0,                       // EhciSsid
    0,                       // XhciSsid
    FALSE,                   // UsbPhyPowerDown
    0                        // UserDefineXhciRomAddr
  },

  {                          // FCH_SATA
    FALSE,                   // SataMsiEnable
    0x00000000,              // SataIdeSsid
    0x00000000,              // SataRaidSsid
    0x00000000,              // SataRaid5Ssid
    0x00000000,              // SataAhciSsid
    {                        // SATA_ST
      0,                     // SataModeReg
      TRUE,                  // SataEnable
      0,                     // Sata6AhciCap
      FALSE,                  // SataSetMaxGen2
      TRUE,                  // IdeEnable
      9,                     // SataClkMode
    },
    SataAhci,           // SataClass
    1,                       // SataIdeMode
    0,                       // SataDisUnusedIdePChannel
    0,                       // SataDisUnusedIdeSChannel
    0,                       // IdeDisUnusedIdePChannel
    0,                       // IdeDisUnusedIdeSChannel
    0,                       // SataOptionReserved
    {                        // SATA_PORT_ST
      0,                     // SataPortReg
      FALSE,                  // Port0
      FALSE,                  // Port1
      FALSE,                  // Port2
      FALSE,                  // Port3
      FALSE,                  // Port4
      FALSE,                  // Port5
      FALSE,                  // Port6
      FALSE,                  // Port7
    },
    {                        // SATA_PORT_ST
      0,                     // SataPortReg
      FALSE,                 // Port0
      FALSE,                 // Port1
      FALSE,                 // Port2
      FALSE,                 // Port3
      FALSE,                 // Port4
      FALSE,                 // Port5
      FALSE,                 // Port6
      FALSE,                 // Port7
    },
    {                        // SATA_PORT_MD
      0,                     // SataPortMode
      0,                     // Port0
      0,                     // Port1
      0,                     // Port2
      0,                     // Port3
      0,                     // Port4
      0,                     // Port5
      0,                     // Port6
      0,                     // Port7
    },
    0,                       // SataAggrLinkPmCap
    0,                       // SataPortMultCap
    0,                       // SataClkAutoOff
    0,                       // SataPscCap
    0,                       // BiosOsHandOff
    0,                       // SataFisBasedSwitching
    0,                       // SataCccSupport
    0,                       // SataSscCap
    0,                       // SataMsiCapability
    0,                       // SataForceRaid
    0,                       // SataInternal100Spread
    0,                       // SataDebugDummy
    0,                       // SataTargetSupport8Device
    0,                       // SataDisableGenericMode
    FALSE,                   // SataAhciEnclosureManagement
    0,                       // SataSgpio0
    0,                       // SataSgpio1
    0,                       // SataPhyPllShutDown
    TRUE,                    // SataHotRemovalEnh
    {                        // SATA_PORT_ST
      0,                     // SataPortReg
      FALSE,                 // Port0
      FALSE,                 // Port1
      FALSE,                 // Port2
      FALSE,                 // Port3
      FALSE,                 // Port4
      FALSE,                 // Port5
      FALSE,                 // Port6
      FALSE,                 // Port7
    },
    0                        // TempMmio
  },

  {                          // FCH_SMBUS
    0x00000000               // SmbusSsid
  },

  {                          // FCH_IDE
    TRUE,                    // IdeEnable
    FALSE,                   // IdeMsiEnable
    0x00000000               // IdeSsid
  },

  {                          // FCH_AZALIA
    AzEnable,               // AzaliaEnable
    FALSE,                   // AzaliaMsiEnable
    0x00000000,              // AzaliaSsid
    1,                       // AzaliaPinCfg
    0,                       // AzaliaFrontPanel
    0,                       // FrontPanelDetected
    0,                       // AzaliaSnoop
    0,                       // AzaliaDummy
    {                        // AZALIA_PIN
      2,                     // AzaliaSdin0
      2,                     // AzaliaSdin1
      2,                     // AzaliaSdin2
      2,                     // AzaliaSdin3
    },
    NULL,                    // *AzaliaOemCodecTablePtr
    NULL,                    // *AzaliaOemFpCodecTablePtr
  },

  {                          // FCH_SPI
    FALSE,                   // LpcMsiEnable
    0x00000000,              // LpcSsid
    0,                       // RomBaseAddress
    0,                       // Speed
    0,                       // FastSpeed
    0,                       // WriteSpeed
    0,                       // Mode
    0,                       // AutoMode
    0,                       // BurstWrite
  },

  {                          // FCH_PCIB
    FALSE,                   // PcibMsiEnable
    0x00000000,              // PcibSsid
    0x0F,                    // PciClks
    0,                       // PcibClkStopOverride
    FALSE,                   // PcibClockRun
  },

  {                          // FCH_GEC
    FALSE,                   // GecEnable
    0,                       // GecPhyStatus
    0,                       // GecPowerPolicy
    0,                       // GecDebugBus
    0xFED61000,              // GecShadowRomBase
    NULL,                    // *PtrDynamicGecRomAddress
  },

  {                          // FCH_SD
    SdAmda,               // SdConfig
    0,                       // Speed
    0,                       // BitWidth
    0x00000000,              // SdSsid
    Sd50MhzTraceCableLengthWithinSixInches,   // SdClockControl
    FALSE,
    0,
    0
  },

  {0},                       // FCH_HWM

  {0,                        // FCH_IR
   0x23,                     // IrPinControl
  },

  {                          // FCH_HPET
    TRUE,                    // HpetEnable
    TRUE,                    // HpetMsiDis
    0xFED00000               // HpetBase
  },

  {                          // FCH_GCPU
    0,                       // AcDcMsg
    0,                       // TimerTickTrack
    0,                       // ClockInterruptTag
    0,                       // OhciTrafficHanding
    0,                       // EhciTrafficHanding
    0,                       // GcpuMsgCMultiCore
    0,                       // GcpuMsgCStage
  },

  {0},                       // FCH_IMC

  {                          // FCH_MISC
    FALSE,                   // NativePcieSupport
    FALSE,                   // S3Resume
    FALSE,                   // RebootRequired
    0,                       // FchVariant
    0,                       // CG2PLL
    {                        // TIMER_SMI-LongTimer
      FALSE,                 // Enable
      FALSE,                 // StartNow
      1000                   // CycleDuration
    },
    {                        // TIMER_SMI-ShortTimer
      FALSE,                 // Enable
      FALSE,                 // StartNow
      0x7FFF                 // CycleDuration
    }
  }
};


