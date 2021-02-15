# SPDX-License-Identifier: GPL-2.0-only

tests-y += string-test
tests-y += b64_decode-test
tests-y += hexstrtobin-test
tests-y += imd-test
tests-y += timestamp-test
tests-y += edid-test
tests-y += cbmem_console-romstage-test
tests-y += cbmem_console-ramstage-test
tests-y += list-test
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

list-test-srcs += tests/lib/list-test.c
list-test-srcs += src/lib/list.c

fmap-test-srcs += tests/lib/fmap-test.c
fmap-test-srcs += src/lib/fmap.c
fmap-test-srcs += tests/stubs/console.c
fmap-test-srcs += src/lib/boot_device.c
fmap-test-srcs += src/commonlib/region.c
fmap-test-cflags += -I tests/include/tests/lib/fmap
fmap-test-cflags += -I 3rdparty/vboot/firmware/include

imd_cbmem-ramstage-test-stage := ramstage
imd_cbmem-ramstage-test-srcs += tests/lib/imd_cbmem-test.c
imd_cbmem-ramstage-test-srcs += tests/stubs/console.c
imd_cbmem-ramstage-test-srcs += src/lib/imd.c
imd_cbmem-ramstage-test-mocks += cbmem_top_chipset

imd_cbmem-romstage-test-stage := romstage
imd_cbmem-romstage-test-srcs += tests/lib/imd_cbmem-test.c
imd_cbmem-romstage-test-srcs += tests/stubs/console.c
imd_cbmem-romstage-test-srcs += src/lib/imd.c
imd_cbmem-romstage-test-mocks += cbmem_top_chipset

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

