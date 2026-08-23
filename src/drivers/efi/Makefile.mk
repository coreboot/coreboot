# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_DRIVERS_EFI_VARIABLE_STORE),y)
# VariableFormat.h lives in MdeModulePkg, which uefi_2.4 binding omits.
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/UDK2017/MdeModulePkg/Include
endif

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
