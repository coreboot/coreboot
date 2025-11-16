/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/barrier.h>
#include <arch/encoding.h>
#include <arch/smp/smp.h>
#include <arch/smp/atomic.h>
#include <assert.h>
#include <console/console.h>
#include <mcall.h>

/*
 *                                                               BSP
 *                                                 --------------------------------- <---
 *                          AP                     |     sync_b=HART_SLEEPING      |     |
 *    ---> ---------------------------------       ---------------------------------     |
 *   |     |     sync_a=HART_SLEEPING      | ----> | Wait for sync_a=HART_SLEEPING |     |
 *   |     ---------------------------------       ---------------------------------     |
 *   |     |      Wait for Interrupt       | <---- |        set_msip()             |     |
 *   |     ---------------------------------       ---------------------------------     |
 *   |     |     sync_a=HART_AWAKE         | ----> | Wait for sync_a=HART_AWAKE    |     |
 *   |     ---------------------------------       ---------------------------------     |
 *   |     |  Wait for sync_b=HART_AWAKE   | <---- |     sync_b=HART_AWAKE         |     |
 *    ---- ---------------------------------       --------------------------------- ----
 */

// made up value to sync hart state
#define HART_SLEEPING 0x1
#define HART_AWAKE    0x2

void smp_pause(int working_hartid)
{
	int hartid = read_csr(mhartid);

	// pause all harts which are not the working hart
	if (hartid != working_hartid) {
		clear_csr(mstatus, MSTATUS_MIE); // disable all interrupts
		set_msip(hartid, 0); // clear pending interrupts
		write_csr(mie, MIP_MSIP); // enable only IPI (for smp_resume)
		barrier();
		atomic_set(&HLS()->entry.sync_a, HART_SLEEPING); // mark the hart as sleeping.

		// pause hart
		do {
			__asm__ volatile ("wfi"); // wait for interrupt
		} while ((read_csr(mip) & MIP_MSIP) == 0);

		atomic_set(&HLS()->entry.sync_a, HART_AWAKE); // mark the hart as awake

		// wait for working_hart to notice that this hart is awake
		while (atomic_read(&HLS()->entry.sync_b) != HART_AWAKE)
			;

		HLS()->entry.fn(HLS()->entry.arg);
	}
}

// must only be called by the WORKING_HARTID
void smp_resume(void (*fn)(void *), void *arg)
{
	if (fn == NULL) {
		printk(BIOS_ERR, "must pass a non-null function pointer\n");
		return; // we can still boot with one hart
	}

	int working_hartid = read_csr(mhartid);

	int hart_count = CONFIG_MAX_CPUS;
	if (CONFIG(RISCV_GET_HART_COUNT_AT_RUNTIME))
		hart_count = smp_get_hart_count();

	assert(hart_count <= CONFIG_MAX_CPUS);

	// check that all harts are present

	u32 count_awake_harts = 0;
	for (int i = 0; i < hart_count; i++) {
		// The working hart never sleeps. It is a hard working hart.
		if (i == working_hartid)
			continue;

		atomic_set(&OTHER_HLS(i)->entry.sync_b, HART_SLEEPING);

		if (atomic_read(&OTHER_HLS(i)->entry.sync_a) != HART_SLEEPING) {
			/*
			 * we assmue here that the time between smp_pause and smp_resume
			 * is enough for all harts to reach the smp_pause state.
			 * But for some reason that was not the case for this hart ...
			 */
			printk(BIOS_ERR, "hart %d did not enter smp_pause\n", i);
			OTHER_HLS(i)->enabled = 0; // disable hart
		} else {
			// hart is in wfi (wait for interrupt) state like it should be.

			OTHER_HLS(i)->entry.fn = fn;
			OTHER_HLS(i)->entry.arg = arg;
			barrier();
			set_msip(i, 1); // wake up hart
		}
	}

	printk(BIOS_DEBUG, "waiting for all harts to wake up...\n");
	// confirm that all harts are wake
	for (int i = 0; i < hart_count; i++) {
		// The working hart never sleeps. It is a hard working hart.
		if (i == working_hartid || !OTHER_HLS(i)->enabled)
			continue;

		// wait for hart to publish its waking state
		while (atomic_read(&OTHER_HLS(i)->entry.sync_a) != HART_AWAKE)
			;

		// signal to hart that we noticed it woke up
		atomic_set(&OTHER_HLS(i)->entry.sync_b, HART_AWAKE);
		count_awake_harts++;
	}
	printk(BIOS_DEBUG, "all harts up and running...\n");

	if ((hart_count - 1) != count_awake_harts) { // exclude working hart
		/*
		 * Apparently one or more harts did not reach smp_pause before smp_resume has
		 * been called by the working hart. That should not happen and may indicate we
		 * need a timeout of sorts to make sure we get all harts resumed.
		 */
		printk(BIOS_ERR, "some harts were too slow and could not resume\n");
	}
	fn(arg); // jump to fn with working hart
}
