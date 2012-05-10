/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: S3SCRIPT
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
 ***************************************************************************/

#ifndef _OPTION_S3SCRIPT_INSTALL_H_
#define _OPTION_S3SCRIPT_INSTALL_H_

#include "S3SaveState.h"
/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#ifndef OPTION_S3SCRIPT
  #define OPTION_S3SCRIPT  FALSE    //if not define assume PI not use script
#endif

#if (AGESA_ENTRY_INIT_LATE == TRUE) || (AGESA_ENTRY_INIT_ENV == TRUE) || (AGESA_ENTRY_INIT_MID == TRUE)
  #if OPTION_S3SCRIPT == TRUE
    #define P_S3_SCRIPT_INIT      S3ScriptInitState
  #endif
#endif

#if AGESA_ENTRY_INIT_LATE_RESTORE == TRUE
  #if OPTION_S3SCRIPT == TRUE
    #define P_S3_SCRIPT_RESTORE   S3ScriptRestoreState
  #endif
#endif

#ifndef P_S3_SCRIPT_INIT
  #define P_S3_SCRIPT_INIT        S3ScriptInitStateStub
#endif

#ifndef P_S3_SCRIPT_RESTORE
  #define P_S3_SCRIPT_RESTORE     S3ScriptInitStateStub
  #undef  GNB_S3_DISPATCH_FUNCTION_TABLE
#endif

#ifndef GNB_S3_DISPATCH_FUNCTION_TABLE
  #define GNB_S3_DISPATCH_FUNCTION_TABLE
#endif

/*  Declare the instance of the S3SCRIPT option configuration structure  */
S3_SCRIPT_CONFIGURATION OptionS3ScriptConfiguration = {
  P_S3_SCRIPT_INIT,
  P_S3_SCRIPT_RESTORE
};

S3_DISPATCH_FUNCTION_ENTRY S3DispatchFunctionTable [] = {
  GNB_S3_DISPATCH_FUNCTION_TABLE
  {0, NULL}
};
#endif  // _OPTION_S3SCRIPT_INSTALL_H_
