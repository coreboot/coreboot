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
 * @e \$Revision: 64988 $   @e \$Date: 2012-02-06 03:07:17 -0600 (Mon, 06 Feb 2012) $
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
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgGecShadowRomBase   </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgSpiRomBaseAddress  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgAcpiPm1EvtBlkAddr  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgAcpiPm1CntBlkAddr  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgAcpiPmTmrBlkAddr   </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgCpuControlBlkAddr  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgAcpiGpe0BlkAddr    </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgSmiCmdPortAddr     </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> BUILD_OPT_CFG::CfgAcpiPmaCntBlkAddr  </TD><TD class="indexvalue"><B>Required </B></TD></TR>
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
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::SataEnable            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::IdeEnable             </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> FCH_INTERFACE::XhciSwitch            </TD><TD class="indexvalue"><B>Optional </B></TD></TR>
 *
 */

/*--------------------------- Documentation Pages ---------------------------*/
/**
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
#define ATI_VID                      0x1002
#define AMD_FCH_VID                  0x1022
#define FCH_DEVICE_ID                0x780B
#define FCH_SATA_VID                 AMD_FCH_VID      // Dev 17 Func 0
#define FCH_SATA_DID                 0x7800
#define FCH_SATA_AHCI_DID            0x7801
#define FCH_SATA_RAID_DID            0x7802
#define FCH_SATA_RAID5_DID           0x7803
#define FCH_SATA_AMDAHCI_DID         0x7804
#define FCH_USB_OHCI_VID             AMD_FCH_VID      // Dev 18 Func 0, Dev 19 Func 0
#define FCH_USB_OHCI_DID             0x7807
#define FCH_USB_EHCI_VID             AMD_FCH_VID      // Dev 18 Func 2, Dev 19 Func 2
#define FCH_USB_EHCI_DID             0x7808
#define FCH_USB_XHCI_VID             AMD_FCH_VID      // Dev 10 Func 0, Dev 10 Func 1
#define FCH_USB_XHCI_DID             0x7812
#define FCH_SMBUS_VID                AMD_FCH_VID      // Dev 20 Func 0
#define FCH_SMBUS_DID                0x780B
#define FCH_IDE_VID                  AMD_FCH_VID      // Dev 20 Func 1
#define FCH_IDE_DID                  0x780C
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

//FCH Variant
#define FCH_Variant_EFUSE_LOCATION        0x1E        // EFUSE bit 240-247

#define FCH_M2                            0x01
#define FCH_M3                            0x03
#define FCH_M3T                           0x07
#define FCH_D2                            0x0F
#define FCH_D3                            0x1F
#define FCH_D4                            0x3F


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
#define LPC_BUS_DEV_FUN             ((0x14 << 3) + 3)
#define FCH_LPC_BUS          0
#define FCH_LPC_DEV          20
#define FCH_LPC_FUNC         3
#define PCIB_BUS_DEV_FUN            ((0x14 << 3) + 4)    // P2P in SB700
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
#define SD_PCI_REG10        0x10
#define SD_PCI_REG2C        0x2C
#define SD_PCI_REGA4        0xA4
#define SD_PCI_REGA8        0xA8
#define SD_PCI_REGAC        0xAC
#define SD_PCI_REGB0        0xB0


#define FCH_GPP_BUS          0
#define FCH_GPP_DEV          21
#define FCH_GPP_FUNC         0
#define GPP0_BUS_DEV_FUN            ((0x15 << 3) + 0)    // GPP P2P bridge PORT0
#define GPP1_BUS_DEV_FUN            ((0x15 << 3) + 1)    // GPP P2P bridge PORT1
#define GPP2_BUS_DEV_FUN            ((0x15 << 3) + 2)    // GPP P2P bridge PORT2
#define GPP3_BUS_DEV_FUN            ((0x15 << 3) + 3)    // GPP P2P bridge PORT3

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
#define CEC_BASE           0x1800
#define XHCI_BASE          0x1C00


// RegSpace field (AB_INDEX[31:29]
#define AXINDC         0               // AXINDC
#define AXINDP         2               // AXINDP
#define ABCFG          6               // ABCFG
#define AXCFG          4               // AXCFG
#define RCINDXC        1               // PCIEIND
#define RCINDXP        3               // PCIEIND_P

#define GPP_DEV_NUM    21              //
#define MAX_GPP_PORTS  4

#define PCIE_FORCE_GEN1_EFUSE_LOCATION     0x14    // EFUSE bit 160
//
// ABCFG Registers
//
#define FCH_ABCFG_REG00     0x00  // VENDOR ID
#define FCH_ABCFG_REG08     0x08  // REVISION ID
#define FCH_ABCFG_REG40     0x40  // BL_EVENTCNT0LO
#define FCH_ABCFG_REG44     0x44  // BL_EVENTCNT1LO
#define FCH_ABCFG_REG48     0x48  // BL_EVENTCNTSEL
#define FCH_ABCFG_REG4A     0x4A  // BL_EVENTCNT0HI
#define FCH_ABCFG_REG4B     0x4B  // BL_EVENTCNT1HI
#define FCH_ABCFG_REG4C     0x4C  // BL_EVENTCNTCTL
#define FCH_ABCFG_REG50     0x50  // MISCCTL_50
#define FCH_ABCFG_REG54     0x54  // MISCCTL_54
#define FCH_ABCFG_REG58     0x58  // BL RAB CONTROL

#define FCH_ABCFG_REG60     0x60  // LINKWIDTH_CTL
#define FCH_ABCFG_REG64     0x64  // LINKWIDTH_UP_INTERVAL
#define FCH_ABCFG_REG68     0x68  // LINKWIDTH_DN_INVERVAL
#define FCH_ABCFG_REG6C     0x6C  // LINKWIDTH_UPSTREAM_DWORDS
#define FCH_ABCFG_REG70     0x70  // LINKWIDTH_DOWNSTREAM_DWORDS
#define FCH_ABCFG_REG74     0x74  // LINKWIDTH_THRESHOLD_INCREASE
#define FCH_ABCFG_REG78     0x78  // LINKWIDTH_THRESHOLD_DECREASE

#define FCH_ABCFG_REG80     0x80  // BL DMA PREFETCH CONTROL
#define FCH_ABCFG_REG88     0x88  //
#define FCH_ABCFG_REG90     0x90  // BIF CONTROL 0
#define FCH_ABCFG_REG94     0x94  // MSI CONTROL
#define FCH_ABCFG_REG98     0x98  // BIF CONTROL 1
#define FCH_ABCFG_REG9C     0x9C  // MISCCTL_9C
#define FCH_ABCFG_REGA0     0xA0  // BIF PHY CONTROL ENABLE
#define FCH_ABCFG_REGA4     0xA4  // BIF PHY CONTROL A4
#define FCH_ABCFG_REGA8     0xA8  // BIF PHY CONTROL A8
#define FCH_ABCFG_REGB4     0xB4  //
#define FCH_ABCFG_REGC0     0xC0  // PCIE_GPP_ENABLE
#define FCH_ABCFG_REGF0     0xF0  // GPP_UPSTREAM_CONTROL
#define FCH_ABCFG_REGF4     0xF4  // GPP_SYSTEM_ERROR_CONTROL
#define FCH_ABCFG_REG208    0x208 // KR New
#define FCH_ABCFG_REG300    0x300 // MCTP_VDM_RX_SMI_CONTROL
#define FCH_ABCFG_REG31C    0x31C // BIF_GPP_STRAP_LINK_CONTROL_0
#define FCH_ABCFG_REG330    0x330 // BIF_GPP_STRAP_BIF_0
#define FCH_ABCFG_REG340    0x340 // BIF_GPP_STRAP_BIF_LANE_A
#define FCH_ABCFG_REG344    0x344 // BIF_GPP_STRAP_BIF_LANE_B
#define FCH_ABCFG_REG348    0x348 // BIF_GPP_STRAP_BIF_LANE_C
#define FCH_ABCFG_REG34C    0x34C // BIF_GPP_STRAP_BIF_LANE_D
#define FCH_ABCFG_REG404    0x404 // GPP0_SHADOW_COMMAND
#define FCH_ABCFG_REG418    0x418 // GPP0_SHADOW_BUS_NUMBER
#define FCH_ABCFG_REG41C    0x41C // GPP0_SHADOW_IO_LIMIT_BASE
#define FCH_ABCFG_REG420    0x420 // GPP0_SHADOW_MEM_LIMIT_BASE
#define FCH_ABCFG_REG424    0x424 // GPP0_SHADOW_PREF_MEM_LIMIT_BASE
#define FCH_ABCFG_REG428    0x428 // GPP0_SHADOW_PREF_MEM_BASE_UPPER
#define FCH_ABCFG_REG42C    0x42C // GPP0_SHADOW_PREF_MEM_LIMIT_UPPER
#define FCH_ABCFG_REG430    0x430 // GPP0_SHADOW_IO_LIMIT_BASE_UPPER
#define FCH_ABCFG_REG43C    0x43C // GPP0_SHADOW_BRIDGE_CONTROL
#define FCH_ABCFG_REG444    0x444 // GPP1_SHADOW_COMMAND
#define FCH_ABCFG_REG458    0x458 // GPP1_SHADOW_BUS_NUMBER
#define FCH_ABCFG_REG45C    0x45C // GPP1_SHADOW_IO_LIMIT_BASE
#define FCH_ABCFG_REG460    0x460 // GPP1_SHADOW_MEM_LIMIT_BASE
#define FCH_ABCFG_REG464    0x464 // GPP1_SHADOW_PREF_MEM_LIMIT_BASE
#define FCH_ABCFG_REG468    0x468 // GPP1_SHADOW_PREF_MEM_BASE_UPPER
#define FCH_ABCFG_REG46C    0x46C // GPP1_SHADOW_PREF_MEM_LIMIT_UPPER
#define FCH_ABCFG_REG470    0x470 // GPP1_SHADOW_IO_LIMIT_BASE_UPPER
#define FCH_ABCFG_REG47C    0x47C // GPP1_SHADOW_BRIDGE_CONTROL
#define FCH_ABCFG_REG484    0x484 // GPP2_SHADOW_COMMAND
#define FCH_ABCFG_REG498    0x498 // GPP2_SHADOW_BUS_NUMBER
#define FCH_ABCFG_REG49C    0x49C // GPP2_SHADOW_IO_LIMIT_BASE
#define FCH_ABCFG_REG4A0    0x4A0 // GPP2_SHADOW_MEM_LIMIT_BASE
#define FCH_ABCFG_REG4A4    0x4A4 // GPP2_SHADOW_PREF_MEM_LIMIT_BASE
#define FCH_ABCFG_REG4A8    0x4A8 // GPP2_SHADOW_PREF_MEM_BASE_UPPER
#define FCH_ABCFG_REG4AC    0x4AC // GPP2_SHADOW_PREF_MEM_LIMIT_UPPER
#define FCH_ABCFG_REG4B0    0x4B0 // GPP2_SHADOW_IO_LIMIT_BASE_UPPER
#define FCH_ABCFG_REG4BC    0x4BC // GPP2_SHADOW_BRIDGE_CONTROL
#define FCH_ABCFG_REG4C4    0x4C4 // GPP3_SHADOW_COMMAND
#define FCH_ABCFG_REG4D8    0x4D8 // GPP3_SHADOW_BUS_NUMBER
#define FCH_ABCFG_REG4DC    0x4DC // GPP3_SHADOW_IO_LIMIT_BASE
#define FCH_ABCFG_REG4E0    0x4E0 // GPP3_SHADOW_MEM_LIMIT_BASE
#define FCH_ABCFG_REG4E4    0x4E4 // GPP3_SHADOW_PREF_MEM_LIMIT_BASE
#define FCH_ABCFG_REG4E8    0x4E8 // GPP3_SHADOW_PREF_MEM_BASE_UPPER
#define FCH_ABCFG_REG4EC    0x4EC // GPP3_SHADOW_PREF_MEM_LIMIT_UPPER
#define FCH_ABCFG_REG4F0    0x4F0 // GPP3_SHADOW_IO_LIMIT_BASE_UPPER
#define FCH_ABCFG_REG4FC    0x4FC // GPP3_SHADOW_BRIDGE_CONTROL
#define FCH_ABCFG_REG10040  0x10040ul  // AL_EVENTCNT0LO
#define FCH_ABCFG_REG10044  0x10044ul  // AL_EVENTCNT1LO
#define FCH_ABCFG_REG1004A  0x1004Aul  // AL_EVENTCNT0HI
#define FCH_ABCFG_REG1004B  0x1004Bul  // AL_EVENTCNT1HI
#define FCH_ABCFG_REG10050  0x10050ul  // MISCCTL_10050
#define FCH_ABCFG_REG10054  0x10054ul  // AL_ARB_CTL
#define FCH_ABCFG_REG10056  0x10056ul  // AL_CLK_CTL
#define FCH_ABCFG_REG10058  0x10058ul  // AL RAB CONTROL
#define FCH_ABCFG_REG1005C  0x1005Cul  // AL MLT CONTROL
#define FCH_ABCFG_REG10060  0x10060ul  // AL DMA PREFETCH ENABLE
#define FCH_ABCFG_REG10064  0x10064ul  // AL DMA PREFETCH FLUSH CONTROL
#define FCH_ABCFG_REG10068  0x10068ul  // AL PREFETCH LIMIT
#define FCH_ABCFG_REG1006C  0x1006Cul  // AL DMA PREFETCH CONTROL
#define FCH_ABCFG_REG10070  0x10070ul  // MISCCTL_10070
#define FCH_ABCFG_REG10080  0x10080ul  // CLKMUXSTATUS
#define FCH_ABCFG_REG10090  0x10090ul  // BIF CONTROL 0
#define FCH_ABCFG_REG1009C  0x1009Cul  // MISCCTL_1009C

//
// RCINDX_P Registers
//
#define FCH_RCINDXP_REG01   0x01 | RCINDXP << 29 // PCIEP_SCRATCH
#define FCH_RCINDXP_REG02   0x02 | RCINDXP << 29 //
#define FCH_RCINDXP_REG10   0x10 | RCINDXP << 29 //
#define FCH_RCINDXP_REG20   0x20 | RCINDXP << 29 // PCIE_TX_CNTL
#define FCH_RCINDXP_REG21   0x21 | RCINDXP << 29 // PCIE_TX_REQUESTER_ID
#define FCH_RCINDXP_REG50   0x50 | RCINDXP << 29 // PCIE_P_PORT_LANE_STATUS
#define FCH_RCINDXP_REG6A   0x6A | RCINDXP << 29 //
#define FCH_RCINDXP_REG70   0x70 | RCINDXP << 29 // PCIE_RX_CNTL
#define FCH_RCINDXP_REGA0   0xA0 | RCINDXP << 29 // PCIE_LC_CNTL
#define FCH_RCINDXP_REGA1   0xA1 | RCINDXP << 29 // PCIE_LC_TRAINING_CNTL
#define FCH_RCINDXP_REGA2   0xA2 | RCINDXP << 29 //
#define FCH_RCINDXP_REGA4   0xA4 | RCINDXP << 29 //
#define FCH_RCINDXP_REGA5   0xA5 | RCINDXP << 29 // PCIE_LC_STATE0
#define FCH_RCINDXP_REGC0   0xC0 | RCINDXP << 29 //

//
// RCINDX_C Registers
//
#define FCH_RCINDXC_REG02   0x02 | RCINDXC << 29  // PCIE_HW_DEBUG
#define FCH_RCINDXC_REG10   0x10 | RCINDXC << 29  // PCIE_CNTL
#define FCH_RCINDXC_REG40   0x40 | RCINDXC << 29  // PCIE_P_CNTL
#define FCH_RCINDXC_REG65   0x65 | RCINDXC << 29  // PCIE_P_PAD_FORCE_DIS
#define FCH_RCINDXC_REGC0   0xC0 | RCINDXC << 29  // PCIE_STRAP_MISC
#define FCH_RCINDXC_REGC1   0xC1 | RCINDXC << 29  // PCIE_STRAP_MISC2


//
// AXINDC Registers
//
#define FCH_AX_INDXC_REG02  0x02 // PCIEP_HW_DEBUG
#define FCH_AX_INDXC_REG10  0x10
#define FCH_AX_INDXC_REG30  0x30
#define FCH_AX_DATAC_REG34  0x34
#define FCH_AX_INDXP_REG38  0x38
#define FCH_AX_DATAP_REG3C  0x3C
#define FCH_AX_INDXC_REG40  0x40 | AXINDC << 29
#define FCH_AX_INDXC_REGA4  0xA4 | AXINDC << 29

#define FCH_AX_INDXP_REG02  0x02 | AXINDP << 29
#define FCH_AX_INDXP_REGA0  0xA0 | AXINDP << 29
#define FCH_AX_INDXP_REGA4  0xA4 | AXINDP << 29
#define FCH_AX_INDXP_REGB1  0xB1 | AXINDP << 29

#define FCH_AX_CFG_REG68  0x68 | AXCFG << 29
#define FCH_AX_CFG_REG88  0x88 | AXCFG << 29

#define FCH_AB_REG04         0x04
#define FCH_AB_REG40         0x40

//Sata Port Configuration
#define SIX_PORTS       0
#define FOUR_PORTS      1

#define SATA_EFUSE_LOCATION     0x10    // EFUSE bit 133
#define SATA_FIS_BASE_EFUSE_LOC 0x15    // EFUSE bit 169
#define SATA_EFUSE_BIT          0x20    //

#define FCH_SATA_BAR5_REG100     0x0100  // Serial ATA SControl - RW - 32 bits - [Offset: 100h (channel 1) / 180
#define FCH_SATA_BAR5_REG104     0x0104  // Serial ATA Sstatus - RW - 32 bits - [Offset: 104h (channel 1) / 184h (cannel
#define FCH_SATA_BAR5_REG108     0x0108  // Serial ATA Serror - RW - 32 bits - [Offset: 108h (channel 1) / 188h (cannel
#define FCH_SATA_BAR5_REG10C     0x010C  // Serial ATA Sdevice - RW - 32 bits - [Offset: 10Ch (channel 1) / 18Ch (cannel
#define FCH_SATA_BAR5_REG120     0x0120  //
#define FCH_SATA_BAR5_REG128     0x0128  // Port Serial ATA Status
#define FCH_SATA_BAR5_REG12C     0x012C  // Port Serial ATA Control
#define FCH_SATA_BAR5_REG130     0x0130
#define FCH_SATA_BAR5_REG1B0     0x01B0
#define FCH_SATA_BAR5_REG230     0x0230
#define FCH_SATA_BAR5_REG2B0     0x02B0
#define FCH_SATA_BAR5_REG330     0x0330
#define FCH_SATA_BAR5_REG3B0     0x03B0


// USB ports
#define NUM_USB1_PORTS           5
#define NUM_USB2_PORTS           5
#define NUM_USB3_PORTS           4
#define NUM_USB4_PORTS           2
#define NUM_XHC0_PORTS           2
#define NUM_XHC1_PORTS           2


// Chip type definition
#define CHIPTYPE_HUDSON2         (1 << 0)
#define CHIPTYPE_YUBA            (1 << 1)


//
//  USB XHCI Device 0x7812
//  Device 16 (0x10) Func 0/1
//
#define XHCI_EFUSE_LOCATION      0x18        // EFUSE bit 192, 193
#define FCH_XHCI_REG48           0x48        // Port Force Reset - RW (800)
#define FCH_XHCI_REG4C           0x4C        // MSI - RW   (800)
//
//  USB OHCI Device 0x7807
//  Device 18 (0x11)/Device 19 (0x12)/Device 22 (0x16) Func 0
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
#define FCH_OHCI_REG46           0x46        // USB PHY Battery Charger - RW (800)
#define FCH_OHCI_REG48           0x48        // Port Force Reset - RW (800)
#define FCH_OHCI_REG4C           0x4C        // MSI - RW   (800)
#define FCH_OHCI_REG51           0x51
#define FCH_OHCI_REG58           0x58        // Over Current Control - RW
#define FCH_OHCI_REG5C           0x5C        // Over Current Control - RW
#define FCH_OHCI_REG60           0x60        // Serial Bus Release Number - RW
#define FCH_OHCI_REG74           0x74        // Target Timeout Control - RW
#define FCH_OHCI_REG80           0x80        //
#define FCH_OHCI_REGD0           0x0D0       // MSI Control - RW
#define FCH_OHCI_REGD4           0x0D4       // MSI Address - RW
#define FCH_OHCI_REGD8           0x0D8       // MSI Data - RW

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

//
//  USB EHCI Device 0x7808
//  Device 18 (0x11)/Device 19 (0x12)/Device 22 (0x16) Func 2
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
#define FCH_EHCI_REG40           0x40        // Config Timers - RW
#define FCH_EHCI_REG4C           0x4C        // MSI - RW
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
#define FCH_EHCI_BAR_REG02       0x002       // CIVERSION- R
#define FCH_EHCI_BAR_REG04       0x004       // CSPARAMS - R
#define FCH_EHCI_BAR_REG08       0x008       // CCPARAMS - R
#define FCH_EHCI_BAR_REG0C       0x00C       // CSP-PORTROUTE - R

#define FCH_EHCI_BAR_REG20       0x020       // USBCMD - RW - 32 bits
#define FCH_EHCI_BAR_REG24       0x024       // USBSTS - RW - 32 bits
#define FCH_EHCI_BAR_REG28       0x028       // USBINTR -RW - 32 bits
#define FCH_EHCI_BAR_REG2C       0x02C       // FRINDEX -RW - 32 bits
#define FCH_EHCI_BAR_REG30       0x030       // CTRLDSSEGMENT -RW - 32 bits
#define FCH_EHCI_BAR_REG34       0x034       // PERIODICLISTBASE -RW - 32 bits
#define FCH_EHCI_BAR_REG38       0x038       // ASYNCLISTADDR -RW - 32 bits
#define FCH_EHCI_BAR_REG60       0x060       // CONFIGFLAG -RW - 32 bits
#define FCH_EHCI_BAR_REG64       0x064       // PORTSC (1-N_PORTS) -RW - 32 bits
#define FCH_EHCI_BAR_REGA4       0x0A4       // Packet Buffer Threshold Values - RW - 32 bits
#define FCH_EHCI_BAR_REGA8       0x0A8       // USB PHY Status 0 - R
#define FCH_EHCI_BAR_REGAC       0x0AC       // USB PHY Status 1 - R
#define FCH_EHCI_BAR_REGB4       0x0B4       // UTMI Control - RW (800)
#define FCH_EHCI_BAR_REGB8       0x0B8       // Loopback Test
#define FCH_EHCI_BAR_REGBC       0x0BC       // EHCI MISC Control
#define FCH_EHCI_BAR_REGC4       0x0C4       // USB Common PHY Control
#define FCH_EHCI_BAR_REGC8       0x0C8       // EHCI Debug Purpose
#define FCH_EHCI_BAR_REGCC       0x0CC       // Ehci Spare 1 (800) **
#define FCH_EHCI_BAR_REG100      0x100       // USB debug port

//
//  USB EHCI Device 0x7812
//  Device 16 (0x10) Func 0/1
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
//  FCH SATA IDE device
//  Device 20 (0x14) Func 1
//

#define FCH_IDE_REG00            0x00        // Vendor ID
#define FCH_IDE_REG02            0x02        // Device ID
#define FCH_IDE_REG04            0x04        // Command
#define FCH_IDE_REG06            0x06        // Status
#define FCH_IDE_REG08            0x08        // Revision ID/Class Code
#define FCH_IDE_REG09            0x09        // Class Code
#define FCH_IDE_REG2C            0x2C        // Subsystem ID and Subsystem Vendor ID
#define FCH_IDE_REG40            0x40        // Configuration - RW - 32 bits
#define FCH_IDE_REG34            0x34
//
//  Device 20 (0x14) Func 2
//

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
#define FCH_LPC_REG49            0x49        // LPC Sync Timeout Count - RW
#define FCH_LPC_REG4A            0x4A        // IO/Mem Port Decode Enable Register 6- RW
#define FCH_LPC_REG4C            0x4C        // Memory Range Register - RW
#define FCH_LPC_REG50            0x50        // Rom Protect 0 - RW
#define FCH_LPC_REG54            0x54        // Rom Protect 1 - RW
#define FCH_LPC_REG58            0x58        // Rom Protect 2 - RW
#define FCH_LPC_REG5C            0x5C        // Rom Protect 3 - RW
#define FCH_LPC_REG60            0x60        // PCI Memory Start Address of LPC Target Cycles -
#define FCH_LPC_REG62            0x62        // PCI Memory End Address of LPC Target Cycles -
#define FCH_LPC_REG64            0x64        // PCI IO base Address of Wide Generic Port - RW
#define FCH_LPC_REG65            0x65
#define FCH_LPC_REG66            0x66
#define FCH_LPC_REG67            0x67
#define FCH_LPC_REG68            0x68        // LPC ROM Address Range 1 (Start Address) - RW
#define FCH_LPC_REG69            0x69
#define FCH_LPC_REG6A            0x6A        // LPC ROM Address Range 1 (End Address) - RW
#define FCH_LPC_REG6B            0x6B
#define FCH_LPC_REG6C            0x6C        // LPC ROM Address Range 2 (Start Address)- RW
#define FCH_LPC_REG6D            0x6D
#define FCH_LPC_REG6E            0x6E        // LPC ROM Address Range 2 (End Address) - RW
#define FCH_LPC_REG6F            0x6F
#define FCH_LPC_REG71            0x71
#define FCH_LPC_REG72            0x72
#define FCH_LPC_REG73            0x73
#define FCH_LPC_REG74            0x74        // Alternative Wide IO Range Enable- W/R
#define FCH_LPC_REG7C            0x7C        // TPM (trusted plant form module) reg- W/R
#define FCH_LPC_REG9C            0x9C
#define FCH_LPC_REG80            0x80        // MSI Capability Register- R
#define FCH_LPC_REGA0            0x0A0       // SPI base address
#define FCH_LPC_REGA1            0x0A1       // SPI base address
#define FCH_LPC_REGA2            0x0A2       // SPI base address
#define FCH_LPC_REGA3            0x0A3       // SPI base address
#define FCH_LPC_REGA4            0x0A4
#define FCH_LPC_REGBA            0x0BA       // EcControl
#define FCH_LPC_REGBB            0x0BB       // HostControl
#define FCH_LPC_REGCC            0x0CC       // AutoRomCfg
#define FCH_LPC_REGD0            0x0D0

//
//  FCH PCIB  0x780F
//  Device 20 (0x14) Func 4
//
#define FCH_PCIB_REG04            0x04       // Command
#define FCH_PCIB_REG0D            0x0D       // Primary Master Latency Timer
#define FCH_PCIB_REG1B            0x1B       // Secondary Latency Timer
#define FCH_PCIB_REG1C            0x1C       // IO Base
#define FCH_PCIB_REG1D            0x1D       // IO Limit
#define FCH_PCIB_REG40            0x40       // CPCTRL
#define FCH_PCIB_REG42            0x42       // CLKCTRL
#define FCH_PCIB_REG48            0x48       //
#define FCH_PCIB_REG4A            0x4A       // PCICLK Enable Bits
#define FCH_PCIB_REG4B            0x4B       // Misc Control
#define FCH_PCIB_REG4C            0x4C       // AutoClockRun Control
#define FCH_PCIB_REG50            0x50       // Dual Address Cycle Enable and PCIB_CLK_Stop Override
#define FCH_PCIB_REG65            0x65       // Misc Control
#define FCH_PCIB_REG66            0x66       // Misc Control
//
//  FCH GEC 0x14E4 0x1699
//  Device 20 (0x14) Func 6
//
#define FCH_GEC_REG10             0x10       // GEC BAR
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
#define FCH_PMIOA_REG00          0x00        // ISA Decode
#define FCH_PMIOA_REG04          0x04        // ISA Control
#define FCH_PMIOA_REG08          0x08        // PCI Control
#define FCH_PMIOA_REG0C          0x0C        // StpClkSmaf
#define FCH_PMIOA_REG10          0x10        // RetryDetect
#define FCH_PMIOA_REG14          0x14        // StuckDetect
#define FCH_PMIOA_REG20          0x20        // BiosRamEn
#define FCH_PMIOA_REG24          0x24        //  AcpiMmioEn
#define FCH_PMIOA_REG2C          0x2C         // Smbus0En
#define FCH_PMIOA_REG2E          0x2E         // Smbus0Sel
#define FCH_PMIOA_REG34          0x34         // IoApicEn
#define FCH_PMIOA_REG3C          0x3C         // SmartVoltEn
#define FCH_PMIOA_REG40          0x40         // SmartVolt2En
#define FCH_PMIOA_REG44          0x44         // BootTimerEn
#define FCH_PMIOA_REG48          0x48         // WatchDogTimerEn
#define FCH_PMIOA_REG4C          0x4C         // WatchDogTimerConfig
#define FCH_PMIOA_REG50          0x50         // HPETEn
#define FCH_PMIOA_REG54          0x54         // SerialIrqConfig
#define FCH_PMIOA_REG56          0x56         // RtcControl
#define FCH_PMIOA_REG58          0x58         // VRT_T1
#define FCH_PMIOA_REG59          0x59         // VRT_T2
#define FCH_PMIOA_REG5A          0x5A         // IntruderControl
#define FCH_PMIOA_REG5B          0x5B         // RtcShadow
#define FCH_PMIOA_REG5C          0x5C
#define FCH_PMIOA_REG5E          0x5E         // RtcExtIndex
#define FCH_PMIOA_REG5F          0x5F         // RtcExtData
#define FCH_PMIOA_REG60          0x60         // AcpiPm1EvtBlk
#define FCH_PMIOA_REG62          0x62         // AcpiPm1CntBlk
#define FCH_PMIOA_REG64          0x64         // AcpiPmTmrBlk
#define FCH_PMIOA_REG66          0x66         // P_CNTBlk
#define FCH_PMIOA_REG68          0x68         // AcpiGpe0Blk
#define FCH_PMIOA_REG6A          0x6A         // AcpiSmiCmd
#define FCH_PMIOA_REG6C          0x6C         // AcpiPm2CntBlk
#define FCH_PMIOA_REG6E          0x6E         // AcpiPmaCntBlk
#define FCH_PMIOA_REG74          0x74         // AcpiConfig
#define FCH_PMIOA_REG78          0x78         // WakeIoAddr
#define FCH_PMIOA_REG7A          0x7A         // HaltCountEn
#define FCH_PMIOA_REG7C          0x7C         // C1eWrPortAdr
#define FCH_PMIOA_REG7E          0x7E         // CStateEn
#define FCH_PMIOA_REG7F          0x7F         // CStateEn
#define FCH_PMIOA_REG80          0x80         // BreakEvent
#define FCH_PMIOA_REG88          0x88         // CStateControl
#define FCH_PMIOA_REG89          0x89         //
#define FCH_PMIOA_REG8C          0x8C         // StpClkHoldTime
#define FCH_PMIOA_REG8E          0x8E         // PopUpEndTime
#define FCH_PMIOA_REG90          0x90         // C4Control
#define FCH_PMIOA_REG94          0x94         // CStateTiming0
#define FCH_PMIOA_REG96          0x96         //
#define FCH_PMIOA_REG97          0x97         //
#define FCH_PMIOA_REG98          0x98         // CStateTiming1
#define FCH_PMIOA_REG99          0x99         //
#define FCH_PMIOA_REG9B          0x9B         //
#define FCH_PMIOA_REG9C          0x9C         // C2Count
#define FCH_PMIOA_REG9D          0x9D         // C3Count
#define FCH_PMIOA_REGA0          0xA0         // MessageCState
#define FCH_PMIOA_REGA4          0xA4         //
#define FCH_PMIOA_REGA8          0xA8         // TrafficMonitorIdleTime
#define FCH_PMIOA_REGAA          0xAA         // TrafficMonitorIntTime
#define FCH_PMIOA_REGAC          0xAC         // TrafficMonitorTrafficCount
#define FCH_PMIOA_REGAE          0xAE         // TrafficMonitorIntrCount
#define FCH_PMIOA_REGB0          0xB0         // TrafficMonitorTimeTick
#define FCH_PMIOA_REGB4          0xB4         // FidVidControl
#define FCH_PMIOA_REGB7          0xB7         // Tpreset1b
#define FCH_PMIOA_REGB8          0xB8         // TPRESET2
#define FCH_PMIOA_REGBA          0xBA         // S_StateControl
#define FCH_PMIOA_REGBB          0xBB         //
#define FCH_PMIOA_REGBC          0xBC         // ThrottlingControl
#define FCH_PMIOA_REGBE          0xBE         // ResetControl
#define FCH_PMIOA_REGBF          0xBF         // ResetControl
#define FCH_PMIOA_REGC0          0xC0         // S5Status
#define FCH_PMIOA_REGC2          0xC2         // ResetStatus
#define FCH_PMIOA_REGC4          0xC4         // ResetCommand
#define FCH_PMIOA_REGC5          0xC5         // CF9Shadow
#define FCH_PMIOA_REGC6          0xC6         // HTControl
#define FCH_PMIOA_REGC8          0xC8         // Misc
#define FCH_PMIOA_REGCC          0xCC         // IoDrvSth
#define FCH_PMIOA_REGD0          0xD0         // CLKRunEn
#define FCH_PMIOA_REGD2          0xD2         // PmioDebug
#define FCH_PMIOA_REGD6          0xD6         // IMCGating
#define FCH_PMIOA_REGE0          0xE0         // ABRegBar
#define FCH_PMIOA_REGE7          0xE7
#define FCH_PMIOA_REGEA          0xEA         // PcibConfig
#define FCH_PMIOA_REGEC          0xEC         // LpcGating
#define FCH_PMIOA_REGED          0xED         // UsbGating
#define FCH_PMIOA_REGEE          0xEE         // UsbCntrl
#define FCH_PMIOA_REGEF          0xEF         // UsbEnable
#define FCH_PMIOA_REGF0          0xF0         // UsbControl
#define FCH_PMIOA_REGF3          0xF3         // UsbDebug
#define FCH_PMIOA_REGF6          0xF6         // GecEn
#define FCH_PMIOA_REGF8          0xF8         // GecConfig
#define FCH_PMIOA_REGFC          0xFC         // TraceMemoryEn

//
//  FCH MMIO Base (PMIO2)
//    offset : 0x400
//


#define FCH_PMIO2_REG63          0x63        // SampleFreqDiv
#define FCH_PMIO2_REG69          0x69        // Fan0 Speed
#define FCH_PMIO2_REG95          0x95        // Temperature
#define FCH_PMIO2_REGB8          0xB8        // Voltage
#define FCH_PMIO2_REGEA          0xEA        // Hwm_Calibration

#define FCH_PMIO2_REG92          0x92        //

#define FCH_PMIO2_REG          0xFC         // TraceMemoryEn


//
//  FCH MMIO Base (GPIO/IoMux)
//    offset : 0x100/0xD00
//
/*
GPIO from 0 ~ 67, (GEVENT 0-23) 128 ~ 150, 160 ~ 226.
*/
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

