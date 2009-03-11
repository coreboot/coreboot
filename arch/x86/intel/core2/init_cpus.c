/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) YingHai Lu
 * Copyright (C) 2005 Advanced Micro Devices, Inc.
 * Copyright (C) 2007 Stefan Reinauer
 * Copyright (C) 2008 coresystems GmbH
 * Copyright (C) 2009 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <types.h>
#include <lib.h>
#include <console.h>
#include <cpu.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <lapic.h>
#include <arch/atomic.h>
#include <arch/spinlock.h>
#include <lapic.h>

#ifdef CONFIG_SMM
void smm_init(void);
#endif

#define SERIAL_CPU_INIT 1

enum {
	AP_START = 1,
	AP_ACTIVEUP,
	AP_LOCKED,
	AP_INITDONE,
	AP_UNLOCKED,
	AP_ACTIVEDOWN,
	AP_STARTFAIL = 0x7f,
	AP_STOP = 0x80,
	AP_SELFIPI,
	AP_IPIDONE,
	AP_DOWNINIT,
	AP_STOP_OK = 0xfe, 
	AP_STOP_FAIL = 0xff,
};
/* this was an empty function in core 2 */
void
cpu_initialize(void)
{
}

/**
 * return count of how many nodes we have. 
 */
unsigned int get_nodes(void)
{
	struct cpuid_result result;
	int nodes, siblings;
	result = cpuid(1);
	/* See how many sibling cpus we have */
	printk(BIOS_DEBUG, "cpuid(1) ebx is %08x\n", result.ebx);
	siblings = (result.ebx >> 16) & 0xff;
	if (siblings < 1) {
		siblings = 1;
	}
	/* it seems this is the number of nodes */
	nodes = siblings;
	printk(BIOS_DEBUG, "%s: %d\n", __func__, nodes);
	return nodes;
}
/**
 * lapic remote read
 * lapics are more than just an interrupt steering system. They are a key part of inter-processor communication.
 * They can be used to start, control, and interrupt other CPUs from the BSP. It is not possible to bring up
 * an SMP system without somehow using the APIC.
 * CPUs and their attached IOAPICs all have an ID. For convenience, these IDs are unique.
 * The communications is packet-based, using (in coreboot) a polling-based strategy. As with most APIC ops,
 * the ID is the APIC ID. Even more fun, code can read registers in remote APICs, and this in turn can
 * provide us with remote CPU status.
 * This function does a remote read given an apic id. It returns the value or an error. It can time out.
 * @param apicid Remote APIC id
 * @param reg The register number to read
 * @param pvalue pointer to int for return value
 * @returns 0 on success, -1 on error
 */
int lapic_remote_read(int apicid, int reg, unsigned int *pvalue)
{
	int timeout;
	unsigned status;
	int result;
	/* Wait for the idle state. Could we enter this with the APIC busy? It's possible. */
	lapic_wait_icr_idle();
	/* The APIC Interrupt Control Registers define the operations and destinations.
	 * In this case, we set ICR2 to the dest, set the op to REMOTE READ, and set the
	 * reg (which is 16-bit aligned, it seems, hence the >> 4
	 */
	lapic_write(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));
	lapic_write(LAPIC_ICR, LAPIC_DM_REMRD | (reg >> 4));

	/* it's started. now we wait. */
	timeout = 0;

	do {
		/* note here the ICR is command and status. */
		/* Why don't we use the lapic_wait_icr_idle() above? */
		/* That said, it's a bad idea to mess with this code too much.
		 * APICs (and their code) are quite fragile.
		 */
		status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (status == LAPIC_ICR_BUSY && timeout++ < 1000);

	/* interesting but the timeout is not checked, hence no error on the send! */

	timeout = 0;
	do {
		status = lapic_read(LAPIC_ICR) & LAPIC_ICR_RR_MASK;
	} while (status == LAPIC_ICR_RR_INPROG && timeout++ < 1000);

	result = -1;
	if (status == LAPIC_ICR_RR_VALID) {
		*pvalue = lapic_read(LAPIC_RRR);
		result = 0;
	}
	return result;
}

#define LAPIC_MSG_REG 0x380

