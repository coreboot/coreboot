#ifndef CPU_K7_MTRR_H
#define CPU_K7_MTRR_H

#include <cpu/p6/mtrr.h>

#define IORR_FIRST 0xC0010016
#define IORR_LAST  0xC0010019
#define SYSCFG     0xC0010010

#define MTRR_READ_MEM			(1 << 4)
#define MTRR_WRITE_MEM			(1 << 3)

#define SYSCFG_MSR			0xC0010010
#define SYSCFG_MSR_MtrrFixDramEn	(1 << 18)
#define SYSCFG_MSR_MtrrFixDramModEn	(1 << 19)
#define SYSCFG_MSR_MtrrVarDramEn	(1 << 20)


#define IORR0_BASE			0xC0010016
#define IORR0_MASK			0xC0010017
#define IORR1_BASE			0xC0010018
#define IORR1_MASK			0xC0010019
#define TOP_MEM				0xC001001A
#define TOP_MEM2			0xC001001D
#define HWCR_MSR			0xC0010015


#endif /* CPU_K7_MTRR_H */
