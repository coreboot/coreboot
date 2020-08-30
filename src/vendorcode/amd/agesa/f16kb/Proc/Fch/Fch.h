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
 * @e \$Revision: 87830 $   @e \$Date: 2013-02-11 12:48:20 -0600 (Mon, 11 Feb 2013) $
 *
 */
/*
*****************************************************************************
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
****************************************************************************
*/
#define FCH_REVISION       "0.0.5.0"
#define FCH_ID             "FCH_A05"
#define FCH_VERSION        0x0000

/**
 * @page fchinitguide FCH implement phase in AGESA
 *
 * FCH provides below access to supported FCH service functions
 * and data.
 *   - @subpage fchreset "FCH_INIT_RESET"
 *   - @subpage fchenv "FCH_INIT_ENV"
 *   - @subpage fchmid "FCH_INIT_MID"
 *   - @subpage fchlate "FCH_INIT_LATE"
 *   - @subpage fchs3early "FCH_INIT_S3_EARLY_RESTORE"
 *   - @subpage fchs3late "FCH_INIT_S3_LATE_RESTORE"
 *   - @subpage fchsmm "FCH_SMM_SERVICE"
 *   - @subpage fchsmmacpion "FCH_SMM_ACPION"
 */

/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page fchreset FCH_INIT_RESET
 *  @section FCH_INIT_RESET Interface Call
 *  @par
 *     Initialize structure referenced by FCH_RESET_DATA_BLOCK to default recommended value.
 *  @subsection FCH_INIT_RESET_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS FchInitReset (IN AMD_RESET_PARAMS *ResetParams);
 *  @subsection FCH_INIT_RESET_CallOut Prepare for Callout
 *  @par
 *     Not Applicable (Not necessary for the current implementation)
 *  @subsection FCH_INIT_RESET_Config Prepare for Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgSmbus0BaseAddress  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgSmbus1BaseAddress  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgSioPmeBaseAddress  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgWatchDogTimerBase  </TD><TD class="indexvalue"><B>Required </B></TD></TR>

 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgSpiRomBaseAddress  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgAcpiPm1EvtBlkAddr  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgAcpiPm1CntBlkAddr  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgAcpiPmTmrBlkAddr   </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgCpuControlBlkAddr  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgAcpiGpe0BlkAddr    </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgSmiCmdPortAddr     </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_RESET_INTERFACE::SataEnable      </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_RESET_INTERFACE::IdeEnable       </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *  </TABLE>
 *
 */

/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page fchenv FCH_INIT_ENV
 *  @section FCH_INIT_ENV Interface Call
 *  @par
 *     Initialize structure referenced by FCH_DATA_BLOCK to default recommended value.
 *  @subsection FCH_INIT_ENV_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS FchInitEnv (IN AMD_ENV_PARAMS *EnvParams);
 *  @subsection FCH_INIT_ENV_CallOut Prepare for Callout
 *  @par
 *     Not Applicable (Not necessary for the current implementation)
 *  @subsection FCH_INIT_ENV_Config Prepare for Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SdConfig              </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::AzaliaController      </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::IrConfig              </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataClass             </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataEnable            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataIdeMode           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::Ohci1Enable           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::Ohci2Enable           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::Ohci3Enable           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::Ohci4Enable           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::XhciSwitch            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *  </TABLE>
 *
 */

/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page fchmid FCH_INIT_MID
 *  @section FCH_INIT_MID Interface Call
 *  @par
 *     Initialize structure referenced by FCH_DATA_BLOCK to default recommended value.
 *  @subsection FCH_INIT_MID_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS FchInitMid (IN AMD_MID_PARAMS *MidParams);
 *  @subsection FCH_INIT_MID_CallOut Prepare for Callout
 *  @par
 *     Not Applicable (Not necessary for the current implementation)
 *  @subsection FCH_INIT_MID_Config Prepare for Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::AzaliaController      </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataClass             </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataEnable            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::IdeEnable             </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::XhciSwitch            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *  </TABLE>
 *
 */

/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page fchlate FCH_INIT_LATE
 *  @section FCH_INIT_LATE Interface Call
 *  @par
 *     Initialize structure referenced by FCH_DATA_BLOCK to default recommended value.
 *  @subsection FCH_INIT_LATE_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS FchInitLate (IN AMD_S3SAVE_PARAMS *LateParams);
 *  @subsection FCH_INIT_LATE_CallOut Prepare for Callout
 *  @par
 *     Not Applicable (Not necessary for the current implementation)
 *  @subsection FCH_INIT_LATE_Config Prepare for Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataClass             </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataEnable            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::XhciSwitch            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgSpiRomBaseAddress  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *  </TABLE>
 *
 */

/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page fchs3early FCH_INIT_S3_EARLY_RESTORE
 *  @section FCH_INIT_S3_EARLY_RESTORE Interface Call
 *  @par
 *     Initialize structure referenced by FCH_DATA_BLOCK to default recommended value.
 *  @subsection FCH_INIT_S3_EARLY_RESTORE_CallIn Call Prototype
 *  @par
 *     VOID FchInitS3EarlyRestore (IN FCH_DATA_BLOCK *FchDataPtr);
 *  @subsection FCH_INIT_S3_EARLY_RESTORE_CallOut Prepare for Callout
 *  @par
 *     Not Applicable (Not necessary for the current implementation)
 *  @subsection FCH_INIT_S3_EARLY_RESTORE_Config Prepare for Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SdConfig              </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::AzaliaController      </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::IrConfig              </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataClass             </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataEnable            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataIdeMode           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::Ohci1Enable           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::Ohci2Enable           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::Ohci3Enable           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::Ohci4Enable           </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::XhciSwitch            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *  </TABLE>
 *
 */

/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page fchs3late FCH_INIT_S3_LATE_RESTORE
 *  @section FCH_INIT_S3_LATE_RESTORE Interface Call
 *  @par
 *     Initialize structure referenced by FCH_DATA_BLOCK to default recommended value.
 *  @subsection FCH_INIT_S3_LATE_RESTORE_CallIn Call Prototype
 *  @par
 *     VOID FchInitS3LateRestore (IN FCH_DATA_BLOCK *FchDataPtr);
 *  @subsection FCH_INIT_S3_LATE_RESTORE_CallOut Prepare for Callout
 *  @par
 *     Not Applicable (Not necessary for the current implementation)
 *  @subsection FCH_INIT_S3_LATE_RESTORE_Config Prepare for Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::AzaliaController      </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataClass             </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataEnable            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::IdeEnable             </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::XhciSwitch            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgSpiRomBaseAddress  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *  </TABLE>
 *
 */

/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page fchsmm FCH_SMM_SERVICE
 *  @section FCH_SMM_SERVICE Interface Call
 *  Initialize structure referenced by FCHCFG to default recommended value.
 *  @subsection FCH_SMM_SERVICE_CallIn Call Prototype
 *  @par
 *     FchSmmService ((FCHCFG*)pConfig)     (Followed PH Interface)
 *  @subsection FCH_SMM_SERVICE_CallID Service ID
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> FCH_SMM_SERVICE  --> 0x00010060 </TD></TR>
 *  </TABLE>
 *  @subsection FCH_SMM_SERVICE_CallOut Prepare for Callout
 *  @par
 *    Not Applicable (Not necessary for the current implementation)
 *  @subsection FCH_SMM_SERVICE_Config Prepare for Configuration Data.
 *  @par
 *     Not necessary on current implementation
 *
 */
#define FCH_SMM_SERVICE              0x00010060ul
/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page fchsmmacpion FCH_SMM_ACPION
 *  @section FCH_SMM_ACPION Interface Call
 *  Initialize structure referenced by FCHCFG to default recommended value.
 *  @subsection FCH_SMM_ACPION_CallIn Call Prototype
 *  @par
 *     FchSmmAcpiOn ((FCHCFG*)pConfig)     (Followed PH Interface)
 *  @subsection FCH_SMM_ACPION_CallID Service ID
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> FCH_SMM_ACPION  --> 0x00010061 </TD></TR>
 *  </TABLE>
 *  @subsection FCH_SMM_ACPION_CallOut Prepare for Callout
 *  @par
 *    Not Applicable (Not necessary for the current implementation)
 *  @subsection FCH_SMM_ACPION_Config Prepare for Configuration Data.
 *  @par
 *     Not necessary on current implementation
 *
 */
#define FCH_SMM_ACPION               0x00010061ul

#ifndef OEM_CALLBACK_BASE
  #define OEM_CALLBACK_BASE         0x00010100ul
#endif

//0x00 - 0x0F callback functions are reserved for bootblock
#define SATA_PHY_PROGRAMMING        OEM_CALLBACK_BASE + 0x10
#define PULL_UP_PULL_DOWN_SETTINGS  OEM_CALLBACK_BASE + 0x20
/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page CB_SBGPP_RESET_ASSERT_Page CB_SBGPP_RESET_ASSERT
 *  @section CB_SBGPP_RESET_ASSERT Interface Call
 *  Initialize structure referenced by FCHCFG to default recommended value.
 *  @subsection CB_SBGPP_RESET_ASSERT_CallID Service ID
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> CB_SBGPP_RESET_ASSERT  --> 0x00010130 </TD></TR>
 *  </TABLE>
 *  @subsection CB_SBGPP_RESET_ASSERT_Config Prepare for Configuration Data.
 *  @par
 *     Not necessary on current implementation
 *
 */
#define CB_SBGPP_RESET_ASSERT       OEM_CALLBACK_BASE + 0x30
/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page CB_SBGPP_RESET_DEASSERT_Page CB_SBGPP_RESET_DEASSERT
 *  @section CB_SBGPP_RESET_DEASSERT  Interface Call
 *  Initialize structure referenced by FCHCFG to default recommended value.
 *  @subsection CB_SBGPP_RESET_DEASSERT _CallID Service ID
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> CB_SBGPP_RESET_DEASSERT   --> 0x00010131 </TD></TR>
 *  </TABLE>
 *  @subsection CB_SBGPP_RESET_DEASSERT _Config Prepare for Configuration Data.
 *  @par
 *     Not necessary on current implementation
 *
 */
#define CB_SBGPP_RESET_DEASSERT     OEM_CALLBACK_BASE + 0x31

#define CFG_ADDR_PORT               0xCF8
#define CFG_DATA_PORT               0xCFC

#define ALINK_ACCESS_INDEX          0x0CD8
#define ALINK_ACCESS_DATA           ALINK_ACCESS_INDEX + 4

/*------------------------------------------------------------------
; I/O Base Address - Should be set by host BIOS
;------------------------------------------------------------------ */
#define DELAY_PORT                  0x0E0

#define FCH_8259_CONTROL_REG_MASTER 0x20
#define FCH_8259_MASK_REG_MASTER    0x21

