##
##
##
## SPDX-License-Identifier: GPL-2.0-only

CFLAGS_x86_32 += -Isrc/vendorcode/siemens/hwilib

romstage-y += hwilib.c
ramstage-y += hwilib.c
