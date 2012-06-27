#ifndef CPU_INTEL_HYPERTHREADING_H
#define CPU_INTEL_HYPERTHREADING_H

struct device;
void intel_sibling_init(struct device *cpu);
int intel_ht_sibling(void);

#endif /* CPU_INTEL_HYPERTHREADING_H */
