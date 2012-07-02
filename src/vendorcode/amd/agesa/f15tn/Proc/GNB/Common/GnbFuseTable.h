/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics controller BIF straps control services.
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


#ifndef _GNBFUSETABLE_H_
#define _GNBFUSETABLE_H_

#pragma pack (push, 1)

#define PP_FUSE_MAX_NUM_DPM_STATE 5
#define PP_FUSE_MAX_NUM_SW_STATE  6

/// Fuse definition structure
typedef struct  {
  UINT8                       PPlayTableRev;              ///< PP table revision
  UINT8                       SclkDpmValid[6];            ///< Valid DPM states
  UINT8                       SclkDpmDid[6];              ///< Sclk DPM DID
  UINT8                       SclkDpmVid[6];              ///< Sclk DPM VID
  UINT8                       SclkDpmCac[5];              ///< Sclk DPM Cac
  UINT8                       PolicyFlags[6];             ///< State policy flags
  UINT8                       PolicyLabel[6];             ///< State policy label
  UINT8                       VclkDid[4];                 ///< VCLK DID
  UINT8                       DclkDid[4];                 ///< DCLK DID
  UINT8                       SclkThermDid;               ///< Thermal SCLK
  UINT8                       VclkDclkSel[6];             ///< Vclk/Dclk selector
  UINT8                       LclkDpmValid[4];            ///< Valid Lclk DPM states
  UINT8                       LclkDpmDid[4];              ///< Lclk DPM DID
  UINT8                       LclkDpmVid[4];              ///< Lclk DPM VID
  UINT8                       DisplclkDid[4];             ///< Displclk DID
  UINT8                       PcieGen2Vid;                ///< Pcie Gen 2 VID
  UINT8                       MainPllId;                  ///< Main PLL Id from fuses
  UINT8                       WrCkDid;                    ///< WRCK SMU clock Divisor
  UINT8                       SclkVid[4];                 ///< Sclk VID
  UINT8                       GpuBoostCap;                ///< GPU boost cap
  UINT16                      SclkDpmTdpLimit[6];         ///< Sclk DPM TDP limit
  UINT16                      SclkDpmTdpLimitPG;          ///< TDP limit PG
  UINT32                      SclkDpmBoostMargin;         ///< Boost margin
  UINT32                      SclkDpmThrottleMargin;      ///< Throttle margin
  BOOLEAN                     VceSateTableSupport;        ///< Support VCE in PP table
  UINT8                       VceFlags[4];                ///< VCE Flags
  UINT8                       VceMclk[4];                 ///< MCLK for VCE
  UINT8                       VceReqSclkSel[4];           ///< SCLK selector for VCE
  UINT8                       EclkDid[4];                 ///< Eclk DID
} PP_FUSE_ARRAY;

#pragma pack (pop)

#endif

