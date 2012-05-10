/**
 * @file
 *
 * AMD CPU Cache Flush On Halt Function.
 *
 * Contains code to Level the Feature in a multi-socket system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x10
 * @e \$Revision: 6626 $   @e \$Date: 2008-07-04 02:01:02 +0800 (Fri, 04 Jul 2008) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 *----------------------------------------------------------------------------
 */


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuPostInit.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10CACHEFLUSHONHALT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          P U B L I C     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *    Enable Cpu Cache Flush On Halt Function
 *
 *    @param[in]       FamilySpecificServices   The current Family Specific Services.
 *    @param[in,out]   PciAddress               Pointer to Pci address struct.
 *    @param[in,out]   AndMask                  Pointer to AND mask bits.
 *    @param[in,out]   OrMask                   Pointer to Or mask bits.
 *    @param[in]       StdHeader                Handle of Header for calling lib functions and services.
 */
STATIC VOID
GetF10CacheFlushOnHaltRegister (
  IN       CPU_CFOH_FAMILY_SERVICES     *FamilySpecificServices,
  IN OUT   PCI_ADDR                     *PciAddress,
  IN OUT   UINT32                       *AndMask,
  IN OUT   UINT32                       *OrMask,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  )
{
  UINT32     CoreCount;
  CPU_LOGICAL_ID LogicalId;

  //
  // F3xDC[25:19] = 28h
  // F3xDC[18:16] = 111b
  //
  PciAddress->Address.Function = FUNC_3;
  PciAddress->Address.Register = CLOCK_POWER_TIMING_CTRL2_REG;
  *AndMask = 0xFC00FFFF;
  *OrMask = 0x01470000;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if ((LogicalId.Revision & AMD_F10_C2) != 0) {
    //
    //For F10_C2 single Core, F3xDC[18:16] = 0
    //
    GetActiveCoresInCurrentSocket (&CoreCount, StdHeader);
    if (CoreCount == 1) {
      *OrMask = 0x01400000;
    }
  }
}

CONST CPU_CFOH_FAMILY_SERVICES ROMDATA F10CacheFlushOnHalt =
{
  0,
  GetF10CacheFlushOnHaltRegister
};