/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef IP_CHECKSUM_H
#define IP_CHECKSUM_H
unsigned long compute_ip_checksum(const void *addr, unsigned long length);
unsigned long add_ip_checksums(unsigned long offset, unsigned long sum,
	unsigned long new);
#endif /* IP_CHECKSUM_H */
