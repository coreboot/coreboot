/*
 * cbfstool
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "cbfstool.h"

int get_size(const char *size)
{
	char *next;
	int val = strtoul(size, &next, 0);

	/* Support modifiers for the size kK and mM for kbytes and 
	   mbytes respectfully */

	if (next != NULL) {
		if (*next == 'k' || *next == 'K')
			val *= 1024;
		else if (*next == 'm' || *next == 'M')
			val *= (1024 * 1024);
	}

	return val;
}

int copy_from_fd(int fd, void *ptr, int size)
{
	unsigned char *p = ptr;

	while (size > 0) {
		int ret = read(fd, p, size);

		if (ret == -1) {
			ERROR("Error while reading: %m\n");
			return -1;
		}

		if (ret == 0) {
			ERROR("Unexpected end of file\n");
			return -1;
		}

		p += ret;
		size -= ret;
	}

	return 0;
}

int size_and_open(const char *filename, unsigned int *size)
{
	struct stat s;

	int fd = open(filename, O_RDONLY);

	if (fd == -1) {
		ERROR("Unable to open %s: %m\n", filename);
		return -1;
	}

	if (fstat(fd, &s)) {
		ERROR("Unable to stat %s: %m\n", filename);
		close(fd);
		return -1;
	}

	*size = s.st_size;
	return fd;
}

int map_rom(struct rom *rom, int size)
{
	rom->ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED,
			rom->fd, 0);

	if (rom->ptr == MAP_FAILED) {
		ERROR("Could not map the rom: %m\n");
		rom->ptr = NULL;
		return -1;
	}

	return 0;
}

int open_rom(struct rom *rom, const char *filename)
{
	struct stat s;
	unsigned long offset;

	if (stat(filename, &s)) {
		ERROR("Could not stat %s: %m\n", filename);
		return -1;
	}

	rom->fd = open(filename, O_RDWR);

	if (rom->fd == -1) {
		ERROR("Could not open %s: %m\n", filename);
		rom->fd = 0;
		return -1;
	}

	if (map_rom(rom, s.st_size))
		goto err;

	/* Find the master header */

	offset = ROM_READL(rom, s.st_size - 4);

	rom->header = (struct cbfs_header *)
	    ROM_PTR(rom, s.st_size - (0xFFFFFFFF - offset) - 1);

	if (ntohl(rom->header->magic) != HEADER_MAGIC) {
		ERROR("This does not appear to be a valid ROM\n");
		goto err;
	}

	/* Check that the alignment is correct */
	if (ntohl(rom->header->align) == 0) {
		ERROR("The alignment in the ROM is 0 - probably malformed\n");
		goto err;
	}

	/* Sanity check that the size matches the file size */

	if (ntohl(rom->header->romsize) != s.st_size) {
		ERROR("The ROM size in the header does not match the file\n");
		ERROR("ROM size is %d bytes, file size is %d bytes\n",
		      ntohl(rom->header->romsize), (unsigned int)s.st_size);
		goto err;
	}

	rom->size = ntohl(rom->header->romsize);
	rom->fssize = rom->size - ntohl(rom->header->bootblocksize);

	return 0;

err:
	if (rom->ptr != NULL)
		munmap(rom->ptr, s.st_size);

	if (rom->fd > 0)
		close(rom->fd);

	rom->ptr = NULL;
	rom->fd = 0;
	return -1;
}

int create_rom(struct rom *rom, const unsigned char *filename,
	       int romsize, const unsigned char *bootblockname,
	       int bootblocksize, int align)
{
	unsigned char null = '\0';

	if (rom->fd != 0) {
		ERROR("%s already exists - cannot create\n", filename);
		return -1;
	}

	/* Remember the size of the entire ROM */
	rom->size = romsize;

	/* The size of the archive section is everything but the bootblock and
	 * the cbfs master header. */
	rom->fssize = romsize - bootblocksize - sizeof(struct cbfs_header);

	/* Open the file descriptor */

	rom->fd = open((char *)filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

	if (rom->fd == -1) {
		ERROR("Could not create %s: %m\n", filename);
		return -1;
	}

	/* Size the new file appropriately */
	lseek(rom->fd, romsize - 1, SEEK_SET);
	write(rom->fd, &null, 1);

	if (map_rom(rom, romsize)) {
		close(rom->fd);
		return -1;
	}

	/* This is a pointer to the header for easy access */
	rom->header = (struct cbfs_header *)
	    ROM_PTR(rom, rom->size - 16 - bootblocksize - sizeof(struct cbfs_header));
	rom->header->magic = htonl(HEADER_MAGIC);
	rom->header->romsize = htonl(romsize);
	rom->header->bootblocksize = htonl(bootblocksize);
	rom->header->align = htonl(align);
	rom->header->offset = htonl(0);

	add_bootblock(rom, bootblockname);

	/* Write the cbfs master header address at the end of the ROM. */

	ROM_WRITEL(rom, rom->size - 4,
		   0xFFFFFFF0 - bootblocksize - sizeof(struct cbfs_header));
	return 0;
}

int add_bootblock(struct rom *rom, const char *filename)
{
	unsigned int size;
	int fd = size_and_open(filename, &size);
	int ret;
	struct cbfs_header tmp;

	if (fd == -1)
		return -1;

	if (size > ntohl(rom->header->bootblocksize)) {
		ERROR("The bootblock size is not correct (%d vs %d)\n",
		      size, ntohl(rom->header->bootblocksize));
		return -1;
	}

	/* Copy the bootblock into place at the end of the file */
	ret = copy_from_fd(fd, ROM_PTR(rom, rom->size - ntohl(rom->header->bootblocksize)), size);

	close(fd);

	if (ret) {
		ERROR("Unable to add %s to the bootblock\n", filename);
		return -1;
	}

	return 0;
}

int rom_exists(struct rom *rom)
{
	if (rom->fd <= 0)
		return 0;
	return 1;
}
