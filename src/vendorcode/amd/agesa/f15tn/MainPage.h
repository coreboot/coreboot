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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
 * @subpage fchmain "FCH Component Documentation."
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
