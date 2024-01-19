# SPDX-License-Identifier: GPL-2.0-only

tests-y += helpers-test
tests-y += gcd-test

helpers-test-srcs += tests/commonlib/bsd/helpers-test.c

gcd-test-srcs += tests/commonlib/bsd/gcd-test.c
gcd-test-srcs += src/commonlib/bsd/gcd.c
