/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006-2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <types.h>
#include <string.h>
#include <lar.h>
#include <console.h>

#ifndef CONFIG_BIG_ENDIAN
#define ntohl(x) ( ((x&0xff)<<24) | ((x&0xff00)<<8) | \
		((x&0xff0000) >> 8) | ((x&0xff000000) >> 24) )
#else
#define ntohl(x) (x)
#endif

/**
 * run_address is passed the address of a function taking no parameters and
 * jumps to it, returning the result. 
 * @param v the address to call as a function. 
 * returns value returned by the function. 
 */

int run_address(void *f)
{
	int (*v) (void);
	v = f;
	return v();
}

/**
 * Given a file name in the LAR , search for it, and fill out a return struct with the 
 * result. 
 * @param archive A descriptor for current archive. This is actually a mem_file type, 
 *    which is a machine-dependent representation of hte actual archive. In particular, 
 *    things like u32 are void * in the mem_file struct. 
 * @param filename filename to find
 * @param result pointer to mem_file struct which is filled in if the file is found
 * returns 0 on success, -1 otherwise
 */

int find_file(struct mem_file *archive, const char *filename, struct mem_file *result)
{
	char *walk, *fullname;
	struct lar_header *header;

	printk(BIOS_INFO, "LAR: Attempting to open '%s'.\n", filename);
	printk(BIOS_SPEW, "LAR: Start %p len 0x%x\n", archive->start,
	       archive->len);

	if (archive->len < sizeof(struct lar_header))
		printk(BIOS_ERR, "Error: truncated archive (%d bytes); minimum possible size is %d bytes\n", 
			archive->len, sizeof(struct lar_header));

	/* Getting this for loop right is harder than it looks. All quantities are 
	  * unsigned. The LAR stretches from (e.g.) 0xfff0000 for 0x100000 
	  * bytes, i.e. to address ZERO. 
	  * As a result, 'walk', can wrap to zero and keep going (this has been 
	  * seen in practice). Recall that these are unsigned; walk can 
	  * wrap to zero; so, question, when is walk less than any of these:
	  * archive->start
	  * Answer: once walk wraps to zero, it is < archive->start
	  * archive->start + archive->len
	  * archive->start + archive->len  - 1
	  * Answer: In the case that archive->start + archive->len == 0, ALWAYS!
	  * A lot of expressions have been tried and all have been wrong. 
	  * So what would work? Simple:
	  * test for walk < archive->start + archive->len - 1 to cover the case
	  *	that the archive does NOT occupy ALL of the top of memory and 
	  *	wrap to zero; 
	  * and test for walk >= archive->start, 
	  * to cover the case that you wrapped to zero. 
	  * Unsigned pointer arithmetic that wraps to zero can be messy.
	  */
	for (walk = archive->start;
	     (walk < (char *)(archive->start + archive->len - sizeof(struct lar_header))) && 
			(walk >= (char *)archive->start); walk += 16) {
		if (strcmp(walk, MAGIC) != 0)
			continue;

		header = (struct lar_header *)walk;
		fullname = walk + sizeof(struct lar_header);

		printk(BIOS_SPEW, "LAR: search for %s\n", fullname);
		// FIXME: check checksum

		if (strcmp(fullname, filename) == 0) {
			printk(BIOS_SPEW, "LAR: CHECK %s @ %p\n", fullname, header);
			result->start = walk + ntohl(header->offset);
			result->len = ntohl(header->len);
			result->reallen = ntohl(header->reallen);
			result->compression = ntohl(header->compression);
			result->entry = (void *)ntohl((u32)header->entry);
			result->loadaddress = (void *)ntohl((u32)header->loadaddress);
			printk(BIOS_SPEW, 
			"start %p len %d reallen %d compression %x entry %p loadaddress %p\n", 
				result->start, result->len, result->reallen, 
				result->compression, result->entry, result->loadaddress);
			return 0;
		}
		// skip file
		walk += (ntohl(header->len) + ntohl(header->offset) -
			1) & 0xfffffff0;
	}
	printk(BIOS_SPEW, "NO FILE FOUND\n");
	return 1;
}

/**
 * Given a file name in the LAR , search for it, and load it into memory, using 
 * the loadaddress pointer in the mem_file struct. 
 * @param archive A descriptor for current archive.
 * @param filename filename to find
 * returns 0 on success, -1 otherwise
 */