void print_apicid_nodeid_coreid(unsigned apicid, struct node_core_id id,
				const char *str)
{
	printk(BIOS_DEBUG, "%s --- {  APICID = %02x NODEID = %02x COREID = %02x} ---\n",
		str, apicid, id.nodeid, id.coreid);
}

/* this is "generic" lapic init. They are now in the core2 until we figure out how
 * a generic version for v3 should look. 
 */
/* These functions are critically dependent on which APIC we are using. That said,
 * this seems to be a universally safe definition. 
 */
# define FORCE_READ_AROUND_WRITE 1
# define lapic_read_around(x) lapic_read(x)
# define lapic_write_around(x,y) lapic_write_atomic((x),(y))

/* This is a lot more paranoid now, since Linux can NOT handle
 * being told there is a CPU when none exists. So any errors 
 * will return 0, meaning no CPU. 
 *
 * We actually handling that case by noting which cpus startup
 * and not telling anyone about the ones that dont.
 */ 
static unsigned long get_valid_start_eip(unsigned long orig_start_eip)
{
	return (unsigned long)orig_start_eip & 0xffff; // 16 bit to avoid 0xa0000 
}

/**
 * copy the secondary start POR code to low memory. 
 * Secondary start assembly code is way up in the FLASH ROM. 
 * we have to copy it to low memory since we turn the processor in 
 * in 16-bit mode. This is a bit of a hack at present since there is no real
 * check for clashes. But hopefully there is no issue. Hmm. 
 */
static u32 * copy_secondary_start_to_1m_below(void) 
{
        extern char _secondary_start[];
        extern char _secondary_start_end[];
        unsigned long code_size;
        unsigned long start_eip;

        start_eip = get_valid_start_eip((unsigned long)_secondary_start);
        code_size = (unsigned long)_secondary_start_end - (unsigned long)_secondary_start;

        /* copy the _secondary_start to the ram below 1M*/
        memcpy((unsigned char *)start_eip, (unsigned char *)_secondary_start, code_size);

        printk(BIOS_DEBUG, "start_eip=0x%08lx, offset=0x%08lx, code_size=0x%08lx\n", start_eip, ((unsigned long)_secondary_start - start_eip), code_size);
	return (u32 *)start_eip;
}

