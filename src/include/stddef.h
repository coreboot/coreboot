#ifndef STDDEF_H
#define STDDEF_H

typedef long ptrdiff_t;
#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
typedef __SIZE_TYPE__ size_t;
typedef long ssize_t;

typedef int wchar_t;
typedef unsigned int wint_t;

#define NULL ((void *)0)

/* Standard units. */
#define KiB (1<<10)
#define MiB (1<<20)
#define GiB (1<<30)
/* Could we ever run into this one? I hope we get this much memory! */
#define TiB (1<<40)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#ifdef __PRE_RAM__
#define ROMSTAGE_CONST const
#else
#define ROMSTAGE_CONST
#endif

#endif /* STDDEF_H */
