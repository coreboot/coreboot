#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif
#include "elf.h"
#include "elf_boot.h"
#include "mkelfImage.h"

static struct file_type file_type[] = {
	{ "linux-i386", linux_i386_probe, linux_i386_mkelf, linux_i386_usage },
	{ "bzImage-i386", bzImage_i386_probe, linux_i386_mkelf, linux_i386_usage },
	{ "vmlinux-i386", vmlinux_i386_probe, linux_i386_mkelf, linux_i386_usage },
	{ "linux-ia64", linux_ia64_probe, linux_ia64_mkelf, linux_ia64_usage },
};
static const int file_types = sizeof(file_type)/sizeof(file_type[0]);

void die(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}



/**************************************************************************
IPCHKSUM - Checksum IP Header
**************************************************************************/
uint16_t ipchksum(const void *data, unsigned long length)
{
	unsigned long sum;
	unsigned long i;
	const uint8_t *ptr;

	/* In the most straight forward way possible,
	 * compute an ip style checksum.
	 */
	sum = 0;
	ptr = data;
	for(i = 0; i < length; i++) {
		unsigned long value;
		value = ptr[i];
		if (i & 1) {
			value <<= 8;
		}
		/* Add the new value */
		sum += value;
		/* Wrap around the carry */
		if (sum > 0xFFFF) {
			sum = (sum + (sum >> 16)) & 0xFFFF;
		}
	}
	return (~cpu_to_le16(sum)) & 0xFFFF;
}

uint16_t add_ipchksums(unsigned long offset, uint16_t sum, uint16_t new)
{
	unsigned long checksum;
	sum = ~sum & 0xFFFF;
	new = ~new & 0xFFFF;
	if (offset & 1) {
		/* byte swap the sum if it came from an odd offset
		 * since the computation is endian independant this
		 * works.
		 */
		new = bswap_16(new);
	}
	checksum = sum + new;
	if (checksum > 0xFFFF) {
		checksum -= 0xFFFF;
	}
	return (~checksum) & 0xFFFF;
}

void *xmalloc(size_t size, const char *name)
{
	void *buf;
	buf = malloc(size);
	if (!buf) {
		die("Cannot malloc %ld bytes to hold %s: %s\n",
			size + 0UL, name, strerror(errno));
	}
	return buf;
}

void *xrealloc(void *ptr, size_t size, const char *name)
{
	void *buf;
	buf = realloc(ptr, size);
	if (!buf) {
		die("Cannot realloc %ld bytes to hold %s: %s\n",
			size + 0UL, name, strerror(errno));
	}
	return buf;
}


char *slurp_file(const char *filename, off_t *r_size)
{
	int fd;
	char *buf;
	off_t size, progress;
	ssize_t result;
	struct stat stats;


	if (!filename) {
		*r_size = 0;
		return 0;
	}
	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		die("Cannot open `%s': %s\n",
			filename, strerror(errno));
	}
	result = fstat(fd, &stats);
	if (result < 0) {
		die("Cannot stat: %s: %s\n",
			filename, strerror(errno));
	}
	size = stats.st_size;
	*r_size = size;
	buf = xmalloc(size, filename);
	progress = 0;
	while(progress < size) {
		result = read(fd, buf + progress, size - progress);
		if (result < 0) {
			if ((errno == EINTR) ||	(errno == EAGAIN))
				continue;
			die("read on %s of %ld bytes failed: %s\n",
				filename, (size - progress)+ 0UL, strerror(errno));
		}
		progress += result;
	}
	result = close(fd);
	if (result < 0) {
		die("Close of %s failed: %s\n",
			filename, strerror(errno));
	}
	return buf;
}

