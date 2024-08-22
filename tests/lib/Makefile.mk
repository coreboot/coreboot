# SPDX-License-Identifier: GPL-2.0-only

tests-y += lib-test
tests-y += string-test
tests-y += b64_decode-test
tests-y += hexstrtobin-test
tests-y += imd-test
tests-y += timestamp-test
tests-y += edid-test
tests-y += cbmem_console-romstage-test
tests-y += cbmem_console-ramstage-test
tests-y += fmap-test
tests-y += imd_cbmem-romstage-test
tests-y += imd_cbmem-ramstage-test
tests-y += region_file-test
tests-y += stack-test
tests-y += memset-test
tests-y += memcmp-test
tests-y += memchr-test
tests-y += memcpy-test
tests-y += malloc-test
tests-y += memmove-test
tests-y += crc_byte-test
tests-y += memrange-test
tests-y += uuid-test
tests-y += bootmem-test
tests-y += dimm_info_util-test
tests-y += coreboot_table-test
tests-y += rtc-test
tests-y += spd_cache-ddr3-test
tests-y += spd_cache-ddr4-test
tests-y += cbmem_stage_cache-test
tests-y += libgcc-test
tests-y += cbfs-verification-no-sha512-test
tests-y += cbfs-verification-has-sha512-test
tests-y += cbfs-no-verification-no-sha512-test
tests-y += cbfs-no-verification-has-sha512-test
tests-y += cbfs-lookup-no-mcache-test
tests-y += cbfs-lookup-has-mcache-test
tests-y += lzma-test
tests-y += ux_locales-test

lib-test-srcs += tests/lib/lib-test.c

string-test-srcs += tests/lib/string-test.c
string-test-srcs += src/lib/string.c

b64_decode-test-srcs += tests/lib/b64_decode-test.c
b64_decode-test-srcs += tests/stubs/console.c
b64_decode-test-srcs += src/lib/b64_decode.c

hexstrtobin-test-srcs += tests/lib/hexstrtobin-test.c
hexstrtobin-test-srcs += src/lib/hexstrtobin.c

imd-test-srcs += tests/lib/imd-test.c
imd-test-srcs += tests/stubs/console.c
imd-test-srcs += src/lib/imd.c
imd-test-srcs += tests/stubs/die.c

timestamp-test-srcs += tests/lib/timestamp-test.c
timestamp-test-srcs += tests/stubs/timestamp.c
timestamp-test-srcs += tests/stubs/console.c
timestamp-test-stage := romstage

edid-test-srcs += tests/lib/edid-test.c
edid-test-srcs += src/lib/edid.c
edid-test-srcs += tests/stubs/console.c

cbmem_console-romstage-test-stage := romstage
cbmem_console-romstage-test-srcs += tests/lib/cbmem_console-test.c
cbmem_console-romstage-test-srcs += tests/stubs/console.c

cbmem_console-ramstage-test-stage := ramstage
cbmem_console-ramstage-test-srcs += tests/lib/cbmem_console-test.c
cbmem_console-ramstage-test-srcs += tests/stubs/console.c

fmap-test-srcs += tests/lib/fmap-test.c
fmap-test-srcs += src/lib/fmap.c
fmap-test-srcs += tests/stubs/console.c
fmap-test-srcs += src/lib/boot_device.c
fmap-test-srcs += src/commonlib/region.c
fmap-test-cflags += -I tests/include/tests/lib/fmap

imd_cbmem-ramstage-test-stage := ramstage
imd_cbmem-ramstage-test-srcs += tests/lib/imd_cbmem-test.c
imd_cbmem-ramstage-test-srcs += tests/stubs/console.c
imd_cbmem-ramstage-test-srcs += src/lib/imd.c
imd_cbmem-ramstage-test-mocks += cbmem_top_chipset
imd_cbmem-ramstage-test-srcs += tests/stubs/die.c

imd_cbmem-romstage-test-stage := romstage
imd_cbmem-romstage-test-srcs += tests/lib/imd_cbmem-test.c
imd_cbmem-romstage-test-srcs += tests/stubs/console.c
imd_cbmem-romstage-test-srcs += src/lib/imd.c
imd_cbmem-romstage-test-mocks += cbmem_top_chipset
imd_cbmem-romstage-test-srcs += tests/stubs/die.c

