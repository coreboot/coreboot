#
# This file is part of the coreboot project.
#
# Copyright (C) 2018-2019 Eltan B.V.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

# call with $1 = stage name to create rules for building the library
# for the stage and adding it to the stage's set of object files.
define vendor-security-lib
VEN_SEC_LIB_$(1) = $(obj)/external/ven_sec_lib-$(1)/vboot_fw21.a
VEN_SEC_CFLAGS_$(1) += $$(patsubst -I%,-I$(top)/%,\
		$$(patsubst $(src)/%.h,$(top)/$(src)/%.h,\
		$$(filter-out -I$(obj), $$(CPPFLAGS_$(1)))))
VEN_SEC_CFLAGS_$(1) += $$(CFLAGS_$(1))
VEN_SEC_CFLAGS_$(1) += $$($(1)-c-ccopts)
VEN_SEC_CFLAGS_$(1) += -I$(abspath $(obj)) -Wno-missing-prototypes

$$(VEN_SEC_LIB_$(1)): $(obj)/config.h
	printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	+FIRMWARE_ARCH=$$(ARCHDIR-$$(ARCH-$(1)-y)) \
	CC="$$(CC_$(1))" \
	CFLAGS="$$(VEN_SEC_CFLAGS_$(1))" VBOOT2="y" \
	$(MAKE) -C $(VBOOT_SOURCE) \
		BUILD=$$(abspath $$(dir $$(VEN_SEC_LIB_$(1)))) \
		V=$(V) \
		fwlib21
endef # vendor-security-for-stage

CFLAGS_common += -I3rdparty/vboot/firmware/2lib/include
CFLAGS_common += -I3rdparty/vboot/firmware/lib21/include

ifneq ($(filter y,$(CONFIG_VENDORCODE_ELTAN_VBOOT) $(CONFIG_VENDORCODE_ELTAN_MBOOT)),)

bootblock-y += cb_sha.c
bootblock-y += ../../../../security/vboot/vboot_logic.c
$(eval $(call vendor-security-lib,bootblock))
bootblock-srcs  += $(obj)/external/ven_sec_lib-bootblock/vboot_fw21.a

postcar-y += cb_sha.c
$(eval $(call vendor-security-lib,postcar))
postcar-srcs += $(obj)/external/ven_sec_lib-postcar/vboot_fw21.a

ramstage-y += cb_sha.c
$(eval $(call vendor-security-lib,ramstage))
ramstage-srcs += $(obj)/external/ven_sec_lib-ramstage/vboot_fw21.a

romstage-y += cb_sha.c
$(eval $(call vendor-security-lib,romstage))
romstage-srcs += $(obj)/external/ven_sec_lib-romstage/vboot_fw21.a

endif