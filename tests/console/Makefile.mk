# SPDX-License-Identifier: GPL-2.0-only

tests-y += routing-with-cbmemcons-test
tests-y += routing-without-cbmemcons-test

routing-with-cbmemcons-test-srcs += tests/console/routing-test.c
routing-with-cbmemcons-test-config += CONFIG_CONSOLE_CBMEM=1

routing-without-cbmemcons-test-srcs += tests/console/routing-test.c
routing-without-cbmemcons-test-config += CONFIG_CONSOLE_CBMEM=0
