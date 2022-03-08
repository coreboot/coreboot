/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CPU_INTEL_COMMON_H
#define _CPU_INTEL_COMMON_H

#include <types.h>
#include <acpi/acpigen.h>

void set_vmx_and_lock(void);
void set_feature_ctrl_vmx(void);
void set_feature_ctrl_vmx_arg(bool enable);
void set_feature_ctrl_lock(void);

/*
 * Init CPPC block with MSRs for Intel Enhanced Speed Step Technology.
 * Version 2 is suggested--this function's implementation of version 3
 * may have room for improvement.
 */
struct cppc_config;
void cpu_init_cppc_config(struct cppc_config *config, u32 version);

/*
 * Returns true if CPU supports Hyper-Threading.
 */
bool intel_ht_supported(void);

/*
 * Returns true if it's not thread 0 on a hyperthreading enabled core.
 */
bool intel_ht_sibling(void);

/*
 * Lock AES-NI feature (MSR_FEATURE_CONFIG) to prevent unintended changes
 * to the enablement state as suggested in Intel document 325384-070US.
 */
void set_aesni_lock(void);

/* Enable local CPU APIC TPR (Task Priority Register) updates */
void enable_lapic_tpr(void);

/* Enable DCA (Direct Cache Access) */
void configure_dca_cap(void);

/*
 * Set EPB (Energy Performance Bias)
 * Possible values are 0 (performance) to 15 (powersave).
 */
void set_energy_perf_bias(u8 policy);

/*
 * Check energy performance preference and HWP capabilities from Thermal and
 * Power Management Leaf CPUID.
 */
bool check_energy_perf_cap(void);

/*
 * Set the IA32_HWP_REQUEST Energy-Performance Preference bits on the logical
 * thread. 0 is a hint to the HWP to prefer performance, and 255 is a hint to
 * prefer energy efficiency.
 */
void set_energy_perf_pref(u8 pref);

/*
 * Instructs the CPU to use EPP hints. This means that any energy policies set
 * up in `set_energy_perf_bias` will be ignored afterwards.
 */
void enable_energy_perf_pref(void);

#endif
