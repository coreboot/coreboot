#ifndef SUPERIO_W83627HF_H
#define SUPERIO_W83627HF_H

/* The base address is 0x2e,0x4e depending on config bytes */
#ifndef SIO_BASE
#define SIO_BASE 0x2e
#endif

#define SIO_SYSTEM_CLK_INPUT_48MHZ	(1<<6)
#define SIO_SYSTEM_CLK_INPUT_24MHZ	(0<<6)

#if defined(SIO_SYSTEM_CLK_INPUT)
#if (SIO_SYSTEM_CLK_INPUT != SIO_SYSTEM_CLK_INPUT_48MHZ) &&  (SIO_SYSTEM_CLK_INPUT != SIO_SYSTEM_CLK_INPUT_24MHZ) 
#error BAD SIO_SYSTEM_CLK_INPUT_PARAMETER
#endif
#endif

#define FLOPPY_DEVICE 0
#define PARALLEL_DEVICE 1
#define COM1_DEVICE 2
#define COM2_DEVICE 3
#define KBC_DEVICE  5
#define CIR_DEVICE  6
#define GAME_PORT_DEVICE 7
#define GPIO_PORT2_DEVICE 8
#define GPIO_PORT3_DEVICE 9
#define ACPI_DEVICE 0xa
#define HW_MONITOR_DEVICE 0xb


#define FLOPPY_DEFAULT_IOBASE		0x3f0
#define FLOPPY_DEFAULT_IRQ		6
#define FLOPPY_DEFAULT_DRQ		2
#define PARALLEL_DEFAULT_IOBASE		0x378
#define PARALLEL_DEFAULT_IRQ		7
#define PARALLEL_DEFAULT_DRQ		4 /* No dma */
#define COM1_DEFAULT_IOBASE		0x3f8
#define COM1_DEFAULT_IRQ		4
#define COM1_DEFAULT_BAUD		115200
#define COM2_DEFAULT_IOBASE		0x2f8
#define COM2_DEFAULT_IRQ		3
#define COM2_DEFAULT_BAUD		115200
#define KBC_DEFAULT_IOBASE0		0x60
#define KBC_DEFAULT_IOBASE1		0x64
#define KBC_DEFAULT_IRQ0		0x1
#define KBC_DEFAULT_IRQ1		0xc
#define HW_MONITOR_DEFAULT_IOBASE0	0x295

#if !defined(ASSEMBLY)
void w83627hf_enter_pnp(unsigned char port);
void w83627hf_exit_pnp(unsigned char port);

#define POWER_OFF  0
#define POWER_ON   1
#define POWER_PREV 2

void w83627hf_power_after_power_fail(int state);

#define LED_OFF		0x00
#define LED_ON		0x40
#define LED_BLINK_1SEC	0x80
#define LED_BLINK_4SEC	0xC0

void w83627hf_power_led(int);
#endif

#endif /* SUPERIO_W83627HF_H */
