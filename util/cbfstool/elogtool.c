/* SPDX-License-Identifier: BSD-3-Clause */

#include <assert.h>
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

/* Only refers to the data max size. The "-1" is the checksum byte */
#define ELOG_MAX_EVENT_DATA_SIZE  (ELOG_MAX_EVENT_SIZE - sizeof(struct event_header) - 1)

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
static int cmd_add(const struct buffer *);

static const struct {
	const char *name;
	int (*func)(const struct buffer *buf);
	/* Whether it requires to write the buffer back */
	bool write_back;
} cmds[] = {
	{"list", cmd_list, false},
	{"clear", cmd_clear, true},
	{"add", cmd_add, true},
};

static char **cmd_argv;		/* Command arguments */
static char *argv0;		/* Used as invoked_as */

static struct option long_options[] = {
	{"file", required_argument, 0, 'f'},
	{"help", no_argument, 0, 'h'},
	{NULL, 0, 0, 0},
};

static void usage(char *invoked_as)
{
	fprintf(stderr, "elogtool: edit elog events\n\n"
			"USAGE:\n"
			"\t%s COMMAND [-f <filename>]\n\n"
			"where, COMMAND is:\n"
			"  list                          lists all the event logs in human readable format\n"
			"  clear                         clears all the event logs\n"
			"  add <event_type> [event_data] add an entry to the event log\n"
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
		if (flashrom_host_read(buffer, ELOG_RW_REGION_NAME) != 0) {
			fprintf(stderr, "Could not read RW_ELOG region using flashrom\n");
			return ELOGTOOL_EXIT_READ_ERROR;
		}
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
static int elog_write(struct buffer *buffer, const char *filename)
{
	if (filename == NULL) {
		if (flashrom_host_write(buffer, ELOG_RW_REGION_NAME) != 0) {
			fprintf(stderr,
				"Failed to write to RW_ELOG region using flashrom\n");
			return ELOGTOOL_EXIT_WRITE_ERROR;
		}
		return ELOGTOOL_EXIT_SUCCESS;
	}

	if (buffer_write_file(buffer, filename) != 0) {
		fprintf(stderr, "Failed to write to file %s\n", filename);
		return ELOGTOOL_EXIT_WRITE_ERROR;
	}
	return ELOGTOOL_EXIT_SUCCESS;
}

/* Buffer offset must point to a valid event_header struct */
static size_t next_available_event_offset(const struct buffer *buf)
{
	const struct event_header *event;
	struct buffer copy, *iter = &copy;

	assert(buffer_offset(buf) >= sizeof(struct elog_header));

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

/*
 * Shrinks buffer by ~bytes_to_shrink, then appends a LOG_CLEAR event,
 * and finally fills the remaining area with EOL events.
 * Buffer offset must point to a valid event_header struct.
 */
static int shrink_buffer(const struct buffer *buf, size_t bytes_to_shrink)
{
	struct buffer copy, *iter = &copy;
	const struct event_header *event;
	uint32_t cleared;
	int remaining;
	uint8_t *data;

	assert(buffer_offset(buf) >= sizeof(struct elog_header));

	buffer_clone(&copy, buf);

	/* Save copy of first event for later */
	data = buffer_get(buf);

	/* Set buffer offset pointing to the event right after bytes_to_shrink */
	while (buffer_offset(iter) < bytes_to_shrink) {
		event = buffer_get(iter);
		assert(!(event->type == ELOG_TYPE_EOL || event->length == 0));

		buffer_seek(iter, event->length);
	}

	/* Must be relative to the buffer offset */
	cleared = buffer_offset(iter) - buffer_offset(buf);
	remaining = buffer_size(iter);

	/* Overlapping copy */
	memmove(data, data + cleared, remaining);
	memset(data + remaining, ELOG_TYPE_EOL, cleared);

	/* Re-init copy to have a clean offset. Needed for init_event() */
	buffer_clone(&copy, buf);
	buffer_seek(&copy, next_available_event_offset(&copy));

	if (!eventlog_init_event(&copy, ELOG_TYPE_LOG_CLEAR, &cleared, sizeof(cleared)))
		return ELOGTOOL_EXIT_NOT_ENOUGH_BUFFER_SPACE;

	return ELOGTOOL_EXIT_SUCCESS;
}

static int cmd_list(const struct buffer *buf)
{
	const struct event_header *event;
	unsigned int count = 0;

	/* Point to the first event */
	event = buffer_get(buf) + sizeof(struct elog_header);

	while ((const void *)(event) < buffer_end(buf)) {
		if (((const void *)event + sizeof(*event)) >= buffer_end(buf)
			|| event->length <= sizeof(*event)
			|| event->length > ELOG_MAX_EVENT_SIZE
			|| ((const void *)event + event->length) >= buffer_end(buf)
			|| event->type == ELOG_TYPE_EOL)
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

static void cmd_add_usage(void)
{
	usage(argv0);

	fprintf(stderr, "\n\nSpecific to ADD command:\n"
		"\n"
		"<event_type>:          an hexadecimal number (0-255). Prefix '0x' is optional\n"
		"[event_data]:          (optional) a series of hexadecimal numbers. Must be:\n"
		"                       - len(event_data) %% 2 == 0\n"
		"                       - len(event_data) in bytes <= %zu\n"
		"\n"
		"Example:\n"
		"%s add 0x16 01ABF0  # 01ABF0 is actually three bytes: 0x01, 0xAB and 0xF0\n"
		"%s add 17           # 17 is in hexa\n",
		ELOG_MAX_EVENT_DATA_SIZE, argv0, argv0
	);
}

static int cmd_add_parse_args(uint8_t *type, uint8_t *data, size_t *data_size)
{
	char byte[3] = {0};
	int argc = 0;
	char *endptr;
	long value;
	int len;

	while (cmd_argv[argc] != NULL)
		argc++;

	if (argc != 1 && argc != 2)
		return ELOGTOOL_EXIT_BAD_ARGS;

	/* Force type to be an hexa value to be consistent with the data values */
	value = strtol(cmd_argv[0], NULL, 16);
	if (value > 255) {
		fprintf(stderr, "Error: Event type should be between 0-0xff; "
			"got: 0x%04lx\n", value);
		return ELOGTOOL_EXIT_BAD_ARGS;
	}

	*type = value;

	if (argc == 1)
		return ELOGTOOL_EXIT_SUCCESS;

	/* Assuming argc == 2 */
	len = strlen(cmd_argv[1]);

	/* Needs 2 bytes per number */
	if (len % 2 != 0) {
		fprintf(stderr,
			"Error: Event data length should be an even number; got: %d\n", len);
		return ELOGTOOL_EXIT_BAD_ARGS;
	}

	*data_size = len / 2;

	if (*data_size > ELOG_MAX_EVENT_DATA_SIZE) {
		fprintf(stderr,
			"Error: Event data length (in bytes) should be <= %zu; got: %zu\n",
			ELOG_MAX_EVENT_DATA_SIZE, *data_size);
		return ELOGTOOL_EXIT_BAD_ARGS;
	}

	for (unsigned int i = 0; i < *data_size; i++) {
		byte[0] = *cmd_argv[1]++;
		byte[1] = *cmd_argv[1]++;
		data[i] = strtol(byte, &endptr, 16);
		if (endptr != &byte[2]) {
			fprintf(stderr, "Error: Event data length contains invalid data. "
				"Only hexa digits are valid\n");
			return ELOGTOOL_EXIT_BAD_ARGS;
		}
	}

	return ELOGTOOL_EXIT_SUCCESS;
}

/* Appends an elog entry to EventLog buffer. */
static int cmd_add(const struct buffer *buf)
{
	uint8_t data[ELOG_MAX_EVENT_DATA_SIZE];
	size_t data_size = 0;
	struct buffer copy;
	uint8_t type = 0;
	size_t next_event;
	size_t threshold;
	int ret;

	if (cmd_add_parse_args(&type, data, &data_size) != ELOGTOOL_EXIT_SUCCESS) {
		cmd_add_usage();
		return ELOGTOOL_EXIT_BAD_ARGS;
	}

	buffer_clone(&copy, buf);
	buffer_seek(&copy, sizeof(struct elog_header));

	threshold = buffer_size(&copy) * 3 / 4;
	next_event = next_available_event_offset(&copy);

	if (next_event > threshold) {
		/* Shrink ~ 1/4 of the size */
		ret = shrink_buffer(&copy, buffer_size(buf) - threshold);
		if (ret != ELOGTOOL_EXIT_SUCCESS)
			return ret;
		next_event = next_available_event_offset(&copy);
	}

	buffer_seek(&copy, next_event);

	if (!eventlog_init_event(&copy, type, data, data_size))
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
			/* For commands that parse their own arguments. */
			cmd_argv = &argv[optind+1];
			argv0 = argv[0];
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
