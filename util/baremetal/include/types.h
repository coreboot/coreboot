/* Copyright (C) 1991,92,94,95,96,97,98,99, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/*
 *	POSIX Standard: 2.6 Primitive System Data Types	<sys/types.h>
 */

#ifndef	_SYS_TYPES_H
#define	_SYS_TYPES_H	1

#include <features.h>

__BEGIN_DECLS

#include <bits/types.h>

#ifdef	__USE_BSD
typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;
#endif

typedef __loff_t loff_t;

#ifndef ino_t
# ifndef __USE_FILE_OFFSET64
typedef __ino_t ino_t;
# else
typedef __ino64_t ino_t;
# endif
# define ino_t ino_t
#endif
#if defined __USE_LARGEFILE64 && !defined ino64_t
typedef __ino64_t ino64_t;
# define ino64_t ino64_t
#endif

#ifndef dev_t
typedef __dev_t dev_t;
# define dev_t dev_t
#endif

#ifndef gid_t
typedef __gid_t gid_t;
# define gid_t gid_t
#endif

#ifndef mode_t
typedef __mode_t mode_t;
# define mode_t mode_t
#endif

#ifndef nlink_t
typedef __nlink_t nlink_t;
# define nlink_t nlink_t
#endif

#ifndef uid_t
typedef __uid_t uid_t;
# define uid_t uid_t
#endif

#ifndef off_t
# ifndef __USE_FILE_OFFSET64
typedef __off_t off_t;
# else
typedef __off64_t off_t;
# endif
# define off_t off_t
#endif
#if defined __USE_LARGEFILE64 && !defined off64_t
typedef __off64_t off64_t;
# define off64_t off64_t
#endif

#ifndef pid_t
typedef __pid_t pid_t;
# define pid_t pid_t
#endif

#if defined __USE_SVID || defined __USE_XOPEN
typedef __id_t id_t;
#endif

#ifndef ssize_t
typedef __ssize_t ssize_t;
# define ssize_t ssize_t
#endif

#ifdef	__USE_BSD
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;
#endif

#if defined __USE_SVID || defined __USE_XOPEN
typedef __key_t key_t;
#endif

#ifdef __USE_XOPEN
# define __need_clock_t
#endif
#define	__need_time_t
#include <time.h>

#define	__need_size_t
#include <stddef.h>

#ifdef __USE_MISC
/* Old compatibility names for C types.  */
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
#endif

/* These size-specific names are used by some of the inet code.  */

#if !defined __GNUC__ || __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)

/* These types are defined by the ISO C 9x header <inttypes.h>. */
# ifndef __int8_t_defined
#  define __int8_t_defined
typedef	char int8_t;
typedef	short int int16_t;
typedef	int int32_t;
#  ifdef __GNUC__
__extension__ typedef long long int int64_t;
#  endif
# endif

/* But these were defined by ISO C without the first `_'.  */
typedef	unsigned char u_int8_t;
typedef	unsigned short int u_int16_t;
typedef	unsigned int u_int32_t;
# ifdef __GNUC__
__extension__ typedef unsigned long long int u_int64_t;
# endif

typedef int register_t;

#else

/* For GCC 2.7 and later, we can use specific type-size attributes.  */
# define __intN_t(N, MODE) \
  typedef int int##N##_t __attribute__ ((__mode__ (MODE)))
# define __u_intN_t(N, MODE) \
  typedef unsigned int u_int##N##_t __attribute__ ((__mode__ (MODE)))

# ifndef __int8_t_defined
#  define __int8_t_defined
__intN_t (8, __QI__);
__intN_t (16, __HI__);
__intN_t (32, __SI__);
__intN_t (64, __DI__);
# endif

__u_intN_t (8, __QI__);
__u_intN_t (16, __HI__);
__u_intN_t (32, __SI__);
__u_intN_t (64, __DI__);

typedef int register_t __attribute__ ((__mode__ (__word__)));


/* Some code from BIND tests this macro to see if the types above are
   defined.  */
#endif
#define __BIT_TYPES_DEFINED__	1


#ifdef	__USE_BSD
/* In BSD <sys/types.h> is expected to define BYTE_ORDER.  */
# include <endian.h>

/* It also defines `fd_set' and the FD_* macros for `select'.  */
# include <sys/select.h>

/* BSD defines these symbols, so we follow.  */
# include <sys/sysmacros.h>
#endif /* Use BSD.  */


/* Types from the Large File Support interface.  */
#ifndef __USE_FILE_OFFSET64
typedef __blkcnt_t blkcnt_t;	 /* Type to count number of disk blocks.  */
typedef __fsblkcnt_t fsblkcnt_t; /* Type to count file system blocks.  */
typedef __fsfilcnt_t fsfilcnt_t; /* Type to count file system inodes.  */
#else
typedef __blkcnt64_t blkcnt_t;	   /* Type to count number of disk blocks.  */
typedef __fsblkcnt64_t fsblkcnt_t; /* Type to count file system blocks.  */
typedef __fsfilcnt64_t fsfilcnt_t; /* Type to count file system inodes.  */
#endif

#ifdef __USE_LARGEFILE64
typedef __blkcnt64_t blkcnt64_t;     /* Type to count number of disk blocks. */
typedef __fsblkcnt64_t fsblkcnt64_t; /* Type to count file system blocks.  */
typedef __fsfilcnt64_t fsfilcnt64_t; /* Type to count file system inodes.  */
#endif

__END_DECLS

#endif /* sys/types.h */
