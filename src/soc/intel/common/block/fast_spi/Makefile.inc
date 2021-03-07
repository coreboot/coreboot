bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

verstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
verstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
ifeq ($(CONFIG_SPI_FLASH_SMM),y)
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c
endif

CPPFLAGS_common += -I$(src)/soc/intel/common/block/fast_spi

ifeq ($(CONFIG_FAST_SPI_SUPPORTS_EXT_BIOS_WINDOW),y)

# mmap_boot.c provides a custom boot media device for the platforms that support
# additional window for BIOS regions greater than 16MiB. This is used instead of
# the default boot media device in arch/x86/mmap_boot.c
bootblock-y += mmap_boot.c
verstage-y += mmap_boot.c
romstage-y += mmap_boot.c
postcar-y += mmap_boot.c
ramstage-y += mmap_boot.c
smm-y += mmap_boot.c

# Check to ensure that no sections in the FMAP cross 16MiB boundary if
# the platform supports split decode windows for BIOS region greater
# than 16MiB.

$(call add_intermediate, check-fmap-16mib-crossing)
check-fmap-16mib-crossing: $(obj)/fmap_config.h
	fmap_get() { awk "/$$1/ { print \$$NF }" < $<; };		\
									\
	flash_offset=$$(fmap_get FMAP_SECTION_FLASH_START);		\
	for x in $$(grep "FMAP_TERMINAL_SECTIONS" < $< | cut -d\" -f2);	\
	do								\
	    start=$$(fmap_get "FMAP_SECTION_$${x}_START");		\
	    size=$$(fmap_get "FMAP_SECTION_$${x}_SIZE");		\
	    start=$$((start-flash_offset));				\
	    end=$$((start+size-1));					\
	    if [ $$start -lt 16777216 ] && [ $$end -ge 16777216 ]; then	\
	        echo "ERROR: $$x crosses 16MiB boundary";		\
	        fail=1;							\
	        break;							\
	    fi;								\
	done;								\
	exit $$fail

CBFSTOOL_ADD_CMD_OPTIONS += --ext-win-base $(CONFIG_EXT_BIOS_WIN_BASE) --ext-win-size $(CONFIG_EXT_BIOS_WIN_SIZE)

endif # CONFIG_FAST_SPI_SUPPORTS_EXT_BIOS_WINDOW
