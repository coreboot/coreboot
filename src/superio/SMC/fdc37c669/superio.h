#ifndef _SMC_SUPER_IO_H_
#define _SMC_SUPER_IO_H_

#define SMC_PP_MODE_SPP     0x00
#define SMC_PP_MODE_EPP_SPP 0x01
#define SMC_PP_MODE_ECP     0x02
#define SMC_PP_MODE_EPP_ECP 0x03

int smc_uart_setup(int smc_addr,
		   int addr1, int irq1,
		   int addr2, int irq2);

int smc_pp_setup(int smc_addr, int pp_addr, int mode);

int smc_validbit(int smc_addr, int valid);

#endif /* _SMC_SUPER_IO_H_ */
