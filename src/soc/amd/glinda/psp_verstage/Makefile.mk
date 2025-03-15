# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),y)

subdirs-y += ../../common/psp_verstage

verstage-generic-ccopts += -Isrc/vendorcode/amd/psp_verstage/glinda/include
verstage-generic-ccopts += -Isrc/vendorcode/amd/psp_verstage/common/include

verstage-y += svc.c
verstage-y += chipset.c
verstage-y += uart.c

verstage-y +=$(top)/src/vendorcode/amd/psp_verstage/common/bl_uapp/bl_uapp_startup.S
verstage-y += $(top)/src/vendorcode/amd/psp_verstage/common/bl_uapp/bl_uapp_end.S

endif
