/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef STRING_H
#define STRING_H

#include <commonlib/bsd/string.h>
#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memchr(const void *s, int c, size_t n);
char *strdup(const char *s);
char *strconcat(const char *s1, const char *s2);
char *strchr(const char *s, int c);
char *strncpy(char *to, const char *from, size_t count);
char *strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t maxlen);
size_t strspn(const char *str, const char *spn);
size_t strcspn(const char *str, const char *spn);
char *strstr(const char *haystack, const char *needle);
char *strtok_r(char *str, const char *delim, char **ptr);
char *strtok(char *str, const char *delim);
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

#endif /* STRING_H */