#if HAVE_ZLIB_H
char *slurp_decompress_file(const char *filename, off_t *r_size)
{
	gzFile fp;
	int errnum;
	const char *msg;
	char *buf;
	off_t size, allocated;
	ssize_t result;

	if (!filename) {
		*r_size = 0;
		return 0;
	}
	fp = gzopen(filename, "rb");
	if (fp == 0) {
		msg = gzerror(fp, &errnum);
		if (errnum == Z_ERRNO) {
			msg = strerror(errno);
		}
		die("Cannot open `%s': %s\n", filename, msg);
	}
	size = 0;
	allocated = 65536;
	buf = xmalloc(allocated, filename);
	do {
		if (size == allocated) {
			allocated <<= 1;
			buf = xrealloc(buf, allocated, filename);
		}
		result = gzread(fp, buf + size, allocated - size);
		if (result < 0) {
			if ((errno == EINTR) || (errno == EAGAIN))
				continue;

			msg = gzerror(fp, &errnum);
			if (errnum == Z_ERRNO) {
				msg = strerror(errno);
			}
			die ("read on %s of %ld bytes failed: %s\n",
				filename, (allocated - size) + 0UL, msg);
		}
		size += result;
	} while(result > 0);
	result = gzclose(fp);
	if (result != Z_OK) {
		msg = gzerror(fp, &errnum);
		if (errnum == Z_ERRNO) {
			msg = strerror(errno);
		}
		die ("Close of %s failed: %s\n", filename, msg);
	}
	*r_size =  size;
	return buf;
}
#else
char *slurp_decompress_file(const char *filename, off_t *r_size)
{
	return slurp_file(filename, r_size);
}
#endif

struct memelfphdr *add_program_headers(struct memelfheader *ehdr, int count)
{
	struct memelfphdr *phdr;
	int i;
	ehdr->e_phnum = count;
	ehdr->e_phdr = phdr = xmalloc(count *sizeof(*phdr), "Program headers");
	/* Set the default values */
	for(i = 0; i < count; i++) {
		phdr[i].p_type   = PT_LOAD;
		phdr[i].p_flags  = PF_R | PF_W | PF_X;
		phdr[i].p_vaddr  = 0;
		phdr[i].p_paddr  = 0;
		phdr[i].p_filesz = 0;
		phdr[i].p_memsz  = 0;
		phdr[i].p_data   = 0;
	}
	return phdr;
}

struct memelfnote *add_notes(struct memelfheader *ehdr, int count)
{
	struct memelfnote *notes;
	ehdr->e_notenum = count;
	ehdr->e_notes = notes = xmalloc(count *sizeof(*notes), "Notes");
	memset(notes, 0, count *sizeof(*notes));
	return notes;
}

static int sizeof_notes(struct memelfnote *note, int notes)
{
	int size;
	int i;

	size = 0;
	for(i = 0; i < notes; i++) {
		size += sizeof(Elf_Nhdr);
		size += roundup(strlen(note[i].n_name)+1, 4);
		size += roundup(note[i].n_descsz, 4);
	}
	return size;
}

static uint16_t cpu_to_elf16(struct memelfheader *ehdr, uint16_t val)
{
	if (ehdr->ei_data == ELFDATA2LSB) {
		return cpu_to_le16(val);
	}
	else if (ehdr->ei_data == ELFDATA2MSB) {
		return cpu_to_be16(val);
	}
	die("Uknown elf layout in cpu_to_elf16");
	return 0;
}

static uint32_t cpu_to_elf32(struct memelfheader *ehdr, uint32_t val)
{
	if (ehdr->ei_data == ELFDATA2LSB) {
		return cpu_to_le32(val);
	}
	else if (ehdr->ei_data == ELFDATA2MSB) {
		return cpu_to_be32(val);
	}
	die("Uknown elf layout in cpu_to_elf32");
	return 0;
}

static uint64_t cpu_to_elf64(struct memelfheader *ehdr, uint64_t val)
{
	if (ehdr->ei_data == ELFDATA2LSB) {
		return cpu_to_le64(val);
	}
	else if (ehdr->ei_data == ELFDATA2MSB) {
		return cpu_to_be64(val);
	}
	die("Uknown elf layout in cpu_to_elf64");
	return 0;
}

