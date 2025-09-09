## SPDX-License-Identifier: GPL-2.0-only
TOOLCPPFLAGS += -include $(top)/src/commonlib/bsd/include/commonlib/bsd/compiler.h

RBUTOOL:= $(objutil)/gigabyte/rbutool

$(RBUTOOL): $(dir)/rbutool/rbutool.c
	printf "    HOSTCC     Creating Gigabyte RBU tool\n"
	mkdir -p $(objutil)/gigabyte
	$(HOSTCC) $(TOOLCPPFLAGS) $< -o $@

ifeq ($(CONFIG_VENDOR_GIGABYTE)$(CONFIG_HAVE_GIGABYTE_BMC_BIOS_UPDATE),yy)
build_complete:: $(RBUTOOL)
	printf "    TOOL       Creating RBU image\n"
	$(RBUTOOL) -i $(obj)/coreboot.rom -o $(obj)/coreboot.rbu

endif
