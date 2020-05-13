## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp2_0/quark

bootblock-y += gpio.c
bootblock-y += reg_access.c

verstage-y += gpio.c
verstage-y += reg_access.c
verstage-$(CONFIG_VBOOT) += vboot.c

romstage-y += gpio.c
romstage-y += reg_access.c
romstage-$(CONFIG_COMMONLIB_STORAGE_SD) += sd.c
romstage-$(CONFIG_VBOOT) += vboot.c

postcar-y += gpio.c
postcar-y += reg_access.c

ramstage-y += gpio.c
ramstage-y += reg_access.c
ramstage-$(CONFIG_COMMONLIB_STORAGE_SD) += sd.c
