/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU BrandId related functions and structures.
 *
 * Contains code that provides CPU BrandId information
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuEarlyInit.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_CPUF15BRANDID_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define NAME_STRING_ADDRESS_PORT 0x194
#define NAME_STRING_DATA_PORT 0x198

extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// A structure containing brand string
typedef struct {
  CONST CHAR8 *Stringstart;  ///< The literal string
} CPU_F15_EXCEPTION_BRAND;

/// FAM15_BRAND_STRING_MSR
typedef struct _PROCESSOR_NAME_STRING {
  UINT32 lo;                 ///< lower 32-bits of 64-bit value
  UINT32 hi;                 ///< highest 32-bits of 64-bit value
} PROCESSOR_NAME_STRING;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
STATIC
IsException (
     OUT   UINT32             *ExceptionId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
F15SetBrandIdRegistersAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
// This is an example, need to be updated once Processor Revision Guide define brand string exception
// Brand string is always 48 bytes
CONST CHAR8 ROMDATA str_Exception_0[48] = "AMD Phenom(tm) Octal-Core";
CONST CHAR8 ROMDATA str_Unprogrammed_Sample[48] = "AMD Unprogrammed Engineering Sample";
/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
CONST CPU_F15_EXCEPTION_BRAND ROMDATA CpuF15ExceptionBrandIdString[] =
{
  {str_Exception_0}
};

/*---------------------------------------------------------------------------------------*/
/**
 * Set the Processor Name String register based on F5x194/198
 *
 * This function copies F5x198_x[B:0] to MSR_C001_00[35:30]
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
F15SetBrandIdRegistersAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32 PciData;
  UINT32 ExceptionId;
  UINT32 MsrIndex;
  UINT64 MsrData;
  UINT64 *MsrNameStringPtrPtr;
  PCI_ADDR PciAddress;

  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    if (IsException (&ExceptionId, StdHeader)) {
      ASSERT (ExceptionId < (sizeof (CpuF15ExceptionBrandIdString) / sizeof (CpuF15ExceptionBrandIdString[0])));

      MsrNameStringPtrPtr = (UINT64 *) CpuF15ExceptionBrandIdString[ExceptionId].Stringstart;
    } else {
      OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
      PciAddress.Address.Function = FUNC_5;
      PciAddress.Address.Register = NAME_STRING_ADDRESS_PORT;
      // check if D18F5x198_x0 is 00000000h.
      PciData = 0;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
      PciAddress.Address.Register = NAME_STRING_DATA_PORT;
      LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
      if (PciData != 0) {
        for (MsrIndex = 0; MsrIndex <= (MSR_CPUID_NAME_STRING5 - MSR_CPUID_NAME_STRING0); MsrIndex++) {
          PciAddress.Address.Register = NAME_STRING_ADDRESS_PORT;
          PciData = MsrIndex * 2;
          LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
          PciAddress.Address.Register = NAME_STRING_DATA_PORT;
          LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
          ((PROCESSOR_NAME_STRING *) (&MsrData))->lo = PciData;

          PciAddress.Address.Register = NAME_STRING_ADDRESS_PORT;
          PciData = (MsrIndex * 2) + 1;
          LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
          PciAddress.Address.Register = NAME_STRING_DATA_PORT;
          LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
          ((PROCESSOR_NAME_STRING *) (&MsrData))->hi = PciData;

          LibAmdMsrWrite ((MsrIndex + MSR_CPUID_NAME_STRING0), &MsrData, StdHeader);
        }
        return;
      } else {
        // It is unprogrammed (unfused) parts and use a name string of "AMD Unprogrammed Engineering Sample"
        MsrNameStringPtrPtr = (UINT64 *) str_Unprogrammed_Sample;
      }
    }
    // Put values into name MSRs,  Always write the full 48 bytes
    for (MsrIndex = MSR_CPUID_NAME_STRING0; MsrIndex <= MSR_CPUID_NAME_STRING5; MsrIndex++) {
      LibAmdMsrWrite (MsrIndex, MsrNameStringPtrPtr, StdHeader);
      MsrNameStringPtrPtr++;
    }
  }
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Check if it's an exception
 *
 * For family 15h, brand string is obtained from F5x198_x[B:0], but there may be exceptions.
 * This function checks if it's an exception.
 *
 *  @param[out]  ExceptionId         Id of exception
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 *  @retval      TRUE                It's an exception
 *  @retval      FALSE               It's NOT an exception
 */
BOOLEAN
STATIC
IsException (
     OUT   UINT32                 *ExceptionId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  // This function will be updated, once Processor Revision Guide defines Fam15 brand string exception
  *ExceptionId = 0xFFFF;

  return FALSE;
}