#define FCH_GEVENT_REG00         0x60
#define FCH_GEVENT_REG01         0x61
#define FCH_GEVENT_REG02         0x62
#define FCH_GEVENT_REG03         0x63
#define FCH_GEVENT_REG04         0x64
#define FCH_GEVENT_REG05         0x65
#define FCH_GEVENT_REG06         0x66
#define FCH_GEVENT_REG07         0x67
#define FCH_GEVENT_REG08         0x68
#define FCH_GEVENT_REG09         0x69
#define FCH_GEVENT_REG10         0x6A
#define FCH_GEVENT_REG11         0x6B
#define FCH_GEVENT_REG12         0x6C
#define FCH_GEVENT_REG13         0x6D
#define FCH_GEVENT_REG14         0x6E
#define FCH_GEVENT_REG15         0x6F
#define FCH_GEVENT_REG16         0x70
#define FCH_GEVENT_REG17         0x71
#define FCH_GEVENT_REG18         0x72
#define FCH_GEVENT_REG19         0x73
#define FCH_GEVENT_REG20         0x74
#define FCH_GEVENT_REG21         0x75
#define FCH_GEVENT_REG22         0x76
#define FCH_GEVENT_REG23         0x77
// S5-DOMAIN GPIO
#define FCH_GPIO_REG166          0xA6
#define FCH_GPIO_REG167          0xA7
#define FCH_GPIO_REG168          0xA8
#define FCH_GPIO_REG169          0xA9
#define FCH_GPIO_REG170          0xAA

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
#define FCH_MISC_REG40           0x40         //  MiscCntrl      for clock only
#define FCH_MISC_REG41           0x41         //  MiscCntr2
#define FCH_MISC_REG42           0x42         //  MiscCntr3
#define FCH_MISC_REG44           0x44         //  ValueOnPort80
#define FCH_MISC_REG50           0x50         //
/*
FCH_MISC_REG40                   EQU     040h
*/