static int lapic_start_cpu(unsigned long apicid, u32 *secondary_base)
{
	int timeout;
	unsigned long send_status, accept_status, start_eip;
	int j, num_starts, maxlvt;
	extern char _secondary_start[];
		
	/*
	 * Starting actual IPI sequence...
	 */

	printk(BIOS_SPEW, "Before Startup.apicid %ld\n",  apicid);
	printk(BIOS_SPEW, "Before Startup.sb[0] %p @0 %08x\n",
			(void *) secondary_base[0], *(u32 *) 0);
	printk(BIOS_SPEW, "Asserting INIT.\n");

	/*
	 * Turn INIT on target chip
	 */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));

	/*
	 * Send IPI
	 */
	
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_INT_ASSERT
				| LAPIC_DM_INIT);

	printk(BIOS_SPEW, "Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk(BIOS_SPEW, "+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk(BIOS_ERR, "CPU %ld: First apic write timed out. Disabling\n",
			 apicid);
		// too bad. 
		printk(BIOS_ERR, "ESR is 0x%lx\n", lapic_read(LAPIC_ESR));
		if (lapic_read(LAPIC_ESR)) {
			printk(BIOS_ERR, "Try to reset ESR\n");
			lapic_write_around(LAPIC_ESR, 0);
			printk(BIOS_ERR, "ESR is 0x%lx\n", lapic_read(LAPIC_ESR));
		}
		return 0;
	}
	mdelay(10);

	printk(BIOS_SPEW, "Deasserting INIT.\n");

	/* Target chip */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));

	/* Send IPI */
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_DM_INIT);
	
	printk(BIOS_SPEW, "Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk(BIOS_SPEW, "+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk(BIOS_ERR, "CPU %ld: Second apic write timed out. Disabling\n",
			 apicid);
		// too bad. 
		return 0;
	}

	start_eip = (u32) secondary_base;
	num_starts = 2;

	/*
	 * Run STARTUP IPI loop.
	 */
	printk(BIOS_SPEW, "#startup loops: %d.\n", num_starts);

	maxlvt = 4;

	for (j = 1; j <= num_starts; j++) {
		printk(BIOS_SPEW, "Sending STARTUP #%d to %lu.\n", j, apicid);
		lapic_read_around(LAPIC_SPIV);
		lapic_write(LAPIC_ESR, 0);
		lapic_read(LAPIC_ESR);
		printk(BIOS_SPEW, "After apic_write.\n");

		/*
		 * STARTUP IPI
		 */

		/* Target chip */
		lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));

		/* Boot on the stack */
		/* Kick the second */
		printk(BIOS_SPEW, "Send start_eip %#lx(%#lx) sb[-1] %p\n", start_eip, start_eip>>12, (void *) secondary_base[-1]);
		lapic_write_around(LAPIC_ICR, LAPIC_DM_STARTUP
					| (start_eip >> 12));

		/*
		 * Give the other CPU some time to accept the IPI.
		 */
		udelay(300);

		printk(BIOS_SPEW, "Startup point 1.\n");

		printk(BIOS_SPEW, "Waiting for send to finish...\n");
		timeout = 0;
		do {
			printk(BIOS_SPEW, "+");
			udelay(100);
			send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
		} while (send_status && (timeout++ < 1000));

		/*
		 * Give the other CPU some time to accept the IPI.
		 */
		udelay(200);
		/*
		 * Due to the Pentium erratum 3AP.
		 */
		if (maxlvt > 3) {
			lapic_read_around(LAPIC_SPIV);
			lapic_write(LAPIC_ESR, 0);
		}
		accept_status = (lapic_read(LAPIC_ESR) & 0xEF);
		if (send_status || accept_status)
			break;
	}
	printk(BIOS_SPEW, "udelay(1000000)\n");
	udelay(1000000);
	printk(BIOS_SPEW, "After Startup.sb[-1] %p\n", (void *) secondary_base[-1]);
	printk(BIOS_SPEW, "After Startup.sb[0] %p @0 %08x\n", 
			 (void *) secondary_base[0],  *(u32 *) 0);
	if (send_status)
		printk(BIOS_WARNING, "APIC never delivered???\n");
	if (accept_status)
		printk(BIOS_WARNING, "APIC delivery error (%lx).\n", accept_status);
	if (send_status || accept_status)
		return 0;
	return 1;
}

/* stack is 16k minus the status pointer */
/* we compile the secondary start code such that this looks like a stack frame
 */
struct stack {
	u32 data[16384/sizeof(u32) - 7];
	struct spinlock *start_cpu_lock;
	struct atomic *active_cpus;
	u32 apicid;
	u32 index;
	u32 post;
	u32  callerpc;
};

struct stackmem {
	unsigned char pagetable[20480];
	struct stack stacks[0];
};

int start_cpu(u32 apicid, struct atomic *active_cpus, struct spinlock *start_cpu_lock, int *last_cpu_index, u32 *secondary_base)
{

	u32 stack_end;
	unsigned long index;
	unsigned long count;
	int result;
	struct stackmem *stackmem = (void *) 0x100000;

	/* Get an index for the new processor */
	
	index = *last_cpu_index + 1;
	*last_cpu_index = index;
	
	/* Find end of the new processors stack */
	/* on this CPU we are going to always use memory above 1M. Or so we think. */
	/* since the SP points to data, it will appear as though pc, and other 
	 * structure components are on the stack. Thus the BSP and AP 
	 * can see them. 
	 */
	stack_end = (u32)&stackmem->stacks[index].data;
	printk(BIOS_SPEW, "Stack for AP %ld is %x\n", index, stack_end);
	
	stackmem->stacks[index].index= index;
	stackmem->stacks[index].apicid   = apicid;
	stackmem->stacks[index].post   = 0;
	stackmem->stacks[index].active_cpus  = active_cpus;
	stackmem->stacks[index].start_cpu_lock = start_cpu_lock;
	printk(BIOS_SPEW, "stack[index, apicid, post, active_cpus, start_cpu_lock] = [%lx, %x, %d, %p, %p]\n", index, apicid, 0, active_cpus, start_cpu_lock);
	/* Advertise the new stack to start_cpu */
	printk(BIOS_SPEW, "Set stack @ %p to %p\n", &secondary_base[-1], (void *)stack_end);
	secondary_base[-1] = stack_end;

	/* Start the cpu */
	result = lapic_start_cpu(apicid, secondary_base);
	printk(BIOS_SPEW, "we think we started it. The stack value is 0x%p (should be 0)\n", (void *)secondary_base[-1]);

	if (result) {
		printk(BIOS_SPEW, "Spinning on post which is now 0x%x\n", 
				stackmem->stacks[index].post);
		result = 0;
		/* Wait 1s or until the new the new cpu calls in */
		for(count = 0; count < 1000000 ; count++) {
			printk(BIOS_SPEW, 
				"BSP post 0x%x\n", 
				stackmem->stacks[index].post);
			if (stackmem->stacks[index].post >= AP_STOP_OK) {
				result = 1;
				break;
			}
			udelay(10);
		}
	}
	spin_unlock(start_cpu_lock);
	return result;
}