static void serialize_notes(char *buf, struct memelfheader *ehdr)
{
	struct Elf_Nhdr hdr;
	struct memelfnote *note;
	int notes;
	size_t size, offset;
	int i;

	/* Clear the buffer */
	note = ehdr->e_notes;
	notes = ehdr->e_notenum;
	size = sizeof_notes(note, notes);
	memset(buf, 0, size);

	/* Write the Elf Notes */
	offset = 0;
	for(i = 0; i < notes; i++) {
		/* Compute the note header */
		size_t n_namesz;
		n_namesz = strlen(note[i].n_name) +1;
		hdr.n_namesz = cpu_to_elf32(ehdr, n_namesz);
		hdr.n_descsz = cpu_to_elf32(ehdr, note[i].n_descsz);
		hdr.n_type   = cpu_to_elf32(ehdr, note[i].n_type);

		/* Copy the note into the buffer */
		memcpy(buf + offset, &hdr,       sizeof(hdr));
		offset += sizeof(hdr);
		memcpy(buf + offset, note[i].n_name, n_namesz);
		offset += roundup(n_namesz, 4);
		memcpy(buf + offset, note[i].n_desc, note[i].n_descsz);
		offset += roundup(note[i].n_descsz, 4);

	}
}
static void serialize_ehdr(char *buf, struct memelfheader *ehdr)
{
	if (ehdr->ei_class == ELFCLASS32) {
		Elf32_Ehdr *hdr = (Elf32_Ehdr *)buf;
		hdr->e_ident[EI_MAG0]    = ELFMAG0;
		hdr->e_ident[EI_MAG1]    = ELFMAG1;
		hdr->e_ident[EI_MAG2]    = ELFMAG2;
		hdr->e_ident[EI_MAG3]    = ELFMAG3;
		hdr->e_ident[EI_CLASS]   = ehdr->ei_class;
		hdr->e_ident[EI_DATA]    = ehdr->ei_data;
		hdr->e_ident[EI_VERSION] = EV_CURRENT;
		hdr->e_type      = cpu_to_elf16(ehdr, ehdr->e_type);
		hdr->e_machine   = cpu_to_elf16(ehdr, ehdr->e_machine);
		hdr->e_version   = cpu_to_elf32(ehdr, EV_CURRENT);
		hdr->e_entry     = cpu_to_elf32(ehdr, ehdr->e_entry);
		hdr->e_phoff     = cpu_to_elf32(ehdr, sizeof(*hdr));
		hdr->e_shoff     = cpu_to_elf32(ehdr, 0);
		hdr->e_flags     = cpu_to_elf32(ehdr, ehdr->e_flags);
		hdr->e_ehsize    = cpu_to_elf16(ehdr, sizeof(*hdr));
		hdr->e_phentsize = cpu_to_elf16(ehdr, sizeof(Elf32_Phdr));
		hdr->e_phnum     = cpu_to_elf16(ehdr, ehdr->e_phnum);
		hdr->e_shentsize = cpu_to_elf16(ehdr, 0);
		hdr->e_shnum     = cpu_to_elf16(ehdr, 0);
		hdr->e_shstrndx  = cpu_to_elf16(ehdr, 0);
	}
	else if (ehdr->ei_class == ELFCLASS64) {
		Elf64_Ehdr *hdr = (Elf64_Ehdr *)buf;
		hdr->e_ident[EI_MAG0]    = ELFMAG0;
		hdr->e_ident[EI_MAG1]    = ELFMAG1;
		hdr->e_ident[EI_MAG2]    = ELFMAG2;
		hdr->e_ident[EI_MAG3]    = ELFMAG3;
		hdr->e_ident[EI_CLASS]   = ehdr->ei_class;
		hdr->e_ident[EI_DATA]    = ehdr->ei_data;
		hdr->e_ident[EI_VERSION] = EV_CURRENT;
		hdr->e_type      = cpu_to_elf16(ehdr, ehdr->e_type);
		hdr->e_machine   = cpu_to_elf16(ehdr, ehdr->e_machine);
		hdr->e_version   = cpu_to_elf32(ehdr, EV_CURRENT);
		hdr->e_entry     = cpu_to_elf64(ehdr, ehdr->e_entry);
		hdr->e_phoff     = cpu_to_elf64(ehdr, sizeof(*hdr));
		hdr->e_shoff     = cpu_to_elf64(ehdr, 0);
		hdr->e_flags     = cpu_to_elf32(ehdr, ehdr->e_flags);
		hdr->e_ehsize    = cpu_to_elf16(ehdr, sizeof(*hdr));
		hdr->e_phentsize = cpu_to_elf16(ehdr, sizeof(Elf64_Phdr));
		hdr->e_phnum     = cpu_to_elf16(ehdr, ehdr->e_phnum);
		hdr->e_shentsize = cpu_to_elf16(ehdr, 0);
		hdr->e_shnum     = cpu_to_elf16(ehdr, 0);
		hdr->e_shstrndx  = cpu_to_elf16(ehdr, 0);
	}
	else die("Uknown elf class: %x\n", ehdr->ei_class);
}
static void serialize_phdrs(char *buf, struct memelfheader *ehdr, size_t note_size)
{
	int i;
	size_t offset, note_offset;
	if (ehdr->ei_class == ELFCLASS32) {
		Elf32_Phdr *phdr = (Elf32_Phdr *)buf;
		note_offset =
			sizeof(Elf32_Ehdr) + (sizeof(Elf32_Phdr)*ehdr->e_phnum);
		offset = note_offset + note_size;
		for(i = 0; i < ehdr->e_phnum; i++) {
			struct memelfphdr *hdr = ehdr->e_phdr + i;
			phdr[i].p_type   = cpu_to_elf32(ehdr, hdr->p_type);
			phdr[i].p_offset = cpu_to_elf32(ehdr, offset);
			phdr[i].p_vaddr  = cpu_to_elf32(ehdr, hdr->p_vaddr);
			phdr[i].p_paddr  = cpu_to_elf32(ehdr, hdr->p_paddr);
			phdr[i].p_filesz = cpu_to_elf32(ehdr, hdr->p_filesz);
			phdr[i].p_memsz  = cpu_to_elf32(ehdr, hdr->p_memsz);
			phdr[i].p_flags  = cpu_to_elf32(ehdr, hdr->p_flags);
			phdr[i].p_align  = cpu_to_elf32(ehdr, 0);
			if (phdr[i].p_type == PT_NOTE) {
				phdr[i].p_filesz = cpu_to_elf32(ehdr, note_size);
				phdr[i].p_memsz  = cpu_to_elf32(ehdr, note_size);
				phdr[i].p_offset = cpu_to_elf32(ehdr, note_offset);
			} else {
				offset += hdr->p_filesz;
			}
		}
	}
	else if (ehdr->ei_class == ELFCLASS64) {
		Elf64_Phdr *phdr = (Elf64_Phdr *)buf;
		note_offset =
			sizeof(Elf64_Ehdr) + (sizeof(Elf64_Phdr)*ehdr->e_phnum);
		offset = note_offset + note_size;
		for(i = 0; i < ehdr->e_phnum; i++) {
			struct memelfphdr *hdr = ehdr->e_phdr + i;
			phdr[i].p_type   = cpu_to_elf32(ehdr, hdr->p_type);
			phdr[i].p_flags  = cpu_to_elf32(ehdr, hdr->p_flags);
			phdr[i].p_offset = cpu_to_elf64(ehdr, offset);
			phdr[i].p_vaddr  = cpu_to_elf64(ehdr, hdr->p_vaddr);
			phdr[i].p_paddr  = cpu_to_elf64(ehdr, hdr->p_paddr);
			phdr[i].p_filesz = cpu_to_elf64(ehdr, hdr->p_filesz);
			phdr[i].p_memsz  = cpu_to_elf64(ehdr, hdr->p_memsz);
			phdr[i].p_align  = cpu_to_elf64(ehdr, 0);
			if (phdr[i].p_type == PT_NOTE) {
				phdr[i].p_filesz = cpu_to_elf64(ehdr, note_size);
				phdr[i].p_memsz  = cpu_to_elf64(ehdr, note_size);
				phdr[i].p_offset = cpu_to_elf64(ehdr, note_offset);
			} else {
				offset += hdr->p_filesz;
			}
		}
	}
	else {
		die("Unknwon elf class: %x\n", ehdr->ei_class);
	}
}

