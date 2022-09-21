# SPDX-License-Identifier: GPL-2.0-only

subdirs-y += gnat

ifeq ($(CONFIG_UBSAN),y)
ramstage-y += ubsan.c
CFLAGS_ramstage += -fsanitize=undefined
endif

# Ensure that asan_shadow_offset_callback patch is applied to GCC before ASan is used.
CFLAGS_asan += -fsanitize=kernel-address --param asan-use-shadow-offset-callback=1 \
		--param asan-stack=1 -fsanitize-address-use-after-scope \
		--param asan-instrumentation-with-call-threshold=0 \
		--param use-after-scope-direct-emission-threshold=0

ifeq ($(CONFIG_ASAN_IN_ROMSTAGE),y)
romstage-y += asan.c
CFLAGS_asan += --param asan-globals=0
CFLAGS_romstage += $(CFLAGS_asan)
# Allow memory access without __asan_load and __asan_store checks.
$(obj)/romstage/lib/asan.o: CFLAGS_asan =
endif

ifeq ($(CONFIG_ASAN_IN_RAMSTAGE),y)
ramstage-y += asan.c
CFLAGS_asan += --param asan-globals=1
CFLAGS_ramstage += $(CFLAGS_asan)
$(obj)/ramstage/lib/asan.o: CFLAGS_asan =
endif

all-y += list.c

decompressor-y += decompressor.c
$(call src-to-obj,decompressor,$(dir)/decompressor.c): $(objcbfs)/bootblock.lz4
$(call src-to-obj,decompressor,$(dir)/decompressor.c): CCACHE_EXTRAFILES=$(objcbfs)/bootblock.lz4
# Must reset CCACHE_EXTRAFILES or make applies it transitively to dependencies.
$(objcbfs)/bootblock.lz4: CCACHE_EXTRAFILES=

decompressor-y += delay.c
decompressor-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
decompressor-y += memchr.c
decompressor-y += memcmp.c
decompressor-$(CONFIG_CBFS_VERIFICATION) += metadata_hash.c
decompressor-y += prog_ops.c
decompressor-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

bootblock-y += bootblock.c
bootblock-y += prog_loaders.c
bootblock-y += prog_ops.c
bootblock-y += cbfs.c
bootblock-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
bootblock-y += libgcc.c
ifneq ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),y)
bootblock-$(CONFIG_CBFS_VERIFICATION) += metadata_hash.c
else # ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),y)
verstage-$(CONFIG_CBFS_VERIFICATION) += metadata_hash.c
endif # ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),y)
bootblock-$(CONFIG_GENERIC_UDELAY) += timer.c

bootblock-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

bootblock-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
bootblock-y += delay.c
bootblock-y += memchr.c
bootblock-y += memcmp.c
bootblock-y += boot_device.c
bootblock-y += fmap.c

verstage-y += prog_loaders.c
verstage-y += prog_ops.c
verstage-y += delay.c
verstage-y += cbfs.c
verstage-y += halt.c
verstage-y += fmap.c
verstage-y += libgcc.c
verstage-y += memcmp.c
verstage-y += string.c

verstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
verstage-y += boot_device.c
verstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c

verstage-$(CONFIG_GENERIC_UDELAY) += timer.c
verstage-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c

romstage-$(CONFIG_VENDOR_EMULATION) += ramdetect.c
romstage-y += prog_loaders.c
romstage-y += prog_ops.c
romstage-y += memchr.c
romstage-y += memcmp.c
$(foreach arch,$(ARCH_SUPPORTED),\
	    $(eval rmodules_$(arch)-y += memcmp.c) \
	    $(eval rmodules_$(arch)-y += rmodule.ld))

romstage-y += fmap.c
romstage-y += delay.c
romstage-y += cbfs.c
romstage-$(CONFIG_COMPRESS_RAMSTAGE) += lzma.c lzmadecode.c
romstage-y += libgcc.c
romstage-y += memrange.c
romstage-$(CONFIG_PRIMITIVE_MEMTEST) += primitive_memtest.c
ramstage-$(CONFIG_PRIMITIVE_MEMTEST) += primitive_memtest.c
romstage-y += ramtest.c
romstage-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
ramstage-y += region_file.c
romstage-y += region_file.c
ramstage-y += romstage_handoff.c
romstage-y += romstage_handoff.c
romstage-y += selfboot.c
romstage-y += stack.c
romstage-y += rtc.c
ramstage-y += rtc.c

romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
romstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c

romstage-y += compute_ip_checksum.c
romstage-y += dimm_info_util.c
ifeq ($(CONFIG_COMPILER_GCC),y)
bootblock-$(CONFIG_ARCH_BOOTBLOCK_X86_32) += gcc.c
verstage-$(CONFIG_ARCH_VERSTAGE_X86_32) += gcc.c
romstage-$(CONFIG_ARCH_ROMSTAGE_X86_32) += gcc.c
ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += gcc.c
smm-y += gcc.c
endif

romstage-$(CONFIG_GENERIC_UDELAY) += timer.c

ramstage-$(CONFIG_VENDOR_EMULATION) += ramdetect.c
ramstage-y += prog_loaders.c
ramstage-y += prog_ops.c
ramstage-y += hardwaremain.c
ramstage-y += selfboot.c
ramstage-y += coreboot_table.c
ramstage-y += bootmem.c
ramstage-y += fmap.c
ramstage-y += memchr.c
ramstage-y += memcmp.c
ramstage-y += malloc.c
ramstage-y += dimm_info_util.c
ramstage-y += delay.c
ramstage-y += fallback_boot.c
ramstage-y += compute_ip_checksum.c
ramstage-y += cbfs.c
ramstage-y += lzma.c lzmadecode.c
ramstage-y += stack.c
ramstage-y += hexstrtobin.c
ramstage-y += wrdd.c
ramstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
ramstage-$(CONFIG_BMP_LOGO) += bmp_logo.c
ramstage-$(CONFIG_BOOTSPLASH) += bootsplash.c
ramstage-$(CONFIG_BOOTSPLASH) += jpeg.c
ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
ramstage-$(CONFIG_COVERAGE) += libgcov.c
ramstage-y += dp_aux.c
ramstage-y += edid.c
ramstage-y += edid_fill_fb.c
ramstage-y += memrange.c
ramstage-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
ramstage-$(CONFIG_GENERIC_UDELAY) += timer.c
ramstage-y += b64_decode.c
ramstage-$(CONFIG_ACPI_NHLT) += nhlt.c
ramstage-$(CONFIG_FLATTENED_DEVICE_TREE) += device_tree.c
ramstage-$(CONFIG_PAYLOAD_FIT_SUPPORT) += fit.c
ramstage-$(CONFIG_PAYLOAD_FIT_SUPPORT) += fit_payload.c

romstage-$(CONFIG_TIMER_QUEUE) += timer_queue.c
ramstage-$(CONFIG_TIMER_QUEUE) += timer_queue.c

romstage-$(CONFIG_COOP_MULTITASKING) += thread.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread.c

romstage-y += cbmem_common.c
romstage-y += imd_cbmem.c
romstage-y += imd.c

ramstage-y += cbmem_common.c
ramstage-y += imd_cbmem.c
ramstage-y += imd.c

postcar-$(CONFIG_VENDOR_EMULATION) += ramdetect.c
postcar-y += cbmem_common.c
postcar-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
postcar-y += imd_cbmem.c
postcar-y += imd.c
postcar-y += romstage_handoff.c

bootblock-y += hexdump.c
postcar-y += hexdump.c
ramstage-y += hexdump.c
romstage-y += hexdump.c
verstage-y += hexdump.c
smm-y += hexdump.c

bootblock-$(CONFIG_FW_CONFIG) += fw_config.c
verstage-$(CONFIG_FW_CONFIG) += fw_config.c
romstage-$(CONFIG_FW_CONFIG) += fw_config.c
ramstage-$(CONFIG_FW_CONFIG) += fw_config.c

bootblock-$(CONFIG_ESPI_DEBUG) += espi_debug.c
verstage-$(CONFIG_ESPI_DEBUG) += espi_debug.c
romstage-$(CONFIG_ESPI_DEBUG) += espi_debug.c
ramstage-$(CONFIG_ESPI_DEBUG) += espi_debug.c

bootblock-$(CONFIG_REG_SCRIPT) += reg_script.c
verstage-$(CONFIG_REG_SCRIPT) += reg_script.c
romstage-$(CONFIG_REG_SCRIPT) += reg_script.c
ramstage-$(CONFIG_REG_SCRIPT) += reg_script.c

ramstage-$(CONFIG_TSEG_STAGE_CACHE) += ext_stage_cache.c
romstage-$(CONFIG_TSEG_STAGE_CACHE) += ext_stage_cache.c
postcar-$(CONFIG_TSEG_STAGE_CACHE) += ext_stage_cache.c