/*------------------------------------------------------------------
; DEBUG_PORT = 8-bit I/O Port Address for POST Code Display
;------------------------------------------------------------------ */
// ASIC VendorID and DeviceIDs
#define AMD_FCH_VID                  0x1022
#define FCH_DEVICE_ID                0x780B
#define FCH_SATA_VID                 AMD_FCH_VID      // Dev 17 Func 0
#define FCH_SATA_DID                 0x7800
#define FCH_SATA_AHCI_DID            0x7801
#define FCH_SATA_RAID_DID            0x7802
#define FCH_SATA_RAID5_DID           0x7803
#define FCH_SATA_AMDAHCI_DID         0x7804
#define FCH_SATA_RAID_DOTHILL_DID    0x7805
#define FCH_SATA_RAID5_DOTHILL_DID   0x780A
#define FCH_USB_OHCI_VID             AMD_FCH_VID      // Dev 18 Func 0,
#define FCH_USB_OHCI_DID             0x7807
#define FCH_USB_EHCI_VID             AMD_FCH_VID      // Dev 18 Func 2,
#define FCH_USB_EHCI_DID             0x7808
#define FCH_USB_XHCI_VID             AMD_FCH_VID      // Dev 10 Func 0
#define FCH_USB_XHCI_DID             0x7812
#define FCH_SMBUS_VID                AMD_FCH_VID      // Dev 20 Func 0
#define FCH_SMBUS_DID                0x780B
#define FCH_IDE_VID                  AMD_FCH_VID      // Dev 20 Func 1
#define FCH_IDE_DID                  0x780C
#define FCH_AZALIA_VID               AMD_FCH_VID      // Dev 20 Func 2
#define FCH_AZALIA_DID               0x780D
#define FCH_LPC_VID                  AMD_FCH_VID      // Dev 20 Func 3
#define FCH_LPC_DID                  0x780E
#define FCH_PCIB_VID                 AMD_FCH_VID      // Dev 20 Func 4
#define FCH_PCIB_DID                 0x780F
#define FCH_USB_OHCIF_VID            AMD_FCH_VID      // dev 20 Func 5
#define FCH_USB_OHCIF_DID            0x7809
#define FCH_NIC_VID                  0x14E4          // Dev 20 Func 6
#define FCH_NIC_DID                  0x1699
#define FCH_SD_VID                   AMD_FCH_VID      // Dev 20 Func 7
#define FCH_SD_DID                   0x7806


#define FCH_YANGTZE                       0x39
//Misc
#define R_FCH_ACPI_PM1_STATUS              0x00
#define R_FCH_ACPI_PM1_ENABLE              0x02
#define R_FCH_ACPI_PM_CONTROL              0x04
#define R_FCH_ACPI_EVENT_STATUS            0x20
#define R_FCH_ACPI_EVENT_ENABLE            0x24
#define R_FCH_PM_ACPI_PMA_CNT_BLK_LO       0x2C

// ACPI Sleep Type
#define ACPI_SLPTYP_S0                     0
#define ACPI_SLPTYP_S1                     1
#define ACPI_SLPTYP_S3                     3
#define ACPI_SLPTYP_S4                     4
#define ACPI_SLPTYP_S5                     5

//#define SATA_BUS_DEV_FUN_FPGA            0x228
#define SATA_BUS_DEV_FUN            ((0x11 << 3) + 0)
#define FCH_SATA1_BUS        0
#define FCH_SATA1_DEV        17
#define FCH_SATA1_FUNC       0

#define FC_BUS_DEV_FUN              ((0x11 << 3) + 1)
#define FCH_XHCI_BUS        0
#define FCH_XHCI_DEV        16
#define FCH_XHCI_FUNC       0
#define USB_XHCI_BUS_DEV_FUN        ((FCH_XHCI_DEV << 3) + FCH_XHCI_FUNC)
#define FCH_XHCI1_BUS        0
#define FCH_XHCI1_DEV        16
#define FCH_XHCI1_FUNC       1
#define USB_XHCI1_BUS_DEV_FUN       ((FCH_XHCI1_DEV << 3) + FCH_XHCI1_FUNC)
#define USB1_OHCI_BUS_DEV_FUN       ((0x12 << 3) + 0)    // PORT 0-4
#define FCH_OHCI1_BUS        0
#define FCH_OHCI1_DEV        18
#define FCH_OHCI1_FUNC       0
#define USB2_OHCI_BUS_DEV_FUN       ((0x13 << 3) + 0)    // PORT 5-9
#define FCH_OHCI2_BUS        0
#define FCH_OHCI2_DEV        19
#define FCH_OHCI2_FUNC       0
#define USB3_OHCI_BUS_DEV_FUN       ((0x16 << 3) + 0)    // PORT 10-13
#define FCH_OHCI3_BUS        0
#define FCH_OHCI3_DEV        22
#define FCH_OHCI3_FUNC       0
#define USB1_EHCI_BUS_DEV_FUN       ((0x12 << 3) + 2)    // PORT 0-4
#define FCH_EHCI1_BUS        0
#define FCH_EHCI1_DEV        18
#define FCH_EHCI1_FUNC       2
#define USB2_EHCI_BUS_DEV_FUN       ((0x13 << 3) + 2)    // PORT 5-9
#define FCH_EHCI2_BUS        0
#define FCH_EHCI2_DEV        19
#define FCH_EHCI2_FUNC       2
#define USB3_EHCI_BUS_DEV_FUN       ((0x16 << 3) + 2)  // PORT 10-13
#define FCH_EHCI3_BUS        0
#define FCH_EHCI3_DEV        22
#define FCH_EHCI3_FUNC       2
#define SMBUS_BUS_DEV_FUN           ((0x14 << 3) + 0)
#define FCH_ISA_BUS          0
#define FCH_ISA_DEV          20
#define FCH_ISA_FUNC         0
#define IDE_BUS_DEV_FUN             ((0x14 << 3) + 1)
#define FCH_IDE_BUS          0
#define FCH_IDE_DEV          20
#define FCH_IDE_FUNC         1
#define AZALIA_BUS_DEV_FUN          ((0x14 << 3) + 2)
#define FCH_AZALIA_BUS       0
#define FCH_AZALIA_DEV       20
#define FCH_AZALIA_FUNC      2
#define LPC_BUS_DEV_FUN             ((0x14 << 3) + 3)
#define FCH_LPC_BUS          0
#define FCH_LPC_DEV          20
#define FCH_LPC_FUNC         3
#define FCH_PCI_BUS          0
#define FCH_PCI_DEV          20
#define FCH_PCI_FUNC         4
#define USB4_OHCI_BUS_DEV_FUN       ((0x14 << 3) + 5)    // PORT FL0 - FL1
#define FCH_OHCI4_BUS        0
#define FCH_OHCI4_DEV        20
#define FCH_OHCI4_FUNC       5
//Gigabyte Ethernet Controller
#define GEC_BUS_DEV_FUN             ((0x14 << 3) + 6)
#define FCH_GBEC_BUS         0
#define FCH_GBEC_DEV         20
#define FCH_GBEC_FUNC        6

#define SD_BUS_DEV_FUN            ((0x14 << 3) + 7)      // SD Controller
#define SD_PCI_BUS          0
#define SD_PCI_DEV          20
#define SD_PCI_FUNC         7


#define ACPI_MMIO_BASE  0xFED80000ul
#define FCH_CFG_BASE     0x000   // DWORD
#define GPIO_BASE       0x100   // BYTE
#define SMI_BASE        0x200   // DWORD
#define PMIO_BASE       0x300   // DWORD
#define PMIO2_BASE      0x400   // BYTE
#define BIOS_RAM_BASE   0x500   // BYTE
#define CMOS_RAM_BASE   0x600   // BYTE
#define CMOS_BASE       0x700   // BYTE
#define ASF_BASE        0x900   // DWORD
#define SMBUS_BASE      0xA00   // DWORD
#define WATCHDOG_BASE   0xB00   //
#define HPET_BASE       0xC00   // DWORD
#define IOMUX_BASE      0xD00   // BYTE
#define MISC_BASE       0xE00
#define SERIAL_DEBUG_BASE  0x1000
#define GFX_DAC_BASE       0x1400
//#define CEC_BASE           0x1800
#define XHCI_BASE          0x1C00


// Chip type definition
#define CHIPTYPE_HUDSON2         (1 << 0)

//
// ROM SIG type definition
//
#define NUM_OF_ROMSIG_FILED      0x04
#define XHCI_FILED_NUM           0x03
#define ROMSIG_CFG_MASK          0x07
#define XHCI_BOOT_RAM_OFFSET     0x8000
#define INSTRUCTION_RAM_SIG      0x55AA
#define ROMSIG_SIG               0x55AA55AAul

// RegSpace field (AB_INDEX[31:29]
#define ABCFG          6               // ABCFG

#define GPP_DEV_NUM    21              //
#define MAX_GPP_PORTS  4

//
// ABCFG Registers
//
#define FCH_ABCFG_REG54     0x54  // MISCCTL_54
#define FCH_ABCFG_REG58     0x58  // BL RAB CONTROL


#define FCH_ABCFG_REG80     0x80  // BL DMA PREFETCH CONTROL
#define FCH_ABCFG_REG90     0x90  // BIF CONTROL 0
#define FCH_ABCFG_REG94     0x94  // MSI CONTROL
#define FCH_ABCFG_REG10054  0x10054ul  // AL_ARB_CTL
#define FCH_ABCFG_REG10090  0x10090ul  // BIF CONTROL 0



//
// AXINDC Registers
//

#define FCH_AB_REG04         0x04
#define FCH_AB_REG40         0x40

//Sata Port Configuration
#define SIX_PORTS       0
#define FOUR_PORTS      1




// USB ports
#define NUM_USB1_PORTS           5
#define NUM_USB2_PORTS           5
#define NUM_USB3_PORTS           4
#define NUM_USB4_PORTS           2
#define NUM_XHC0_PORTS           2
#define NUM_XHC1_PORTS           2


//
//  USB OHCI Device 0x7807
//  Device 18 (0x12)/Device 19 (0x13)/Device 22 (0x16) Func 0
//  Device 20 (0x14) Func 5 (FL) 0x7809
//
#define FCH_OHCI_REG00           0x00        // Device/Vendor ID - R (0x43971002ul)
#define FCH_OHCI_REG04           0x04        // Command - RW
#define FCH_OHCI_REG06           0x06        // Status - R
#define FCH_OHCI_REG08           0x08        // Revision ID/Class Code - R
#define FCH_OHCI_REG0C           0x0C        // Miscellaneous - RW
#define FCH_OHCI_REG10           0x10        // Bar_OCI - RW
#define FCH_OHCI_REG2C           0x2C        // Subsystem Vendor ID/ Subsystem ID - RW
#define FCH_OHCI_REG34           0x34        // Capability Pointer - R
#define FCH_OHCI_REG3C           0x3C        // Interrupt Line - RW
#define FCH_OHCI_REG3D           0x3D        // Interrupt Line - RW
#define FCH_OHCI_REG40           0x40        // Config Timers - RW
#define FCH_OHCI_REG42           0x42        // Port Disable Control - RW (800)
#define FCH_OHCI_REG48           0x48        // Port Force Reset - RW (800)
#define FCH_OHCI_REG58           0x58        // Over Current Control - RW
#define FCH_OHCI_REG68           0x68        // Over Current PME Enable - RW
#define FCH_OHCI_REG74           0x74        // Target Timeout Control - RW
#define FCH_OHCI_REG80           0x80        //
#define FCH_OHCI_REGD0           0x0D0       // MSI Control - RW
#define FCH_OHCI_REGD4           0x0D4       // MSI Address - RW
#define FCH_OHCI_REGD8           0x0D8       // MSI Data - RW
#define FCH_OHCI_REGE4           0x0E4       // HT MSI Support
#define FCH_OHCI_REGF0           0x0F0       // Function Level Reset Capability
#define FCH_OHCI_REGF4           0x0F4       // Function Level Reset Control

