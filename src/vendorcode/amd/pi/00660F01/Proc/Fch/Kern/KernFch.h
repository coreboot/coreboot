/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH registers definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2015, Advanced Micro Devices, Inc.
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

#define KERN_FCH_SATA_DID                 0x7900
#define KERN_FCH_SATA_AHCI_DID            0x7901
#define KERN_FCH_SATA_AMDAHCI_DID         0x7904

#define KERN_FCH_USB_XHCI_DID             0x7914      // Dev 0x10 Func 0
#define KERN_FCH_USB_EHCI_DID             0x7908      // Dev 0x12 Func 0
#define KERN_FCH_SMBUS_DID                0x790B      // Dev 0x14 Func 0
#define KERN_FCH_LPC_DID                  0x790E      // Dev 0x14 Func 3
#define KERN_FCH_SD_DID                   0x7906      // Dev 0x14 Func 7

#define KERN_EHCI1_BUS_DEV_FUN            ((0x12 << 3) + 0)
#define KERN_EHCI1_BUS        0
#define KERN_EHCI1_DEV        18
#define KERN_EHCI1_FUNC       0
#define KERN_EHCI2_BUS_DEV_FUN            ((0x13 << 3) + 0)
#define KERN_EHCI2_BUS        0
#define KERN_EHCI2_DEV        19
#define KERN_EHCI2_FUNC       0
#define KERN_EHCI3_BUS_DEV_FUN            ((0x16 << 3) + 0)
#define KERN_EHCI3_BUS        0
#define KERN_EHCI3_DEV        22
#define KERN_EHCI3_FUNC       0

// Specific FCH register for Kern
#define FCH_EHCI_REG64                    0x64
#define FCH_EHCI_REGA4                    0xA4

