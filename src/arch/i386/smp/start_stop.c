#include <smp/start_stop.h>
#include <arch/smp/mpspec.h>
#include <cpu/p6/apic.h>
#include <delay.h>
#include <string.h>
#include <console/console.h>

static inline void hlt(void)
{
	asm("hlt");
	return;
}

unsigned long this_processors_id(void)
{
	return apic_read(APIC_ID) >> 24;
}

int processor_index(unsigned long apicid)
{
	int i;
	for(i = 0; i < CONFIG_MAX_CPUS; i++) {
		if (initial_apicid[i] == apicid) {
			return i;
		}
	}
	return -1;
}

void stop_cpu(unsigned long apicid)
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
		printk_err("timed out\n");
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
		printk_err("timed out\n");
	}

	while(1) {
		hlt();
	}
}

/* This is a lot more paranoid now, since Linux can NOT handle
 * being told there is a CPU when none exists. So any errors 
 * will return 0, meaning no CPU. 
 *
 * We actually handling that case by noting which cpus startup
 * and not telling anyone about the ones that dont.
 */ 
int start_cpu(unsigned long apicid)
{
	int timeout;
	unsigned long send_status, accept_status, start_eip;
	int j, num_starts, maxlvt;
	extern char _secondary_start[];
	
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
		printk_err("ESR is 0x%x\n", apic_read(APIC_ESR));
		if (apic_read(APIC_ESR)) {
			printk_err("Try to reset ESR\n");
			apic_write_around(APIC_ESR, 0);
			printk_err("ESR is 0x%x\n", apic_read(APIC_ESR));
		}
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

	start_eip = (unsigned long)_secondary_start;
	printk_spew("start_eip=0x%08lx\n", start_eip);
       
	num_starts = 2;

	/*
	 * Run STARTUP IPI loop.
	 */
	printk_spew("#startup loops: %d.\n", num_starts);

	maxlvt = 4;

	for (j = 1; j <= num_starts; j++) {
		printk_spew("Sending STARTUP #%d to %u.\n", j, apicid);
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
	unsigned long apicid = this_processors_id();
	int i;

	/* Assume the cpus are densly packed by apicid */
	for(i = 0; i < CONFIG_MAX_CPUS; i++) {
		unsigned long cpu_apicid = initial_apicid[i];
		if (cpu_apicid == -1) {
			printk_err("CPU %d not found\n",i);
			processor_map[i] = 0;
			continue;
		}
		if (cpu_apicid == apicid ) {
			continue;
		}
		if (!start_cpu(cpu_apicid)) {
			/* Put an error in processor_map[i]? */
			printk_err("CPU %d/%u would not start!\n",
				i, cpu_apicid);
			processor_map[i] = 0;
		}
	}
}
