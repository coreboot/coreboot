# SPDX-License-Identifier: GPL-2.0-only

all-$(CONFIG_DRIVERS_EFI_VARIABLE_STORE)	+= efivars.c
smm-$(CONFIG_DRIVERS_EFI_VARIABLE_STORE)	+= efivars.c

all-$(CONFIG_USE_UEFI_VARIABLE_STORE)	+= option.c
smm-$(CONFIG_USE_UEFI_VARIABLE_STORE)	+= option.c

ramstage-$(CONFIG_DRIVERS_EFI_FW_INFO)	+= info.c
