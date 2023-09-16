# SPDX-License-Identifier: GPL-2.0-only

subdirs-y += bsd

tests-y += list-test
tests-y += rational-test
tests-y += region-test
tests-y += device_tree-test

device_tree-test-srcs += tests/commonlib/device_tree-test.c
device_tree-test-srcs += tests/stubs/console.c
device_tree-test-srcs += src/commonlib/device_tree.c
device_tree-test-syssrcs += tests/helpers/file.c

list-test-srcs += tests/commonlib/list-test.c
list-test-srcs += src/commonlib/list.c

rational-test-srcs += tests/commonlib/rational-test.c
rational-test-srcs += src/commonlib/rational.c

region-test-srcs += tests/commonlib/region-test.c
region-test-srcs += src/commonlib/region.c