static void write_buf(int fd, char *buf, size_t size)
{
	size_t progress = 0;
	ssize_t result;
	while(progress < size) {
		result = write(fd, buf + progress, size - progress);
		if (result < 0) {
			if ((errno == EAGAIN) || (errno == EINTR)) {
				continue;
			}
			die ("write of %ld bytes failed: %s\n",
				size - progress, strerror(errno));
		}
		progress += result;
	}
}
static void write_elf(struct memelfheader *ehdr, char *output)
{
	size_t ehdr_size;
	size_t phdr_size;
	size_t note_size;
	size_t size;
	uint16_t checksum;
	size_t bytes;
	char *buf;
	int result, fd;
	int i;
	/* Prep for adding the checksum */
	for(i = 0; i < ehdr->e_notenum; i++) {
		if ((memcmp(ehdr->e_notes[i].n_name, "ELFBoot", 8) == 0) &&
			(ehdr->e_notes[i].n_type == EIN_PROGRAM_CHECKSUM)) {
			ehdr->e_notes[i].n_desc = &checksum;
			ehdr->e_notes[i].n_descsz = 2;
		}
	}
	/* Compute the sizes */
	ehdr_size = 0;
	phdr_size = 0;
	note_size = 0;
	if (ehdr->e_notenum) {
		note_size = sizeof_notes(ehdr->e_notes, ehdr->e_notenum);
	}
	if (ehdr->ei_class == ELFCLASS32) {
		ehdr_size = sizeof(Elf32_Ehdr);
		phdr_size = sizeof(Elf32_Phdr) * ehdr->e_phnum;
	}
	else if (ehdr->ei_class == ELFCLASS64) {
		ehdr_size = sizeof(Elf64_Ehdr);
		phdr_size = sizeof(Elf64_Phdr) * ehdr->e_phnum;
	}
	else {
		die("Unknown elf class: %x\n", ehdr->ei_class);
	}

	/* Allocate a buffer to temporarily hold the serialized forms */
	size = ehdr_size + phdr_size + note_size;
	buf = xmalloc(size, "Elf Headers");
	memset(buf, 0, size);
	serialize_ehdr(buf, ehdr);
	serialize_phdrs(buf + ehdr_size, ehdr, note_size);

	/* Compute the checksum... */
	checksum = ipchksum(buf, ehdr_size + phdr_size);
	bytes = ehdr_size + phdr_size;
	for(i = 0; i < ehdr->e_phnum; i++) {
		checksum = add_ipchksums(bytes, checksum,
			ipchksum(ehdr->e_phdr[i].p_data, ehdr->e_phdr[i].p_filesz));
		bytes += ehdr->e_phdr[i].p_memsz;
	}

	/* Compute the final form of the notes */
	serialize_notes(buf + ehdr_size + phdr_size, ehdr);

	/* Now write the elf image */
	fd = open(output, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IRGRP | S_IROTH);
	if (fd < 0) {
		die("Cannot open ``%s'':%s\n",
			output, strerror(errno));
	}
	write_buf(fd, buf, size);
	for(i = 0; i < ehdr->e_phnum; i++) {
		write_buf(fd, ehdr->e_phdr[i].p_data, ehdr->e_phdr[i].p_filesz);
	}
	result = close(fd);
	if (result < 0) {
		die("Close on %s failed: %s\n",
			output, strerror(errno));
	}
}

