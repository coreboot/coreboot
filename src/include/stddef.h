#ifndef STDDEF_H
#define STDDEF_H

#include <commonlib/helpers.h>
#include <rules.h>

typedef long ptrdiff_t;
#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
typedef __SIZE_TYPE__ size_t;
/* There is a GCC macro for a size_t type, but not
 * for a ssize_t type. Below construct tricks GCC
 * into making __SIZE_TYPE__ signed.
 */
#define unsigned signed
typedef __SIZE_TYPE__ ssize_t;
#undef unsigned

typedef int wchar_t;
typedef unsigned int wint_t;

#define NULL ((void *)0)

/* The devicetree data structures are only mutable in ramstage. All other
   stages have a constant devicetree. */
#if !ENV_RAMSTAGE
#define DEVTREE_EARLY 1
#else
#define DEVTREE_EARLY 0
#endif

#if DEVTREE_EARLY
#define DEVTREE_CONST const
#else
#define DEVTREE_CONST
#endif

/* Work around non-writable data segment in execute-in-place romstage on x86. */
#if defined(__PRE_RAM__) && CONFIG_ARCH_X86
#define MAYBE_STATIC
#else
#define MAYBE_STATIC static
#endif

#ifndef __ROMCC__
/* Provide a pointer to address 0 that thwarts any "accessing this is
 * undefined behaviour and do whatever" trickery in compilers.
 * Use when you _really_ need to read32(zeroptr) (ie. read address 0).
 */
extern char zeroptr[];
#endif

#endif /* STDDEF_H */
