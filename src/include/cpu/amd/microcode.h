#ifndef CPU_AMD_MICROCODE_H
#define CPU_AMD_MICROCODE_H

void update_microcode(u32 cpu_deviceid);
void amd_update_microcode_from_cbfs(u32 equivalent_processor_rev_id);

#endif /* CPU_AMD_MICROCODE_H */
