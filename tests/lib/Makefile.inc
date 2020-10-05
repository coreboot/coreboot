# SPDX-License-Identifier: GPL-2.0-only

tests-y += string-test
tests-y += b64_decode-test
tests-y += hexstrtobin-test
tests-y += imd-test

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