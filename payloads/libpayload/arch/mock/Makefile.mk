# SPDX-License-Identifier: GPL-2.0-only

head.o-y += head.c

libc-y += virtual.c

CFLAGS += -Wno-address-of-packed-member
