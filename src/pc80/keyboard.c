#include <cpu/p5/io.h>
#include <subr.h>
/* much better keyboard init courtesy ollie@sis.com.tw 
   TODO: Typematic Setting, the keyboard is too slow for me */
void pc_keyboard_init()
{
	volatile unsigned char regval;

	/* send cmd = 0xAA, self test 8042 */
	outb(0xaa, 0x64);

	/* empty inut bufferm or any other command/data will be lost */
	while ((inb(0x64) & 0x02))
		intel_post(0);
	/* empty output buffer or any other command/data will be lost */
	while ((inb(0x64) & 0x01) == 0)
		intel_post(1);

	/* read self-test result, 0x55 should be returned form 0x60 */
	if ((regval = inb(0x60) != 0x55))
		return;

	/* enable keyboard interface */
	outb(0x60, 0x64);
	while ((inb(0x64) & 0x02))
		intel_post(2);

	/* send cmd: enable IRQ 1 */
	outb(0x61, 0x60);
	while ((inb(0x64) & 0x02))
		intel_post(3);

	/* reset kerboard and self test  (keyboard side) */
	outb(0xff, 0x60);

	/* empty inut bufferm or any other command/data will be lost */
	while ((inb(0x64) & 0x02))
		intel_post(4);
	/* empty output buffer or any other command/data will be lost */
	while ((inb(0x64) & 0x01) == 0)
		intel_post(5);

	if ((regval = inb(0x60) != 0xfa))
		return;

	while ((inb(0x64) & 0x01) == 0)
		intel_post(6);
	if ((regval = inb(0x60) != 0xaa))
		return;
}