#define FCH_OHCI_BAR_REG00       0x00        // cRevision - R
#define FCH_OHCI_BAR_REG04       0x04        // cControl
#define FCH_OHCI_BAR_REG08       0x08        // cCommandStatus
#define FCH_OHCI_BAR_REG0C       0x0C        // cInterruptStatus  RW
#define FCH_OHCI_BAR_REG10       0x10        // cInterruptEnable
#define FCH_OHCI_BAR_REG14       0x14        // cInterruptDisable
#define FCH_OHCI_BAR_REG18       0x18        // HcCCA
#define FCH_OHCI_BAR_REG1C       0x1C        // cPeriodCurrentED
#define FCH_OHCI_BAR_REG20       0x20        // HcControleadED
#define FCH_OHCI_BAR_REG24       0x24        // cControlCurrentED  RW
#define FCH_OHCI_BAR_REG28       0x28        // HcBulkeadED
#define FCH_OHCI_BAR_REG2C       0x2C        // cBulkCurrentED- RW
#define FCH_OHCI_BAR_REG30       0x30        // HcDoneead
#define FCH_OHCI_BAR_REG34       0x34        // cFmInterval
#define FCH_OHCI_BAR_REG38       0x38        // cFmRemaining
#define FCH_OHCI_BAR_REG3C       0x3C        // cFmNumber
#define FCH_OHCI_BAR_REG40       0x40        // cPeriodicStart
#define FCH_OHCI_BAR_REG44       0x44        // HcLSThresold
#define FCH_OHCI_BAR_REG48       0x48        // HcRDescriptorA
#define FCH_OHCI_BAR_REG4C       0x4C        // HcRDescriptorB
#define FCH_OHCI_BAR_REG50       0x50        // HcRStatus
#define FCH_OHCI_BAR_REG54       0x54        // HcRhPortStatus (800)
#define FCH_OHCI_BAR_REG58       0x58        // HcRhPortStatus NPD (800)
#define FCH_OHCI_BAR_REGF0       0xF0        // OHCI Loop Back feature Support (800)

//
//  USB EHCI Device 0x7808
//  Device 18 (0x12)/Device 19 (0x13)/Device 22 (0x16) Func 2
//
#define FCH_EHCI_REG00           0x00        // DEVICE/VENDOR ID - R
#define FCH_EHCI_REG04           0x04        // Command - RW
#define FCH_EHCI_REG06           0x06        // Status - R
#define FCH_EHCI_REG08           0x08        // Revision ID/Class Code - R
#define FCH_EHCI_REG0C           0x0C        // Miscellaneous - RW
#define FCH_EHCI_REG10           0x10        // BAR - RW
#define FCH_EHCI_REG2C           0x2C        // Subsystem ID/Subsystem Vendor ID - RW
#define FCH_EHCI_REG34           0x34        // Capability Pointer - R
#define FCH_EHCI_REG3C           0x3C        // Interrupt Line - RW
#define FCH_EHCI_REG3D           0x3D        // Interrupt Line - RW
#define FCH_EHCI_REG54           0x54        // EHCI Misc Control - RW
#define FCH_EHCI_REG60           0x60        // SBRN - R
#define FCH_EHCI_REG61           0x61        // FLADJ - RW
#define FCH_EHCI_REGC0           0x0C0       // PME control - RW (800)
#define FCH_EHCI_REGC4           0x0C4       // PME Data /Status - RW (800)
#define FCH_EHCI_REGD0           0x0D0       // MSI Control - RW
#define FCH_EHCI_REGD4           0x0D4       // MSI Address - RW
#define FCH_EHCI_REGD8           0x0D8       // MSI Data - RW
#define FCH_EHCI_REGF0           0x0F0       // Function Level Reset Capability - R (800)
#define FCH_EHCI_REGF4           0x0F4       // Function Level Reset Capability - R (800)

#define FCH_EHCI_BAR_REG00       0x00        // CAPLENGT - R
#define FCH_EHCI_BAR_REG02       0x002
#define FCH_EHCI_BAR_REG04       0x004
#define FCH_EHCI_BAR_REG08       0x008

#define FCH_EHCI_BAR_REG20       0x020
#define FCH_EHCI_BAR_REG24       0x024
#define FCH_EHCI_BAR_REG28       0x028
#define FCH_EHCI_BAR_REG2C       0x02C
#define FCH_EHCI_BAR_REG30       0x030
#define FCH_EHCI_BAR_REG34       0x034
#define FCH_EHCI_BAR_REG38       0x038
#define FCH_EHCI_BAR_REG60       0x060
#define FCH_EHCI_BAR_REG64       0x064
#define FCH_EHCI_BAR_REGA4       0x0A4
#define FCH_EHCI_BAR_REGA8       0x0A8
#define FCH_EHCI_BAR_REGAC       0x0AC
#define FCH_EHCI_BAR_REGB4       0x0B4
#define FCH_EHCI_BAR_REGB8       0x0B8
#define FCH_EHCI_BAR_REGBC       0x0BC
#define FCH_EHCI_BAR_REGC0       0x0C0
#define FCH_EHCI_BAR_REGC4       0x0C4
#define FCH_EHCI_BAR_REGC8       0x0C8
#define FCH_EHCI_BAR_REGD0       0x0D0
#define FCH_EHCI_BAR_REGD4       0x0D4

//
//  USB XHCI Device 0x7812/0x7814
//  Device 16 (0x10) Func 0
//
#define FCH_XHCI_REG00           0x00        // DEVICE/VENDOR ID - R
#define FCH_XHCI_REG04           0x04        // Command - RW
#define FCH_XHCI_REG10           0x10        // Bar0
#define FCH_XHCI_REG2C           0x2C        // Sub System ID
#define FCH_XHCI_REG40           0x40        // Index0
#define FCH_XHCI_REG44           0x44        // Data0
#define FCH_XHCI_REG48           0x48        // Index1
#define FCH_XHCI_REG4C           0x4C        // Data0
#define FCH_XHCI_REG54           0x54        // PME Control/Status

//
//  FCH CFG device  0x780B
//  Device 20 (0x14) Func 0
//
#define FCH_CFG_REG00            0x000       // VendorID - R
#define FCH_CFG_REG02            0x002       // DeviceID - R
#define FCH_CFG_REG04            0x004       // Command- RW
#define FCH_CFG_REG05            0x005       // Command- RW
#define FCH_CFG_REG06            0x006       // STATUS- RW
#define FCH_CFG_REG08            0x008       // Revision ID/Class Code- R
#define FCH_CFG_REG0A            0x00A       //
#define FCH_CFG_REG0B            0x00B       //
#define FCH_CFG_REG0C            0x00C       // Cache Line Size- R
#define FCH_CFG_REG0D            0x00D       // Latency Timer- R
#define FCH_CFG_REG0E            0x00E       // Header Type- R
#define FCH_CFG_REG0F            0x00F       // BIST- R
#define FCH_CFG_REG10            0x010       // Base Address 0- R
#define FCH_CFG_REG11            0x011       //;
#define FCH_CFG_REG12            0x012       //;
#define FCH_CFG_REG13            0x013       //;
#define FCH_CFG_REG14            0x014       // Base Address 1- R
#define FCH_CFG_REG18            0x018       // Base Address 2- R
#define FCH_CFG_REG1C            0x01C       // Base Address 3- R
#define FCH_CFG_REG20            0x020       // Base Address 4- R
#define FCH_CFG_REG24            0x024       // Base Address 5- R
#define FCH_CFG_REG28            0x028       // Cardbus CIS Pointer- R
#define FCH_CFG_REG2C            0x02C       // Subsystem Vendor ID- W
#define FCH_CFG_REG2E            0x02E       // Subsystem ID- W
#define FCH_CFG_REG30            0x030       // Expansion ROM Base Address - R
#define FCH_CFG_REG34            0x034       // Capability Pointer - R  (800) default changed as 0x00
#define FCH_CFG_REG3C            0x03C       // Interrupt Line - R
#define FCH_CFG_REG3D            0x03D       // Interrupt Pin - R
#define FCH_CFG_REG3E            0x03E       // Min_Gnt - R
#define FCH_CFG_REG3F            0x03F       // Max_Lat - R
#define FCH_CFG_REG90            0x090       // Smbus Base Address - R
#define FCH_CFG_REG9C            0x09C       // SBResourceMMIO_BASE

//
//  FCH AZALIA device  0x780D
//  Device 20 (0x14) Func 2
//

#define FCH_AZ_REG00             0x00        // Vendor ID - R
#define FCH_AZ_REG02             0x02        // Device ID - R/W
#define FCH_AZ_REG04             0x04        // PCI Command
#define FCH_AZ_REG06             0x06        // PCI Status - R/W
#define FCH_AZ_REG08             0x08        // Revision ID
#define FCH_AZ_REG09             0x09        // Programming Interface
#define FCH_AZ_REG0A             0x0A        // Sub Class Code
#define FCH_AZ_REG0B             0x0B        // Base Class Code
#define FCH_AZ_REG0C             0x0C        // Cache Line Size - R/W
#define FCH_AZ_REG0D             0x0D        // Latency Timer
#define FCH_AZ_REG0E             0x0E        // Header Type
#define FCH_AZ_REG0F             0x0F        // BIST
#define FCH_AZ_REG10             0x10        // Lower Base Address Register
#define FCH_AZ_REG14             0x14        // Upper Base Address Register
#define FCH_AZ_REG2C             0x2C        // Subsystem Vendor ID
#define FCH_AZ_REG34             0x34        // Capabilities Pointer
#define FCH_AZ_REG3C             0x3C        // Interrupt Line
#define FCH_AZ_REG3D             0x3D        // Interrupt Pin
#define FCH_AZ_REG3E             0x3E        // Minimum Grant
#define FCH_AZ_REG3F             0x3F        // Maximum Latency
#define FCH_AZ_REG40             0x40        // Misc Control 1
#define FCH_AZ_REG44             0x44        // Interrupt Pin Control Register
#define FCH_AZ_REG50             0x50        // Power Management Capability ID
#define FCH_AZ_REG54             0x54        // Power Management Control/Status
#define FCH_AZ_REG60             0x60        // MSI Capability ID
#define FCH_AZ_REG64             0x64        // MSI Message Lower Address
#define FCH_AZ_REG68             0x68        // MSI Message Upper Address
#define FCH_AZ_REG6C             0x6C        // MSI Message Data

