# SPDX-License-Identifier: GPL-2.0-only


tests-y += amd-cpu-power-shared-test
amd-cpu-power-shared-test-srcs = $(amd-cpu-power-test-srcs)
amd-cpu-power-shared-test-cflags = $(amd-cpu-power-test-cflags)
amd-cpu-power-shared-test-config = $(filter-out CONFIG_ACPI_SSDT_PSD_INDEPENDENT=1, \
	$(amd-cpu-power-test-config))

tests-y += amd-cpu-power-test
amd-cpu-power-test-srcs += tests/soc/amd/common/amd-cpu-power-test.c
amd-cpu-power-test-srcs += src/soc/amd/common/block/acpi/cpu_power_state.c
amd-cpu-power-test-srcs += src/soc/amd/common/block/acpi/cppc.c src/acpi/acpigen.c
amd-cpu-power-test-srcs += tests/stubs/console.c src/acpi/acpi_apic.c
amd-cpu-power-test-srcs += src/device/device_util.c
amd-cpu-power-test-cflags += -Isrc/soc/amd/cezanne/include -Isrc/soc/amd/common/block/include
amd-cpu-power-test-cflags += -include tests/soc/amd/common/include/mock-msr.h
amd-cpu-power-test-config += CONFIG_MAX_CPUS=768 CONFIG_ACPI_CPU_STRING=\"C%03X\"
amd-cpu-power-test-config += CONFIG_ACPI_SSDT_PSD_INDEPENDENT=1
amd-cpu-power-test-config += CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_CPPC=1
