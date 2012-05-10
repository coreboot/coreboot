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
 * @e \$Revision: 49633 $   @e \$Date: 2011-03-26 06:52:29 +0800 (Sat, 26 Mar 2011) $
 *
 */
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
    0xFEC00000,              // WatchDogTimerBase
    0x400,                   // AcpiPm1EvtBlkAddr
    0x404,                   // AcpiPm1CntBlkAddr
    0x408,                   // AcpiPmTmrBlkAddr
    0x410,                   // CpuControlBlkAddr
    0x420,                   // AcpiGpe0BlkAddr
    0x00B0,                  // SmiCmdPortAddr
    0xFE00,                  // AcpiPmaCntBlkAddr
    TRUE,                    // AnyHt200MhzLink
    TRUE,                    // SpreadSpectrum
    UsePrevious,             // PwrFailShadow
    0,                       // StressResetMode
    FALSE,                   // MtC1eEnable
    NULL                     // OemProgrammingTablePtr
  },

  {                          // FCH_AB
    FALSE,                   // AbMsiEnable
    0,                       // AlinkPhyPllPowerDown
    0,                       // ALinkClkGateOff
    0,                       // BLinkClkGateOff
    0,                       // AbClockGating
    0,                       // GppClockGating
    0,                       // UmiL1TimerOverride
    0,                       // UmiLinkWidth
    0,                       // UmiDynamicSpeedChange
    0,                       // PcieRefClockOverClocking
    0,                       // UmiGppTxDriverStrength
    FALSE,                   // NbSbGen2
    0,                       // FchPcieOrderRule
    0,                       // SlowSpeedAbLinkClock
    0                        // ResetCpuOnSyncFlood
  },

  {                          // FCH_GPP
    {                        // Array of FCH_GPP_PORT_CONFIG       PortCfg[4]
      {
        FALSE,               // PortPresent
        FALSE,               // PortDetected
        FALSE,               // PortIsGen2
        FALSE,               // PortHotPlug
        0,                   // PortMisc
      },
      {
        FALSE,               // PortPresent
        FALSE,               // PortDetected
        FALSE,               // PortIsGen2
        FALSE,               // PortHotPlug
        0,                   // PortMisc
      },
      {
        FALSE,               // PortPresent
        FALSE,               // PortDetected
        FALSE,               // PortIsGen2
        FALSE,               // PortHotPlug
        0,                   // PortMisc
      },
      {
        FALSE,               // PortPresent
        FALSE,               // PortDetected
        FALSE,               // PortIsGen2
        FALSE,               // PortHotPlug
        0,                   // PortMisc
      },
    },
    PortA4,                  // GppLinkConfig
    0,                       // GppFoundGfxDev
    0,                       // GppGen2
    0,                       // GppGen2Strap
    0,                       // GppMemWrImprove
    FALSE,                   // GppFunctionEnable
    0,                       // GppUnhidePorts
    0,                       // GppPortAspm
    0,                       // GppLaneReversal
    0,                       // GppPhyPllPowerDown
    0,                       // GppDynamicPowerSaving
    0,                       // PcieAer
    0,                       // PcieRas
    0,                       // PcieCompliance
    0,                       // PcieSoftwareDownGrade
    0,                       // GppHardwareDownGrade
    FALSE,                   // GppToggleReset
  },

  {                          // FCH_USB
    TRUE,                    // Ohci1Enable
    TRUE,                    // Ohci2Enable
    TRUE,                    // Ohci3Enable
    TRUE,                    // Ohci4Enable
    TRUE,                    // Ehci1Enable
    TRUE,                    // Ehci2Enable
    TRUE,                    // Ehci3Enable
    FALSE,                   // Xhci0Enable
    FALSE,                   // Xhci1Enable
    FALSE,                   // UsbMsiEnable
    0,                       // OhciSsid
    0,                       // Ohci4Ssid
    0,                       // EhciSsid
    0,                       // XhciSsid
    FALSE                    // UsbPhyPowerDown
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
      TRUE,                  // SataSetMaxGen2
      TRUE,                  // IdeEnable
      0,                     // SataClkMode
    },
    0,                       // SataClass
    0,                       // SataIdeMode
    0,                       // SataDisUnusedIdePChannel
    0,                       // SataDisUnusedIdeSChannel
    0,                       // IdeDisUnusedIdePChannel
    0,                       // IdeDisUnusedIdeSChannel
    0,                       // SataOptionReserved
    {                        // SATA_PORT_ST
      0,                     // SataPortReg
      TRUE,                  // Port0
      TRUE,                  // Port1
      TRUE,                  // Port2
      TRUE,                  // Port3
      TRUE,                  // Port4
      TRUE,                  // Port5
      TRUE,                  // Port6
      TRUE,                  // Port7
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
    0,                       // SataAhciEnclosureManagement:1
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
    AzDisable,               // AzaliaEnable
    FALSE,                   // AzaliaMsiEnable
    0x00000000,              // AzaliaSsid
    0,                       // AzaliaPinCfg
    0,                       // AzaliaFrontPanel
    0,                       // FrontPanelDetected
    0,                       // AzaliaSnoop
    0,                       // AzaliaDummy
    {                        // AZALIA_PIN
      0,                     // AzaliaSdin0
      0,                     // AzaliaSdin1
      0,                     // AzaliaSdin2
      0,                     // AzaliaSdin3
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
    SdDisable,               // SdConfig
    0,                       // Speed
    0,                       // BitWidth
  },

  {0},                       // FCH_HWM
  {0},                       // FCH_IR
  {                          // FCH_HPET
    FALSE,                   // HpetEnable
    0,                       // HpetTimer
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
  {FALSE},                   // FCH_SERIALDB

  {                          // FCH_MISC
    FALSE,                   // NativePcieSupport
    FALSE,                   // S3Resume
    FALSE,                   // RebootRequired
    0,                       // FchVariant
    0,                       // CG2PLL
  }
};


