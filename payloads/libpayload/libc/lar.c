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

#include <endian.h>
#include <libpayload.h>

#define ROM_RESET_VECTOR 0xFFFFFFF0

static void * next_header(void * cur)
{
	struct lar_header *header = (struct lar_header *) cur;
	int offset = ((ntohl(header->offset) + ntohl(header->len)) + 15) &
		0xFFFFFFF0;

	return (void *) (cur + offset);
}

static struct lar_header *lar_get_header(struct LAR *lar, int index)
{
	int i;

	if (index < lar->count)
		return (struct lar_header *) lar->headers[index];

	if (lar->eof && index >= lar->eof)
		return NULL;

	for(i = lar->count; i <= index; i++) {
		void *next = (i == 0) ?
			lar->start : next_header(lar->headers[i - 1]);

		if (strncmp((const char *) next, LAR_MAGIC, 8)) {
			lar->eof = lar->count;
			return NULL;
		}

		if (lar->count == lar->alloc) {
			void *tmp = realloc(lar->headers,
					    (lar->alloc + 16) * sizeof(void *));

			if (tmp == NULL)
				return NULL;

			lar->headers = tmp;
			lar->alloc += 16;
		}

		lar->headers[lar->count++] = next;
	}

	return (struct lar_header *) lar->headers[index];
}


/**
 * Open a LAR stream
 *
 * @param addr The address in memory where the LAR is located.
 * Use NULL to specify the boot LAR
 * @return a pointer to the LAR stream
 */

struct LAR *openlar(void *addr)
{
	struct LAR *lar;

	/* If the address is null, then figure out the start of the
	   boot LAR */

	if (addr == NULL) {
		u32 size = *((u32 *) (ROM_RESET_VECTOR + 4));
		addr = (void *) ((ROM_RESET_VECTOR  + 16) - size);
	}

	/* Check the magic to make sure this is a LAR */
	if (strncmp((const char *) addr, LAR_MAGIC, strlen(LAR_MAGIC)))
		return NULL;

	lar = calloc(sizeof(struct LAR), 1);

	if (!lar)
		return NULL;

	lar->start = addr;

	/* Preallocate 16 slots in the cache - this saves wear and
	 * tear on the heap */

	lar->headers = malloc(16 * sizeof(void *));

	if (!lar->headers)
		return NULL;

	lar->alloc = 16;
	lar->count = lar->eof = 0;
	lar->cindex = 0;

	return lar;
}

/**
 * Close a LAR stream
 *
 * @param lar A pointer to the LAR stream
 * @return Return 0 on success, -1 on error
 */

int closelar(struct LAR *lar)
{
	if (!lar)
		return 0;

	if (lar->headers)
		free(lar->headers);

	free(lar);

	return 0;
}

/**
 * Read an entry from the LAR
 *
 * @param lar A pointer to the LAR stream
 * @return A pointer to a larent structure
           representing the next file in the LAR
 */

struct larent *readlar(struct LAR *lar)
{
	static struct larent _larent;
	struct lar_header *header;
	int nlen;

	if (!lar)
		return NULL;

	header = lar_get_header(lar, lar->cindex);

	if (header == NULL)
		return NULL;

	nlen = ntohl(header->offset) - sizeof(*header);

	if (nlen > LAR_MAX_PATHLEN - 1)
		nlen = LAR_MAX_PATHLEN - 1;

	memcpy((void *) _larent.name, ((char *) header + sizeof(*header)),
		nlen);

	_larent.name[nlen] = 0;

	lar->cindex++;

	return (struct larent *) &_larent;
}

void rewindlar(struct LAR *lar)
{
	if (lar != NULL)
		lar->cindex = 0;
}

static struct lar_header *get_header_by_name(struct LAR *lar, const char *name)
{
	struct lar_header *header;
	int i;

	for(i = 0; ; i++) {
		header = lar_get_header(lar, i);

		if (header == NULL)
			return NULL;

		if (!strcmp(name, ((char *) header + sizeof(*header))))
			return header;
	}
}

int larstat(struct LAR *lar, const char *path, struct larstat *buf)
{
	struct lar_header *header = get_header_by_name(lar, path);

	if (header == NULL || buf == NULL)
		return -1;

	buf->len = ntohl(header->len);
	buf->reallen = ntohl(header->reallen);
	buf->checksum = ntohl(header->checksum);
	buf->compchecksum = ntohl(header->compchecksum);
	buf->compression = ntohl(header->compression);
	buf->entry = ntohll(header->entry);
	buf->loadaddress = ntohll(header->loadaddress);
	buf->offset = ((u32) header - (u32) lar->start) + ntohl(header->offset);

	return 0;
}

void * larfptr(struct LAR *lar, const char *filename)
{
	struct lar_header *header = get_header_by_name(lar, filename);

	if (header == NULL)
		return NULL;

	return (void *) ((u8 *) header + ntohl(header->offset));
}

/**
 * Verify the checksum on a particular LAR entry
 *
 * @param lar A pointer to the LAR stream
 * @param filename The lar entry to verify
 * @return Return 1 if the entry is valid, 0 if it is not, or -1
 * on error
 */

int lfverify(struct LAR *lar, const char *filename)
{
	struct lar_header *header = get_header_by_name(lar, filename);

	u8 *ptr = (u8 *) header;
	int len = ntohl(header->len) + ntohl(header->offset);
	int offset;
	u32 csum = 0;

	if (header == NULL)
		return -1;

	/* The checksum needs to be calulated on entire data section,
	 * including any padding for the 16 byte alignment (which should
	 * be zeros
	 */

	len = (len + 15) & 0xFFFFFFF0;

	for(offset = 0; offset < len; offset += 4) {
		csum += *((u32 *) (ptr + offset));
	}

	return (csum == 0xFFFFFFFF) ? 1 : 0;
}

struct LFILE * lfopen(struct LAR *lar, const char *filename)
{
	struct LFILE *file;
	struct lar_header *header = get_header_by_name(lar, filename);

	if (header == NULL)
		return NULL;

	/* FIXME: What other validations do we want to do on the file here? */

	file = malloc(sizeof(struct LFILE));

	if (file == NULL)
		return NULL;

	file->lar = lar;
	file->header = header;
	file->size = ntohl(header->len);
	file->start = ((u8 *) header + ntohl(header->offset));
	file->offset = 0;

	return file;
}

void *lfmap(struct LFILE *file, int offset)
{
	if (file == NULL)
		return (void *) -1;

	if (offset > file->size)
		return (void *) -1;

	return (void *) (file->start + offset);
};

int lfread(void *ptr, size_t size, size_t nmemb, struct LFILE *stream)
{
	size_t tsize, actual;
	size_t remain = stream->size - stream->offset;

	if (!stream || !remain)
		return 0;

	tsize = (size * nmemb);
	actual = (tsize > remain) ? remain : tsize;

	memcpy(ptr, (void *) (stream->start + stream->offset), actual);
	stream->offset += actual;

	return actual;
}

int lfseek(struct LFILE *file, long offset, int whence)
{
	int o = file->offset;

	switch(whence) {
	case SEEK_SET:
		o = offset;
		break;
	case SEEK_CUR:
		o += offset;
		break;

	case SEEK_END:
		return -1;
	}

	if (o < 0 || o > file->size)
		return -1;

	file->offset = o;
	return file->offset;
}

int lfclose(struct LFILE *file)
{
	if (file)
		free(file);
	return 0;
}
