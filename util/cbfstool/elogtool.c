/* SPDX-License-Identifier: BSD-3-Clause */

#include <getopt.h>
#include <stdbool.h>
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
	ELOGTOOL_EXIT_READ_ERROR,
	ELOGTOOL_EXIT_WRITE_ERROR,
	ELOGTOOL_EXIT_NOT_ENOUGH_MEMORY,
	ELOGTOOL_EXIT_INVALID_ELOG_FORMAT,
	ELOGTOOL_EXIT_NOT_ENOUGH_BUFFER_SPACE,
};

static int cmd_list(const struct buffer *);
static int cmd_clear(const struct buffer *);

static const struct {
	const char *name;
	int (*func)(const struct buffer *buf);
	/* Whether it requires to write the buffer back */
	bool write_back;
} cmds[] = {
	{"list", cmd_list, false},
	{"clear", cmd_clear, true},
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
			"  list                    lists all the event logs in human readable format\n"
			"  clear                   clears all the event logs\n"
			"\n"
			"ARGS\n"
			"-f, --file <filename>   File that holds event log partition.\n"
			"                        If empty it will try to read/write from/to\n"
			"                        the " ELOG_RW_REGION_NAME " using flashrom.\n"
			"-h, --help              Print this help\n",
			invoked_as);
}

/*
 * If filename is empty, read RW_ELOG from flashrom.
 * Otherwise read the RW_ELOG from a file.
 * It fails if the ELOG header is invalid.
 * On success, buffer must be freed by caller.
 */
static int elog_read(struct buffer *buffer, const char *filename)
{
	if (filename == NULL) {
		uint8_t *buf;
		uint32_t buf_size;

		if (flashrom_read(FLASHROM_PROGRAMMER_INTERNAL_AP, ELOG_RW_REGION_NAME,
				  &buf, &buf_size) != VB2_SUCCESS) {
			fprintf(stderr, "Could not read RW_ELOG region using flashrom\n");
			return ELOGTOOL_EXIT_READ_ERROR;
		}
		buffer_init(buffer, NULL, buf, buf_size);
	} else if (buffer_from_file(buffer, filename) != 0) {
		fprintf(stderr, "Could not read input file: %s\n", filename);
		return ELOGTOOL_EXIT_READ_ERROR;
	}

	if (elog_verify_header(buffer_get(buffer)) != CB_SUCCESS) {
		fprintf(stderr, "FATAL: Invalid elog header\n");
		buffer_delete(buffer);
		return ELOGTOOL_EXIT_INVALID_ELOG_FORMAT;
	}

	return ELOGTOOL_EXIT_SUCCESS;
}

/*
 * If filename is NULL, it saves the buffer using flashrom.
 * Otherwise, it saves the buffer in the given filename.
 */
static int elog_write(struct buffer *buf, const char *filename)
{
	if (filename == NULL) {
		if (flashrom_write(FLASHROM_PROGRAMMER_INTERNAL_AP, ELOG_RW_REGION_NAME,
				   buffer_get(buf), buffer_size(buf)) != VB2_SUCCESS) {
			fprintf(stderr,
				"Failed to write to RW_ELOG region using flashrom\n");
			return ELOGTOOL_EXIT_WRITE_ERROR;
		}
		return ELOGTOOL_EXIT_SUCCESS;
	}

	if (buffer_write_file(buf, filename) != 0) {
		fprintf(stderr, "Failed to write to file %s\n", filename);
		return ELOGTOOL_EXIT_WRITE_ERROR;
	}
	return ELOGTOOL_EXIT_SUCCESS;
}

static size_t next_available_event_offset(const struct buffer *buf)
{
	const struct event_header *event;
	struct buffer copy, *iter = &copy;

	buffer_clone(iter, buf);

	while (buffer_size(iter) >= sizeof(struct event_header)) {
		event = buffer_get(iter);
		if (event->type == ELOG_TYPE_EOL || event->length == 0)
			break;

		assert(event->length <= buffer_size(iter));
		buffer_seek(iter, event->length);
	}

	return buffer_offset(iter) - buffer_offset(buf);
}

static int cmd_list(const struct buffer *buf)
{
	const struct event_header *event;
	unsigned int count = 0;

	/* Point to the first event */
	event = buffer_get(buf) + sizeof(struct elog_header);

	while ((const void *)(event) < buffer_end(buf)) {
		if (event->type == ELOG_TYPE_EOL || event->length == 0)
			break;

		eventlog_print_event(event, count);
		event = elog_get_next_event(event);
		count++;
	}

	return ELOGTOOL_EXIT_SUCCESS;
}

/*
 * Clears the elog events from the given buffer, which is a valid RW_ELOG region.
 * A LOG_CLEAR event is appended.
 */
static int cmd_clear(const struct buffer *buf)
{
	uint32_t used_data_size;
	struct buffer copy;

	/* Clone the buffer to avoid changing the offset of the original buffer */
	buffer_clone(&copy, buf);
	buffer_seek(&copy, sizeof(struct elog_header));

	/*
	 * Calculate the size of the "used" buffer, needed for ELOG_TYPE_LOG_CLEAR.
	 * Then overwrite the entire buffer with ELOG_TYPE_EOL.
	 * Finally insert a LOG_CLEAR event into the buffer.
	 */
	used_data_size = next_available_event_offset(&copy);
	memset(buffer_get(&copy), ELOG_TYPE_EOL, buffer_size(&copy));

	if (!eventlog_init_event(&copy, ELOG_TYPE_LOG_CLEAR,
				 &used_data_size, sizeof(used_data_size)))
		return ELOGTOOL_EXIT_NOT_ENOUGH_BUFFER_SPACE;

	return ELOGTOOL_EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	char *filename = NULL;
	struct buffer buf;
	unsigned int i;
	int argflag;
	int ret;

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

			filename = optarg;
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

	/* Returned buffer must be freed. */
	ret = elog_read(&buf, filename);
	if (ret)
		return ret;

	for (i = 0; i < ARRAY_SIZE(cmds); i++) {
		if (!strcmp(cmds[i].name, argv[optind])) {
			ret = cmds[i].func(&buf);
			break;
		}
	}

	if (i == ARRAY_SIZE(cmds)) {
		usage(argv[0]);
		ret = ELOGTOOL_EXIT_BAD_ARGS;
	}

	if (!ret && cmds[i].write_back)
		ret = elog_write(&buf, filename);

	buffer_delete(&buf);

	return ret;
}
