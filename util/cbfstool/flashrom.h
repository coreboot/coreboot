/* Copyright 2020 The ChromiumOS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Host utilities to execute flashrom command.
 */

#ifndef UTIL_CBFSTOOL_FLASHROM_H_
#define UTIL_CBFSTOOL_FLASHROM_H_

#include <stdint.h>
#include "common.h" /* from cbfstool for buffer API. */
/*
 * Both util/cbfstool and vboot_reference provide a header named flashrom.h.
 * Because -Iutil/cbfstool precedes the vboot_reference include path, use
 * #include_next to include vboot_reference's flashrom.h for
 * flashrom_read_region() and flashrom_write_region().
 */
#include_next <flashrom.h>

/**
 * Read using flashrom into an allocated buffer.
 *
 * @param buffer	The parameter that contains the buffer to use
 *			in the read operation.
 * @param region	The name of the fmap region to read.
 *
 * @return 0 on success, or < 0 on error.
 */
int flashrom_host_read(struct buffer *buffer, const char *region);

/**
 * Write using flashrom from a buffer.
 *
 * @param buffer	The parameter that contains the buffer to use
 *			in the write operation.
 * @param region	The name of the fmap region to write.
 *
 * @return 0 on success, or < 0 on error.
 */
int flashrom_host_write(struct buffer *buffer, const char *region);

#endif /* UTIL_CBFSTOOL_FLASHROM_H_ */