#define FCH_AZ_BAR_REG00         0x00        // Global Capabilities - R
#define FCH_AZ_BAR_REG02         0x02        // Minor Version - R
#define FCH_AZ_BAR_REG03         0x03        // Major Version - R
#define FCH_AZ_BAR_REG04         0x04        // Output Payload Capability - R
#define FCH_AZ_BAR_REG06         0x06        // Input Payload Capability - R
#define FCH_AZ_BAR_REG08         0x08        // Global Control - R/W
#define FCH_AZ_BAR_REG0C         0x0C        // Wake Enable - R/W
#define FCH_AZ_BAR_REG0E         0x0E        // State Change Status - R/W
#define FCH_AZ_BAR_REG10         0x10        // Global Status - R/W
#define FCH_AZ_BAR_REG18         0x18        // Output Stream Payload Capability - R
#define FCH_AZ_BAR_REG1A         0x1A        // Input Stream Payload Capability - R
#define FCH_AZ_BAR_REG20         0x20        // Interrupt Control - R/W
#define FCH_AZ_BAR_REG24         0x24        // Interrupt Status - R/W
#define FCH_AZ_BAR_REG30         0x30        // Wall Clock Counter - R
#define FCH_AZ_BAR_REG38         0x38        // Stream Synchronization - R/W
#define FCH_AZ_BAR_REG40         0x40        // CORB Lower Base Address - R/W
#define FCH_AZ_BAR_REG44         0x44        // CORB Upper Base Address - RW
#define FCH_AZ_BAR_REG48         0x48        // CORB Write Pointer - R/W
#define FCH_AZ_BAR_REG4A         0x4A        // CORB Read Pointer - R/W
#define FCH_AZ_BAR_REG4C         0x4C        // CORB Control - R/W
#define FCH_AZ_BAR_REG4D         0x4D        // CORB Status - R/W
#define FCH_AZ_BAR_REG4E         0x4E        // CORB Size - R/W
#define FCH_AZ_BAR_REG50         0x50        // RIRB Lower Base Address - RW
#define FCH_AZ_BAR_REG54         0x54        // RIRB Upper Address - RW
#define FCH_AZ_BAR_REG58         0x58        // RIRB Write Pointer - RW
#define FCH_AZ_BAR_REG5A         0x5A        // RIRB Response Interrupt Count - R/W
#define FCH_AZ_BAR_REG5C         0x5C        // RIRB Control - R/W
#define FCH_AZ_BAR_REG5D         0x5D        // RIRB Status - R/W
#define FCH_AZ_BAR_REG5E         0x5E        // RIRB Size - R/W
#define FCH_AZ_BAR_REG60         0x60        // Immediate Command Output Interface - R/W
#define FCH_AZ_BAR_REG64         0x64        // Immediate Command Input Interface - R/W
#define FCH_AZ_BAR_REG68         0x68        // Immediate Command Input Interface - R/W
#define FCH_AZ_BAR_REG70         0x70        // DMA Position Lower Base Address - R/W
#define FCH_AZ_BAR_REG74         0x74        // DMA Position Upper Base Address - R/W
#define FCH_AZ_BAR_REG2030       0x2030      // Wall Clock Counter Alias - R

//
//  FCH LPC Device  0x780E
//  Device 20 (0x14) Func 3
//
#define FCH_LPC_REG00            0x00        // VID- R
#define FCH_LPC_REG02            0x02        // DID- R
#define FCH_LPC_REG04            0x04        // CMD- RW
#define FCH_LPC_REG06            0x06        // STATUS- RW
#define FCH_LPC_REG08            0x08        // Revision  ID/Class Code - R
#define FCH_LPC_REG0C            0x0C        // Cache Line Size - R
#define FCH_LPC_REG0D            0x0D        // Latency Timer - R
#define FCH_LPC_REG0E            0x0E        // Header Type - R
#define FCH_LPC_REG0F            0x0F        // BIST- R
#define FCH_LPC_REG10            0x10        // Base Address Reg 0- RW*
#define FCH_LPC_REG2C            0x2C        // Subsystem ID & Subsystem Vendor ID - Wo/Ro
#define FCH_LPC_REG34            0x34        // Capabilities Pointer - Ro
#define FCH_LPC_REG40            0x40        // PCI Control - RW
#define FCH_LPC_REG44            0x44        // IO Port Decode Enable Register 1- RW
#define FCH_LPC_REG45            0x45        // IO Port Decode Enable Register 2- RW
#define FCH_LPC_REG46            0x46        // IO Port Decode Enable Register 3- RW
#define FCH_LPC_REG47            0x47        // IO Port Decode Enable Register 4- RW
#define FCH_LPC_REG48            0x48        // IO/Mem Port Decode Enable Register 5- RW
#define FCH_LPC_REG4C            0x4C        // Memory Range Register - RW
#define FCH_LPC_REG50            0x50        // Rom Protect 0 - RW
#define FCH_LPC_REG54            0x54        // Rom Protect 1 - RW
#define FCH_LPC_REG58            0x58        // Rom Protect 2 - RW
#define FCH_LPC_REG5C            0x5C        // Rom Protect 3 - RW
#define FCH_LPC_REG60            0x60        // PCI Memory Start Address of LPC Target Cycles -
#define FCH_LPC_REG62            0x62        // PCI Memory End Address of LPC Target Cycles -
#define FCH_LPC_REG64            0x64        // PCI IO base Address of Wide Generic Port - RW
#define FCH_LPC_REG68            0x68        // LPC ROM Address Range 1 (Start Address) - RW
#define FCH_LPC_REG6C            0x6C        // LPC ROM Address Range 2 (Start Address)- RW
#define FCH_LPC_REG74            0x74        // Alternative Wide IO Range Enable- W/R
#define FCH_LPC_REG7C            0x7C        // TPM (trusted plant form module) reg- W/R
#define FCH_LPC_REGA0            0x0A0       // SPI base address
#define FCH_LPC_REGA4            0x0A4
#define FCH_LPC_REGB8            0x0B8
#define FCH_LPC_REGBA            0x0BA
#define FCH_LPC_REGBB            0x0BB
#define FCH_LPC_REGC8            0x0C8
#define FCH_LPC_REGCC            0x0CC       // AutoRomCfg
#define FCH_LPC_REGD0            0x0D0

//
//  FCH GEC 0x14E4 0x1699
//  Device 20 (0x14) Func 6
//

//
//  FCH SD
//  Device 20 (0x14) Func 7
//
#define SD_PCI_REG10              0x10
#define SD_PCI_REG2C              0x2C
#define SD_PCI_REGA4              0xA4
#define SD_PCI_REGA8              0xA8
#define SD_PCI_REGB0              0xB0
#define SD_PCI_REGBC              0xBC
#define SD_PCI_REGD0              0xD0
#define SD_PCI_REGF0              0xF0
#define SD_PCI_REGF4              0xF4
#define SD_PCI_REGF8              0xF8
#define SD_PCI_REGFC              0xFC
#define FCH_SD_BAR_REG28          0x28
#define SD_CARD_PRESENT           BIT0
#define FCH_SD_BAR_REG2C          0x2C
#define FCH_SD_FREQUENCY_SLT      BIT2
#define FCH_SD_BAR_REG3C          0x3C
#define FCH_SD_1_8V               BIT3

//
//  FCH MMIO Base (SMI)
//    offset : 0x200
//
#define FCH_SMI_REG00            0x00         // EventStatus
#define FCH_SMI_REG04            0x04         // EventEnable
#define FCH_SMI_REG08            0x08         // SciTrig
#define FCH_SMI_REG0C            0x0C         // SciLevl
#define FCH_SMI_REG10            0x10         // SmiSciStatus
#define FCH_SMI_REG14            0x14         // SmiSciEn
#define FCH_SMI_REG18            0x18         // ForceSciEn
#define FCH_SMI_REG1C            0x1C         // SciRwData
#define FCH_SMI_REG3C            0x3C         // DataErrorStatus
#define FCH_SMI_REG20            0x20         // SciS0En
#define FCH_SMI_Gevent0          0x40         // SciMap0
#define FCH_SMI_Gevent1          0x41         // SciMap1
#define FCH_SMI_Gevent2          0x42         // SciMap2
#define FCH_SMI_Gevent3          0x43         // SciMap3
#define FCH_SMI_Gevent4          0x44         // SciMap4
#define FCH_SMI_Gevent5          0x45         // SciMap5
#define FCH_SMI_Gevent6          0x46         // SciMap6
#define FCH_SMI_Gevent7          0x47         // SciMap7
#define FCH_SMI_Gevent8          0x48         // SciMap8
#define FCH_SMI_Gevent9          0x49         // SciMap9
#define FCH_SMI_Gevent10         0x4A         // SciMap10
#define FCH_SMI_Gevent11         0x4B         // SciMap11
#define FCH_SMI_Gevent12         0x4C         // SciMap12
#define FCH_SMI_Gevent13         0x4D         // SciMap13
#define FCH_SMI_Gevent14         0x4E         // SciMap14
#define FCH_SMI_Gevent15         0x4F         // SciMap15
#define FCH_SMI_Gevent16         0x50         // SciMap16
#define FCH_SMI_Gevent17         0x51         // SciMap17
#define FCH_SMI_Gevent18         0x52         // SciMap18
#define FCH_SMI_Gevent19         0x53         // SciMap19
#define FCH_SMI_Gevent20         0x54         // SciMap20
#define FCH_SMI_Gevent21         0x55         // SciMap21
#define FCH_SMI_Gevent22         0x56         // SciMap22
#define FCH_SMI_Gevent23         0x57         // SciMap23
#define FCH_SMI_Usbwakup0        0x58         // SciMap24
#define FCH_SMI_Usbwakup1        0x59         // SciMap25
#define FCH_SMI_Usbwakup2        0x5A         // SciMap26
#define FCH_SMI_Usbwakup3        0x5B         // SciMap27
#define FCH_SMI_SBGppPme0        0x5C         // SciMap28
#define FCH_SMI_SBGppPme1        0x5D         // SciMap29
#define FCH_SMI_SBGppPme2        0x5E         // SciMap30
#define FCH_SMI_SBGppPme3        0x5F         // SciMap31
#define FCH_SMI_SBGppHp0         0x60         // SciMap32
#define FCH_SMI_SBGppHp1         0x61         // SciMap33
#define FCH_SMI_SBGppHp2         0x62         // SciMap34
#define FCH_SMI_SBGppHp3         0x63         // SciMap35
#define FCH_SMI_AzaliaPme        0x64         // SciMap36
#define FCH_SMI_SataGevent0      0x65         // SciMap37
#define FCH_SMI_SataGevent1      0x66         // SciMap38
#define FCH_SMI_GecPme           0x67         // SciMap39
#define FCH_SMI_IMCGevent0       0x68         // SciMap40
#define FCH_SMI_IMCGevent1       0x69         // SciMap41
#define FCH_SMI_CIRPme           0x6A         // SciMap42
#define FCH_SMI_WakePinGevent    0x6B         // SciMap43
#define FCH_SMI_FanThGevent      0x6C         // SciMap44    //FanThermalGevent
#define FCH_SMI_ASFMasterIntr    0x6D         // SciMap45
#define FCH_SMI_ASFSlaveIntr     0x6E         // SciMap46
#define FCH_SMI_SMBUS0           0x6F         // SciMap47
#define FCH_SMI_TWARN            0x70         // SciMap48
#define FCH_SMI_TMI              0x71         // SciMap49    // TrafficMonitorIntr
#define FCH_SMI_iLLB             0x72         // SciMap50
#define FCH_SMI_PowerButton      0x73         // SciMap51
#define FCH_SMI_ProcHot          0x74         // SciMap52
#define FCH_SMI_APUHwAssertion   0x75         // SciMap53
#define FCH_SMI_APUSciAssertion  0x76         // SciMap54
#define FCH_SMI_RAS              0x77         // SciMap55
#define FCH_SMI_xHC0Pme          0x78         // SciMap56
#define FCH_SMI_xHC1Pme          0x79         // SciMap57

