# SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_DRIVERS_EFI_VARIABLE_STORE)	+= efivars.c
romstage-$(CONFIG_DRIVERS_EFI_VARIABLE_STORE)	+= efivars.c
ramstage-$(CONFIG_DRIVERS_EFI_VARIABLE_STORE)	+= efivars.c
smm-$(CONFIG_DRIVERS_EFI_VARIABLE_STORE)	+= efivars.c

ramstage-$(CONFIG_DRIVERS_EFI_UPDATE_CAPSULES)	+= capsules.c

bootblock-$(CONFIG_USE_UEFI_VARIABLE_STORE)	+= option.c
romstage-$(CONFIG_USE_UEFI_VARIABLE_STORE)	+= option.c
ramstage-$(CONFIG_USE_UEFI_VARIABLE_STORE)	+= option.c
smm-$(CONFIG_USE_UEFI_VARIABLE_STORE)	+= option.c

ramstage-$(CONFIG_DRIVERS_EFI_FW_INFO)	+= info.c
