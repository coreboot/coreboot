/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef TEST_ACPI_MOCK_MSR_H
#define TEST_ACPI_MOCK_MSR_H

#include <cpu/x86/msr_access.h>

/* Keep the real MSR types and register definitions; replace only the read. */
msr_t cpu_power_test_rdmsr(unsigned int index);
#define rdmsr cpu_power_test_rdmsr

#endif