// Empty from 0x72-0x7F
//#Define FCH_SMI_REG7C            0x7F         // SciMap63  ***

#define FCH_SMI_REG80            0x80         //  SmiStatus0
#define FCH_SMI_REG84            0x84         // SmiStatus1
#define FCH_SMI_REG88            0x88         // SmiStatus2
#define FCH_SMI_REG8C            0x8C         // SmiStatus3
#define FCH_SMI_REG90            0x90         // SmiStatus4
#define FCH_SMI_REG94            0x94         // SmiPointer
#define FCH_SMI_REG96            0x96         // SmiTimer
#define FCH_SMI_REG98            0x98         // SmiTrig
#define FCH_SMI_REG9C            0x9C         // SmiTrig
#define FCH_SMI_REGA0            0xA0
#define FCH_SMI_REGA1            0xA1
#define FCH_SMI_REGA2            0xA2
#define FCH_SMI_REGA3            0xA3
#define FCH_SMI_REGA4            0xA4
#define FCH_SMI_REGA5            0xA5
#define FCH_SMI_REGA6            0xA6
#define FCH_SMI_REGA7            0xA7
#define FCH_SMI_REGA8            0xA8
#define FCH_SMI_REGA9            0xA9
#define FCH_SMI_REGAA            0xAA
#define FCH_SMI_REGAB            0xAB
#define FCH_SMI_REGAC            0xAC
#define FCH_SMI_REGAD            0xAD
#define FCH_SMI_REGAE            0xAE
#define FCH_SMI_REGAF            0xAF
#define FCH_SMI_REGB0            0xB0
#define FCH_SMI_REGB1            0xB1
#define FCH_SMI_REGB2            0xB2
#define FCH_SMI_REGB3            0xB3
#define FCH_SMI_REGB4            0xB4
#define FCH_SMI_REGB5            0xB5
#define FCH_SMI_REGB6            0xB6
#define FCH_SMI_REGB7            0xB7
#define FCH_SMI_REGB8            0xB8
#define FCH_SMI_REGB9            0xB9
#define FCH_SMI_REGBA            0xBA
#define FCH_SMI_REGBB            0xBB
#define FCH_SMI_REGBC            0xBC
#define FCH_SMI_REGBD            0xBD
#define FCH_SMI_REGBE            0xBE
#define FCH_SMI_REGBF            0xBF
#define FCH_SMI_REGC0            0xC0
#define FCH_SMI_REGC1            0xC1
#define FCH_SMI_REGC2            0xC2
#define FCH_SMI_REGC3            0xC3
#define FCH_SMI_REGC4            0xC4
#define FCH_SMI_REGC5            0xC5
#define FCH_SMI_REGC6            0xC6
#define FCH_SMI_REGC7            0xC7
#define FCH_SMI_REGC8            0xC8
#define FCH_SMI_REGCA            0xCA         //  IoTrapping1
#define FCH_SMI_REGCC            0xCC         //  IoTrapping2
#define FCH_SMI_REGCE            0xCE         //  IoTrapping3
#define FCH_SMI_TRAPPING_WRITE   0x01
#define FCH_SMI_REGD0            0xD0         //  MemTrapping0
#define FCH_SMI_REGD4            0xD4         //  MemRdOvrData0
#define FCH_SMI_REGD8            0xD8         //  MemTrapping1
#define FCH_SMI_REGDC            0xDC         //  MemRdOvrData1
#define FCH_SMI_REGE0            0xE0         //  MemTrapping2
#define FCH_SMI_REGE4            0xE4         //  MemRdOvrData2
#define FCH_SMI_REGE8            0xE8         //  MemTrapping3
#define FCH_SMI_REGEC            0xEC         //  MemRdOvrData3
#define FCH_SMI_REGF0            0xF0         //  CfgTrapping0
#define FCH_SMI_REGF4            0xF4         //  CfgTrapping1
#define FCH_SMI_REGF8            0xF8         //  CfgTrapping2
#define FCH_SMI_REGFC            0xFC         //  CfgTrapping3

//
//  FCH MMIO Base (PMIO)
//    offset : 0x300
//
#define FCH_PMIOA_REG00          0x00
#define FCH_PMIOA_REG04          0x04
#define FCH_PMIOA_REG08          0x08
#define FCH_PMIOA_REG0C          0x0C
#define FCH_PMIOA_REG10          0x10
#define FCH_PMIOA_REG14          0x14
#define FCH_PMIOA_REG20          0x20
#define FCH_PMIOA_REG24          0x24
#define FCH_PMIOA_REG28          0x28
#define FCH_PMIOA_REG2C          0x2C
#define FCH_PMIOA_REG2E          0x2E
#define FCH_PMIOA_REG34          0x34
#define FCH_PMIOA_REG3C          0x3C
#define FCH_PMIOA_REG44          0x44
#define FCH_PMIOA_REG48          0x48
#define FCH_PMIOA_REG4C          0x4C
#define FCH_PMIOA_REG50          0x50
#define FCH_PMIOA_REG54          0x54
#define FCH_PMIOA_REG56          0x56
#define FCH_PMIOA_REG58          0x58
#define FCH_PMIOA_REG59          0x59
#define FCH_PMIOA_REG5B          0x5B
#define FCH_PMIOA_REG5C          0x5C
#define FCH_PMIOA_REG5E          0x5E
#define FCH_PMIOA_REG5F          0x5F
#define FCH_PMIOA_REG60          0x60
#define FCH_PMIOA_REG62          0x62
#define FCH_PMIOA_REG64          0x64
#define FCH_PMIOA_REG66          0x66
#define FCH_PMIOA_REG68          0x68
#define FCH_PMIOA_REG6A          0x6A
#define FCH_PMIOA_REG6C          0x6C
#define FCH_PMIOA_REG6E          0x6E
#define FCH_PMIOA_REG74          0x74
#define FCH_PMIOA_REG78          0x78
#define FCH_PMIOA_REG7A          0x7A
#define FCH_PMIOA_REG7C          0x7C
#define FCH_PMIOA_REG7E          0x7E
#define FCH_PMIOA_REG7F          0x7F
#define FCH_PMIOA_REG84          0x84
#define FCH_PMIOA_REG88          0x88
#define FCH_PMIOA_REG8C          0x8C
#define FCH_PMIOA_REG8E          0x8E
#define FCH_PMIOA_REG94          0x94
#define FCH_PMIOA_REG98          0x98
#define FCH_PMIOA_REG9C          0x9C
#define FCH_PMIOA_REG9D          0x9D
#define FCH_PMIOA_REGA0          0xA0
#define FCH_PMIOA_REGA4          0xA4
#define FCH_PMIOA_REGA8          0xA8
#define FCH_PMIOA_REGAA          0xAA
#define FCH_PMIOA_REGAC          0xAC
#define FCH_PMIOA_REGAE          0xAE
#define FCH_PMIOA_REGB0          0xB0
#define FCH_PMIOA_REGB4          0xB4
#define FCH_PMIOA_REGB8          0xB8
#define FCH_PMIOA_REGB9          0xB9
#define FCH_PMIOA_REGBA          0xBA
#define FCH_PMIOA_REGBB          0xBB
#define FCH_PMIOA_REGBC          0xBC
#define FCH_PMIOA_REGBE          0xBE
#define FCH_PMIOA_REGC0          0xC0
#define FCH_PMIOA_REGC2          0xC2
#define FCH_PMIOA_REGC4          0xC4
#define FCH_PMIOA_REGC5          0xC5
#define FCH_PMIOA_REGC8          0xC8
#define FCH_PMIOA_REGCC          0xCC
#define FCH_PMIOA_REGD0          0xD0
#define FCH_PMIOA_REGD2          0xD2
#define FCH_PMIOA_REGD3          0xD3
#define FCH_PMIOA_REGD6          0xD6
#define FCH_PMIOA_REGD7          0xD7
#define FCH_PMIOA_REGE0          0xE0
#define FCH_PMIOA_REGE8          0xE8
#define FCH_PMIOA_REGEB          0xEB
#define FCH_PMIOA_REGEC          0xEC
#define FCH_PMIOA_REGED          0xED
#define FCH_PMIOA_REGEE          0xEE
#define FCH_PMIOA_REGEF          0xEF
#define FCH_PMIOA_REGF0          0xF0
#define FCH_PMIOA_REGF2          0xF2
#define FCH_PMIOA_REGF4          0xF4
#define FCH_PMIOA_REGF8          0xF8

//
//  FCH MMIO Base (PMIO2)
//    offset : 0x400
//
#define FCH_PMIO2_REG00          0x00
#define FCH_PMIO2_REG01          0x01
#define FCH_PMIO2_REG02          0x02
#define FCH_PMIO2_REG03          0x03
#define FCH_PMIO2_REG04          0x04


#define FCH_PMIO2_REG63          0x63
#define FCH_PMIO2_REG69          0x69

#define FCH_PMIO2_REG92          0x92