void *load_file(struct mem_file *archive, const char *filename)
{
	int ret;
	struct mem_file result;
	void *where;
	void *entry;

	ret = find_file(archive, filename, &result);
	if (ret) {
		printk(BIOS_INFO, "LAR: load_file: No such file '%s'\n",
		       filename);
		return (void *)-1;
	}
	entry = result.entry;
	where = result.loadaddress;
	printk(BIOS_SPEW, "LAR: Compression algorithm #%i used\n", result.compression);
	/* no compression */
	if (result.compression == 0) {
		memcpy(where, result.start, result.len);
		return entry;
	}
#ifdef CONFIG_COMPRESSION_LZMA
	/* lzma */
	unsigned long ulzma(unsigned char *src, unsigned char *dst);
	if (result.compression == 1) {
		ulzma(result.start, where);
		return entry;
	}
#endif
#ifdef CONFIG_COMPRESSION_NRV2B
	/* nrv2b */
	unsigned long unrv2b(u8 *src, u8 *dst, unsigned long *ilen_p);
	if (result.compression == 2) {
		int tmp;
		unrv2b(result.start, where, &tmp);
		return entry;
	}
#endif
	printk(BIOS_INFO, "LAR: Compression algorithm #%i not supported!\n", result.compression);
	return (void *)-1;
}

/* FIXME -- most of copy_file should be replaced by load_file */
/**
 * Given a file name in the LAR , search for it, and load it into memory, 
 * using the passed-in pointer as the address
 * @param archive A descriptor for current archive.
 * @param filename filename to find
 * @param where pointer to where to load the data
 * returns 0 on success, -1 otherwise
 */
int copy_file(struct mem_file *archive, const char *filename, void *where)
{
	int ret;
	struct mem_file result;

	ret = find_file(archive, filename, &result);
	if (ret) {
		printk(BIOS_INFO, "LAR: copy_file: No such file '%s'\n",
		       filename);
		return 1;
	}

	printk(BIOS_SPEW, "LAR: Compression algorithm #%i used\n", result.compression);
	/* no compression */
	if (result.compression == 0) {
		memcpy(where, result.start, result.len);
		return 0;
	}
#ifdef CONFIG_COMPRESSION_LZMA
	/* lzma */
	unsigned long ulzma(unsigned char *src, unsigned char *dst);
	if (result.compression == 1) {
		ulzma(result.start, where);
		return 0;
	}
#endif
#ifdef CONFIG_COMPRESSION_NRV2B
	/* nrv2b */
	unsigned long unrv2b(u8 *src, u8 *dst, unsigned long *ilen_p);
	if (result.compression == 2) {
		int tmp;
		unrv2b(result.start, where, &tmp);
		return 0;
	}
#endif
	printk(BIOS_INFO, "LAR: Compression algorithm #%i not supported!\n", result.compression);
	return 1;
}


/**
 * Given a file name in the LAR , search for it, and load it into memory, 
 * using the passed-in pointer as the address; jump to the file. 
 * If the passed-in pointer is (void *)-1, then execute the file in place. 
 * @param archive A descriptor for current archive.
 * @param filename filename to find
 * @param where pointer to where to load the data
 * returns 0 on success, -1 otherwise
 */
int run_file(struct mem_file *archive, const char *filename, void *where)
{
	struct mem_file result;
	int ret;

	if ((u32) where != 0xFFFFFFFF) {
		if (copy_file(archive, filename, where)) {
			printk(BIOS_INFO,
			      "LAR: Run file %s failed: No such file.\n",
			       filename);
			return 1;
		}
	} else { /* XIP */
		if (find_file(archive, filename, &result)) {
			printk(BIOS_INFO,
			       "LAR: Run file %s failed: No such file.\n",
			       filename);
			return 1;
		}
		if (result.compression != 0) {
			printk(BIOS_INFO,
			       "LAR: Run file %s failed: Compressed file"
			       " not supported for in-place execution\n",
			       filename);
			return 1;
		}
		where = result.start;
	}
	printk(BIOS_SPEW, "where is %p\n", where);
	ret = run_address(where);
	printk(BIOS_SPEW, "run_file returns with %d\n", ret);
	return ret;
}

/**
 * Given a file name in the LAR , search for it, and execute it in place. 
 * @param archive A descriptor for current archive.
 * @param filename filename to find
 * returns 0 on success, -1 otherwise
 */
int execute_in_place(struct mem_file *archive, const char *filename)
{
	return run_file(archive, filename, (void *) 0xFFFFFFFF);
}
