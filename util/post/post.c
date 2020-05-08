/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>

#define POST_DEFAULT_IO_PORT 0x80

void usage(char *progname, const char *error, ...)
{
	printf("Usage: %s <VALUE> [PORT]\n", progname);
	printf("The VALUE argument is an integer between 0x00 and 0xff\n");
	printf("The PORT argument is an integer between 0x00 and 0xffff\n");

	if (error) {
		va_list args;

		va_start(args, error);
		vprintf(error, args);
		va_end(args);
	};
}

void check_int(long val, int min, int max, int err, char *string, char *endptr,
	       char *progname)
{
	if (val < min || val > max) {
		usage(progname,
		      "\nError: The value has to be between 0x%x and 0x%x\n",
		      min, max);
		exit(EXIT_FAILURE);
	}

	if (endptr == string || *endptr != '\0') {
		usage(progname, "\nError: An integer is required\n");
		exit(EXIT_FAILURE);
	}

	if ((err) && (!val)) {
		perror("strtol");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	unsigned long val;
	unsigned long port = POST_DEFAULT_IO_PORT;
	char *endptr;
	int err;

	if (argc != 2 && argc != 3) {
		usage(argv[0], NULL);
		exit(EXIT_FAILURE);
	}

	val = strtol(argv[1], &endptr, 0);
	err = errno;
	check_int(val, 0x00, 0xff, err, argv[1], endptr, argv[0]);

	if (argc > 2) {
		port = strtol(argv[2], &endptr, 0);
		err = errno;
		check_int(port, 0x0000, 0xffff, err, argv[2], endptr, argv[0]);
	}

	err = iopl(3);
	if (err == -1) {
		perror("Not root");
		exit(EXIT_FAILURE);
	}

	outb(val, port);

	return 0;
}