//
//  FCH MMIO Base (GPIO/IoMux)
//    offset : 0x100/0xD00
//
/*
GPIO from 0 ~ 67, (GEVENT 0-23) 128 ~ 150, 160 ~ 226.
*/
#define FCH_GPIO_REG00           0x00
#define FCH_GPIO_REG06           0x06
#define FCH_GPIO_REG09           0x09
#define FCH_GPIO_REG10           0x0A
#define FCH_GPIO_REG17           0x11
#define FCH_GPIO_REG21           0x15
#define FCH_GPIO_REG28           0x1C
#define FCH_GPIO_REG32           0x20
#define FCH_GPIO_REG33           0x21
#define FCH_GPIO_REG34           0x22
#define FCH_GPIO_REG35           0x23
#define FCH_GPIO_REG36           0x24
#define FCH_GPIO_REG37           0x25
#define FCH_GPIO_REG38           0x26
#define FCH_GPIO_REG39           0x27
#define FCH_GPIO_REG40           0x28
#define FCH_GPIO_REG41           0x29
#define FCH_GPIO_REG42           0x2A
#define FCH_GPIO_REG43           0x2B
#define FCH_GPIO_REG44           0x2C
#define FCH_GPIO_REG45           0x2D
#define FCH_GPIO_REG46           0x2E
#define FCH_GPIO_REG47           0x2F
#define FCH_GPIO_REG48           0x30
#define FCH_GPIO_REG49           0x31
#define FCH_GPIO_REG50           0x32
#define FCH_GPIO_REG51           0x33
#define FCH_GPIO_REG52           0x34
#define FCH_GPIO_REG53           0x35
#define FCH_GPIO_REG54           0x36
#define FCH_GPIO_REG55           0x37
#define FCH_GPIO_REG56           0x38
#define FCH_GPIO_REG57           0x39
#define FCH_GPIO_REG58           0x3A
#define FCH_GPIO_REG59           0x3B
#define FCH_GPIO_REG60           0x3C
#define FCH_GPIO_REG61           0x3D
#define FCH_GPIO_REG62           0x3E
#define FCH_GPIO_REG63           0x3F
#define FCH_GPIO_REG64           0x40
#define FCH_GPIO_REG65           0x41
#define FCH_GPIO_REG66           0x42
#define FCH_GPIO_REG67           0x43
#define FCH_GPIO_REG68           0x44
#define FCH_GPIO_REG69           0x45
#define FCH_GPIO_REG70           0x46
#define FCH_GPIO_REG71           0x47
#define FCH_GPIO_REG72           0x48
#define FCH_GPIO_REG73           0x49
#define FCH_GPIO_REG74           0x4A
#define FCH_GPIO_REG75           0x4B
#define FCH_GPIO_REG76           0x4C
#define FCH_GPIO_REG77           0x4D
#define FCH_GPIO_REG78           0x4E
#define FCH_GPIO_REG79           0x4F
#define FCH_GPIO_REG80           0x50

// S5-DOMAIN GPIO

//
//  FCH MMIO Base (SMBUS)
//    offset : 0xA00
//
#define FCH_SMBUS_REG12           0x12        //   I2CbusConfig

//
//  FCH MMIO Base (MISC)
//    offset : 0xE00
//
#define FCH_MISC_REG00           0x00        //  ClkCntrl0
/*
FCH_MISC_REG00                   EQU     000h
  ClkCntrl0                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG04           0x04        //  ClkCntrl1
/*
FCH_MISC_REG04                   EQU     004h
  ClkCntrl1                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG08           0x08        //  ClkCntrl2
/*
FCH_MISC_REG08                   EQU     008h
  ClkCntrl2                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG0C           0x0C        //  ClkCntrl3
/*
FCH_MISC_REG0C                   EQU     00Ch
  ClkCntrl3                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG10           0x10        //  ClkCntrl4
/*
FCH_MISC_REG10                   EQU     010h
  ClkCntrl4                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG14           0x14        //   ClkCntrl5
/*
FCH_MISC_REG14                   EQU     014h
  ClkCntrl5                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG18           0x18        //   ClkCntrl6
/*
FCH_MISC_REG18                   EQU     018h
  ClkCntrl6                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG1C           0x1C
#define FCH_MISC_REG30           0x30         // OscFreqCounter
/*
FCH_MISC_REG30                   EQU     030h
  OscCounter                  EQU     0FFFFFFFFh                  ; The 32bit register shows the number of OSC clock per second.
*/
#define FCH_MISC_REG34           0x34         //  HpetClkPeriod
/*
FCH_MISC_REG34                   EQU     034h
  HpetClkPeriod               EQU     0FFFFFFFFh                  ; default - 0x429B17Eh (14.31818M).
*/
#define FCH_MISC_REG28           0x28         //  ClkDrvSth2
#define FCH_MISC_REG2C           0x2C
#define FCH_MISC_REG40           0x40         //  MiscCntrl      for clock only
#define FCH_MISC_REG50           0x50         //
#define FCH_MISC_REG6C           0x6C
/*
FCH_MISC_REG40                   EQU     040h
*/

#define FCH_MISC_REG80           0x80

#define  ChipSysNotUseFWHRom    0x0001                            // EcPwm3 pad
#define  ChipSysNotUseLpcRom    0x0002                            // Inverted version from EcPwm2 pad (default - 1)
                                                                  // Note: Both EcPwm3 and EcPwm2 straps pins are used to select boot ROM type.
#define  ChipSysEcEnable        0x0004                            // Enable Embedded Controller (EC)
#define  ChipSysBootFailTmrEn   0x0008                            // Enable Watchdog function
#define  ChipSysIntClkGen       0x0010                            // Select 25Mhz crystal clock or 100Mhz PCI-E clock **

/*
FCH_MISC_REG84                   EQU     084h
  Override FWHDisableStrap    EQU     BIT0                        ; Override FWHDiableStrap value from external pin.
  Override UseLpcRomStrap     EQU     BIT1                        ; Override UseLpcRomStrap value from external pin.
  Override EcEnableStrap      EQU     BIT2                        ; Override EcEnableStrap value from external pin.
  Override BootFailTmrEnStrap EQU     BIT3                        ; Override BootFailTmrEnStrap value from external pin.
  Override DefaultModeStrap   EQU     BIT5                        ; Override DefaultModeStrap value from external pin.
  Override I2CRomStrap        EQU     BIT7                        ; Override I2CRomStrap value from external pin.
  Override ILAAutorunEnBStrap EQU     BIT8                        ; Override ILAAutorunEnBStrap value from external pin.
  Override FcPllBypStrap      EQU     BIT9                        ; Override FcPllBypStrap value from external pin.
  Override PciPllBypStrap     EQU     BIT10                       ; Override PciPllBypStrap value from external pin.
  Override ShortResetStrap    EQU     BIT11                       ; Override ShortResetStrap value from external pin.
  Override FastBif2ClkStrap   EQU     BIT13                       ; Override FastBif2ClkStrap value from external pin
  PciRomBootStrap             EQU     BIT15                       ; Override PCI Rom Boot Strap value from external pin
  BlinkSlowModestrap          EQU     BIT16                       ; Override Blink Slow mode (100Mhz) from external pin
  ClkGenStrap                 EQU     BIT17                       ; Override CLKGEN from external pin.
  BIF_GEN2_COMPL_Strap        EQU     BIT18                       ; Override BIF_ GEN2_COMPLIANCE strap from external pin.
  StrapOverrideEn             EQU     BIT31                       ; Enable override strapping feature.
*/
#define FCH_MISC_REGC0           0xC0        //  CPU_Pstate0
/*
FCH_MISC_REGC0                   EQU     0C0h
  Core0_PState                EQU     BIT0+BIT1+BIT2              ; 000: P0  001: P1  010: P2 011: P3 100: P4 101: P5 110: P6 111: P7
  Core1_PState                EQU     BIT4+BIT5+BIT6
  Core2_PState                EQU     BIT8+BIT9+BIT10
  Core3_PState                EQU     BIT12+BIT13+BIT14
  Core4_PState                EQU     BIT16++BIT17+BIT18
  Core5_PState                EQU     BIT20+BIT21+BIT22
  Core6_PState                EQU     BIT24+BIT25+BIT26
  Core7_PState                EQU     BIT28+BIT29+BIT30
*/
#define FCH_MISC_REGC4           0xC4         //  CPU_Pstate1
/*
FCH_MISC_REGC4                   EQU     0C4h
  Core8_PState                EQU     BIT0+BIT1+BIT2              ; 000: P0  001: P1  010: P2 011: P3 100: P4 101: P5 110: P6 111: P7
  Core9_PState                EQU     BIT4+BIT5+BIT6
  Core10_PState               EQU     BIT8+BIT9+BIT10
  Core11_PState               EQU     BIT12+BIT13+BIT14
  Core12_PState               EQU     BIT16++BIT17+BIT18
  Core13_PState               EQU     BIT20+BIT21+BIT22
  Core14_PState               EQU     BIT24+BIT25+BIT26
  Core15_PState               EQU     BIT28+BIT29+BIT30
*/
#define FCH_MISC_REGD0           0xD0         //  CPU_Cstate0
/*
FCH_MISC_REGD0                   EQU     0D0h
  Core0_CState                EQU     BIT0+BIT1+BIT2              ; 000: C0  001: C1  010: C2 011: C3 100: C4 101: C5 110: C6 111: C7
  Core1_CState                EQU     BIT4+BIT5+BIT6
  Core2_CState                EQU     BIT8+BIT9+BIT10
  Core3_CState                EQU     BIT12+BIT13+BIT14
  Core4_CState                EQU     BIT16++BIT17+BIT18
  Core5_CState                EQU     BIT20+BIT21+BIT22
  Core6_CState                EQU     BIT24+BIT25+BIT26
  Core7_CState                EQU     BIT28+BIT29+BIT30
*/
#define FCH_MISC_REGD4           0xD4         //  CPU_Cstate1
/*
FCH_MISC_REGD4                   EQU     0D4h
  Core8_CState                EQU     BIT0+BIT1+BIT2              ; 000: C0  001: C1  010: C2 011: C3 100: C4 101: C5 110: C6 111: C7
  Core9_CState                EQU     BIT4+BIT5+BIT6
  Core10_CState               EQU     BIT8+BIT9+BIT10
  Core11_CState               EQU     BIT12+BIT13+BIT14
  Core12_CState               EQU     BIT16++BIT17+BIT18
  Core13_CState               EQU     BIT20+BIT21+BIT22
  Core14_CState               EQU     BIT24+BIT25+BIT26
  Core15_CState               EQU     BIT28+BIT29+BIT30
*/
#define FCH_MISC_REGF0           0xF0         //  SataPortSts
/*
FCH_MISC_REGF0                   EQU     0F0h
  Port0Sts                    EQU     BIT0                        ; The selected status of Port 0.
  Port1Sts                    EQU     BIT1                        ; The selected status of Port 1
  Port2Sts                    EQU     BIT2                        ; The selected status of Port 2.
  Port3Sts                    EQU     BIT3                        ; The selected status of Port 3
  Port4Sts                    EQU     BIT4                        ; The selected status of Port 4.
  Port5Sts                    EQU     BIT5                        ; The selected status of Port 5
  SataPortSel                 EQU     BIT24+BIT25                 ; 00  - Select "led" for Port 0 to 5
                                  ; 01 - Select "delete" for Port 0 to 5
                                  ; 10 - Select "err" for Port 0 to 5
                                  ; 11 - Select "led" for Port 0 to 5
*/

