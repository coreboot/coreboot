/* $NoKeywords:$ */
/**
 * @file
 *
 * Describes compiler dependencies - to support several compile time environments
 *
 * Contains compiler environment porting descriptions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Includes
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 03:16:51 -0600 (Wed, 22 Dec 2010) $
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2011 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
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
 *
 ***************************************************************************/

#ifndef _PORTING_H_
#define _PORTING_H_

#if defined (_MSC_VER)
  #include <intrin.h>
  void _disable (void);
  void _enable (void);
  #pragma warning(disable: 4103 4001 4733)
  #pragma intrinsic (_disable, _enable)
  #pragma warning(push)
  // -----------------------------------------------------------------------
  //   Define a code_seg MACRO
  //
  #define MAKE_AS_A_STRING(arg) #arg

  #define CODE_GROUP(arg) __pragma (code_seg (MAKE_AS_A_STRING (.t##arg)))

  #define RDATA_GROUP(arg) __pragma (const_seg (MAKE_AS_A_STRING (.d##arg)))
  #define FUNC_ATTRIBUTE(arg) __declspec(arg)
  //#include <intrin.h>  // MS has built-in functions

  #if _MSC_VER < 900
    // -----------------------------------------------------------------------
    //    Assume MSVC 1.52C (16-bit)
    //
    //    NOTE: When using MSVC 1.52C use the following command line:
    //
    //       CL.EXE /G3 /AL /O1i /Fa <FILENAME.C>
    //
    //    This will produce 32-bit code in USE16 segment that is optimized for code
    //    size.
    typedef void          VOID;

    // Create the universal 32, 16, and 8-bit data types
    typedef unsigned long   UINTN;
    typedef          long   INT32;
    typedef unsigned long   UINT32;
    typedef          int    INT16;
    typedef unsigned int    UINT16;
    typedef          char   INT8;
    typedef unsigned char   UINT8;
    typedef          char   CHAR8;
    typedef unsigned short  CHAR16;

    /// struct for 16-bit environment handling of 64-bit value
    typedef struct _UINT64 {
      IN OUT  UINT32 lo;      ///< lower 32-bits of 64-bit value
      IN OUT  UINT32 hi;      ///< highest 32-bits of 64-bit value
    } UINT64;

    // Create the Boolean type
    #define TRUE  1
    #define FALSE 0
    typedef unsigned char BOOLEAN;

    #define CONST const
    #define STATIC static
    #define VOLATILE volatile
    #define CALLCONV __pascal
    #define ROMDATA __based( __segname( "_CODE" ) )
    #define _16BYTE_ALIGN   __declspec(align(16))

    // Force tight packing of structures
    // Note: Entire AGESA (Project / Solution) will be using pragma pack 1
    #pragma warning( disable : 4103 ) // Disable '#pragma pack' in .h warning
    #pragma pack(1)

    //   Disable WORD->BYTE automatic conversion warnings.  Example:
    //   BYTE LocalByte;
    //   void MyFunc(BYTE val);
    //
    //   MyFunc(LocalByte*2+1); // Warning, automatic conversion
    //
    //   The problem is any time math is performed on a BYTE, it is converted to a
    //   WORD by MSVC 1.52c, and then when it is converted back to a BYTE, a warning
    //   is generated.  Disable warning C4761
    #pragma warning( disable : 4761 )

  #else
    // -----------------------------------------------------------------------
    //   Assume a 32-bit MSVC++
    //
    // Disable the following warnings:
    // 4100 - 'identifier' : unreferenced formal parameter
    // 4276 - 'function' : no prototype provided; assumed no parameters
    // 4214 - non standard extension used : bit field types other than int
    // 4001 - nonstandard extension 'single line comment' was used
    // 4142 - benign redefinition of type for following declaration
    //      - typedef char    INT8
    #if defined (_M_IX86)
      #pragma warning (disable: 4100 4276 4214 4001 4142 4305 4306)

      #ifndef VOID
        typedef void VOID;
      #endif
    // Create the universal 32, 16, and 8-bit data types
      #ifndef UINTN
        typedef unsigned __w64 UINTN;
      #endif
      typedef          __int64 INT64;
      typedef unsigned __int64 UINT64;
      typedef          int   INT32;
      typedef unsigned int   UINT32;
      typedef          short INT16;
      typedef unsigned short UINT16;
      typedef          char  INT8;
      typedef unsigned char  UINT8;
      typedef          char  CHAR8;
      typedef unsigned short CHAR16;

    // Create the Boolean type
      #ifndef TRUE
        #define TRUE  1
      #endif
      #ifndef FALSE
        #define FALSE 0
      #endif
      typedef unsigned char BOOLEAN;

      // Force tight packing of structures
      // Note: Entire AGESA (Project / Solution) will be using pragma pack 1
      #pragma pack(1)

      #define CONST const
      #define STATIC static
      #define VOLATILE volatile
      #define CALLCONV
      #define ROMDATA
      #define _16BYTE_ALIGN __declspec(align(64))
      // 64 bit of compiler
    #else
      #pragma warning (disable: 4100 4276 4214 4001 4142 4305 4306 4366)

      #ifndef VOID
        typedef void VOID;
      #endif
      // Create the universal 32, 16, and 8-bit data types
      #ifndef UINTN
        typedef unsigned __int64 UINTN;
      #endif
      typedef          __int64 INT64;
      typedef unsigned __int64 UINT64;
      typedef          int   INT32;
      typedef unsigned int   UINT32;
      typedef          short INT16;
      typedef unsigned short UINT16;
      typedef          char  INT8;
      typedef unsigned char  UINT8;
      typedef          char  CHAR8;
      typedef unsigned short CHAR16;

      // Create the Boolean type
      #ifndef TRUE
        #define TRUE  1
      #endif
      #ifndef FALSE
        #define FALSE 0
      #endif
      typedef unsigned char BOOLEAN;

      // Force tight packing of structures
      // Note: Entire AGESA (Project / Solution) will be using pragma pack 1
      #pragma pack(1)

      #define CONST const
      #define STATIC static
      #define VOLATILE volatile
      #define CALLCONV
      #define ROMDATA
    #endif
  #endif
  // -----------------------------------------------------------------------
  // End of MS compiler versions

