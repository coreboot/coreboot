#ifndef STRING_H
#define STRING_H

#include <stddef.h>

#include <console/vtxprintf.h>

/* Stringify a token */
#ifndef STRINGIFY
#define _STRINGIFY(x)	#x
#define STRINGIFY(x)	_STRINGIFY(x)
#endif

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memchr(const void *s, int c, size_t n);
int snprintf(char *buf, size_t size, const char *fmt, ...);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
char *strdup(const char *s);
char *strconcat(const char *s1, const char *s2);
size_t strnlen(const char *src, size_t max);
size_t strlen(const char *src);
char *strchr(const char *s, int c);
char *strncpy(char *to, const char *from, int count);
char *strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int maxlen);
int strspn(const char *str, const char *spn);
int strcspn(const char *str, const char *spn);
long atol(const char *str);

/**
 * Find a character in a string.
 *
 * @param s The string.
 * @param c The character.
 * @return A pointer to the last occurrence of the character in the
 * string, or NULL if the character was not encountered within the string.
 */
char *strrchr(const char *s, int c);

/*
 * Parses an unsigned integer and moves the input pointer forward to the first
 * character that's not a valid digit. s and *s must not be NULL. Result
 * undefined if it overruns the return type size.
 */
unsigned int skip_atoi(char **s);

#endif /* STRING_H */
