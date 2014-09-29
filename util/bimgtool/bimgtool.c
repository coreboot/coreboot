/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Imagination Technologies Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

struct bimg_header {
	uint32_t magic;
	uint16_t ver_major;
	uint16_t ver_minor;
	uint32_t data_size;
	uint32_t entry_addr;
	uint32_t flags;
	uint32_t data_crc;
	uint32_t crc;
} __attribute__((packed));

struct bimg_data_header {
	uint32_t size;
	uint32_t dest_addr;
	uint16_t crc;
} __attribute__((packed));

#define BIMG_MAGIC	/* y */	0xabbadaba /* doo! */

#define BIMG_OP_MASK		(0xf << 0)
#define BIMG_OP_EXEC_RETURN	(0x1 << 0)
#define BIMG_OP_EXEC_NO_RETURN	(0x2 << 0)
#define BIMG_DATA_CHECKSUM	(0x1 << 4)

#define MAX_RECORD_BYTES	0x8000

#define CRC_INIT		0xffff

#define error(msg...) fprintf(stderr, "ERROR: " msg)

#define error_ret(ret, msg...) {		\
	error(msg);				\
	return ret;				\
}

static uint16_t crc_x25(uint16_t crc, void *void_buf, size_t size)
{
	static const uint16_t crc_table[16] = {
		0x0000, 0x1021, 0x2042, 0x3063,
		0x4084, 0x50a5, 0x60c6, 0x70e7,
		0x8108, 0x9129, 0xa14a, 0xb16b,
		0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	};
	uint8_t *buf, data;

	for (buf = void_buf; size; size--) {
		data = *buf++;
		crc = (crc << 4) ^ crc_table[((crc >> 12) ^ (data >> 4)) & 0xf];
		crc = (crc << 4) ^ crc_table[((crc >> 12) ^ (data >> 0)) & 0xf];
	}

	return crc;
}

static int write_binary(FILE *out, FILE *in, struct bimg_header *hdr)
{
	static uint8_t file_buf[MAX_RECORD_BYTES];
	struct bimg_data_header data_hdr;
	size_t n_written;

	data_hdr.dest_addr = hdr->entry_addr;

	while ((data_hdr.size = fread(file_buf, 1, sizeof(file_buf), in))) {
		data_hdr.crc = crc_x25(CRC_INIT, &data_hdr,
			sizeof(data_hdr) - sizeof(data_hdr.crc));

		if (fwrite(&data_hdr, sizeof(data_hdr), 1, out) != 1)
			error_ret(-EIO, "Failed to write data header: %d\n",
				  errno);

		n_written = fwrite(file_buf, 1, data_hdr.size, out);
		if (n_written != data_hdr.size)
			error_ret(-EIO, "Failed to write to output file: %d\n",
				  errno);

		data_hdr.dest_addr += n_written;
		hdr->data_size += sizeof(data_hdr) + n_written;
		hdr->data_crc = crc_x25(hdr->data_crc, file_buf, n_written);
	}

	if (ferror(in))
		error_ret(-EIO, "Failed to read input file\n");

	return 0;
}

static int write_final(FILE *out, struct bimg_header *hdr)
{
	struct bimg_data_header data_hdr = {
		.size = 0,
		.dest_addr = ~0,
	};

	data_hdr.crc = crc_x25(CRC_INIT, &data_hdr,
		sizeof(data_hdr) - sizeof(data_hdr.crc));

	if (fwrite(&data_hdr, sizeof(data_hdr), 1, out) != 1)
		error_ret(-EIO, "Failed to write data header: %d\n", errno);

	hdr->data_size += sizeof(data_hdr);

	return 0;
}

static void usage(FILE *f)
{
	fprintf(f,
		"Usage: bimgtool <input> <output> <base-address>\n"
		"\n"
		"bimgtool is a simple tool which generates boot images in the "
		"BIMG format used in systems designed by Imagination "
		"Technologies, for example the Pistachio SoC. This version of the "
		"tool generates BIMG version 1.0 images.\n"
		"\n"
		"  input:          The binary file to be converted to a BIMG\n"
		"  output:         The name of the output BIMG file\n"
		"  base-address:   The address in memory at which you wish the "
		"input binary to be loaded.\n");
}

int main(int argc, char *argv[])
{
	const char *in_filename, *out_filename;
	FILE *in_file, *out_file;
	int err;
	struct bimg_header hdr = {
		.magic = BIMG_MAGIC,
		.ver_major = 1,
		.ver_minor = 0,
		.flags = BIMG_OP_EXEC_NO_RETURN | BIMG_DATA_CHECKSUM,
		.data_crc = CRC_INIT,
	};

	if (argc != 4) {
		usage(stderr);
		goto out_err;
	}

	in_filename = argv[1];
	out_filename = argv[2];
	hdr.entry_addr = strtoul(argv[3], NULL, 16);

	in_file = fopen(in_filename, "r");
	if (!in_file) {
		error("Failed to open input file '%s'\n", in_filename);
		goto out_err;
	}

	out_file = fopen(out_filename, "w");
	if (!out_file) {
		error("Failed to open output file '%s'\n", out_filename);
		goto out_err_close_in;
	}

	if (fseek(out_file, sizeof(hdr), SEEK_SET)) {
		error("Failed to seek past header: %d\n", errno);
		goto out_err_close_out;
	}

	err = write_binary(out_file, in_file, &hdr);
	if (err) {
		error("Failed to write binary: %d\n", err);
		goto out_err_close_out;
	}

	err = write_final(out_file, &hdr);
	if (err) {
		error("Failed to write final record: %d\n", err);
		goto out_err_close_out;
	}

	hdr.crc = crc_x25(CRC_INIT, &hdr, sizeof(hdr) - sizeof(hdr.crc));

	if (fseek(out_file, 0, SEEK_SET)) {
		error("Failed to seek to header: %d\n", errno);
		goto out_err_close_out;
	}

	if (fwrite(&hdr, sizeof(hdr), 1, out_file) != 1) {
		error("Failed to write header: %d\n", errno);
		goto out_err_close_out;
	}

	fclose(in_file);
	fclose(out_file);
	return EXIT_SUCCESS;

out_err_close_out:
	fclose(out_file);
out_err_close_in:
	fclose(in_file);
out_err:
	return EXIT_FAILURE;
}
