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

