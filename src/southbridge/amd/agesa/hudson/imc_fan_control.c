#include "imc_fan_control.h"
#include <delay.h>

void imc_fan_control(void)
{
	u8 val;
	val = read8(ACPI_MMIO_BASE + PMIO2_BASE + 0x01);
	val |= BIT2;
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x01, val);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x02, 0xf7);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x03, 0xff);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x04, 0xff);

	val = read8(ACPI_MMIO_BASE + PMIO2_BASE + 0x11);
	val |= BIT2;
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x11, val);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x12, 0xf7);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x13, 0xff);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x14, 0xff);

	imc_func_0x83();
	imc_func_0x85();
	imc_func_0x81();
}

void write_imc_msg(u8 idx, u8 val)
{
	outb(idx, 0x3e);
	outb(val, 0x3f);
}

void imc_func_0x81(void)
{
	write_imc_msg(MSG_REG0, 0);
	write_imc_msg(MSG_REG1, 0);
	write_imc_msg(MSG_REG2, 0x3d);
	write_imc_msg(MSG_REG3, 0xe);
	write_imc_msg(MSG_REG4, 0);
	write_imc_msg(MSG_REG5, 0x54);
	write_imc_msg(MSG_REG6, 0x98);
	write_imc_msg(MSG_REG7, 0x2);
	write_imc_msg(MSG_REG8, 0x1);
	write_imc_msg(MSG_REG9, 0);
	write_imc_msg(MSG_SYS_TO_IMC, Fun_81);
	mdelay(50);
}

void imc_func_0x83(void)
{
	write_imc_msg(MSG_REG0, 0);
	write_imc_msg(MSG_REG1, 0);
	write_imc_msg(MSG_REG2, 0x69);
	write_imc_msg(MSG_REG3, 0x3c);
	write_imc_msg(MSG_REG4, 0xff);
	write_imc_msg(MSG_REG5, 0xff);
	write_imc_msg(MSG_REG6, 0xff);
	write_imc_msg(MSG_REG7, 0xff);
	write_imc_msg(MSG_REG8, 0xff);
	write_imc_msg(MSG_REG9, 0xff);
	write_imc_msg(MSG_REGA, 0x69);
	write_imc_msg(MSG_REGB, 0);
	write_imc_msg(MSG_SYS_TO_IMC, Fun_83);
	mdelay(50);
}

void imc_func_0x85(void)
{
	write_imc_msg(MSG_REG0, 0);
	write_imc_msg(MSG_REG1, 0);
	write_imc_msg(MSG_REG2, 0x64);
	write_imc_msg(MSG_REG3, 0xff);
	write_imc_msg(MSG_REG4, 0xff);
	write_imc_msg(MSG_REG5, 0xff);
	write_imc_msg(MSG_REG6, 0xff);
	write_imc_msg(MSG_REG7, 0xff);
	write_imc_msg(MSG_REG8, 0xff);
	write_imc_msg(MSG_REG9, 0xff);
	write_imc_msg(MSG_SYS_TO_IMC, Fun_85);
	mdelay(50);
}

void imc_func_0x89(void)
{
	/* not use */
}
