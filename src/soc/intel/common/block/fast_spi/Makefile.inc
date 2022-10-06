## SPDX-License-Identifier: GPL-2.0-only
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

# If the platform supports extended window and the SPI flash size is greater
# than 16MiB, then create a mapping for the extended window as well.
# The assumptions here are:
# 1. Top 16MiB is still decoded in the fixed decode window just below 4G
# boundary.
# 2. Rest of the SPI flash below the top 16MiB is mapped at the top of extended
# window. Even though the platform might support a larger extended window, the
# SPI flash part used by the mainboard might not be large enough to be mapped
# in the entire window. In such cases, the mapping is assumed to be in the top
# part of the extended window with the bottom part remaining unused.
#
# Example:
# ext_win_base = 0xF8000000
# ext_win_size = 32 # MiB
# ext_win_limit = ext_win_base + ext_win_size - 1 = 0xF9FFFFFF
#
# If SPI flash is 32MiB, then top 16MiB is mapped from 0xFF000000 - 0xFFFFFFFF
# whereas the bottom 16MiB is mapped from 0xF9000000 - 0xF9FFFFFF. The extended
# window 0xF8000000 - 0xF8FFFFFF remains unused.
#

ifeq ($(call int-gt, $(CONFIG_ROM_SIZE) 0x1000000), 1)
DEFAULT_WINDOW_SIZE=0x1000000
DEFAULT_WINDOW_FLASH_BASE=$(call int-subtract, $(CONFIG_ROM_SIZE) $(DEFAULT_WINDOW_SIZE))
DEFAULT_WINDOW_MMIO_BASE=0xff000000
EXT_WINDOW_FLASH_BASE=0
EXT_WINDOW_SIZE=$(DEFAULT_WINDOW_FLASH_BASE)
EXT_WINDOW_MMIO_BASE=$(call int-subtract, $(call int-add, $(CONFIG_EXT_BIOS_WIN_BASE) $(CONFIG_EXT_BIOS_WIN_SIZE)) \
	$(EXT_WINDOW_SIZE))
CBFSTOOL_ADD_CMD_OPTIONS += --mmap $(DEFAULT_WINDOW_FLASH_BASE):$(DEFAULT_WINDOW_MMIO_BASE):$(DEFAULT_WINDOW_SIZE)
CBFSTOOL_ADD_CMD_OPTIONS += --mmap $(EXT_WINDOW_FLASH_BASE):$(EXT_WINDOW_MMIO_BASE):$(EXT_WINDOW_SIZE)
endif


endif # CONFIG_FAST_SPI_SUPPORTS_EXT_BIOS_WINDOW
