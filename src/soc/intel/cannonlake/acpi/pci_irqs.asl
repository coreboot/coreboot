/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Method(_PRT)
{
	Return(Package() {

		// PCI Bridge
		// cAVS, SMBus, GbE, Nothpeak
		Package(){0x001FFFFF, 0, 0, 16 },
		Package(){0x001FFFFF, 1, 0, 17 },
		Package(){0x001FFFFF, 2, 0, 18 },
		Package(){0x001FFFFF, 3, 0, 19 },
		// SerialIo and SCS
		Package(){0x001EFFFF, 0, 0, 20 },
		Package(){0x001EFFFF, 1, 0, 21 },
		Package(){0x001EFFFF, 2, 0, 22 },
		Package(){0x001EFFFF, 3, 0, 23 },
		// PCI Express Port 9-16
		Package(){0x001DFFFF, 0, 0, 16 },
		Package(){0x001DFFFF, 1, 0, 17 },
		Package(){0x001DFFFF, 2, 0, 18 },
		Package(){0x001DFFFF, 3, 0, 19 },
		// PCI Express Port 1-8
		Package(){0x001CFFFF, 0, 0, 16 },
		Package(){0x001CFFFF, 1, 0, 17 },
		Package(){0x001CFFFF, 2, 0, 18 },
		Package(){0x001CFFFF, 3, 0, 19 },
		// PCI Express Port 17-20
		Package(){0x001BFFFF, 0, 0, 16 },
		Package(){0x001BFFFF, 1, 0, 17 },
		Package(){0x001BFFFF, 2, 0, 18 },
		Package(){0x001BFFFF, 3, 0, 19 },
		// eMMC
		Package(){0x001AFFFF, 0, 0, 16 },
		Package(){0x001AFFFF, 1, 0, 17 },
		Package(){0x001AFFFF, 2, 0, 18 },
		Package(){0x001AFFFF, 3, 0, 19 },
		// SerialIo
		Package(){0x0019FFFF, 0, 0, 32 },
		Package(){0x0019FFFF, 1, 0, 33 },
		Package(){0x0019FFFF, 2, 0, 34 },
		// SATA controller
		Package(){0x0017FFFF, 0, 0, 16 },
		// CSME (HECI, IDE-R, Keyboard and Text redirection
		Package(){0x0016FFFF, 0, 0, 16 },
		Package(){0x0016FFFF, 1, 0, 17 },
		Package(){0x0016FFFF, 2, 0, 18 },
		Package(){0x0016FFFF, 3, 0, 19 },
		// SerialIo
		Package(){0x0015FFFF, 0, 0, 16 },
		Package(){0x0015FFFF, 1, 0, 17 },
		Package(){0x0015FFFF, 2, 0, 18 },
		Package(){0x0015FFFF, 3, 0, 19 },
		// CNL: D20: xHCI, OTG, CNVi WiFi, SDcard
		Package(){0x0014FFFF, 0, 0, 16 },
		Package(){0x0014FFFF, 1, 0, 17 },
		Package(){0x0014FFFF, 2, 0, 18 },
		Package(){0x0014FFFF, 3, 0, 19 },
		// Integrated Sensor Hub
		Package(){0x0013FFFF, 0, 0, 20 },
		// Thermal, UFS, SerialIo SPI 2
		Package(){0x0012FFFF, 0, 0, 16 },
		Package(){0x0012FFFF, 1, 0, 24 },
		Package(){0x0012FFFF, 2, 0, 18 },
		Package(){0x0012FFFF, 3, 0, 19 },

		// Host Bridge
		// Root Port D1F0
		Package(){0x0001FFFF, 0, 0, 16 },
		Package(){0x0001FFFF, 1, 0, 17 },
		Package(){0x0001FFFF, 2, 0, 18 },
		Package(){0x0001FFFF, 3, 0, 19 },
		// Root Port D1F1
		// Root Port D1F2
		// IGFX Device
		Package(){0x0002FFFF, 0, 0, 16 },
		// Thermal Device
		Package(){0x0004FFFF, 0, 0, 16 },
		// IPU Device
		Package(){0x0005FFFF, 0, 0, 16 },
		// GNA Device
		Package(){0x0008FFFF, 0, 0, 16 },
	})
}
