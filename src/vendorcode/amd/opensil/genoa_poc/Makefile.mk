## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += mpio

CPPFLAGS_ramstage += -I$(opensil_dir)/Include -I$(opensil_dir)/xUSL -I$(opensil_dir)/xUSL/Include -I$(opensil_dir)/xUSL/FCH -I$(opensil_dir)/xUSL/FCH/Common -I$(opensil_dir)/xSIM -I$(opensil_dir)/xPRF
CPPFLAGS_romstage += -I$(opensil_dir)/Include -I$(opensil_dir)/xUSL -I$(opensil_dir)/xUSL/Include -I$(opensil_dir)/xSIM -I$(opensil_dir)/xPRF

romstage-y += opensil_console.c
romstage-y += romstage.c

ramstage-y += acpi.c
ramstage-y += memmap.c
ramstage-y += opensil_console.c
ramstage-y += ramstage.c

$(obj)/romstage/vendorcode/amd/opensil/genoa_poc/opensil_console.o: CFLAGS_romstage += -D_MSC_EXTENSIONS=0 -DHAS_STRING_H=1 -Wno-unknown-pragmas
$(obj)/romstage/vendorcode/amd/opensil/genoa_poc/romstage.o: CFLAGS_romstage += -D_MSC_EXTENSIONS=0 -DHAS_STRING_H=1 -Wno-unknown-pragmas

$(obj)/ramstage/vendorcode/amd/opensil/genoa_poc/opensil_console.o: CFLAGS_ramstage += -D_MSC_EXTENSIONS=0 -DHAS_STRING_H=1 -Wno-unknown-pragmas
