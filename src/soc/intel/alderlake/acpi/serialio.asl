/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel Serial IO Devices */

Device (I2C0)
{
	Name (_ADR, 0x00150000)
	Name (_DDN, "Serial IO I2C Controller 0")
}

Device (I2C1)
{
	Name (_ADR, 0x00150001)
	Name (_DDN, "Serial IO I2C Controller 1")
}

Device (I2C2)
{
	Name (_ADR, 0x00150002)
	Name (_DDN, "Serial IO I2C Controller 2")
}

Device (I2C3)
{
	Name (_ADR, 0x00150003)
	Name (_DDN, "Serial IO I2C Controller 3")
}

Device (I2C4)
{
	Name (_ADR, 0x00190000)
	Name (_DDN, "Serial IO I2C Controller 4")
}

Device (I2C5)
{
	Name (_ADR, 0x00190001)
	Name (_DDN, "Serial IO I2C Controller 5")
}

Device (I2C6)
{
	Name (_ADR, 0x00100000)
	Name (_DDN, "Serial IO I2C Controller 6")
}

Device (I2C7)
{
	Name (_ADR, 0x00100001)
	Name (_DDN, "Serial IO I2C Controller 7")
}

Device (SPI0)
{
	Name (_ADR, 0x001e0002)
	Name (_DDN, "Serial IO SPI Controller 0")
}

Device (SPI1)
{
	Name (_ADR, 0x001e0003)
	Name (_DDN, "Serial IO SPI Controller 1")
}

Device (SPI2)
{
	Name (_ADR, 0x00120006)
	Name (_DDN, "Serial IO SPI Controller 2")
}

Device (SPI3)
{
	Name (_ADR, 0x00130000)
	Name (_DDN, "Serial IO SPI Controller 3")
}

Device (UAR0)
{
	Name (_ADR, 0x001e0000)
	Name (_DDN, "Serial IO UART Controller 0")
}

Device (UAR1)
{
	Name (_ADR, 0x001e0001)
	Name (_DDN, "Serial IO UART Controller 1")
}

Device (UAR2)
{
	Name (_ADR, 0x00190002)
	Name (_DDN, "Serial IO UART Controller 2")
}

Device (UAR3)
{
	Name (_ADR, 0x00110000)
	Name (_DDN, "Serial IO UART Controller 3")
}
