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

#endif /* I82801EX_H */
