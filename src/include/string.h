// yes, linux has fancy ones. We don't care. This stuff gets used 
// hardly at all. And the pain of including those files is just too high.

//extern inline void strcpy(char *dst, char *src) {while (*src) *dst++ = *src++;}

//extern inline int strlen(char *src) { int i = 0; while (*src++) i++; return i;}
extern inline int strnlen(const char *src, int max) { 
  int i = 0; 
  while ((*src++) && (i < max)) 
    i++; 
 return i;
}

