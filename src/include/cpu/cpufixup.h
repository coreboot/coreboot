#ifndef CPU_CPUFIXUP_H
#define CPU_CPUFIXUP_H

#include <cpu/k7/cpufixup.h>
#include <cpu/p6/cpufixup.h>

#ifdef CPU_FIXUP
#  if defined(k7)
#    define cpufixup(totalram) k7_cpufixup(totalram)
#  elif defined(i686)
#    define cpufixup(totalram) p6_cpufixup(totalram)
#  endif
#else
#  define cpufixup(totalram) do {} while(0)
#endif

#endif /* CPU_CPUFIXUP_H */
