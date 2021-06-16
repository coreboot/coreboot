#ifndef _WCHAR_H
#define _WCHAR_H

#include <stddef.h>

#ifndef __WINT_TYPE__
# define __WINT_TYPE__ unsigned int
#endif
typedef __WINT_TYPE__ wint_t;

#endif
