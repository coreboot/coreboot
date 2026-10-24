# SPDX-License-Identifier: GPL-2.0-only

tests-y += acpigen-test

acpigen-test-srcs += tests/acpi/acpigen-test.c
acpigen-test-srcs += src/acpi/acpigen.c
acpigen-test-srcs += tests/stubs/console.c
acpigen-test-config += CONFIG_ACPI_CPU_STRING=\"C%03X\"

tests-y += cpu-apic-test
cpu-apic-test-srcs += tests/acpi/cpu-apic-test.c src/acpi/acpi_apic.c
cpu-apic-test-srcs += src/device/device_util.c tests/stubs/console.c
cpu-apic-test-config += CONFIG_MAX_CPUS=8
