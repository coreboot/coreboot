# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_COMMONLIB_STORAGE),y)

bootblock-y += sd_mmc.c
bootblock-y += storage.c

verstage-y += sd_mmc.c
verstage-y += storage.c

romstage-y += sd_mmc.c
romstage-y += storage.c

postcar-y += sd_mmc.c
postcar-y += storage.c

ramstage-y += sd_mmc.c
ramstage-y += storage.c

# Determine the type of controller being used
ifeq ($(CONFIG_SDHCI_CONTROLLER),y)
bootblock-y += pci_sdhci.c
bootblock-y += sdhci.c
bootblock-$(CONFIG_SDHCI_ADMA_IN_BOOTBLOCK) += sdhci_adma.c
bootblock-y += sdhci_display.c

verstage-y += pci_sdhci.c
verstage-y += sdhci.c
verstage-$(CONFIG_SDHCI_ADMA_IN_VERSTAGE) += sdhci_adma.c
verstage-y += sdhci_display.c

romstage-y += pci_sdhci.c
romstage-y += sdhci.c
romstage-$(CONFIG_SDHCI_ADMA_IN_ROMSTAGE) += sdhci_adma.c
romstage-y += sdhci_display.c

postcar-y += pci_sdhci.c
postcar-y += sdhci.c
postcar-y += sdhci_adma.c
postcar-y += sdhci_display.c

ramstage-y += pci_sdhci.c
ramstage-y += sdhci.c
ramstage-y += sdhci_adma.c
ramstage-y += sdhci_display.c

# Determine if the bounce buffer is necessary
ifeq ($(CONFIG_SDHCI_BOUNCE_BUFFER),y)
bootblock-y += bouncebuf.c
verstage-y += bouncebuf.c
romstage-y += bouncebuf.c
postcar-y += bouncebuf.c
ramstage-y += bouncebuf.c
endif # CONFIG_SDHCI_BOUNCE_BUFFER

endif # CONFIG_SDHCI_CONTROLLER

# Determine if MultiMediaCards or embedded MMC devices are supported
ifeq ($(CONFIG_COMMONLIB_STORAGE_MMC),y)
bootblock-y += mmc.c
verstage-y += mmc.c
romstage-y += mmc.c
postcar-y += mmc.c
ramstage-y += mmc.c
endif # CONFIG_COMMONLIB_STORAGE_MMC

# Determine if Secure Digital cards are supported
ifeq ($(CONFIG_COMMONLIB_STORAGE_SD),y)
bootblock-y += sd.c
verstage-y += sd.c
romstage-y += sd.c
postcar-y += sd.c
ramstage-y += sd.c
endif # CONFIG_COMMONLIB_STORAGE_SD

# Determine if erase operations are supported
ifeq ($(CONFIG_STORAGE_ERASE),y)
bootblock-$(CONFIG_STORAGE_EARLY_ERASE) += storage_erase.c
verstage-$(CONFIG_STORAGE_EARLY_ERASE) += storage_erase.c
romstage-y += storage_erase.c
postcar-y += storage_erase.c
ramstage-y += storage_erase.c
endif # CONFIG_STORAGE_ERASE

# Determine if write operations are supported
ifeq ($(CONFIG_STORAGE_WRITE),y)
bootblock-$(CONFIG_STORAGE_EARLY_WRITE) += storage_write.c
verstage-$(CONFIG_STORAGE_EARLY_WRITE) += storage_write.c
romstage-y += storage_write.c
postcar-y += storage_write.c
ramstage-y += storage_write.c
endif # CONFIG_STORAGE_WRITE

endif # CONFIG_COMMONLIB_STORAGE
