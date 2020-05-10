/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1

#define BUG() assert(0)

#define COMMON_RESULT_START 0x10000
#define LAYOUT_RESULT_START 0x20000
#define CMOS_RESULT_START 0x30000
#define CMOS_OP_RESULT_START 0x40000

#define OK 0			/* 0 is used universally to indicate success. */

#define LINE_EOF (COMMON_RESULT_START + 0)
#define LINE_TOO_LONG (COMMON_RESULT_START + 1)

#ifdef __MINGW32__
#define PROT_READ 1
#define PROT_WRITE 2
#define MAP_PRIVATE 1

void *win32_mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);
int win32_munmap(void *start, size_t length);

#define mmap win32_mmap
#define munmap win32_munmap

#define MAP_FAILED ((void *)-1)
#define MAP_SHARED 1
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define noreturn _Noreturn
#else
#define noreturn
#endif

/* basename of this program, as reported by argv[0] */
extern const char prog_name[];

/* version of this program */
extern const char prog_version[];

int get_line_from_file(FILE * f, char line[], int line_buf_size);
noreturn void out_of_memory(void);
void usage(FILE * outfile);

#endif				/* COMMON_H */
