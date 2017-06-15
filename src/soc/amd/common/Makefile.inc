ifeq ($(CONFIG_SOC_AMD_COMMON),y)

cpu_incs-y += $(src)/soc/amd/common/cache_as_ram.inc

romstage-y += agesawrapper.c
romstage-y += def_callouts.c
romstage-y += heapmanager.c

ramstage-y += agesawrapper.c
ramstage-y += amd_late_init.c
ramstage-y += amd_pci_util.c
ramstage-y += def_callouts.c
ramstage-y += heapmanager.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c

subdirs-$(CONFIG_SOC_AMD_COMMON_BLOCK) += block

endif
