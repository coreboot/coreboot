/**
 * @file
 *
 * PCIe link ASPM Black List
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
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
#include  "AGESA.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "PcieAspmBlackList.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEASPMBLACKLIST_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


UINT16  AspmBrDeviceTable[] = {
 0x1002, 0x9441, (UINT16) ~(AspmL1 | AspmL0s),
 0x10B5, 0xFFFF, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x0402, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x0193, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x0422, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x0292, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x00F9, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x0141, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x0092, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01D0, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01D1, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01D2, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01D3, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01D5, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01D7, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01D8, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01DC, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01DE, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x01DF, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x016A, (UINT16) ~(AspmL1 | AspmL0s),
 0x10DE, 0x0392, (UINT16) ~(AspmL1 | AspmL0s),
 0x168C, 0xFFFF, (UINT16) ~(AspmL0s),
 0x1B4B, 0x91A3, (UINT16) ~(AspmL0s),
 0x1B4B, 0x9123, (UINT16) ~(AspmL0s),
 0x1969, 0x1083, (UINT16) ~(AspmL0s)
};

/*----------------------------------------------------------------------------------------*/
/**
 * Pcie ASPM Black List
 *
 *
 *
 * @param[in] LinkAsmp            PCie ASPM black list
 * @param[in] StdHeader           Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
PcieAspmBlackListFeature (
  IN       PCIe_LINK_ASPM         *LinkAsmp,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32  UpstreamDeviceId;
  UINT32  DownstreamDeviceId;
  UINTN   i;
  UINT32  DeviceId;
  UINT32  VendorId;

  GnbLibPciRead (LinkAsmp->UpstreamPort.AddressValue, AccessWidth32, &UpstreamDeviceId, StdHeader);
  GnbLibPciRead (LinkAsmp->DownstreamPort.AddressValue, AccessWidth32, &DownstreamDeviceId, StdHeader);
  for (i = 0; i < (sizeof (AspmBrDeviceTable) / sizeof (UINT16)); i = i + 3) {
    VendorId = AspmBrDeviceTable[i];
    DeviceId = AspmBrDeviceTable[i + 1];
    if (VendorId == (UINT16)UpstreamDeviceId || VendorId == (UINT16)DownstreamDeviceId ) {
      if (DeviceId == 0xFFFF || DeviceId == (UpstreamDeviceId >> 16) || DeviceId == (DownstreamDeviceId >> 16)) {
        LinkAsmp->UpstreamAspm &= AspmBrDeviceTable[i + 2];
        LinkAsmp->DownstreamAspm &= AspmBrDeviceTable[i + 2];
      }
    }
  }
  if ((UINT16)UpstreamDeviceId == 0x168c) {
    LinkAsmp->UpstreamAspm = LinkAsmp->RequestedAspm & AspmL1;
    LinkAsmp->DownstreamAspm = LinkAsmp->UpstreamAspm;
    GnbLibPciRMW (LinkAsmp->UpstreamPort.AddressValue | 0x70C, AccessS3SaveWidth32, 0x0, 0x0F003F01, StdHeader);

    DeviceId = UpstreamDeviceId >> 16;
    if ((DeviceId == 0x002C) || (DeviceId == 0x002B) || (DeviceId == 0x002E)) {
      LinkAsmp->UpstreamAspm = LinkAsmp->RequestedAspm & AspmL0sL1;
      LinkAsmp->DownstreamAspm = LinkAsmp->UpstreamAspm & AspmL1;
    }
  }
  if (UpstreamDeviceId == 0x10831969) {
    GnbLibPciRMW (LinkAsmp->UpstreamPort.AddressValue | 0x12F8, AccessS3SaveWidth32, 0xFFF7F7FF, 0, StdHeader);
  }

  return AGESA_SUCCESS;
}