//
//  FCH MMIO Base (SERIAL_DEBUG_BASE)
//    offset : 0x1000
//

#define FCH_RTC_REG00            0x00        // Seconds - RW
#define FCH_RTC_REG01            0x01        // Seconds Alarm - RW
#define FCH_RTC_REG02            0x02        // Minutes - RW
#define FCH_RTC_REG03            0x03        // Minutes Alarm - RW
#define FCH_RTC_REG04            0x04        // ours - RW
#define FCH_RTC_REG05            0x05        // ours Alarm- RW
#define FCH_RTC_REG06            0x06        // Day of Week - RW
#define FCH_RTC_REG07            0x07        // Date of Mont - RW
#define FCH_RTC_REG08            0x08        // Mont - RW
#define FCH_RTC_REG09            0x09        // Year - RW
#define FCH_RTC_REG0A            0x0A        // Register A - RW
#define FCH_RTC_REG0B            0x0B        // Register B - RW
#define FCH_RTC_REG0C            0x0C        // Register C - R
#define FCH_RTC_REG0D            0x0D        // DateAlarm - RW
#define FCH_RTC_REG32            0x32        // AltCentury - RW
#define FCH_RTC_REG48            0x48        // Century - RW
#define FCH_RTC_REG50            0x50        // Extended RAM Address Port - RW
#define FCH_RTC_REG53            0x53        // Extended RAM Data Port - RW
#define FCH_RTC_REG7E            0x7E        // RTC Time Clear - RW
#define FCH_RTC_REG7F            0x7F        // RTC RAM Enable - RW


#define FCH_IOMAP_REG00          0x000   // Dma_C 0
#define FCH_IOMAP_REG02          0x002   // Dma_C 1
#define FCH_IOMAP_REG04          0x004   // Dma_C 2
#define FCH_IOMAP_REG06          0x006   // Dma_C 3
#define FCH_IOMAP_REG08          0x008   // Dma_Status
#define FCH_IOMAP_REG09          0x009   // Dma_WriteRest
#define FCH_IOMAP_REG0A          0x00A   // Dma_WriteMask
#define FCH_IOMAP_REG0B          0x00B   // Dma_WriteMode
#define FCH_IOMAP_REG0C          0x00C   // Dma_Clear
#define FCH_IOMAP_REG0D          0x00D   // Dma_MasterClr
#define FCH_IOMAP_REG0E          0x00E   // Dma_ClrMask
#define FCH_IOMAP_REG0F          0x00F   // Dma_AllMask
#define FCH_IOMAP_REG20          0x020   // IntrCntrlReg1
#define FCH_IOMAP_REG21          0x021   // IntrCntrlReg2
#define FCH_IOMAP_REG40          0x040   // TimerC0
#define FCH_IOMAP_REG41          0x041   // TimerC1
#define FCH_IOMAP_REG42          0x042   // TimerC2
#define FCH_IOMAP_REG43          0x043   // Tmr1CntrlWord
#define FCH_IOMAP_REG61          0x061   // Nmi_Status
#define FCH_IOMAP_REG70          0x070   // Nmi_Enable
#define FCH_IOMAP_REG71          0x071   // RtcDataPort
#define FCH_IOMAP_REG72          0x072   // AlternatRtcAddrPort
#define FCH_IOMAP_REG73          0x073   // AlternatRtcDataPort
#define FCH_IOMAP_REG80          0x080   // Dma_Page_Reserved0
#define FCH_IOMAP_REG81          0x081   // Dma_PageC2
#define FCH_IOMAP_REG82          0x082   // Dma_PageC3
#define FCH_IOMAP_REG83          0x083   // Dma_PageC1
#define FCH_IOMAP_REG84          0x084   // Dma_Page_Reserved1
#define FCH_IOMAP_REG85          0x085   // Dma_Page_Reserved2
#define FCH_IOMAP_REG86          0x086   // Dma_Page_Reserved3
#define FCH_IOMAP_REG87          0x087   // Dma_PageC0
#define FCH_IOMAP_REG88          0x088   // Dma_Page_Reserved4
#define FCH_IOMAP_REG89          0x089   // Dma_PageC6
#define FCH_IOMAP_REG8A          0x08A   // Dma_PageC7
#define FCH_IOMAP_REG8B          0x08B   // Dma_PageC5
#define FCH_IOMAP_REG8C          0x08C   // Dma_Page_Reserved5
#define FCH_IOMAP_REG8D          0x08D   // Dma_Page_Reserved6
#define FCH_IOMAP_REG8E          0x08E   // Dma_Page_Reserved7
#define FCH_IOMAP_REG8F          0x08F   // Dma_Refres
#define FCH_IOMAP_REG92          0x092   // FastInit
#define FCH_IOMAP_REGA0          0x0A0   // IntrCntrl2Reg1
#define FCH_IOMAP_REGA1          0x0A1   // IntrCntrl2Reg2
#define FCH_IOMAP_REGC0          0x0C0   // Dma2_C4Addr
#define FCH_IOMAP_REGC2          0x0C2   // Dma2_C4Cnt
#define FCH_IOMAP_REGC4          0x0C4   // Dma2_C5Addr
#define FCH_IOMAP_REGC6          0x0C6   // Dma2_C5Cnt
#define FCH_IOMAP_REGC8          0x0C8   // Dma2_C6Addr
#define FCH_IOMAP_REGCA          0x0CA   // Dma2_C6Cnt
#define FCH_IOMAP_REGCC          0x0CC   // Dma2_C7Addr
#define FCH_IOMAP_REGCE          0x0CE   // Dma2_C7Cnt
#define FCH_IOMAP_REGD0          0x0D0   // Dma_Status
#define FCH_IOMAP_REGD2          0x0D2   // Dma_WriteRest
#define FCH_IOMAP_REGD4          0x0D4   // Dma_WriteMask
#define FCH_IOMAP_REGD6          0x0D6   // Dma_WriteMode
#define FCH_IOMAP_REGD8          0x0D8   // Dma_Clear
#define FCH_IOMAP_REGDA          0x0DA   // Dma_Clear
#define FCH_IOMAP_REGDC          0x0DC   // Dma_ClrMask
#define FCH_IOMAP_REGDE          0x0DE   // Dma_ClrMask
#define FCH_IOMAP_REGF0          0x0F0   // NCP_Error
#define FCH_IOMAP_REGC00         0x0C00  // Pci_Intr_Index
#define FCH_IOMAP_REGC01         0x0C01  // Pci_Intr_Data
#define FCH_IOMAP_REGC14         0x0C14  // Pci_Error
#define FCH_IOMAP_REGCD0         0x0CD0  // PMio2_Index
#define FCH_IOMAP_REGCD1         0x0CD1  // PMio2_Data
#define FCH_IOMAP_REGCD4         0x0CD4  // BIOSRAM_Index
#define FCH_IOMAP_REGCD5         0x0CD5  // BIOSRAM_Data
#define FCH_IOMAP_REGCD6         0x0CD6  // PM_Index
#define FCH_IOMAP_REGCD7         0x0CD7  // PM_Data
#define FCH_IOMAP_REGCF9         0x0CF9          // CF9Rst reg

#define FCH_IRQ_INTA             0x00    // INTA#
#define FCH_IRQ_INTB             0x01    // INTB#
#define FCH_IRQ_INTC             0x02    // INTC#
#define FCH_IRQ_INTD             0x03    // INTD#
#define FCH_IRQ_INTE             0x04    // INTE#
#define FCH_IRQ_INTF             0x05    // INTF#
#define FCH_IRQ_INTG             0x06    // INTG#
#define FCH_IRQ_INTH             0x07    // INTH#
#define FCH_IRQ_SCI              0x10    // SCI
#define FCH_IRQ_SMBUS0           0x11    // SMBUS0
#define FCH_IRQ_ASF              0x12    // ASF
#define FCH_IRQ_HDAUDIO          0x13    // HD Audio
#define FCH_IRQ_FC               0x14    // FC
#define FCH_IRQ_GEC              0x15    // GEC
#define FCH_IRQ_SD               0x17    // SD
#define FCH_IRQ_IMCINT0          0x20    // IMC INT0
#define FCH_IRQ_IMCINT1          0x21    // IMC INT1
#define FCH_IRQ_IMCINT2          0x22    // IMC INT2
#define FCH_IRQ_IMCINT3          0x23    // IMC INT3
#define FCH_IRQ_IMCINT4          0x24    // IMC INT4
#define FCH_IRQ_IMCINT5          0x25    // IMC INT5
#define FCH_IRQ_USB18INTA        0x30    // Dev 18 (USB) INTA#
#define FCH_IRQ_USB18INTB        0x31    // Dev 18 (USB) INTB#
#define FCH_IRQ_USB19INTA        0x32    // Dev 19 (USB) INTA#
#define FCH_IRQ_USB19INTB        0x33    // Dev 19 (USB) INTB#
#define FCH_IRQ_USB22INTA        0x34    // Dev 22 (USB) INTA#
#define FCH_IRQ_USB22INTB        0x35    // Dev 22 (USB) INTB#
#define FCH_IRQ_USB20INTC        0x36    // Dev 20 (USB) INTC#
#define FCH_IRQ_IDE              0x40    // IDE pci interrupt
#define FCH_IRQ_SATA             0x41    // SATA pci interrupt
#define FCH_IRQ_GPPINT0          0x50    // Gpp Int0
#define FCH_IRQ_GPPINT1          0x51    // Gpp Int1
#define FCH_IRQ_GPPINT2          0x52    // Gpp Int2
#define FCH_IRQ_GPPINT3          0x53    // Gpp Int3
#define FCH_IRQ_IOAPIC           0x80    // Select IRQ routing to IoApic mode
#define FCH_IRQ_PIC              0x00    // Select IRQ routing to PIC mode

#define FCH_SPI_MMIO_REG00       0x00        //SPI_
#define FCH_SPI_OPCODE           0x000000FFl  //
#define FCH_SPI_TX_COUNT         0x00000F00l  //
#define FCH_SPI_RX_COUNT         0x0000F000l  //
#define FCH_SPI_EXEC_OPCODE      0x00010000l  //
#define FCH_SPI_FIFO_PTR_CRL     0x00100000l  //
#define FCH_SPI_FIFO_PTR_INC     0x00200000l  //
#define FCH_SPI_BUSY             0x80000000l  //
#define FCH_SPI_MMIO_REG0C       0x0C        //SPI_Cntrl1 Register
#define FCH_SPI_PARAMETER        0x000000FFl  //
#define FCH_SPI_FIFO_PTR         0x00000700l  //
#define FCH_SPI_BYTE_PROGRAM     0xFF000000l  //
#define FCH_SPI_MMIO_REG1C       0x1C        //
#define FCH_SPI_RETRY_TIMES      0x3         //

