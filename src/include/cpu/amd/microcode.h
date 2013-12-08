#ifndef CPU_AMD_MICROCODE_H
#define CPU_AMD_MICROCODE_H

#if CONFIG_SUPPORT_CPU_UCODE_IN_CBFS
void amd_update_microcode_from_cbfs(unsigned processor_rev_id);
void model_fxx_update_microcode(unsigned cpu_deviceid);
void update_microcode(u32 processor_rev_id);
#else
#define update_microcode(x)
#endif
#endif /* CPU_AMD_MICROCODE_H */

