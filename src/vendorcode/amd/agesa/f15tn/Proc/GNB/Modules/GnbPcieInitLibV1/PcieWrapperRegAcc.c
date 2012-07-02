/* $NoKeywords:$ */
/**
 * @file
 *
 * Supporting services to access PCIe wrapper/core/PIF/PHY indirect register spaces
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

#include  "AGESA.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEWRAPPERREGACC_FILECODE
/*----------------------------------------------------------------------------------------*/
/**
 * Read PCIe register value.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to Wrapper descriptor
 * @param[in]  Address     Register address
 * @param[in]  Pcie        Pointer to global PCIe configuration
 * @retval                 Register Value
 */
UINT32
PcieRegisterRead (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if ((Wrapper->Features.AccessEncoding == 1) && ((Address & 0xff0000) == 0x010000)) {
    Address = (Address & 0xffff) | 0x1400000 | ((Address >> 8) & 0xF0000);
  }
  return PcieSiliconRegisterRead (PcieConfigGetParentSilicon (Wrapper), Address, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read PCIe register value.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Silicon     Pointer to silicon descriptor
 * @param[in]  Address     Register address
 * @param[in]  Pcie        Pointer to global PCIe configuration
 * @retval                 Register Value
 */

UINT32
PcieSiliconRegisterRead (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32              Value;
  GnbLibPciWrite (Silicon->Address.AddressValue | 0xE0, AccessWidth32, &Address, GnbLibGetHeader (Pcie));
  GnbLibPciRead (Silicon->Address.AddressValue | 0xE4, AccessWidth32, &Value, GnbLibGetHeader (Pcie));
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCIe register value.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]   Wrapper     Pointer to wrapper descriptor
 * @param[in]   Address     Register address
 * @param[in]   Value       New register value
 * @param[in]   S3Save      Save register for S3 (True/False)
 * @param[in]   Pcie        Pointer to global PCIe configuration
 */
VOID
PcieRegisterWrite (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if ((Wrapper->Features.AccessEncoding == 1) && ((Address & 0xff0000) == 0x010000)) {
    Address = (Address & 0xffff) | 0x1400000 | ((Address >> 8) & 0xF0000);
  }
  PcieSiliconRegisterWrite (
     PcieConfigGetParentSilicon (Wrapper),
     Address,
     Value,
     S3Save,
     Pcie
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCIe register value.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]   Silicon     Pointer to silicon descriptor
 * @param[in]   Address     Register address
 * @param[in]   Value       New register value
 * @param[in]   S3Save      Save register for S3 (True/False)
 * @param[in]   Pcie        Pointer to global PCIe configuration
 */
VOID
PcieSiliconRegisterWrite (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  IDS_HDT_CONSOLE (PCIE_HOSTREG_TRACE, "    *WR %s (%d:%d:%d):0x%08x = 0x%08x\n",
    PcieFmDebugGetHostRegAddressSpaceString (Silicon, (UINT16) (Address >> 16)),
    Silicon->Address.Address.Bus,
    Silicon->Address.Address.Device,
    Silicon->Address.Address.Function,
    Address,
    Value
    );
  GnbLibPciWrite (Silicon->Address.AddressValue | 0xE0, S3Save ? AccessS3SaveWidth32 : AccessWidth32, &Address, GnbLibGetHeader (Pcie));
  GnbLibPciWrite (Silicon->Address.AddressValue | 0xE4, S3Save ? AccessS3SaveWidth32 : AccessWidth32, &Value, GnbLibGetHeader (Pcie));
}
/*----------------------------------------------------------------------------------------*/
/**
 * Read PCIe register field.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper descriptor
 * @param[in]  Address     Register address
 * @param[in]  FieldOffset Field offset
 * @param[in]  FieldWidth  Field width
 * @param[in]  Pcie        Pointer to global PCIe configuration
 * @retval                 Register field value
 */

UINT32
PcieRegisterReadField (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  Value;
  Value = PcieRegisterRead (Wrapper, Address, Pcie);
  Value = (Value >> FieldOffset) & (~(0xFFFFFFFF << FieldWidth));
  return  Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCIe register field.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper descriptor
 * @param[in]  Address     Register address
 * @param[in]  FieldOffset Field offset
 * @param[in]  FieldWidth  Field width
 * @param[in]  Value       Value to write
 * @param[in]  S3Save      Save register for S3 (True/False)
 * @param[in]  Pcie        Pointer to global PCIe configuration
 */


VOID
PcieRegisterWriteField (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  TempValue;
  UINT32  Mask;
  TempValue = PcieRegisterRead (Wrapper, Address, Pcie);
  Mask = (~(0xFFFFFFFF << FieldWidth));
  Value &= Mask;
  TempValue &= (~(Mask << FieldOffset));
  PcieRegisterWrite (Wrapper, Address, TempValue | (Value << FieldOffset), S3Save, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write PCIe register.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper descriptor
 * @param[in]  Address     Register address
 * @param[in]  AndMask     Value & (~AndMask)
 * @param[in]  OrMask      Value | OrMask
 * @param[in]  S3Save      Save register for S3 (True/False)
 * @param[in]  Pcie        Pointer to global PCIe configuration
 */

VOID
PcieRegisterRMW (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PcieSiliconRegisterRMW (
    PcieConfigGetParentSilicon (Wrapper),
    Address,
    AndMask,
    OrMask,
    S3Save,
    Pcie
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write PCIe register.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Silicon     Pointer to silicon descriptor
 * @param[in]  Address     Register address
 * @param[in]  AndMask     Value & (~AndMask)
 * @param[in]  OrMask      Value | OrMask
 * @param[in]  S3Save      Save register for S3 (True/False)
 * @param[in]  Pcie        Pointer to global PCIe configuration
 */

VOID
PcieSiliconRegisterRMW (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  Value;
  Value = PcieSiliconRegisterRead (Silicon, Address, Pcie);
  Value = (Value & (~AndMask)) | OrMask;
  PcieSiliconRegisterWrite (Silicon, Address, Value, S3Save, Pcie);
}