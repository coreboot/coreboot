/* Comment this out unless you are debugging under linux */
#define EMULATE
#undef EMULATE

#ifdef EMULATE
#include <unistd.h>
#else
typedef unsigned long size_t;
#endif

#ifdef EMULATE
  #define MEMSIZE 2*1024*1024       /* Make a 2MB fake memory space */
  char memimage[MEMSIZE];          /* Make a 2MB fake memory space */
#else
  #define memimage 0x0            /* Ignore memimage */
#endif

#define KERNEL_START  (0x100000 + memimage) /* Put our copy of linux here   */

#if 0
#define ZIP_START     (0x30000  + memimage) /* The zip file starts here     */
#define ZIP_SIZE      262164                /* linux.gz size (we ought to   */
#endif

/* with flash, it's a bunch of 64k segments. */
#define ZIP_START (0xfff40000)
#define ZIP_SIZE (0x10000)

#ifdef EMULATE
char input_array[0x100000];
#undef ZIP_START
#define ZIP_START (input_array + 0x40000)
#endif
                                            /*   make this dynamic)         */

