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
