## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_VENDORCODE_ELTAN_MBOOT),y)
postcar-y += mboot.c

ramstage-y += mboot.c
ramstage-y += mboot_func.c

romstage-y += mboot.c
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)

endif # CONFIG_VENDORCODE_ELTAN_VBOOT or CONFIG_VENDORCODE_ELTAN_MBOOT