/**
 * Normally this function is defined in lapic.h as an always inline function
 * that just keeps the CPU in a hlt() loop. This does not work on all CPUs.
 * I think all hyperthreading CPUs might need this version, but I could only
 * verify this on the Intel Core Duo
 */
void stop_cpu(u32 *post)
{
	int timeout;
	unsigned long send_status;
	unsigned long lapicid;

	lapicid = lapic_read(LAPIC_ID) >> 24;
	*post = AP_STOP;

	printk(BIOS_DEBUG, "CPU %ld going down...\n", lapicid);

	/* send an LAPIC INIT to myself */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(lapicid));
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_INT_ASSERT | LAPIC_DM_INIT);
	*post = AP_SELFIPI;

	/* wait for the ipi send to finish */
#if 0
	// When these two printk_spew calls are not removed, the
	// machine will hang when log level is SPEW. Why?
	printk(BIOS_SPEW, "Waiting for send to finish...\n");
#endif
	timeout = 0;
	do {
#if 0
		printk(BIOS_SPEW, "+");
#endif
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		*post = AP_STOP_FAIL;
		printk(BIOS_ERR, "timed out\n");
	}
	mdelay(10);

	printk(BIOS_SPEW, "Deasserting INIT.\n");
	/* Deassert the LAPIC INIT */
	*post = AP_IPIDONE;
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(lapicid));	
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_DM_INIT);

	printk(BIOS_SPEW, "Waiting for send to finish...\n");
	timeout = 0;
	*post = AP_DOWNINIT;
	do {
		printk(BIOS_SPEW, "+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk(BIOS_ERR, "timed out\n");
	}

	while(1) {
		*post = AP_STOP_OK;
		hlt();
	}
}
/* C entry point of secondary cpus */
/* we call this with the stack set up such that the args are visible to BOTH
 * this function and cpu0. It's an easy trick and a simple way to get shared 
 * memory
 */
void __attribute__((regparm(0))) secondary_cpu_init(
	u32 post,
	u32 index,
	u32 apicid,
	struct atomic *active_cpus,
	struct spinlock *start_cpu_lock)
{
//printk(BIOS_SPEW, "secondary start\n");
//printk(BIOS_SPEW, "[post, index, apicid,active_cpus, start_cpu_lock] = [%x, %x, %d, %p, %p]\n", post, index, apicid, active_cpus, start_cpu_lock);
	post = AP_START;
//printk(BIOS_SPEW, "secondary post %d\n", post);
	atomic_inc(active_cpus);
	post = AP_ACTIVEUP;
//printk(BIOS_SPEW, "secondary post %d\n", post);
	if (SERIAL_CPU_INIT && (CONFIG_MAX_PHYSICAL_CPUS > 2))
		spin_lock(start_cpu_lock);
	post = AP_LOCKED;
	cpu_initialize();
	post = AP_INITDONE;
	if (SERIAL_CPU_INIT && (CONFIG_MAX_PHYSICAL_CPUS > 2))
		spin_unlock(start_cpu_lock);
	post = AP_UNLOCKED;

	atomic_dec(active_cpus);
	post = AP_ACTIVEDOWN;

	stop_cpu(&post);
}

