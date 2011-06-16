/* $NoKeywords:$ */
/**
 * @file
 *
 * Family 12h 'early sample' support
 *
 * This file defines the required structures for family 12h pre-production processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 37456 $   @e \$Date: 2010-09-04 04:17:05 +0800 (Sat, 04 Sep 2010) $
 */
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
 * 
 * ***************************************************************************
 *
 */

#ifndef _OPTION_FAMILY_12H_EARLY_SAMPLE_H_
#define _OPTION_FAMILY_12H_EARLY_SAMPLE_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/**
 *  Return the appropriate values of certain NB P-state related registers.
 *
 *  @param[in,out] FCRxFE00_6000           The value of FCRxFE006000.
 *  @param[in,out] FCRxFE00_6002           The value of FCRxFE006002.
 *  @param[in,out] FCRxFE00_7006           The value of FCRxFE007006.
 *  @param[in,out] FCRxFE00_7009           The value of FCRxFE007009.
 *  @param[in]     MainPll                 The main PLL frequency.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID F_F12_ES_NB_PSTATE_INIT (
  IN OUT   VOID *FCRxFE00_6000,
  IN OUT   VOID *FCRxFE00_6002,
  IN OUT   VOID *FCRxFE00_7006,
  IN OUT   VOID *FCRxFE00_7009,
  IN       UINT32 MainPll,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_F12_ES_NB_PSTATE_INIT *PF_F12_ES_NB_PSTATE_INIT;

/**
 *  Return the appropriate value of a certain NB P-state related register.
 *
 *  @param[in,out] FCRxFE00_6000           The value of FCRxFE006000.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID F_F12_ES_POWER_PLANE_INIT (
  IN OUT   VOID *FCRxFE00_6000,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_F12_ES_POWER_PLANE_INIT *PF_F12_ES_POWER_PLANE_INIT;

/**
 *  Implements an early sample workaround required for C6
 *
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID F_F12_ES_C6_INIT (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_F12_ES_C6_INIT *PF_F12_ES_C6_INIT;


/// Hook points in the core functionality necessary for
/// providing support for pre-production CPUs.
typedef struct {
  PF_F12_ES_POWER_PLANE_INIT     F12PowerPlaneInitHook;     ///< Allows for override of a certain processor register value during power plane init
  PF_F12_ES_NB_PSTATE_INIT       F12NbPstateInitHook;       ///< Allows for override of certain processor register values for proper NB P-state init
} F12_ES_CORE_SUPPORT;

/// Hook points in the C6 feature necessary for
/// providing support for pre-production CPUs.
typedef struct {
  PF_F12_ES_C6_INIT            F12InitializeC6;        ///< Hook for erratum 453 workaround
} F12_ES_C6_SUPPORT;


/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */


#endif  // _OPTION_FAMILY_12H_EARLY_SAMPLE_H_
