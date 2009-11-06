#ifndef CPU_AMD_DUALCORE_H
#define CPU_AMD_DUALCORE_H

#if defined(__GNUC__) 
unsigned int read_nb_cfg_54(void);
#endif

struct node_core_id {
        unsigned nodeid;
        unsigned coreid;
};

#if defined(__GNUC__)
// it can be used to get unitid and coreid it running only
struct node_core_id get_node_core_id(unsigned int nb_cfg_54);
#endif

#if !defined( __ROMCC__ ) && !defined(__PRE_RAM__)
struct device;
unsigned get_apicid_base(unsigned ioapic_num);
void amd_sibling_init(struct device *cpu);
#endif

#endif /* CPU_AMD_DUALCORE_H */
