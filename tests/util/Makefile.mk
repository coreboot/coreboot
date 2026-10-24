# SPDX-License-Identifier: GPL-2.0-only

tests-y += amdfwtool-key-test
amdfwtool-key-test-syssrcs += tests/util/amdfwtool-key-test.c
amdfwtool-key-test-syssrcs += util/amdfwtool/data_parse.c util/amdfwtool/soc.c
amdfwtool-key-test-cflags += -Isrc -Isrc/commonlib/bsd/include
amdfwtool-key-test-no_test_framework := y

tests-y += amdfwtool-subprogram-test
amdfwtool-subprogram-test-syssrcs += tests/util/amdfwtool-subprogram-test.c
amdfwtool-subprogram-test-syssrcs += util/amdfwtool/data_parse.c
amdfwtool-subprogram-test-syssrcs += util/amdfwtool/soc.c
amdfwtool-subprogram-test-cflags += -Isrc -Isrc/commonlib/bsd/include
amdfwtool-subprogram-test-no_test_framework := y
