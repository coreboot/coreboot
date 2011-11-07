#ifndef I82801EX_H
#define I82801EX_H

#include "chip.h"

extern void i82801ex_enable(device_t dev);

#define PCICMD		0x4

#define ACPI_CNTL	0x44
#define SERIRQ_CNTL	0x64
#define PCI_DMA_CFG	0x90
#define GEN_PMCON_3	0xa4
#define SATA_RD_CFG	0xae
#define GEN_CNTL	0xd0
#define GEN_STA		0xd4
#define RTC_CONF	0xd8
#define GPIO_CNTL	0x5c
#define LPC_EN		0xe6
#define FUNC_DIS	0xf2

/* IDE, SATA */
#define IDE_TIMP	0x40
#define IDE_TIMS	0x42
#define SDMA_CNT	0x48
#define SDMA_TIM	0x4a
#define IDE_CONFIG	0x54

#endif /* I82801EX_H */
