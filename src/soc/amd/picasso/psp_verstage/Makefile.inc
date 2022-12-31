# SPDX-License-Identifier: GPL-2.0-only

verstage-generic-ccopts += -I$(src)/vendorcode/amd/fsp/picasso/include

verstage-y += svc.c
verstage-y += chipset.c
verstage-y += uart.c

verstage-y += $(top)/src/vendorcode/amd/fsp/picasso/bl_uapp/bl_uapp_startup.S
verstage-y += $(top)/src/vendorcode/amd/fsp/picasso/bl_uapp/bl_uapp_end.S
