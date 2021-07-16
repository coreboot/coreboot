/* SPDX-License-Identifier: BSD-3-Clause */

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <common.h>
#include <commonlib/bsd/elog.h>
#include <flashrom.h>

#include "eventlog.h"

enum elogtool_return {
	ELOGTOOL_EXIT_SUCCESS = 0,
	ELOGTOOL_EXIT_BAD_ARGS,
	ELOGTOOL_EXIT_BAD_INPUT_PATH,
	ELOGTOOL_EXIT_NOT_ENOUGH_MEMORY,
	ELOGTOOL_EXIT_INVALID_ELOG_FORMAT,
};

static struct option long_options[] = {
	{"file", required_argument, 0, 'f'},
	{"help", no_argument, 0, 'h'},
	{NULL, 0, 0, 0},
};

static void usage(char *invoked_as)
{
	fprintf(stderr, "elogtool: list elog events\n\n"
			"USAGE:\n"
			"\t%s COMMAND [-f <filename>]\n\n"
			"where, COMMAND is:\n"
			"  list = lists all the event logs in human readable format\n\n"
			"ARGS\n"
			"-f, --file <filename>   Input file that holds event log partition.\n"
			"                        If empty it will try to read from the RW_ELOG ROM region\n"
			"-h, --help              Print this help\n",
			invoked_as);
}

// If filename is empty, read RW_ELOG from flashrom. Otherwise read the RW_ELOG from a file.
// Buffer must be freed by caller.
static int elog_read(const char *filename, struct buffer *buffer)
{
	if (filename == NULL) {
		uint8_t *buf;
		uint32_t buf_size;

		if (flashrom_read(FLASHROM_PROGRAMMER_INTERNAL_AP, "RW_ELOG", &buf, &buf_size)
		    != VB2_SUCCESS) {
			fprintf(stderr, "Could not read RW_ELOG region using flashrom\n");
			return ELOGTOOL_EXIT_BAD_INPUT_PATH;
		}
		buffer_init(buffer, NULL, buf, buf_size);
	} else {
		if (buffer_from_file(buffer, filename) != 0) {
			fprintf(stderr, "Could not read input file: %s\n", filename);
			return ELOGTOOL_EXIT_BAD_INPUT_PATH;
		}
	}

	return 0;
}

static int elog_list_events(const struct buffer *buf)
{
	const struct event_header *event;
	const void *data;
	uint32_t data_len;
	unsigned int count = 0;

	data = buffer_get(buf);
	data_len = buffer_size(buf);

	if (elog_verify_header(data) != CB_SUCCESS) {
		fprintf(stderr, "FATAL: Invalid elog header\n");
		return ELOGTOOL_EXIT_INVALID_ELOG_FORMAT;
	}

	/* Point to first event */
	event = (const struct event_header *)(data + sizeof(struct elog_header));

	while ((const void *)(event) < data + data_len) {
		if (event->type == ELOG_TYPE_EOL || event->length == 0)
			break;

		eventlog_print_event(event, count);
		event = elog_get_next_event(event);
		count++;
	}

	return ELOGTOOL_EXIT_SUCCESS;
}

static int cmd_list(const char *filename)
{
	int ret;

	// Returned buffer must be freed.
	struct buffer buf;
	ret = elog_read(filename, &buf);
	if (ret != 0)
		return ret;

	ret = elog_list_events(&buf);

	buffer_delete(&buf);
	return ret;
}


int main(int argc, char **argv)
{
	int argflag;
	char *input_file = NULL;

	if (argc < 2) {
		usage(argv[0]);
		return ELOGTOOL_EXIT_BAD_ARGS;
	}

	while (1) {
		int option_index;
		argflag = getopt_long(argc, argv, "hf:", long_options, &option_index);
		if (argflag == -1)
			break;

		switch (argflag) {
		case 'h':
		case '?':
			usage(argv[0]);
			return ELOGTOOL_EXIT_SUCCESS;

		case 'f':
			if (!optarg) {
				usage(argv[0]);
				return ELOGTOOL_EXIT_BAD_ARGS;
			}

			input_file = optarg;
			break;

		default:
			break;
		}
	}

	/* At least one command must be available. */
	if (optind >= argc) {
		usage(argv[0]);
		return ELOGTOOL_EXIT_BAD_ARGS;
	}

	if (!strcmp(argv[optind], "list"))
		return cmd_list(input_file);

	return ELOGTOOL_EXIT_SUCCESS;
}
