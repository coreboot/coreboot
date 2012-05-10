/**
 * @file
 *
 * mnS3da.h
 *
 * S3 resume memory related function for DA.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/DA)
 * @e \$Revision: 6474 $ @e \$Date: 2008-06-20 03:07:59 -0500 (Fri, 20 Jun 2008) $
 *
 **/
/*****************************************************************************
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

#ifndef _MNS3DA_H_
#define _MNS3DA_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
/// ID for register list of DA
typedef enum {
  PCI_LST_ESR_DA,   ///< Assign 0x0000 for PCI register list for pre exit self refresh.
  PCI_LST_DA,       ///< Assign 0x0001 for PCI register list for post exist self refresh.
  CPCI_LST_ESR_DA,  ///< Assign 0x0002 for conditional PCI register list for pre exit self refresh.
  CPCI_LST_DA,      ///< Assign 0x0003 for conditional PCI register list for post exit self refresh.
  MSR_LST_ESR_DA,   ///< Assign 0x0004 for MSR register list for pre exit self refresh.
  MSR_LST_DA,       ///< Assign 0x0005 for MSR register list for post exit self refresh.
  CMSR_LST_ESR_DA,  ///< Assign 0x0006 for conditional MSR register list for pre exit self refresh.
  CMSR_LST_DA,      ///< Assign 0x0007 for conditional MSR register list for post exit self refresh.
} RegisterListIDDA;

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

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

#endif //_MNS3DA_H_
