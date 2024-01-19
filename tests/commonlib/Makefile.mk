# SPDX-License-Identifier: GPL-2.0-only

subdirs-y += bsd

tests-y += rational-test
tests-y += region-test

rational-test-srcs += tests/commonlib/rational-test.c
rational-test-srcs += src/commonlib/rational.c

region-test-srcs += tests/commonlib/region-test.c
region-test-srcs += src/commonlib/region.c
