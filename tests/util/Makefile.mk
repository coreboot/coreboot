# SPDX-License-Identifier: GPL-2.0-only

tests-y += amdfwtool-key-test
amdfwtool-key-test-syssrcs += tests/util/amdfwtool-key-test.c
amdfwtool-key-test-syssrcs += util/amdfwtool/data_parse.c util/amdfwtool/soc.c
amdfwtool-key-test-cflags += -Isrc -Isrc/commonlib/bsd/include
amdfwtool-key-test-no_test_framework := y