#define FCH_MISC_REG80           0x80        /**< FCH_MISC_REG80
                                   *   @par
                                   *   StrapStatus [15.0] - FCH chip Strap Status
                                   *    @li <b>0001</b> - Not USED FWH
                                   *    @li <b>0002</b> - Not USED LPC ROM
                                   *    @li <b>0004</b> - EC enabled
                                   *    @li <b>0008</b> - Reserved
                                   *    @li <b>0010</b> - Internal Clock mode
                                   */
#define FCH_MISC_REGB6           0xB6         //

#define  ChipSysNotUseFWHRom    0x0001                            // EcPwm3 pad
#define  ChipSysNotUseLpcRom    0x0002                            // Inverted version from EcPwm2 pad (default - 1)
                                                                  // Note: Both EcPwm3 and EcPwm2 straps pins are used to select boot ROM type.
#define  ChipSysEcEnable        0x0004                            // Enable Embedded Controller (EC)
#define  ChipSysBootFailTmrEn   0x0008                            // Enable Watchdog function
#define  ChipSysIntClkGen       0x0010                            // Select 25Mhz crystal clock or 100Mhz PCI-E clock **

#define FCH_MISC_REG84           0x84         //  StrapOverride
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
#define FCH_SDB_REG00            0x00        //
#define FCH_SDB_REG74            0x74




