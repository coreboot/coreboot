/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_IIO_H_
#define _SOC_IIO_H_

#include <soc/soc_util.h>

#include <fsp/util.h>
#include <FspmUpd.h>
#include <IioPcieConfigUpd.h>

#define CB_IIO_BIFURCATE_xxxxxxxx		IIO_BIFURCATE_xxxxxxxx
#define CB_IIO_BIFURCATE_x4x4x4x4		IIO_BIFURCATE_x4x4x4x4
#define CB_IIO_BIFURCATE_x8xxx4x4		IIO_BIFURCATE_x4x4xxx8
#define CB_IIO_BIFURCATE_x4x4x8xx		IIO_BIFURCATE_xxx8x4x4
#define CB_IIO_BIFURCATE_x8xxx8xx		IIO_BIFURCATE_xxx8xxx8
#define CB_IIO_BIFURCATE_x16xxxxx		IIO_BIFURCATE_xxxxxx16
#define CB_IIO_BIFURCATE_x8x4x2x2		IIO_BIFURCATE_x2x2x4x8
#define CB_IIO_BIFURCATE_x8x2x2x4		IIO_BIFURCATE_x4x2x2x8
#define CB_IIO_BIFURCATE_x4x2x2x8		IIO_BIFURCATE_x8x2x2x4
#define CB_IIO_BIFURCATE_x2x2x4x8		IIO_BIFURCATE_x8x4x2x2
#define CB_IIO_BIFURCATE_x4x4x4x2x2		IIO_BIFURCATE_x2x2x4x4x4
#define CB_IIO_BIFURCATE_x4x4x2x2x4		IIO_BIFURCATE_x4x2x2x4x4
#define CB_IIO_BIFURCATE_x4x2x2x4x4		IIO_BIFURCATE_x4x4x2x2x4
#define CB_IIO_BIFURCATE_x2x2x4x4x4		IIO_BIFURCATE_x4x4x4x2x2
#define CB_IIO_BIFURCATE_x8x2x2x2x2		IIO_BIFURCATE_x2x2x2x2x8
#define CB_IIO_BIFURCATE_x2x2x2x2x8		IIO_BIFURCATE_x8x2x2x2x2
#define CB_IIO_BIFURCATE_x4x4x2x2x2x2		IIO_BIFURCATE_x2x2x2x2x4x4
#define CB_IIO_BIFURCATE_x4x2x2x4x2x2		IIO_BIFURCATE_x2x2x4x2x2x4
#define CB_IIO_BIFURCATE_x2x2x4x4x2x2		IIO_BIFURCATE_x2x2x4x4x2x2
#define CB_IIO_BIFURCATE_x4x2x2x2x2x4		IIO_BIFURCATE_x4x2x2x2x2x4
#define CB_IIO_BIFURCATE_x2x2x4x2x2x4		IIO_BIFURCATE_x4x2x2x4x2x2
#define CB_IIO_BIFURCATE_x2x2x2x2x4x4		IIO_BIFURCATE_x4x4x2x2x2x2
#define CB_IIO_BIFURCATE_x4x2x2x2x2x2x2		IIO_BIFURCATE_x2x2x2x2x2x2x4
#define CB_IIO_BIFURCATE_x2x2x4x2x2x2x2		IIO_BIFURCATE_x2x2x2x2x4x2x2
#define CB_IIO_BIFURCATE_x2x2x2x2x4x2x2		IIO_BIFURCATE_x2x2x4x2x2x2x2
#define CB_IIO_BIFURCATE_x2x2x2x2x2x2x4		IIO_BIFURCATE_x4x2x2x2x2x2x2
#define CB_IIO_BIFURCATE_x2x2x2x2x2x2x2x2	IIO_BIFURCATE_x2x2x2x2x2x2x2x2
#define CB_IIO_BIFURCATE_AUTO			IIO_BIFURCATE_AUTO

struct iio_port_config {
	uint8_t	vpp_address;			// SMBUS address of IO expander which provides VPP register
	uint8_t	vpp_port;			// Port or bank on IoExpander which provides VPP register
	uint8_t	vpp_mux_address;		// SMBUS address of MUX used to access VPP
	uint8_t	vpp_mux_channel;		// Channel of the MUX used to access VPP

	uint8_t	slot_eip:1;			// Electromechanical Interlock Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B17)
	uint8_t	slot_hps:1;			// Hot Plug surprise supported -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B5)
	uint8_t	slot_pind:1;			// Power Indicator Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B4)
	uint8_t	slot_aind:1;			// Attention Inductor Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B3)
	uint8_t	slot_pctl:1;			// Power Controller Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B1)
	uint8_t	slot_abtn:1;			// Attention Button Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B0)
	uint8_t	slot_rsvd:2;			// Reserved

