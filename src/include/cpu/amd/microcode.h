#ifndef CPU_AMD_MICROCODE_H
#define CPU_AMD_MICROCODE_H

#if CONFIG_UPDATE_CPU_MICROCODE || CONFIG_NORTHBRIDGE_AMD_AMDK8
void update_microcode(u32 cpu_deviceid);
void amd_update_microcode_from_cbfs(u32 equivalent_processor_rev_id);
#else
#define update_microcode(x)
#endif
#endif /* CPU_AMD_MICROCODE_H */

