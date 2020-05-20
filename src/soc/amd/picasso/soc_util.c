/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <soc/cpu.h>
#include <soc/soc_util.h>
#include <types.h>

#define SOCKET_TYPE_SHIFT	28
#define SOCKET_TYPSE_MASK	(0xf << SOCKET_TYPE_SHIFT)

static enum socket_type get_socket_type(void)
{
	uint32_t ebx = cpuid_ebx(0x80000001);
	ebx = (ebx & SOCKET_TYPSE_MASK) >> SOCKET_TYPE_SHIFT;
	return (enum socket_type)ebx;
}

void print_socket_type(void)
{
	enum socket_type socket = get_socket_type();

	printk(BIOS_INFO, "Socket type: ");

	switch (socket) {
	case SOCKET_FP5:
		printk(BIOS_INFO, "FP5\n");
		break;
	case SOCKET_AM4:
		printk(BIOS_INFO, "AM4\n");
		break;
	case SOCKET_FT5:
		printk(BIOS_INFO, "FT5\n");
		break;
	default:
		printk(BIOS_INFO, "unknown\n");
	}
}

bool soc_is_pollock(void)
{
	return soc_is_zen_plus() && get_socket_type() == SOCKET_FT5;
}

/*
 * TODO: This detection works for the Dali SKUs used in Chrome-devices, but fails for other
 * Dali SKUs, since other Dali SKUs have a Zen+ CPUID and not a Raven2 one.
 */
bool soc_is_dali(void)
{
	return soc_is_raven2() && get_socket_type() == SOCKET_FP5;
}

bool soc_is_picasso(void)
{
	return soc_is_zen_plus() && get_socket_type() == SOCKET_FP5;
}

bool soc_is_raven2(void)
{
	/* mask lower model number nibble and stepping */
	return cpuid_eax(1) >> 8 == RAVEN2_CPUID >> 8;
}

bool soc_is_zen_plus(void)
{
	/* mask lower model number nibble and stepping */
	return cpuid_eax(1) >> 8 == PICASSO_CPUID >> 8;
}
