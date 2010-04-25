#ifndef RAMINIT_H
#define RAMINIT_H

#define NODE_NUMS 8

#define DIMM_SOCKETS 4
struct mem_controller {
	unsigned node_id;
	device_t f0, f1, f2, f3;
	uint16_t channel0[DIMM_SOCKETS];
	uint16_t channel1[DIMM_SOCKETS];
};

struct sys_info;
void exit_from_self(int controllers, const struct mem_controller *ctrl, struct sys_info *sysinfo);

#if defined(__PRE_RAM__) && defined(RAMINIT_SYSINFO) && RAMINIT_SYSINFO == 1
void sdram_initialize(int controllers, const struct mem_controller *ctrl, void *sysinfo);
#else
void sdram_initialize(int controllers, const struct mem_controller *ctrl);
#endif

#endif /* RAMINIT_H */
