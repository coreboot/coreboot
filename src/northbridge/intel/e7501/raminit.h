#ifndef RAMINIT_H
#define RAMINIT_H

#define MAX_DIMM_SOCKETS_PER_CHANNEL 4
#define MAX_NUM_CHANNELS 2
#define MAX_DIMM_SOCKETS (MAX_NUM_CHANNELS * MAX_DIMM_SOCKETS_PER_CHANNEL)

struct mem_controller {
	device_t d0, d0f1;					// PCI bus/device/fcns of E7501 memory controller

	// SMBus addresses of DIMM slots for each channel,
	// in order from closest to MCH to furthest away
	// 0 == not present
	uint16_t channel0[MAX_DIMM_SOCKETS_PER_CHANNEL];
	uint16_t channel1[MAX_DIMM_SOCKETS_PER_CHANNEL];
};

void sdram_initialize(int controllers, const struct mem_controller *ctrl);

#endif /* RAMINIT_H */
