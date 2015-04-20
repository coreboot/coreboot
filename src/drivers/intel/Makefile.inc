subdirs-y += gma
subdirs-y += wifi
subdirs-$(CONFIG_PLATFORM_USES_FSP1_0) += fsp1_0
subdirs-$(CONFIG_PLATFORM_USES_FSP1_1) += fsp1_1
subdirs-$(CONFIG_DRIVER_INTEL_I210) += i210
