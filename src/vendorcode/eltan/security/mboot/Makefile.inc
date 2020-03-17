##
## This file is part of the coreboot project.
##
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ifneq ($(filter y,$(CONFIG_VENDORCODE_ELTAN_VBOOT) $(CONFIG_VENDORCODE_ELTAN_MBOOT)),)
CPPFLAGS_common += -I$(src)/security/mboot
endif

ifeq ($(CONFIG_VENDORCODE_ELTAN_MBOOT),y)
postcar-y += mboot.c

ramstage-y += mboot.c
ramstage-y += mboot_func.c

romstage-y += mboot.c
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)

endif # CONFIG_VENDORCODE_ELTAN_VBOOT or CONFIG_VENDORCODE_ELTAN_MBOOT
