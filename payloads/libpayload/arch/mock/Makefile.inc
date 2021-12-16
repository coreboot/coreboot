# SPDX-License-Identifier: GPL-2.0-only

head.o-y += head.c

libc-y += virtual.c

libcbfs-$(CONFIG_LP_CBFS) += mock_media.c

CFLAGS += -Wno-address-of-packed-member
