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
 * Foundation, Inc.
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	uint16_t dummy;
	uint16_t crc;
} __attribute__((packed));

struct crc_t {
	 uint16_t (*crc_f)(uint16_t crc, void *void_buf, size_t size);
	 uint32_t crc_init;
	 uint16_t ver_major;
	 uint16_t ver_minor;
};


#define BIMG_MAGIC	/* y */	0xabbadaba /* doo! */

#define BIMG_OP_MASK		(0xf << 0)
#define BIMG_OP_EXEC_RETURN	(0x1 << 0)
#define BIMG_OP_EXEC_NO_RETURN	(0x2 << 0)
#define BIMG_DATA_CHECKSUM	(0x1 << 4)

/* Typical use case for this utility. */
#define BIMG_FLAGS (BIMG_OP_EXEC_NO_RETURN | BIMG_DATA_CHECKSUM)

#define MAX_RECORD_BYTES	0x8000

#define CRC_16

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

static uint16_t crc_16(uint16_t crc, void *void_buf, size_t size)
{
	/*
	 * CRC table for the CRC-16.
	 * The poly is 0x8005 (x^16 + x^15 + x^2 + 1)
	 */
	static const uint16_t crc16_table[256] = {
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
	};
	uint8_t *buf, data;

	for (buf = void_buf; size; size--) {
		data = *buf++;
		crc = (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
	}

	return crc;

}

static const struct crc_t crc_type = {
#if defined(CRC_16)
	.crc_f = crc_16,
	.crc_init = 0,
	.ver_major = 2,
	.ver_minor = 0
#elif defined(CRC_X25)
	.crc_f = crc_x25,
	.crc_init = 0xffff,
	.ver_major = 1,
	.ver_minor = 0
#endif
};

static int write_binary(FILE *out, FILE *in, struct bimg_header *hdr)
{
	static uint8_t file_buf[MAX_RECORD_BYTES];
	struct bimg_data_header data_hdr = { 0 };
	size_t n_written;

	data_hdr.dest_addr = hdr->entry_addr;

	/*
	 * The read binary data has to be split in chunks of max 64KiB - 1 byte
	 * (SPI controller limitation). Each chunk will have its own header in
	 * order to respect the BIMG format.
	 */
	while ((data_hdr.size = fread(file_buf, 1, sizeof(file_buf), in))) {
		data_hdr.crc = crc_type.crc_f(crc_type.crc_init, &data_hdr,
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
		hdr->data_crc = crc_type.crc_f(hdr->data_crc,
						file_buf, n_written);
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

	data_hdr.crc = crc_type.crc_f(crc_type.crc_init, &data_hdr,
		sizeof(data_hdr) - sizeof(data_hdr.crc));

	if (fwrite(&data_hdr, sizeof(data_hdr), 1, out) != 1)
		error_ret(-EIO, "Failed to write data header: %d\n", errno);

	hdr->data_size += sizeof(data_hdr);

	return 0;
}

static const char *help_message =
	"Usage: bimgtool <input> [<output> <base-address>]\n"
	"\n"
	"This is a simple tool which generates and verifies boot images in\n"
	"the BIMG format, used in systems designed by Imagination\n"
	"Technologies, for example the Pistachio SoC. This version of the\n"
	"tool works with BIMG images version %d.\n"
	"\n"
	"  input:          The binary file to be converted to a BIMG\n"
	"                  or verified\n"
	"  output:         The name of the output BIMG file\n"
	"  base-address:   The address in memory at which you wish the "
	"                  input binary to be loaded.\n";

static void usage(FILE *f)
{
	fprintf(f, help_message, crc_type.ver_major);
}

static int verify_file(FILE *f)
{
	struct bimg_header file_header;
	struct bimg_data_header data_header;
	char *file_pointer;
	char *file_data;
	struct stat buf;
	int data_size;
	int fd = fileno(f);
	uint32_t data_crc = crc_type.crc_init;
	uint32_t crc_result;

	if (fread(&file_header, 1, sizeof(struct bimg_header), f) !=
	    sizeof(struct bimg_header)) {
		perror("Problems trying to read input file header\n");
		return -1;
	}

	if (fstat(fd, &buf)) {
		perror("Problems trying to stat input file\n");
		return -1;
	}

	if (file_header.magic != BIMG_MAGIC) {
		fprintf(stderr, "Wrong magic value %#x\n", file_header.magic);
		return -1;
	}

	crc_result = crc_type.crc_f(crc_type.crc_init, &file_header,
				    sizeof(file_header) -
				    sizeof(file_header.crc));
	if (file_header.crc != crc_result) {
		fprintf(stderr, "File header CRC mismatch\n");
		return -1;
	}

	if ((file_header.data_size + sizeof(struct bimg_header)) >
	    buf.st_size) {
		fprintf(stderr, "Data size too big: %d > %d\n",
			file_header.data_size, buf.st_size);
		return -1;
	}

	if (file_header.ver_major != crc_type.ver_major) {
		fprintf(stderr, "Image version mismatch: %d\n",
			file_header.ver_major);
		return -1;
	}

	if ((file_header.flags & BIMG_FLAGS) != BIMG_FLAGS) {
		fprintf(stderr, "Unexpected file header flags: %#x\n",
			file_header.flags);
		return -1;
	}

	if (file_header.ver_minor != crc_type.ver_minor) {
		fprintf(stderr,
			"Minor version mismatch: %d, will try anyways\n",
			file_header.ver_minor);
	}

	data_size = file_header.data_size;
	file_pointer = malloc(data_size);
	if (!file_pointer) {
		fprintf(stderr, "Failed to allocate %d bytes\n",
			file_header.data_size);
		return -1;
	}

	if (fread(file_pointer, 1, data_size, f) != data_size) {
		fprintf(stderr, "Failed to read %d bytes\n", data_size);
		free(file_pointer);
		return -1;
	}

	file_data = file_pointer;
	while (data_size > 0) {
		memcpy(&data_header, file_data,  sizeof(data_header));

		/* Check the data block header integrity. */
		crc_result = crc_type.crc_f(crc_type.crc_init, &data_header,
					    sizeof(data_header) -
					    sizeof(data_header.crc));
		if (data_header.crc != crc_result) {
			fprintf(stderr, "Data header CRC mismatch at %d\n",
				file_header.data_size - data_size);
			free(file_pointer);
			return -1;
		}

		/*
		 * Add the block data to the CRC stream, the last block size
		 * will be zero.
		 */
		file_data += sizeof(data_header);
		data_crc = crc_type.crc_f(data_crc,
					  file_data, data_header.size);

		data_size -= data_header.size + sizeof(data_header);
		file_data += data_header.size;
	}

	if (data_size) {
		fprintf(stderr, "File size mismatch\n");
		free(file_pointer);
		return -1;
	}

	if (data_crc != file_header.data_crc) {
		fprintf(stderr, "File data CRC mismatch\n");
		free(file_pointer);
		return -1;
	}

	free(file_pointer);
	return 0;
}

int main(int argc, char *argv[])
{
	const char *in_filename, *out_filename;
	FILE *in_file, *out_file;
	int err;
	struct bimg_header hdr = {
		.magic = BIMG_MAGIC,
		.ver_major = crc_type.ver_major,
		.ver_minor = crc_type.ver_minor,
		.flags = BIMG_FLAGS,
		.data_crc = crc_type.crc_init,
	};

	if ((argc != 4) && (argc != 2)) {
		usage(stderr);
		goto out_err;
	}

	in_filename = argv[1];

	in_file = fopen(in_filename, "r");
	if (!in_file) {
		error("Failed to open input file '%s'\n", in_filename);
		goto out_err;
	}

	if (argc == 2)
		return verify_file(in_file);

	out_filename = argv[2];
	hdr.entry_addr = strtoul(argv[3], NULL, 16);

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

	hdr.crc = crc_type.crc_f(crc_type.crc_init, &hdr,
					sizeof(hdr) - sizeof(hdr.crc));

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
