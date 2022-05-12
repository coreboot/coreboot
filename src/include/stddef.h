#ifndef STDDEF_H
#define STDDEF_H

#include <commonlib/helpers.h>

typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __SIZE_TYPE__ size_t;
/* There is a GCC macro for a size_t type, but not
 * for a ssize_t type. Below construct tricks GCC
 * into making __SIZE_TYPE__ signed.
 */
#define unsigned signed
typedef __SIZE_TYPE__ ssize_t;
#undef unsigned

typedef __WCHAR_TYPE__ wchar_t;
typedef __WINT_TYPE__ wint_t;

#define NULL ((void *)0)

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

#if ENV_HAS_DATA_SECTION
#define MAYBE_STATIC_NONZERO static
#else
#define MAYBE_STATIC_NONZERO
#endif

/* Provide a pointer to address 0 that thwarts any "accessing this is
 * undefined behaviour and do whatever" trickery in compilers.
 * Use when you _really_ need to read32(zeroptr) (ie. read address 0).
 */
extern char zeroptr[];

#endif /* STDDEF_H */