region_file-test-srcs += tests/lib/region_file-test.c
region_file-test-srcs += src/commonlib/region.c
region_file-test-srcs += tests/stubs/console.c

stack-test-srcs += tests/lib/stack-test.c
stack-test-srcs += src/lib/stack.c
stack-test-srcs += tests/stubs/console.c

memset-test-srcs += tests/lib/memset-test.c
memset-test-srcs += src/lib/memset.c

memcmp-test-srcs += tests/lib/memcmp-test.c

memchr-test-srcs += tests/lib/memchr-test.c
memchr-test-srcs += src/lib/memchr.c

memcpy-test-srcs += tests/lib/memcpy-test.c

malloc-test-srcs += tests/lib/malloc-test.c
malloc-test-srcs += tests/stubs/console.c

memmove-test-srcs += tests/lib/memmove-test.c

crc_byte-test-srcs += tests/lib/crc_byte-test.c
crc_byte-test-srcs += src/lib/crc_byte.c

memrange-test-srcs += tests/lib/memrange-test.c
memrange-test-srcs += src/lib/memrange.c
memrange-test-srcs += tests/stubs/console.c
memrange-test-srcs += src/device/device_util.c

uuid-test-srcs += tests/lib/uuid-test.c
uuid-test-srcs += src/lib/hexstrtobin.c
uuid-test-srcs += src/lib/uuid.c

bootmem-test-srcs += tests/lib/bootmem-test.c
bootmem-test-srcs += tests/stubs/console.c
bootmem-test-srcs += src/device/device_util.c
bootmem-test-srcs += src/lib/bootmem.c
bootmem-test-srcs += src/lib/memrange.c

dimm_info_util-test-srcs += tests/lib/dimm_info_util-test.c
dimm_info_util-test-srcs += src/device/dram/spd.c
dimm_info_util-test-srcs += src/lib/dimm_info_util.c
dimm_info_util-test-srcs += tests/stubs/console.c

coreboot_table-test-srcs += tests/lib/coreboot_table-test.c
coreboot_table-test-srcs += tests/stubs/console.c
coreboot_table-test-srcs += src/commonlib/bsd/ipchksum.c
coreboot_table-test-srcs += src/lib/coreboot_table.c
coreboot_table-test-srcs += src/lib/imd_cbmem.c
coreboot_table-test-srcs += src/lib/imd.c
coreboot_table-test-cflags += -I tests/include/tests/lib/fmap
coreboot_table-test-mocks += cbmem_top_chipset

rtc-test-srcs += tests/lib/rtc-test.c
rtc-test-srcs += src/lib/rtc.c

spd_cache-ddr3-test-srcs += tests/lib/spd_cache-test.c
spd_cache-ddr3-test-srcs += tests/stubs/console.c
spd_cache-ddr3-test-srcs += src/lib/spd_cache.c
spd_cache-ddr3-test-srcs += src/lib/crc_byte.c
spd_cache-ddr3-test-srcs += src/commonlib/region.c
spd_cache-ddr3-test-mocks += fmap_locate_area_as_rdev
spd_cache-ddr3-test-config += CONFIG_SPD_CACHE_FMAP_NAME=\"RW_SPD_CACHE\" \
				CONFIG_DIMM_MAX=4 CONFIG_DIMM_SPD_SIZE=256 \
				CONFIG_BOOT_DEVICE_MEMORY_MAPPED=1
spd_cache-ddr3-test-cflags += -D__TEST_SPD_CACHE_DDR=3

spd_cache-ddr4-test-srcs += tests/lib/spd_cache-test.c
spd_cache-ddr4-test-srcs += tests/stubs/console.c
spd_cache-ddr4-test-srcs += src/lib/spd_cache.c
spd_cache-ddr4-test-srcs += src/lib/crc_byte.c
spd_cache-ddr4-test-srcs += src/commonlib/region.c
spd_cache-ddr4-test-mocks += fmap_locate_area_as_rdev
spd_cache-ddr4-test-config += CONFIG_SPD_CACHE_FMAP_NAME=\"RW_SPD_CACHE\" \
				CONFIG_DIMM_MAX=4 CONFIG_DIMM_SPD_SIZE=512 \
				CONFIG_BOOT_DEVICE_MEMORY_MAPPED=1
