#ifndef CPU_L2_CACHE_H
#define CPU_L2_CACHE_H

#include <cpu/p6/l2_cache.h>

#ifdef CONFIGURE_L2_CACHE
#  if defined(i686)
#    define configure_l2_cache() p6_configure_l2_cache()
#  endif
#else
#  define configure_l2_cache() do {} while(0)
#endif

#endif /* CPU_L2_CACHE_H */
