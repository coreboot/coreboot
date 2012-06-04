/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Rudolf Marek <r.marek@assembler.cz>
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

#include <stdlib.h>
#include <console/console.h>
#include <serialice_host.h>
#include <ulinux.h>

#if CONFIG_ULINUX_VALGRIND
#include "valgrind.h"
#include "memcheck.h"
extern unsigned char _heap, _eheap;
#endif

#define NR_EXIT 1
#define NR_MMAP 192
#define NR_READ 3
#define NR_WRITE 4
#define NR_OPEN 5
#define NR_CLOSE 6
#define NR_IOCTL 54
#define NR_FCNTL 55

#define TCGETS 0x5401
#define TCSETS 0x5402
#define TCFLSH 0x540b

int ulinux_errno;


unsigned long __stack_chk_guard = 0xdeadbeef;

void __stack_chk_init(void)
{
}

void __stack_chk_fail(void);

void __attribute__ ((noreturn)) __stack_chk_fail(void)
{
	die("Stack SMASHED\n");
}

void ulinux_init(void)
{
	/* Map last 64KB of 1MB */
	ulinux_mmap((960 * 1024), 64 * 1024);

#if CONFIG_ULINUX_VALGRIND
	VALGRIND_CREATE_MEMPOOL(&_heap, 8, 0);
	VALGRIND_MAKE_MEM_NOACCESS(&_heap,  &_eheap - &_heap);
#endif

//	serialice_init();

}

void *ulinux_mmap(unsigned long addr, unsigned int size)
{
	return (void *) _call_linux(NR_MMAP, (uint32_t) addr, size,
				    (uint32_t) 3, 0x32, -1, 0);
}

int ulinux_write(int fd, const void *buf, unsigned int count)
{
	return _call_linux(NR_WRITE, (uint32_t) fd, (uint32_t) buf,
			   (uint32_t) count, 0, 0, 0);
}

int ulinux_close(int fd)
{
	return _call_linux(NR_CLOSE, (uint32_t) fd, 0, 0, 0, 0, 0);
}

int ulinux_read(int fd, void *buf, unsigned int count)
{
	int ret =
	    _call_linux(NR_READ, (uint32_t) fd, (uint32_t) buf,
			(uint32_t) count,
			0, 0, 0);
	ulinux_errno = (ret < 0) ? -ret : 0;
	return ret;
}


int ulinux_open(const char *pathname, int flags)
{
	return _call_linux(NR_OPEN, (uint32_t) pathname, (uint32_t) flags,
			   (uint32_t) 0, 0, 0, 0);

}


int ulinux_ioctl(int fd, int request)
{
	return _call_linux(NR_IOCTL, (uint32_t) fd, (uint32_t) request,
			   (uint32_t) 0, 0, 0, 0);
}

int ulinux_tcgetattr(int fd, struct termios *termios_p)
{
	return _call_linux(NR_IOCTL, (uint32_t) fd, TCGETS,
			   (uint32_t) termios_p, 0, 0, 0);
}


int ulinux_cfsetispeed(struct termios *termios_p, speed_t speed)
{
	termios_p->c_ispeed = speed;
	return 0;
}

int ulinux_cfsetospeed(struct termios *termios_p, speed_t speed)
{
	termios_p->c_ospeed = speed;
	return 0;
}

int ulinux_tcsetattr(int fd, int optional_actions,
		     const struct termios *termios_p)
{
	return _call_linux(NR_IOCTL, (uint32_t) fd, TCSETS,
			   (uint32_t) termios_p,
			   (uint32_t) optional_actions, 0, 0);

}

void ulinux_exit(int status)
{
	_call_linux(NR_EXIT, status, 0, 0, 0, 0, 0);
}

int ulinux_fcntl(int fd, int cmd, int a)
{
	return _call_linux(NR_FCNTL, (uint32_t) fd, (uint32_t) cmd,
			   (uint32_t) a, 0, 0, 0);
}

int ulinux_tcflush(int fd, int queue_selector)
{
	return _call_linux(NR_IOCTL, (uint32_t) fd, TCFLSH,
			   (uint32_t) queue_selector, 0, 0, 0);

}
