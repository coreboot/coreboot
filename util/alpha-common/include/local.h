#ifndef __LOCAL_H_LOADED
#define __LOCAL_H_LOADED
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

/*
 *  $Id$;
 */

/*
 * $Log$
 * Revision 1.1  2001/03/22 21:26:32  rminnich
 * testing I HATE CVS
 *
 * Revision 1.1  2000/03/21 03:56:31  stepan
 * Check in current version which is a nearly a 2.2-16
 *
 * Revision 1.3  1995/02/27  21:27:56  fdh
 * Include <ctype.h>
 *
 * Revision 1.2  1995/02/10  02:03:16  fdh
 * Added definitions for TRUE and FALSE.
 *
 * Revision 1.1  1995/02/07  00:44:43  fdh
 * Initial revision
 *
 */

/*
 * Source files that are common between the ebfw source
 * tree and the ebtools source tree could have inherent
 * conflicts in their requirements for libraries, definitions,
 * etc.  The basic difference is that ebtools are built
 * to run natively with an operating system where the ebfw
 * tree is built to run in a freestanding environment on,
 * typically, an evaluation board target.  Therefore, this
 * file is used to provide the proper environment for building
 * those common files in the ebtools source tree. 
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "c_32_64.h"

#define ub unsigned char
#define uw unsigned short int

#ifdef	CROSS_COMPILE_32_TO_64_BITS

#define ui unsigned int
#define sl int64
#define ul int64

#else	/* CROSS_COMPILE_32_TO_64_BITS */

#ifdef _WIN32
#define ul unsigned __int64
#define sl __int64
#define ui unsigned long
#else
#define ul unsigned long
#define sl long
#define ui unsigned int
#endif

#endif	/* CROSS_COMPILE_32_TO_64_BITS */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif  /* __LOCAL_H_LOADED */
