/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _BOOT_DEVICE_H
#define _BOOT_DEVICE_H

#include <stddef.h>

/**
 * This is a boot device access function, which is used by libpayload to read data from
 * the flash memory (or other boot device). It has to be implemented by payloads that want
 * to use FMAP or libcbfs.
 *
 * @param buf The output buffer to which the data should be written to.
 * @param offset Absolute offset in bytes of the requested boot device memory area. Not aligned.
 * @param size Size in bytes of the requested boot device memory area. Not aligned.
 *
 * @returns Number of bytes returned to the buffer, or negative value on error. Typically should
 *          be equal to the `size`, and not aligned forcefully.
 */
ssize_t boot_device_read(void *buf, size_t offset, size_t size);

#endif /* _BOOT_DEVICE_H */
