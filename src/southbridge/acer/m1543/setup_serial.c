#include <stdint.h>
#include <arch/io.h>

void enable_serial(void)
{
	const uint16_t config_port = 0x3f0; /* can be 0x370 */
	/* enter configuration mode */
	outb(0x51, config_port);
	outb(0x23, config_port);

	/* program logical device 4 */
	outb(0x07, config_port);
	outb(0x04, config_port+1);

	/* enable uart1 */
	outb(0x30, config_port);
	outb(0x01, config_port+1);

	/* select uart1 I/O base of 0x3f8 */
	outb(0x60, config_port);
	outb(0x03, config_port+1);
	outb(0x61, config_port);
	outb(0xf8, config_port+1);
	
	/* select uart1 irq 4 */
	outb(0x70, config_port);
	outb(0x04, config_port+1);

	/* select uart1 normal (not highspeed) mode */
	outb(0xf0, config_port);
	outb(0x00, config_port+1);

	/* select uart1 normal (not IR) mode */
	outb(0xf1, config_port);
	outb(0x00, config_port+1);

	/* select uart1 normal (no IR params) */
	outb(0xf2, config_port);
	outb(0x00, config_port+1);
	
	/* exit configuration mode */
	outb(0xbb, config_port);
}
