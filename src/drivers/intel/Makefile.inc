subdirs-y += gma
subdirs-$(CONFIG_GENERATE_SMBIOS_TABLES) += wifi
subdirs-$(CONFIG_PLATFORM_USES_FSP1_0) += fsp1_0
subdirs-$(CONFIG_PLATFORM_USES_FSP1_1) += fsp1_1
subdirs-$(CONFIG_PLATFORM_USES_FSP2_0) += fsp2_0
subdirs-$(CONFIG_DRIVER_INTEL_I210) += i210
