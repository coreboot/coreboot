# SPDX-License-Identifier: GPL-2.0-only

all_x86-$(CONFIG_DRIVERS_EFI_VARIABLE_STORE)	+= efivars.c
smm-$(CONFIG_DRIVERS_EFI_VARIABLE_STORE)	+= efivars.c

ramstage-$(CONFIG_DRIVERS_EFI_UPDATE_CAPSULES)	+= capsules.c

all_x86-$(CONFIG_USE_UEFI_VARIABLE_STORE)	+= option.c
smm-$(CONFIG_USE_UEFI_VARIABLE_STORE)	+= option.c

ramstage-$(CONFIG_DRIVERS_EFI_FW_INFO)	+= info.c
