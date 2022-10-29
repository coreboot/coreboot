## SPDX-License-Identifier: GPL-2.0-only
TOOLCPPFLAGS += -include $(top)/src/commonlib/bsd/include/commonlib/bsd/compiler.h

SMCBIOSINFOTOOL:= $(objutil)/supermicro/smcbiosinfo

$(SMCBIOSINFOTOOL): $(dir)/smcbiosinfo/smcbiosinfo.c
	printf "    HOSTCC     Creating SMCBIOSINFO tool\n"
	mkdir -p $(objutil)/supermicro
	$(HOSTCC) $(TOOLCPPFLAGS) $< -o $@

ifeq ($(CONFIG_VENDOR_SUPERMICRO),y)
ifneq ($(call strip_quotes, $(CONFIG_SUPERMICRO_BOARDID)),)

cbfs-files-y += smcbiosinfo.bin

smcbiosinfo.bin-file := $(obj)/mainboard/$(MAINBOARDDIR)/smcbiosinfo.bin
smcbiosinfo.bin-type := raw
smcbiosinfo.bin-compression := none

$(obj)/mainboard/$(MAINBOARDDIR)/smcbiosinfo.bin: $(SMCBIOSINFOTOOL) $(build_h)
	printf "    TOOL       Creating SMC BIOSINFO metadata\n"
	$(SMCBIOSINFOTOOL) -i $(build_h) -b $(CONFIG_SUPERMICRO_BOARDID) -o $@
endif
endif
