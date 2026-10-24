# SPDX-License-Identifier: GPL-2.0-only

.PHONY: run-opensil-configure-test
run-unit-tests: run-opensil-configure-test
run-opensil-configure-test:
	@if command -v meson >/dev/null && command -v ninja >/dev/null; then \
		python3 $(top)/tests/vendorcode/amd/opensil/configure-test.py \
			--output $(testobj)/vendorcode/opensil-configure; \
	else \
		printf '%s\n' 'SKIP openSIL configure test: Meson and Ninja are required'; \
	fi

turin_test_opensil ?= src/vendorcode/amd/opensil/turin_poc/opensil

ifneq ($(wildcard $(turin_test_opensil)/Include/xPrfFabric.h),)
tests-y += turin-resources-1p-test turin-resources-2p-test
turin-resources-1p-test-srcs += tests/vendorcode/amd/opensil/turin-resources-test.c
turin-resources-1p-test-srcs += src/vendorcode/amd/opensil/turin_poc/resources.c
turin-resources-1p-test-srcs += tests/stubs/console.c
turin-resources-1p-test-cflags += -Itests/vendorcode/amd/opensil/include
turin-resources-1p-test-cflags += -I$(turin_test_opensil)/Include
turin-resources-1p-test-cflags += -I$(turin_test_opensil)/xUSL
turin-resources-1p-test-cflags += -I$(turin_test_opensil)/xUSL/Include
turin-resources-1p-test-cflags += -I$(turin_test_opensil)/xSIM
turin-resources-1p-test-cflags += -D_MSC_EXTENSIONS=0 -Wno-unknown-pragmas
turin-resources-1p-test-cflags += -DCONFIG_SOC_F1AM00=1
turin-resources-1p-test-cflags += -DCONFIG_PLAT_NUMBER_SOCKETS=CONFIG_MAX_SOCKET
turin-resources-1p-test-config += CONFIG_MAX_SOCKET=1 CONFIG_ECAM_MMCONF_BASE_ADDRESS=0xb0000000
$(call copy-test,turin-resources-1p-test,turin-resources-2p-test)
turin-resources-2p-test-config := CONFIG_MAX_SOCKET=2 CONFIG_ECAM_MMCONF_BASE_ADDRESS=0xb0000000
endif

# Tests that use openSIL are optional when the submodule is absent.
ifneq ($(wildcard $(turin_test_opensil)/xUSL/Mpio/Common/MpioSupportFunctions.c),)
tests-y += turin-mpio-1p-test
tests-y += turin-mpio-2p-test

turin-mpio-1p-test-srcs += tests/vendorcode/amd/opensil/turin-mpio-test.c
turin-mpio-1p-test-srcs += src/vendorcode/amd/opensil/turin_poc/mpio/chip.c
turin-mpio-1p-test-srcs += src/drivers/amd/opensil/mpio/chip.c
turin-mpio-1p-test-srcs += src/device/device_util.c
turin-mpio-1p-test-srcs += src/device/device_const.c
turin-mpio-1p-test-srcs += $(turin_test_opensil)/xUSL/Mpio/Common/MpioSupportFunctions.c
turin-mpio-1p-test-srcs += tests/stubs/die.c

turin-mpio-1p-test-cflags += -Itests/vendorcode/amd/opensil/include
turin-mpio-1p-test-cflags += -I$(turin_test_opensil)/Include
turin-mpio-1p-test-cflags += -I$(turin_test_opensil)/xUSL
turin-mpio-1p-test-cflags += -I$(turin_test_opensil)/xUSL/Include
turin-mpio-1p-test-cflags += -I$(turin_test_opensil)/xSIM
turin-mpio-1p-test-cflags += -include tests/vendorcode/amd/opensil/include/opensil_config.h
turin-mpio-1p-test-cflags += -D_MSC_EXTENSIONS=0 -DHAS_STRING_H=1 -Wno-unknown-pragmas
turin-mpio-1p-test-cflags += -DCONFIG_SOC_F1AM00=1 -DCONFIG_PLAT_NUMBER_SOCKETS=CONFIG_MAX_SOCKET
turin-mpio-1p-test-config += CONFIG_MAX_SOCKET=1
turin-mpio-1p-test-config += CONFIG_OPENSIL_MPIO_EARLY_BMC_LINK_LANE=135
turin-mpio-1p-test-config += \
	CONFIG_AMD_OPENSIL_MPIO_CHIP_H_FILE=\"vendorcode/amd/opensil/turin_poc/mpio/chip.h\"

$(call copy-test,turin-mpio-1p-test,turin-mpio-2p-test)
turin-mpio-2p-test-config := $(filter-out CONFIG_MAX_SOCKET=1,$(turin-mpio-2p-test-config))
turin-mpio-2p-test-config += CONFIG_MAX_SOCKET=2
endif
