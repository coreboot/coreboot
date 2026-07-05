## SPDX-License-Identifier: GPL-2.0-only

# Third-party attribution / notices file.
#
# 'make' decides which projects are actually part of this build using the same
# CONFIG_* variables that gate the corresponding sources; gen_attribution.sh
# then renders the summary blocks for the selected keys from attribution.list.
# Keep the keys below in sync with the 'key' column in attribution.list.

attribution-tool-dir := util/attribution
attribution-build-dir := $(obj)/attribution

# --- in-tree third-party source -------------------------------------------

attribution-keys-y += coreboot			# always
attribution-keys-y += xxhash			# commonlib/bsd/xxhash.c: always

# Compression decoders (LZMA/LZ4/zstd): instead of mirroring the Kconfig that
# selects each one, detect the decoder's source in the aggregated per-stage
# source lists. This tracks exactly what is linked into the image and is robust
# against however the compression was chosen - a Kconfig symbol or a hardcoded
# "<file>-compression := ..." in any Makefile. The lists are only complete
# after the full parse, so this is evaluated lazily (=) when the recipe runs.
# zstd keys on the library (zstd_decompress.c), not the always-present
# zstd_wrapper.c stub that --gc-sections drops when unused.
attribution-decomp-srcs = $(foreach c,decompressor bootblock verstage romstage \
	postcar ramstage smm smmstub,$($(c)-srcs))
attribution-comp-keys = \
	$(if $(filter %/lzmadecode.c,$(attribution-decomp-srcs)),lzma) \
	$(if $(filter %/lz4_wrapper.c,$(attribution-decomp-srcs)),lz4) \
	$(if $(filter %/zstd_decompress.c,$(attribution-decomp-srcs)),zstd)

attribution-keys-$(CONFIG_UBSAN)			+= ubsan
attribution-keys-$(CONFIG_BOOTSPLASH)			+= wuffs
attribution-keys-$(CONFIG_PCI_OPTION_ROM_RUN_YABEL)	+= x86emu
attribution-keys-$(CONFIG_FONT_GOOGLE_SANS_FLEX_MEDIUM_24X32) += fonts

# Code derived from other (GPL) projects, compiled unconditionally from the
# coreboot tree: Linux (console printk/vtxprintf, src/console/Makefile.mk),
# U-Boot (src/lib/rtc.c) and depthcharge (src/commonlib/list.c, device_tree.c).
attribution-keys-y += linux
attribution-keys-y += uboot
attribution-keys-y += depthcharge

attribution-keys-$(CONFIG_CONSOLE_NE2K)		+= etherboot
attribution-keys-$(CONFIG_EC_GOOGLE_CHROMEEC)	+= chromeec

# vboot (verstage/libvboot) is compiled into the image from the 3rdparty/vboot
# submodule, so it belongs in the in-tree section (its manifest row has no kind).
attribution-keys-$(CONFIG_VBOOT)		+= vboot

# Ada graphics init: the GNAT runtime plus the libhwbase and libgfxinit
# libraries (3rdparty submodules) compiled into the image.
ifneq ($(CONFIG_RAMSTAGE_LIBHWBASE)$(CONFIG_ROMSTAGE_LIBHWBASE),)
attribution-keys-y += gnat
attribution-keys-y += libhwbase
endif
attribution-keys-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += libgfxinit

# --- standalone binaries the build adds to the image (kind = binary) -------
# These are stitched into the image but not compiled from coreboot source:
# vendor firmware blobs, separately-built programs and the payload.

attribution-keys-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE)	+= arm_tf
attribution-keys-$(CONFIG_RISCV_OPENSBI)			+= opensbi
attribution-keys-$(CONFIG_USE_CPU_MICROCODE_CBFS_BINS)		+= intel_microcode
attribution-keys-$(CONFIG_HAVE_ME_BIN)				+= intel_me
attribution-keys-$(CONFIG_HAVE_GBE_BIN)				+= intel_gbe
attribution-keys-$(CONFIG_INTEL_TXT)				+= intel_acm
attribution-keys-$(CONFIG_VGA_BIOS)				+= vga_bios

