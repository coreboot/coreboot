# SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-$(CONFIG_BOARD_STARLABS_ADL_SERIES) += bootblock.c
bootblock-$(CONFIG_BOARD_STARLABS_STARFIGHTER_SERIES) += bootblock.c

ifneq ($(filter y,$(CONFIG_BOARD_STARLABS_LITE_SERIES) $(CONFIG_BOARD_STARLABS_STARBOOK_SERIES)),)
verstage-$(CONFIG_VBOOT) += vboot.c
romstage-$(CONFIG_VBOOT) += vboot.c
endif

ramstage-$(CONFIG_BOARD_STARLABS_STARBOOK_SERIES) += mainboard.c
ramstage-$(CONFIG_BOARD_STARLABS_STARFIGHTER_SERIES) += mainboard.c

ifneq ($(filter y,$(CONFIG_BOARD_STARLABS_LITE_SERIES) \
		$(CONFIG_BOARD_STARLABS_LABTOP_KBL) $(CONFIG_BOARD_STARLABS_LABTOP_CML)),)
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
endif

subdirs-$(CONFIG_VENDOR_STARLABS) += cfr
subdirs-$(CONFIG_VENDOR_STARLABS) += hda
subdirs-$(CONFIG_BOARD_STARLABS_STARFIGHTER_SERIES) += touchpad
subdirs-$(CONFIG_VENDOR_STARLABS) += powercap
subdirs-$(CONFIG_VENDOR_STARLABS) += fsp_params
subdirs-$(CONFIG_VENDOR_STARLABS) += pin_mux
subdirs-$(CONFIG_VENDOR_STARLABS) += smbios

ramstage-$(CONFIG_STARLABS_NVME_POWER_SEQUENCE) += nvme_seq.c

CPPFLAGS_common += -I$(src)/mainboard/starlabs/common/include

ramstage-$(CONFIG_STARLABS_ACPI_EFI_OPTION_SMI) += gnvs.c
smm-$(CONFIG_STARLABS_ACPI_EFI_OPTION_SMI) += smihandler.c
