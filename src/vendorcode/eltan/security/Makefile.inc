## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += verified_boot
subdirs-y += mboot

ifneq ($(filter y,$(CONFIG_VENDORCODE_ELTAN_VBOOT) $(CONFIG_VENDORCODE_ELTAN_MBOOT)),)
CPPFLAGS_common += -I$(src)/vendorcode/eltan/security/mboot
CPPFLAGS_common += -I$(src)/vendorcode/eltan/security/include
CPPFLAGS_common += -I$(src)/vendorcode/eltan/security/verified_boot
endif
