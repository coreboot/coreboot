/*
 *
 * Copyright (C) 2010 coresystems GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _LIMITS_H
#define _LIMITS_H

#ifndef PATH_MAX
# if defined(_POSIX_PATH_MAX)
#  define PATH_MAX _POSIX_PATH_MAX
# elif defined(MAXPATHLEN)
#  define PATH_MAX MAXPATHLEN
# else
#  define PATH_MAX 255	/* the Posix minimum path-size */
# endif
#endif

#define CHAR_BIT	8

#define USHRT_MAX	((unsigned short int)~0U)
#define SHRT_MIN	((short int)(USHRT_MAX & ~(USHRT_MAX >> 1)))
#define SHRT_MAX	((short int)(USHRT_MAX >> 1))

#define UINT_MAX	((unsigned int)~0U)
#define INT_MIN		((int)(UINT_MAX & ~(UINT_MAX >> 1)))
#define INT_MAX		((int)(UINT_MAX >> 1))

#define ULONG_MAX	((unsigned long int)~0UL)
#define LONG_MIN	((long int)(ULONG_MAX & ~(ULONG_MAX >> 1)))
#define LONG_MAX	((long int)(ULONG_MAX >> 1))

#define ULLONG_MAX	((unsigned long long int)~0UL)
#define LLONG_MIN	((long long int)(ULLONG_MAX & ~(ULLONG_MAX >> 1)))
#define LLONG_MAX	((long long int)(ULLONG_MAX >> 1))

#endif
