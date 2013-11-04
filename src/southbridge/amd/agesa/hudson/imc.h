#ifndef _IMC_FAN_CONTROL_H_
#define _IMC_FAN_CONTROL_H_

#include <arch/io.h>
#include "FchPlatform.h"

void imc_fan_control(void);
void write_imc_msg(u8 idx, u8 val);
void imc_func_0x81(void);
void imc_func_0x83(void);
void imc_func_0x85(void);
void imc_func_0x89(void);

#endif
