#include <delay.h>
#include "imc.h"

void imc_func_0x81(void)
{
	write_imc_msg(MSG_REG0, 0x00);
	write_imc_msg(MSG_REG1, 0x00);
	write_imc_msg(MSG_REG2, 0x3d);
	write_imc_msg(MSG_REG3, 0x4e);
	write_imc_msg(MSG_REG4, 0x00);
	write_imc_msg(MSG_REG5, 0x04);
	write_imc_msg(MSG_REG6, 0x9a);
	write_imc_msg(MSG_REG7, 0x01);
	write_imc_msg(MSG_REG8, 0x01);
	write_imc_msg(MSG_REG9, 0x00);
	write_imc_msg(MSG_SYS_TO_IMC, Fun_81);
	mdelay(50);
}

void imc_func_0x83(void)
{
	write_imc_msg(MSG_REG0, 0x00);
	write_imc_msg(MSG_REG1, 0x00);
	write_imc_msg(MSG_REG2, 0x46);
	write_imc_msg(MSG_REG3, 0x3c);
	write_imc_msg(MSG_REG4, 0x32);
	write_imc_msg(MSG_REG5, 0xff);
	write_imc_msg(MSG_REG6, 0xff);
	write_imc_msg(MSG_REG7, 0xff);
	write_imc_msg(MSG_REG8, 0xff);
	write_imc_msg(MSG_REG9, 0xff);
	write_imc_msg(MSG_REGA, 0x4b);
	write_imc_msg(MSG_REGB, 0x00);
	write_imc_msg(MSG_SYS_TO_IMC, Fun_83);
	mdelay(50);
}

void imc_func_0x85(void)
{
	write_imc_msg(MSG_REG0, 0x00);
	write_imc_msg(MSG_REG1, 0x00);
	write_imc_msg(MSG_REG2, 0x5a);
	write_imc_msg(MSG_REG3, 0x46);
	write_imc_msg(MSG_REG4, 0x28);
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
