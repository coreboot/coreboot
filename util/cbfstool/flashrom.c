/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* For strdup */
#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h" /* from cbfstool for buffer API. */
#include "subprocess.h" /* from vboot_reference */
#include "flashrom.h"

#define FLASHROM_EXEC_NAME "flashrom"
#define FLASHROM_PROGRAMMER_INTERNAL_AP "host"

/**
 * Helper to create a temporary file.
 *
 * @param path_out	An output pointer for the filename. Caller should free.
 *
 * @return 0 on success, -1 for file open error, or -2 for write error.
 */
static int create_temp_file(char **path_out)
{
	int fd;
	int rv;
	char *path;
	mode_t umask_save;

#if defined(__FreeBSD__)
#define P_tmpdir "/tmp"
#endif
	*path_out = NULL;
	path = strdup(P_tmpdir "/flashrom.XXXXXX");
	/* Set the umask before mkstemp for security considerations. */
	umask_save = umask(077);
	fd = mkstemp(path);
	umask(umask_save);
	if (fd < 0) {
		rv = -1;
		goto fail;
	}

	close(fd);
	*path_out = path;

	return 0;
fail:
	free(path);
	return rv;
}

static int run_flashrom(const char *const argv[])
{
	int status = subprocess_run(argv, &subprocess_null, &subprocess_null,
				    &subprocess_null);
	if (status) {
		fprintf(stderr, "Flashrom invocation failed (exit status %d):",
			status);
		for (const char *const *argp = argv; *argp; argp++)
			fprintf(stderr, " %s", *argp);
		fprintf(stderr, "\n");
		return -1;
	}

	return 0;
}

int flashrom_host_read(struct buffer *buffer, const char *region)
{
	char *tmpfile;
	char region_param[PATH_MAX];
	int rv;

	if (create_temp_file(&tmpfile) != 0)
		return -1;
	if (region)
		snprintf(region_param, sizeof(region_param), "%s:%s", region,
			 tmpfile);
	const char *const argv[] = {
		FLASHROM_EXEC_NAME,
		"-p",
		FLASHROM_PROGRAMMER_INTERNAL_AP,
		"-r",
		region ? "-i" : tmpfile,
		region ? region_param : NULL,
		NULL,
	};
	rv = run_flashrom(argv);
	if (!rv)
		rv = buffer_from_file(buffer, tmpfile);

	unlink(tmpfile);
	free(tmpfile);

	return rv;
}

int flashrom_host_write(struct buffer *buffer, const char *region)
{
	char *tmpfile;
	char region_param[PATH_MAX];
	int rv;

	if (create_temp_file(&tmpfile) != 0)
		return -1;
	if (buffer_write_file(buffer, tmpfile) != 0) {
		rv = -2;
		goto fail;
	}

	if (region)
		snprintf(region_param, sizeof(region_param), "%s:%s", region,
			 tmpfile);
	const char *const argv[] = {
		FLASHROM_EXEC_NAME,
		"-p",
		FLASHROM_PROGRAMMER_INTERNAL_AP,
		"--noverify-all",
		"-w",
		region ? "-i" : tmpfile,
		region ? region_param : NULL,
		NULL,
	};

	rv = run_flashrom(argv);

fail:
	unlink(tmpfile);
	free(tmpfile);

	return rv;
}
