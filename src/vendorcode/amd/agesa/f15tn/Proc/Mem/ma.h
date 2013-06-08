/* $NoKeywords:$ */
/**
 * @file
 *
 * ma.h
 *
 * ARDK common header file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
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
* ***************************************************************************
*
*/

#ifndef _MA_H_
#define _MA_H_

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


#define MAX_CS_PER_CHANNEL      8   ///< Max CS per channel
/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/** MARDK Structure*/
typedef struct {
  UINT16   Speed;                  ///< Dram speed in MHz
  UINT8    Loads;                  ///< Number of Data Loads
  UINT32   AddrTmg;                ///< Address Timing value
  UINT32   Odc;                    ///< Output Driver Compensation Value
} PSCFG_ENTRY;

/** MARDK Structure*/
typedef struct {
  UINT16   Speed;                  ///< Dram speed in MHz
  UINT8    Loads;                  ///< Number of Data Loads
  UINT32   AddrTmg;                ///< Address Timing value
  UINT32   Odc;                    ///< Output Driver Compensation Value
  UINT8    Dimms;                  ///< Number of Dimms
} ADV_PSCFG_ENTRY;

/** MARDK Structure for RDIMMs*/
typedef struct {
  UINT16   Speed;                  ///< Dram speed in MHz
  UINT16   DIMMRankType;         ///< Bitmap of Ranks   //Bit0-3:DIMM0(1:SR, 2:DR, 4:QR, 0:No Dimm, 0xF:Any), Bit4-7:DIMM1, Bit8-11:DIMM2, Bit12-16:DIMM3
  UINT32   AddrTmg;                ///< Address Timing value
  UINT16   RC2RC8;                   ///< RC2 and RC8 value  //High byte: 1st pair value, Low byte: 2nd pair value
  UINT8    Dimms;                  ///< Number of Dimms
} ADV_R_PSCFG_ENTRY;

/** MARDK Structure*/
typedef struct {
  UINT16   DIMMRankType;         ///< Bitmap of Ranks   //Bit0-3:DIMM0(1:SR, 2:DR, 4:QR, 0:No Dimm, 0xF:Any), Bit4-7:DIMM1, Bit8-11:DIMM2, Bit12-16:DIMM3
  UINT32   PhyRODTCSLow;          ///< Fn2_9C 180
  UINT32   PhyRODTCSHigh;         ///< Fn2_9C 181
  UINT32   PhyWODTCSLow;          ///< Fn2_9C 182
  UINT32   PhyWODTCSHigh;         ///< Fn2_9C 183
  UINT8    Dimms;                  ///< Number of Dimms
} ADV_PSCFG_ODT_ENTRY;

/** MARDK Structure for Write Levelization ODT*/
typedef struct {
  UINT16   DIMMRankType;         ///< Bitmap of Ranks   //Bit0-3:DIMM0(1:SR, 2:DR, 4:QR, 0:No Dimm, 0xF:Any), Bit4-7:DIMM1, Bit8-11:DIMM2, Bit12-16:DIMM3
  UINT8    PhyWrLvOdt[MAX_CS_PER_CHANNEL / 2];            ///< WrLvOdt (Fn2_9C_0x08[11:8]) Value for each Dimm
  UINT8    Dimms;                ///< Number of Dimms
} ADV_R_PSCFG_WL_ODT_ENTRY;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

AGESA_STATUS
MemAGetPsCfgDef (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgRDr2 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgRDr3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgUDr3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgSDA2 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgSDA3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgSNi3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgUNi3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgSRb3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgURb3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgSPh3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgUPh3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgUDA3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgRHy3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgUHy3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgRC32_3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgUC32_3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgSLN3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgULN3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgSON3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgUON3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgROr3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemAGetPsCfgUOr3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

UINT16
MemAGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemRecNGetPsCfgDef (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

UINT16
MemRecNGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemRecNGetPsCfgUDIMM3Nb (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemRecNGetPsCfgSODIMM3Nb (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

AGESA_STATUS
MemRecNGetPsCfgRDIMM3Nb (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

#endif  /* _MA_H_ */
