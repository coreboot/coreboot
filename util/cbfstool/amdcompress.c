/* AMD Family 17h and later BIOS compressor */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <elfparsing.h>
#include "zlib.h"

#define DEBUG_FILE 0

#define HDR_SIZE 256
#define UNCOMP_MAX 0x300000

#define DIR_UNDEF 0
#define DIR_COMP 1
#define DIR_UNCOMP 2

typedef struct _header {
	uint32_t rsvd1[5];
	uint32_t size;
	uint32_t rsvd2[58];
}  __attribute__((packed)) header;

static const char *optstring  = "i:o:cm:uh";

static struct option long_options[] = {
	{"infile",           required_argument, 0, 'i' },
	{"outfile",          required_argument, 0, 'o' },
	{"compress",         no_argument,       0, 'c' },
	{"maxsize",          required_argument, 0, 'm' },
	{"uncompress",       no_argument,       0, 'u' },
	{"help",             no_argument,       0, 'h' },
};

static void usage(void)
{
	printf("<name>: Extract or create a zlib compressed BIOS binary\n");
	printf("        image.  A compressed image contains a 256 byte\n");
	printf("        header with a 32-bit size at 0x14.\n");
	printf("Usage: <name> -i in_file -o out_file -[c|u]\n");
	printf("-i | --infile <FILE>         Input file\n");
	printf("-o | --outfile <FILE>        Output file\n");
	printf("-c | --compress              Compress\n");
	printf("-m | --maxsize <HEX_VAL>     Maximum uncompressed size (optional)\n");
	printf("                              * On compress: verify uncompressed size\n");
	printf("                                will be less than or equal maxsize\n");
	printf("                              * On uncompress: override default buffer size\n");
	printf("                                allocation of 0x%x bytes\n", UNCOMP_MAX);
	printf("-u | --uncompress            Uncompress\n");
	printf("-h | --help                  Display this message\n");

	exit(1);
}

static int do_file(char *name, size_t *size, int oflag)
{
	struct stat fd_stat;
	int fd;

	fd = open(name, oflag, 0666);
	if (fd < 0)
		return -1;

	if (fstat(fd, &fd_stat)) {
		close(fd);
		return -1;
	}

	if (size)
		*size = fd_stat.st_size;
	return fd;
}

static int parse_elf_to_xip_ram(const struct buffer *input,
					struct buffer *output)
{
	struct parsed_elf pelf;

	if (parse_elf(input, &pelf, ELF_PARSE_ALL))
		return 1;
	if (buffer_create(output, pelf.phdr->p_filesz, "") != 0)
		return 1;

	memcpy(output->data, input->data + pelf.phdr->p_offset, output->size);

	return 0;
}

static int convert_elf(struct buffer *buf)
{
	struct buffer out;

	if (parse_elf_to_xip_ram(buf, &out)) {
		printf("\tError parsing ELF file\n");
		return -1;
	}

	/* Discard the elf file in buf and replace with the progbits */
	free(buf->data);
	buf->data = out.data;
	buf->size = out.size;

	return 0;
}

static int iself(const void *input)
{
	const Elf32_Ehdr *ehdr = input;
	return !memcmp(ehdr->e_ident, ELFMAG, 4);
}

/* todo: Consider using deflate() and inflate() instead of compress() and
 * decompress(), especially if memory allocation somehow becomes a problem.
 * Those two functions can operate on streams and process chunks of data.
 */

/* Build the required header and follow it with the compressed image.  Detect
 * whether the input is an elf image, and if so, compress only the progbits.
 *
 *     header
 *   0 +------+-------+-------+-------+
 *     |      |       |       |       |
 *     +----------------------+-------+
 *     |      | size  |       |       |
 *     +----------------------+-------+
 *     |      |       |       |       |
 *     |      |       |          ...  |
 * 256 +------------------------------+
 *     |compressed image              |
 *     |   ...                        |
 *     |   ...                        |
 *     |   ...                        |
 *   n +------------------------------+
 */