static void version(void)
{
	printf("mkelfImage " VERSION " released " RELEASE_DATE "\n");
}
void usage(void)
{
	int i;
	version();
	printf(
		"Usage: mkelfImage [OPTION]... <kernel> <elf_kernel>\n"
		"Build an ELF bootable kernel image from a normal kernel image\n"
		"\n"
		" -h, --help                  Print this help.\n"
		" -v, --version               Print the version of kexec.\n"
		"     --kernel=<filename>     Set the kernel to <filename>\n"
		"     --output=<filename>     Output to <filename>\n"
		" -t, --type=TYPE             Specify the new kernel is of <type>.\n"
		"\n"
		"Supported kernel types: \n"
		);
	for(i = 0; i < file_types; i++) {
		printf("%s\n", file_type[i].name);
		file_type[i].usage();
	}
	printf("\n");
}

void error(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	usage();
	exit(1);
}

int main(int argc, char **argv)
{
	int opt;
	int fileind;
	char *type, *kernel, *output;
	off_t kernel_size;
	char *kernel_buf;
	int result;
	int i;
	struct memelfheader hdr;

	static const struct option options[] = {
		MKELF_OPTIONS
		{ 0, 0, 0, 0 },
	};
	static const char short_options[] = MKELF_OPT_STR;

	memset(&hdr, 0, sizeof(hdr));
	kernel = 0;
	output = 0;

	/* Get the default type from the program name */
	type = strrchr(argv[0], '/');
	if (!type) type = argv[0];
	if (memcmp(type, "mkelf-", 6) == 0) {
		type = type + 6;
	} else {
		type = 0;
	}
	opterr = 0; /* Don't complain about unrecognized options here */
	while ((opt = getopt_long(argc, argv, short_options, options, 0)) != -1) {
		switch(opt) {
		case OPT_HELP:
			usage();
			return 0;
		case OPT_VERSION:
			version();
			return 0;
		case OPT_KERNEL:
			kernel = optarg;
			break;
		case OPT_OUTPUT:
			output = optarg;
			break;
		case OPT_TYPE:
			type = optarg;
			break;
		default:
			break;
		}
	}
	fileind = optind;

	/* Reset getopt for the next pass */
	opterr = 1;
	optind = 1;

	if (argc - fileind > 0) {
		kernel = argv[fileind++];
	}
	if (argc - fileind > 0) {
		output = argv[fileind++];
	}
	if (!kernel) {
		error("No kernel specified!\n");
	}
	if (!output) {
		error("No output file specified!\n");
	}
	if (argc - fileind > 0) {
		error("%d extra options specified!\n", argc - fileind);
	}

	/* slurp in the input kernel */
	kernel_buf = slurp_decompress_file(kernel, &kernel_size);

	/* Find/verify the kernel type */
	for(i = 0; i < file_types; i++) {
		char *reason;
		if (type && (strcmp(type, file_type[i].name) != 0)) {
			continue;
		}
		reason = file_type[i].probe(kernel_buf, kernel_size);
		if (reason == 0) {
			break;
		}
		if (type) {
			die("Not %s: %s\n", type, reason);
		}
	}
	if (i == file_types) {
		die("Can not determine the file type of %s\n", kernel);
	}
	result = file_type[i].mkelf(argc, argv, &hdr, kernel_buf, kernel_size);
	if (result < 0) {
		die("Cannot create %s result: %d\n", output, result);
	}
	/* open the output file */
	write_elf(&hdr, output);
	return 0;
}
