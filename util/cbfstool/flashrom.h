/* Copyright 2020 The ChromiumOS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Host utilities to execute flashrom command.
 */

#include <stdint.h>
#include "common.h" /* from cbfstool for buffer API. */

/**
 * Read using flashrom into an allocated buffer.
 *
 * @param buffer	The parameter that contains the buffer to use
 *			in the read operation.
 * @param region	The name of the fmap region to read, or NULL to
 *			read the entire flash chip.
 *
 * @return 0 on success, or < 0 on error.
 */
int flashrom_host_read(struct buffer *buffer, const char *region);

/**
 * Write using flashrom from a buffer.
 *
 * @param buffer	The parameter that contains the buffer to use
 *			in the write operation.
 * @param regions	The name of the fmap region to write, or NULL to
 *			write the entire flash chip.
 *
 * @return 0 on success, or < 0 on error.
 */
int flashrom_host_write(struct buffer *buffer, const char *region);