#define FCH_IOMAP_REG70          0x070   // Nmi_Enable
#define FCH_IOMAP_REG71          0x071   // RtcDataPort
#define FCH_IOMAP_REG72          0x072   // AlternatRtcAddrPort
#define FCH_IOMAP_REG73          0x073   // AlternatRtcDataPort
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
#define FCH_IOMAP_REG40B         0x040B  // DMA1_Extend
#define FCH_IOMAP_REG4D0         0x04D0  // IntrEdgeControl
#define FCH_IOMAP_REG4D6         0x04D6  // DMA2_Extend
#define FCH_IOMAP_REGC00         0x0C00  // Pci_Intr_Index
#define FCH_IOMAP_REGC01         0x0C01  // Pci_Intr_Data
#define FCH_IOMAP_REGC14         0x0C14  // Pci_Error
#define FCH_IOMAP_REGC50         0x0C50  // CMIndex
#define FCH_IOMAP_REGC51         0x0C51  // CMData
#define FCH_IOMAP_REGC52         0x0C52  // GpmPort
#define FCH_IOMAP_REGC6F         0x0C6F  // Isa_Misc
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
#define FCH_SPI_MMIO_REG0C       0x0C        //SPI_Cntrl1 Register
#define FCH_SPI_MMIO_REG1C       0x1C        //

