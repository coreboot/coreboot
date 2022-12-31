# SPDX-License-Identifier: GPL-2.0-only

verstage-generic-ccopts += -I$(src)/vendorcode/amd/fsp/cezanne/include

verstage-generic-ccopts += -I$(src)/soc/amd/common/psp_verstage/include

subdirs-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += ../../common/psp_verstage

verstage-y += svc.c
verstage-y += chipset.c
verstage-y += uart.c

verstage-y += $(top)/src/vendorcode/amd/fsp/cezanne/bl_uapp/bl_uapp_startup.S
verstage-y += $(top)/src/vendorcode/amd/fsp/cezanne/bl_uapp/bl_uapp_end.S
