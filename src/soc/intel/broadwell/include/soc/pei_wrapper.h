/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_PEI_WRAPPER_H_
#define _BROADWELL_PEI_WRAPPER_H_

#include <soc/pei_data.h>

typedef int ABI_X86 (*pei_wrapper_entry_t)(struct pei_data *pei_data);

static inline void pei_data_usb2_port(struct pei_data *pei_data, int port,
				      uint16_t length, uint8_t enable,
				      uint8_t oc_pin, uint8_t location)
{
	pei_data->usb2_ports[port].length   = length;
	pei_data->usb2_ports[port].enable   = enable;
	pei_data->usb2_ports[port].oc_pin   = oc_pin;
	pei_data->usb2_ports[port].location = location;
}

static inline void pei_data_usb3_port(struct pei_data *pei_data, int port,
				      uint8_t enable, uint8_t oc_pin,
				      uint8_t fixed_eq)
{
	pei_data->usb3_ports[port].enable   = enable;
	pei_data->usb3_ports[port].oc_pin   = oc_pin;
	pei_data->usb3_ports[port].fixed_eq = fixed_eq;
}

#define SPD_MEMORY_DOWN	0xff

void broadwell_fill_pei_data(struct pei_data *pei_data);
void mainboard_fill_pei_data(struct pei_data *pei_data);
void mainboard_fill_spd_data(struct pei_data *pei_data);

#endif
