#ifndef I82801CX_H
#define I82801CX_H

#if !defined(__PRE_RAM__)
#include <device/device.h>
void i82801cx_enable(device_t dev);
#endif


#define PCI_DMA_CFG     0x90
#define SERIRQ_CNTL     0x64
#define GEN_CNTL        0xd0
#define GEN_STS         0xd4
#define RTC_CONF        0xd8
#define GEN_PMCON_3     0xa4

#define PMBASE          0x40
#define ACPI_CNTL       0x44
#define BIOS_CNTL       0x4E
#define GPIO_BASE       0x58
#define GPIO_CNTL       0x5C
#define PIRQA_ROUT      0x60
#define PIRQE_ROUT      0x68
#define COM_DEC         0xE0
#define LPC_EN          0xE6
#define FUNC_DIS        0xF2

// GEN_PMCON_3 bits
#define RTC_BATTERY_DEAD		(1<<2)
#define RTC_POWER_FAILED		(1<<1)
#define SLEEP_AFTER_POWER_FAIL	(1<<0)

/* IDE controller: */

// PCI Configuration Space (D31:F1)
#define IDE_TIM_PRI		0x40		// IDE timings, primary
#define IDE_TIM_SEC		0x42		// IDE timings, secondary


// IDE_TIM bits
#define IDE_DECODE_ENABLE	(1<<15)

/* SMBus: */

// PCI Configuration Space (D31:F3)
#define SMB_BASE	0x20
#define HOSTC		0x40

// HOSTC bits
#define I2C_EN		(1<<2)
#define SMB_SMI_EN	(1<<1)
#define HST_EN		(1<<0)

#define SMBUS_IO_BASE 0x1000

// I/O registers (relative to SMBUS_IO_BASE)
#define SMBHSTSTAT		0
#define SMBHSTCTL		2
#define SMBHSTCMD		3
#define SMBXMITADD		4
#define SMBHSTDAT0		5
#define SMBHSTDAT1		6
#define SMBBLKDAT		7
#define SMBTRNSADD		9
#define SMBSLVDATA		10
#define SMLINK_PIN_CTL	14
#define SMBUS_PIN_CTL	15

/* Between 1-10 seconds, We should never timeout normally
 * Longer than this is just painful when a timeout condition occurs.
 */
#define SMBUS_TIMEOUT (100*1000)

#endif /* I82801CX_H */
