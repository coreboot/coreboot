#ifndef CPU_INTEL_MSR_H
#define CPU_INTEL_MSR_H

/*
 * Common MSRs for Intel CPUs
 */

#define MSR_FEATURE_CONFIG	0x13c
#define  AESNI_DISABLE		(1 << 1)
#define  AESNI_LOCK		(1 << 0)

#endif /* CPU_INTEL_MSR_H */
