/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/lapic.h>
#include <delay.h>
#include <halt.h>
#include <stdint.h>

/**
 * Sending INIT IPI to self is equivalent of asserting #INIT with a bit of
 * delay.
 * An undefined number of instruction cycles will complete. All global locks
 * must be released before INIT IPI and no printk is allowed after this.
 * De-asserting INIT IPI is a no-op on later Intel CPUs.
 *
 * If you set DEBUG_HALT_SELF to 1, printk's after INIT IPI are enabled
 * but running thread may halt without releasing the lock and effectively
 * deadlock other CPUs.
 */
#define DEBUG_HALT_SELF 0

#if DEBUG_HALT_SELF
#define dprintk(LEVEL, args...) do { printk(LEVEL, ##args); } while (0)
#else
#define dprintk(LEVEL, args...) do { } while (0)
#endif

static void wait_for_ipi_completion_without_printk(const int timeout_ms)
{
	int loops = timeout_ms * 10;
	uint32_t send_status;

	/* wait for the ipi send to finish */
	dprintk(BIOS_SPEW, "Waiting for send to finish...\n");
	do {
		dprintk(BIOS_SPEW, "+");
		udelay(100);
		send_status = lapic_busy();
	} while (send_status && (--loops > 0));

	if (send_status)
		dprintk(BIOS_ERR, "timed out\n");
}

/**
 * Normally this function is defined in lapic.h as an always inline function
 * that just keeps the CPU in a hlt() loop. This does not work on all CPUs.
 * I think all hyperthreading CPUs might need this version, but I could only
 * verify this on the Intel Core Duo
 */
void stop_this_cpu(void)
{
	const int timeout_100ms = 100;
	unsigned long id = lapicid();

	printk(BIOS_DEBUG, "CPU %ld going down...\n", id);

	/* send an LAPIC INIT to myself */
	lapic_send_ipi_self(LAPIC_INT_LEVELTRIG | LAPIC_INT_ASSERT | LAPIC_DM_INIT);
	wait_for_ipi_completion_without_printk(timeout_100ms);

	mdelay(10);

	dprintk(BIOS_SPEW, "Deasserting INIT.\n");

	/* Deassert the LAPIC INIT */
	lapic_send_ipi_self(LAPIC_INT_LEVELTRIG | LAPIC_DM_INIT);
	wait_for_ipi_completion_without_printk(timeout_100ms);

	halt();
}
