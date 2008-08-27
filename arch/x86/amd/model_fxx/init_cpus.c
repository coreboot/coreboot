/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Advanced Micro Devices, Inc.
 * Copyright (C) 2007 Stefan Reinauer
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
#include <mainboard.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <cpu.h>
#include <amd/k8/k8.h>
#include <mc146818rtc.h>
#include <spd.h>

// for rev F, need to set FID to max
#define K8_SET_FIDVID 1

#ifndef K8_SET_FIDVID_CORE0_ONLY
	/* MSR FIDVID_CTL and FIDVID_STATUS are shared by cores, so may don't need to do twice*/
       	#define K8_SET_FIDVID_CORE0_ONLY 1
#endif

static void print_initcpu8 (const char *strval, unsigned int val)
{
        printk(BIOS_DEBUG, "%s%02x\n", strval, val);
}

static void print_initcpu8_nocr (const char *strval, unsigned val)
{
        printk(BIOS_DEBUG, "%s%02x", strval, val);
}


static void print_initcpu16 (const char *strval, unsigned val)
{
        printk(BIOS_DEBUG, "%s%04x\n", strval, val);
}

static inline void print_initcpu(const char *strval, unsigned val)
{
        printk(BIOS_DEBUG, "%s%08x\n", strval, val);

}

typedef void (*process_ap_t)(unsigned apicid, void *gp);
/* See page 330 of Publication # 26094       Revision: 3.30 Issue Date: February 2006
 * for APIC id discussion
 */
/**
 * for_each_ap
 * iterate over all APs and have them run a function. 
 * The behaviour is modified by the core_range parameter
 * @param bsp_apicid The BSP APIC ID number
 * @param core_range modifier for the range of cores to run on: 
 * core_range = 0 : all cores
 * core range = 1 : core 0 only
 * core range = 2 : cores other than core0
 * @param process pointer to the function to run
 * @param gp general purpose argument to be passed as a parameter to the function
 */
