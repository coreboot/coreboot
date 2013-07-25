/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CRAT option API.
 *
 * Contains structures and values used to control the CRAT option code.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  OPTION
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*****************************************************************************
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
 ******************************************************************************
 */

#ifndef _OPTION_CRAT_H_
#define _OPTION_CRAT_H_

#include "cpuLateInit.h"
#include "cpuCrat.h"

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */

typedef AGESA_STATUS OPTION_CRAT_FEATURE (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   VOID                  **CratPtr
  );

#define CRAT_STRUCT_VERSION  0x01

/// The Option Configuration of CRAT
typedef struct  {
  UINT16              OptCratVersion;       ///< The version number of CRAT
  OPTION_CRAT_FEATURE  *CratFeature;        ///< The Option Feature of CRAT
  UINT8               OemIdString[6];       ///< Configurable OEM Id
  UINT8               OemTableIdString[8];  ///< Configurable OEM Table Id
} OPTION_CRAT_CONFIGURATION;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to generat CRAT cache affinity structure.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 *
 */

typedef VOID F_GENERATE_CRAT_CACHE (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/// Reference to a Method.
typedef F_GENERATE_CRAT_CACHE         *PF_GENERATE_CRAT_CACHE;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to generat CRAT TLB affinity structure.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 *
 */

typedef VOID F_GENERATE_CRAT_TLB (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/// Reference to a Method.
typedef F_GENERATE_CRAT_TLB         *PF_GENERATE_CRAT_TLB;

/**
 * Provide the interface to the CRAT Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
typedef struct _CRAT_FAMILY_SERVICES {
  UINT16          Revision;                                        ///< Interface version
  // Public Methods.
  PF_GENERATE_CRAT_CACHE   generateCratCacheEntry;                 ///< Method: Family specific call to generat CRAT cache affinity structure
  PF_GENERATE_CRAT_TLB     generateCratTLBEntry;                   ///< Method: Family specific call to generat CRAT TLB affinity structure

} CRAT_FAMILY_SERVICES;

/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */
VOID
MakeHSAProcUnitEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
MakeMemoryEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
MakeCacheEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
MakeTLBEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

/// @todo
//VOID
//MakeFPUEntry (
//  IN       CRAT_HEADER *CratHeaderStructPtr,
//  IN OUT   UINT8       **TableEnd,
//  IN OUT   AMD_CONFIG_PARAMS *StdHeader
//  );

/// @todo
//VOID
//MakeIOEntry (
//  IN       CRAT_HEADER *CratHeaderStructPtr,
//  IN OUT   UINT8       **TableEnd,
//  IN OUT   AMD_CONFIG_PARAMS *StdHeader
//  );

UINT8 *
AddOneCratEntry (
  IN       CRAT_ENTRY_TYPE CratEntryType,
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

#endif  // _OPTION_CRAT_H_

