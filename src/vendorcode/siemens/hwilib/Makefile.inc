## SPDX-License-Identifier: GPL-2.0-only

romstage-y += hwilib.c
ramstage-y += hwilib.c

CPPFLAGS_common += -I$(src)/vendorcode/siemens/hwilib
