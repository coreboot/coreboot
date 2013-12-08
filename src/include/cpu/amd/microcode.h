#ifndef CPU_AMD_MICROCODE_H
#define CPU_AMD_MICROCODE_H

void amd_update_microcode(void *microcode_updates, unsigned processor_rev_id);
void model_fxx_update_microcode(unsigned cpu_deviceid);

#if CONFIG_UPDATE_CPU_MICROCODE
void update_microcode(u32 processor_rev_id);
#else
#define update_microcode(x)
#endif
#endif /* CPU_AMD_MICROCODE_H */