spd_cache-ddr4-test-cflags += -D__TEST_SPD_CACHE_DDR=4

cbmem_stage_cache-test-srcs += tests/lib/cbmem_stage_cache-test.c
cbmem_stage_cache-test-srcs += tests/stubs/console.c
cbmem_stage_cache-test-srcs += src/lib/cbmem_stage_cache.c
cbmem_stage_cache-test-srcs += src/lib/imd_cbmem.c
cbmem_stage_cache-test-srcs += src/lib/imd.c
cbmem_stage_cache-test-config += CONFIG_CBMEM_STAGE_CACHE=1

libgcc-test-srcs += tests/lib/libgcc-test.c

# CBFS varification tests are compiled with CONFIG_CBFS_VERIFICATION
# and VB2_SUPPORT_SHA512 set and unset. Code should work with and without
# verification and with hash structure of different sizes.
cbfs-verification-no-sha512-test-stage := bootblock
cbfs-verification-no-sha512-test-srcs := tests/lib/cbfs-verification-test.c \
					tests/stubs/console.c \
					tests/stubs/die.c \
					tests/mock/cbfs_file_mock.c \
					src/lib/cbfs.c \
					src/commonlib/bsd/cbfs_private.c \
					src/commonlib/mem_pool.c \
					src/commonlib/region.c
cbfs-verification-no-sha512-test-mocks += cbfs_get_boot_device cbfs_lookup
cbfs-verification-no-sha512-test-config += CONFIG_COLLECT_TIMESTAMPS=0 \
					CONFIG_CBFS_VERIFICATION=1 \
					CONFIG_NO_CBFS_MCACHE=1 \
					VB2_SUPPORT_SHA512=0

$(call copy-test,cbfs-verification-no-sha512-test,cbfs-verification-has-sha512-test)
cbfs-verification-has-sha512-test-config += VB2_SUPPORT_SHA512=1

$(call copy-test,cbfs-verification-no-sha512-test,cbfs-no-verification-no-sha512-test)
cbfs-no-verification-no-sha512-test-config += CONFIG_CBFS_VERIFICATION=0

$(call copy-test,cbfs-verification-no-sha512-test,cbfs-no-verification-has-sha512-test)
cbfs-no-verification-has-sha512-test-config += CONFIG_CBFS_VERIFICATION=0 \
						VB2_SUPPORT_SHA512=1

cbfs-lookup-no-mcache-test-srcs = tests/lib/cbfs-lookup-test.c \
				tests/stubs/console.c \
				tests/stubs/die.c \
				tests/mock/cbfs_file_mock.c \
				src/lib/cbfs.c \
				src/commonlib/bsd/cbfs_private.c \
				src/commonlib/bsd/cbfs_mcache.c \
				src/commonlib/mem_pool.c \
				src/commonlib/region.c
cbfs-lookup-no-mcache-test-mocks += cbfs_get_boot_device \
					cbfs_lookup \
					cbfs_mcache_lookup \
					mem_pool_alloc \
					mem_pool_free
cbfs-lookup-no-mcache-test-config += CONFIG_ARCH_X86=0 \
				CONFIG_COLLECT_TIMESTAMPS=0 \
				CONFIG_NO_CBFS_MCACHE=1

$(call copy-test,cbfs-lookup-no-mcache-test,cbfs-lookup-has-mcache-test)
cbfs-lookup-has-mcache-test-config += CONFIG_NO_CBFS_MCACHE=0

lzma-test-srcs += tests/lib/lzma-test.c
lzma-test-srcs += tests/stubs/console.c
lzma-test-srcs += src/lib/lzma.c
lzma-test-srcs += src/lib/lzmadecode.c
lzma-test-syssrcs += tests/helpers/file.c

ux_locales-test-srcs += tests/lib/ux_locales-test.c
ux_locales-test-srcs += tests/stubs/console.c
ux_locales-test-srcs += src/lib/ux_locales.c
ux_locales-test-mocks += _cbfs_alloc \
			cbfs_unmap \
			vb2api_get_locale_id \
			vboot_get_context
ux_locales-test-config += CONFIG_VBOOT=1
