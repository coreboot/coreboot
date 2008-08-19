/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _LIBPAYLOAD_H
#define _LIBPAYLOAD_H

#include <stddef.h>
#include <arch/types.h>
#include <arch/io.h>
#include <sysinfo.h>
#include <stdarg.h>
#include <lar.h>
#include <pci.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define LITTLE_ENDIAN	1234
#define BIG_ENDIAN	4321

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define RAND_MAX 0x7fffffff

/* Payload information parameters - these are used to pass information
 * to the entity loading the payload
 * Usage:  PAYLOAD_INFO(key, value)
 * Example:  PAYLOAD_INFO(name, "CoreInfo!")
 */

#define _pstruct(key) __pinfo_ ##key
#define PAYLOAD_INFO(key, value)                                        \
static const char _pstruct(key)[]                                        \
  __attribute__((__used__))                                              \
  __attribute__((section(".note.pinfo"),unused)) = #key "=" value

/* Some NVRAM byte definitions */
#define NVRAM_RTC_SECONDS        0
#define NVRAM_RTC_MINUTES        2
#define NVRAM_RTC_HOURS          4
#define NVRAM_RTC_DAY            7
#define NVRAM_RTC_MONTH          8
#define NVRAM_RTC_YEAR           9
#define NVRAM_RTC_FREQ_SELECT    10
#define  NVRAM_RTC_UIP           0x80

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

/* drivers/nvram.c */
u8 nvram_read(u8 addr);
void nvram_write(u8 val, u8 addr);
int nvram_updating(void);
void rtc_read_clock(struct tm *tm);

/* drivers/keyboard.c */
void keyboard_init(void);
int keyboard_havechar(void);
unsigned char keyboard_get_scancode(void);
int keyboard_getchar(void);

/* drivers/serial.c */
void serial_init(void);
void serial_putchar(unsigned char c);
int serial_havechar(void);
int serial_getchar(void);

void serial_clear(void);
void serial_start_bold(void);
void serial_end_bold(void);
void serial_start_altcharset(void);
void serial_end_altcharset(void);
void serial_set_cursor(int y, int x);

/* drivers/speaker.c */
void speaker_enable(u16 freq);
void speaker_disable(void);
void speaker_tone(u16 freq, unsigned int duration);

/* video/video.c */
int video_console_init(void);
void video_console_putchar(unsigned int ch);
void video_console_putc(u8 row, u8 col, unsigned int ch);
void video_console_clear(void);
void video_console_cursor_enable(int state);
void video_console_get_cursor(unsigned int *x, unsigned int *y, unsigned int *en);
void video_console_set_cursor(unsigned int cursorx, unsigned int cursory);

/* drivers/option.c */
int get_option(void *dest, char *name);

/* libc/console.c */
void console_init(void);
int putchar(int c);
int puts(const char *s);
int havekey(void);
int getchar(void);
int getchar_timeout(int *ms);

extern int last_putchar;

#define havechar havekey

/* libc/ctype.c */
int isalnum(int c);
int isalpha(int c);
int isascii(int c);
int isblank(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);
int tolower(int c);
int toupper(int c);

/* libc/ipchecksum.c */
unsigned short ipchksum(const void *ptr, unsigned long nbytes);

/* libc/malloc.c */
void free(void *ptr);
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

/* libc/exec.c */
int exec(long addr, int argc, char **argv);

/* libc/lib.c */
int bcd2dec(int b);
int dec2bcd(int d);
int abs(int j);
long int labs(long int j);
long long int llabs(long long int j);
u8 bin2hex(u8 b);
u8 hex2bin(u8 h);

/* libc/memory.c */
void *memset(void *s, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t len);

/* libc/printf.c */
int snprintf(char *str, size_t size, const char *fmt, ...);
int sprintf(char *str, const char *fmt, ...);
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap);
int vsprintf(char *str, const char *fmt, va_list ap);
int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list ap);

/* libc/rand.c */
int rand_r(unsigned int *seed);
int rand(void);
void srand(unsigned int seed);

/* libc/sha1.c */
#define SHA1_BLOCK_LENGTH	64
#define SHA1_DIGEST_LENGTH	20
typedef struct {
	u32 state[5];
	u64 count;
	u8 buffer[SHA1_BLOCK_LENGTH];
} SHA1_CTX;
void SHA1Init(SHA1_CTX *context);
void SHA1Transform(u32 state[5], const u8 buffer[SHA1_BLOCK_LENGTH]);
void SHA1Update(SHA1_CTX *context, const u8 *data, size_t len);
void SHA1Final(u8 digest[SHA1_DIGEST_LENGTH], SHA1_CTX *context);
u8 *sha1(const u8 *data, size_t len, u8 *buf);

/* libc/string.c */
size_t strnlen(const char *str, size_t maxlen);
size_t strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int maxlen);
char *strncpy(char *d, const char *s, int n);
char *strcpy(char *d, const char *s);
char *strncat(char *d, const char *s, int n);
char *strchr(const char *s, int c);
char *strdup(const char *s);
char *strstr(const char *h, const char *n);

/* libc/time.c */

struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

int gettimeofday(struct timeval *tv, void *tz);

/* libc/lar.c */

struct LAR {
	void * start;
	int cindex;
	int count;
	int alloc;
	int eof;
	void **headers;
};

struct larent {
	u8 name[LAR_MAX_PATHLEN];
};

struct larstat {
	u32 len;
	u32 reallen;
	u32 checksum;
	u32 compchecksum;
	u32 offset;
	u32 compression;
	u64 entry;
	u64 loadaddress;
};

struct LFILE {
	struct LAR *lar;
	struct lar_header *header;
	u32 size;
	void *start;
	u32 offset;
};

struct LAR *openlar(void *addr);
int closelar(struct LAR *lar);
struct larent *readlar(struct LAR *lar);
void rewindlar(struct LAR *lar);
int larstat(struct LAR *lar, const char *path, struct larstat *buf);
void *larfptr(struct LAR *lar, const char *filename);
int lfverify(struct LAR *lar, const char *filename);
struct LFILE * lfopen(struct LAR *lar, const char *filename);
int lfread(void *ptr, size_t size, size_t nmemb, struct LFILE *stream);

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int lfseek(struct LFILE *stream, long offset, int whence);
int lfclose(struct LFILE *file);

/* i386/coreboot.c */
int get_coreboot_info(struct sysinfo_t *info);

/* i386/sysinfo.c */
void lib_get_sysinfo(void);

/* i386/timer.c */
/* Timer functions - defined by each architecture. */
unsigned int get_cpu_speed(void);
void ndelay(unsigned int n);
void udelay(unsigned int n);
void mdelay(unsigned int n);
void delay(unsigned int n);

/* i386/util.S */
#define abort() halt()
void halt(void) __attribute__ ((noreturn));

#endif
