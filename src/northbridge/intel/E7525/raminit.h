#ifndef RAMINIT_H
#define RAMINIT_H

#define DIMM_SOCKETS 4
struct mem_controller {
	unsigned node_id;
	device_t f0, f1, f2, f3;
	uint16_t channel0[DIMM_SOCKETS];
	uint16_t channel1[DIMM_SOCKETS];
};

#endif /* RAMINIT_H */
