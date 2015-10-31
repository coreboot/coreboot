#
# This file is part of the libpayload project.
#
# Copyright (C) 2014 Imagination Technologies
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; version 2 of
# the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

###############################################################################
CFLAGS += -march=mips32r2 -mxgot

head.o-y += head.S

libc-y += cache.c
libc-y += coreboot.c
libc-y += dummy_media.c
libc-y += exception_asm.S
libc-y += exception.c
libc-y += gdb.c
libc-y += main.c
libc-y += selfboot.c
libc-y += sysinfo.c
libc-y += string.c
libc-y += timer.c
libc-y += util.S
