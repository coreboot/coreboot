#ifndef RAMINIT_H
#define RAMINIT_H

#define DIMM_SOCKETS 4
struct mem_controller {
	device_t d0, d0f1; 
	uint16_t channel0[DIMM_SOCKETS];
	uint16_t channel1[DIMM_SOCKETS];
};


#endif /* RAMINIT_H */
