#ifndef RAMINIT_H
#define RAMINIT_H

struct mem_controller {
	unsigned node_id;
	device_t f0, f1, f2, f3;
	uint16_t channel0[4]; //By LYH
	uint16_t channel1[4]; //By LYH
};


#endif /* RAMINIT_H */
