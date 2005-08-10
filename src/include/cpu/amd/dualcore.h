#ifndef CPU_AMD_DUALCORE_H
#define CPU_AMD_DUALCORE_H

struct device;
void amd_sibling_init(struct device *cpu);

int is_e0_later_in_bsp(int nodeid);
unsigned int read_nb_cfg_54(void);

struct node_core_id {
        unsigned nodeid;
        unsigned coreid;
};

// it can be used to get unitid and coreid it running only
struct node_core_id get_node_core_id(unsigned int nb_cfg_54);
unsigned get_apicid_base(unsigned ioapic_num);

#endif /* CPU_AMD_DUALCORE_H */
