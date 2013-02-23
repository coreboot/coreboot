/*
 * This file is part of the bayou project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <arpa/inet.h>

#include "liblar.h"
#include "self.h"

static int lar_compress(struct LAR *lar, int algo, char *src, char *dst,
			int len)
{
	int ret;

	if (!lar->cfuncs[algo])
		return -1;

	lar->cfuncs[algo] (src, len, dst, &ret);
	return ret;
}

static int lar_decompress(struct LAR *lar, int algo, char *src, char *dst,
			  int slen, int dlen)
{
	if (!lar->dfuncs[algo])
		return -1;

	lar->dfuncs[algo] (src, slen, dst, dlen);
	return dlen;
}

static struct LARHeader *lar_get_header(struct LAR *lar, const char *filename)
{
	char *buffer;
	int offset = 0;
	struct LARHeader *lheader = NULL;
	struct lar_header *header;

	printf("Getting %s\n", filename);

	buffer = malloc(sizeof(struct lar_header) + MAX_PATHLEN);

	if (buffer == NULL)
		return NULL;

	while (1) {
		int ret;

		if (lseek(lar->fd, offset, SEEK_SET) == -1)
			goto err;

		ret = read(lar->fd, buffer, sizeof(struct lar_header));

		if (ret <= 0)
			goto err;

		header = (struct lar_header *)buffer;

		if (strncmp(header->magic, MAGIC, sizeof(MAGIC)))
			goto err;

		ret = read(lar->fd, buffer + sizeof(struct lar_header),
			   ntohl(header->offset) - sizeof(struct lar_header));

		if (ret <= 0)
			goto err;

		if (!strcmp(buffer + sizeof(struct lar_header), filename))
			break;

		offset += ntohl(header->offset) +
		    ((ntohl(header->len) + 15) & ~0xF);
	}

	lheader = calloc(sizeof(struct LARHeader), 1);

	if (lheader == NULL)
		goto err;

	lheader->hoffset = offset;
	lheader->offset = offset + ntohl(header->offset);

	lheader->reallen = ntohl(header->reallen);
	lheader->len = ntohl(header->len);

	lheader->loadaddress = ntohl(header->loadaddress);
	lheader->compression = ntohl(header->compression);
	lheader->entry = ntohl(header->entry);
	lheader->checksum = ntohl(header->checksum);

err:
	free(buffer);
	return lheader;
}

static int LAR_AppendBlob(struct LAR *lar, unsigned char *buffer,
			  int len, int rlen, struct LARAttr *attr)
{
	int nlen, nsize, lsize, i;
	struct lar_header *header;
	u8 *lptr;
	u32 csum = 0;

	if (attr == NULL)
		return -1;

	nlen = strlen(attr->name) + 1;

	if (nlen > MAX_PATHLEN - 1)
		nlen = MAX_PATHLEN - 1;

	nsize = (nlen + 15) & ~0xF;

	lsize = sizeof(struct lar_header) + nsize + len;
	lptr = calloc(lsize + 1, 1);

	if (lptr == NULL)
		return -1;

	header = (struct lar_header *)lptr;

	memcpy(header->magic, MAGIC, 8);
	header->reallen = htonl(rlen);
	header->len = htonl(len);
	header->offset = htonl(lsize - len);
	header->loadaddress = htonl(attr->loadaddr);
	header->compression = htonl(attr->compression);
	header->entry = htonl(attr->entry);

	strncpy(((char *)header) + sizeof(struct lar_header), attr->name, nlen);

	for (i = 0; i < sizeof(struct lar_header) + nsize; i += 4)
		csum += *((u32 *) (lptr + i));

	for (i = 0; i < len; i += 4) {
		/*
		 * The checksum needs to include the 16 byte padding at
		 * the end of the data before the next lar header. The
		 * problem is that the padding isn't going to be in the
		 * buffer, and if we try to read off the end of the buffer,
		 * we are just asking for trouble. So account for the
		 * situation where the datalen is not a multiple of four
		 * and get a safe value to add into the checksum.
		 * The rest of the padding will be zero, and can be safely
		 * ignored here.
		 */
		if ((len - i) < 4) {
			u32 val = 0;
			int t;

			for (t = 0; t < (len - i); t++)
				val |= *((u8 *) buffer + (i + t)) << (t * 8);
			csum += val;
		} else
			csum += *((u32 *) (buffer + i));
	}

	header->checksum = (u32) (~0 - csum);

	lseek(lar->fd, 0, SEEK_END);

	/* FIXME: Error check here. */

	write(lar->fd, header, sizeof(struct lar_header) + nsize);
	write(lar->fd, buffer, len);

	/* Add in padding to the next 16 byte boundary. */
	if (lsize & 0xF) {
		int i;
		char null = '\0';

		for (i = lsize & 0xF; i < 0x10; i++)
			write(lar->fd, &null, 1);
	}

	return 0;
}

int LAR_AppendBuffer(struct LAR *lar, unsigned char *buffer, int len,
		     struct LARAttr *attr)
{
	unsigned char *tbuf;
	int rlen, ret = -1;

	if (attr->compression == ALGO_NONE)
		return LAR_AppendBlob(lar, buffer, len, len, attr);

	tbuf = malloc(len);

	if (tbuf == NULL)
		return -1;

	rlen = lar_compress(lar, attr->compression, (char *)buffer,
			    (char *)tbuf, len);

