/* SPDX-License-Identifier: GPL-2.0-only */

/* Keep the host entry point separate from bootstate.h's firmware main(). */
int psp_boot_done_test_main(void);

int main(void)
{
	return psp_boot_done_test_main();
}
