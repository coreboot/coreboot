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

# When using extended BIOS window, no sub-region within the BIOS region must
# cross 16MiB boundary from the end of the BIOS region. This is because the
# top 16MiB of the BIOS region are decoded by the standard window from
# (4G - 16M) to 4G. There is no standard section name that identifies the BIOS
# region in flashmap. This check assumes that BIOS region is placed at the top
# of SPI flash and hence calculates the boundary as flash_size - 16M. If any
# region within the SPI flash crosses this boundary, then the check complains
# and exits.

$(call add_intermediate, check-fmap-16mib-crossing, $(obj)/fmap_config.h)
	fmap_get() { awk "/$$1/ { print \$$NF }" < $<; };		\
									\
	flash_offset=$$(fmap_get FMAP_SECTION_FLASH_START);		\
	flash_size=$$(fmap_get FMAP_SECTION_FLASH_SIZE);		\
	if [ $$((flash_size)) -le $$((0x1000000)) ]; then		\
	   exit;							\
	fi;								\
	bios_16M_boundary=$$((flash_size-0x1000000)); 			\
	for x in $$(grep "FMAP_TERMINAL_SECTIONS" < $< | cut -d\" -f2);	\
	do								\
	    start=$$(fmap_get "FMAP_SECTION_$${x}_START");		\
	    size=$$(fmap_get "FMAP_SECTION_$${x}_SIZE");		\
	    start=$$((start-flash_offset));				\
	    end=$$((start+size-1));					\
	    if [ $$((start)) -lt $$((bios_16M_boundary)) ] &&		\
			[ $$((end)) -ge $$((bios_16M_boundary)) ];	\
	    then							\
	        echo "ERROR: $$x crosses 16MiB boundary";		\
	        fail=1;							\
	        break;							\
	    fi;								\
	done;								\
	exit $$fail

CBFSTOOL_ADD_CMD_OPTIONS += --ext-win-base $(CONFIG_EXT_BIOS_WIN_BASE) --ext-win-size $(CONFIG_EXT_BIOS_WIN_SIZE)

endif # CONFIG_FAST_SPI_SUPPORTS_EXT_BIOS_WINDOW
