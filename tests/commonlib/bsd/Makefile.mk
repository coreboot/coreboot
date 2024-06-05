# SPDX-License-Identifier: GPL-2.0-only

tests-y += helpers-test
tests-y += gcd-test
tests-y += ipchksum-test
tests-y += string-test

helpers-test-srcs += tests/commonlib/bsd/helpers-test.c

gcd-test-srcs += tests/commonlib/bsd/gcd-test.c
gcd-test-srcs += src/commonlib/bsd/gcd.c

ipchksum-test-srcs += tests/commonlib/bsd/ipchksum-test.c
ipchksum-test-srcs += src/commonlib/bsd/ipchksum.c

string-test-srcs += tests/commonlib/bsd/string-test.c
string-test-srcs += src/commonlib/bsd/string.c
