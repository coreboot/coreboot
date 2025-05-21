/* SPDX-License-Identifier: GPL-2.0-only */

#include <libpayload.h>

/* Include source to gain access to private defines */
#include "../drivers/speaker.c"

#include <tests/test.h>

void outb(unsigned char val, int port)
{
	check_expected(val);
	check_expected(port);
}

unsigned char inb(int port)
{
	check_expected(port);
	return mock_type(unsigned char);
}

static void setup_speaker_enable_calls(u16 freq, u8 port_val)
{
	/* Minimal correct value should be at leaset 256. For values lowe than that,
	   counter will have an incorrect value. Regardless, there is  */
	u16 reg16 = 1193180 / freq;

	/* Select counter 2 */
	expect_value(outb, val, 0xb6);
	expect_value(outb, port, I82C54_CONTROL_WORD_REGISTER);

	/* Write freq. [LSB, MSB] */
	expect_value(outb, val, (u8)(reg16 & 0xff));
	expect_value(outb, port, I82C54_COUNTER2);
	expect_value(outb, val, (u8)(reg16 >> 8));
	expect_value(outb, port, I82C54_COUNTER2);

	/* Enable PC speaker */
	expect_value(inb, port, PC_SPEAKER_PORT);
	will_return(inb, port_val);
	expect_value(outb, val, port_val | 0x3);
	expect_value(outb, port, PC_SPEAKER_PORT);
}

static void test_speaker_enable(void **state)
{
	setup_speaker_enable_calls(1, 0);
	speaker_enable(1);

	setup_speaker_enable_calls(1, 0xff);
	speaker_enable(1);

	setup_speaker_enable_calls(1, 123);
	speaker_enable(1);

	setup_speaker_enable_calls(1, -1);
	speaker_enable(1);

	setup_speaker_enable_calls(-1, 0);
	speaker_enable(-1);

	setup_speaker_enable_calls(-1, 0xff);
	speaker_enable(-1);

	setup_speaker_enable_calls(-1, 222);
	speaker_enable(-1);

	setup_speaker_enable_calls(-1, -1);
	speaker_enable(-1);

	setup_speaker_enable_calls(10000, 0);
	speaker_enable(10000);

	setup_speaker_enable_calls(10000, 0xff);
	speaker_enable(10000);

	setup_speaker_enable_calls(10000, 91);
	speaker_enable(10000);

	setup_speaker_enable_calls(10000, -1);
	speaker_enable(10000);
}

static void setup_speaker_disable_calls(u8 value)
{
	expect_value(inb, port, PC_SPEAKER_PORT);
	will_return(inb, value);
	expect_value(outb, val, value & 0xfc);
	expect_value(outb, port, PC_SPEAKER_PORT);
}

static void test_speaker_disable(void **state)
{
	setup_speaker_disable_calls(0);
	speaker_disable();

	setup_speaker_disable_calls(0xfc);
	speaker_disable();

	setup_speaker_disable_calls(0xff);
	speaker_disable();

	setup_speaker_disable_calls(0xff - 0xfc);
	speaker_disable();
}

void ndelay(uint64_t ns)
{
	check_expected(ns);
}

static void setup_speaker_tone_calls(u16 freq, unsigned int duration)
{
	setup_speaker_enable_calls(freq, ~freq & 0xff);
	expect_value(ndelay, ns, (uint64_t)duration * NSECS_PER_MSEC);
	setup_speaker_disable_calls(0xff);
	expect_any(ndelay, ns);
}

static void test_speaker_tone(void **state)
{
	setup_speaker_tone_calls(500, 100);
	speaker_tone(500, 100);

	setup_speaker_tone_calls(4321, 0);
	speaker_tone(4321, 0);

	setup_speaker_tone_calls(-1, -1);
	speaker_tone(-1, -1);

	setup_speaker_tone_calls(10000, 1000);
	speaker_tone(10000, 1000);

	setup_speaker_tone_calls(433, 890);
	speaker_tone(433, 890);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_speaker_enable),
		cmocka_unit_test(test_speaker_disable),
		cmocka_unit_test(test_speaker_tone),
	};

	return lp_run_group_tests(tests, NULL, NULL);
}
