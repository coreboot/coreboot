#ifndef AMDK8_H

#define AMDK8_H

#if CONFIG_K8_REV_F_SUPPORT
        #include "f.h"
#else
        #include "pre_f.h"
#endif

#ifdef __PRE_RAM__
void showallroutes(int level, device_t dev);
void setup_resource_map_offset(const unsigned int *register_values, int max, unsigned offset_pci_dev, unsigned offset_io_base);
void fill_mem_ctrl(int controllers, struct mem_controller *ctrl_a, const uint16_t *spd_addr);
#endif

void cpus_ready_for_init(void);

#endif /* AMDK8_H */