/* this is an index, not an apicid */
static void start_other_cpus(u32 numcpus,struct atomic *active_cpus, struct spinlock *start_cpu_lock, int *last_cpu_index, u32 *secondary_base)
{
	int i;
	/* Loop through the cpus once getting them started */

	for(i = 1; i < numcpus; i++) {

		if (!start_cpu(i, active_cpus, start_cpu_lock, last_cpu_index, secondary_base)) {
			/* Record the error in cpu? */
			printk(BIOS_ERR, "CPU 0x%02x would not start!\n", i);
		}

		if (SERIAL_CPU_INIT && (CONFIG_MAX_PHYSICAL_CPUS > 2))
			udelay(10);
	}

}

static void wait_other_cpus_stop(struct atomic *active_cpus)
{
	/* todo: walk the stacks and print out POST codes from APs */
	int old_active_count, active_count;
	/* Now loop until the other cpus have finished initializing */
	old_active_count = 1;
	active_count = atomic_read(active_cpus);
	while(active_count > 1) {
		if (active_count != old_active_count) {
			printk(BIOS_INFO, "Waiting for %d CPUS to stop\n", active_count - 1);
			old_active_count = active_count;
		}
		udelay(10);
		active_count = atomic_read(active_cpus);
	}
	printk(BIOS_DEBUG, "All AP CPUs stopped\n");
}

/* this should be a library call. */
static void fill_processor_name(char *processor_name, int len)
{
	struct cpuid_result regs;
	char temp_processor_name[49];
	char *processor_name_start;
	unsigned int *name_as_ints = (unsigned int *)temp_processor_name;
	int i;

	if (len > sizeof(temp_processor_name))
		len = sizeof(temp_processor_name);

	for (i=0; i<3; i++) {
		regs = cpuid(0x80000002 + i);
		name_as_ints[i*4 + 0] = regs.eax;
		name_as_ints[i*4 + 1] = regs.ebx;
		name_as_ints[i*4 + 2] = regs.ecx;
		name_as_ints[i*4 + 3] = regs.edx;
	}

	temp_processor_name[48] = 0;

	/* Skip leading spaces */
	processor_name_start = temp_processor_name;
	while (*processor_name_start == ' ') 
		processor_name_start++;

	memset(processor_name, 0, 49);
	memcpy(processor_name, processor_name_start, len);
}

#define IA32_FEATURE_CONTROL 0x003a

#define CPUID_VMX (1 << 5)
#define CPUID_SMX (1 << 6)
static void enable_vmx(void)
{
	struct cpuid_result regs;
	struct msr msr;

	msr = rdmsr(IA32_FEATURE_CONTROL);

	if (msr.lo & (1 << 0)) {
		/* VMX locked. If we set it again we get an illegal
		 * instruction
		 */
		return;
	}

	regs = cpuid(1);
	if (regs.ecx & CPUID_VMX) {
		msr.lo |= (1 << 2);
		if (regs.ecx & CPUID_SMX)
			msr.lo |= (1 << 1);
	}

	wrmsr(IA32_FEATURE_CONTROL, msr);

	msr.lo |= (1 << 0); /* Set lock bit */

	wrmsr(IA32_FEATURE_CONTROL, msr);
}

#define PMG_CST_CONFIG_CONTROL	0xe2
static void configure_c_states(void)
{
	struct msr msr;

	msr = rdmsr(PMG_CST_CONFIG_CONTROL);
	msr.lo &= ~(1 << 9); // Issue a  single stop grant cycle upon stpclk

	// TODO Do we want Deep C4 and  Dynamic L2 shrinking?
	wrmsr(PMG_CST_CONFIG_CONTROL, msr);
}

#define IA32_MISC_ENABLE	0x1a0
static void configure_misc(void)
{
	struct msr msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 3); 	/* TM1 enable */
	msr.lo |= (1 << 13);	/* TM2 enable */
	msr.lo |= (1 << 17);	/* Bidirectional PROCHOT# */

	msr.lo |= (1 << 10);	/* FERR# multiplexing */

	// TODO: Only if  IA32_PLATFORM_ID[17] = 0 and IA32_PLATFORM_ID[50] = 1
	msr.lo |= (1 << 16);	/* Enhanced SpeedStep Enable */

	// TODO Do we want Deep C4 and  Dynamic L2 shrinking?
	wrmsr(IA32_MISC_ENABLE, msr);

	msr.lo |= (1 << 20);	/* Lock Enhanced SpeedStep Enable */
	wrmsr(IA32_MISC_ENABLE, msr);
}