static void do_compress(char *outf, char *inf, size_t max_size)
{
	int out_fd, in_fd;
	struct buffer inbf, outbf;
	int err;

	in_fd = do_file(inf, &inbf.size, O_RDONLY);
	if (in_fd < 0) {
		printf("\tError opening input file %s\n", inf);
		err = 1;
		goto out;
	}

	out_fd = do_file(outf, 0, O_CREAT | O_WRONLY);
	if (out_fd < 0) {
		printf("\tError opening output file %s\n", outf);
		err = 1;
		goto out_close_in;
	}

	inbf.data = calloc(inbf.size, 1);
	if (!inbf.data) {
		printf("\tError allocating 0x%zx bytes for input buffer\n", inbf.size);
		err = 1;
		goto out_close_out;
	}

	if (read(in_fd, inbf.data, inbf.size) != (ssize_t)inbf.size) {
		printf("\tError reading input file %s\n", inf);
		err = 1;
		goto out_free_in;
	}

	if (iself(inbf.data)) {
		if (convert_elf(&inbf)) {
			err = 1;
			goto out_free_in;
		}
	}

	if (max_size && inbf.size > max_size) {
		printf("\tError - size (%zx) exceeds specified max_size (%zx)\n",
				inbf.size, max_size);
		err = 1;
		goto out_free_in;
	}

	outbf.size = inbf.size; /* todo: tbd worst case? */
	outbf.size += sizeof(header);
	outbf.data = calloc(outbf.size, 1);
	if (!outbf.size) {
		printf("\tError allocating 0x%zx bytes for output buffer\n", outbf.size);
		err = 1;
		goto out_free_in;
	}

	err = compress((Bytef *)(outbf.data + sizeof(header)), &outbf.size,
				(Bytef *)inbf.data, inbf.size);
	if (err != Z_OK) {
		printf("\tzlib compression error %d\n", err);
		err = 1;
		goto out_free_out;
	}

	if (DEBUG_FILE)
		printf("\tCompressed 0x%zx bytes into 0x%zx\n", inbf.size,
				outbf.size - sizeof(header));

	((header *)outbf.data)->size = outbf.size;

	if (write(out_fd, outbf.data, outbf.size + sizeof(header))
				!= (ssize_t)(outbf.size + sizeof(header))) {
		printf("\tError writing to %s\n", outf);
		err = 1;
		/* fall through to out_free_out */
	}

out_free_out:
	free(outbf.data);
out_free_in:
	free(inbf.data);
out_close_out:
	close(out_fd);
out_close_in:
	close(in_fd);
out:
	if (err)
		exit(err);
}

static void do_uncompress(char *outf, char *inf, size_t max_size)
{
	int out_fd, in_fd;
	char *in_buf, *out_buf;
	size_t size_unc, size_comp;
	size_t bytes;
	int err;

	in_fd = do_file(inf, &size_comp, O_RDONLY);
	if (in_fd < 0) {
		printf("\tError opening input file %s\n", inf);
		err = 1;
		goto out;
	}

	out_fd = do_file(outf, 0, O_CREAT | O_WRONLY);
	if (out_fd < 0) {
		printf("\tError opening output file %s\n", outf);
		err = 1;
		goto out_close_in;
	}

	in_buf = calloc(size_comp, 1);
	if (!in_buf) {
		printf("\tError allocating 0x%zx bytes for input buffer\n", size_comp);
		err = 1;
		goto out_close_out;
	}

	bytes = read(in_fd, in_buf, size_comp);
	if (bytes != size_comp) {
		printf("\tError reading input file %s\n", inf);
		err = 1;
		goto out_free_in;
	}

	size_comp = ((header *)in_buf)->size;

	size_unc = max_size ? max_size : UNCOMP_MAX;
	out_buf = calloc(size_unc, 1);
	if (!out_buf) {
		printf("\tError allocating 0x%zx bytes for output buffer\n", size_unc);
		err = 1;
		goto out_free_in;
	}

	err = uncompress((Bytef *)out_buf, &size_unc,
				(Bytef *)in_buf + sizeof(header), size_comp);
	if (err != Z_OK) {
		printf("\tzlib uncompression error %d\n", err);
		err = 1;
		goto out_free_out;
	}

	if (DEBUG_FILE)
		printf("Uncompressed 0x%zx bytes into 0x%zx\n", size_comp, size_unc);

	bytes = write(out_fd, out_buf, size_unc);
	if (bytes != size_unc) {
		printf("\tError writing to %s\n", outf);
		err = 1;
		/* fall through to out_free_out */
	}

out_free_out:
	free(out_buf);
out_free_in:
	free(in_buf);
out_close_out:
	close(out_fd);
out_close_in:
	close(in_fd);
out:
	if (err)
		exit(err);
}

int main(int argc, char *argv[])
{
	int c;
	char *inf = 0, *outf = 0, *scratch;
	int direction = DIR_UNDEF;
	size_t max_size = 0;

	while (1) {
		int optindex = 0;

		c = getopt_long(argc, argv, optstring, long_options, &optindex);
		if (c == -1)
			break;

		switch (c) {
		case 'i':
			inf = optarg;
			break;
		case 'o':
			outf = optarg;
			break;
		case 'c':
			if (direction != DIR_UNDEF)
				usage();
			direction = DIR_COMP;
			break;
		case 'u':
			if (direction != DIR_UNDEF)
				usage();
			direction = DIR_UNCOMP;
			break;
		case 'm':
			max_size = strtoull(optarg, &scratch, 16);
			break;
		case 'h':
			usage();
		}
	}
	if (!inf || !outf || direction == DIR_UNDEF)
		usage();

	if (direction == DIR_COMP)
		do_compress(outf, inf, max_size);
	else
		do_uncompress(outf, inf, max_size);

	return 0;
}
