/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build options for a Scorpius platform solution
 *
 * This file generates the defaults tables for the "Scorpius" platform solution
 * set of processors. The documented build options are imported from a user
 * controlled file for processing.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 59375 $   @e \$Date: 2011-09-21 13:24:35 -0600 (Wed, 21 Sep 2011) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/

#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "AdvancedApi.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "cpuFeatures.h"
#include "Table.h"
#include "CommonReturns.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "GnbInterfaceStub.h"

/*****************************************************************************
 *   Define the RELEASE VERSION string
 *
 * The Release Version string should identify the next planned release.
 * When a branch is made in preparation for a release, the release manager
 * should change/confirm that the branch version of this file contains the
 * string matching the desired version for the release. The trunk version of
 * the file should always contain a trailing 'X'. This will make sure that a
 * development build from trunk will not be confused for a released version.
 * The release manager will need to remove the trailing 'X' and update the
 * version string as appropriate for the release. The trunk copy of this file
 * should also be updated/incremented for the next expected version, + trailing 'X'
 ****************************************************************************/
                  // This is the delivery package title, "OrochiPI"
                  // This string MUST be exactly 8 characters long
#define AGESA_PACKAGE_STRING  {'O', 'r', 'o', 'c', 'h', 'i', 'P', 'I'}

                  // This is the release version number of the AGESA component
                  // This string MUST be exactly 12 characters long
#define AGESA_VERSION_STRING  {'V', '1', '.', '2', '.', '0', '.', '0', ' ', ' ', ' ', ' '}


// The Scorpius solution is defined to be families 0x10 and 0x15 models 0x0 - 0xF in the AM3 socket.
#define INSTALL_AM3_SOCKET_SUPPORT           TRUE
#define INSTALL_FAMILY_10_SUPPORT            TRUE
#define INSTALL_FAMILY_15_MODEL_0x_SUPPORT   TRUE

#ifdef BLDOPT_REMOVE_FAMILY_10_SUPPORT
  #if BLDOPT_REMOVE_FAMILY_10_SUPPORT == TRUE
    #undef INSTALL_FAMILY_10_SUPPORT
    #define INSTALL_FAMILY_10_SUPPORT     FALSE
  #endif
#endif

#ifdef BLDOPT_REMOVE_FAMILY_15_SUPPORT
  #if BLDOPT_REMOVE_FAMILY_15_SUPPORT == TRUE
    #undef INSTALL_FAMILY_15_MODEL_0x_SUPPORT
    #define INSTALL_FAMILY_15_MODEL_0x_SUPPORT     FALSE
  #endif
#endif


// The following definitions specify the default values for various parameters in which there are
// no clearly defined defaults to be used in the common file.  The values below are based on product
// and BKDG content, please consult the AGESA Memory team for consultation.
#define DFLT_SCRUB_DRAM_RATE            (0xFF)
#define DFLT_SCRUB_L2_RATE              (0x10)
#define DFLT_SCRUB_L3_RATE              (0x10)
#define DFLT_SCRUB_IC_RATE              (0)
#define DFLT_SCRUB_DC_RATE              (0x12)
#define DFLT_MEMORY_QUADRANK_TYPE       QUADRANK_REGISTERED
#define DFLT_VRM_SLEW_RATE              (2500)


// Instantiate all solution relevant data.
#include "PlatformInstall.h"

