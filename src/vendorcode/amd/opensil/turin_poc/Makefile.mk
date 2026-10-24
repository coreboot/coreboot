## SPDX-License-Identifier: GPL-2.0-only

CFLAGS_opensil = -D_MSC_EXTENSIONS=0 -DHAS_STRING_H=1 -Wno-unknown-pragmas

subdirs-y += mpio

CPPFLAGS_common += -I$(opensil_dir)/Include -I$(opensil_dir)/xUSL -I$(opensil_dir)/xUSL/Include -I$(opensil_dir)/xUSL/FCH -I$(opensil_dir)/xUSL/FCH/Common -I$(opensil_dir)/xSIM -I$(opensil_dir)/xPRF

romstage-y += opensil_console.c
romstage-y += romstage.c

ramstage-y += acpi.c
ramstage-y += memmap.c
ramstage-y += opensil_console.c
ramstage-y += ramstage.c

# These host-side objects include the generated openSIL configuration too.
# Depending on opensil.a alone does not order their compilation on a clean build.
$(addprefix $(obj)/ramstage/vendorcode/amd/opensil/turin_poc/, \
	ramstage.o memmap.o mpio/chip.o): $(OBJPATH)/$(OPENSIL_CONFIG).h

$(obj)/romstage/vendorcode/amd/opensil/turin_poc/opensil_console.o: CFLAGS_romstage += $(CFLAGS_opensil)
$(obj)/romstage/vendorcode/amd/opensil/turin_poc/romstage.o: CFLAGS_romstage += $(CFLAGS_opensil)

$(obj)/ramstage/vendorcode/amd/opensil/turin_poc/opensil_console.o: CFLAGS_ramstage += $(CFLAGS_opensil)

# These substitutions are specific to Turin's openSIL configuration.
opensil_config_sed += -e "s,\#\#PLAT_NUMBER_SOCKETS\#\#,$(CONFIG_MAX_SOCKET),"
opensil_config_sed += -e "s,\#\#IOAPIC_ID_BASE\#\#,$(CONFIG_OPENSIL_IOAPIC_ID_BASE),"
opensil_config_sed += -e "s,\#\#MPIO_EARLY_BMC_LINK_LANE\#\#,$(CONFIG_OPENSIL_MPIO_EARLY_BMC_LINK_LANE),"