ramstage-$(CONFIG_CBMEM_STAGE_CACHE) += cbmem_stage_cache.c
romstage-$(CONFIG_CBMEM_STAGE_CACHE) += cbmem_stage_cache.c
postcar-$(CONFIG_CBMEM_STAGE_CACHE) += cbmem_stage_cache.c

romstage-y += boot_device.c
ramstage-y += boot_device.c

smm-y += boot_device.c
smm-y += delay.c
smm-y += fmap.c
smm-y += cbfs.c memcmp.c
smm-$(CONFIG_GENERIC_UDELAY) += timer.c
ifeq ($(CONFIG_DEBUG_SMI),y)
smm-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
endif

bootblock-y += version.c
romstage-y += version.c
ramstage-y += version.c
smm-y += version.c
verstage-y += version.c
postcar-y += version.c

$(call src-to-obj,bootblock,$(dir)/version.c) : $(obj)/build.h
$(call src-to-obj,romstage,$(dir)/version.c) : $(obj)/build.h
$(call src-to-obj,ramstage,$(dir)/version.c) : $(obj)/build.h
$(call src-to-obj,smm,$(dir)/version.c) : $(obj)/build.h
$(call src-to-obj,verstage,$(dir)/version.c) : $(obj)/build.h
$(call src-to-obj,postcar,$(dir)/version.c) : $(obj)/build.h

$(call src-to-obj,bootblock,$(dir)/fmap.c) : $(obj)/fmap_config.h
$(call src-to-obj,romstage,$(dir)/fmap.c) : $(obj)/fmap_config.h
$(call src-to-obj,ramstage,$(dir)/fmap.c) : $(obj)/fmap_config.h
$(call src-to-obj,smm,$(dir)/fmap.c) : $(obj)/fmap_config.h
$(call src-to-obj,verstage,$(dir)/fmap.c) : $(obj)/fmap_config.h
$(call src-to-obj,postcar,$(dir)/fmap.c) : $(obj)/fmap_config.h

bootblock-y += bootmode.c
romstage-y += bootmode.c
ramstage-y += bootmode.c
verstage-y += bootmode.c

decompressor-y += halt.c
bootblock-y += halt.c
romstage-y += halt.c
ramstage-y += halt.c
smm-y += halt.c

decompressor-y += reset.c
bootblock-y += reset.c
verstage-y += reset.c
romstage-y += reset.c
postcar-y += reset.c
ramstage-y += reset.c
smm-y += reset.c

decompressor-y += string.c
bootblock-y += string.c
verstage-y += string.c
romstage-y += string.c
postcar-y += string.c
ramstage-y += string.c
smm-y += string.c

decompressor-y += crc_byte.c
bootblock-y += crc_byte.c
verstage-y += crc_byte.c
romstage-y += crc_byte.c
postcar-y += crc_byte.c
ramstage-y += crc_byte.c
smm-y += crc_byte.c

romstage-y += xxhash.c
ramstage-y += xxhash.c

postcar-y += bootmode.c
postcar-y += boot_device.c
postcar-y += cbfs.c
postcar-y += delay.c
postcar-y += fmap.c
postcar-y += gcc.c
postcar-y += halt.c
postcar-y += libgcc.c
postcar-$(CONFIG_COMPRESS_RAMSTAGE) += lzma.c lzmadecode.c
postcar-y += memchr.c
postcar-y += memcmp.c
postcar-y += prog_loaders.c
postcar-y += prog_ops.c
postcar-y += rmodule.c
postcar-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
postcar-$(CONFIG_GENERIC_UDELAY) += timer.c

# Use program.ld for all the platforms which use C fo the bootblock.
bootblock-y += program.ld

decompressor-y += program.ld
postcar-y += program.ld
romstage-y += program.ld
ramstage-y += program.ld
verstage-y += program.ld

ifeq ($(CONFIG_RELOCATABLE_MODULES),y)
ramstage-y += rmodule.c
romstage-y += rmodule.c

RMODULE_LDFLAGS  := -z defs -Bsymbolic

