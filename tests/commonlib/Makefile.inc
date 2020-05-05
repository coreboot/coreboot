# SPDX-License-Identifier: GPL-2.0-only
# This file is part of the coreboot project.

tests-y += region-test

region-test-srcs += tests/commonlib/region-test.c
region-test-srcs += src/commonlib/region.c
