# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_EC_CLEVO_IT5570E),y)

bootblock-y += early_init.c
bootblock-y += i2ec.c

ramstage-y += ec.c
ramstage-y += commands.c
ramstage-y += smbios.c
ramstage-y += ssdt.c

smm-y += commands.c
smm-y += smihandler.c

endif