#define FCH_SPI_MODE_FAST        0x7        //
#define FCH_SPI_MODE_NORMAL      0x6        //
#define FCH_SPI_MODE_QUAL_144    0x5        //
#define FCH_SPI_MODE_QUAL_122    0x4        //
#define FCH_SPI_MODE_QUAL_114    0x3        //
#define FCH_SPI_MODE_QUAL_112    0x2        //

#define AMD_NB_REG78            0x78
#define AMD_NB_SCRATCH          AMD_NB_REG78
#define MailBoxPort             0x3E

#define MAX_LT_POLLINGS         0x4000
#define SMI_TIMER_ENABLE        BIT15


#define ACPIMMIO32(x) (*(volatile UINT32*)(UINTN)(x))
#define ACPIMMIO16(x) (*(volatile UINT16*)(UINTN)(x))
#define ACPIMMIO8(x)  (*(volatile UINT8*)(UINTN)(x))

#define XHCI_ACPI_MMIO_AMD_REG00     0x00
#define U3PLL_LOCK                   BIT7
#define U3PLL_RESET                  BIT8
#define U3PHY_RESET                  BIT9
#define U3CORE_RESET                 BIT10
#define XHC0_FUNC_RESET              BIT11
#define XHC1_FUNC_RESET              BIT12

