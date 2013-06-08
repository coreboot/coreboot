/* $NoKeywords:$ */
/**
 * @file
 *
 * IDS NV definition for F15Tn
 *
 * Auto generated from CBS XML file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS F15Tn
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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


#ifndef _IDSF15TNNVDEF_H_
#define _IDSF15TNNVDEF_H_
///HTC Enable
///Enable or disable Hardware Thermal Control. D18F3x64[0]
typedef enum {
  IdsNvThermalHTCEnDisabled = 0,///<Disabled
  IdsNvThermalHTCEnEnabled = 1,///<Enabled
  IdsNvThermalHTCEnAuto = 3,///<Auto
} IdsNvThermalHTCEn;

///HTC Override
///Enable or disable Thermal Control Override
typedef enum {
  IdsNvThermalHTCOverrideDisabled = 0,///<Disabled
  IdsNvThermalHTCOverrideEnabled = 1,///<Enabled
} IdsNvThermalHTCOverride;

///HTC P-state limit select
///Specifies the P-state limit of all cores when in the P-state based HTC-active state
#define IdsNvThermalHtcPstateLimitMin 0
#define IdsNvThermalHtcPstateLimitMax 7

///HTC Temperature Hysteresis
///Sets the temperature hysteresis for HTC. D18F3x64[27:24]
#define IdsNvThermalHTCTempHysMin 0
#define IdsNvThermalHTCTempHysMax 0xF

///HTC Activation Temp
///Sets the HTC temperature limit. D18F3x64[22:16]
#define IdsNvThermalHTCActTempMin 0
#define IdsNvThermalHTCActTempMax 0x7F

///Bank Interleave
///Interleave memory blocks across the DRAM chip selects for node 0.
typedef enum {
  IdsNvMemMappingBankInterleaveDisabled = 0,///<Disabled
  IdsNvMemMappingBankInterleaveAuto = 1,///<Auto
} IdsNvMemMappingBankInterleave;

///DRAM Channel Interleave
///Interleave between two DCTs when they are in unganged mode.
typedef enum {
  IdsNvMemMappingChlInterleaveAddress_bit_6 = 0,///<Address bit 6
  IdsNvMemMappingChlInterleaveAddress_bit_12 = 1,///<Address bit 12
  IdsNvMemMappingChlInterleaveHash__exclusive_OR_of_address_bits_20_16__6_ = 2,///<Hash: exclusive OR of address bits[20:16, 6]
  IdsNvMemMappingChlInterleaveHash__excluseive_OR_of_address_bits_20_16__9_ = 3,///<Hash: excluseive OR of address bits[20:16, 9]
  IdsNvMemMappingChlInterleaveAddress_bit_8 = 4,///<Address bit 8
  IdsNvMemMappingChlInterleaveAddress_bit_9 = 5,///<Address bit 9
  IdsNvMemMappingChlInterleaveDisabled = 0xF,///<Disabled
  IdsNvMemMappingChlInterleaveAuto = 0xFF,///<Auto
} IdsNvMemMappingChlInterleave;

///ECC Symbol Size
///ECC symbol size and code selection. D18F3x180[25]
typedef enum {
  IdsNvEccSymbolSizex4 = 0,///<x4
  IdsNvEccSymbolSizex8 = 1,///<x8
  IdsNvEccSymbolSizeAuto = 3,///<Auto
} IdsNvEccSymbolSize;

///DRAM prefetches triggered from CPU requests
///Enable or disable DRAM prefetches Prefetch triggered by CPU requests.
typedef enum {
  IdsNvPrefetchPrefCpuDis0 = 0,///<0
  IdsNvPrefetchPrefCpuDis1 = 1,///<1
  IdsNvPrefetchPrefCpuDisAuto = 3,///<Auto
} IdsNvPrefetchPrefCpuDis;

///HW prefetch training on SW Prefetches
///Enable or disable Hardware Prefetch training on Software Prefetches
typedef enum {
  IdsNvPrefetchDisHWPFforSWPF0 = 0,///<0
  IdsNvPrefetchDisHWPFforSWPF1 = 1,///<1
  IdsNvPrefetchDisHWPFforSWPFAuto = 3,///<Auto
} IdsNvPrefetchDisHWPFforSWPF;

///Hardware Prefetches
///Enable or disable Hardware Prefetches.
typedef enum {
  IdsNvPrefetchDisHWPF0 = 0,///<0
  IdsNvPrefetchDisHWPF1 = 1,///<1
  IdsNvPrefetchDisHWPFAuto = 3,///<Auto
} IdsNvPrefetchDisHWPF;

///UMI Gen2
///Enable or disable UMI link Gen2
typedef enum {
  IdsNvFchGppUmiGen2Disabled = 0,///<Disabled
  IdsNvFchGppUmiGen2Enabled = 1,///<Enabled
} IdsNvFchGppUmiGen2;

///SATA Controller
///Disable or enable OnChip SATA controller
typedef enum {
  IdsNvFchSataEnableDisabled = 0,///<Disabled
  IdsNvFchSataEnableEnabled = 1,///<Enabled
} IdsNvFchSataEnable;

///SATA Mode
///Select OnChip SATA Type
typedef enum {
  IdsNvFchSataClassNative_IDE = 0,///<Native IDE
  IdsNvFchSataClassRAID = 1,///<RAID
  IdsNvFchSataClassAHCI = 3,///<AHCI
  IdsNvFchSataClassLegacy_IDE = 3,///<Legacy IDE
  IdsNvFchSataClassIDE__AHCI = 4,///<IDE->AHCI
  IdsNvFchSataClassAHCI_as_ID_0x7804 = 5,///<AHCI as ID 0x7804
  IdsNvFchSataClassIDE__AHCI_as_ID_0x7804 = 6,///<IDE->AHCI as ID 0x7804
} IdsNvFchSataClass;

///OnChip IDE
///Select OnChip IDE controller mode
typedef enum {
  IdsNvFchSataIdeModeLegacy_IDE = 0,///<Legacy IDE
  IdsNvFchSataIdeModeNative_IDE = 1,///<Native IDE
} IdsNvFchSataIdeMode;

///IDE Controller
///Disable or enable OnChip IDE controller
typedef enum {
  IdsNvFchSataIdeEnableDisabled = 0,///<Disabled
  IdsNvFchSataIdeEnableEnabled = 1,///<Enabled
} IdsNvFchSataIdeEnable;

///XHC Switch (Bus 0 Dev 16 Fn 0/1)
///Select disable or enable XHCI HCs (Bus 0 Dev 16 Fn 0/1)
typedef enum {
  IdsNvFchUsbXhciSwitchDisabled = 0,///<Disabled
  IdsNvFchUsbXhciSwitchEnabled = 1,///<Enabled
} IdsNvFchUsbXhciSwitch;

///USB1(Bus 0 Dev 18 Fn 0/2)
///Select disable or enable USB1 HCs (Bus 0 Dev 18 Fn 0/2)
typedef enum {
  IdsNvFchUsbOhci1EnableDisabled = 0,///<Disabled
  IdsNvFchUsbOhci1EnableEnabled = 1,///<Enabled
} IdsNvFchUsbOhci1Enable;

///USB2 (Bus 0 Dev 19 Fn 0/2)
///Select disable or enable USB2 HCs (Bus 0 Dev 19 Fn 0/2)
typedef enum {
  IdsNvFchUsbOhci2EnableDisabled = 0,///<Disabled
  IdsNvFchUsbOhci2EnableEnabled = 1,///<Enabled
} IdsNvFchUsbOhci2Enable;

///USB3 (Bus 0 Dev 22 Fn 0/2)
///Select disable or enable USB3 HCs (Bus 0 Dev 22 Fn 0/2)
typedef enum {
  IdsNvFchUsbOhci3EnableDisabled = 0,///<Disabled
  IdsNvFchUsbOhci3EnableEnabled = 1,///<Enabled
} IdsNvFchUsbOhci3Enable;

///USB4 (Bus 0 Dev 20 Fn 5)
///Select disable or enable USB4 HC (Bus 0 Dev 20 Fn 5)
typedef enum {
  IdsNvFchUsbOhci4EnableDisabled = 0,///<Disabled
  IdsNvFchUsbOhci4EnableEnabled = 1,///<Enabled
} IdsNvFchUsbOhci4Enable;

///Hardware Monitor Enable
///Master switch to enable or disable hardware monitor function
typedef enum {
  IdsNvFchHwmEnableDisabled = 0,///<Disabled
  IdsNvFchHwmEnableEnabled = 1,///<Enabled
} IdsNvFchHwmEnable;

///In-Chip IR
///Enable or disable the In-Chip IR
typedef enum {
  IdsNvFchIrConfigDisabled = 0,///<Disabled
  IdsNvFchIrConfigRX_TX0_Only = 1,///<RX_TX0 Only
  IdsNvFchIrConfigRX_TX1_Only = 2,///<RX_TX1 Only
  IdsNvFchIrConfigRX__TX0__and_TX1 = 3,///<RX, TX0, and TX1
} IdsNvFchIrConfig;

///SD Configuration Mode
///Select SD Mode
typedef enum {
  IdsNvFchSdConfigDisabled = 0,///<Disabled
  IdsNvFchSdConfigADMA = 1,///<ADMA
  IdsNvFchSdConfigDMA = 2,///<DMA
  IdsNvFchSdConfigPIO = 3,///<PIO
} IdsNvFchSdConfig;

typedef enum {
  IdsNvFchAzaliaControlAuto = 0,///<Auto
  IdsNvFchAzaliaControlDisabled = 1,///<Disabled
  IdsNvFchAzaliaControlEnabled = 2,///<Enabled
  IdsNvFchAzaliaControlReserved = 3,///<Reserved
} IdsNvFchAzaliaControl;

///Integrated Graphics Controller
///Enable Integrate Graphics controller
typedef enum {
  IdsNvGnbGfxiGPU_CONTROLAuto = 0,///<Auto
  IdsNvGnbGfxiGPU_CONTROLDisabled = 1,///<Disabled
  IdsNvGnbGfxiGPU_CONTROLForces = 2,///<Forces
} IdsNvGnbGfxiGPU_CONTROL;

///UMA Frame buffer Size
///Set UMA FB size
typedef enum {
  IdsNvGnbGfxUmaFrameBufferSizeAuto = 0,///<Auto
  IdsNvGnbGfxUmaFrameBufferSize32M = 1,///<32M
  IdsNvGnbGfxUmaFrameBufferSize64M = 2,///<64M
  IdsNvGnbGfxUmaFrameBufferSize128M = 3,///<128M
  IdsNvGnbGfxUmaFrameBufferSize256M = 4,///<256M
  IdsNvGnbGfxUmaFrameBufferSize384M = 5,///<384M
  IdsNvGnbGfxUmaFrameBufferSize512M = 6,///<512M
  IdsNvGnbGfxUmaFrameBufferSize1G = 7,///<1G
  IdsNvGnbGfxUmaFrameBufferSize2G = 8,///<2G
  IdsNvGnbGfxUmaFrameBufferSize4G = 9,///<4G
} IdsNvGnbGfxUmaFrameBufferSize;

///Enable Integrate HD Audio controller
typedef enum {
  IdsNvGnbGfxNbAzaliaAuto = 0,///<Auto
  IdsNvGnbGfxNbAzaliaDisabled = 1,///<Disabled
  IdsNvGnbGfxNbAzaliaEnabled = 2,///<Enabled
} IdsNvGnbGfxNbAzalia;

///PSPP Policy
///PCIe speed power policy
typedef enum {
  IdsNvGnbPciePsppPolicyDisabled = 0,///<Disabled
  IdsNvGnbPciePsppPolicyPerformance = 1,///<Performance
  IdsNvGnbPciePsppPolicyBalanced_High = 2,///<Balanced-High
  IdsNvGnbPciePsppPolicyBalanced_Low = 3,///<Balanced-Low
  IdsNvGnbPciePsppPolicyPower_Saving = 4,///<Power Saving
  IdsNvGnbPciePsppPolicyAuto = 5,///<Auto
} IdsNvGnbPciePsppPolicy;

///IOMMU
///
typedef enum {
  IdsNvGnbNbIOMMUDisabled = 0,///<Disabled
  IdsNvGnbNbIOMMUEnabled = 1,///<Enabled
} IdsNvGnbNbIOMMU;

#endif // _IDSF15TNNVDEF_H_