#elif defined __GNUC__

  #define IN
  #define OUT
  #define STATIC static
  #define VOLATILE volatile
  #define TRUE 1
  #define FALSE 0
//  #undef CONST
  #define CONST const
  #define ROMDATA
  #define CALLCONV
  #define _16BYTE_ALIGN __attribute__ ((aligned (16)))

  typedef unsigned char  BOOLEAN;
  typedef   signed char  INT8;
  typedef   signed short INT16;
  typedef   signed long  INT32;
  typedef          char  CHAR8;
  typedef unsigned char  UINT8;
  typedef unsigned short UINT16;
  typedef unsigned long  UINT32;
  typedef unsigned long  UINTN;
  typedef unsigned long  long UINT64;
  typedef long  long INT64;
  typedef void VOID;
  //typedef unsigned long  size_t;

  //#include <intrin.h>                   // MingW-w64 library header
#pragma pack(1)

#define CODE_GROUP(arg)
#define RDATA_GROUP(arg)

#define FUNC_ATTRIBUTE(arg) __attribute__((arg))
#define MAKE_AS_A_STRING(arg) #arg
#include <stddef.h>
#include "gcc-intrin.h"

#include <assert.h>
#include <console/console.h>
#include <console/loglevel.h>

#ifndef NULL
  #define NULL              (void *)0
#endif

#else
  // -----------------------------------------------------------------------
  // Unknown or unsupported compiler
  //
  #error "Unknown compiler in use"
#endif

// -----------------------------------------------------------------------
// Common definitions for all compilers
//

//Support forward reference construct
#define AGESA_FORWARD_DECLARATION(x) typedef struct _##x x

// The following are use in conformance to the UEFI style guide
#define IN
#define OUT

#endif // _PORTING_H_
