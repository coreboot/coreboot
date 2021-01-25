## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd

bootblock-y += bootblock.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c
ifeq ($(CONFIG_SOC_INTEL_ALDERLAKE_PCH_M),y)
bootblock-y += early_gpio_m.c
ramstage-y += gpio_m.c
else
bootblock-y += early_gpio.c
ramstage-y += gpio.c
endif

verstage-$(CONFIG_CHROMEOS) += chromeos.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += romstage_fsp_params.c
romstage-y += board_id.c
romstage-y += memory.c

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-y += mainboard.c
ramstage-y += board_id.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

subdirs-y += variants/$(VARIANT_DIR)
