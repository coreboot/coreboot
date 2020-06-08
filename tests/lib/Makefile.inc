# SPDX-License-Identifier: GPL-2.0-only

tests-y += string-test
tests-y += b64_decode-test

string-test-srcs += tests/lib/string-test.c
string-test-srcs += src/lib/string.c

b64_decode-test-srcs += tests/lib/b64_decode-test.c
b64_decode-test-srcs += src/lib/b64_decode.c
