/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe early post initialization.
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
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbBapmCoeffCalcTN.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbRegistersTN.h"
#include  "GnbInitTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GNBBAPMCOEFFCALC_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


#define GnbFpLibGetExp(V)  ((INT32) ((*((UINT64*) &Value) >> 52) & 0x7FF) - (1023 + 52))
#define GnbFpLibGetMnts(V) (INT64) ((*((UINT64*) &Value) & ((1ull << 52) - 1)) | (1ull << 52))

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

INT32 _fltused = 0;

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate power of
 *
 *
 * @param[in]  Value    value
 * @param[in]  Pow      power
 * @retval  Value^Pow
 */

STATIC DOUBLE
GnbBapmPowerOf (
  IN      DOUBLE    Value,
  IN      UINTN     Pow
  )
{
  DOUBLE Result;
  Result = Value;
  while ( --Pow > 0) {
    Result *= Value;
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Decode R from fuse
 *
 *
 * @param[in]  FuseR    R fused value
 * @retval  R
 */
STATIC DOUBLE
GnbBapmDecodeR (
  IN      UINT32  FuseR
  )
{
  DOUBLE Value;
  Value = ((DOUBLE) (FuseR & 0x1ff)) / (2 << (8 - 1));
  Value = GnbBapmPowerOf (Value, 4);
  return ((FuseR & 0x200) != 0) ? (-1) * Value : Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Decode Tau from fuse
 *
 *
 * @param[in]  FuseTau    Tau fused value
 * @retval  Tau
 */
STATIC DOUBLE
GnbBapmDecodeTau (
  IN      UINT32  FuseTau
  )
{
  DOUBLE  Value;
  Value = FuseTau;
  Value = GnbBapmPowerOf (Value / (2 << (9 - 1)), 16);
  return  Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Calaculate X
 *
 *
 * @param[in]  Ts         Samplig rate
 * @param[in]  Tau        Tau value
 * @param[in]  R          R value
 * @retval  X
 */
STATIC DOUBLE
GnbBapmCalculateX (
  IN      DOUBLE  Ts,
  IN      DOUBLE  Tau,
  IN      DOUBLE  R
  )
{
  //X=(R*Ts)/(2*Tau+Ts);
  DOUBLE Result;
  Result = (R * Ts) / (2 * Tau + Ts);
  return (Result * GnbBapmPowerOf (2, 36)) + 0.5;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Calaculate Y
 *
 *
 * @param[in]  Ts         Samplig rate
 * @param[in]  Tau        Tau value
 * @retval  Y
 */
STATIC DOUBLE
GnbBapmCalculateY (
  IN      DOUBLE  Ts,
  IN      DOUBLE  Tau
  )
{
  //Y=(2*Tau-Ts)/(2*Tau+Ts);
  DOUBLE Result;
  Result = (2 * Tau - Ts) / (2 * Tau + Ts);
  return (Result *  GnbBapmPowerOf (2, 32)) + 0.5;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set X & Y value
 *
 *
 * @param[in]  X          X value
 * @param[in]  Y          Y value
 * @param[in]  AddrOffset Offset of address
 * @param[in]  StdHeader  Standard configuration header
 */
STATIC VOID
GnbBapmSetYAndX (
  IN       INT32              X,
  IN       INT32              Y,
  IN       UINT32             AddrOffset,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  GnbRegisterWriteTN (
    D0F0xBC_x1F480_TYPE,
    D0F0xBC_x1F480_ADDRESS + AddrOffset,
    &X,
    0,
    StdHeader
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "X: 0x%08x\n", X);
  GnbRegisterWriteTN (
    D0F0xBC_x1F480_TYPE,
    D0F0xBC_x1F480_ADDRESS + AddrOffset + 4,
    &Y,
    0,
    StdHeader
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "Y: 0x%08x\n", Y);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Extract INt32 from DOUBLE
 *
 *
 *
 * @param[in]  Value  Double value
 * @retval     int32
 */
INT32
GnbFpLibDoubleToInt32 (
  IN      DOUBLE  Value
  )
{
  INT64   Mantissa;
  INT32   Exponent;
  Mantissa = GnbFpLibGetMnts (Value);
  Exponent = GnbFpLibGetExp (Value);
  if (Exponent < -64) {
    Mantissa = 0;
  } else if (Exponent < 0) {
    Mantissa >>=  - Exponent;
  } else {
    Mantissa <<= Exponent;
  }
  return (INT32) ((Value < 0) ? - Mantissa :  Mantissa);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Calcuate BAPM coefficient
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

VOID
GnbBapmCalculateCoeffsTN (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32  RFuse1;
  UINT32  TauFuse1;
  UINT32  Index;
  DOUBLE  R;
  DOUBLE  Tau;
  DOUBLE  Ts;
  INT32   X;
  INT32   Y;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBapmCalculateCoeffsTN Enter\n");

  LibAmdFinit ();

  Ts = ((DOUBLE) (D0F0xBC_x1F468_TimerPeriod_Value * D0F0xBC_x1F46C_BapmPeriod_Value) / 100) / 1000000;

  for (Index = 0; Index < 15; Index++) {
    GnbRegisterReadTN (
      TYPE_D0F0xBC ,
      0xe01040c4  + Index * 4,
      &RFuse1,
      0,
      StdHeader
      );
    GnbRegisterReadTN (
      TYPE_D0F0xBC ,
      0xe01040c4  + (Index + 15) * 4,
      &TauFuse1,
      0,
      StdHeader
      );

    R = GnbBapmDecodeR (RFuse1 & 0x3FF);
    Tau = GnbBapmDecodeTau (TauFuse1 & 0x3FF);

    X = GnbFpLibDoubleToInt32 (GnbBapmCalculateX (Ts, Tau, R));
    Y = GnbFpLibDoubleToInt32 (GnbBapmCalculateY (Ts, Tau));
    GnbBapmSetYAndX (X, Y, Index * 2 * 4, StdHeader);

    R = GnbBapmDecodeR ((RFuse1 >> 10) & 0x3FF);
    Tau = GnbBapmDecodeTau ((TauFuse1 >> 10) & 0x3FF);

    X = GnbFpLibDoubleToInt32 (GnbBapmCalculateX (Ts, Tau, R));
    Y = GnbFpLibDoubleToInt32 (GnbBapmCalculateY (Ts, Tau));

    GnbBapmSetYAndX (X, Y, (Index * 2 + 30) * 4 , StdHeader);

    R = GnbBapmDecodeR ((RFuse1 >> 20) & 0x3FF);
    Tau = GnbBapmDecodeTau ((TauFuse1 >> 20) & 0x3FF);

    X = GnbFpLibDoubleToInt32 (GnbBapmCalculateX (Ts, Tau, R));
    Y = GnbFpLibDoubleToInt32 (GnbBapmCalculateY (Ts, Tau));

    GnbBapmSetYAndX (X, Y, (Index * 2 + 60) * 4 , StdHeader);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBapmCalculateCoeffsTN Exit\n");
}



