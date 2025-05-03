# SPDX-License-Identifier: GPL-2.0-only

tests-y += graphics-test speaker-test

graphics-test-srcs += tests/drivers/graphics-test.c
graphics-test-srcs += libc/fpmath.c

speaker-test-srcs += tests/drivers/speaker-test.c
speaker-test-mocks += inb
speaker-test-mocks += outb
speaker-test-mocks += arch_ndelay
