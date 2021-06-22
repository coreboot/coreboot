# SPDX-License-Identifier: GPL-2.0-only

subdirs-y += bsd

tests-y += region-test

region-test-srcs += tests/commonlib/region-test.c
region-test-srcs += src/commonlib/region.c
