/*
 * uio_usbdebug - Run coreboot's usbdebug driver in userspace
 *
 * Copyright (C) 2013 Nico Huber <nico.h@gmx.de>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

/* coreboot's arch/io.h conflicts with libc's sys/io.h, so declare this here: */
int ioperm(unsigned long from, unsigned long num, int turn_on);

#include <arch/io.h>
#include <usbdebug.h>

void *ehci_bar;

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <uio-dev>\n", argv[0]);
		return 1;
	}
	const int fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("Failed to open uio device");
		return 2;
	}
	ehci_bar =
		mmap(NULL, 1 << 8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == ehci_bar) {
		perror("Failed to map ehci bar");
		close(fd);
		return 3;
	}

	ioperm(0x80, 1, 1);

	usbdebug_init((unsigned)ehci_bar);

	munmap(ehci_bar, 1 << 8);
	close(fd);
	return 0;
}
