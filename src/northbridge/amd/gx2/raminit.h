#ifndef RAMINIT_H
#define RAMINIT_H

#define DIMM_SOCKETS 2

struct mem_controller {
	uint16_t channel0[DIMM_SOCKETS];
};

#endif /* RAMINIT_H */
