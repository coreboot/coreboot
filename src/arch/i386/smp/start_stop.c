#include <smp/start_stop.h>
#include <arch/smp/mpspec.h>
#include <cpu/p6/apic.h>
#include <delay.h>

static inline void hlt(void)
{
	asm("hlt");
	return;
}

int this_processors_id(void)
{
	return apic_read(APIC_ID) >> 24;
}

void stop_cpu(int apicid)
{
	int timeout;
	unsigned long send_status;

	/* send an APIC INIT to myself */
	apic_write_around(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));
	apic_write_around(APIC_ICR, APIC_INT_LEVELTRIG | APIC_INT_ASSERT | APIC_DM_INIT);

	/* wait for the ipi send to finish */
	printk_spew("Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk_spew("+");
		udelay(100);
		send_status = apic_read(APIC_ICR) & APIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk_spew("timed out\n");
	}
	mdelay(10);

	printk_spew("Deasserting INIT.\n");
	/* Deassert the APIC INIT */
	apic_write_around(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));	
	apic_write_around(APIC_ICR, APIC_INT_LEVELTRIG | APIC_DM_INIT);

	printk_spew("Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk_spew("+");
		udelay(100);
		send_status = apic_read(APIC_ICR) & APIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk_debug("timed out\n");
	}

	while(1) {
		hlt();
	}

}

// This is a lot more paranoid now, since Linux can NOT handle
// being told there is a CPU when none exists. So any errors 
// will return 0, meaning no CPU. 
int start_cpu(int apicid)
{
	int timeout;
	unsigned long send_status, accept_status, start_eip;
	int j, num_starts, maxlvt;
	/*
	 * Starting actual IPI sequence...
	 */

	printk_spew("Asserting INIT.\n");

	/*
	 * Turn INIT on target chip
	 */
	apic_write_around(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));

	/*
	 * Send IPI
	 */
	apic_write_around(APIC_ICR, APIC_INT_LEVELTRIG | APIC_INT_ASSERT
				| APIC_DM_INIT);

	printk_spew("Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk_spew("+");
		udelay(100);
		send_status = apic_read(APIC_ICR) & APIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk_err("CPU %d: First apic write timed out. Disabling\n",
			 apicid);
		// too bad. 
		return 0;
	}

	mdelay(10);

	printk_spew("Deasserting INIT.\n");

	/* Target chip */
	apic_write_around(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));

	/* Send IPI */
	apic_write_around(APIC_ICR, APIC_INT_LEVELTRIG | APIC_DM_INIT);
	
	printk_spew("Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk_spew("+");
		udelay(100);
		send_status = apic_read(APIC_ICR) & APIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk_err("CPU %d: Second apic write timed out. Disabling\n",
			 apicid);
		// too bad. 
		return 0;
	}

	/* FIXME start_eip should be more flexible! */
	start_eip = 0xf0000;
	printk_spew("start eip=0x%08lx\n", start_eip);
       
	num_starts = 2;

	/*
	 * Run STARTUP IPI loop.
	 */
	printk_spew("#startup loops: %d.\n", num_starts);

	maxlvt = 4;

	for (j = 1; j <= num_starts; j++) {
		printk_spew("Sending STARTUP #%d.\n",j);
		apic_read_around(APIC_SPIV);
		apic_write(APIC_ESR, 0);
		apic_read(APIC_ESR);
		printk_spew("After apic_write.\n");

		/*
		 * STARTUP IPI
		 */

		/* Target chip */
		apic_write_around(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));

		/* Boot on the stack */
		/* Kick the second */
		apic_write_around(APIC_ICR, APIC_DM_STARTUP
					| (start_eip >> 12));

		/*
		 * Give the other CPU some time to accept the IPI.
		 */
		udelay(300);

		printk_spew("Startup point 1.\n");

		printk_spew("Waiting for send to finish...\n");
		timeout = 0;
		do {
			printk_spew("+");
			udelay(100);
			send_status = apic_read(APIC_ICR) & APIC_ICR_BUSY;
		} while (send_status && (timeout++ < 1000));

		/*
		 * Give the other CPU some time to accept the IPI.
		 */
		udelay(200);
		/*
		 * Due to the Pentium erratum 3AP.
		 */
		if (maxlvt > 3) {
			apic_read_around(APIC_SPIV);
			apic_write(APIC_ESR, 0);
		}
		accept_status = (apic_read(APIC_ESR) & 0xEF);
		if (send_status || accept_status)
			break;
	}
	printk_spew("After Startup.\n");
	if (send_status)
		printk_warning("APIC never delivered???\n");
	if (accept_status)
		printk_warning("APIC delivery error (%lx).\n", accept_status);
	if (send_status || accept_status)
		return 0;
	return 1;
}

void startup_other_cpus(unsigned long *processor_map)
{
	int apicid = this_processors_id();
	int i;
	/* Assume the cpus are densly packed by apicid */
	for(i = 0; i < MAX_CPUS; i++) {
		if (i == apicid ) {
			continue;
		}
		if (start_cpu(i))
			processor_map[i] = CPU_ENABLED;

	}
}
