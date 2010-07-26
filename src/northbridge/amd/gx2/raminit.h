#ifndef RAMINIT_H
#define RAMINIT_H

#define DIMM_SOCKETS 2

struct mem_controller {
	uint16_t channel0[DIMM_SOCKETS];
};

void sdram_initialize(int controllers, const struct mem_controller *ctrl);

#endif /* RAMINIT_H */
