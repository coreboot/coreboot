/*
 * ftdump.c - Contributed by Pantelis Antoniou <pantelis.antoniou AT gmail.com>
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <byteswap.h>

#include "flat_dt.h"

#define cpu_to_be16(x)	htons(x)
#define be16_to_cpu(x)	ntohs(x)

#define cpu_to_be32(x)	htonl(x)
#define be32_to_cpu(x)	ntohl(x)

#if __BYTE_ORDER == __BIG_ENDIAN
#define cpu_to_be64(x)	(x)
#define be64_to_cpu(x)	(x)
#else
#define cpu_to_be64(x)	bswap_64(x)
#define be64_to_cpu(x)	bswap_64(x)
#endif

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))
#define PALIGN(p, a)	((void *)(ALIGN((unsigned long)(p), (a))))
#define GET_CELL(p)	(p += 4, *((uint32_t *)(p-4)))

static int is_printable_string(const void *data, int len)
{
	const char *s = data;
	const char *ss;

	/* zero length is not */
	if (len == 0)
		return 0;

	/* must terminate with zero */
	if (s[len - 1] != '\0')
		return 0;

	ss = s;
	while (*s && isprint(*s))
		s++;

	/* not zero, or not done yet */
	if (*s != '\0' || (s + 1 - ss) < len)
		return 0;

	return 1;
}

static void print_data(const void *data, int len)
{
	int i;
	const uint8_t *s;

	/* no data, don't print */
	if (len == 0)
		return;

	if (is_printable_string(data, len)) {
		printf(" = \"%s\"", (char *)data);
	} else if ((len % 4) == 0) {
		printf(" = <");
		for (i = 0; i < len; i += 4)
			printf("%08x%s", *((uint32_t *)data + i),
			       i < (len - 4) ? " " : "");
		printf(">");
	} else {
		printf(" = [");
		for (i = 0, s = data; i < len; i++)
			printf("%02x%s", s[i], i < len - 1 ? " " : "");
		printf("]");
	}
}

static void dump_blob(void *blob)
{
	struct boot_param_header *bph = blob;
	struct reserve_entry *p_rsvmap = 
		(struct reserve_entry *)(blob
					 + be32_to_cpu(bph->off_mem_rsvmap));
	char *p_struct = blob + be32_to_cpu(bph->off_dt_struct);
	char *p_strings = blob + be32_to_cpu(bph->off_dt_strings);
	uint32_t version = be32_to_cpu(bph->version);
	uint32_t tag;
	char *p;
	char *s, *t;
	int depth, sz, shift;
	int i;
	uint64_t addr, size;

	depth = 0;
	shift = 4;

	printf("// Version 0x%x tree\n", version);
	for (i = 0; ; i++) {
		addr = be64_to_cpu(p_rsvmap[i].address);
		size = be64_to_cpu(p_rsvmap[i].size);
		if (addr == 0 && size == 0)
			break;

		printf("/memreserve/ %llx %llx;\n",
		       (unsigned long long)addr, (unsigned long long)size);
	}

	p = p_struct;
	while ((tag = be32_to_cpu(GET_CELL(p))) != OF_DT_END) {

		/* printf("tag: 0x%08x (%d)\n", tag, p - p_struct); */

		if (tag == OF_DT_BEGIN_NODE) {
			s = p;
			p = PALIGN(p + strlen(s) + 1, 4);

			if (*s == '\0')
				s = "/";

			printf("%*s%s {\n", depth * shift, "", s);

			depth++;
			continue;
		}

		if (tag == OF_DT_END_NODE) {
			depth--;

			printf("%*s};\n", depth * shift, "");
			continue;
		}

		if (tag == OF_DT_NOP) {
			printf("%*s// [NOP]\n", depth * shift, "");
			continue;
		}

		if (tag != OF_DT_PROP) {
			fprintf(stderr, "%*s ** Unknown tag 0x%08x\n", depth * shift, "", tag);
			break;
		}
		sz = GET_CELL(p);
		s = p_strings + be32_to_cpu(GET_CELL(p));
		if (version < 0x10 && sz >= 8)
			p = PALIGN(p, 8);
		t = p;

		p = PALIGN(p + sz, 4);

		printf("%*s%s", depth * shift, "", s);
		print_data(t, sz);
		printf(";\n");
	}
}


int main(int argc, char *argv[])
{
	FILE *fp;
	char buf[16384];	/* 16k max */
	int size;

	if (argc < 2) {
		fprintf(stderr, "supply input filename\n");
		return 5;
	}

	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		fprintf(stderr, "unable to open %s\n", argv[1]);
		return 10;
	}

	size = fread(buf, 1, sizeof(buf), fp);
	if (size == sizeof(buf)) {	/* too large */
		fprintf(stderr, "file too large\n");
		return 10;
	}

	dump_blob(buf);

	fclose(fp);

	return 0;
}
