/**
 * @file
 *
 * Install of build option: WHEA
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 6049 $   @e \$Date: 2008-05-14 01:58:02 -0500 (Wed, 14 May 2008) $
 */
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
 *
 ***************************************************************************/

#ifndef _OPTION_WHEA_INSTALL_H_
#define _OPTION_WHEA_INSTALL_H_

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#if AGESA_ENTRY_INIT_LATE == TRUE
  #ifndef OPTION_WHEA
    #error  BLDOPT: Option not defined: "OPTION_WHEA"
  #endif
  #if OPTION_WHEA == TRUE
    OPTION_WHEA_FEATURE          GetAcpiWheaMain;
    #define USER_WHEA_OPTION     GetAcpiWheaMain
  #else
    OPTION_WHEA_FEATURE          GetAcpiWheaStub;
    #define USER_WHEA_OPTION     GetAcpiWheaStub
  #endif

#else
  OPTION_WHEA_FEATURE            GetAcpiWheaStub;
  #define USER_WHEA_OPTION       GetAcpiWheaStub
#endif

/*  Declare the instance of the WHEA option configuration structure  */
OPTION_WHEA_CONFIGURATION OptionWheaConfiguration = {
  WHEA_STRUCT_VERSION,
  USER_WHEA_OPTION
};

#endif  // _OPTION_WHEA_INSTALL_H_
