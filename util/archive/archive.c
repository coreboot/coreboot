/* SPDX-License-Identifier: GPL-2.0-only */

#include "archive.h"
#include <endian.h>
#include <errno.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct directory *archive;

static void usage(void)
{
	printf("Name:\n");
	printf("	archive - concatenate files and create an archive\n");
	printf("Usage:\n");
	printf("	archive archive_name create file0 file1 ...\n");
}

static int get_file_size(const char *file)
{
	FILE *fp = fopen(file, "rb");
	int size;

	if (!fp) {
		fprintf(stderr, "Error: failed to open %s\n", file);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	if (size < 0) {
		fprintf(stderr, "Error: failed to get file size\n");
		return -1;
	}

	return size;
}

static int set_file_name(const char *path, struct dentry *dest)
{
	struct dentry *entry;
	char *name, *copy;
	uint32_t i;

	copy = strdup(path);
	name = basename(copy);

	/* check name length */
	if (strlen(name) > NAME_LENGTH) {
		fprintf(stderr, "Error: file name '%s' exceeds %d chars\n",
			name, NAME_LENGTH);
		free(copy);
		return -1;
	}

	/* check if there is a duplicate name */
	entry = get_first_dentry(archive);
	for (i = 0; i < archive->count && &entry[i] != dest; i++) {
		if (!strncmp(entry[i].name, name, NAME_LENGTH)) {
			fprintf(stderr, "Error: duplicate name '%s'\n", name);
			free(copy);
			return -1;
		}
	}

	/* copy the name to the entry */
	strncpy(dest->name, name, NAME_LENGTH);
	free(copy);

	return 0;
}

/*
 * Add a file to the archive in RAM
 *
 * path: path to the file to be added
 * entry: pointer to struct dentry where file header is created
 * offset: offset of the file contents from the archive header
 *
 * return: 0 on success or -1 on error
 */
static int add_file(const char *path, struct dentry *entry, uint32_t offset)
{
	FILE *fp;
	int size;

	if (!path || !*path || !entry) {
		fprintf(stderr, "Error: invalid path or entry\n");
		return -1;
	}

	size = get_file_size(path);
	if (size < 0)
		return -1;
	if (offset + size > archive->size) {
		fprintf(stderr, "Error: invalid offset or size\n");
		return -1;
	}

	fp = fopen(path, "rb");
	if (!fp) {
		fprintf(stderr, "Error: failed to open %s (%d: %s)\n",
			path, errno, strerror(errno));
		return -1;
	}
	if (fread((char *)archive + offset, sizeof(char), size, fp) != (size_t)size) {
		fprintf(stderr, "Error: failed to read %s\n", path);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	/* set file name*/
	if (set_file_name(path, entry))
		return -1;

	entry->offset = offset;
	entry->size = size;

	return 0;
}

/*
 * Allocate memory for archive
 *
 * count: number of files to add
 * files: pointer to the array of file names
 *
 * return: 0 on success or -1 on error
 */
static int setup_archive(int count, const char **files)
{
	uint32_t size;
	int i, s;

	size = sizeof(*archive);
	for (i = 0; i < count; i++) {
		s = get_file_size(files[i]);
		if (s < 0)
			return -1;
		size += sizeof(struct dentry);
		size += s;
	}

	archive = calloc(size, 1);
	if (!archive) {
		fprintf(stderr, "Error: failed to allocate memory\n");
		return -1;
	}

	/* install magic string */
	memcpy(archive->magic, CBAR_MAGIC, sizeof(archive->magic));
	archive->version = VERSION;
	archive->size = size;
	archive->count = count;

	printf("Set up archive: size=%d count=%d\n", size, count);

	return 0;
}

/*
 * Store files in archive
 */
static int archive_files(const char **files)
{
	struct dentry *entry;
	uint32_t offset;
	uint32_t i;

	entry = get_first_dentry(archive);
	offset = get_first_offset(archive);
	for (i = 0; i < archive->count; i++) {
		if (add_file(files[i], entry, offset))
			return -1;
		offset += entry->size;
		entry++;
	}

	return 0;
}

static void convert_endian(void)
{
	struct dentry *entry;
	uint32_t i;

	entry = get_first_dentry(archive);
	for (i = 0; i < archive->count; i++) {
		entry[i].offset = htole32(entry[i].offset);
		entry[i].size = htole32(entry[i].size);
	}

	archive->version = htole32(archive->version);
	archive->size = htole32(archive->size);
	archive->count = htole32(archive->count);
}

/*
 * Write archive to file
 */
static int output_archive(const char *path)
{
	FILE *fp;

	convert_endian();

	fp = fopen(path, "wb");
	if (!fp) {
		fprintf(stderr, "Error: failed to open %s\n", path);
		fclose(fp);
		return -1;
	}
	if (fwrite(archive, sizeof(char), archive->size, fp) != archive->size) {
		fprintf(stderr, "Error: failed to write to %s\n", path);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	printf("Wrote archive to %s\n", path);

	return 0;
}

static int cmd_create(const char *archive_path, int count, const char **files)
{
	if (count < 1 || !files) {
		fprintf(stderr, "Error: no input files specified\n");
		return -1;
	}

	if (setup_archive(count, files))
		return -1;

	if (archive_files(files))
		return -1;

	if (output_archive(archive_path))
		return -1;

	return 0;
}

int main(int argc, const char *argv[])
{
	const char *command;

	if (argc < 3) {
		fprintf(stderr, "Error: invalid number of arguments\n");
		usage();
		return -1;
	}

	command = argv[2];

	/* branch by command name */
	if (!strncmp(command, "create", sizeof("create"))) {
		if (cmd_create(argv[1], argc - 3, &argv[3]))
			return -1;
	} else {
		fprintf(stderr, "Error: invalid command: %s\n", command);
		return -1;
	}

	return 0;
}
