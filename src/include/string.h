// yes, linux has fancy ones. We don't care. This stuff gets used 
// hardly at all. And the pain of including those files is just too high.

//extern inline void strcpy(char *dst, char *src) {while (*src) *dst++ = *src++;}

//extern inline int strlen(char *src) { int i = 0; while (*src++) i++; return i;}

#if 1
extern inline int strnlen(const char *src, int max) { 
  int i = 0; 
  if (max<0) {
    while (*src++) 
      i++; 
    return i;
  }
  else {
    while ((*src++) && (i < max)) 
      i++; 
    return i;
  }
}
#else
static inline size_t strnlen(const char * s, size_t count)
{
int d0;
register int __res;
__asm__ __volatile__(
        "movl %2,%0\n\t"
        "jmp 2f\n"
        "1:\tcmpb $0,(%0)\n\t"
        "je 3f\n\t"
        "incl %0\n"
        "2:\tdecl %1\n\t"
        "cmpl $-1,%1\n\t"
        "jne 1b\n"
        "3:\tsubl %2,%0"
        :"=a" (__res), "=&d" (d0)
        :"c" (s),"1" (count));
return __res;
}
#endif
