# SPDX-License-Identifier: GPL-2.0-only

tests-y += i2c-test
tests-y += ddr4-test

i2c-test-srcs += tests/device/i2c-test.c
i2c-test-srcs += src/device/i2c.c
i2c-test-mocks += platform_i2c_transfer

ddr4-test-srcs += tests/device/ddr4-test.c
ddr4-test-srcs += tests/stubs/console.c
ddr4-test-srcs += src/device/dram/ddr4.c