# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_AHCI_BIOS),y)
stripped_ahcibios_id = $(call strip_quotes,$(CONFIG_AHCI_BIOS_ID))
cbfs-files-$(CONFIG_AHCI_BIOS) += pci$(stripped_ahcibios_id).rom
pci$(stripped_ahcibios_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_BIOS_FILE))
pci$(stripped_ahcibios_id).rom-type := optionrom
endif

bootblock-y += variants/$(VARIANT_DIR)/bootblock.c

romstage-y += buildOpts.c
romstage-y += variants/$(VARIANT_DIR)/BiosCallOuts.c
romstage-y += variants/$(VARIANT_DIR)/OemCustomize.c

ramstage-y += buildOpts.c
ramstage-y += variants/$(VARIANT_DIR)/BiosCallOuts.c
ramstage-y += variants/$(VARIANT_DIR)/OemCustomize.c
ramstage-y += variants/$(VARIANT_DIR)/mainboard.c

# Minimal SEMA watchdog support
romstage-y += sema.c
ramstage-y += sema.c
