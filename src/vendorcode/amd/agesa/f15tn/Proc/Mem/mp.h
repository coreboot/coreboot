/* $NoKeywords:$ */
/**
 * @file
 *
 * mp.h
 *
 * Platform Specific common header file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
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

#ifndef _MP_H_
#define _MP_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
/// Type of an entry for Dram Term table
typedef struct {
  UINT32 Speed; ///< BitMap for the supported speed
  UINT8  Dimms; ///< BitMap for supported number of dimm
  UINT8  QR_Dimms; ///< BitMap for supported number of QR dimm
  UINT8  DramTerm; ///< DramTerm value
  UINT8  QR_DramTerm; ///< DramTerm value for QR
  UINT8  DynamicDramTerm; ///< Dynamic DramTerm
} DRAM_TERM_ENTRY;

/// Type of an entry for POR speed limit table
typedef struct {
  UINT16   DIMMRankType;  ///< Bitmap of Ranks
  UINT8    Dimms;  ///< Number of dimm
  UINT16    SpeedLimit_1_5V; ///< POR speed limit for 1.5V
  UINT16    SpeedLimit_1_35V; ///< POR speed limit for 1.35V
  UINT16    SpeedLimit_1_25V; ///< POR speed limit for 1.25V
} POR_SPEED_LIMIT;

/// UDIMM&RDIMM Max. Frequency
typedef union {
  struct {                       ///< PSCFG_MAXFREQ_ENTRY
    UINT8 DimmPerCh;             ///< Dimm slot per chanel
    UINT16 Dimms:4;              ///< Number of Dimms on a channel
    UINT16 SR:4;                 ///< Number of single-rank Dimm
    UINT16 DR:4;                 ///< Number of dual-rank Dimm
    UINT16 QR:4;                 ///< Number of quad-rank Dimm
    UINT16 Speed1_5V;            ///< Speed limit with voltage 1.5V
    UINT16 Speed1_35V;           ///< Speed limit with voltage 1.35V
    UINT16 Speed1_25V;           ///< Speed limit with voltage 1.25V
  } _MAXFREQ_ENTRY;
  struct {
    UINT8 DimmSlotPerCh;
    UINT16 CDN;                  ///< Condition
    UINT16 Speed[3];             ///< Speed limit
  } MAXFREQ_ENTRY;
} PSCFG_MAXFREQ_ENTRY;

///  LRDIMM Max. Frequency
typedef union {
  struct {                       ///< PSCFG_LR_MAXFREQ_ENTRY
    UINT8 DimmPerCh;             ///< Dimm slot per chanel
    UINT16 Dimms:4;              ///< Number of Dimms on a channel
    UINT16 LR:12;                ///< Number of LR-DIMM
    UINT16 Speed1_5V;            ///< Speed limit with voltage 1.5V
    UINT16 Speed1_35V;           ///< Speed limit with voltage 1.35V
    UINT16 Speed1_25V;           ///< Speed limit with voltage 1.25V
  } _LR_MAXFREQ_ENTRY;
  struct {
    UINT8 DimmSlotPerCh;
    UINT16 CDN;
    UINT16 Speed[3];
  } LR_MAXFREQ_ENTRY;
} PSCFG_LR_MAXFREQ_ENTRY;

/// UDIMM&RDIMM RttNom and RttWr
typedef struct {
  UINT64 DimmPerCh:8;          ///< Dimm slot per chanel
  UINT64 DDRrate:32;           ///< Bitmap of DDR rate
  UINT64 VDDIO:4;              ///< Bitmap of VDDIO
  UINT64 Dimm0:4;              ///< Bitmap of rank type of Dimm0
  UINT64 Dimm1:4;              ///< Bitmap of rank type of Dimm1
  UINT64 Dimm2:4;              ///< Bitmap of rank type of Dimm2
  UINT64 Dimm:4;               ///< Bitmap of rank type of Dimm
  UINT64 Rank:4;               ///< Bitmap of rank
  UINT8 RttNom:3;              ///< Dram term
  UINT8 RttWr:5;               ///< Dynamic dram term
} PSCFG_RTT_ENTRY;

/// LRDIMM RttNom and RttWr
typedef struct {
  UINT64 DimmPerCh:8;          ///< Dimm slot per chanel
  UINT64 DDRrate:32;           ///< Bitmap of DDR rate
  UINT64 VDDIO:4;              ///< Bitmap of VDDIO
  UINT64 Dimm0:4;              ///< Dimm0 population
  UINT64 Dimm1:4;              ///< Dimm1 population
  UINT64 Dimm2:12;             ///< Dimm2 population
  UINT8 RttNom:3;              ///< Dram term
  UINT8 RttWr:5;               ///< Dynamic dram term
} PSCFG_LR_RTT_ENTRY;

/// UDIMM&RDIMM&LRDIMM ODT pattern OF 1 DPC
typedef struct {
  UINT16 Dimm0;                ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  UINT32 RdODTCSHigh;          ///< RdODTCSHigh
  UINT32 RdODTCSLow;           ///< RdODTCSLow
  UINT32 WrODTCSHigh;          ///< WrODTCSHigh
  UINT32 WrODTCSLow;           ///< WrODTCSLow
} PSCFG_1D_ODTPAT_ENTRY;

/// UDIMM&RDIMM&LRDIMM ODT pattern OF 2 DPC
typedef struct {
  UINT16 Dimm0:4;              ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  UINT16 Dimm1:12;             ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  UINT32 RdODTCSHigh;          ///< RdODTCSHigh
  UINT32 RdODTCSLow;           ///< RdODTCSLow
  UINT32 WrODTCSHigh;          ///< WrODTCSHigh
  UINT32 WrODTCSLow;           ///< WrODTCSLow
} PSCFG_2D_ODTPAT_ENTRY;

/// UDIMM&RDIMM&LRDIMM ODT pattern OF 3 DPC
typedef struct {
  UINT16 Dimm0:4;              ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  UINT16 Dimm1:4;              ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  UINT16 Dimm2:8;              ///< Bitmap of dimm2 rank type or dimm2 population of LRDIMM
  UINT32 RdODTCSHigh;          ///< RdODTCSHigh
  UINT32 RdODTCSLow;           ///< RdODTCSLow
  UINT32 WrODTCSHigh;          ///< WrODTCSHigh
  UINT32 WrODTCSLow;           ///< WrODTCSLow
} PSCFG_3D_ODTPAT_ENTRY;

/// UDIMM&RDIMM&LRDIMM SlowMode, AddrTmgCtl and ODC
typedef struct {
  UINT64 DimmPerCh:8;          ///< Dimm slot per channel
  UINT64 DDRrate:32;           ///< Bitmap of DDR rate
  UINT64 VDDIO:4;              ///< Bitmap of VDDIO
  UINT64 Dimm0:4;              ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  UINT64 Dimm1:4;              ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  UINT64 Dimm2:11;             ///< Bitmap of dimm2 rank type or dimm2 population of LRDIMM
  UINT64 SlowMode:1;           ///< SlowMode
  UINT32 AddTmgCtl;            ///< AddTmgCtl
  UINT32 ODC;                  ///< ODC
} PSCFG_SAO_ENTRY;

/// UDIMM&RDIMM&LRDIMM 2D training config entry
typedef struct {
  UINT64 DimmPerCh:8;          ///< Dimm per channel
  UINT64 DDRrate:32;           ///< Bitmap of DDR rate
  UINT64 VDDIO:4;              ///< Bitmap of VDDIO
  UINT64 Dimm0:4;              ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  UINT64 Dimm1:4;              ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  UINT64 Dimm2:11;             ///< Bitmap of dimm2 rank type or dimm2 population of LRDIMM
  UINT64 Enable2D:1;           ///< SlowMode
} PSCFG_S2D_ENTRY;

/// UDIMM&RDIMM MR0[WR]
typedef struct {
  UINT8 Timing;                ///< Fn2_22C_dct[1:0][Twr]
  UINT8 Value;                 ///< MR0[WR] : bit0 - bit2 available
} PSCFG_MR0WR_ENTRY;

/// UDIMM&RDIMM MR0[CL]
typedef struct {
  UINT8 Timing;                ///< Fn2_200_dct[1:0][Tcl]
  UINT8 Value:3;               ///< MR0[CL] : bit0 - bit2 CL[3:1]
  UINT8 Value1:5;              ///< MR0[CL] : bit3 CL[0]
} PSCFG_MR0CL_ENTRY;

/// UDIMM&RDIMM MR2[IBT]
typedef struct {
  UINT64 DimmPerCh:4;          ///< Dimm per channel
  UINT64 DDRrate:32;           ///< Bitmap of DDR rate
  UINT64 VDDIO:4;              ///< Bitmap of VDDIO
  UINT64 Dimm0:4;              ///< Bitmap of dimm0 rank type
  UINT64 Dimm1:4;              ///< Bitmap of dimm1 rank type
  UINT64 Dimm2:4;              ///< Bitmap of dimm2 rank type
  UINT64 Dimm:4;               ///< Bitmap of rank type of Dimm
  UINT64 NumOfReg:4;           ///< Number of registers
  UINT64 IBT:4;                 ///< MR2[IBT] value
} PSCFG_MR2IBT_ENTRY;

/// UDIMM&RDIMM&LRDIMM Operating Speed
typedef struct {
  UINT32 DDRrate;              ///< Bitmap of DDR rate
  UINT8 OPSPD;                 ///< RC10[OperatingSpeed]
} PSCFG_OPSPD_ENTRY;

/// LRDIMM IBT
typedef struct {
  UINT64 DimmPerCh:4;          ///< Dimm per channel
  UINT64 DDRrate:32;           ///< Bitmap of DDR rate
  UINT64 VDDIO:4;              ///< Bitmap of VDDIO
  UINT64 Dimm0:4;              ///< Dimm0 population
  UINT64 Dimm1:4;              ///< Dimm1 population
  UINT64 Dimm2:4;              ///< Dimm2 population
  UINT64 F0RC8:3;              ///< F0RC8
  UINT64 F1RC0:3;              ///< F1RC0
  UINT64 F1RC1:3;              ///< F1RC1
  UINT64 F1RC2:3;              ///< F1RC2
} PSCFG_L_IBT_ENTRY;

/// LRDIMM F0RC13[NumPhysicalRanks]
typedef struct {
  UINT8 NumRanks:3;            ///< NumRanks
  UINT8 NumPhyRanks:5;         ///< NumPhyRanks
} PSCFG_L_NPR_ENTRY;

/// LRDIMM F0RC13[NumLogicalRanks]
typedef struct {
  UINT16 NumPhyRanks:3;        ///< NumPhyRanks
  UINT16 DramCap:4;            ///< DramCap
  UINT16 NumDimmSlot:9;        ///< NumDimmSlot
  UINT8 NumLogRanks;           ///< NumLogRanks
} PSCFG_L_NLR_ENTRY;

/// UDIMM&RDIMM&LRDIMM pass1 seed entry
typedef struct {
  UINT8 DimmPerCh;             ///< Dimm per channel
  UINT8 Channel;               ///< Channel#
  UINT16 SeedVal;              ///< Seed value
} PSCFG_SEED_ENTRY;

/// Platform specific configuration types
typedef enum {
  PSCFG_MAXFREQ,               ///< PSCFG_MAXFREQ
  PSCFG_LR_MAXFREQ,            ///< PSCFG_LR_MAXFREQ
  PSCFG_RTT,                   ///< PSCFG_RTT
  PSCFG_LR_RTT,                ///< PSCFG_LR_RTT
  PSCFG_ODT_PAT_1D,            ///< PSCFG_ODT_PAT_1D
  PSCFG_ODT_PAT_2D,            ///< PSCFG_ODT_PAT_2D
  PSCFG_ODT_PAT_3D,            ///< PSCFG_ODT_PAT_3D
  PSCFG_LR_ODT_PAT_1D,         ///< PSCFG_LR_ODT_PAT_1D
  PSCFG_LR_ODT_PAT_2D,         ///< PSCFG_LR_ODT_PAT_2D
  PSCFG_LR_ODT_PAT_3D,         ///< PSCFG_LR_ODT_PAT_3D
  PSCFG_SAO,                   ///< PSCFG_SAO
  PSCFG_LR_SAO,                ///< PSCFG_LR_SAO
  PSCFG_MR0WR,                 ///< PSCFG_MR0WR
  PSCFG_MR0CL,                 ///< PSCFG_MR0CL
  PSCFG_RC2IBT,                ///< PSCFG_RC2IBT
  PSCFG_RC10OPSPD,             ///< PSCFG_RC10OPSPD
  PSCFG_LR_IBT,                ///< PSCFG_LR_IBT
  PSCFG_LR_NPR,                ///< PSCFG_LR_NPR
  PSCFG_LR_NLR,                ///< PSCFG_LR_NLR
  PSCFG_S2D,                   ///< PSCFG_S2D
  PSCFG_WL_PASS1_SEED,         ///< PSCFG_WL_PASS1_SEED
  PSCFG_HWRXEN_PASS1_SEED,     ///< PSCFG_HWRXEN_SEED

  // The type of general table entries could be added between
  // PSCFG_GEN_START and PSCFG_GEN_END so that the PSCGen routine
  // is able to look for the entries per the PSCType.
  PSCFG_GEN_START,             ///< PSCFG_GEN_START
  PSCFG_CLKDIS,                ///< PSCFG_CLKDIS
  PSCFG_CKETRI,                ///< PSCFG_CKETRI
  PSCFG_ODTTRI,                ///< PSCFG_ODTTRI
  PSCFG_CSTRI,                 ///< PSCFG_CSTRI
  PSCFG_GEN_END                ///< PSCFG_GEN_END
} PSCFG_TYPE;

/// Dimm types
typedef enum {
  UDIMM_TYPE = 0x01,           ///< UDIMM_TYPE
  RDIMM_TYPE = 0x02,           ///< RDIMM_TYPE
  SODIMM_TYPE = 0x04,          ///< SODIMM_TYPE
  LRDIMM_TYPE = 0x08,          ///< LRDIMM_TYPE
  SODWN_SODIMM_TYPE = 0x10,    ///< SODWN_SODIMM_TYPE
  DT_DONT_CARE = 0xFF          ///< DT_DONT_CARE
} DIMM_TYPE;

/// Number of DRAM devices or DIMM slots
typedef enum {
  _1DIMM = 0x01,               ///< _1DIMM
  _2DIMM = 0x02,               ///< _2DIMM
  _3DIMM = 0x04,               ///< _3DIMM
  _4DIMM = 0x08,               ///< _4DIMM
  _DIMM_NONE = 0xF0,           ///< _DIMM_NONE (no DIMM slot)
  NOD_DONT_CARE = 0xFF         ///< NOD_DONT_CARE
} NOD_SUPPORTED;

/// Table header related definitions
typedef struct {
  PSCFG_TYPE PSCType;           ///< PSC Type
  DIMM_TYPE DimmType;           ///< Dimm Type
  NOD_SUPPORTED NumOfDimm;      ///< Numbef of dimm
  CPU_LOGICAL_ID LogicalCpuid;  ///< Logical Cpuid
  UINT8 PackageType;            ///< Package Type
  TECHNOLOGY_TYPE TechType;     ///< Technology type
} PSC_TBL_HEADER;

/// Table entry
typedef struct {
  PSC_TBL_HEADER Header;        ///< PSC_TBL_HEADER
  UINT8 TableSize;              ///< Table size
  VOID *TBLPtr;                 ///< Pointer of the table
} PSC_TBL_ENTRY;

#define PT_DONT_CARE 0xFF
#define NP 1
#define V1_5  1
#define V1_35  2
#define V1_25  4
#define VOLT_ALL (V1_5 | V1_35 | V1_25)
#define DIMM_SR 2
#define DIMM_DR 4
#define DIMM_QR 8
#define DIMM_LR 2
#define R0 1
#define R1 2
#define R2 4
#define R3 8
#define CH_A 0x01
#define CH_B 0x02
#define CH_C 0x04
#define CH_D 0x08
#define CH_ALL 0x0F
/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

AGESA_STATUS
MemPConstructPsUDef (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

BOOLEAN
MemPGetDramTerm (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ArraySize,
  IN       CONST DRAM_TERM_ENTRY *DramTermPtr
  );

AGESA_STATUS
MemPConstructPsSHy3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsUHy3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsRHy3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsUC32_3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsRC32_3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );


AGESA_STATUS
MemPConstructPsSDr3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsUDr3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsRDr3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsUDA3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsSNi3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsUNi3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsSRb3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsURb3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsSPh3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsUPh3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsSDA3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsSDA2 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsSLN3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsULN3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsRLN3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsSON3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

AGESA_STATUS
MemPConstructPsUON3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  );

UINT16
MemPGetPorFreqLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 FreqLimitSize,
  IN       CONST POR_SPEED_LIMIT *FreqLimitPtr
  );

VOID
MemPGetPORFreqLimitDef (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemPPSCFlow (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemPConstructRankTypeMap (
  IN          UINT16 Dimm0,
  IN          UINT16 Dimm1,
  IN          UINT16 Dimm2,
  IN OUT   UINT16 *RankTypeInTable
 );

BOOLEAN
MemPIsIdSupported (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID LogicalId,
  IN       UINT8 PackageType
 );

UINT16
MemPGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  );

BOOLEAN
MemPRecPSCFlow (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemPRecConstructRankTypeMap (
  IN          UINT16 Dimm0,
  IN          UINT16 Dimm1,
  IN          UINT16 Dimm2,
  IN OUT   UINT16 *RankTypeInTable
 );

BOOLEAN
MemPRecIsIdSupported (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID LogicalId,
  IN       UINT8 PackageType
 );

UINT16
MemPRecGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  );

UINT16
MemPProceedTblDrvOverride (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 ProceededPSOType
  );

BOOLEAN
MemPGetPSCPass1Seed (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

#endif  /* _MP_H_ */
