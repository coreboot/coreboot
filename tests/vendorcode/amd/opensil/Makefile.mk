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

