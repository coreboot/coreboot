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

	uint8_t	npem_address;			// SMBUS address of IO expander which provides NPEM
	uint8_t	npem_bank;			// Port or bank on IoExpander which provides NPEM
	uint8_t	npem_mux_address;		// SMBUS address of MUX used to access NPEM
	uint8_t	npem_mux_channel;		// Channel of the MUX used to access NPEM

	uint8_t	slot_eip;			// Electromechanical Interlock Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B17)
	uint8_t	slot_hps;			// Hot Plug surprise supported -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B5)
	uint8_t	slot_pind;			// Power Indicator Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B4)
	uint8_t	slot_aind;			// Attention Inductor Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B3)
	uint8_t	slot_pctl;			// Power Controller Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B1)
	uint8_t	slot_abtn;			// Attention Button Present -
						//   Slot Capabilities (D0-10 / F0 / R0xA4 / B0)

	uint8_t	vpp_enabled;			// If VPP is supported on given port
	uint8_t	vpp_exp_type;			// IO Expander type used for VPP (see IIO_VPP_EXPANDER_TYPE
						//   for values definitions)
	uint8_t	npem_supported;			// If NPEM is supported on given port

	uint8_t	slot_implemented;
	uint8_t	retimer_1_present;
	uint8_t	retimer_2_present;
	uint8_t	common_clock;
	uint8_t	sris;

	uint16_t hot_plug;			// If hotplug is supported on slot connected to this port
	uint16_t mrl_sensor_present;		// If MRL is present on slot connected to this port
	uint16_t slot_power_limit_scale;	// Slot Power Scale for slot connected to this port
	uint16_t slot_power_limit_value;	// Slot Power Value for slot connected to this port

	uint16_t physical_slot_number;		// Slot number for slot connected to this port
};

struct iio_pe_config {
	uint8_t			socket;
	IIO_PACKAGE_PE		pe;
	IIO_BIFURCATION		bifurcation;
	uint8_t			cxl_support;
	uint8_t			reserved;
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

#define _IIO_PORT_CFG_STRUCT(vppen, vppex, vaddr, vport, vmuxa, vmuxc,\
		slteip, slthps, sltpind, sltaind, sltpctl, sltabtn,\
		sltimpl, hotp, mrlsp, sltpls, sltplv, psn,\
		retimer1, retimer2, comclk, dsris,\
		npmspt, npaddr, npb, npma, npmc\
		) {\
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
	.slot_implemented	= (sltimpl),\
	.retimer_1_present	= (retimer1),\
	.retimer_2_present	= (retimer2),\
	.common_clock		= (comclk),\
	.sris			= (dsris),\
	.hot_plug		= (hotp),\
	.mrl_sensor_present	= (mrlsp),\
	.slot_power_limit_scale	= (sltpls),\
	.slot_power_limit_value	= (sltplv),\
	.physical_slot_number	= (psn),\
	.npem_supported		= (npmspt),\
	.npem_address		= (npaddr),\
	.npem_bank		= (npb),\
	.npem_mux_address	= (npma),\
	.npem_mux_channel	= (npmc),\
}

#define _IIO_PORT_CFG_STRUCT_DISABLED \
	_IIO_PORT_CFG_STRUCT(0x0, 0x0, 0x0, 0x0, 0x0, 0x0,\
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0,\
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0,\
		0x0, 0x0, 0x0, 0x0,\
		0x0, 0x0, 0x0, 0x0, 0x0)

#define _IIO_PORT_CFG_STRUCT_X8 _IIO_PORT_CFG_STRUCT
#define _IIO_PORT_CFG_STRUCT_X4 _IIO_PORT_CFG_STRUCT
#define _IIO_PORT_CFG_STRUCT_X2 _IIO_PORT_CFG_STRUCT

#define _IIO_PORT_CFG_STRUCT_BASIC(sltpls, sltplv, psn)\
	_IIO_PORT_CFG_STRUCT(0x0, 0x0, 0x0, 0x0, 0x0, 0x0,\
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0,\
		0x1, 0x0, 0x0, sltpls, sltplv, psn,\
		0x0, 0x0, 0x1, 0x0,\
		0x0, 0x0, 0x0, 0x0, 0x0)

#define _IIO_PORT_CFG_STRUCT_BASIC_X8 _IIO_PORT_CFG_STRUCT_BASIC
#define _IIO_PORT_CFG_STRUCT_BASIC_X4 _IIO_PORT_CFG_STRUCT_BASIC
#define _IIO_PORT_CFG_STRUCT_BASIC_X2 _IIO_PORT_CFG_STRUCT_BASIC

#define _IIO_PE_CFG_DISABLED(socket, pe) {\
	_IIO_PE_CFG_STRUCT(socket, pe, IIO_BIFURCATE_AUTO, PE_TYPE_PCIE) {\
		_IIO_PORT_CFG_STRUCT_DISABLED,\
		_IIO_PORT_CFG_STRUCT_DISABLED,\
		_IIO_PORT_CFG_STRUCT_DISABLED,\
		_IIO_PORT_CFG_STRUCT_DISABLED,\
		_IIO_PORT_CFG_STRUCT_DISABLED,\
		_IIO_PORT_CFG_STRUCT_DISABLED,\
		_IIO_PORT_CFG_STRUCT_DISABLED,\
		_IIO_PORT_CFG_STRUCT_DISABLED,\
	}}

void soc_config_iio_pe_ports(FSPM_UPD *mupd, const struct iio_pe_config *config_table,
				unsigned int num_entries);

const struct iio_pe_config *get_iio_config_table(int *size);

#endif /* _SOC_IIO_H_ */