#define FCH_SPI_MMIO_REG1D       0x1D        //
#define FCH_SPI_MMIO_REG1E       0x1E        //
#define FCH_SPI_MMIO_REG1F       0x1F        //

#define FCH_SPI_MMIO_REG1F_X05_TX_BYTE_COUNT   0x05        //
#define FCH_SPI_MMIO_REG1F_X06_RX_BYTE_COUNT   0x06        //

#define FCH_SPI_MMIO_REG20       0x20        //
#define FCH_SPI_MMIO_REG22       0x22        //

#define FCH_SPI_MMIO_REG45_CMDCODE           0x45        //
#define FCH_SPI_MMIO_REG47_CMDTRIGGER        0x47        //
#define FCH_SPI_MMIO_REG48_TXBYTECOUNT       0x48        //
#define FCH_SPI_MMIO_REG4B_RXBYTECOUNT       0x4B        //
#define FCH_SPI_MMIO_REG4C_SPISTATUS         0x4C        //
#define FCH_SPI_MMIO_REG80_FIFO              0x80        //

#define FCH_SPI_MODE_FAST        0x7        //
#define FCH_SPI_MODE_NORMAL      0x6        //
#define FCH_SPI_MODE_QUAL_144    0x5        //
#define FCH_SPI_MODE_QUAL_122    0x4        //
#define FCH_SPI_MODE_QUAL_114    0x3        //
#define FCH_SPI_MODE_QUAL_112    0x2        //

#define FCH_SPI_DEVICE_MODE_DIS  0x7        //
#define FCH_SPI_DEVICE_MODE_144  0x4        //
#define FCH_SPI_DEVICE_MODE_114  0x3        //
#define FCH_SPI_DEVICE_MODE_122  0x2        //
#define FCH_SPI_DEVICE_MODE_112  0x1        //
#define FCH_SPI_DEVICE_MODE_FAST 0x0        //

#define FCH_SPI_SPEED_16M        0x4        //
#define FCH_SPI_SPEED_22M        0x3        //
#define FCH_SPI_SPEED_33M        0x2        //
#define FCH_SPI_SPEED_66M        0x1        //
#define FCH_SPI_SPEED_100M       0x5        //

#define AMD_NB_REG78            0x78
#define AMD_NB_SCRATCH          AMD_NB_REG78
#define MailBoxPort             0x3E

#define MAX_LT_POLLINGS         0x4000
#define SMI_TIMER_ENABLE        BIT15


#define ACPIMMIO32(x) (*(volatile UINT32*)(UINTN)(x))
#define ACPIMMIO16(x) (*(volatile UINT16*)(UINTN)(x))
#define ACPIMMIO8(x)  (*(volatile UINT8*)(UINTN)(x))

#define U3PLL_LOCK                   BIT7
#define U3PLL_RESET                  BIT8
#define U3PHY_RESET                  BIT9
#define U3CORE_RESET                 BIT10
#define XHC0_FUNC_RESET              BIT11
#define XHC1_FUNC_RESET              BIT12

#define XHCI_ACPI_MMIO_AMD_REG00     0x00
#define XHCI_ACPI_MMIO_AMD_REG04     0x04
#define XHCI_ACPI_MMIO_AMD_REG08     0x08
#define XHCI_ACPI_MMIO_AMD_REG10     0x10
#define XHCI_ACPI_MMIO_AMD_REG14     0x14
#define XHCI_ACPI_MMIO_AMD_REG20     0x20
#define XHCI_ACPI_MMIO_AMD_REG24     0x24
#define XHCI_ACPI_MMIO_AMD_REG30     0x30
#define XHCI_ACPI_MMIO_AMD_REG40     0x40
#define XHCI_ACPI_MMIO_AMD_REG48     0x48 // USB3.0_Ind_REG Index
#define XHCI_ACPI_MMIO_AMD_REG4C     0x4C // USB2.0_Ind_REG Data
#define XHCI_ACPI_MMIO_AMD_REG8C     0x8C
#define XHCI_ACPI_MMIO_AMD_REG90     0x90 // adaptation timer settings
#define XHCI_ACPI_MMIO_AMD_REGA0     0xA0 // BAR 0
#define XHCI_ACPI_MMIO_AMD_REGA4     0xA4 // BAR 1
#define XHCI_ACPI_MMIO_AMD_REGA8     0xA8 // BAR 2
#define XHCI_ACPI_MMIO_AMD_REGB0     0xB0 // SPI_Valid_Base.
#define XHCI_ACPI_MMIO_AMD_REGC0     0xC0 // Firmware starting offset for coping
#define XHCI_ACPI_MMIO_AMD_REGB4     0xB4
#define XHCI_ACPI_MMIO_AMD_REGD0     0xD0

#define FCH_XHCI_REG48                0x48 // XHCI IND_REG Index registers
#define FCH_XHCI_REG4C                0x4C // XHCI IND_REG Data registers

#define FCH_XHCI_IND60_BASE           0x40000000ul //

#define FCH_XHCI_IND60_REG00           FCH_XHCI_IND60_BASE + 0x00 //
#define FCH_XHCI_IND60_REG04           FCH_XHCI_IND60_BASE + 0x04 //
#define FCH_XHCI_IND60_REG08           FCH_XHCI_IND60_BASE + 0x08 //
#define FCH_XHCI_IND60_REG0C           FCH_XHCI_IND60_BASE + 0x0C //
#define FCH_XHCI_IND60_REG18           FCH_XHCI_IND60_BASE + 0x18 //
#define FCH_XHCI_IND60_REG48           FCH_XHCI_IND60_BASE + 0x48 //
#define FCH_XHCI_IND60_REG50           FCH_XHCI_IND60_BASE + 0x50 //
#define FCH_XHCI_IND60_REG54           FCH_XHCI_IND60_BASE + 0x54 //

#define FCH_XHCI_IND_REG00            0x00
#define FCH_XHCI_IND_REG04            0x04
#define FCH_XHCI_IND_REG48            0x48
#define FCH_XHCI_IND_REG54            0x54
#define FCH_XHCI_IND_REG88            0x88
#define FCH_XHCI_IND_REG94            0x94
#define FCH_XHCI_IND_REG98            0x98
#define FCH_XHCI_IND_REGC8            0xC8
#define FCH_XHCI_IND_REGD4            0xD4
#define FCH_XHCI_IND_REGD8            0xD8
#define FCH_XHCI_IND_REG100           0x100
#define FCH_XHCI_IND_REG120           0x120
#define FCH_XHCI_IND_REG128           0x128
#define MAX_XHCI_PORTS                0x04

//SMBUS
#define FCH_SMB_IOREG00               0x00        // SMBusStatus
#define FCH_SMB_IOREG01               0x01        // SMBusSlaveStatus
#define FCH_SMB_IOREG02               0x02        // SMBusControl
#define FCH_SMB_IOREG03               0x03        // SMBusHostCmd
#define FCH_SMB_IOREG04               0x04        // SMBusAddress
#define FCH_SMB_IOREG05               0x05        // SMBusData0
#define FCH_SMB_IOREG06               0x06        // SMBusData1
#define FCH_SMB_IOREG07               0x07        // SMBusBlockData
#define FCH_SMB_IOREG08               0x08        // SMBusSlaveControl
#define FCH_SMB_IOREG14               0x14        // SMBusAutoPoll
#define FCH_SMB_IOREG16               0x16        // SMBusPausePoll
#define FCH_SMB_IOREG17               0x17        // SMBusHostCmd2

#define FCH_SMB_CMD_QUICK             0x00 << 2   // Quick Read or Write
#define FCH_SMB_CMD_BYTE              0x01 << 2   // Byte Read or Write
#define FCH_SMB_CMD_BYTE_DATA         0x02 << 2   // Byte Data Read or Write
#define FCH_SMB_CMD_WORD_DATA         0x03 << 2   // Word Data Read or Write
#define FCH_SMB_CMD_BLOCK             0x05 << 2   // Block Read or Write

#define FCH_SMB_ALL_HOST_STATUS       0x1f        // HostBusy+SMBInterrupt+DeviceErr+BusCollision+Failed
#define FCH_SMB_CMD_BYTE_DATA_START   0x48        // Byte Data Read or Write
#define FCH_SMB_CMD_START             BIT6
#define FCH_SMB_READ_ENABLE           BIT0
#define FCH_SMB_AUTO_POLL_EN          BIT0
#define FCH_SMB_POLL2BYTE             BIT7


#define FCH_EC_ENTER_CONFIG           0x5A
#define FCH_EC_EXIT_CONFIG            0xA5
#define FCH_EC_REG07                  0x07
#define FCH_EC_REG30                  0x30
#define FCH_EC_REG60                  0x60
#define FCH_EC_REG61                  0x61

#define FCH_IMC_ROMSIG                0x55aa55aaul

#define SPI_HEAD_LENGTH 0x0E
#define SPI_BAR0_VLD  0x01
#define SPI_BASE0     (0x00 << 7)
#define SPI_BAR1_VLD  (0x01 << 8)
#define SPI_BASE1     (SPI_HEAD_LENGTH << 10)
#define SPI_BAR2_VLD  (0x01 << 16)
#define SPI_BASE2(x)  ((SPI_HEAD_LENGTH + ACPIMMIO16(x)) << 18)

#define FW_TO_SIGADDR_OFFSET  0x0C
#define BCD_ADDR_OFFSET       0x02
#define BCD_SIZE_OFFSET       0x04
#define FW_ADDR_OFFSET        0x06
#define FW_SIZE_OFFSET        0x08
#define ACD_ADDR_OFFSET       0x0A
#define ACD_SIZE_OFFSET       0x0C
#define XHC_BOOT_RAM_SIZE     0x8000

#define PKT_DATA_REG       ACPI_MMIO_BASE + GFX_DAC_BASE + 0x00
#define PKT_LEN_REG        ACPI_MMIO_BASE + GFX_DAC_BASE + 0x14
#define PKT_CTRL_REG       ACPI_MMIO_BASE + GFX_DAC_BASE + 0x15
#define EFUS_DAC_ADJUSTMENT_CONTROL    0x850A8ul
#define BGADJ                             0x1F
#define DACADJ                            0x1B
#define EFUS_DAC_ADJUSTMENT_CONTROL_DATA  (BGADJ + (DACADJ << 8) + BIT16 )

#define KABINI_OSC_OUT_CLOCK_SEL_48MHz  0x02
#define KABINI_OSC_OUT_CLOCK_SEL_25MHz  0x01

#define RTC_WORKAROUND_SECOND           0x00
#define RTC_VALID_SECOND_VALUE          0x59
#define RTC_SECOND_RESET_VALUE          0x30
#define RTC_SECOND_LOWER_NIBBLE         0x0F
#define RTC_VALID_SECOND_VALUE_LN       0x09

#ifndef FCH_DEADLOOP
  #define FCH_DEADLOOP()    { volatile UINTN __i; __i = 1; while (__i); }
#endif