# rmodule_link_rules is a function that should be called with:
# (1) the object name to link
# (2) the dependencies
# (3) heap size of the relocatable module
# (4) arch for which the rmodules are to be linked
# It will create the necessary Make rules to create a rmodule. The resulting
# rmdoule is named $(1).rmod
define rmodule_link
$(strip $(1)): $(strip $(2)) $$(COMPILER_RT_rmodules_$(4)) $(call src-to-obj,rmodules_$(4),src/lib/rmodule.ld) | $$(RMODTOOL)
	$$(LD_rmodules_$(4)) $$(LDFLAGS_rmodules_$(4)) $(RMODULE_LDFLAGS) $($(1)-ldflags) -T $(call src-to-obj,rmodules_$(4),src/lib/rmodule.ld) --defsym=__heap_size=$(strip $(3)) -o $$@ --whole-archive --start-group $(filter-out %.ld,$(2)) --end-group
	$$(NM_rmodules_$(4)) -n $$@ > $$(basename $$@).map
endef

endif

$(objcbfs)/%.debug.rmod: $(objcbfs)/%.debug | $(RMODTOOL)
	$(RMODTOOL) -i $< -o $@

$(obj)/%.elf.rmod: $(obj)/%.elf | $(RMODTOOL)
	$(RMODTOOL) -i $< -o $@

ramstage-$(CONFIG_RAMSTAGE_ADA) += cb.ads

ifeq ($(CONFIG_RAMSTAGE_LIBHWBASE),y)

to-ada-hex = $(eval $(1) := 16\\\#$(patsubst 0x%,%,$($(1)))\\\#)

$(call to-ada-hex,CONFIG_HWBASE_DEFAULT_MMCONF)

$(call add-special-class,hw)
hw-handler = $(eval ramstage-srcs += $$(addprefix $(1),$(2)))

$(call add-special-class,hw-gen)
hw-gen-handler = \
	$(eval additional-dirs += $(dir $(2))) \
	$(eval ramstage-srcs += $(2)) \
	$(eval ramstage-ads-deps += $(2)) \
	$(eval ramstage-adb-deps += $(2)) \
	$(eval $(2): $(obj)/config.h)

subdirs-y += ../../3rdparty/libhwbase

ramstage-$(CONFIG_HAVE_MONOTONIC_TIMER) += hw-time-timer.adb

endif # CONFIG_RAMSTAGE_LIBHWBASE

romstage-y += spd_bin.c

ifeq ($(CONFIG_HAVE_SPD_IN_CBFS),y)
LIB_SPD_BIN = $(obj)/spd.bin

LIB_SPD_DEPS = $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)

# Include spd ROM data
$(LIB_SPD_BIN): $(LIB_SPD_DEPS)
	test -n "$(SPD_SOURCES)" || \
	    (echo "HAVE_SPD_IN_CBFS is set but SPD_SOURCES is empty" && exit 1)
	test -n "$(LIB_SPD_DEPS)" || \
	    (echo "SPD_SOURCES is set but no SPD file was found" && exit 1)
	if [ "$(SPD_SOURCES)" = "placeholder" ]; then \
	    printf '\0'; \
	else \
	    for f in $(LIB_SPD_DEPS); do \
	        if [ ! -f $$f ]; then \
	            echo "File not found: $$f" >&2; \
	            exit 1; \
	        fi; \
	        for c in $$(cat $$f | grep --binary-files=text -v ^#); \
	            do printf $$(printf '\\%o' 0x$$c); \
	        done; \
	    done; \
	fi > $@

cbfs-files-y += spd.bin
spd.bin-file := $(LIB_SPD_BIN)
spd.bin-type := spd
endif

ramstage-y += uuid.c

romstage-$(CONFIG_SPD_CACHE_IN_FMAP) += spd_cache.c

cbfs-files-y += cbfs_master_header
cbfs_master_header-file := cbfs_master_header.c:struct
cbfs_master_header-type := "cbfs header"
cbfs_master_header-position := 0

ifeq ($(CONFIG_ARCH_X86),y)
$(call src-to-obj,bootblock,$(dir)/master_header_pointer.c): $(obj)/fmap_config.h
bootblock-y += master_header_pointer.c
endif

NEED_CBFS_POINTER=

ifneq ($(CONFIG_ARCH_X86),y)
NEED_CBFS_POINTER=y
endif
ifneq ($(CONFIG_BOOTBLOCK_IN_CBFS),y)
NEED_CBFS_POINTER=y
endif

cbfs-files-$(NEED_CBFS_POINTER) += header_pointer
header_pointer-file := master_header_pointer.c:struct
header_pointer-position := -4
header_pointer-type := "cbfs header"
