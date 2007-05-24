#include <console/console.h>
#include <pc80/keyboard.h>
#include <device/device.h>
#include <arch/io.h>

static int kbd_empty_input_buffer(void)
{
	unsigned long timeout;
	for(timeout = 1000000; timeout && (inb(0x64) & 0x02); timeout--) {
		post_code(0);
	}
	return !!timeout;
}

static int kbd_empty_output_buffer(void)
{
	unsigned long timeout;
	for(timeout = 1000000; timeout && ((inb(0x64) & 0x01) == 0); timeout--) {
		post_code(0);
	}
	return !!timeout;
}

/* much better keyboard init courtesy ollie@sis.com.tw 
   TODO: Typematic Setting, the keyboard is too slow for me */
static void pc_keyboard_init(struct pc_keyboard *keyboard)
{
	unsigned char regval;

	/* send cmd = 0xAA, self test 8042 */
	outb(0xaa, 0x64);

	/* empty input buffer or any other command/data will be lost */
	if (!kbd_empty_input_buffer()) {
		printk_err("Keyboard input buffer would not empty\n");
		return;
	}

	/* empty output buffer or any other command/data will be lost */
	if (!kbd_empty_output_buffer()) {
		printk_err("Keyboard output buffer would not empty\n");
		return;
	}

	/* read self-test result, 0x55 should be returned form 0x60 */
	if ((regval = inb(0x60) != 0x55)) {
		printk_err("Keyboard selftest failed\n");
		return;
	}

	/* enable keyboard interface */
	outb(0x60, 0x64);
	kbd_empty_input_buffer();

	/* send cmd: enable IRQ 1 */
	outb(0x61, 0x60);
	kbd_empty_input_buffer();

	/* reset kerboard and self test  (keyboard side) */
	outb(0xff, 0x60);

	/* empty inut bufferm or any other command/data will be lost */
	kbd_empty_input_buffer();

	/* empty output buffer or any other command/data will be lost */
	kbd_empty_output_buffer();

	if ((regval = inb(0x60) != 0xfa))
		return;

	kbd_empty_output_buffer();
	if ((regval = inb(0x60) != 0xaa))
		return;
}

void init_pc_keyboard(unsigned port0, unsigned port1, struct pc_keyboard *kbd)
{
	if ((port0 == 0x60) && (port1 == 0x64)) {
		pc_keyboard_init(kbd);
	}
}