// HD Audio/Azalia
#define KERN_EVENT_HD_AUDIO_MSI_ENABLE                 0x02010100ul   // HD Audio/Azalia MSI enable.
#define KERN_EVENT_HD_AUDIO_DISABLE                    0x02010101ul   // HD Audio/Azalia Disable.
#define KERN_EVENT_HD_AUDIO_CONFIG_CODEC               0x02010102ul   // HD Audio/Azalia Configure CODEC.
#define KERN_EVENT_HD_AUDIO_CONFIG_CODEC_BUSY          0x02010103ul   // HD Audio/Azalia Configure CODEC Reset.
#define KERN_ERROR_HD_AUDIO_RESET                      0x02030100ul   // HD Audio/Azalia Reset Failure.
#define KERN_ERROR_HD_AUDIO_CODEC                      0x02030101ul   // HD Audio/Azalia Codec Not Found.
// HW ACPI
#define KERN_EVENT_HWACPI_PROG_ACPI_PMTBL              0x02010200ul   // FCH program ACPI PM Controller Base Address.
#define KERN_EVENT_HWACPI_PROG_ACPI_MMIO_IN_RESET      0x02010201ul   // FCH program ACPIMMIO registers in RESET.
#define KERN_EVENT_HWACPI_PROG_SCI_MAP                 0x02010202ul   // FCH program SCI map by OEM table (ACPIMMIO 0xFED803xx).
#define KERN_EVENT_HWACPI_PROG_GPIO_MAP                0x02010203ul   // FCH program GPIO setting by OEM table (ACPIMMIO 0xFED801xx).
#define KERN_EVENT_HWACPI_PROG_SATA_PHY                0x02010204ul   // FCH program SATA PHY by OEM table.
#define KERN_EVENT_RTC_DAYLIGHT_SAVING_TIME            0x02010205ul   // FCH RTC DayLight Saving Workaround.
#define KERN_EVENT_RTC_CLEAR_BANK_SELECTION            0x02010206ul   // FCH RTC clear BankSelection 0x0A Bit4 prevent error.
#define KERN_EVENT_RTC_WA                              0x02010207ul   // FCH RTC workaround is set.
#define KERN_EVENT_KBD_CONNECTED_THRU_USB              0x02010208ul   // FCH IRQ1/IRQ12 connected to USB controller.
#define KERN_EVENT_LEGACY_FREE_SET                     0x02010209ul   // FCH None Legacy IO setting.
#define KERN_EVENT_ENABLED_ASF_REMOTE_CTRL             0x0201020Aul   // FCH Enable ASF remote control function.
#define KERN_EVENT_PCIEXP_WAKESTATUS_WA                0x0201020Bul   // FCH PciExp Wake Status Workaround.
#define KERN_EVENT_HWACPI_PROG_OEM_MMIOTBL_IN_RESET    0x0201020Cul   // FCH program OEM/Platform BIOS ACPIMMIO registers.
#define KERN_EVENT_HWACPI_PROG_MMIOTBL                 0x02010200ul   // FCH program ACPIMMIO registers. (1st.)
#define KERN_EVENT_HWACPI_NO_SIOKBC_ENABLE             0x02010201ul   // FCH No SIO/KBC function is enabled.
#define KERN_EVENT_HWACPI_NO_SIOKBC_DISABLE            0x02010202ul   // FCH No SIO/KBC function is disabled.
#define KERN_EVENT_HWACPI_PROG_SPECIFIC_MMIOTBL        0x02010203ul   // FCH program Specific ACPIMMIO registers. (2nd.)
#define KERN_EVENT_HWACPI_PROG_OEM_MMIOTBL             0x02010204ul   // FCH program OEM/Platform BIOS ACPIMMIO registers (3rd.)
#define KERN_EVENT_HWACPI_SPREAD_SPECTRUM              0x02010205ul   // FCH Enable Spread Spectrum function.
#define KERN_EVENT_HWACPI_INT_CLK_SET                  0x02010206ul   // FCH Set Intermal Clock Display as 48Mhz.
#define KERN_EVENT_HPET_TIMER_TICK_INTERVAL_WA_SET     0x02010207ul   // FCH HPET timer tick interval workaround is set.
#define KERN_EVENT_C1E_ENABLE                          0x02010208ul   // FCH C1e Enabled.
#define KERN_EVENT_NATIVEPCIE_MODE_SET                 0x02010208ul   // FCH Native PCIe mode is set.
#define KERN_ERROR_HPET_TBL_NOT_FOUND                  0x02030200ul   // FCH HPET Table does not found.
// HWM
#define KERN_EVENT_IMC_DISABLE                         0x02010300ul   // FCH HWM/IMC is disabled.
#define KERN_EVENT_IMC_ENABLE                          0x02010301ul   // FCH HWM/IMC is enabled.
// IMC
#define KERN_EVENT_IMC_SW_TOGGLE_EVENT                 0x02010302ul   // FCH IMC Software Toggle Strapping. (IMC statement change)
#define KERN_EVENT_EC_ENABLE                           0x02020303ul   // FCH EC enabled.
#define KERN_EVENT_EC_KBD_ENABLE                       0x02010304ul   // FCH IMC EC KBD enabled.
#define KERN_EVENT_EC_CHANNEL0_ENABLE                  0x02010305ul   // FCH IMC EC channel0 function enabled.
#define KERN_EVENT_IMC_INTO_SLEEP_MODE                 0x02010306ul   // FCH IMC goes into sleep mode.
#define KERN_EVENT_IMC_CRASH_RESET                     0x02010307ul   // FCH IMC Crash Reset to prevent repeating the reset forever.
#define KERN_EVENT_IMC_SW_DISABLE_IMC                  0x02010308ul   // FCH Software disable IMC thru mailbox command.
#define KERN_EVENT_IMC_DISABLE_SUREBOOT_TIMMER         0x02010309ul   // FCH Disable SureBoot Timmer.
#define KERN_EVENT_IMC_WAKEUP                          0x0201030Aul   // FCH IMC wakeup command.
#define KERN_EVENT_IMC_IDLE                            0x0201030Bul   // FCH IMC idle command.
#define KERN_EVENT_EC_DISABLE                          0x0201030Cul   // FCH EC disable by jumper setting or board config.
#define KERN_ERROR_IMC_FW_VALIDATE_FAILED              0x02030300ul   // FCH IMC firmware validation failure.
// PCIE/AB
#define KERN_EVENT_AB_SLOW_SPEED_ABLINK_CLOCK          0x02010400ul   // FCH Set Low Speed AB link clock.
#define KERN_EVENT_AB_RESET_CPU_ON_SYNC_FLOOD          0x02010401ul   // FCH set AB reset CPU on sync flood enabled.
#define KERN_EVENT_AB_CLOCK_GATING_ENABLE              0x02010402ul   // FCH set AB Clock Gating function enabled.
#define KERN_EVENT_AB_CLOCK_GATING_DISABLE             0x02010403ul   // FCH set AB clock Gating function disabled.
#define KERN_EVENT_AB_DMA_MEMORY_W3264B_ENABLE         0x02010404ul   // FCH set AB DMA MEMORY Write 32/64B enabled.
#define KERN_EVENT_AB_DMA_MEMORY_W3264B_DISABLED       0x02010405ul   // FCH set AB DMA MEMORY Write 32/64B disabled.
#define KERN_EVENT_AB_MEMORY_POWERSAVING_ENABLED       0x02010406ul   // FCH Set AB Memory Power Saving enabled.
#define KERN_EVENT_AB_MEMORY_POWERSAVING_DISABLED      0x02010407ul   // FCH Set AB Memory Power Saving disabled.
#define KERN_EVENT_AB_ALINK_CLK_GATEOFF_ENABLED        0x02010408ul   // FHC set ALink clock Gate-off enabled.
#define KERN_EVENT_AB_ALINK_CLK_GATEOFF_DISABLED       0x02010409ul   // FHC set ALink clock Gate-off disabled.
#define KERN_EVENT_AB_BLINK_CLK_GATEOFF_ENABLED        0x0201040Aul   // FHC set BLink clock Gate-off enabled.
#define KERN_EVENT_AB_BLINK_CLK_GATEOFF_DISABLED       0x0201040Bul   // FHC set BLink clock Gate-off disabled.
// SATA
#define KERN_EVENT_SATA_ENABLE                         0x02010500ul   // FCH set SATA controller enabled.
#define KERN_EVENT_SATA_MAX_GEN2_MODE_ENABLE           0x02010501ul   // FCH set SATA support only Max. GEN2 mode.
#define KERN_EVENT_SATA_SET_CLK_SOURCE_ORG_EXT         0x02010502ul   // FCH set SATA clock source as external 48Mhz.
#define KERN_EVENT_SATA_SET_CLK_SOURCE_ORG_INT         0x02010502ul   // FCH set SATA clock source as internal 48Mhz.
#define KERN_EVENT_SATA_SET_CLK_SOURCE_100_INT         0x02010503ul   // FCH set SATA clock source as internal 100Mhz.
#define KERN_EVENT_SATA_STABLE_MEM_SHUTDOWN_ENABLE     0x02010504ul   // FCH set SATA stable memory sutdown enable * Misc.
#define KERN_EVENT_SATA_PROG_SATA_PORT_PHY             0x02010505ul   // FCH programming SATA port's PHY.
#define KERN_EVENT_SATA_READ_SQUELCH_FROM_EFUSE        0x02010506ul   // FCH programming SATA squelch value from eFuse.
#define KERN_EVENT_SATA_DISABLE                        0x02010507ul   // FCH set SATA controller disabled.
#define KERN_EVENT_SATA_AHCI_MODE                      0x02010508ul   // FCH set SATA as AHCI mode.
#define KERN_EVENT_SATA_IDE_2_AHCI_MODE                0x02010509ul   // FCH set SATA as IDE_2_AHCI mode.
#define KERN_EVENT_SATA_IDE_MODE                       0x0201050Aul   // FCH set SATA as IDE mode.
#define KERN_EVENT_SATA_RAID_MODE                      0x0201050Bul   // FCH set SATA as RAID mode.
#define KERN_EVENT_SATA_MSI_CAP_ENABLE                 0x0201050Cul   // FCH set SATA MSI Capability Enabled.
#define KERN_EVENT_SATA_SUPPORT_8_DEVICE               0x0201050Dul   // FCH set SATA support 8 device mode.
#define KERN_EVENT_SATA_DISABLED_GENERIC_MODE          0x0201050Eul   // FCH set SATA disable generic mode.
#define KERN_EVENT_SATA_PHY_PLL_SHUTDOWN               0x0201050Ful   // FCH set SATA PHY PLL shutdown.
#define KERN_EVENT_SATA_OOB_DETECTION_ENH              0x02010510ul   // FCH set SATA OOB Detection Enhance Mode.
#define KERN_EVENT_SATA_MEM_POWER_SAVING               0x02010511ul   // FCH set SATA memory power saving.
#define KERN_EVENT_SATA_DEV_SLP_PORT0                  0x02010512ul   // FCH set SATA Deep Sleep Mode on Port0.
#define KERN_EVENT_SATA_DEV_SLP_PORT1                  0x02010513ul   // FCH set SATA Deep Sleep Mode on Port1.
#define KERN_EVENT_SATA_AHCI_DIS_PREFETCH              0x02010514ul   // FCH set SATA AHCI disable Prefetch.
#define KERN_EVENT_SATA_PORT_MULT_CAP                  0x02010515ul   // FCH set SATA Port Mult Capability Enabled.
#define KERN_EVENT_SATA_FIS_BASE_SWITCHING             0x02010516ul   // FCH set SATA support FIS-based switching.
#define KERN_EVENT_SATA_AGGR_LINK_PM_CAP               0x02010517ul   // FCH set SATA aggressive link power management.
#define KERN_EVENT_SATA_PSC_CAP                        0x02010518ul   // FCH set SATA support partial state.
#define KERN_EVENT_SATA_SSC_CAP                        0x02010519ul   // FCH set SATA support Slumber mode.
#define KERN_EVENT_SATA_CCC_CAP                        0x0201051Aul   // FCH set SATA support command completion coalescing.
#define KERN_EVENT_SATA_AHCI_ENCLOSURE_MANAGEMENT      0x0201051Bul   // FCH set SATA support Enclosure Management.
#define KERN_EVENT_SATA_ESP_PORT_ENABLE                0x0201051Cul   // FCH set SATA ESP ports (one of ESP ports are set).
#define KERN_EVENT_SATA_BIOS_OS_HANDOFF                0x0201051Dul   // FCH set SATA HBA supports the BIOS/OS handoff mechanism.
#define KERN_EVENT_SATA_DRIVE_DETECTION                0x0201051Eul   // FCH SATA is excuting SATA drive detection.
#define KERN_EVENT_SATA_CLK_AUTO_OFF                   0x0201051Ful   // FCH set SATA Auto Clock off function.
#define KERN_EVENT_SATA_PORT_GEN_MODE                  0x02010520ul   // FCH set SATA port GEN mode.
#define KERN_EVENT_SATA_HOT_REMOVAL_ENH                0x02010521ul   // FCH set SATA hot removal enhance mode.
// SD
#define KERN_EVENT_SD_ENABLE                           0x02010600ul   // FCH set SD controller to enable.
#define KERN_EVENT_SD_AS_DMA_MODE                      0x02010601ul   // FCH set SD as DMA mode.
#define KERN_EVENT_SD_AS_PIO_MODE                      0x02010602ul   // FCH set SD as PIO mode.
#define KERN_EVENT_SD_AS_2_0_MODE                      0x02010603ul   // FCH set SD as 2.0 mode.
#define KERN_EVENT_SD_AS_3_0_MODE                      0x02010604ul   // FCH set SD as 3.0 mode.
#define KERN_EVENT_SD_CLOCK_MULTIPLIER                 0x02010605ul   // FCH set SD clock multiplier.
#define KERN_EVENT_SD_DISABLE                          0x02010606ul   // FCH set SD controller to disable.
// LPC/SPI
#define KERN_EVENT_LPC_CLK0_DISABLE                    0x02010700ul   // FCH set LPC0 clock disabled.
#define KERN_EVENT_LPC_CLK1_DISABLE                    0x02010701ul   // FCH set LPC1 clock disabled.
#define KERN_EVENT_LPC_LEGACY_FREE_MODE                0x02010702ul   // FCH set LPC as legacy free mode.
#define KERN_EVENT_SPI_QUAL_MODE                       0x02010703ul   // FCH SPI Qual Mode is enabled (by user selection).
#define KERN_EVENT_SPI_SPEED                           0x02010704ul   // FCH SPI speed is set by user selection.
#define KERN_EVENT_SPI_FAST_SPEED                      0x02010705ul   // FCH SPI fast speed is set.
#define KERN_EVENT_SPI_QUALIFY_QUAL_MODE               0x02010706ul   // FCH SPI Qual Mode is validated.
// USB
#define KERN_ERROR_USB_ROMSIG_NOT_FOUND                0x02030800ul   // FCH ROMSIG not found.
#define KERN_ERROR_USB_XHCI_FW_FOUND                   0x02030801ul   // FCH XHCI firmware does not exist.
#define KERN_EVENT_USB_EHCI1_ENABLE                    0x02010800ul   // FCH set EHCI1 enable.
#define KERN_EVENT_USB_EHCI2_ENABLE                    0x02010801ul   // FCH set EHCI2 enable.
#define KERN_EVENT_USB_EHCI3_ENABLE                    0x02010802ul   // FCH set EHCI3 enable.
#define KERN_EVENT_USB_XHCI_DISABLE                    0x02010803ul   // FCH set XHCI disable.
#define KERN_EVENT_USB_PHY_POWER_DOWN                  0x02010804ul   // FCH set USB PHY powerdown enable.
#define KERN_EVENT_USB_PHY_CALIBRATED                  0x02010805ul   // FCH set USB PHY calibrated.
#define KERN_EVENT_USB_PORT_PHY_SETTING                0x02010806ul   // FCH set USB Port PHY setting.
#define KERN_EVENT_USB_PORT_PHY_CLK_GATING             0x02010807ul   // FCH set USB Port PHY Clock Gating.
#define KERN_EVENT_USB_XHCI_USED_PREDEFINE_ADDRESS     0x02010808ul   // FCH XHCI ROM location is used user-define address.
#define KERN_EVENT_USB_XHCI_BOOTRAM_PRELOAD            0x02010809ul   // FCH XHCI preload its BOOTRAM.
#define KERN_EVENT_USB_XHCI_INSTRUCTRAM_PRELOAD        0x0201080Aul   // FCH XHCI preload its InstructionRAM.
#define KERN_EVENT_USB_XHCI_ROM_PREINIT_COMPLETED      0x0201080Bul   // FCH XHCI preinit completed.
#define KERN_EVENT_USB_XHCI0_ENABLE                    0x0201080Cul   // FCH set XHCI0 enable.
#define KERN_EVENT_USB_XHCI1_ENABLE                    0x0201080Dul   // FCH set XHCI1 enable.
