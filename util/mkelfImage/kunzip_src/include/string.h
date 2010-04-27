#ifndef STRING_H
#define STRING_H

#include <stddef.h>

// yes, linux has fancy ones. We don't care. This stuff gets used
// hardly at all. And the pain of including those files is just too high.

//extern inline void strcpy(char *dst, char *src) {while (*src) *dst++ = *src++;}

//extern inline int strlen(char *src) { int i = 0; while (*src++) i++; return i;}

static inline size_t strnlen(const char *src, size_t max) {
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

extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memset(void *s, int c, size_t n);
extern int memcmp(const void *s1, const void *s2, size_t n);

#endif /* STRING_H */
