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

#ifndef _SECIMAGE_H_
#define _SECIMAGE_H_

#include <stdint.h>
#include <sys/types.h>

#define FIELD1_OFFSET 16
#define FIELD2_OFFSET 18
#define FIELD3_OFFSET 20
#define FIELD4_OFFSET 36
#define FIELD5_OFFSET 40
#define FIELD6_OFFSET 44
#define FIELD7_OFFSET 48
#define FIELD8_OFFSET 50
#define FIELD9_OFFSET 60

typedef struct Header_t {
	uint32_t Tag;
	uint32_t Length;
	uint32_t Reserved;
	uint32_t crc;
} HEADER;

int DataWrite(char *filename, char *buf, int length);
int DataRead(char *filename, uint8_t *buf, int *length);
int AppendHMACSignature(uint8_t *data, uint32_t length, char *filename,
			uint32_t offset);
int ReadBinaryFile(char *fname, uint8_t *buf, int maxlen);
void FillHeaderFromConfigFile(char *h, char *ConfigFileName);

#endif /* _SECIMAGE_H_ */
