/**
 * @file
 *
 * AMD AGESA CPU Preserve Registers used for AP Mailbox.
 *
 * Save and Restore the normal feature content of the registers being used for
 * the AP Mailbox.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
 */

#ifndef _PRESERVE_MAILBOX_H_
#define _PRESERVE_MAILBOX_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */
#define MAX_PRESERVE_REGISTER_ENTRIES 2      ///< There is room on the heap for up to this per node. 

/// Reference to a save buffer.
typedef UINT32 (*MAILBOX_REGISTER_SAVE_ENTRY) [MAX_PRESERVE_REGISTER_ENTRIES];

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/**
 * Descriptor for family specific save-restore.
 *
 * Provide a list of the register offsets to save-restore on each node.  Optionally, zero the
 * register instead of restoring it.
 */
typedef struct  {
  UINT16          Revision;                        ///< Interface version
  // Public Data.
  BOOLEAN   IsZeroOnCold;                          ///< On a cold boot, zero the register instead of restore.
  PCI_ADDR *RegisterList;                          ///< The list of registers, terminated by ILLEGAL_SBDFO.
} PRESERVE_MAILBOX_FAMILY_SERVICES;


/*----------------------------------------------------------------------------------------
 *                          F U N C T I O N S     P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

#endif  // _PRESERVE_MAILBOX_H_
