#ifndef AMDK8_H

#define AMDK8_H

#if CONFIG_K8_REV_F_SUPPORT == 1
        #include "amdk8_f.h"
#else
        #include "amdk8_pre_f.h"
#endif

#ifdef __PRE_RAM__
void showallroutes(int level, device_t dev);
void setup_resource_map_offset(const unsigned int *register_values, int max, unsigned offset_pci_dev, unsigned offset_io_base);
#endif

#endif /* AMDK8_H */
