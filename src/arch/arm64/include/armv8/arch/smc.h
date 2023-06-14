/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARM_ARM64_SMC_H
#define ARM_ARM64_SMC_H

#include <types.h>

uint64_t smc(uint32_t function_id, uint64_t arg1, uint64_t arg2, uint64_t arg3,
		uint64_t arg4, uint64_t arg5, uint64_t arg6, uint64_t arg7);

#define smc_call0(function_id)			smc(function_id, 0, 0, 0, 0, 0, 0, 0)
#define smc_call1(function_id, a1)		smc(function_id, a1, 0, 0, 0, 0, 0, 0)
#define smc_call2(function_id, a1, a2)		smc(function_id, a1, a2, 0, 0, 0, 0, 0)
#define smc_call3(function_id, a1, a2, a3)	smc(function_id, a1, a2, a3, 0, 0, 0, 0)

/* Documented in https://developer.arm.com/documentation/den0022/ */
enum psci_return_values {
	PSCI_SUCCESS = 0,
	PSCI_NOT_SUPPORTED = -1,
	PSCI_INVALID_PARAMETERS = -2,
	PSCI_DENIED = -3,
	PSCI_ALREADY_ON = -4,
	PSCI_ON_PENDING = -5,
	PSCI_INTERNAL_FAILURE = -6,
	PSCI_NOT_PRESENT = -7,
	PSCI_DISABLED = -8,
	PSCI_INVALID_ADDRESS = -9,
};

/* PSCI functions */
#define PSCI_VERSION		0x84000000
#define PSCI_FEATURES		0x8400000a

/* Documented in https://developer.arm.com/documentation/den0028/ */
enum smccc_return_values {
	SMC_SUCCESS = 0,
	SMC_NOT_SUPPORTED = -1,
	SMC_NOT_REQUIRED = -2,
	SMC_INVALID_PARAMETER = -3,
};

/* SMCCC functions */
#define SMCCC_VERSION		0x80000000
#define SMCCC_ARCH_FEATURES	0x80000001
#define SMCCC_ARCH_SOC_ID	0x80000002
#define  SMCCC_GET_SOC_VERSION	0
#define  SMCCC_GET_SOC_REVISION	1

uint8_t smccc_supports_arch_soc_id(void);
enum cb_err smccc_arch_soc_id(uint32_t *jep106code, uint32_t *soc_revision);

#endif /* ARM_ARM64_SMC_H */
