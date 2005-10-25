#ifndef CPU_AMD_DUALCORE_H
#define CPU_AMD_DUALCORE_H

struct device;

struct node_core_id {
        unsigned nodeid;
        unsigned coreid;
};

void amd_sibling_init(struct device *cpu, struct node_core_id id);
struct node_core_id get_node_core_id(void);

#endif /* CPU_AMD_DUALCORE_H */
