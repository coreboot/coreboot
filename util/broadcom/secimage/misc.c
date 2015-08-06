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

static unsigned char filebuffer[2048];

void FillHeaderFromConfigFile(char *h, char *ConfigFileName)
{

	int byte_count = 0;
	char *ptr;
	FILE *fp;
	unsigned int Tag;
	unsigned int Length;
	unsigned int Reserved;
	HEADER *h1 = (HEADER *)h;

	fp = fopen(ConfigFileName, "rb");
	if (fp != NULL) {
		printf("\r\n Reading config information from file \r\n");
		byte_count = fread(filebuffer, 1, 2048, fp);
		if (byte_count > 0) {
			ptr = strstr((char *)filebuffer, "Tag=");
			if (ptr) {
				ptr += strlen("Tag=");
				sscanf(ptr, "%x", &Tag);
				h1->Tag = Tag;
			}
			ptr = strstr((char *)filebuffer, "Length=");
			if (ptr) {
				ptr += strlen("Length=");
				sscanf(ptr, "%x", &Length);
				h1->Length = Length;
			}
			ptr = strstr((char *)filebuffer, "Reserved=");
			if (ptr) {
				ptr += strlen("Reserved=");
				sscanf(ptr, "%x", &Reserved);
				h1->Reserved = Reserved;
			}
		}
	}
}