static void for_each_ap(unsigned bsp_apicid, unsigned core_range, process_ap_t process_ap, void *gp)
{
	/* Assume the OS will not change our APIC ID. Why does this matter? Because some of the setup
	 * we do for other cores may depend on it not being changed. 
	 */
	unsigned int ap_apicid;

	unsigned int nodes;
        unsigned int siblings = 0;
        unsigned int disable_siblings = 1;
        unsigned int e0_later_single_core;
        unsigned int nb_cfg_54;
        int i,j;

	/* The get_nodes function is defined in northbridge/amd/k8/coherent_ht.c */
	nodes = get_nodes();

	/* if the get_option fails siblings remain disabled. */
       disable_siblings = !CONFIG_LOGICAL_CPUS;
	get_option(&disable_siblings, "dual_core");
 
	/* There is an interesting problem in different steppings. See page 373. The interpretation of the 
	 * APIC ID bits is different. To determine which order is used, check bit 54 of the 
	/* here we assume that all nodes are the same stepping. If not, "otherwise we can use use nb_cfg_54 from bsp for all nodes" */
        nb_cfg_54 = read_nb_cfg_54();


        for(i=0; i<nodes;i++) {
		e0_later_single_core = 0;
		/* Page 166. This field indicates the number of cores, with 0 meaning 1, 1 meaning 2, and all else reserved */
                j = ((pci_conf1_read_config32(PCI_BDF(0, 0x18+i, 3), NORTHBRIDGE_CAP) >> NBCAP_CmpCap_SHIFT) & NBCAP_CmpCap_MASK);
                if(nb_cfg_54) {
 	               if(j == 0 ) // if it is single core, we need to increase siblings for apic calculation 
                               J = 1; 
                }
                siblings = j;

                unsigned jstart, jend;

                if(core_range == 2) {
                        jstart = 1;
                }
		else {
			jstart = 0;
		}

                if (disable_siblings || (core_range==1)) {
                        jend = 0;
                } else {
                        jend = siblings;
		}	
		
	
                for(j=jstart; j<=jend; j++) {

                        ap_apicid = i * (nb_cfg_54?(siblings+1):1) + j * (nb_cfg_54?1:8);

                #if (ENABLE_APIC_EXT_ID == 1)
			#if LIFT_BSP_APIC_ID == 0
			if( (i!=0) || (j!=0)) /* except bsp */
			#endif
                        	ap_apicid += APIC_ID_OFFSET;
                #endif

			if(ap_apicid == bsp_apicid) continue;

			process_ap(ap_apicid, gp); 

                }
        }
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
static int lapic_remote_read(int apicid, int reg, unsigned int *pvalue)
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


#if K8_SET_FIDVID == 1
static void init_fidvid_ap(unsigned bsp_apicid, unsigned apicid);
#endif

static inline __attribute__((always_inline)) void print_apicid_nodeid_coreid(unsigned apicid, struct node_core_id id, const char *str)
{
	#if CONFIG_USE_PRINTK_IN_CAR
                printk_debug("%s --- {  APICID = %02x NODEID = %02x COREID = %02x} ---\r\n", str, apicid, id.nodeid, id.coreid);
	#else
		printk(BIOS_DEBUG, str);
        	printk(BIOS_DEBUG, " ---- {APICID = "); print_debug_hex8(apicid);
		printk(BIOS_DEBUG, " NODEID = "), print_debug_hex8(id.nodeid); printk(BIOS_DEBUG, " COREID = "), print_debug_hex8(id.coreid);
	        printk(BIOS_DEBUG, "} --- \r\n");
        #endif
}


/**
 * Using the APIC remote read code, wait for the CPU to enter a given state
 * This function can time out. 
 * @param apicid The apicid of the remote CPU
 * @param state The state we are waiting for
 * @return 0 on success, readback value on error 
 */
static unsigned int wait_cpu_state(unsigned apicid, unsigned state)
{
        unsigned readback = 0;
	unsigned timeout = 1;
	int loop = 2000000;
        while(--loop>0) {
                if(lapic_remote_read(apicid, LAPIC_MSG_REG, &readback)!=0) continue;
                if((readback & 0xff) == state) {
			timeout = 0;
			break; //target cpu is in stage started
		}
        }
	if(timeout) {
		if(readback) {
			timeout = readback;
		}
	}

	return timeout;
}

/** 
 * Wait for an AP to start. 
 * @param ap_apicid the apic id of the CPu
 * @param gp arbitrary parameter
 */
static void wait_ap_started(unsigned ap_apicid, void *gp )
{
	unsigned timeout;
        timeout = wait_cpu_state(ap_apicid, 0x33); // started
	if(timeout) {
	        print_initcpu8_nocr("*", ap_apicid);
	        print_initcpu("*", timeout);
	}
	else {
        	print_initcpu8_nocr(" ", ap_apicid);
	}
}

/**
 * wait for all apics to start. Make sure we don't wait on ourself. 
 * @param bsp_apicid The BSP APIC ID
 */
static void wait_all_aps_started(unsigned bsp_apicid)
{
        for_each_ap(bsp_apicid, 0 , wait_ap_started, (void *)0);
}

/**
 * Wait on all other cores to start. This includes  cores on bsp, we think. 
 * @param bsp_apicid The BSP APIC ID
 */
static void wait_all_other_cores_started(unsigned bsp_apicid) // all aps other than core0
{
        printk(BIOS_DEBUG, "started ap apicid: ");
        for_each_ap(bsp_apicid, 2 , wait_ap_started, (void *)0);
        printk(BIOS_DEBUG, "\r\n");
}

/**
 * Stop all APs
 * @param bsp_apicid The BSP apic id, to make sure we don't send ourselves the stop
 */
static void allow_all_aps_stop(unsigned bsp_apicid)
{
        lapic_write(LAPIC_MSG_REG, (bsp_apicid<<24) | 0x44); // allow aps to stop
}

static void STOP_CAR_AND_CPU(void)
{
	disable_cache_as_ram(); // inline
	stop_this_cpu(); // inline, it will stop all cores except node0/core0 the bsp ....
}

#ifndef MEM_TRAIN_SEQ
#define MEM_TRAIN_SEQ 0
#endif


#if MEM_TRAIN_SEQ == 1
static inline void train_ram_on_node(unsigned nodeid, unsigned coreid, struct sys_info *sysinfo, unsigned retcall); 
#endif

unsigned int init_cpus(unsigned cpu_init_detectedx ,struct sys_info *sysinfo)
{
		unsigned bsp_apicid = 0;
		unsigned apicid;
                struct node_core_id id;

		/* 
                 * MTRR must be set by this point.
		 */

		/* that is from initial apicid, we need nodeid and coreid later */
		id = get_node_core_id_x(); 


                /* NB_CFG MSR is shared between cores, so we need make sure core0 is done at first --- use wait_all_core0_started  */
		if(id.coreid == 0) {
                	set_apicid_cpuid_lo(); /* only set it on core0 */
			#if ENABLE_APIC_EXT_ID == 1
                        enable_apic_ext_id(id.nodeid);
			#endif
                }

		enable_lapic();
//              init_timer(); // We need TMICT to pass msg for FID/VID change

        #if (ENABLE_APIC_EXT_ID == 1)
		unsigned initial_apicid = get_initial_apicid();	
                #if LIFT_BSP_APIC_ID == 0
                if( initial_apicid != 0 ) // other than bsp
                #endif
                {
                                /* use initial apic id to lift it */
                                u32 dword = lapic_read(LAPIC_ID);
                                dword &= ~(0xff<<24);
                                dword |= (((initial_apicid + APIC_ID_OFFSET) & 0xff)<<24);

                                lapic_write(LAPIC_ID, dword);
                }

                #if LIFT_BSP_APIC_ID == 1
                bsp_apicid += APIC_ID_OFFSET;
                #endif

        #endif

		/* get the apicid, it may be lifted already */
		apicid = lapicid();

#if 0 
		// show our apicid, nodeid, and coreid
		if( id.coreid==0 ) {
			if (id.nodeid!=0) //all core0 except bsp
				print_apicid_nodeid_coreid(apicid, id, " core0: ");
		}
	#if 0 
                else { //all core1
			print_apicid_nodeid_coreid(apicid, id, " core1: ");
                }
	#endif

#endif

                if (cpu_init_detectedx) {
                        print_apicid_nodeid_coreid(apicid, id, "\r\n\r\n\r\nINIT detected from ");
                        printk(BIOS_DEBUG, "\r\nIssuing SOFT_RESET...\r\n");
                        soft_reset();
                }

		if(id.coreid==0) {
	                distinguish_cpu_resets(id.nodeid);
	//              start_other_core(id.nodeid); // start second core in first cpu, only allowed for nb_cfg_54 is not set
		}

		//here don't need to wait 
		lapic_write(LAPIC_MSG_REG, (apicid<<24) | 0x33); // mark the cpu is started

		if(apicid != bsp_apicid) {
			unsigned timeout=1;
			unsigned loop = 100;
	#if K8_SET_FIDVID == 1
		#if (CONFIG_LOGICAL_CPUS == 1) && (K8_SET_FIDVID_CORE0_ONLY == 1)
			if(id.coreid == 0 ) // only need set fid for core0
		#endif 
       		                init_fidvid_ap(bsp_apicid, apicid);
	#endif

                       // We need to stop the CACHE as RAM for this CPU, really?
			while(timeout && (loop-->0)) {	
	                        timeout = wait_cpu_state(bsp_apicid, 0x44);
			}
			if(timeout) {
			        print_initcpu8("while waiting for BSP signal to STOP, timeout in ap ", apicid);
			}
                        lapic_write(LAPIC_MSG_REG, (apicid<<24) | 0x44); // bsp can not check it before stop_this_cpu
                        set_init_ram_access();
	#if MEM_TRAIN_SEQ == 1
			train_ram_on_node(id.nodeid, id.coreid, sysinfo, STOP_CAR_AND_CPU);
	#endif

			STOP_CAR_AND_CPU();
                } 

		return bsp_apicid;
}


static unsigned is_core0_started(unsigned nodeid)
{
        u32 htic;
        struct device * device;
        device = PCI_DEV(0, 0x18 + nodeid, 0);
        htic = pci_conf1_read_config32(device, HT_INIT_CONTROL);
        htic &= HTIC_INIT_Detect;
        return htic;
}

static void wait_all_core0_started(void)
{
	//When core0 is started, it will distingush_cpu_resets. So wait for that
	unsigned i;
	unsigned nodes = get_nodes();

        printk(BIOS_DEBUG, "core0 started: ");
        for(i=1;i<nodes;i++) { // skip bsp, because it is running on bsp
                while(!is_core0_started(i)) {}
                print_initcpu8_nocr(" ", i);
        }
        printk(BIOS_DEBUG, "\r\n");

}