	if (rlen > 0)
		ret = LAR_AppendBlob(lar, tbuf, rlen, len, attr);

	free(tbuf);
	return ret;
}

int LAR_AppendSelf(struct LAR *lar, const char *filename, struct LARAttr *attr)
{
	unsigned char *buffer;
	int len = elf_to_self(filename, &buffer,
			      lar->cfuncs[attr->compression]);
	int ret;

	if (len == -1)
		return -1;

	ret = LAR_AppendBlob(lar, buffer, len, len, attr);
	free(buffer);

	return ret;
}

int LAR_AppendFile(struct LAR *lar, const char *filename, struct LARAttr *attr)
{
	int fd;
	struct stat s;
	char *filep;
	int ret;

	if (iself((char *)filename))
		return LAR_AppendSelf(lar, filename, attr);

	fd = open(filename, O_RDONLY);

	if (fd == -1)
		return -1;

	if (fstat(fd, &s))
		return -1;

	filep = (char *)mmap(0, s.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (filep == MAP_FAILED)
		return -1;

	ret = LAR_AppendBuffer(lar, (unsigned char *)filep, s.st_size, attr);

	munmap(filep, s.st_size);
	return ret;
}

int LAR_DeleteFile(struct LAR *lar, const char *filename)
{
	struct LARHeader *header = lar_get_header(lar, filename);
	int len, ret = -1;
	char *filep, *buffer;

	if (header == NULL)
		return -1;

	buffer = malloc(4096);
	if (buffer == NULL)
		return -1;

	len = header->offset + header->len;

	/* First, map the space and zero it out. */

	filep = (char *)mmap(0, len, PROT_READ, MAP_SHARED, lar->fd,
			     header->hoffset);

	if (filep == MAP_FAILED)
		return -1;

	memset(filep, 0, len);
	munmap(filep, len);

	/* Now move the rest of the LAR into place. */
	/* FIXME: This does not account for the bootblock! */

	int dst = header->hoffset;
	int src = header->hoffset + len;

	while (1) {
		int l, w;

		if (lseek(lar->fd, src, SEEK_SET))
			goto err;

		l = read(lar->fd, buffer, 8192);

		if (l == -1)
			goto err;
		if (l == 0)
			goto err;
		if (lseek(lar->fd, dst, SEEK_SET))
			goto err;

		w = write(lar->fd, buffer, l);

		if (w <= 0)
			goto err;

		dst += w;
		src += w;
	}

	ret = 0;

err:
	free(buffer);
	return ret;
}

void LAR_CloseFile(struct LARFile *file)
{
	if (file != NULL) {
		if (file->buffer != NULL)
			free(file->buffer);
		free(file);
	}
}

struct LARFile *LAR_MapFile(struct LAR *lar, const char *filename)
{
	struct LARFile *file;
	struct LARHeader *header = lar_get_header(lar, filename);
	char *filep;
	int ret;

	if (header == NULL)
		return NULL;

	file = calloc(sizeof(struct LARFile), 1);

	if (file == NULL)
		return NULL;

	file->len = header->reallen;
	file->buffer = calloc(header->reallen, 1);

	if (file->buffer == NULL)
		goto err;

	/*
	 * The offset needs to be a multiple of PAGE_SIZE, so just mmap
	 * from offset 0, its easier then doing the math.
	 */

	filep = mmap(0, header->offset + header->len,
		     PROT_READ, MAP_SHARED, lar->fd, 0);

	if (filep == MAP_FAILED) {
		printf("Map failed: %m\n");
		goto err;
	}

	if (header->compression != ALGO_NONE) {
		ret = lar_decompress(lar, header->compression,
				     filep + header->offset, file->buffer,
				     header->len, header->reallen);
	} else {
		memcpy(file->buffer, filep + header->offset, header->len);
		ret = header->len;
	}

	munmap(filep, header->offset + header->len);

	if (ret == header->reallen)
		return file;

err:
	if (file->buffer)
		free(file->buffer);

	free(file);
	return NULL;
}

int LAR_SetCompressionFuncs(struct LAR *lar, int algo,
			    LAR_CompFunc cfunc, LAR_DecompFunc dfunc)
{

	if (algo >= ALGO_INVALID)
		return -1;

	lar->cfuncs[algo] = cfunc;
	lar->dfuncs[algo] = dfunc;

	return 0;
}

void LAR_Close(struct LAR *lar)
{
	if (lar != NULL) {
		if (lar->fd)
			close(lar->fd);

		free(lar);
	}
}

struct LAR *LAR_Open(const char *filename)
{
	struct LAR *lar = calloc(sizeof(struct LAR), 1);

	if (lar == NULL)
		return NULL;

	lar->fd = open(filename, O_RDWR);

	if (lar->fd > 0)
		return lar;

	free(lar);
	return NULL;
}

struct LAR *LAR_Create(const char *filename)
{
	struct LAR *lar = calloc(sizeof(struct LAR), 1);

	if (lar == NULL)
		return NULL;

	lar->fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

	if (lar->fd > 0)
		return lar;

	free(lar);
	return NULL;
}

void LAR_SetAttrs(struct LARAttr *attrs, char *name, int algo)
{
	if (attrs == NULL)
		return;

	memset(attrs, 0, sizeof(*attrs));
	snprintf(attrs->name, sizeof(attrs->name) - 1, name);
	attrs->compression = algo;
}
