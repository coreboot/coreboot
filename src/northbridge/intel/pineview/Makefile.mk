# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_PINEVIEW),y)

bootblock-y += ../../../cpu/x86/early_reset.S
bootblock-y += bootblock.c

ramstage-y += memmap.c
ramstage-y += northbridge.c
ramstage-y += gma.c

pineview-gma-mainboard-ads := \
	$(wildcard $(src)/mainboard/$(MAINBOARDDIR)/gma-mainboard.ads) \
	$(wildcard $(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/gma-mainboard.ads)

ifeq ($(strip $(pineview-gma-mainboard-ads)),)
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
endif

romstage-y += romstage.c
romstage-y += memmap.c
romstage-y += raminit.c
romstage-y += early_init.c

postcar-y += memmap.c

endif
