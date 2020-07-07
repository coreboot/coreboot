# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_AHCI_BIOS),y)
stripped_ahcibios_id = $(call strip_quotes,$(CONFIG_AHCI_BIOS_ID))
cbfs-files-$(CONFIG_AHCI_BIOS) += pci$(stripped_ahcibios_id).rom
pci$(stripped_ahcibios_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_BIOS_FILE))
pci$(stripped_ahcibios_id).rom-type := optionrom
endif

ifeq ($(CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF),y)
bootblock-y += bootblock-fr.c
else ifeq ($(CONFIG_BOARD_LIPPERT_TOUCAN_AF),y)
bootblock-y += bootblock-tc.c
endif

romstage-y += buildOpts.c
ifeq ($(CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF),y)
romstage-y += BiosCallOuts-fr.c
romstage-y += OemCustomize-fr.c
else ifeq ($(CONFIG_BOARD_LIPPERT_TOUCAN_AF),y)
romstage-y += BiosCallOuts-tc.c
romstage-y += OemCustomize-tc.c
endif

ramstage-y += buildOpts.c
ifeq ($(CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF),y)
ramstage-y += BiosCallOuts-fr.c
ramstage-y += OemCustomize-fr.c
ramstage-y += mainboard-fr.c
else ifeq ($(CONFIG_BOARD_LIPPERT_TOUCAN_AF),y)
ramstage-y += BiosCallOuts-tc.c
ramstage-y += OemCustomize-tc.c
ramstage-y += mainboard-tc.c
endif

# Minimal SEMA watchdog support
romstage-y += sema.c
ramstage-y += sema.c
