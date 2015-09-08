#ifndef STDDEF_H
#define STDDEF_H

#include <commonlib/helpers.h>

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

#ifdef __PRE_RAM__
#define ROMSTAGE_CONST const
#else
#define ROMSTAGE_CONST
#endif

/* Work around non-writable data segment in execute-in-place romstage on x86. */
#if defined(__PRE_RAM__) && CONFIG_ARCH_X86
#define MAYBE_STATIC
#else
#define MAYBE_STATIC static
#endif

#endif /* STDDEF_H */
