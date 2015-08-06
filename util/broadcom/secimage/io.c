/*
 * Copyright (C) 2015 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <string.h>
#include "secimage.h"

/*----------------------------------------------------------------------
 * Name    : ReadBinaryFile
 * Purpose : Read some data from file of raw binary
 * Input   : fname : file to be read
 *           buf : buffer which is the data desitnation
 *           maxlen : maiximum length of data to be read
 * Output  : none
 *---------------------------------------------------------------------*/
int ReadBinaryFile(char *fname, uint8_t *buf, int maxlen)
{
	FILE *fp = NULL;
	int len = 0;

	fp = fopen(fname, "rb");
	if (fp == NULL)
		return 0;
	printf("fname=%s, len=%d\n", fname, maxlen);
	len = fread(buf, 1, maxlen, fp);
	fclose(fp);

	return len;
}

/*----------------------------------------------------------------------
 * Name    : FileSizeGet
 * Purpose : Return the size of the file
 * Input   : file: FILE * to the file to be processed
 * Output  : none
 *---------------------------------------------------------------------*/
size_t FileSizeGet(FILE *file)
{
	long length;

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	rewind(file);
	return (size_t)length;
}

/*----------------------------------------------------------------------
 * Name    : DataRead
 * Purpose : Read all the data from a file
 * Input   : filename : file to be read
 *           buf : buffer which is the data destination
 *           length : length of data to be read
 * Output  : none
 *---------------------------------------------------------------------*/
int DataRead(char *filename, uint8_t *buf, int *length)
{
	FILE *file;
	int len = *length;

	file = fopen(filename, "rb");
	if (file == NULL) {
		printf("Unable to open file: %s\n", filename);
		return -1;
	}
	len = FileSizeGet(file);
	if (len < *length)
		*length = len;
	else
		/* Do not exceed the maximum length of the buffer */
		len = *length;
	if (fread((uint8_t *)buf, 1, len, file) != len) {
		printf("Error reading data (%d bytes) from file: %s\n",
		       len, filename);
		return -1;
	}
	fclose(file);
	return 0;
}

/*----------------------------------------------------------------------
 * Name    : DataWrite
 * Purpose : Write some binary data to a file
 * Input   : filename : file to be written
 *           buf : buffer which is the data source
 *           length : length of data to be written
 * Output  : none
 *---------------------------------------------------------------------*/
int DataWrite(char *filename, char *buf, int length)
{
	FILE *file;

	file = fopen(filename, "wb");
	if (file == NULL) {
		printf("Unable to open output file %s\n", filename);
		return -1;
	}
	if (fwrite(buf, 1, length, file) < length) {
		printf("Unable to write %d bytes to output file %s (0x%X).\n",
		       length, filename, ferror(file));
		fclose(file);
		return -1;
	}

	fflush(file);
	fclose(file);
	return 0;
}
