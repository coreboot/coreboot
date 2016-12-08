/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Scope (\_SB.PCI0.SPI1)
{
	Device (FPC)
	{
		Name (_HID, "PRP0001")
		Name (_UID, 1)
		Name (_CRS, ResourceTemplate ()
		{
			SpiSerialBus (
				0,                   // DeviceSelection (CS0)
				PolarityLow,         // DeviceSelectionPolarity
				FourWireMode,        // WireMode
				8,                   // DataBitLength
				ControllerInitiated, // SlaveMode
				1000000,             // ConnectionSpeed (1MHz)
				ClockPolarityLow,    // ClockPolarity
				ClockPhaseFirst,     // ClockPhase
				"\\_SB.PCI0.SPI1",   // ResourceSource
				0,                   // ResourceSourceIndex
				ResourceConsumer,    // ResourceUsage
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow) { 0x50 }
		})
		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {
					"compatible",
					Package () { "fpc,fpc1020" }
				},
			}
		})
	}
}
