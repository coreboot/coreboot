#ifndef CPU_CPUFIXUP_H
#define CPU_CPUFIXUP_H

struct mem_range;

#include <cpu/k8/cpufixup.h>
#include <cpu/k7/cpufixup.h>
#include <cpu/p6/cpufixup.h>

#if CPU_FIXUP == 1 
#  if (k8==1)
#warning "====>Temporary notice that we are using k8 cpufixup"
#    define cpufixup(mem) k8_cpufixup(mem)
#  elif (k7==1)
#warning "====>Temporary notice that we are using k7 cpufixup"
#    define cpufixup(mem) k7_cpufixup(mem)
#  elif (i786==1)
#warning "====>Temporary notice that we are using i786 cpufixup"
#    define cpufixup(mem) i786_cpufixup(mem)
#  elif (i686==1)
#warning "====>Temporary notice that we are using i686 cpufixup"
#    define cpufixup(mem) p6_cpufixup(mem)
#  endif
#else
#warning "====>YOU DID NOT DEFINE ONE OF: k8, k7, i786, i686"
#  define cpufixup(mem) do {} while(0)
#endif

#endif /* CPU_CPUFIXUP_H */
