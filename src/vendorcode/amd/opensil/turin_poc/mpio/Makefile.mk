## SPDX-License-Identifier: GPL-2.0-only

ramstage-y += chip.c

$(obj)/ramstage/vendorcode/amd/opensil/turin_poc/mpio/chip.o: CFLAGS_ramstage += $(CFLAGS_opensil)
