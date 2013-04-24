/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google, Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdint.h>
#include <bootstate.h>
#include <cbmem.h>

typedef struct file {
	uint32_t magic;
	struct file *next;
	char *filename;
	char *data;
	int offset;
	int len;
} FILE;

#define SEEK_SET       0       /* Seek from beginning of file.  */

#define DIR_SEPARATOR '/'
#define IS_DIR_SEPARATOR(ch) ((ch) == DIR_SEPARATOR)
#define HAS_DRIVE_SPEC(f) (0)

#define COVERAGE_SIZE (32*1024)

static FILE *current_file = NULL;
static FILE *previous_file = NULL;

static FILE *fopen(const char *path, const char *mode)
{
#if CONFIG_DEBUG_COVERAGE
	printk(BIOS_DEBUG, "fopen %s with mode %s\n",
		path, mode);
#endif
	if (!current_file) {
		current_file = cbmem_add(CBMEM_ID_COVERAGE, 32*1024);
	} else {
		previous_file = current_file;
		current_file = (FILE *)(ALIGN(((unsigned long)previous_file->data + previous_file->len), 16));
	}

	// TODO check if we're at the end of the CBMEM region (ENOMEM)
	if (current_file) {
		current_file->magic = 0x584d4153;
		current_file->next = NULL;
		if (previous_file)
			previous_file->next = current_file;
		current_file->filename = (char *)&current_file[1];
		strcpy(current_file->filename, path);
		current_file->data = (char *)ALIGN(((unsigned long)current_file->filename + strlen(path) + 1), 16);
		current_file->offset = 0;
		current_file->len = 0;
	}

	return current_file;
}

static int fclose(FILE *stream)
{
#if CONFIG_DEBUG_COVERAGE
	printk(BIOS_DEBUG, "fclose %s\n", stream->filename);
#endif
	return 0;
}

static int fseek(FILE *stream, long offset, int whence)
{
	/* fseek should only be called with offset==0 and whence==SEEK_SET
	 * to a freshly opened file. */
	gcc_assert (offset == 0 && whence == SEEK_SET);
#if CONFIG_DEBUG_COVERAGE
	printk(BIOS_DEBUG, "fseek %s offset=%ld whence=%d\n",
		stream->filename, offset, whence);
#endif
	return 0;
}

static long ftell(FILE *stream)
{
	/* ftell should currently not be called */
	gcc_assert(0);
#if CONFIG_DEBUG_COVERAGE
	printk(BIOS_DEBUG, "ftell %s\n", stream->filename);
#endif
	return 0;
}

static size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
#if CONFIG_DEBUG_COVERAGE
	printk(BIOS_DEBUG, "fread: ptr=%p size=%zd nmemb=%zd FILE*=%p\n",
		ptr, size, nmemb, stream);
#endif
	return 0;
}

static size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
#if CONFIG_DEBUG_COVERAGE
	printk(BIOS_DEBUG, "fwrite: %zd * 0x%zd bytes to file %s\n",
		nmemb, size, stream->filename);
#endif
	// TODO check if file is last opened file and fail otherwise.

	memcpy(stream->data + stream->offset, ptr, size * nmemb);
	stream->len += (nmemb * size) - (stream->len - stream->offset);
	stream->offset += nmemb * size;
	return nmemb;
}

static void setbuf(FILE *stream, char *buf)
{
	gcc_assert(buf == 0);
}

static void coverage_init(void *unused)
{
	extern long __CTOR_LIST__;
	typedef void (*func_ptr)(void) ;
	func_ptr *ctor = (func_ptr*) &__CTOR_LIST__;
	if (ctor == NULL)
		return;

	for ( ; *ctor != (func_ptr) 0; ctor++) {
		(*ctor)();
	}
}

void __gcov_flush(void);
static void coverage_exit(void *unused)
{
#if CONFIG_DEBUG_COVERAGE
	printk(BIOS_DEBUG, "Syncing coverage data.\n");
#endif
	__gcov_flush();
}

BOOT_STATE_INIT_ENTRIES(gcov_bscb) = {
	BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, coverage_init, NULL),
	BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, coverage_exit, NULL),
	BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, coverage_exit, NULL),
};
