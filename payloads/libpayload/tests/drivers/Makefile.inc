# SPDX-License-Identifier: GPL-2.0-only

tests-y += speaker-test

speaker-test-srcs += tests/drivers/speaker-test.c
speaker-test-mocks += inb
speaker-test-mocks += outb
speaker-test-mocks += arch_ndelay
speaker-test-cflags += -include $(testsrc)/include/mocks/x86_io.h
