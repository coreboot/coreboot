## SPDX-License-Identifier: GPL-2.0-only

libc-y += head.S
libc-y += main.c sysinfo.c
libc-y += timer.c coreboot.c cache.c util.S virtual.c

CFLAGS += -mcmodel=medany
