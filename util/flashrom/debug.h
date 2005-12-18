#ifndef __DEBUG_H__
#define __DEBUG_H__ 1

//#define printf_debug(x...) printf(x)

extern int verbose;

#define printf_debug(x...) { if(verbose) printf(x); }

#endif
