/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

struct northbridge_via_vx900_config {
	/**
	 * \brief PCIe Lane[3:0] Function Select
	 *
	 * PCIe Lane3~Lane0 (PEXTX[3:0]P/VCC) can be used by the integrated
	 * graphic controller to output its display data. The PCIe lanes will
	 * be used to output DisplayPort data.
	 */
	u8 assign_pex_to_dp;

	/**
	 * \brief Lane Width for Root Port 1
	 *
	 * Two PCIe lanes are used for Root port 1. Root port 2 is disabled.
	 */
	u8 pcie_port1_2_lane_wide;

	/**
	 * \brief PIRQ line to which to route the external interrupt
	 *
	 * The VX900 features an external interrupt which can be routed to any
	 * of the PIRQA->PIRQH lines. Usually, on-board devices are connected
	 * to the external interrupt. In some vendor BIOS's pirq table, this
	 * appears as link 9.
	 *
	 * Setting this line only affects the behavior of the integrated PIC. It
	 * has no effect on the IOAPIC.
	 *
	 * The value of this register must be a literal upper-case character
	 * from 'A' to 'H'.
	 */
	char ext_int_route_to_pirq;
};
