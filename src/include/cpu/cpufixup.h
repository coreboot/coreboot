#ifndef CPU_CPUFIXUP_H
#define CPU_CPUFIXUP_H

struct mem_range;

#include <cpu/k8/cpufixup.h>
#include <cpu/k7/cpufixup.h>
#include <cpu/p6/cpufixup.h>

#if CPU_FIXUP == 1 
#  if defined(k8)
#    define cpufixup(mem) k8_cpufixup(mem)
#  elif defined(k7)
#    define cpufixup(mem) k7_cpufixup(mem)
#  elif defined(i786)
#    define cpufixup(mem) i786_cpufixup(mem)
#  elif defined(i686)
#    define cpufixup(mem) p6_cpufixup(mem)
#  endif
#else
#  define cpufixup(mem) do {} while(0)
#endif

#endif /* CPU_CPUFIXUP_H */
