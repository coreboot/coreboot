# SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += ../../common/psp_verstage

verstage-generic-ccopts += -Isrc/vendorcode/amd/psp_verstage/phoenix/include
verstage-generic-ccopts += -Isrc/vendorcode/amd/psp_verstage/common/include

verstage-y += svc.c
verstage-y += chipset.c
verstage-y += uart.c

verstage-y +=$(top)/src/vendorcode/amd/psp_verstage/common/bl_uapp/bl_uapp_startup.S
verstage-y += $(top)/src/vendorcode/amd/psp_verstage/common/bl_uapp/bl_uapp_end.S
