/*
 * This file is part of the LinuxBIOS project.
 *
 * It was taken from kexec-tools 1.101.
 *
 * Copyright (C) 2003,2004 Eric Biederman <ebiederm@xmission.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef IP_CHECKSUM_H
#define IP_CHECKSUM_H

unsigned long compute_ip_checksum(void *addr, unsigned long length);
unsigned long add_ip_checksums(unsigned long offset, unsigned long sum, unsigned long new);
unsigned long negate_ip_checksum(unsigned long sum);

#endif /* IP_CHECKSUM_H */