# AMD firmware blobs (PSP bootloader/firmware via amdfwtool, microcode).
# USE_AMD_BLOBS only depends on USE_BLOBS, so it can be set on a non-AMD build;
# credit AMD microcode only when an AMD SoC that actually bundles it is built
# (SOC_AMD_COMMON_BLOCK_UCODE, the symbol that pulls in the microcode).
attribution-keys-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP)	+= amd_psp
ifeq ($(CONFIG_USE_AMD_BLOBS)$(CONFIG_SOC_AMD_COMMON_BLOCK_UCODE),yy)
attribution-keys-y += amd_microcode
endif

# FSP: both Intel and AMD SoCs use PLATFORM_USES_FSP2_0, but the binary and its
# license differ by vendor. SOC_AMD_COMMON is set only for AMD SoCs. Credit an
# FSP blob only when one is actually stitched into the image, so a board built
# without the binaries is not credited an FSP that isn't there: FSP 2.0 adds its
# binaries under ADD_FSP_BINARIES, FSP 1.1 (Intel only) under HAVE_FSP_BIN.
ifeq ($(CONFIG_PLATFORM_USES_FSP2_0)$(CONFIG_ADD_FSP_BINARIES),yy)
attribution-fsp := y
endif
ifeq ($(CONFIG_HAVE_FSP_BIN),y)		# FSP 1.1
attribution-fsp := y
endif
ifeq ($(attribution-fsp),y)
ifeq ($(CONFIG_SOC_AMD_COMMON),y)
attribution-keys-y += amd_fsp
else
attribution-keys-y += intel_fsp
endif
endif

# Payloads - the external program the build adds to CBFS as fallback/payload.
attribution-keys-$(CONFIG_PAYLOAD_EDK2)		+= payload_edk2
# Credit the actual edk2 source in addition to the general edk2 codebase.
# EDK2_REPO_OFFICIAL uses TianoCore upstream, already covered by payload_edk2.
attribution-keys-$(CONFIG_EDK2_REPO_MRCHROMEBOX) += payload_edk2_mrchromebox
ifeq ($(CONFIG_EDK2_REPO_CUSTOM),y)
ifeq ($(CONFIG_EDK2_UNIVERSAL_PAYLOAD),y)
# Universal Payload defaults its (custom) repo to Star Labs' edk2 fork.
attribution-keys-y += payload_edk2_starlabs
else
# A truly custom repo cannot be named statically; emit the actual configured
# URL/revision at build time via gen_attribution.sh's --extra mechanism.
attribution-edk2-custom-url := $(or $(call strip_quotes,$(CONFIG_EDK2_REPOSITORY)),(see CONFIG_EDK2_REPOSITORY))
attribution-edk2-custom-rev := $(or $(call strip_quotes,$(CONFIG_EDK2_TAG_OR_REV)),unspecified revision)
endif
endif
attribution-keys-$(CONFIG_PAYLOAD_DEPTHCHARGE)	+= payload_depthcharge
attribution-keys-$(CONFIG_PAYLOAD_FILO)		+= payload_filo
attribution-keys-$(CONFIG_PAYLOAD_LINUXBOOT)	+= payload_linuxboot
attribution-keys-$(CONFIG_PAYLOAD_SKIBOOT)	+= payload_skiboot
attribution-keys-$(CONFIG_PAYLOAD_UBOOT)	+= payload_uboot
attribution-keys-$(CONFIG_PAYLOAD_BOOTBOOT)	+= payload_bootboot
attribution-keys-$(CONFIG_PAYLOAD_LINUX)	+= payload_linux

attribution-keys-$(CONFIG_PAYLOAD_LEANEFI)	+= payload_leanefi

# SeaBIOS and GRUB2 may be the main payload, a secondary payload, or part of
# SeaGRUB. PAYLOAD_BUILD_* is set whenever each is actually built, so it is the
# canonical gate (a plain PAYLOAD_SEABIOS check would miss the SeaGRUB case).
attribution-keys-$(CONFIG_PAYLOAD_BUILD_SEABIOS)	+= payload_seabios
attribution-keys-$(CONFIG_PAYLOAD_BUILD_GRUB2)		+= payload_grub2

