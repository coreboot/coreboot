#ifndef I82801EX_H
#define I82801EX_H

#include "chip.h"

extern void i82801ex_enable(device_t dev);

#define PCI_DMA_CFG     0x90
#define SERIRQ_CNTL     0x64
#define GEN_CNTL        0xd0
#define GEN_STS         0xd4
#define RTC_CONF        0xd8
#define GEN_PMCON_3     0xa4

#define PMBASE          0x40
#define ACPI_CNTL       0x44
#define   ACPI_EN	(1 << 4)
#define GPIO_BASE       0x58
#define GPIO_CNTL       0x5C
#define   GPIO_EN	(1 << 4)

#endif /* I82801EX_H */
