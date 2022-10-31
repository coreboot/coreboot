/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <intelblocks/acpi.h>
#include <intelblocks/sgx.h>

#define SGX_RESOURCE_ENUM_CPUID_LEAF	0x12
#define SGX_RESOURCE_ENUM_CPUID_SUBLEAF	0x2
#define SGX_RESOURCE_ENUM_BIT		0x1
#define SGX_RESOURCE_MASK_LO		0xfffff000UL
#define SGX_RESOURCE_MASK_HI		0xfffffUL

static inline uint64_t sgx_resource(uint32_t low, uint32_t high)
{
	uint64_t val;
	val = (uint64_t)(high & SGX_RESOURCE_MASK_HI) << 32;
	val |= low & SGX_RESOURCE_MASK_LO;
	return val;
}

void sgx_fill_ssdt(void)
{
	bool epcs = false;
	struct cpuid_result cpuid_regs;
	uint64_t emna = 0, elng = 0;

	if (is_sgx_supported()) {
		/*
		 * Get EPC base and size.
		 * Intel SDM: Table 36-6. CPUID Leaf 12H, Sub-Leaf Index 2 or
		 * higher for enumeration of SGX resources
		 */
		cpuid_regs = cpuid_ext(SGX_RESOURCE_ENUM_CPUID_LEAF,
					SGX_RESOURCE_ENUM_CPUID_SUBLEAF);

		if (cpuid_regs.eax & SGX_RESOURCE_ENUM_BIT) {
			/* EPC section enumerated */
			epcs = true;
			emna = sgx_resource(cpuid_regs.eax, cpuid_regs.ebx);
			elng = sgx_resource(cpuid_regs.ecx, cpuid_regs.edx);
		}

		printk(BIOS_DEBUG, "SGX: EPC status = %d base = 0x%llx len = 0x%llx\n",
				   epcs, emna, elng);
	} else {
		printk(BIOS_DEBUG, "SGX: not supported.\n");
	}

	acpigen_write_scope("\\_SB.EPC");
	{
		acpigen_write_name_byte("EPCS", epcs);
		acpigen_write_name_qword("EMNA", emna);
		acpigen_write_name_qword("ELNG", elng);
	}
	acpigen_pop_len();
}
