#ifndef I386_STDDEF_H
#define I386_STDDEF_H

typedef long ptrdiff_t;
#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
typedef __SIZE_TYPE__ size_t;
typedef long ssize_t;

typedef int wchar_t;
typedef unsigned int wint_t;

#define NULL ((void *)0)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#endif /* I386_STDDEF_H */
