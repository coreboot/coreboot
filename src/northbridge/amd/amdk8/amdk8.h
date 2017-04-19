#ifndef AMDK8_H

#define AMDK8_H

#if CONFIG_K8_REV_F_SUPPORT

#define K8_REV_F_SUPPORT_F0_F1_WORKAROUND 0

	#include "f.h"
#else
	#include "pre_f.h"
#endif

#define HTIC_ColdR_Detect  (1<<4)
#define HTIC_BIOSR_Detect  (1<<5)
#define HTIC_INIT_Detect   (1<<6)

#ifdef __PRE_RAM__
void showallroutes(int level, pci_devfn_t dev);
void setup_resource_map_offset(const unsigned int *register_values, int max, unsigned offset_pci_dev, unsigned offset_io_base);
void fill_mem_ctrl(int controllers, struct mem_controller *ctrl_a, const uint16_t *spd_addr);
#endif

void set_bios_reset(void);
void distinguish_cpu_resets(unsigned int nodeid);
unsigned int get_sblk(void);
unsigned int get_sbbusn(unsigned sblk);

void cpus_ready_for_init(void);

#endif /* AMDK8_H */