/**
 * Init all the CPUs. 
 * this was unnecessarily tricky in v2 (surprise!)
 * let's do some math. If you look at the lapic start stuff, we do a few IPIs and spin for 200 microseconds
 * on them. Then we wait for the CPU to start. This process takes maybe 1 millisecond worst case. 
 * So on core 2, need we worry about serial CPU 
 * vs. parallel CPU init? We need not. In fact in most cases we need not until we get to machines with 
 * huge memory per CPU; we'll do them later. We know how. Further, on v2 there is this 
 * unnecessarily complex atomic CPU mask to support the asynchrony. We're not an OS, we're a B IOS. 
 * Simple rule for parallel 
 * programming: whenever possible, don't have multiple writers for variables. 
 * It's easier and cleaner to give every CPU 
 * its own status word; no atomic ops needed, no worry later about all the problems that come with 
 * them. 
 * So, basic algorithm: 
 * 1. Figure out how many nodes there are
 * 2. If < 2, done. 
 * 3. Copy the .S-based code to low memory (< 0xa0000) since it starts in 16-bit mode
 * 4. Craft up a stack. The stack will have info at the base. 
 *     The AP will increment this status word as it passes through its startup. 
 * 5. start the AP with appropriate IPI messages. 
 * 6. Wait for AP to transition variable from 0 -> 1 (means started) -> 0xff (means done). 
 *     Why 0xff? Because we can use this variable as a POST code! 8 bits are ever so much nicer than one bit. 
 * 
 * @param soft_reset Are we here from a soft reset?
 * @param sysinfo The sys_info pointer
 * @returns the BSP APIC ID
 */
unsigned int cpu_phase1(unsigned cpu_init_detectedx,
			struct sys_info *sysinfo)
{
	/* Number of cpus that are currently running in coreboot */
	struct atomic active_cpus;
	u32 *secondary_base;
	char processor_name[49];

	/* every AP gets a stack. The stacks are linear and start at 0x100000 */
	struct spinlock start_cpu_lock;
	int last_cpu_index;

	atomic_set(&active_cpus, 1);
	start_cpu_lock = SPIN_LOCK_UNLOCKED;
	last_cpu_index = 0;
	/* Ensure the local apic is enabled */
	/* this was conditional in V2 but will always be true in V3. */
	enable_lapic();
	// why here? In case some day we can start core1 in amd_sibling_init
	secondary_base = copy_secondary_start_to_1m_below(); 

	/* Turn on caching if we haven't already */
	enable_cache();

	/* Update the microcode *
	intel_update_microcode(microcode_updates);
	 * needs to be rewritten to use LAR, not code include hacks
	 */

	/* Print processor name */
	fill_processor_name(processor_name, sizeof(processor_name));
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

#ifdef CONFIG_USBDEBUG_DIRECT
	// Is this caution really needed?
	if(!ehci_debug_addr) 
		ehci_debug_addr = get_ehci_debug();
	set_ehci_debug(0);
#endif

	/* Setup MTRRs */
	x86_setup_mtrrs(36);
	x86_mtrr_check();

#ifdef CONFIG_USBDEBUG_DIRECT
	set_ehci_debug(ehci_debug_addr);
#endif

	/* Enable the local cpu apics */
	/* do we need this in v3? not sure
	setup_lapic();
	 */

	/* Enable virtualization */
	enable_vmx();

	/* Configure C States */
	configure_c_states();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* TODO: PIC thermal sensor control */


#ifdef CONFIG_SMM
	smm_init();
#endif

	/* Start up my cpu siblings */
	if (! SERIAL_CPU_INIT)
	/* start all aps at first, so we can init ECC all together */
    	    start_other_cpus(get_nodes(), &active_cpus, &start_cpu_lock, &last_cpu_index, secondary_base);

        /* Initialize the bootstrap processor */
        cpu_initialize();

	if (SERIAL_CPU_INIT)
		start_other_cpus(get_nodes(), &active_cpus, &start_cpu_lock, &last_cpu_index, secondary_base);

	/* Now wait the rest of the cpus stop*/
	wait_other_cpus_stop(&active_cpus);

	return 0;
}

#ifdef WAIT_BEFORE_CPUS_INIT
	#define cpus_ready_for_init() do {} while(0)
#endif

