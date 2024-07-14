/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef STDDEF_H
#define STDDEF_H

#include <commonlib/helpers.h>

typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __SIZE_TYPE__ size_t;
#define SIZE_MAX __SIZE_MAX__
/* There is a GCC macro for a size_t type, but not
 * for a ssize_t type. Below construct tricks GCC
 * into making __SIZE_TYPE__ signed.
 */
#define unsigned signed
typedef __SIZE_TYPE__ ssize_t;
#undef unsigned

typedef __WCHAR_TYPE__ wchar_t;
typedef __WINT_TYPE__ wint_t;

#if __STDC_VERSION__ >= 202300L
#define NULL nullptr
#else
#define nullptr ((void *)0)
#define NULL ((void *)0)
#endif

/* The devicetree data structures are only mutable in ramstage. All other
   stages have a constant devicetree. */
#if !ENV_PAYLOAD_LOADER
#define DEVTREE_EARLY 1
#else
#define DEVTREE_EARLY 0
#endif

#if DEVTREE_EARLY
#define DEVTREE_CONST const
#else
#define DEVTREE_CONST
#endif

/* Provide a pointer to address 0 that thwarts any "accessing this is
 * undefined behaviour and do whatever" trickery in compilers.
 * Use when you _really_ need to read32(zeroptr) (ie. read address 0).
 */
extern char zeroptr[];

#endif /* STDDEF_H */
