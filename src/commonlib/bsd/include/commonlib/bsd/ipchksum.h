/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _COMMONLIB_BSD_IPCHKSUM_H_
#define _COMMONLIB_BSD_IPCHKSUM_H_

#include <stddef.h>
#include <stdint.h>

uint16_t ipchksum(const void *data, size_t size);
uint16_t ipchksum_add(size_t offset, uint16_t first, uint16_t second);

#endif /* _COMMONLIB_BSD_IPCHKSUM_H_ */