#define XHCI_ACPI_MMIO_AMD_REG04     0x04
#define XHCI_ACPI_MMIO_AMD_REG08     0x08
#define XHCI_ACPI_MMIO_AMD_REG20     0x20
#define XHCI_ACPI_MMIO_AMD_REG90     0x90 // adaptation timer settings
#define XHCI_ACPI_MMIO_AMD_REG98     0x98
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


#define FCH_XHCI_IND_REG00            0x00 //
#define FCH_XHCI_IND_REG04            0x04 //
#define FCH_XHCI_IND_REG88            0x88 //
#define FCH_XHCI_IND_REG94            0x94 // adaptation mode settings
#define FCH_XHCI_IND_REG98            0x98 // CR phase and frequency filter settings
#define FCH_XHCI_IND_REGC8            0xC8 //
#define FCH_XHCI_IND_REGD4            0xD4 // adaptation mode settings
#define FCH_XHCI_IND_REGD8            0xD8 // CR phase and frequency filter settings

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

#define PKT_DATA_REG       ACPI_MMIO_BASE + GFX_DAC_BASE + 0x00
#define PKT_LEN_REG        ACPI_MMIO_BASE + GFX_DAC_BASE + 0x14
#define PKT_CTRL_REG       ACPI_MMIO_BASE + GFX_DAC_BASE + 0x15
#define EFUS_DAC_ADJUSTMENT_CONTROL    0x850A8ul
#define BGADJ                             0x1F
#define DACADJ                            0x1B
#define EFUS_DAC_ADJUSTMENT_CONTROL_DATA  (BGADJ + (DACADJ << 8) + BIT16 )

#ifndef FCH_DEADLOOP
  #define FCH_DEADLOOP()    { volatile UINTN __i; __i = 1; while (__i); }
#endif