	uint8_t	vpp_enabled:1;			// If VPP is supported on given port
	uint8_t	vpp_exp_type:1;			// IO Expander type used for VPP (see IIO_VPP_EXPANDER_TYPE
						//   for values definitions)

	uint8_t	slot_implemented:1;
	uint8_t	reserved:4;

	uint16_t hot_plug:1;			// If hotplug is supported on slot connected to this port
	uint16_t mrl_sensor_present:1;		// If MRL is present on slot connected to this port
	uint16_t slot_power_limit_scale:2;	// Slot Power Scale for slot connected to this port
	uint16_t slot_power_limit_value:12;	// Slot Power Value for slot connected to this port

	uint16_t physical_slot_number;		// Slot number for slot connected to this port
};

struct iio_pe_config {
	uint8_t			socket;
	IIO_PACKAGE_PE		pe;
	IIO_BIFURCATION		bifurcation;
	uint8_t			cxl_support:1;
	uint8_t			reserved:7;
	struct iio_port_config	port_config[MAX_IIO_PORTS_PER_STACK];
};

/*
 * {_IIO_PE_CFG_STRUCT(socket, pe, bif, cxl) {
 *    _IIO_PORT_CFG_STRUCT(vppen vppex vaddr vport vmuxa vmuxc ...),
 *    _IIO_PORT_CFG_STRUCT(..),
 *    ...
 *    _IIO_PORT_CFG_STRUCT(..) //MAX_IIO_PORTS_PER_STACK port configs
 * }}
 */

#define PE_TYPE_CXL  1
#define PE_TYPE_PCIE 0

#define _IIO_PE_CFG_STRUCT(s, p, bif, cxl) \
	.socket			= (s),\
	.pe			= (p),\
	.bifurcation		= (bif),\
	.cxl_support		= (cxl),\
	.reserved		= 0,\
	.port_config		=

/* TODO: to update rsv1 - rsv5 after SoC launch */
#define _IIO_PORT_CFG_STRUCT(vppen, vppex, vaddr, vport, vmuxa, vmuxc,\
		slteip, slthps, sltpind, sltaind, sltpctl, sltabtn, hotp, mrlsp,\
		sltimpl, sltpls, sltplv, psn,\
		rsv1, rsv2, rsv3, rsv4, rsv5) {\
	.vpp_enabled		= (vppen),\
	.vpp_exp_type		= (vppex),\
	.vpp_address		= (vaddr),\
	.vpp_port		= (vport),\
	.vpp_mux_address	= (vmuxa),\
	.vpp_mux_channel	= (vmuxc),\
	.slot_eip		= (slteip),\
	.slot_hps		= (slthps),\
	.slot_pind		= (sltpind),\
	.slot_aind		= (sltaind),\
	.slot_pctl		= (sltpctl),\
	.slot_abtn		= (sltabtn),\
	.slot_rsvd		= 0,\
	.slot_implemented	= (sltimpl),\
	.reserved		= 0,\
	.hot_plug		= (hotp),\
	.mrl_sensor_present	= (mrlsp),\
	.slot_power_limit_scale	= (sltpls),\
	.slot_power_limit_value	= (sltplv),\
	.physical_slot_number	= (psn)\
}

#define _IIO_PORT_CFG_STRUCT_DISABLED \
	_IIO_PORT_CFG_STRUCT(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,\
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0)

#define _IIO_PORT_CFG_STRUCT_X8 _IIO_PORT_CFG_STRUCT
#define _IIO_PORT_CFG_STRUCT_X4 _IIO_PORT_CFG_STRUCT
#define _IIO_PORT_CFG_STRUCT_X2 _IIO_PORT_CFG_STRUCT

#define _IIO_PORT_CFG_STRUCT_BASIC(sltpls, sltplv, psn)\
	_IIO_PORT_CFG_STRUCT(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,\
		0x0, 0x0, 0x0, 0x1, sltpls, sltplv, psn, 0x0, 0x0, 0x0, 0x0, 0x0)

#define _IIO_PORT_CFG_STRUCT_BASIC_X8 _IIO_PORT_CFG_STRUCT_BASIC
#define _IIO_PORT_CFG_STRUCT_BASIC_X4 _IIO_PORT_CFG_STRUCT_BASIC
#define _IIO_PORT_CFG_STRUCT_BASIC_X2 _IIO_PORT_CFG_STRUCT_BASIC

void soc_config_iio_pe_ports(FSPM_UPD *mupd, const struct iio_pe_config *config_table,
				unsigned int num_entries);

const struct iio_pe_config *get_iio_config_table(int *size);

#endif /* _SOC_IIO_H_ */