# Secondary payloads and the iPXE PXE option ROM (built and stitched in).
attribution-keys-$(CONFIG_COREINFO_SECONDARY_PAYLOAD)	+= payload_coreinfo
attribution-keys-$(CONFIG_NVRAMCUI_SECONDARY_PAYLOAD)	+= payload_nvramcui
attribution-keys-$(CONFIG_TINT_SECONDARY_PAYLOAD)	+= payload_tint
attribution-keys-$(CONFIG_MEMTEST_SECONDARY_PAYLOAD)	+= payload_memtest
attribution-keys-$(CONFIG_COREDOOM_SECONDARY_PAYLOAD)	+= payload_coredoom
attribution-keys-$(CONFIG_BUILD_IPXE)			+= payload_ipxe

# --- src/vendorcode third-party codebases ---------------------------------

# EDK II headers/code are gated by the UEFI/UDK "binding" version selected
# (src/vendorcode/intel/Makefile.mk).
ifneq ($(CONFIG_UEFI_2_4_BINDING)$(CONFIG_UDK_2017_BINDING)$(CONFIG_UDK_202005_BINDING)$(CONFIG_UDK_202111_BINDING)$(CONFIG_UDK_202302_BINDING)$(CONFIG_UDK_202305_BINDING),)
attribution-keys-y += edk2
endif

attribution-keys-$(CONFIG_TCG_OPAL_S3_UNLOCK)	+= tcg_storage
attribution-keys-$(CONFIG_SOC_AMD_OPENSIL)	+= opensil

# AMD AGESA (src/vendorcode/amd/pi) - only the older PI-based AMD parts.
ifneq ($(CONFIG_SOC_AMD_STONEYRIDGE)$(CONFIG_CPU_AMD_PI_00730F01),)
attribution-keys-y += agesa
endif

attribution-keys-$(CONFIG_SOC_CAVIUM_CN81XX)	+= cavium_bdk

ifneq ($(CONFIG_SOC_MEDIATEK_MT8192)$(CONFIG_SOC_MEDIATEK_MT8195),)
attribution-keys-y += mediatek_dram
endif

# --- generation + CBFS registration ---------------------------------------

# Depend on $(DOTCONFIG) so the file is regenerated whenever the configuration
# changes: the selected keys come from CONFIG_* options, not from any of the
# other prerequisites, so without this the file would go stale on a reconfigure.
$(attribution-build-dir)/attribution.txt: $(attribution-tool-dir)/attribution.list \
					  $(attribution-tool-dir)/gen_attribution.sh \
					  $(DOTCONFIG)
	@printf "    GEN        attribution\n"
	mkdir -p $(dir $@)
	: > $@.extra
ifneq ($(attribution-edk2-custom-url),)
	printf '%s\n' '+|edk2 UEFI payload - custom repository|BSD-2-Clause-Patent|$(attribution-edk2-custom-url)|Built from $(attribution-edk2-custom-rev); based on TianoCore EDK II|binary' >> $@.extra
endif
	$(attribution-tool-dir)/gen_attribution.sh \
		--extra $@.extra \
		$(attribution-tool-dir)/attribution.list \
		$(sort $(attribution-keys-y) $(attribution-comp-keys)) > $@.tmp
	mv $@.tmp $@
	rm -f $@.extra

# This Makefile is only parsed when CONFIG_INCLUDE_ATTRIBUTION_FILE is set
# (subdirs-$(CONFIG_INCLUDE_ATTRIBUTION_FILE)), so a plain -y gate suffices.
cbfs-files-y += attribution
attribution-file := $(attribution-build-dir)/attribution.txt
attribution-type := raw
# Reuse the build's own CBFS compression (none/LZMA/LZ4/ZSTD) rather than
# forcing a specific one. The file is only read offline via cbfstool, so this
# is purely a host-side size choice, but it keeps us consistent with the image.
attribution-compression := $(CBFS_COMPRESS_FLAG)
