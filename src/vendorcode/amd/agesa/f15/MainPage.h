/* $NoKeywords:$ */
/**
 * @file
 *
 * Create outline and references for mainpage documentation.
 *
 * Design guides, maintenance guides, and general documentation, are
 * collected using this file onto the documentation mainpage.
 * This file contains doxygen comment blocks, only.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Documentation
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/**
 * @mainpage
 *
 * The design and maintenance documentation for AGESA Sample Code is organized as
 * follows.  On this page, you can reference design guides, maintenance guides, and
 * general documentation.  Detailed Data Structure, Function, and Interface documentation
 * may be found using the Data Structures or Files tabs.  See Related Pages for a
 * Release content summary, and, if this is not a production release, lists of To Do's,
 * Deprecated items, etc.
 *
 * @subpage starthere "Start Here - Initial Porting and Integration."
 *
 * @subpage optionmain "Build Configuration and Options Guides and Documentation."
 *
 * @subpage commonmain "Processor Common Component Guides and Documentation."
 *
 * @subpage cpumain "CPU Component Guides and Documentation."
 *
 * @subpage htmain "HT Component Guides and Documentation."
 *
 * @subpage memmain "MEM Component Guides and Documentation."
 *
 * @subpage gnbmain "GNB Component Documentation."
 *
 * @subpage idsmain "IDS Component Guides and Documentation."
 *
 * @subpage recoverymain "Recovery Component Guides and Documentation."
 *
 */

/**
 * @page starthere Initial Porting and Integration
 *
 * @par Basic Check List
 *
 * <ul>
 * <li> Copy the \<plat\>Options.c file from the Addendum directory to the platform tip build directory.
 * AMD recommends the use of a sub-directory named AGESA to contain these files and the build output files.
 * <li> Copy the OptionsIds.h content in the spec to OptionsIds.h in the platform build tip directory
 * and make changes to enable the IDS support desired. It is highly recommended to set the following for
 * initial integration and development:@n
 * @code
 * #define IDSOPT_IDS_ENABLED TRUE
 * #define IDSOPT_ERROR_TRAP_ENABLED TRUE
 * #define IDSOPT_ASSERT_ENABLED TRUE
 * @endcode
 * <li> Edit and modify the option selections in those two files to meet the needs of the specific platform.
 * <li> Set the environment variable AGESA_ROOT to the root folder of the AGESA code.
 * <li> Set the environment variable AGESA_OptsDir the platform build tip AGESA directory.
 * <li> Generate the doxygen documentation or locate the file arch2008.chm within your AGESA release package.
 * </ul>
 *
 * @par Debugging Using ASSERT and IDS_ERROR_TRAP
 *
 * While AGESA code uses ::ASSERT and ::IDS_ERROR_TRAP to check for internal errors, these macros can also
 * catch and assist debug of wrapper and platform BIOS issues.
 *
 * When an ::ASSERT fails or an ::IDS_ERROR_TRAP is executed, the AGESA code will enter a halt loop and display a
 * Stop Code.  A Stop Code is eight hex digits.  The first (most significant) four are the FILECODE.
 * FILECODEs can be looked up in Filecode.h to determine which file contains the stop macro.  Each file has a
 * unique code value.
 * The least significant digits are the line number in that file.
 * For example, 0210 means the macro is on line two hundred ten.
 * (see ::IdsErrorStop for more details on stop code display.)
 *
 * Enabling ::ASSERT and ::IDS_ERROR_TRAP ensure errors are caught and also provide a useful debug assist.
 * Comments near each macro use will describe the nature of the error and typical wrapper errors or other
 * root causes.
 *
 * After your wrapper consistently executes ::ASSERT and ::IDS_ERROR_TRAP stop free, you can disable them in
 * OptionsIds.h, except for regression testing.  IDS is not expected to be enabled in production BIOS builds.
 *
 */
