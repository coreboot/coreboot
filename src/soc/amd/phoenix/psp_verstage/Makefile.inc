# SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += ../../common/psp_verstage

verstage-generic-ccopts += -I$(src)/soc/amd/common/psp_verstage/include
verstage-generic-ccopts += -Isrc/vendorcode/amd/fsp/phoenix/include
verstage-generic-ccopts += -Isrc/vendorcode/amd/fsp/common/include

verstage-y += svc.c
verstage-y += chipset.c
verstage-y += uart.c

verstage-y +=$(top)/src/vendorcode/amd/fsp/common/bl_uapp/bl_uapp_startup.S
verstage-y += $(top)/src/vendorcode/amd/fsp/common/bl_uapp/bl_uapp_end.S
