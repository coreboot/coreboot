#ifndef _CPU_AMD_CAR_H
#define _CPU_AMD_CAR_H

#if CONFIG_CPU_AMD_GEODE_LX
void done_cache_as_ram_main(void);
#endif

#if !(CONFIG_CPU_AMD_GEODE_LX || CONFIG_CPU_AMD_GEODE_GX1 || CONFIG_CPU_AMD_GEODE_GX2)
void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx);
#endif

#if CONFIG_CPU_AMD_AGESA
void disable_cache_as_ram(void);
#endif

#if CONFIG_CPU_AMD_MODEL_FXX || CONFIG_CPU_AMD_MODEL_10XXX
void cache_as_ram_switch_stack(void *resume_backup_memory);
void cache_as_ram_new_stack(void *resume_backup_memory);
void post_cache_as_ram(void);
#endif

#endif
