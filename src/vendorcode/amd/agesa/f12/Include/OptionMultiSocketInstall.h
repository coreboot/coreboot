/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: Multiple Socket Support
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 48937 $   @e \$Date: 2011-03-15 03:37:15 +0800 (Tue, 15 Mar 2011) $
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

#ifndef _OPTION_MULTISOCKET_INSTALL_H_
#define _OPTION_MULTISOCKET_INSTALL_H_

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#ifndef OPTION_MULTISOCKET
  #error  BLDOPT: Option not defined: "OPTION_MULTISOCKET"
#endif

#if OPTION_MULTISOCKET == TRUE
  OPTION_MULTISOCKET_PM_STEPS GetNumberOfSystemPmStepsPtrMulti;
  #define GET_NUM_PM_STEPS    GetNumberOfSystemPmStepsPtrMulti
  OPTION_MULTISOCKET_PM_CORE0_TASK RunCodeOnAllSystemCore0sMulti;
  #define CORE0_PM_TASK       RunCodeOnAllSystemCore0sMulti
  OPTION_MULTISOCKET_PM_NB_COF GetSystemNbCofMulti;
  #define GET_SYS_NB_COF      GetSystemNbCofMulti
  OPTION_MULTISOCKET_PM_NB_COF_UPDATE GetSystemNbCofVidUpdateMulti;
  #define GET_SYS_NB_COF_UPDATE GetSystemNbCofVidUpdateMulti
  OPTION_MULTISOCKET_PM_GET_EVENTS GetEarlyPmErrorsMulti;
  #define GET_EARLY_PM_ERRORS GetEarlyPmErrorsMulti
  OPTION_MULTISOCKET_PM_NB_MIN_COF GetMinNbCofMulti;
  #define GET_MIN_NB_COF GetMinNbCofMulti
#else
  OPTION_MULTISOCKET_PM_STEPS GetNumberOfSystemPmStepsPtrSingle;
  #define GET_NUM_PM_STEPS    GetNumberOfSystemPmStepsPtrSingle
  OPTION_MULTISOCKET_PM_CORE0_TASK RunCodeOnAllSystemCore0sSingle;
  #define CORE0_PM_TASK       RunCodeOnAllSystemCore0sSingle
  OPTION_MULTISOCKET_PM_NB_COF GetSystemNbCofSingle;
  #define GET_SYS_NB_COF      GetSystemNbCofSingle
  OPTION_MULTISOCKET_PM_NB_COF_UPDATE GetSystemNbCofVidUpdateSingle;
  #define GET_SYS_NB_COF_UPDATE GetSystemNbCofVidUpdateSingle
  OPTION_MULTISOCKET_PM_GET_EVENTS GetEarlyPmErrorsSingle;
  #define GET_EARLY_PM_ERRORS GetEarlyPmErrorsSingle
  OPTION_MULTISOCKET_PM_NB_MIN_COF GetMinNbCofSingle;
  #define GET_MIN_NB_COF GetMinNbCofSingle
#endif

/*  Declare the instance of the DMI option configuration structure  */
OPTION_MULTISOCKET_CONFIGURATION  OptionMultiSocketConfiguration = {
  MULTISOCKET_STRUCT_VERSION,
  GET_NUM_PM_STEPS,
  CORE0_PM_TASK,
  GET_SYS_NB_COF,
  GET_SYS_NB_COF_UPDATE,
  GET_EARLY_PM_ERRORS,
  GET_MIN_NB_COF
};

#endif  // _OPTION_MULTISOCKET_INSTALL_H_
